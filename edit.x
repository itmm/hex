# Edit slides in place

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
