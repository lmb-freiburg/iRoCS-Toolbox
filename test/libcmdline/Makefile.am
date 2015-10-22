TESTS          = testCmdLine
check_PROGRAMS = $(TESTS)

bin_PROGRAMS   = test_CmdLine 

AM_CPPFLAGS = -I$(top_srcdir)/src

LDADD = $(top_builddir)/src/libcmdline/libcmdline.la

noinst_HEADERS = lmbunit.hh

test_CmdLine_SOURCES = test_CmdLine.cc
testCmdLine_SOURCES = testCmdLine.cc
