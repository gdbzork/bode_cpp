TESTS = alignment_test
check_PROGRAMS = $(TESTS)
alignment_test_SOURCES = alignment_test.cpp alignment_test.h

AM_CPPFLAGS = -I@top_srcdir@/src
AM_LDFLAGS = -L@top_builddir@/src -lbodealign

TESTLIB = @top_builddir@/src/libbodealign.la
CLEANFILES = alignment_test.cpp

SUFFIXES = .cpp .h

$(TESTS): $(TESTLIB)

.h.cpp: $(TESTLIB)
	cxxtestgen --have-eh --abort-on-fail --error-printer -o $@ $<

valgrind: $(TESTS)
	for x in $(TESTS); do echo "$$x"; libtool --mode=execute valgrind --leak-check=full "$$x"; done
