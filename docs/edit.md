# Edit blocks in place
* using a terminal editor to edit blocks

```
@Add(global elements)
	void insert_before(
		const std::string &prefix,
		std::vector<std::string> &c
	) {
		@put(insert before);
	}
@End(global elements)
```
* function to interactively insert lines at the end of the container
* the `prefix` is the prompt that is printed

```
@Add(run loop)
	if (cmd == "l" || cmd == "list") {
		insert_before(
			"l", curBlock->notes
		);
		continue;
	}
@End(run loop)
```
* the `list` or `l` command adds notes

```
@Add(run loop)
	if (cmd == "a" || cmd == "add") {
		std::string prefix;
		switch (curBlock->state) {
			case RS::header:
				prefix = "h"; break;
			case RS::code:
				prefix = "c"; break;
			case RS::para:
				prefix = "p"; break;
			default:
				prefix = "?"; break;
		}
		insert_before(
			prefix,
			curBlock->value
		);
		continue;
	}
@End(run loop)
```
* the `add` or `a` command adds value lines

```
@def(insert before)
	int next = c.size();
	@Put(do str range);
	std::string l;
	for (;;) {
		std::cout << prefix << ' ' << (next + 1) << "? ";
		std::getline(std::cin, l);
		auto b = l.begin();
		auto e = l.end();
		while (b != e && *b <= ' ') { ++b; }
		std::string t { b, e };
		if (t.empty()) { continue; }
		if (t == ".") { break; }
		c.insert(c.begin() + next, l);
		++next;
	}
	draw_block();
@end(insert before)
```
* insert strings at the end of the container
* read lines until a single `.` is read
* spaces at the beginning of the line are stripped

```
@Add(run loop)
	if (cmd == ">>") {
		if (curBlock->state == RS::header) {
			++curBlock->level;
			draw_block();
			continue;
		}
	}
@End(run loop)
```
* `>>` increases the level of a header

```
@Add(run loop)
	if (cmd == "<<") {
		if (curBlock->level > 1) {
			--curBlock->level;
			draw_block();
			continue;
		}
	}
@End(run loop)
```
* `<<` decreases the level of a header
