SUBDIRS = bode
lib_LTLIBRARIES = libbodeutil.la
libbodeutil_la_SOURCES = bitBucket.cpp       \
                         bodeException.cpp   \
                         chromosome.cpp      \
                         cigar.cpp           \
                         configuration.cpp   \
                         densitySet.cpp      \
                         flags.cpp           \
                         genomeData.cpp      \
                         histogram.cpp       \
                         intervalDensity.cpp \
                         log.cpp             \
                         ticker.cpp          \
                         util.cpp
libbodeutil_la_CXXFLAGS = -I@top_srcdir@/include $(AM_CXXFLAGS)
libbodeutil_la_LDFLAGS = -version-info 1:1:0
