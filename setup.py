# Update this if needed
# It has to point to the directory containing the boost headers
boost_path = "."


from distribute_setup import use_setuptools
use_setuptools()

from setuptools import setup, find_packages, Extension
setup(name='Osm4routing',
      version='1.0.3',
      author= 'Tristram Graebener',
      author_email = 'tristramg@gmail.com',
      description = 'A simple tool to parse OpenStreetMap data to use them for routing',
      license = 'GPLv3',
      url = 'http://github.com/Tristramg/osm4routing/',
      install_requires = ['sqlalchemy', 'setuptools-git', 'geoalchemy'],
      py_modules = ['osm4routing', 'osm4routing_xml'],

      ext_modules = [
          Extension("_osm4routing_xml",
              sources=["parse.cc", "parameters.cc", "parse.i"],
              swig_opts=['-c++'],
              include_dirs=[boost_path],
              libraries=['expat'])
          ],
       entry_points = {
           'console_scripts': ['osm4routing = osm4routing:main'],
        }

      )

