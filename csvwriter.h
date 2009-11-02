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

#include "writer.h"
#include <iostream>

#ifndef _CSVWRITER_H
#define _CSVWRITER_H

class CSVWriter : public Writer
{
    std::ofstream edges_file;
    std::string nodes_filename;
    public:
    CSVWriter(const std::string & nodes_fn, const std::string & edges_fn);

    int save_nodes(const NodeMapType & nodes);

    void save_edge(int edge_id,
            node_t source, node_t target, float length,
            char car, char car_d,
            char bike, char bike_d,
            char foot,
            std::string geom);

    ~CSVWriter();
};

#endif
