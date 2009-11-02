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

#include "reader.h"
#include <fstream>

#ifndef _OSMREADER_H
#define _OSMREADER_H
class OsmReader : public Reader
{
    std::ifstream file;
    public:
    OsmReader(const std::string & filename);
    int read(char * buff, int buffsize);
    bool eof() const;
};
#endif
