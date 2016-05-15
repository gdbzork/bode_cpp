SUBDIRS = util

bin_PROGRAMS = coverage dustfilter errorRates filterLength filterOut lc \
               makeWiggle sampleFQ screenLinker screenLinkerExact \
               screenLinker_paired solexa2phred sortBed summarizeDuplicates \
               summarizeDuplicatesBam summarizeDuplicates_dict summarizeFile \
               testQualityScale fixFastq

coverage_SOURCES = coverage.c
coverage_LDADD = util/libgutil.a
coverage_LDFLAGS = -lm

dustfilter_SOURCES = dustfilter.c
dustfilter_LDADD = util/libgutil.a
dustfilter_LDFLAGS = -lm

errorRates_SOURCES = errorRates.c
errorRates_LDADD = util/libgutil.a
errorRates_LDFLAGS = -lm

filterLength_SOURCES = filterLength.c
filterLength_LDADD = util/libgutil.a
filterLength_LDFLAGS = -lm

filterOut_SOURCES = filterOut.c
filterOut_LDADD = util/libgutil.a
filterOut_LDFLAGS = -lm

lc_SOURCES = lc.c

makeWiggle_SOURCES = makeWiggle.c
makeWiggle_LDADD = util/libgutil.a
makeWiggle_LDFLAGS = -lm

sampleFQ_SOURCES = sampleFQ.c
sampleFQ_LDADD = util/libgutil.a
sampleFQ_LDFLAGS = -lm

screenLinker_SOURCES = screenLinker.c
screenLinker_LDADD = util/libgutil.a
screenLinker_LDFLAGS = -lm

screenLinkerExact_SOURCES = screenLinkerExact.c
screenLinkerExact_LDADD = util/libgutil.a
screenLinkerExact_LDFLAGS = -lm

screenLinker_paired_SOURCES = screenLinker_paired.c
screenLinker_paired_LDADD = util/libgutil.a
screenLinker_paired_LDFLAGS = -lm

solexa2phred_SOURCES = solexa2phred.c
solexa2phred_LDADD = util/libgutil.a
solexa2phred_LDFLAGS = -lm

fixFastq_SOURCES = fixFastq.c
fixFastq_LDADD = util/libgutil.a
fixFastq_LDFLAGS = -lm

sortBed_SOURCES = sortBed.c
sortBed_LDADD = util/libgutil.a
sortBed_LDFLAGS = -lm

summarizeDuplicates_SOURCES = summarizeDuplicates.c
summarizeDuplicates_LDADD = util/libgutil.a
summarizeDuplicates_LDFLAGS = -lm

summarizeDuplicatesBam_SOURCES = summarizeDuplicatesBam.c
summarizeDuplicatesBam_LDADD = util/libgutil.a
summarizeDuplicatesBam_LDFLAGS = -lhts -lz -lm -lpthread

summarizeDuplicates_dict_SOURCES = summarizeDuplicates_dict.c
summarizeDuplicates_dict_LDADD = util/libgutil.a
summarizeDuplicates_dict_LDFLAGS = -lm

summarizeFile_SOURCES = summarizeFile.c
summarizeFile_LDADD = util/libgutil.a
summarizeFile_LDFLAGS = -lm

testQualityScale_SOURCES = testQualityScale.c
testQualityScale_LDADD = util/libgutil.a
testQualityScale_LDFLAGS = -lm

AM_CPPFLAGS = -I@top_srcdir@/include
