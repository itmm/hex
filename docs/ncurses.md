# NCurses Support
* use NCurses interface for a full screen viewer/editor

```
@Add(includes)
	#if defined HAVE_CONFIG_H
		#include "config.h"
	#endif
	#if defined HAVE_NCURSESW_CURSES_H
		#include <ncursesw/curses.h>
	#elif defined HAVE_NCURSESW_H
		#include <ncursesw.h>
	#elif defined HAVE_NCURSES_CURSES_H
		#include <ncurses/curses.h>
	#elif defined HAVE_NCURSES_H
		#include <ncurses.h>
	#elif defined HAVE_CURSES_H
		#include <curses.h>
	#endif
@End(includes)
```
* use autoconf to include the right headers

```
@Add(includes)
	#include <locale.h>
@End(includes)
```
* needed for switching to `UTF-8`

```
@Add(global elements)
	#if HAVE_CURSES
		@Put(ncurses globals)
	#endif
@End(global elements)
```
* only define if available

```
@Def(ncurses globals)
	@put(globals)
@End(ncurses globals)
```
* local fragment to reduce typing overhead

```
@def(globals)
	bool with_ncurses = false;
@end(globals)
```
* should NCurses be used

```
@Add(process argument)
	#if HAVE_CURSES
		if (
			arg == "-c" ||
			arg == "--curses"
		) {
			with_ncurses = true;
			interactive = false;
			write_files = false;
			process_files = false;
			html_files = false;
			continue;
		}
	#endif
@End(process argument)
```
* command line argument can signal that NCurses should be used
* this will disable automatic file generation
* and the command line mode

```
@Add(main body)
	#if HAVE_CURSES
		if (with_ncurses) {
			@Put(curses interact);
		}
	#endif
@End(main body)
```
* add NCurses interaction to the `@f(main)` function

```
@add(globals)
	class Ncurses_Handler {
		public:
			Ncurses_Handler() {
				@put(setup curses);
			}
			~Ncurses_Handler() {
				@put(teardown curses);
			}
	};
@end(globals)
```
* special handler to automatically setup and tear down NCurses

```
@def(setup curses)
	setlocale(LC_CTYPE, "");
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
@end(setup curses)
```
* initialize NCurses

```
@def(teardown curses)
	endwin();
@end(teardown curses)
```
* close NCurses window

```
@add(globals)
	struct End_Of_Curses {};
@end(globals)
```
* special NCurses exception that terminates the NCurses interaction

```
@add(globals)
	@put(needed by draw page)
	void draw_page() {
		clear();
		move(0, 0);
		@put(draw page);
		refresh();
	}
@end(globals)
```
* draw page with NCurses

```
@Def(curses interact)
	Ncurses_Handler handler;
	curInput = inputs.begin();
	curBlock = curInput->blocks.begin();
	draw_page();
	int ch;
	try {
		for (;;) {
			switch (ch = getch()) {
				@put(curses cases)
			}
			draw_page();
		}
	} catch (const End_Of_Curses &) {}
@End(curses interact)
```
* loop until NCurses should be terminated

```
@def(curses cases)
	case 'q': throw End_Of_Curses {};
@end(curses cases);
```
* terminates NCurses

```
@def(needed by draw page)
	void draw_number(int l) {
		int r = l / 10;
		if (r) { draw_number(r); }
		addch((l % 10) + '0');
	}
@end(needed by draw page)
```
* draw a number recursively

```
@add(needed by draw page)
	void draw_line(int l) {
		if (l <= 9) {
			addch(' ');
		}
		draw_number(l);
		addstr(": ");
	}
@end(needed by draw page)
```
* draw a line number prefix
* line numbers smaller than `10` are padded with a space

```
@def(draw page)
	if (curBlock->state == RS::header) {
		int i = 0;
		for (const auto &l : curBlock->value) {
			draw_line(++i);
			for (int i = 0; i < curBlock->level; ++i) {
				addch('#');
			}
			addch(' ');
			addstr(l.c_str());
			addstr("\n\n");
		}
	}
@end(draw page)
```
* headers are written with the level number of `#`s

```
@add(draw page)
	if (curBlock->state == RS::code) {
		addstr("    ```\n");
		int i = 0;
		for (const auto &l : curBlock->value) {
			draw_line(++i);
			addstr(l.c_str());
			addch('\n');
		}
		addstr("    ```\n\n");
	}
@end(draw page)
```
* code blocks are prefixed with the code tag from Markdown

```
@add(draw page)
	if (curBlock->state == RS::para) {
		int i = 0;
		for (const auto &l : curBlock->value) {
			draw_line(++i);
			addstr(l.c_str());
			addstr("\n\n");
		}
	}
@end(draw page)
```
* paragraphs are separated by empty lines

```
@add(draw page)
	int j = 0;
	for (const auto &l : curBlock->notes) {
		draw_line(++j);
		addstr("* ");
		addstr(l.c_str());
		addch('\n');
	}
	addch('\n');
@end(draw page)
```
* notes are prefixed with `*`

```
@add(draw page)
	int idx = (curInput - inputs.begin()) + 1;
	draw_number(idx);
	if (idx == static_cast<int>(inputs.size())) {
		addstr(" = $");
	}
	addch(' ');
	addstr(curInput->path().c_str());
	addch(':');
	auto &bs { curInput->blocks };
	idx = (curBlock - bs.begin()) + 1;
	draw_number(idx);
	if (
		idx == static_cast<int>(bs.size())
	) {
		addstr(" = $");
	}
@end(draw page)
```
* draw the current input file number
* then draw the input file name
* draw the current block number


```
@add(curses cases)
	case 'n': {
		int next = (curBlock - curInput->blocks.begin()) + 1;
		while (next >= static_cast<int>(curInput->blocks.size())) {
			--next;
		}
		@Mul(do block range);
		curBlock = curInput->blocks.begin() + next;
		draw_page();
		break;
	}
@end(curses cases)
```
* got to the next block

```
@add(curses cases)
	case 'p' : {
		int next = curBlock - curInput->blocks.begin();
		if (next > 0) {
			--next;
		}
		@Mul(do block range);
		curBlock = curInput->blocks.begin() + next;
		draw_page();
		break;
	}
@end(curses cases)
```
* go to the previous block

```
@add(curses cases)
	case 'f': {
		int next =(curInput - inputs.begin()) + 1;
		while (next >= static_cast<int>(inputs.size())) {
			--next;
		}
		@Mul(do inputs range);
		curInput = inputs.begin() + next;
		curBlock = curInput->blocks.begin();
		draw_page();
		continue;
	}
@end(curses cases)
```
* go to the next input file

```
@add(curses cases)
	case 'b': {
		int next = curInput - inputs.begin();
		if (next) {
			--next;
		}
		@Mul(do inputs range);
		curInput = inputs.begin() + next;
		curBlock = curInput->blocks.begin();
		draw_page();
		continue;
	}
@end(curses cases)
```
* go to the previous input file

