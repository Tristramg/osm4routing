from distribute_setup import use_setuptools
use_setuptools()

from setuptools import setup, find_packages, Extension
setup(name='Osm4routing',
      version='1.0',
      author= 'Tristram',
      author_email = 'tristramg@gmail.com',
      url = 'http://github.com/Tristramg/osm4routing/',
      install_requires = ['progressbar', 'sqlalchemy', 'setuptools-git'],
      py_modules = ['osm4routing', 'osm4routing_xml'],

      ext_modules = [
          Extension("_osm4routing_xml",
              sources=["parse.cc", "parameters.cc", "parse.i"],
              swig_opts=['-c++'],
              include_dirs=['.'],
              libraries=['expat'])
          ],
       entry_points = {
           'console_scripts': ['osm4routing = osm4routing:main'],
        }

      )

