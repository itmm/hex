AM_CXXFLAGS = -Wall
bin_PROGRAMS = hx
hx_SOURCES = hx.cpp
hx_LDADD = @CURSES_LIBS@
hx_CPPFLAGS = @CURSES_CFLAGS@

hx.cpp: hx_run

hx_run: \
  ../docs/add.md \
  ../docs/blocks.md \
  ../docs/edit.md \
  ../docs/frag.md \
  ../docs/html.md \
  ../docs/index.md \
  ../docs/input.md \
  ../docs/line.md \
  ../docs/log.md \
  ../docs/ncurses.md \
  ../docs/range.md \
  ../docs/read.md \
  ../docs/todos.md \
  ../docs/view.md \
  ../docs/write.md
	@echo "  HX"
	@cd ../docs && hx index.md
	@date > hx_run

dist_bin_SCRIPTS = \
	hx-files.sh \
	hx-incs.sh \
	hx-srcs.sh \
	hx-undoc.sh

