AM_CXXFLAGS = --pedantic -Wall -Werror -std=c++17 -O2
bin_PROGRAMS = hx
hx_SOURCES = hx.cpp
hx_LDADD = @CURSES_LIBS@ -lstdc++fs
hx_CPPFLAGS = @CURSES_CFLAGS@
hx_DEPENDENCIES = $(srcdir)/../docs/hx_run

dist_bin_SCRIPTS = \
	hx-files.sh \
	hx-incs.sh \
	hx-srcs.sh \
	hx-undoc.sh

