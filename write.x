# Write all `x`-files
* Replace all read `x`-files with the current content

```
@Add(global elements)
	@put(needed by write_x)
	void write_x() {
		for (const auto &cur : inputs) {
			@put(write cur);
		}
	}
@End(global elements)
```

```
@Add(run loop)
	if (cmd == "W" || cmd == "Write") {
		write_x();
		continue;
	}
@End(run loop)
```

```
@def(write cur)
	std::ofstream out {
		cur.path().c_str()
	};
@end(write cur)
```

```
@add(write cur)
	bool first = true;
	for (const auto &b : cur.blocks) {
		if (first) {
			first = false;
		} else { out << '\n'; }
		switch (b.state) {
			@put(write block);
			default: ;
		}
		@put(write notes);
	}
@end(write cur)
```

```
@def(write block)
	case RS::header: {
		@put(write header);
		break;
	}
@end(write block)
```

```
@def(write header)
	for (const auto &n : b.value) {
		for (
			int i = 0; i < b.level; ++i
		) {
			out << '#';
		}
		out << ' ';
		out << n << '\n';
	}
@end(write header)
```

```
@add(write block)
	case RS::code: {
		@put(write code);
		break;
	}
@end(write block)
```

```
@def(write code)
	out << "```\n";
	for (const auto &n: b.value) {
		out << n << '\n';
	}
	out << "```\n";
@end(write code)
```

```
@add(write block)
	case RS::para: {
		@put(write para);
		break;
	}
@end(write block)
```

```
@def(needed by write_x)
	std::string split(
		std::string &s, int width
	) {
		auto b { s.begin() };
		auto e { s.end() };
		while (b != e && *b == ' ') { ++b; }
		auto c = b;
		while (c != e) {
			auto t = c;
			while (t != e && *t == ' ') { ++t; }
			while (t != e && *t != ' ') { ++t; }
			if (c == b || t - b <= width) {
				c = t;
			} else {
				break;
			}
		}
		std::string res { b, c };
		s.erase(s.begin(), c);
		return res;
	}
@end(needed by write_x)
```

```
@add(needed by write_x)
	void multi_write(
		std::ofstream &out,
		std::string str,
		std::string first_in,
		const std::string &other_in
	) {
		while (! str.empty()) {
			std::string p = split(str, 72 - first_in.size());
			out << first_in << p << '\n';
			first_in = other_in;
		}
	}
@end(needed by write_x)
```

```
@def(write para)
	bool first = true;
	for (const auto &n: b.value) {
		if (first) {
			first = false;
		} else { out << '\n'; }
		multi_write(out, n, {}, {});
	}
@end(write para)
```

```
@def(write notes)
	for (const auto &n: b.notes) {
		multi_write(out, n, "* ", "  ");
	}
@end(write notes)
```

```
@Add(run loop)
	if (cmd == "H" || cmd == "Html") {
		write_x();
		write_html();
		continue;
	}
@End(run loop)
```

```
@Add(run loop)
	if (cmd == "F" || cmd == "Files") {
		write_x();
		write_html();
		Inputs old { std::move(inputs) };
		try {
			read_sources();
			files_write();
		} catch (...) {
			std::cerr << "!! aborted\n";
			inputs = std::move(old);
		}
		curInput = inputs.begin();
		curBlock =
			curInput->blocks.begin();
		continue;
	}
@End(run loop)
```

```
@Add(run loop)
	if (cmd == "P" || cmd == "Process") {
		write_x();
		write_html();
		Inputs old { std::move(inputs) };
		try {
			read_sources();
			files_write();
			files_process();
		} catch (...) {
			std::cerr << "!! aborted\n";
			inputs = std::move(old);
		}
		curInput = inputs.begin();
		curBlock =
			curInput->blocks.begin();
		continue;
	}
@End(run loop)
```
