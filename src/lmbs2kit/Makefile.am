lib_LTLIBRARIES = liblmbs2kit.la
lmbs2kitincludedir = $(pkgincludedir)/lmbs2kit

AM_CPPFLAGS = $(FFTW_CFLAGS)
AM_LDFLAGS = -no-undefined -version-info 1:1:0

liblmbs2kit_la_LIBADD = $(FFTW_LIBS)

lmbs2kitinclude_HEADERS = \
    seminaive.h \
    FST_semi_memo.h \
    primitive.h \
    cospmls.h \
    pmls.h \
    naive_synthesis.h \
    makeweights.h \
    csecond.h

liblmbs2kit_la_SOURCES = \
    seminaive.c \
    FST_semi_memo.c \
    primitive.c \
    cospmls.c \
    pmls.c \
    naive_synthesis.c \
    makeweights.c \
    csecond.c
