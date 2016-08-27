if HAVE_CXXTESTGEN
TESTS = bitBucket_test       \
        bodeException_test   \
        chromosome_test      \
        cigar_test           \
        configuration_test   \
        densitySet_test      \
        flags_test           \
        histogram_test       \
        intervalDensity_test \
        lineType_test        \
        log_test             \
        util_test
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
bitBucket_test_SOURCES = bitBucket_test.h
nodist_bitBucket_test_SOURCES = bitBucket_test.cpp
bodeException_test_SOURCES = bodeException_test.h
nodist_bodeException_test_SOURCES = bodeException_test.cpp
chromosome_test_SOURCES = chromosome_test.h
nodist_chromosome_test_SOURCES = chromosome_test.cpp
cigar_test_SOURCES = cigar_test.h
nodist_cigar_test_SOURCES = cigar_test.cpp
configuration_test_SOURCES = configuration_test.h
nodist_configuration_test_SOURCES = configuration_test.cpp
densitySet_test_SOURCES = densitySet_test.h
densitySet_test_LDFLAGS = -L@top_builddir@/src -lbodeutil -lhts -lz -lpthread
nodist_densitySet_test_SOURCES = densitySet_test.cpp
flags_test_SOURCES = flags_test.h
nodist_flags_test_SOURCES = flags_test.cpp
histogram_test_SOURCES = histogram_test.h
nodist_histogram_test_SOURCES = histogram_test.cpp
intervalDensity_test_SOURCES = intervalDensity_test.h
nodist_intervalDensity_test_SOURCES = intervalDensity_test.cpp
lineType_test_SOURCES = lineType_test.h
nodist_lineType_test_SOURCES = lineType_test.cpp
log_test_SOURCES = log_test.h
nodist_log_test_SOURCES = log_test.cpp
util_test_SOURCES = util_test.h
nodist_util_test_SOURCES = util_test.cpp

AM_CPPFLAGS = -I@top_srcdir@/src
AM_LDFLAGS = -L@top_builddir@/src -lbodeutil

TESTLIB = @top_builddir@/src/libbodeutil.la
CLEANFILES = bitBucket_test.cpp \
             bodeException_test.cpp \
             chromosome_test.cpp \
             cigar_test.cpp \
             configuration_test.cpp \
             densitySet_test.cpp \
             flags_test.cpp \
             histogram_test.cpp \
             intervalDensity_test.cpp \
             lineType_test.cpp \
             log_test.cpp \
             util_test.cpp

SUFFIXES = .cpp .h

EXTRA_DIST = testdata/runProgMeter.cpp testdata/test_data_chr21.bam testdata/test_peaks_chr21.bed

$(TESTS): $(TESTLIB)

.h.cpp: $(TESTLIB)
	cxxtestgen --have-eh --abort-on-fail --error-printer -o $@ $<

valgrind: $(TESTS)
	for x in $(TESTS); do echo "$$x"; libtool --mode=execute valgrind --leak-check=full "$$x"; done
