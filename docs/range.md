# Pair of `Line`s
* one or two `Line`s form a range expression

```c++
@Def(range prereqs)
	class Range {
		public:
			@put(range elements);
		private:
			Line prev_;
			Line last_;
	};
@End(range prereqs)
```
* range has two `Line`s

```c++
@def(range elements)
	Line prev() {
		return prev_ ? prev_ : last_;
	}
@end(range elements)
```
* first `Line`
* if `_prev` is available, it is returned
* otherwise `_last` is returned

```c++
@add(range elements)
	Line last() {
		return last_;
	}
@end(range elements)
```
* getter for last read `Line`

```c++
@add(range elements)
	Range &operator<<(const Line &l) {
		prev_ = last_;
		last_ = l;
		return *this;
	}
@end(range elements)
```
* push `Line` into `Range`

```c++
@add(range elements)
	operator bool() {
		return last_;
	}
@end(range elements)
```
* is `_last` a valid `Line`

```c++
@Add(range vars)
	Range range;
@End(range vars)
```
* global `Range` element

```c++
@Rep(do range)
	range = Range {};
	range << get_line(cmd);
	if (! cmd.empty() && cmd[0] == ',') {
		cmd.erase(0, 1);
		range << get_line(cmd);
	}
@End(do range)
```
* read one or two `Line` elements
* two `Line` elements are separated with `,`

```c++
@Rep(do block range)
	if (range) {
		next = range.last()((curBlock -
			curInput->second.blocks.begin()) + 1,
			curInput->second.blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}
@End(do block range)
```
* blocks use only one element from range

```c++
@Rep(do inputs range)
	if (range) {
		int idx = 1;
		for (const auto &xx : inputs) {
			if (xx.first == curInput->first) { break; }
			++idx;
		}
		next = range.last()(idx, inputs.size()) - 1;
		if (next < 0) { next = 0; }
	}
@End(do inputs range)
```
* input files use only one element from range

```c++
@Rep(do str range)
	if (range) {
		next = range.last()(Line::max, c.size() + 1) - 1;
		if (next < 0) { next = 0; }
		int p = range.prev()(Line::max, c.size() + 1) - 1;
		if (p < 0) { p = 0; }
		@put(erase range);
	}
@End(do str range)
```
* strings can replace a full range with the new elements

```c++
@def(erase range)
	if (p < next) {
		c.erase(c.begin() + p, c.begin() + next);
		next = p;
	}
@end(erase range)
```
* erase whole range

```c++
@Rep(range vars)
	Range range;
@End(range vars)
```
* the fragment is replaced
* other local variables are no longer needed
