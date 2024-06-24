CXX = g++
CXXFLAGS = -std=c++14 -Wall

all: correctness persistence

correctness: kvstore.o correctness.o skiplist.o sstable.o vlog.o bloomfilter.o
	$(CXX) $(CXXFLAGS) -o correctness kvstore.o correctness.o skiplist.o sstable.o vlog.o bloomfilter.o

persistence: kvstore.o persistence.o skiplist.o sstable.o bloomfilter.o
	$(CXX) $(CXXFLAGS) -o persistence kvstore.o persistence.o skiplist.o sstable.o vlog.o bloomfilter.o

kvstore.o: kvstore.cc kvstore.h skiplist.o vlog.o bloomfilter.o sstable.o
	$(CXX) $(CXXFLAGS) -c kvstore.cc

correctness.o: correctness.cc kvstore.h
	$(CXX) $(CXXFLAGS) -c correctness.cc

persistence.o: persistence.cc kvstore.h
	$(CXX) $(CXXFLAGS) -c persistence.cc

skiplist.o: skiplist.cc skiplist.h
	$(CXX) $(CXXFLAGS) -c skiplist.cc

bloomfilter.o: bloomfilter.cc bloomfilter.h
	$(CXX) $(CXXFLAGS) -c bloomfilter.cc

sstable.o: sstable.cc sstable.h bloomfilter.o skiplist.o
	$(CXX) $(CXXFLAGS) -c sstable.cc

vlog.o: vlog.cc vlog.h kvstore.h
	$(CXX) $(CXXFLAGS) -c vlog.cc

clean:
	-rm -f correctness persistence *.o
