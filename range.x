# Pair of lines

```
@Def(range prereqs)
	class Range {
		public:
			@put(range elements);
		private:
			Line _prev;
			Line _last;
	};
@End(range prereqs)
```

```
@def(range elements)
	Line prev() {
		return _prev ?: _last;
	}
@end(range elements)
```

```
@add(range elements)
	Line last() {
		return _last;
	}
@end(range elements)
```

```
@add(range elements)
	Range &operator<<(const Line &l) {
		_prev = _last;
		_last = l;
		return *this;
	}
@end(range elements)
```

```
@add(range elements)
	operator bool() {
		return _last;
	}
@end(range elements)
```

```
@Add(range vars)
	Range range;
@End(range vars)
```

```
@Rep(do range)
	range = Range {};
	range << get_line(cmd);
	if (! cmd.empty() && cmd[0] == ',') {
		cmd.erase(0, 1);
		range << get_line(cmd);
	}
@End(do range)
```

```
@Rep(do block range)
	if (range) {
		next = range.last()(
			(curBlock - curInput->blocks.begin()) + 1,
			curInput->blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}
@End(do block range)
```

```
@Rep(do inputs range)
	if (range) {
		next = range.last()(
			(curInput - inputs.begin()) + 1,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}
@End(do inputs range)
```

```
@Rep(do str range)
	if (range) {
		next = range.last()(
			Line::max, c.size() + 1
		) - 1;
		if (next < 0) { next = 0; }
		int p = range.prev()(
			Line::max, c.size() + 1
		) - 1;
		if (p < 0) { p = 0; }
		if (p < next) {
			c.erase(
				c.begin() + p,
				c.begin() + next
			);
			next = p;
		}
	}
@End(do str range)
```

```
@Rep(range vars)
	Range range;
@End(range vars)
```
