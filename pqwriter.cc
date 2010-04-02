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

#include "pqwriter.h"
#include <iostream>
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

PqWriter::PqWriter(const std::string & conn_str, const std::string & nodes_table, const std::string & edges_table, bool drop_table) :
    nodes_table(nodes_table),
    edges_table(edges_table),
    drop_table(drop_table)
{

    /* Make a connection to the database */
    conn = PQconnectdb(conn_str.c_str());

    /* Check to see that the backend connection was successfully made */
    if (PQstatus(conn) != CONNECTION_OK)
    {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        exit(1);
    }

    PGresult   *res;
    if(drop_table)
    {
        //First drop edges because of foreign keys
        res = PQexec(conn, ("DROP TABLE IF EXISTS " + edges_table).c_str());
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            std::cerr << "Unable to drop table " << edges_table <<  PQerrorMessage(conn) << std::endl;
        }
        PQclear(res);

        res = PQexec(conn, ("DROP TABLE IF EXISTS " + nodes_table).c_str());
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            std::cerr << "Unable to drop table " << nodes_table <<  PQerrorMessage(conn) << std::endl;
        }
        PQclear(res);
        res = PQexec(conn, ("CREATE TABLE " + nodes_table + " (ID bigint, lon double precision, lat double precision)").c_str());
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            std::cerr << "Unable to create table " << nodes_table <<  PQerrorMessage(conn) << std::endl;
        }
        PQclear(res);

        res = PQexec(conn, ("CREATE TABLE " + edges_table + " (ID integer, source bigint, target bigint, length double precision, car smallint, car_rev smallint, bike smallint, bike_rev smallint, foot smallint)").c_str());      
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            std::cerr << "Unable to create table " << edges_table <<  PQerrorMessage(conn) << std::endl;
        }
        PQclear(res);

        res = PQexec(conn, ("SELECT AddGeometryColumn('" + nodes_table + "','the_geom',4326,'POINT',2)").c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK)
        {
            std::cerr << "Unable to add a geometry column on table " << nodes_table <<  PQerrorMessage(conn) << std::endl;
        }
        PQclear(res);

res = PQexec(conn, ("SELECT AddGeometryColumn('" + edges_table + "','the_geom',4326,'LINESTRING',2)").c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK)
        {
            std::cerr << "Unable to add a geometry column on table " << edges_table <<  PQerrorMessage(conn) << std::endl;
        }
        PQclear(res);
    }

    res = PQprepare(conn,
            "add_edge",
            ("INSERT INTO " + edges_table + " VALUES($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)").c_str() ,
            0,
            NULL);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        std::cerr << "Unable to prepare SQL query for adding edges in table " << edges_table <<  PQerrorMessage(conn) << std::endl;
    }
    PQclear(res);


    res = PQprepare(conn,
            "add_node",
            ("INSERT INTO " + nodes_table + " VALUES($1, $2, $3, $4)").c_str() ,
            0,
            NULL);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        std::cerr << "Unable to prepare SQL query for adding edges in nodes " << nodes_table <<  PQerrorMessage(conn) << std::endl;
    }
    PQclear(res);

    res = PQexec(conn, "BEGIN");
    if( PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        std::cerr << "Unable to start transaction (nodes) " << PQerrorMessage(conn) << std::endl;
    }
    PQclear(res);




}

int PqWriter::save_nodes(const NodeMapType & nodes)
{
    PGresult * res;
    int count = 0;
    for(NodeMapType::const_iterator i = nodes.begin(); i != nodes.end(); i++)
    {
        if( (*i).second.uses > 1 )
        {
            std::stringstream geom;
            geom << "SRID=4326;POINT(" << i->second.lon << " " << i->second.lat << ")";
            const char * params[4] = {
                lexical_cast<std::string>(i->first).c_str(),
                lexical_cast<std::string>(i->second.lon).c_str(),
                lexical_cast<std::string>(i->second.lat).c_str(),
                geom.str().c_str()
            };
            res = PQexecPrepared(conn, "add_node", 4, params, NULL, NULL, 0);
            if (PQresultStatus(res) != PGRES_COMMAND_OK)
            {
                std::cerr << "Unable to add node in table " << nodes_table <<  PQerrorMessage(conn) << std::endl;
                exit(1);
            }
            else
                count++;

            PQclear(res);
        }
    }
    res = PQexec(conn, ("alter table " + nodes_table + " add primary key (id)").c_str());
    if( PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        std::cerr << "Unable to add primary key on table " << nodes_table << PQerrorMessage(conn) << std::endl;
    }
    return count;
}
void PqWriter::save_edge(int edge_id,
        node_t source, node_t target, float length,
        char car, char car_d,
        char bike, char bike_d,
        char foot,
        std::string geom)
{

    PGresult * res;
    std::string tmp_geom = "SRID=4326;LINESTRING(" + geom + ")";
    const char * params[10] = {
        lexical_cast<std::string>(edge_id).c_str(),
        lexical_cast<std::string>(source).c_str(),
        lexical_cast<std::string>(target).c_str(),
        lexical_cast<std::string>(length).c_str(),
        lexical_cast<std::string>(car).c_str(),
        lexical_cast<std::string>(car_d).c_str(),
        lexical_cast<std::string>(bike).c_str(),
        lexical_cast<std::string>(bike_d).c_str(),
        lexical_cast<std::string>(foot).c_str(),
        tmp_geom.c_str()
    };

    res = PQexecPrepared(conn, "add_edge", 10, params, NULL, NULL, 0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        std::cerr << "Unable to add edge in table " << edges_table <<  PQerrorMessage(conn) << std::endl;
        std::cerr << "GEOM: " << params[9] << std::endl;
        exit(1);
    }
}

PqWriter::~PqWriter()
{

    PGresult * res;
    res = PQexec(conn, "COMMIT");
    if( PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        std::cerr << "Unable to start commit (nodes) " << PQerrorMessage(conn) << std::endl;
    }

    std::string query = "alter table " + edges_table + " add primary key (id);";
    query += "ALTER TABLE " + edges_table + " ADD FOREIGN KEY (source) REFERENCES " + nodes_table;
    query += ";ALTER TABLE " + edges_table + " ADD FOREIGN KEY (target) REFERENCES " + nodes_table;
    query += ";CREATE INDEX idx_" + edges_table + " ON " + edges_table + " USING gist(the_geom)";
    query += ";CREATE INDEX idx_" + nodes_table + " ON " + nodes_table + " USING gist(the_geom)";
    res = PQexec(conn, query.c_str());
    if( PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        std::cerr << "Unable to add primary key on table " << edges_table << PQerrorMessage(conn) << std::endl;
    }


    PQfinish(conn);
}

