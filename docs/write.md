# Write all input files
* Replace all read input files with the current content

```c++
@Add(global elements)
	@put(needed by write_input)
	void write_input() {
		for (const auto &cur : inputs) {
			@put(write cur);
		}
	}
@End(global elements)
```
* write all input files

```c++
@Add(run loop)
	if (cmd == "W" || cmd == "Write") {
		write_input();
		continue;
	}
@End(run loop)
```
* write all input files

```c++
@def(write cur)
	std::ofstream out { cur.first.c_str() };
@end(write cur)
```
* open output file

```c++
@add(write cur)
	bool first = true;
	for (const auto &b : cur.second.blocks) {
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
* write each block

```c++
@def(write block)
	case RS::header: {
		@put(write header);
		break;
	}
@end(write block)
```
* write header

```c++
@def(write header)
	for (const auto &n : b.value) {
		for (int i = 0; i < b.level; ++i) {
			out << '#';
		}
		out << ' ';
		out << n << '\n';
	}
@end(write header)
```
* write header Markdown

```c++
@add(write block)
	case RS::code: {
		@put(write code);
		break;
	}
@end(write block)
```
* write code

```c++
@def(write code)
	out << "```\n";
	for (const auto &n: b.value) {
		out << n << '\n';
	}
	out << "```\n";
@end(write code)
```
* write code Markdown

```c++
@add(write block)
	case RS::para: {
		@put(write para);
		break;
	}
@end(write block)
```
* write paragraph

```c++
@def(needed by write_input)
	std::string split(std::string &s, int width) {
		auto b { s.begin() };
		auto e { s.end() };
		@put(split eat spaces);
		auto c = b;
		@put(fill line);
		std::string res { b, c };
		s.erase(s.begin(), c);
		return res;
	}
@end(needed by write_input)
```
* split strings at word boundaries to a specified width

```c++
@def(split eat spaces)
	while (b != e && *b == ' ') {
		++b;
	}
@end(split eat spaces)
```
* skip starting spaces

```c++
@def(fill line)
	while (c != e) {
		auto t = c;
		@put(next word);
		if (c == b || t - b <= width) {
			c = t;
		} else {
			break;
		}
	}
@end(fill line)
```
* step over words until the line is too long

```c++
@def(next word)
	while (t != e && *t == ' ') {
		++t;
	}
	while (t != e && *t != ' ') {
		++t;
	}
@end(next word)
```
* move to next word
* and move over it

```c++
@add(needed by write_input)
	void multi_write(std::ofstream &out, std::string str,
		std::string first_in, const std::string &other_in
	) {
		@put(multi write);
	}
@end(needed by write_input)
```
* writes a long string with different prefixes

```c++
@def(multi write)
	while (! str.empty()) {
		std::string p = split(str, 72 - first_in.size());
		out << first_in << p << '\n';
		first_in = other_in;
	}
@end(multi write)
```
* split lines and write them line by line

```c++
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
* paragraphs are split without any prefixes

```c++
@def(write notes)
	for (const auto &n: b.notes) {
		multi_write(out, n, "* ", "  ");
	}
@end(write notes)
```
* notes are split

```c++
@Add(run loop)
	if (cmd == "H" || cmd == "Html") {
		write_input();
		write_html();
		continue;
	}
@End(run loop)
```
* write input files
* and generate HTML files

```c++
@Add(run loop)
	if (cmd == "F" || cmd == "Files") {
		write_input();
		write_html();
		Inputs old { std::move(inputs) };
		@put(write files);
		curInput = inputs.begin();
		curBlock = curInput->second.blocks.begin();
		continue;
	}
@End(run loop)
```
* write input files
* and generate HTML files
* and generate source files

```c++
@def(write files)
	try {
		read_sources();
		files_write();
	} catch (...) {
		std::cerr << "!! aborted\n";
		inputs = std::move(old);
	}
@end(write files)
```
* generate source files

```c++
@Add(run loop)
	if (cmd == "P" || cmd == "Process") {
		write_input();
		write_html();
		Inputs old { std::move(inputs) };
		@put(process files);
		curInput = inputs.begin();
		curBlock = curInput->second.blocks.begin();
		continue;
	}
@End(run loop)
```
* write input files
* and generate HTML files
* and generate source files
* and process files

```c++
@def(process files)
	try {
		read_sources();
		files_write();
		files_process();
	} catch (...) {
		std::cerr << "!! aborted\n";
		inputs = std::move(old);
	}
@end(process files)
```
* generate source files
* and process files

```c++
@Add(run loop)
	if (cmd == "M" || cmd == "Make") {
		write_input();
		int rc { system("make") };
		if (rc) { std::cerr << "failed\n"; }
		continue;
	}
@End(run loop)
```
* write input files
* and invoke `make`

```c++
@Add(global elements)
	bool is_prefix(const std::string &s, const std::string &p) {
		return s.size() >= p.size() &&
			std::equal(p.begin(), p.end(), s.begin());
	}
@End(global elements)
```
* check is one string is a prefix of another string

```c++
@Add(run loop) {
	static const std::string p { "M " };
	if (is_prefix(cmd, p)) {
		write_input();
		int rc {system(("make " + cmd.substr(p.size())).c_str()) };
		if (rc) { std::cerr << "failed\n"; }
		continue;
	}
} @End(run loop)
```
* write input files
* and run make with a specific target

```c++
@Add(run loop) {
	static const std::string p {
		"Make "
	};
	if (is_prefix(cmd, p)) {
		write_input();
		int rc { system(("make " + cmd.substr(p.size())).c_str()) };
		if (rc) { std::cerr << "failed\n"; }
		continue;
	}
} @End(run loop)
```
* write input files
* and run make with a specific target

```c++
@Add(run loop)
	if (cmd == "G" || cmd == "Git") {
		write_input();
		int rc = system("git status");
		if (rc) { std::cerr << "failed\n"; }
		continue;
	}
@End(run loop)
```
* write input files
* and run `git status`

```c++
@Add(run loop) {
	static const std::string p { "G " };
	if (is_prefix(cmd, p)) {
		write_input();
		int rc { system(("git " + cmd.substr(p.size())).c_str()) };
		if (rc) { std::cerr << "failed\n"; }
		continue;
	}
} @End(run loop)
```
* write input files
* and run `git` with arguments

```c++
@Add(run loop) {
	static const std::string p { "Git " };
	if (is_prefix(cmd, p)) {
		write_input();
		int rc { system(("git " + cmd.substr(p.size())).c_str()) };
		if (rc) { std::cerr << "failed\n"; }
		continue;
	}
} @End(run loop)
```
* write input files
* and run `git` with arguments
