lib_LTLIBRARIES = libcmdline.la
cmdlineincludedir = $(pkgincludedir)/libcmdline

AM_LDFLAGS = -version-info @lt_major@:@lt_revision@:@lt_age@

libcmdline_la_SOURCES = \
	CmdArg.cc \
	CmdLine.cc \
	CmdLineError.cc \
	Compiler.cc

cmdlineinclude_HEADERS = \
	ArgIter.hh \
	ArgvIter.hh \
	CmdArg.hh CmdArg.icc \
	CmdArgs.hh \
	CmdLine.hh CmdLine.icc \
	CmdLineError.hh \
	Compiler.hh \
	Errors.hh \
	IStreamIter.hh
