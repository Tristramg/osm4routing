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

#include "parse.h"
#include <iostream>
Edge_property::Edge_property() :
        car_direct(unknown),
        car_reverse(unknown),
        bike_direct(unknown),
        bike_reverse(unknown),
        foot(unknown)
    {
    }

bool Edge_property::accessible()
{
    return direct_accessible() || reverse_accessible();
}

bool Edge_property::direct_accessible()
{
    return car_direct > 0 || bike_direct > 0 || foot > 0;
}

bool Edge_property::reverse_accessible()
{
    return car_reverse > 0 || bike_reverse > 0 || foot > 0;
}



void Edge_property::reset()
{
    car_direct = unknown;
    car_reverse = unknown;
    bike_direct = unknown;
    bike_reverse = unknown;
    foot = unknown;
}

void Edge_property::normalize()
{
    if(car_reverse == unknown && car_direct != unknown)
        car_reverse = car_direct;
    if(bike_reverse == unknown && bike_direct != unknown)
        bike_reverse = bike_direct;
    if(car_direct == unknown) car_direct = car_forbiden;
    if(bike_direct == unknown) bike_direct = bike_forbiden;
    if(car_reverse == unknown) car_reverse = car_forbiden;
    if(bike_reverse == unknown) bike_reverse = bike_forbiden;
    if(foot == unknown) foot = foot_forbiden;
}

bool Edge_property::update(const std::string & key, const std::string & val)
{
    if(key == "highway")
    {
        if(val == "cycleway" || val == "path" || val == "footway" ||
                val == "steps" || val == "pedestrian")
        {
            bike_direct = bike_track;
            foot = foot_allowed;
        }
        else if(val == "primary" || val == "primary_link")
        {
            car_direct = car_primary;
            foot = foot_allowed;
            bike_direct = bike_allowed;
        }
        else if(val == "secondary")
        {
            car_direct = car_secondary;
            foot = foot_allowed;
            bike_direct = bike_allowed;
        }
        else if(val == "tertiary")
        {
            car_direct = car_tertiary;
            foot = foot_allowed;
            bike_direct = bike_allowed;
        }
        else if(val == "unclassified" || val == "residential" || val == "living_street" ||
                val == "road" || val == "service" || val == "track")
        {
            car_direct = car_residential;
            foot = foot_allowed;
            bike_direct = bike_allowed;
        }
        else if(val == "motorway" || val == "motorway_link")
        {
            car_direct = car_motorway;
            foot = foot_forbiden;
            bike_direct = bike_forbiden;
        }
        else if(val == "trunk" || val == "trunk_link")
        {
            car_direct = car_motorway;
            foot = foot_forbiden;
            bike_direct = bike_forbiden;
        }
    }

    else if(key == "pedestrian" || key == "foot")
    {
        if(val == "yes" || val == "designated" || val == "permissive")
            foot = foot_allowed;
        else if(val == "no")
            foot = foot_forbiden;
        else
            std::cerr << "I don't know what to do with: " << key << "=" << val << std::endl;
    }

    // http://wiki.openstreetmap.org/wiki/Cycleway
    // http://wiki.openstreetmap.org/wiki/Map_Features#Cycleway
    else if(key == "cycleway")
    {
        if(val == "lane" || val == "yes" || val == "true" || val == "lane_in_the_middle")
            bike_direct = bike_lane;
        else if(val == "track")
            bike_direct = bike_track;
        else if(val == "opposite_lane")
            bike_reverse = bike_lane;
        else if(val == "opposite_track")
            bike_reverse = bike_track;
        else if(val == "opposite")
            bike_reverse = bike_allowed;
        else if(val == "share_busway")
            bike_direct = bike_busway;
        else if(val == "lane_left")
            bike_reverse = bike_lane;
        else
            bike_direct = bike_lane;
    }

    else if(key == "bicycle")
    {
        if(val == "yes" || val == "permissive" || val == "destination" || val == "designated" || val == "private" || val == "true")
            bike_direct = bike_allowed;
        else if(val == "no" || val == "true")
            bike_direct = bike_forbiden;
        else
            std::cerr << "I don't know what to do with: " << key << "=" << val << std::endl;
    }

    else if(key == "busway")
    {
        if(val == "yes" || val == "track" || val == "lane")
            bike_direct = bike_busway;
        else if(val == "opposite_lane" || val == "opposite_track")
            bike_reverse = bike_busway;
        else
            bike_direct = bike_busway;
    }

    else if(key == "oneway")
    {
        if(val == "yes" || val == "true" || val == "1")
        {
            car_reverse = car_forbiden;
            if(bike_reverse == unknown)
                bike_reverse = bike_forbiden;
        }
    }

    else if(key == "junction")
    {
        if(val == "roundabout")
        {
            car_reverse = car_forbiden;
            if(bike_reverse == unknown)
                bike_reverse = bike_forbiden;
        }
    }
    return this->accessible();
}
