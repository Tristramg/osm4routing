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

#include "osmreader.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

OsmReader::OsmReader(const std::string & filename)
{
    file.open(filename.c_str(), ifstream::in);
    if( !file.good() )
    {
        std::cout << endl;
        std::cerr << "Unable to open file " << filename << std::endl;
        exit(1);
    }
}

int OsmReader::read(char * buff, int buffsize)
{
    file.read(buff, buffsize);
    return file.gcount();
}

bool OsmReader::eof() const
{
    return file.eof();
}
