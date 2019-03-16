# Interactive Display of slides

```
@Add(global elements)
	bool interactive = false;
	std::vector<Input>::iterator curInput;
	std::vector<Block>::iterator curBlock;
@End(global elements)
```

```
@Add(process argument)
	if (
		arg == "-i" ||
		arg == "--interactive"
	) {
		interactive = true;
		continue;
	}
@End(process argument)
```

```
@Add(main body)
	if (interactive) {
		@Put(interactive);
	}
@End(main body)
```

```
@Def(interactive)
	curInput = inputs.begin();
	curBlock = curInput != inputs.end() ?
		curInput->blocks.begin() :
		std::vector<Block>::iterator {};
@End(interactive)
```

```
@Add(global elements)
	void draw_block() {
		@put(draw block);
	}
@End(global elements)
```

```
@Add(interactive)
	draw_block();
	for (;;) {
		@Put(run loop);
	}
@End(interactive)
```

```
@Add(global elements)
	void draw_position() {
		@put(draw position);
	}
@End(global elements)
```

```
@Def(run loop)
	std::string cmd;
	draw_position();
	std::cout << "> ";
	std::cin >> cmd;
@End(run loop)
```

```
@Add(run loop)
	if (cmd == "q" || cmd == "quit") {
		break;
	}
@End(run loop)
```

```
@Add(run loop)
	if (cmd == "n" || cmd == "next") {
		if (curInput != inputs.end()) {
			if (curBlock != curInput->blocks.end()) {
				++curBlock;
				draw_block();
				continue;
			}
		}
		std::cerr << "! end\n";
	}
@End(run loop)
```

```
@Add(run loop)
	if (cmd == "p" || cmd == "prev") {
		if (curInput != inputs.end()) {
			if (curBlock != curInput->blocks.begin()) {
				--curBlock;
				draw_block();
				continue;
			}
		}
		std::cerr << "! start\n";
	}
@End(run loop)
```

```
@def(draw block)
	if (curInput == inputs.end()) {
		std::cerr << "! no file\n";
		return;
	}
@end(draw block)
```

```
@add(draw block)
	if (curBlock == curInput->blocks.end()) {
		std::cerr << "! end\n";
		return;
	}
@end(draw block)
```

```
@add(draw block)
	if (curBlock->state == RS::header) {
		for (const auto &l : curBlock->value) {
			for (int i = 0; i < curBlock->level; ++i) {
				std::cout << '#';
			}
			std::cout << ' ' << l << "\n\n";
		}
	}
@end(draw block)
```

```
@add(draw block)
	if (curBlock->state == RS::code) {
		std::cout << "```\n";
		for (const auto &l : curBlock->value) {
			std::cout << l << '\n';
		}
		std::cout << "```\n\n";
	}
@end(draw block)
```

```
@add(draw block)
	if (curBlock->state == RS::para) {
		for (const auto &l : curBlock->value) {
			std::cout << l << "\n\n";
		}
	}
@end(draw block)
```

```
@add(draw block)
	for (const auto &l : curBlock->notes) {
		std::cout << "* " << l << '\n';
	}
	std::cout << '\n';
@end(draw block)
```

```
@def(draw position)
	if (curInput == inputs.end()) {
		std::cout << "no file:end\n";
	}
@end(draw position)
```

```
@add(draw position)
	std::cout << curInput->path() << ':';
	if (curBlock == curInput->blocks.end()) {
		std::cout << "end";
	} else {
		std::cout << (curBlock - curInput->blocks.begin() + 1);
	}
@end(draw position)
```

```
@Add(run loop)
	if (cmd == "f" || cmd == "forward") {
		if (curInput != inputs.end()) {
			++curInput;
			curBlock = curInput != inputs.end() ?
				curInput->blocks.begin() :
				std::vector<Block>::iterator {};
			draw_block();
			continue;
		}
		std::cerr << "! end\n";
	}
@End(run loop)
```

```
@Add(run loop)
	if (cmd == "b" || cmd == "backward") {
		if (curInput != inputs.begin()) {
			--curInput;
			curBlock = curInput != inputs.end() ?
				curInput->blocks.begin() :
				std::vector<Block>::iterator {};
			draw_block();
			continue;
		}
		std::cerr << "! start\n";
	}
@End(run loop)
```

