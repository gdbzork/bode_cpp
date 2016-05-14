SUBDIRS = bode
lib_LTLIBRARIES = libbodealign.la
libbodealign_la_SOURCES = aligner.cpp            \
                          alignerEndsFree.cpp    \
                          alignerGlobal.cpp      \
                          alignerLocal.cpp       \
                          alignerPartialFree.cpp \
                          alignment.cpp          \
                          alignmentException.cpp \
                          bodealign.cpp
libbodealign_la_CXXFLAGS = -I@top_srcdir@/include $(AM_CXXFLAGS)
libbodealign_la_LDFLAGS = -version-info 1:0:0
