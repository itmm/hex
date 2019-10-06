# Entering Lines in Viewer and Editor
* the `Line` class can refer to specific entry in a vector

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
* a line has private and public elements

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
* to calculate the absolute line number, the method must know the
  current line and the last line

```
@def(line privates)
	int _line { -1 };
	bool _relative { false };
@end(line privates)
```
* integer for the line
* and a flag, if the line is relative

```
@add(line elements)
	operator bool() const {
		return _line >= 0 || _relative;
	}
@end(line elements)
```
* a line is valid, if the number is non-negative
* or the line is relative

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
* if the line is not valid, the current line is returned
* otherwise the line is assigned
* if the line is relative, the value is added to the current line
* the result is clamped to the valid interval

```
@add(line privates)
	Line(int line, bool relative):
		_line { line },
		_relative { relative }
	{}
@end(line privates)
```
* constructor copies arguments

```
@add(line elements)
	Line() = default;
@end(line elements)
```
* empty constructor is possible

```
@add(line elements)
	static Line relative(int line) {
		return Line { line, true };
	}
@end(line elements)
```
* create relative line

```
@add(line elements)
	static Line line(int line) {
		return Line { line, false };
	}
@end(line elements)
```
* create absolute line

```
@add(line elements)
	static Line begin() {
		return line(0);
	}
@end(line elements)
```
* create reference to first line

```
@Add(includes)
	#include <limits>
@End(includes)
```
* needs `std::numeric_limits`

```
@add(line elements)
	static const int max =
		std::numeric_limits<int>::max();
		
	static Line end() {
		return line(max);
	}
@end(line elements)
```
* end refers to a very big line number

```
@Add(global elements)
	@Put(range prereqs);
	@Put(range vars);
@End(global elements)
```
* needed global elements

```
@Def(range vars)
	Line line;
@End(range vars)
```
* variable holds current line specification
* if any

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
* parse a decimal non-negative integer

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
* parses the head of a string, if it contains a line specification
* the line parts are removed from the input string

```
@def(parse line)
	if (s[0] == '.') {
		s.erase(0, 1);
		line = Line::relative(0);
		break;
	}
@end(parse line)
```
* a `.` is treated as the current line

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
* a `+` signals a positive relative offset from the current line

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
* a `-` signals a negative relative offset from the current line

```
@add(parse line)
	if (s[0] == '$') {
		line = Line::end();
		s.erase(0, 1);
		continue;
	}
@end(parse line)
```
* a `$` signals the last line

```
@add(parse line)
	if (isdigit(s[0])) {
		int n = get_number(s);
		line = Line::line(n);
		continue;
	}
@end(parse line);
```
* a number without `+` or `-` signals an absolute line position

```
@Def(do range)
	line = get_line(cmd);
@End(do range)
```
* initialize the line from the command

```
@Add(perform unit-tests)
	@put(unit-tests);
@end(perform unit-tests)
```
* own fragment for unit-tests

```
@def(unit-tests)
	ASSERT(! Line {});
@end(unit-tests)
```
* empty `Line` is not valid

```
@add(unit-tests)
	ASSERT(Line::begin());
	ASSERT(Line::end());
@end(unit-tests)
```
* begin and end are valid

```
@add(unit-tests)
	ASSERT(Line::line(0));
@end(unit-tests)
```
* absolute line is valid

```
@add(unit-tests)
	ASSERT(Line::relative(0));
	ASSERT(Line::relative(-2));
@end(unit-tests)
```
* relative lines are valid

```
@add(unit-tests)
	ASSERT(Line {}(5, 10) == 5);
@end(unit-tests)
```
* empty line returns current line

```
@add(unit-tests)
	ASSERT(Line::begin()(5, 10) == 0);
	ASSERT(Line::end()(5, 10) == 10);
@end(unit-tests)
```
* begin and end return the correct values

```
@add(unit-tests)
	ASSERT(Line::line(0)(5, 10) == 0);
	ASSERT(Line::line(6)(5, 10) == 6);
	ASSERT(Line::line(20)(5, 10) == 10);
@end(unit-tests)
```
* absolute lines are returned
* but may be clamped

```
@add(unit-tests)
	ASSERT(
		Line::relative(2)(5, 10) == 7
	);
@end(unit-tests)
```
* simple positive relative line

```
@add(unit-tests)
	ASSERT(
		Line::relative(7)(5, 10) == 10
	);
@end(unit-tests)
```
* too big movement is clamped

```
@add(unit-tests)
	ASSERT(
		Line::relative(-2)(5, 10) == 3
	);
@end(unit-tests)
```
* simple negative relative line

```
@add(unit-tests)
	ASSERT(
		Line::relative(-7)(5, 10) == 0
	);
@end(unit-tests)
```
* too big negative movement is clamped

```
@add(unit-tests) {
	std::string f = "+3";
	ASSERT(
		get_line(f)(5, 10) == 8
	);
} @end(unit-tests)
```
* parse positive relative

```
@add(unit-tests) {
	std::string f = ".";
	ASSERT(
		get_line(f)(5, 10) == 5
	);
} @end(unit-tests)
```
* parse current line

```
@add(unit-tests) {
	std::string f = "$";
	ASSERT(
		get_line(f)(5, 10) == 10 
	);
} @end(unit-tests)
```
* parse last line

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
* parse range over blocks collection

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
* parse range over input files

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
* string ranges do not support relative movement

