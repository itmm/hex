AM_CXXFLAGS = -Wall
bin_PROGRAMS = hx
hx_SOURCES = hx.cpp
hx_LDADD = @CURSES_LIBS@
hx_CPPFLAGS = @CURSES_CFLAGS@

hx.cpp: hx_run

hx_run: \
  ../docs/add.x \
  ../docs/blocks.x \
  ../docs/edit.x \
  ../docs/frag.x \
  ../docs/html.x \
  ../docs/index.x \
  ../docs/input.x \
  ../docs/line.x \
  ../docs/log.x \
  ../docs/ncurses.x \
  ../docs/range.x \
  ../docs/read.x \
  ../docs/todos.x \
  ../docs/view.x \
  ../docs/write.x
	@echo "  HX"
	@cd ../docs && hx
	@touch hx_run

dist_bin_SCRIPTS = hx-files.sh hx-incs.sh
