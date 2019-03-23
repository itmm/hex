# NCurses Support

```
@Add(includes)
	#if WITH_NCURSES
		#include <ncurses.h>
	#endif
@End(includes)
```

```
@Add(global elements)
	#if WITH_NCURSES
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
	#if WITH_NCURSES
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
	#if WITH_NCURSES
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
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
@end(setup curses)
```

```
@def(teardown curses)
	endwin();
	std::cerr << "done\n";
@end(teardown curses)
```

```
@add(globals)
	struct End_Of_Curses {};
@end(globals)
```

```
@add(globals)
	void draw_page() {
		printw("IN CURSES !!");
		refresh();
	}
@end(globals)
```

```
@Def(curses interact)
	Ncurses_Handler handler;
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
