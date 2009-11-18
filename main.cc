/*
    This file is part of osm4routing.

    osm4routing is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Mumoro is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with osm4routing.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main.h"
#include "cmath"
#include <hash_set>
#include <deque>
#include "stdinreader.h"
#include "bz2reader.h"
#include "osmreader.h"
#include "csvwriter.h"
#include "pqwriter.h"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

using namespace std;
namespace po = boost::program_options;

Node * source;
Node * prev;
std::stringstream geom;
NodeMapType nodes;
node_t ways_count;
int edge_length;
node_t ways_progress;
Edge_property ep;
double length;

ofstream temp_edges;

std::deque<node_t> way_nodes;
node_t current_way;

double rad(double deg)
{
    return deg * M_PIl / 180;
}

double distance(double lon1, double lat1, double lon2, double lat2)
{
    const double r = 6371000;

    return acos( sin( rad(lat1) ) * sin( rad(lat2) ) +
            cos( rad(lat1) ) * cos( rad(lat2) ) * cos( rad(lon2-lon1 ) )
            ) * r;
}
   void
start(void *, const char *el, const char **attr)
{
    if (strcmp(el, "node") == 0)
    {
        node_t id = 0;
        double lat = 0, lon = 0;
        while (*attr != NULL)
        {
            const char* name = *attr++;
            const char* value = *attr++;

            if (strcmp(name, "id") == 0)
            {
                id = atoll(value);
            }
            else if (strcmp(name, "lat") == 0)
            {
                lat = atof(value);
            }
            else if (strcmp(name, "lon") == 0)
            {
                lon = atof(value);
            }
        }
        nodes[id] = Node(lon, lat, id);
    }

    else if (strcmp(el, "nd") == 0)
    {
        const char* name = *attr++;
        const char* value = *attr++;
        if (strcmp(name, "ref") == 0)
        {
            node_t node_id = atoll(value);
            way_nodes.push_back(node_id);
        }
    }

    else if(strcmp(el, "way") == 0)
    {
        way_nodes.clear();
        ep.reset();
        const char* name = *attr++;
        const char* value = *attr++;
        if( !strcmp(name, "id") == 0 )
        {
            cout << "fuck" << std::endl;
        }
        else
        {
            current_way = atoll(value);
        }
        ways_count++;
    }

    else if(strcmp(el, "tag") == 0)
    {
        string key;
        while (*attr != NULL)
        {
            const char* name = *attr++;
            const char* value = *attr++;

            if ( strcmp(name, "k") == 0 )
                key = value;
            else if ( strcmp(name, "v") == 0 )
            {
                ep.update(key, value);
            }
        }
    }

}

void end(void * , const char * el)
{
    if(strcmp(el, "way") == 0)
    {
        if(ep.accessible())
        {
            ep.normalize();
            deque<node_t>::const_iterator it;
            temp_edges << ep.foot << " "
                << ep.car_direct << " " << ep.car_reverse << " "
                << ep.bike_direct << " " << ep.bike_reverse << " "
                << way_nodes.size();
            for(it = way_nodes.begin(); it < way_nodes.end(); it++)
            {
                nodes[*it].uses++;
                temp_edges << " " << *it;
            }
            temp_edges << endl;

            nodes[way_nodes.front()].uses++;
            nodes[way_nodes.back()].uses++;
        }
    } 
}

    int
main(int argc, char** argv)
{
    po::options_description desc("Allowed options");
    string informat, outformat, inputfile;
    po::positional_options_description p;
    p.add("input-file", -1);

    desc.add_options()
        ("input-file", po::value<string>(&inputfile), "input file")
        ("help,h", "produce help message")
        ("version,v", "show version")
        ("in-format", po::value<string>(&informat)->default_value("auto"), "input data format:\n    auto: \tguess from file extension. If no input file is given, read from stdin\n    bz2: \tbzip2 compressed file\n    stdin: \tstandart input\n    osm: \tuncompressed XML file")
        ("out-format", po::value<string>(&outformat)->default_value("csv"), "output data format:\n    csv: \tnodes and edges as csv files\n    pg: \tpostgres/postgis database. The database MUST be a postgis geographical database")
        ;

    string pg_conn, pg_nodes, pg_edges;
    po::options_description pg("Postgres output options");
    pg.add_options()
        ("pg_conn", po::value<string>(&pg_conn), "postgres connection string. For example \"dbname=mydb user=John password=pass\"")
        ("pg_nodes", po::value<string>(&pg_nodes)->default_value("nodes"), "nodes table. CAUTION: the table will be dropped")
        ("pg_edges", po::value<string>(&pg_edges)->default_value("edges"), "edges table. CAUTION: the table will be dropped");

    string csv_nodes, csv_edges;
    po::options_description csv("CSV output options");
    csv.add_options()
        ("csv_nodes", po::value<string>(&csv_nodes)->default_value("nodes.csv"), "csv output file for the nodes")
        ("csv_edges", po::value<string>(&csv_edges)->default_value("edges.csv"), "csv output file for the edges");

    po::options_description cmdline_options;
    cmdline_options.add(desc).add(csv).add(pg);

    po::variables_map vm;
    try
    {
    po::store(po::command_line_parser(argc, argv).
            options(cmdline_options).positional(p).run(), vm);
    }
    catch(po::unknown_option e)
    {
        cerr << e.what() << endl;
        return (EXIT_FAILURE);
    }
    po::notify(vm); 

    if (argc == 1 || vm.count("help"))
    {
        cout << "Usage: " << argv[0] << " [input-file] [options]" << endl<< endl;
        cout << desc << endl << pg << endl << csv << endl;

        return (EXIT_SUCCESS);
    }
    if (vm.count("version"))
    {
        cout << argv[0] << " -- version 0.0" << endl;
        return (EXIT_SUCCESS);
    }
    if (vm.count("in-format")
            && (informat == "bz2" || informat == "csv")
            && !vm.count("input-file") )
    {
        cerr << "No input file given !" << endl;
        return (EXIT_FAILURE);
    }
    if (vm.count("in-format")
            && informat == "pg"
            && !vm.count("pg_conn") )
    {
        cerr << "Postgres output choosen, but no connection string given" << endl;
        return (EXIT_FAILURE);
    }
    if(informat ==  "auto")
    {
        if(!vm.count("input-file"))
        {
            informat = "stdin";
        }
        else
        {
            string extension = boost::filesystem::extension(boost::filesystem::path(inputfile));
            if(extension == ".xml" || extension == ".osm")
                informat = "osm";
            else if(extension == ".bz2")
                informat = "bz2";
            else
            {
                cerr << "Unable to guess file format!" << endl;
                return (EXIT_FAILURE);
            }
        }
    }

    //==================== STEP 1 =======================//
    cout << "Step 1: reading the xml file, extracting the Nodes list" << flush;
    temp_edges.open("temp_ways");
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetElementHandler(parser, start, end);
    ios_base::sync_with_stdio(false);

    Reader * reader;
    if(informat == "stdin")
        reader = new StdinReader();
    else if(informat == "bz2")
        reader = new BZReader(inputfile);
    else if(informat == "osm")
        reader = new OsmReader(inputfile);
    else
    {
        cerr << "Unknown input format: " << informat << endl;
        return (EXIT_FAILURE);
    }
    while ( !reader->eof() ) 
    {
        char buf[BUFSIZ];
        int read = reader->read( buf, sizeof(buf) );
        if( !XML_Parse(parser, buf, read, reader->eof()) )
        {
            cerr << XML_ErrorString(XML_GetErrorCode(parser)) <<
                " at line " <<
                XML_GetCurrentLineNumber(parser) << endl;
        }
    }
    XML_ParserFree(parser);
    temp_edges.close();
    delete reader;
    cout << "... DONE!" << endl;
    cout << "    Nodes found: " << nodes.size() << endl;
    cout << "    Ways found: " << ways_count << endl << endl;


    //===================== STEP 2 ==========================//
    Writer * writer;
    if(outformat == "pg")
        writer = new PqWriter(pg_conn, pg_nodes, pg_edges);
    else if (outformat == "csv")
        writer = new CSVWriter(csv_nodes, csv_edges);
    cout << "Step 2: building edges and saving them" << endl;
    ifstream tmp;
    tmp.open("temp_ways");
    node_t id, source=0;
    stringstream geom;
    double length = 0, pred_lon = 0, pred_lat = 0;
    char car_direct, car_rev, foot, bike_direct, bike_rev;
    int nb;
    int edges_inserted = 0;
    Node n;
    string line;


    while(getline(tmp, line))
    {
        stringstream way(line);
        way >> foot >> car_direct >> car_rev >> bike_direct >> bike_rev >> nb;
        for(int i=0; i<nb; i++)
        {
            way >> id;
            n = nodes[id];

            if(i == 0)
            {
                geom.str("");
                source = id;
            }
            else
            {
                length += distance(n.lon, n.lat, pred_lon, pred_lat);
                if(geom.str() != "")
                    geom << ",";
            }

            pred_lon = n.lon;
            pred_lat = n.lat;

            geom << n.lon << " " << n.lat;
            if( i>0 && n.uses > 1 && id != source)
            {
                writer->save_edge(edges_inserted, source, id, length, car_direct, car_rev, bike_direct, bike_rev, foot, geom.str());
                edges_inserted++;
                length = 0;
                geom.str("");
                geom << n.lon << " " << n.lat;
                source = id;
            }
        }
    }

    tmp.close();

    //==================== STEP 3 =======================//
    cout << "Step 3: storing the intersection nodes" << endl;
    int nodes_inserted = writer->save_nodes(nodes);
    delete writer;

    cout << "DONE!" << endl << endl;

    cout << "Nodes in database: " << nodes_inserted << endl;
    cout << "Edges in database: " << edges_inserted << endl;
    cout << "Happy routing! :)" << endl << endl;

    return (EXIT_SUCCESS);
}

