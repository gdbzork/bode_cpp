bin_PROGRAMS = baseComposition       \
               countReadsOnIntervals \
               countReadsOnChroms    \
               filterByRegion        \
               makeBedgraph          \
               makeCDT               \
               makeDensity           \
               qualityHistogram      \
               readDensityMap 
baseComposition_SOURCES = baseComposition.cpp
baseComposition_LDFLAGS = -lbodeio -lbodeseq -lbodeutil -lhts -lz -lpthread

makeCDT_SOURCES = makeCDT.cpp
makeCDT_LDFLAGS = -lbodetree -lbodeio -lbodeseq -lbodeutil -lhts -lz -lpthread

countReadsOnIntervals_SOURCES = countReadsOnIntervals.cpp
countReadsOnIntervals_LDFLAGS = -lbodealgo -lbodealign -lbodetree -lbodeio -lbodeseq -lbodeutil -lhts -lz -lpthread

filterByRegion_SOURCES = filterByRegion.cpp
filterByRegion_LDFLAGS = -lbodealgo -lbodealign -lbodetree -lbodeio -lbodeseq -lbodeutil -lhts -lz -lpthread

makeBedgraph_SOURCES = makeBedgraph.cpp
makeBedgraph_LDFLAGS = -lbodeio -lbodeseq -lbodeutil -lhts -lz -lpthread

qualityHistogram_SOURCES = qualityHistogram.cpp
qualityHistogram_LDFLAGS = -lbodeio -lbodeseq -lbodeutil -lhts -lz -lpthread

countReadsOnChroms_SOURCES = countReadsOnChroms.cpp
countReadsOnChroms_LDFLAGS = -lbodealgo -lbodealign -lbodetree -lbodeio -lbodeseq -lbodeutil -lhts -lz -lpthread

makeDensity_SOURCES = makeDensity.cpp
makeDensity_LDFLAGS = -lbodetree -lbodeio -lbodeseq -lbodeutil -lhts -lz -lpthread

readDensityMap_SOURCES = readDensityMap.cpp
readDensityMap_LDFLAGS = -lbodetree -lbodeio -lbodeseq -lbodeutil -lhts -lz -lpthread

AM_CPPFLAGS = -I/usr/local/include
