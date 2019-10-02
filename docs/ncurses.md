# NCurses Support

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

```
@Add(includes)
	#include <locale.h>
@End(includes)
```

```
@Add(global elements)
	#if HAVE_CURSES
		@Put(ncurses globals)
	#endif
@End(global elements)
```

```
@Def(ncurses globals)
	@put(globals)
@End(ncurses globals)
```

```
@def(globals)
	bool with_ncurses = false;
@end(globals)
```

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

```
@Add(main body)
	#if HAVE_CURSES
		if (with_ncurses) {
			@Put(curses interact);
		}
	#endif
@End(main body)
```

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

```
@def(setup curses)
	setlocale(LC_CTYPE, "");
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
@end(setup curses)
```

```
@def(teardown curses)
	endwin();
@end(teardown curses)
```

```
@add(globals)
	struct End_Of_Curses {};
@end(globals)
```

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

```
@def(curses cases)
	case 'q': throw End_Of_Curses {};
@end(curses cases);
```

```
@def(needed by draw page)
	void draw_number(int l) {
		int r = l / 10;
		if (r) { draw_number(r); }
		addch((l % 10) + '0');
	}

	void draw_line(int l) {
		if (l <= 9) {
			addch(' ');
		}
		draw_number(l);
		addstr(": ");
	}
@end(needed by draw page)
```

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
