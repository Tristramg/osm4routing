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

#include "csvwriter.h"

using namespace std;

CSVWriter::CSVWriter(const std::string & nodes_file, const std::string & edges_filename):
    nodes_filename(nodes_file)
{
    edges_file.open(edges_filename.c_str());
    edges_file << setprecision(9);
    edges_file << "\"edge_id\",\"source\",\"target\",\"length\",\"car\",\"car reverse\",\"bike\",\"bike reverse\",\"foot\",\"WKT\"" << endl;
}

int CSVWriter::save_nodes(const NodeMapType & nodes)
{
    ofstream nodes_file;
    nodes_file.open (nodes_filename.c_str());
    // By default outstream only give 4 digits after the dot (~10m precision)
    nodes_file << setprecision(9);
    nodes_file << "\"node_id\",\"longitude\",\"latitude\"" << endl;
    int nodes_inserted = 0;

    for(NodeMapType::const_iterator i = nodes.begin(); i != nodes.end(); i++)
    {
        if( (*i).second.uses > 1 )
        {
            nodes_file << (*i).first << "," <<
                (*i).second.lon << "," << 
                (*i).second.lat << endl;
            nodes_inserted++;
        }
    }
    nodes_file.close();
    return nodes_inserted;
}

void CSVWriter::save_edge(int edge_id,
            node_t source, node_t target, float length,
            char car_direct, char car_rev,
            char bike_direct, char bike_rev,
            char foot,
            std::string geom)
{
    edges_file << edge_id << "," << source << "," << target << ","
                    << length << ","
                    << car_direct << "," << car_rev << "," 
                    << bike_direct << "," << bike_rev << ","
                    << foot << ","
                    << "LINESTRING(\"" << geom << "\")" << endl;
}


CSVWriter::~CSVWriter()
{
    edges_file.close();
}
