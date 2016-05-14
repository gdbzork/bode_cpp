SUBDIRS = bode
lib_LTLIBRARIES = libbodealgo.la
libbodealgo_la_SOURCES = baseComposition.cpp \
                         bodealgo.cpp        \
                         chromCounter.cpp    \
                         locationFilter.cpp  \
                         readCounter.cpp     \
                         screenLinker.cpp    \
                         screener.cpp
libbodealgo_la_CXXFLAGS = -I@top_srcdir@/include $(AM_CXXFLAGS)
libbodealgo_la_LDFLAGS = -version-info 1:0:0
