if HAVE_CXXTESTGEN
TESTS = bamio_test bedio_test io_test
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

bamio_test_SOURCES = bamio_test.h
nodist_bamio_test_SOURCES = bamio_test.cpp
bedio_test_SOURCES = bedio_test.h
nodist_bedio_test_SOURCES = bedio_test.cpp
io_test_SOURCES = io_test.h
nodist_io_test_SOURCES = io_test.cpp

AM_CPPFLAGS = -I@top_srcdir@/src
AM_LDFLAGS = -L@top_builddir@/src -lbodeio -L/usr/local/lib -lbodeseq -lbodeutil -lhts -lz -lpthread

TESTLIB = @top_builddir@/src/libbodeio.la
CLEANFILES = bamio_test.cpp bedio_test.cpp io_test.cpp

EXTRA_DIST = testdata/test_input.bam testdata/test_input.bed testdata/test_input.sam

SUFFIXES = .cpp .h

$(TESTS): $(TESTLIB)

.h.cpp: $(TESTLIB)
	cxxtestgen --error-printer -o $@ $<

valgrind: $(TESTS)
	for x in $(TESTS); do echo "$$x"; libtool --mode=execute valgrind --leak-check=full "$$x"; done
