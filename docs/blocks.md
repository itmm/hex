# Separating input into blocks
* the input is treated as a sequence of blocks.
* each block has a type, a value and optional notes.
* blocks are separated by empty lines.

```c++
@Def(input prereqs)
	@put(globals)
@End(input prereqs)
```
* this file uses a local shortcut for the global fragment

```c++
@def(globals)
	enum class Read_State {
		new_element
		@put(read states)
	};
@end(globals)
```
* the `Read_State` identifies the mode that the block parser is
  currently in
* the default `new_element` signals that a new block is starting

```c++
@add(globals)
	using @t(RS) = Read_State;
@end(globals)
```
* `RS` is a shortcut for the `Read_State`.

```c++
@Add(open input elements)
	Read_State state = @t(RS)::new_element;
@End(open input elements)
```
* each input element has a `Read_State`.
* the initial value is `new_element`.

```c++
@Def(process line)
	do {
		auto &state = inputs.cur().state;
		@put(line vars);
		@put(states with newlines);
		@put(handle newlines);
		@put(states without newlines);
		@put(unknown line);
	} while (false);
@End(process line)
```
* the `do`-loop is only a grouping construct
* the loop will not iterate
* but intermediate steps are allowed to `break` out of the loop

```c++
@def(handle newlines)
	if (line.empty()) {
		state = RS::new_element;
		break;
	}
@end(handle newlines)
```
* if the line is empty, start a new element

```c++
@def(read states),
	header
@end(read states)
```
* new state for parsing headers

```c++
@def(states without newlines)
	if (line[0] == '#' && (state == RS::new_element || state == RS::header)) {
		bool was_new { state == RS::new_element };
		state = RS::header;
		@put(got header line);
		break;
	}
@end(states without newlines)
```
* headers start if the previous line is empty
* and the current line starts with `#`

```c++
@def(unknown line)
	std::cerr << "!! " << line << '\n';
@end(unknown line)
```
* write error message for unrecognized line

```c++
@Add(includes)
	#include <vector>
@End(includes)
```
* `Block` has `std::vector` of strings

```c++
@add(globals)
	struct Block {
		Read_State state;
		std::vector<std::string> value;
		std::vector<std::string> notes;
		int level;
	};
@end(globals)
```
* `Block` contains values and notes

```c++
@Add(input elements)
	std::vector<Block> blocks;
@End(input elements);
```
* `Input` contains container of `Block`s

```c++
@def(line vars)
	auto &blocks =
		inputs.cur_input().blocks;
@end(line vars)
```
* get reference to `Block`s

```c++
@def(got header line)
	auto b { line.begin() };
	auto e { line.end() };
	int l { 0 };
	for (; b != e && *b == '#'; ++b, ++l) {}
	for (; b != e && *b == ' '; ++b) {}
	if (was_new || blocks.empty() || blocks.back().state != RS::header ||
		blocks.back().notes.size()
	) {
		blocks.push_back({ RS::header, {}, {}, l });
	}
	blocks.back().value.emplace_back(
		b, e
	);
@end(got header line)
```
* count `#`s
* skip spaces
* rest is header title

```c++
@add(read states),
	code,
	after_code
@end(read states)
```
* new states for parsing code blocks

```c++
@def(states with newlines)
	if (line.size() >= 3 && line.substr(0, 3) == "```" &&
		state == RS::new_element
	) {
		state = RS::code;
		@put(enter code block);
		break;
	}
@end(states with newlines)
```
* recognize start of code block

```c++
@add(states with newlines)
	if (state == RS::code) {
		if (line == "```") {
			state = RS::after_code;
		} else {
			@put(got code line);
		}
		break;
	}
@end(states with newlines)
```
* when in code block, distinguish between ending of code block
* and the processing of a code line

```c++
@def(enter code block)
	blocks.push_back({ RS::code, {}, {}, 0 });
@end(enter code block)
```
* add a new code block

```c++
@def(got code line)
	blocks.back().value.push_back(line);
@end(got code line)
```
* add the code line

```c++
@add(read states),
	notes
@end(read states)
```
* special state for handling states

```c++
@add(states without newlines)
	if (line[0] == '*') {
		if (
			state == RS::header ||
			state == RS::after_code ||
			state == RS::notes
			@put(can have notes)
		) {
			state = RS::notes;
			@put(got note);
			break;
		}
	}
@end(states without newlines)
```
* a note starts with a line that has a `*` as its first character
* the line must directly follow a header, a code section or be part of a
  note section

```c++
@add(states without newlines)
	if (line[0] == ' ' && state == RS::notes) {
		@put(add note);
		break;
	}
@end(states without newlines)
```
* longer notes can be continued over multiple lines
* the additional lines need to start with a space

```c++
@def(got note)
	auto b { line.begin() };
	auto e { line.end() };
	for (; b != e && (*b == '*' || *b == ' '); ++b) {}
	blocks.back().notes.emplace_back(b, e);
@end(got note)
```
* to process a note all `*`s and spaces at the beginning of the line are
  ignored
* the rest is added as a note

```c++
@def(add note)
	blocks.back().notes.back() += line;
@end(add note)
```
* if a note is continued over multiple lines, the additional lines are
  added to the last note

```c++
@add(read states),
	para
@end(read states)
```
* special state for non-slide paragraphs
* these can be used to describe functionality outside of simple slides

```c++
@add(states without newlines)
	@put(pre default states);
	if (line[0] != ' ') {
		if (state == RS::new_element || state == RS::para) {
			@put(create para);
			@put(add para);
			state = RS::para;
			break;
		}
	}
@end(states without newlines)
```
* paragraphs are any other line that does not start with a space

```c++
@def(create para)
	if (state == RS::new_element) {
		@put(create para block);
		blocks.back().value.push_back(line);
	}
@end(create para)
```
* needs to add a new block, if the last block is no paragraph block
* then add the line to the last block

```c++
@def(create para block)
	if (blocks.empty() || blocks.back().state != RS::para) {
		blocks.push_back({ RS::para, {}, {}, 0 });
	}
@end(create para block)
```
* if the last block is not a paragraph, then add one

```c++
@def(add para)
	if (state == RS::para) {
		blocks.back().value.back() += " " + line;
	}

@end(add para)
```
* if the current paragraph is extended, the line will be added to the
  last paragraph of the last block

```c++
@add(read states),
	img
@end(read states)
```
* special state for image slides

```c++
@def(can have notes) ||
	state == RS::img
@end(can have notes)
```

```c++
@def(pre default states)
	if (line[0] == '!') {
		if (state == RS::new_element || state == RS::img) {
			@put(create img);
			@put(add img);
			state = RS::img;
			break;
		}
	}
@end(pre default states)
```

```c++
@def(create img)
	if (state == RS::new_element) {
		@put(create img block);
	}
@end(create img)
```

```c++
@def(create img block)
	if (blocks.empty() || blocks.back().state != RS::img ||
		blocks.back().notes.size()
	) {
		blocks.push_back({ RS::img, {}, {}, 0 });
	}
@end(create img block)
```

```c++
@def(add img)
	if (line.size() < 3 || line[1] != '(' || line[line.size() - 1] != ')') {
		std::cerr << "wrong line " << line << "\n";
	}
	blocks.back().value.push_back(line.substr(2, line.size() - 3));
@end(add img)
```
