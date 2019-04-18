# Interactive Display of slides

```
@Add(global elements)
	bool interactive = false;
	std::vector<Input>::iterator curInput;
	std::vector<Block>::iterator curBlock;
@End(global elements)
```

```
@Add(global elements)
	bool write_files = true;
@End(global elements)
```

```
@Add(global elements)
	bool process_files = true;
@End(global elements)
```

```
@Add(global elements)
	bool html_files = true;
@End(global elements)
```

```
@Add(process argument)
	if (
		arg == "-i" ||
		arg == "--interactive"
	) {
		interactive = true;
		write_files = false;
		process_files = false;
		html_files = false;
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
	curBlock = curInput->blocks.begin();
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
@Add(global elements)
	void trim(std::string &s) {
		while (! s.empty() && (s[0] & 0xff) <= ' ') {
			s.erase(0, 1);
		}
	}
@End(global elements)
```

```
@Def(run loop)
	std::string cmd;
	draw_position();
	std::cout << "> ";
	std::getline(std::cin, cmd);
	trim(cmd);
	if (cmd.empty()) { continue; }
	@Put(do range);
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
		int next = (curBlock - curInput->blocks.begin()) + 1;
		while (next >= static_cast<int>(curInput->blocks.size())) {
			--next;
		}
		@Mul(do block range);
		curBlock = curInput->blocks.begin() + next;
		draw_block();
		continue;
	}
@End(run loop)
```

```
@Add(run loop)
	if (cmd == "p" || cmd == "prev") {
		int next = curBlock - curInput->blocks.begin();
		if (next > 0) {
			--next;
		}
		@Mul(do block range);
		curBlock = curInput->blocks.begin() + next;
		draw_block();
		continue;
	}
@End(run loop)
```

```
@def(draw block)
	if (curBlock->state == RS::header) {
		int i = 0;
		for (const auto &l : curBlock->value) {
			std::cout << ++i << ": ";
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
		int i = 0;
		for (const auto &l : curBlock->value) {
			std::cout << ++i << ": " << l << '\n';
		}
		std::cout << "```\n\n";
	}
@end(draw block)
```

```
@add(draw block)
	if (curBlock->state == RS::para) {
		int i = 0;
		for (const auto &l : curBlock->value) {
			std::cout << ++i << ": " << l << "\n\n";
		}
	}
@end(draw block)
```

```
@add(draw block)
	int j = 0;
	for (const auto &l : curBlock->notes) {
		std::cout << ++j << ": * " << l << '\n';
	}
	std::cout << '\n';
@end(draw block)
```

```
@def(draw position)
	auto &bs { curInput->blocks };
	std::cout << curInput->path() << ':';
	int idx =
		(curBlock - bs.begin()) + 1;
	std::cout << idx;
	if (
		idx == static_cast<int>(bs.size())
	) {
		std::cout << " = $";
	}
@end(draw position)
```

```
@Add(run loop)
	if (cmd == "f" || cmd == "forward") {
		int next =(curInput - inputs.begin()) + 1;
		while (next >= static_cast<int>(inputs.size())) {
			--next;
		}
		@Mul(do inputs range);
		curInput = inputs.begin() + next;
		curBlock = curInput->blocks.begin();
		draw_block();
		continue;
	}
@End(run loop)
```

```
@Add(run loop)
	if (cmd == "b" || cmd == "backward") {
		int next = curInput - inputs.begin();
		if (next) {
			--next;
		}
		@Mul(do inputs range);
		curInput = inputs.begin() + next;
		curBlock = curInput->blocks.begin();
		draw_block();
		continue;
	}
@End(run loop)
```