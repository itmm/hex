docdir = $(datadir)/doc/@PACKAGE@
dist_doc_DATA = add.html \
	add.md \
	blocks.html \
	blocks.md \
	edit.html \
	edit.md \
	frag.html \
	frag.md \
	html.html \
	html.md \
	hx_run \
	index.html \
	index.md \
	input.html \
	input.md \
	line.html \
	line.md \
	log.html \
	log.md \
	ncurses.html \
	ncurses.md \
	range.html \
	range.md \
	read.html \
	read.md \
	todos.html \
	todos.md \
	view.html \
	view.md \
	write.html \
	write.md

hx_run: add.md \
	blocks.md \
	edit.md \
	frag.md \
	html.md \
	index.md \
	input.md \
	line.md \
	log.md \
	ncurses.md \
	range.md \
	read.md \
	todos.md \
	view.md \
	write.md
	cd $(srcdir) && which hx 2> /dev/null && hx || true
	date >$(srcdir)/$@

