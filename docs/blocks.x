# Separating input into blocks

```
@Def(input prereqs)
	@put(globals)
@End(input prereqs)
```

```
@def(globals)
	enum class Read_State {
		new_element
		@put(read states)
	};
	using @t(RS) = Read_State;
@end(globals)
```

```
@Add(open input elements)
	Read_State state = @t(RS)::new_element;
@End(open input elements)
```

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

```
@def(handle newlines)
	if (line.empty()) {
		state = RS::new_element;
		break;
	}
@end(handle newlines)
```

```
@def(read states),
	header
@end(read states)
```

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

```
@def(unknown line)
	std::cerr << "!! " << line << '\n';
@end(unknown line)
```

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

```
@Add(includes)
	#include <vector>
@End(includes)
```

```
@Add(input elements)
	std::vector<Block> blocks;
@End(input elements);
```

```
@def(line vars)
	auto &blocks =
		inputs.cur().input().blocks;
@end(line vars)
```

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

```
@add(read states),
	code,
	after_code
@end(read states)
```

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