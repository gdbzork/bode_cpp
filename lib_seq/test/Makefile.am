if HAVE_CXXTESTGEN
TESTS = bed_test
check_PROGRAMS = $(TESTS)
else
TESTS =
.PHONY: check
check:
	@echo 
	@echo 
	@echo 'make check' requires cxxtestgen to be installed.
	@echo 
	@echo 
endif

bed_test_SOURCES = bed_test.h
nodist_bed_test_SOURCES = bed_test.cpp

AM_CPPFLAGS = -I@top_srcdir@/src
AM_LDFLAGS = -L@top_builddir@/src -lbodeseq -lbodeutil -lhts -lpthread

TESTLIB = @top_builddir@/src/libbodeseq.la
CLEANFILES = bed_test.cpp

SUFFIXES = .cpp .h

$(TESTS): $(TESTLIB)

.h.cpp: $(TESTLIB)
	cxxtestgen --have-eh --abort-on-fail --error-printer -o $@ $<

valgrind: $(TESTS)
	for x in $(TESTS); do echo "$$x"; libtool --mode=execute valgrind --leak-check=full "$$x"; done
