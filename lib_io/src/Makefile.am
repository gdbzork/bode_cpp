SUBDIRS = bode
lib_LTLIBRARIES = libbodeio.la
libbodeio_la_SOURCES = bodeio.cpp         \
                       bamReader.cpp      \
                       bamWriter.cpp      \
                       bedReader.cpp      \
                       cffReader.cpp      \
                       fastaReader.cpp    \
                       fastqReader.cpp    \
                       intervalReader.cpp \
                       sequenceReader.cpp \
                       writer.cpp
libbodeio_la_CXXFLAGS = -I@top_srcdir@/include $(AM_CXXFLAGS)
libbodeio_la_LDFLAGS = -version-info 1:0:0
