# Edit slides in place

```
@Add(global elements)
	void insert_before(
		std::vector<std::string> &c,
		std::vector<std::string>::iterator i
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
				curBlock->notes,
				curBlock->notes.end()
			);
		}
		continue;
	}
@End(run loop)
```

```
@Add(run loop)
	if (cmd == "A" || cmd == "Add") {
		if (valid_cur()) {
			insert_before(
				curBlock->value,
				curBlock->value.end()
			);
		}
		continue;
	}
@End(run loop)
```

```
@def(insert before)
	std::string line;
	for (;;) {
		std::getline(std::cin, line);
		auto b = line.begin();
		auto e = line.end();
		while (b != e && *b <= ' ') { ++b; }
		std::string t { b, e };
		if (t.empty()) { continue; }
		if (t == ".") { break; }
		int d = i - c.begin();
		c.insert(i, t);
		i = c.begin() + (d + 1);
	}
	draw_block();
@end(insert before)
```
