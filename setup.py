from distribute_setup import use_setuptools
use_setuptools()

from setuptools import setup, find_packages, Extension
setup(name='Osm4routing',
      version='1.0',
      author= 'Tristram',
      author_email = 'tristramg@gmail.com',
      url = 'http://github.com/Tristramg/osm4routing/',
      install_requires = ['progressbar', 'sqlalchemy', 'setuptools-git'],
      py_modules = ['parse', 'osm4routing'],

      ext_modules = [
          Extension("_osm4routing",
              sources=["parse.cc", "parameters.cc", "parse.i"],
              swig_opts=['-c++'],
              libraries=['expat'])
          ],
       entry_points = {
           'console_scripts': ['osm4routing = parse:main'],
           'setuptools.installation': ['eggsecutable = parse:main',
        ]

        }

      )

