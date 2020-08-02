# Port to Py3
### Instructions
```zsh
git clone https://github.com/Tristramg/osm4routing.git
cd osm4routing
brew install swig
brew install boost
pip install -e .
```

You will need to make a change to one of the installed python packages (`geoalchemy`) to make it work with py3:

In geoalchemy base.py change the following lines from:
```py
from geoalchemy.utils import from_wkt
from geoalchemy.functions import functions, _get_function, BaseFunction
```

to

```py
from utils import from_wkt
from functions import functions, _get_function, BaseFunction
```

---
# Original osm4routing README
This tool provides an OpenStreetMap data parser to turn them into a nodes-edges
adapted for routing applications.

# INPUT FORMAT
The input is an OpenStreetMap XML file. The file can be read:
* from a plain .osm file
* from a bzip2 file
* from a gzip file

# OUTPUT FORMAT
The output can be:
* a csv file
* database (postgres, mysql, sqlite, postgis)

In both output you'll get two files/tables:
* nodes that contain
    * id (64 bit integer)
    * longitude (decimal real)
    * latitude (decimal real)
    * geometry (only in postgis)

* edges that contain
    * id (64 bit integer)
    * source node id (64 bit integer)
    * target node id (64 bit integer)
    * length (real in meters),
    * car accessibility (integer)
    * car reverse accessibility (integer)
    * bike accessibility (integer)
    * bike reverse accessibility (integer)
    * foot accessibility (integer)
    * geometry (string representing a linestring in the WKT format)

The accessibility is an integer describing the edge for every mean of transport.
As for cars an bikes the driving direction might change those properties, the
are direct (source->target direction) an reverse (target->source direction)
information.

The integers mean:
* cars 
    * 0 forbiden
    * 1 residential street
    * 2 tertiary road
    * 3 secondary road
    * 4 primary road
    * 5 trunk
    * 6 motorway
* bike
    * 0 forbiden
    * 1 cycling lane in the opposite direction of the car flow
    * 2 allowed without specific equipment
    * 3 cycling lane
    * 4 bus lane allowed for cycles
    * 5 cycling track
* foot (no distinction in made on the direction)
    * 0 forbiden
    * 1 allowed


# INSTALL
You need:
* Boost (only for unordered_map that will be included in C++1X)
    Read bellow if you have troubles with it
* expat (XML parser) (Ubuntu users install libexpat1-dev)
* python (Ubunut users install python-dev)
* swig
* Python connectors for the database you wan
    * sqlite and text output is by default
    * python-psycopy2 for postgresql

```
# Just run the following command
sudo python setup.py install
# Run it
osm4routing --help
```

## Alternative:
```
# If you don't have the rights to install it system-wide, or don't want to, use virtualenv:
# Create a virtual environment and activate it
python virtualenv.py env
source env/bin/activate
python setup.py install
osm4routing --help
```

* Installing boost:
    You only need the headers. If you have a linux distribution, just install it.
    Under windows or macOSX, grab the sources and unzip them. Place the boost directory
    containing the headers in the same directory as setup.py
    An alternative is to edit setup.py to tell where the directory is located.

# USAGE
Get the .osm XML file of the region that interests you.
For limited regions, use the export tools from the web interface.
For bigger regions you might find what you want at http://download.geofabrik.de/osm/
Osmosis can help you to have a smaller region from a big dump http://wiki.openstreetmap.org/wiki/Osmosis

To know the options, run:
osm4routing --help

# PERFORMANCE
OSM data can get very big and can be very consuming, don't try to parse the whole world ;)
On my laptop from 2006 (core2duo 1.66Ghz, 2Gb Ram, slow hard drive),
it takes 20 minutes to parse 8Gb uncompressed (0.5Gb as bzip2) and represents France in June 2010

# Postgis output
Only if you want to use the spatial abilities of postgis, please read those extra informations
in order to a have spatial database
The usual way to get is to execute the following commands (the location of
lwpostgis.sql and spatial_ref_sys.sql depend on your installation).

```
createdb yourdatabase
createlang plpgsql yourdatabase
psql -d yourdatabase -f lwpostgis.sql
psql -d yourdatabase -f spatial_ref_sys.sql
```
