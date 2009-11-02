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
#include <expat.h>
#include <string>
#include <iostream>
#include <map>
#include <ext/hash_map>
#include <tr1/functional_hash.h>
#include <cstring>
#include <bitset>
#include <iomanip>
#include <fstream>
#include <stdint.h>
#include <sstream>

#ifndef _MAIN_H
#define _MAIN_H

typedef uint64_t node_t;
const int unknown = -1;
const int foot_forbiden = 0;
const int foot_allowed = 1;

const int car_forbiden = 0;
const int car_residential = 1;
const int car_tertiary = 2;
const int car_secondary = 3;
const int car_primary = 4;
const int car_trunk = 5;
const int car_motorway = 6;

const int bike_forbiden = 0;
const int bike_opposite_lane = 1;
const int bike_allowed = 2;
const int bike_lane = 3;
const int bike_busway = 4;
const int bike_track = 5;

class Edge_property
{
    public:
    int car_direct;
    int car_reverse;
    int bike_direct;
    int bike_reverse;
    int foot;

    Edge_property();

    // Can at least one mean use that edge
    bool accessible();
    bool direct_accessible();
    bool reverse_accessible();

    // Update the properties given new information
    bool update(const std::string & tag, const std::string & val);

    // Infer unknown data
    void normalize();

    void reset();
};

struct Node
{
    uint64_t id;
    float lon;
    float lat;
    ushort uses;

    Node() : uses(0) {};

    Node(double _lon, double _lat, uint64_t _id) :
        id(_id), lon(_lon), lat(_lat), uses(0)
    {};
};
typedef __gnu_cxx::hash_map<uint64_t, Node, std::tr1::hash<uint64_t> >NodeMapType;

#endif /* _MAIN_H */

