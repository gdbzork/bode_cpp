TESTS = skel_test
check_PROGRAMS = $(TESTS)
skel_test_SOURCES = skel_test.cpp

AM_CPPFLAGS = -I@top_srcdir@/src
AM_LDFLAGS = -L@top_builddir@/src -lbodealgo -lbodealign -lbodetree -lbodeio -lbodeseq -lbodeutil -lhts -lpthread -lz

TESTLIB = @top_builddir@/src/libbodealgo.la
CLEANFILES = skel_test.cpp

SUFFIXES = .cpp .h

$(TESTS): $(TESTLIB)

.h.cpp: $(TESTLIB)
	cxxtestgen --have-eh --abort-on-fail --error-printer -o $@ $<

valgrind: $(TESTS)
	for x in $(TESTS); do echo "$$x"; libtool --mode=execute valgrind --leak-check=full "$$x"; done
