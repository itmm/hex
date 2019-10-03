# Separating input into blocks
* the input is treated as a sequence of blocks.
* each block has a type, a value and optional notes.
* blocks are separated by empty lines.

```
@Def(input prereqs)
	@put(globals)
@End(input prereqs)
```
* this file uses a local shortcut for the global fragment

```
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

```
@add(globals)
	using @t(RS) = Read_State;
@end(globals)
```
* `RS` is a shortcut for the `Read_State`.

```
@Add(open input elements)
	Read_State state = @t(RS)::new_element;
@End(open input elements)
```
* each input element has a `Read_State`.
* the initial value is `new_element`.

```
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

```
@def(handle newlines)
	if (line.empty()) {
		state = RS::new_element;
		break;
	}
@end(handle newlines)
```
* if the line is empty, start a new element

```
@def(read states),
	header
@end(read states)
```
* new state for parsing headers

```
@def(states without newlines)
	if (
		line[0] == '#' &&
		state == RS::new_element
	) {
		state = RS::header;
		@put(got header line);
		break;
	}
@end(states without newlines)
```
* headers start if the previous line is empty
* and the current line starts with `#`

```
@def(unknown line)
	std::cerr << "!! " << line << '\n';
@end(unknown line)
```
* write error message for unrecognized line

```
@Add(includes)
	#include <vector>
@End(includes)
```
* `Block` has `std::vector` of strings

```
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

```
@Add(input elements)
	std::vector<Block> blocks;
@End(input elements);
```
* `Input` contains container of `Block`s

```
@def(line vars)
	auto &blocks =
		inputs.cur().input().blocks;
@end(line vars)
```
* get reference to `Block`s

```
@def(got header line)
	auto b { line.begin() };
	auto e { line.end() };
	int l { 0 };
	for (
		; b != e && *b == '#'; ++b, ++l
	) {}
	for (; b != e && *b == ' '; ++b) {}
	blocks.push_back({
		RS::header, {{ b, e }}, {}, l
	});
@end(got header line)
```
* count `#`s
* skip spaces
* rest is header title

```
@add(read states),
	code,
	after_code
@end(read states)
```
* new states for parsing code blocks

```
@def(states with newlines)
	if (
		line == "```" &&
		state == RS::new_element
	) {
		state = RS::code;
		@put(enter code block);
		break;
	}
@end(states with newlines)
```

```
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

```
@def(enter code block)
	blocks.push_back({
		RS::code, {}, {}, 0
	});
@end(enter code block)
```

```
@def(got code line)
	blocks.back().value.push_back(
		line
	);
@end(got code line)
```

```
@add(read states),
	notes
@end(read states)
```

```
@add(states without newlines)
	if (line[0] == '*') {
		if (
			state == RS::header ||
			state == RS::after_code ||
			state == RS::notes
		) {
			state = RS::notes;
			@put(got note);
			break;
		}
	}
@end(states without newlines)
```

```
@add(states without newlines)
	if (
		line[0] == ' ' &&
		state == RS::notes
	) {
		@put(add note);
		break;
	}
@end(states without newlines)
```

```
@def(got note)
	auto b { line.begin() };
	auto e { line.end() };
	for (;
		b != e &&
			(*b == '*' || *b == ' ');
		++b
	) {}
	blocks.back().notes.push_back(
		{ b, e }
	);
@end(got note)
```

```
@def(add note)
	blocks.back().notes.back() +=
		line;
@end(add note)
```

```
@add(read states),
	para
@end(read states)
```

```
@add(states without newlines)
	if (line[0] != ' ') {
		if (
			state == RS::new_element ||
			state == RS::para
		) {
			@put(create para);
			@put(add para);
			state = RS::para;
			break;
		}
	}
@end(states without newlines)
```

```
@def(create para)
	if (state == RS::new_element) {
		if (blocks.empty() || blocks.back().state != RS::para) {
			blocks.push_back({
				RS::para, {}, {}, 0
			});
		}
		blocks.back().value.push_back(line);
	}
@end(create para)
```

```
@def(add para)
	if (state == RS::para) {
		blocks.back().value.back() +=
			" " + line;
	}
@end(add para)
```
