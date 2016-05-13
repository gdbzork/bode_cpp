SUBDIRS = bode
lib_LTLIBRARIES = libbodetree.la
libbodetree_la_SOURCES = bodetree.cpp          \
                         intervalNode.cpp      \
                         intervalSet.cpp       \
                         intervalSetLeft.cpp   \
                         intervalSetSplit.cpp  \
                         intervalTree.cpp      \
                         intervalTreeLeft.cpp  \
                         intervalTreeSplit.cpp
libbodetree_la_CXXFLAGS = -I@top_srcdir@/include $(AM_CXXFLAGS)
libbodetree_la_LDFLAGS = -version-info 1:0:0
