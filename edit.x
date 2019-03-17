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
@Add(global elements)
	bool valid_cur() {
		if (curInput == inputs.end()) {
			std::cerr << "! no file\n";
			return false;
		}
		if (curBlock == curInput->blocks.end()) {
			std::cerr << "! end\n";
			return false;
		}
		return true;
	}
@End(global elements)
```

```
@Add(run loop)
	if (cmd == "N" || cmd == "Note") {
		if (valid_cur()) {
			insert_before(
				"n",
				curBlock->notes
			);
		}
		continue;
	}
@End(run loop)
```

```
@Add(run loop)
	if (cmd == "A" || cmd == "Add") {
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
		if (valid_cur()) {
			insert_before(
				prefix,
				curBlock->value
			);
		}
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
