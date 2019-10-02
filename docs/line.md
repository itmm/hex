# Entering Lines in Viewer and Editor

```
@Add(global elements)
	class Line {
		public:
			@put(line elements);
		private:
			@put(line privates);
	};
@End(global elements)
```

```
@def(line elements)
	int operator()(
		int cur, int end
	) const {
		int res {};
		@put(get line);
		return res;
	}
@end(line elements)
```

```
@def(line privates)
	int _line = -1;
	bool _relative = false;
@end(line privates)
```

```
@add(line elements)
	operator bool() const {
		return _line >= 0 ||  _relative;
	}
@end(line elements)
```

```
@def(get line)
	if (! *this) {
		res = cur;
	} else {
		res = _line;
		if (_relative) { res += cur; }
	}
	if (res < 0) { res = 0; }
	if (res > end) { res = end; }
@end(get line)
```

```
@add(line privates)
	Line(int line, bool relative):
		_line { line },
		_relative { relative }
	{}
@end(line privates)
```

```
@add(line elements)
	Line() = default;
@end(line elements)
```

```
@add(line elements)
	static Line relative(int line) {
		return Line { line, true };
	}
@end(line elements)
```

```
@add(line elements)
	static Line line(int line) {
		return Line { line, false };
	}
@end(line elements)
```

```
@add(line elements)
	static Line begin() {
		return line(0);
	}
@end(line elements)
```

```
@Add(includes)
	#include <limits>
@End(includes)
```

```
@add(line elements)
	static const int max =
		std::numeric_limits<int>::max();
		
	static Line end() {
		return line(max);
	}
@end(line elements)
```

```
@Add(global elements)
	@Put(range prereqs);
	@Put(range vars);
@End(global elements)
```

```
@Def(range vars)
	Line line;
@End(range vars)
```

```
@Add(global elements)
	int get_number(std::string &s) {
		int res = 0;
		while (
			! s.empty() && isdigit(s[0])
		) {
			res = res * 10 + s[0] - '0';
			s.erase(0, 1);
		}
		return res;
	}
@End(global elements)
```

```
@Add(global elements)
	Line get_line(std::string &s) {
		Line line {};
		do {
			@put(parse line);
		} while (false);
		trim(s);
		return line;
	}
@End(global elements)
```

```
@def(parse line)
	if (s[0] == '.') {
		s.erase(0, 1);
		line = Line::relative(0);
		break;
	}
@end(parse line)
```

```
@add(parse line)
	if (s[0] == '+') {
		s.erase(0, 1);
		int n = get_number(s);
		line = Line::relative(n);
		break;
	}
@end(parse line)
```

```
@add(parse line)
	if (s[0] == '-') {
		s.erase(0, 1);
		int n = -get_number(s);
		line = Line::relative(n);
		break;
	}
@end(parse line)
```

```
@add(parse line)
	if (s[0] == '$') {
		line = Line::end();
		s.erase(0, 1);
		continue;
	}
@end(parse line)
```

```
@add(parse line)
	if (isdigit(s[0])) {
		int n = get_number(s);
		line = Line::line(n);
		continue;
	}
@end(parse line);
```

```
@Def(do range)
	line = get_line(cmd);
@End(do range)
```

```
@Add(perform unit-tests)
	@put(unit tests);
@end(perform unit-tests)
```

```
@def(unit tests)
	ASSERT(! Line {});
	ASSERT(Line::begin());
	ASSERT(Line::end());
	ASSERT(Line::end());
	ASSERT(Line::line(0));
	ASSERT(Line::relative(0));
	ASSERT(Line::relative(-2));
@end(unit tests)
```

```
@add(unit tests)
	ASSERT(Line {}(5, 10) == 5);
	ASSERT(Line::begin()(5, 10) == 0);
	ASSERT(Line::end()(5, 10) == 10);
@end(unit tests)
```

```
@add(unit tests)
	ASSERT(Line::line(0)(5, 10) == 0);
	ASSERT(Line::line(6)(5, 10) == 6);
	ASSERT(Line::line(20)(5, 10) == 10);
@end(unit tests)
```

```
@add(unit tests)
	ASSERT(
		Line::relative(2)(5, 10) == 7
	);
	ASSERT(
		Line::relative(7)(5, 10) == 10
	);
	ASSERT(
		Line::relative(-2)(5, 10) == 3
	);
	ASSERT(
		Line::relative(-7)(5, 10) == 0
	);
@end(unit tests)
```

```
@add(unit tests) {
	std::string f = "+3";
	ASSERT(
		get_line(f)(5, 10) == 8
	);
} @end(unit tests)
```

```
@add(unit tests) {
	std::string f = ".";
	ASSERT(
		get_line(f)(5, 10) == 5
	);
} @end(unit tests)
```

```
@add(unit tests) {
	std::string f = "$";
	ASSERT(
		get_line(f)(5, 10) == 10 
	);
} @end(unit tests)
```

```
@Def(do block range)
	if (line) {
		next = line(
			(curBlock - curInput->blocks.begin()) + 1,
			curInput->blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}
@End(do block range)
```

```
@Def(do inputs range)
	if (line) {
		next = line(
			(curInput - inputs.begin()) + 1,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}
@End(do inputs range)
```

```
@Def(do str range)
	if (line) {
		next = line(
			Line::max,
			c.size() + 1
		) - 1;
		if (next < 0) { next = 0; }
	}
@End(do str range)
```
