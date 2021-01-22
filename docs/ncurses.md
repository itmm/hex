# NCurses Support
* use NCurses interface for a full screen viewer/editor

```c++
@Add(includes)
	#if defined HAVE_CONFIG_H
		#include "config.h"
	#endif
@End(includes)
```
* use autoconf includes

```c++
@Add(includes)
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

```c++
@Add(includes)
	#include <locale.h>
@End(includes)
```
* needed for switching to `UTF-8`

```c++
@Add(global elements)
	#if HAVE_CURSES
		@Put(ncurses globals)
	#endif
@End(global elements)
```
* only define if available

```c++
@Def(ncurses globals)
	@put(globals)
@End(ncurses globals)
```
* local fragment to reduce typing overhead

```c++
@def(globals)
	bool with_ncurses = false;
@end(globals)
```
* should NCurses be used

```c++
@Add(process argument)
	#if HAVE_CURSES
		if (arg == "-c" || arg == "--curses") {
			@put(activate curses);
			continue;
		}
	#endif
@End(process argument)
```
* command line argument can signal that NCurses should be used

```c++
@def(activate curses)
	with_ncurses = true;
	interactive = false;
	write_files = false;
	process_files = false;
	html_files = false;
@end(activate curses)
```
* disable automatic file generation
* and the command line mode

```c++
@Add(main body)
	#if HAVE_CURSES
		if (with_ncurses) {
			@Put(curses interact);
		}
	#endif
@End(main body)
```
* add NCurses interaction to the `@f(main)` function

```c++
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

```c++
@def(setup curses)
	setlocale(LC_CTYPE, "");
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
@end(setup curses)
```
* initialize NCurses

```c++
@def(teardown curses)
	endwin();
@end(teardown curses)
```
* close NCurses window

```c++
@add(globals)
	struct End_Of_Curses {};
@end(globals)
```
* special NCurses exception that terminates the NCurses interaction

```c++
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

```c++
@Def(curses interact)
	Ncurses_Handler handler;
	curInput = inputs.begin();
	curBlock = curInput->second.blocks.begin();
	draw_page();
@End(curses interact)
```
* set iterator to first block of first file
* and display block

```c++
@Add(curses interact)
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

```c++
@def(curses cases)
	case 'q': throw End_Of_Curses {};
@end(curses cases);
```
* terminates NCurses

```c++
@def(needed by draw page)
	void draw_number(int l) {
		int r = l / 10;
		if (r) { draw_number(r); }
		addch((l % 10) + '0');
	}
@end(needed by draw page)
```
* draw a number recursively

```c++
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

```c++
@def(draw page)
	if (curBlock->state == RS::header) {
		@put(draw header);
	}
@end(draw page)
```
* draw header block

```c++
@def(draw header)
	int i = 0;
	for (const auto &l : curBlock->value) {
		@put(draw header line);
	}
@end(draw header)
```
* draw all headers in the current header block

```c++
@def(draw header line)
	draw_line(++i);
	for (int j = 0; j < curBlock->level; ++j) {
		addch('#');
	}
	addch(' ');
	addstr(l.c_str());
	addstr("\n\n");
@end(draw header line)
```
* headers are written with the level number of `#`s

```c++
@add(draw page)
	if (curBlock->state == RS::code) {
		@put(draw code);
	}
@end(draw page)
```
* draw code block

```c++
@def(draw code)
	addstr("    ```\n");
	int i = 0;
	for (const auto &l : curBlock->value) {
		draw_line(++i);
		addstr(l.c_str());
		addch('\n');
	}
	addstr("    ```\n\n");
@end(draw code)
```
* code blocks are prefixed with the code tag from Markdown

```c++
@add(draw page)
	if (curBlock->state == RS::para) {
		@put(draw para);
	}
@end(draw page)
```
* draw paragraph block

```c++
@def(draw para)
	int i = 0;
	for (const auto &l : curBlock->value) {
		draw_line(++i);
		addstr(l.c_str());
		addstr("\n\n");
	}
@end(draw para)
```
* paragraphs are separated by empty lines

```c++
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

```c++
@add(draw page)
	int idx = 1;
	for (const auto &xx : inputs) {
		if (xx.first == curInput->first) { break; }
		++idx;
	}
	draw_number(idx);
	if (idx == static_cast<int>(inputs.size())) {
		addstr(" = $");
	}
	addch(' ');
	addstr(curInput->first.c_str());
	addch(':');
@end(draw page)
```
* draw the current input file number
* then draw the input file name

```c++
@add(draw page)
	auto &bs { curInput->second.blocks };
	idx = (curBlock - bs.begin()) + 1;
	draw_number(idx);
	if (idx == static_cast<int>(bs.size())) {
		addstr(" = $");
	}
@end(draw page)
```
* draw the current block number

```c++
@add(curses cases)
	case 'n': {
		@put(next block);
		draw_page();
		break;
	}
@end(curses cases)
```
* got to the next block

```c++
@def(next block)
	int next = (curBlock - curInput->second.blocks.begin()) + 1;
	while (next >= static_cast<int>(curInput->second.blocks.size())) {
		--next;
	}
	@Mul(do block range);
	curBlock = curInput->second.blocks.begin() + next;
@end(next block)
```
* got to the next block

```c++
@add(curses cases)
	case 'p' : {
		@put(prev block);
		draw_page();
		break;
	}
@end(curses cases)
```
* go to the previous block

```c++
@def(prev block)
	int next = curBlock - curInput->second.blocks.begin();
	if (next > 0) {
		--next;
	}
	@Mul(do block range);
	curBlock = curInput->second.blocks.begin() + next;
@end(prev block)
```
* go to the previous block

```c++
@add(curses cases)
	case 'f': {
		@put(next input);
		draw_page();
		continue;
	}
@end(curses cases)
```
* go to the next input file

```c++
@def(next input)
	int next = 1;
	for (const auto &xx : inputs) {
		if (xx.first == curInput->first) { break; }
		++next;
	}
	while (next >= static_cast<int>(inputs.size())) {
		--next;
	}
	@Mul(do inputs range);
	curInput = inputs.begin();
	for (; next > 0 && curInput != inputs.end(); --next) {
		++curInput;
	}
	curBlock = curInput->second.blocks.begin();
@end(next input)
```
* go to the next input file

```c++
@add(curses cases)
	case 'b': {
		@put(prev input);
		draw_page();
		continue;
	}
@end(curses cases)
```
* go to the previous input file

```c++
@def(prev input)
	int next = 0;
	for (const auto &xx : inputs) {
		if (xx.first == curInput->first) { break; }
		++next;
	}
	if (next) {
		--next;
	}
	@Mul(do inputs range);
	curInput = inputs.begin();
	for (; next > 0 && curInput != inputs.end(); --next) {
		++curInput;
	}
	curBlock = curInput->second.blocks.begin();
@end(prev input)
```
* go to the previous input file
