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

#include "bz2reader.h"
#include <iostream>
#include <cerrno>
#include <string.h>
#include <stdlib.h>


using namespace std;
BZReader::BZReader(const std::string & filename)
{
    FILE* fp = fopen64(filename.c_str(), "rb");
    if(!fp)
    {
        std::cout << std::endl;
        std::cerr << "Error opening file " << filename << " errorno " << errno << " " << strerror(errno) << std::endl;
        exit(1);
    }

    b = BZ2_bzReadOpen ( &bzerror, fp, 0, 0, NULL, 0 );
    if ( bzerror != BZ_OK )
    {
        std::cerr << "Error opening file " << filename << " as bzip2 file, errno " << bzerror << " " << 
            BZ2_bzerror(b, &bzerror) << std::endl;
        BZ2_bzReadClose ( &bzerror, b );
    }
}

int BZReader::read(char * buff, int buffsize)
{
    return BZ2_bzRead ( &bzerror, b, buff, buffsize );
}

bool BZReader::eof() const
{
    return bzerror == BZ_STREAM_END;
}
