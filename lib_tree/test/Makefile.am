TESTS = intervalNode_test intervalSet_test intervalTreeSplit_test intervalTree_test map_test
check_PROGRAMS = $(TESTS)
intervalNode_test_SOURCES = intervalNode_test.cpp intervalNode_test.h
intervalSet_test_SOURCES = intervalSet_test.cpp intervalSet_test.h
intervalTreeSplit_test_SOURCES = intervalTreeSplit_test.cpp intervalTreeSplit_test.h
intervalTree_test_SOURCES = intervalTree_test.cpp intervalTree_test.h
map_test_SOURCES = map_test.cpp map_test.h

AM_CPPFLAGS = -I@top_srcdir@/src
AM_LDFLAGS = -L@top_builddir@/src -lbodetree -lbodeseq -lbodeutil

TESTLIB = @top_builddir@/src/libbodetree.la
CLEANFILES = map_test.cpp

SUFFIXES = .cpp .h

$(TESTS): $(TESTLIB)

.h.cpp: $(TESTLIB)
	cxxtestgen --have-eh --abort-on-fail --error-printer -o $@ $<

valgrind: $(TESTS)
	for x in $(TESTS); do echo "$$x"; libtool --mode=execute valgrind --leak-check=full "$$x"; done
