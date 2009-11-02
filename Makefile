CXX=g++
CXXFLAGS=-W -Wall -ansi -pedantic -O3 -Wno-deprecated 
LDFLAGS=-lexpat -lbz2 -lpq -lboost_program_options -lboost_filesystem
EXEC=osm4routing

all: $(EXEC)

osm4routing: parameters.o main.o osmreader.o bz2reader.o stdinreader.o csvwriter.o pqwriter.o
	$(CXX) -o $@ $^ $(LDFLAGS)

main.o: main.cc main.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

parameters.o: parameters.cc main.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

osmreader.o: osmreader.cc osmreader.h reader.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

bz2reader.o: bz2reader.cc bz2reader.h reader.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

stdinreader.o: stdinreader.cc stdinreader.h reader.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

csvwriter.o: csvwriter.cc csvwriter.h writer.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

pqwriter.o: pqwriter.cc pqwriter.h writer.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)

