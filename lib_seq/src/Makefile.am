SUBDIRS = bode
lib_LTLIBRARIES = libbodeseq.la
libbodeseq_la_SOURCES = bam.cpp      \
                        bed.cpp      \
                        bodeseq.cpp  \
                        fasta.cpp    \
                        fastq.cpp    \
                        interval.cpp \
                        sequence.cpp
libbodeseq_la_CXXFLAGS = -I@top_srcdir@/include $(AM_CXXFLAGS)
libbodeseq_la_LDFLAGS = -version-info 1:1:0
