# Interactive Display of slides
* display slides in a terminal editor

```c++
@Add(global elements)
	bool interactive = false;
@End(global elements)
```
* is `hx` running in interactive mode

```c++
@Add(global elements)
	std::map<std::string, Input>::iterator curInput;
	std::vector<Block>::iterator curBlock;
@End(global elements)
```
* iterator to the current block

```c++
@Add(global elements)
	bool write_files = true;
@End(global elements)
```
* should source files be automatically written

```c++
@Add(global elements)
	bool process_files = true;
@End(global elements)
```
* should files be automatically processed

```c++
@Add(global elements)
	bool html_files = true;
@End(global elements)
```
* should HTML slides be generated

```c++
@Add(process argument)
	if (arg == "-i" || arg == "--interactive") {
		interactive = true;
		write_files = false;
		process_files = false;
		html_files = false;
		continue;
	}
@End(process argument)
```
* interactive mode turns off automatic generation

```c++
@Add(main body)
	if (interactive) {
		@Put(interactive);
	}
@End(main body)
```
* run interactive editor

```c++
@Def(interactive)
	curInput = inputs.begin();
	std::cerr << "curInput == " << curInput->first << "; is end == " <<
	 	(curInput == inputs.end()) << "\n";
	curBlock = curInput->second.blocks.begin();
	std::cerr << "curBlock == " << &*curBlock << "; is end == " <<
	 	(curBlock == curInput->second.blocks.end()) << "\n";
@End(interactive)
```
* initialize on first block in first input file

```c++
@Add(global elements)
	void draw_block() {
		@put(draw block);
	}
@End(global elements)
```
* draw current block

```c++
@Add(interactive)
	draw_block();
	for (;;) {
		@Put(run loop);
	}
@End(interactive)
```
* run interactive loop

```c++
@Add(global elements)
	void draw_position() {
		@put(draw position);
	}
@End(global elements)
```
* draw current position

```c++
@Add(global elements)
	void trim(std::string &s) {
		while (! s.empty() && (s[0] & 0xff) <= ' ') {
			s.erase(0, 1);
		}
	}
@End(global elements)
```
* remove non-printable characters at the beginning of a string

```c++
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
* draw prompt
* read command
* process command

```c++
@Add(run loop)
	if (cmd == "q" || cmd == "quit") {
		break;
	}
@End(run loop)
```
* quit interactive mode

```c++
@Add(run loop)
	if (cmd == "n" || cmd == "next") {
		@put(do next cmd);
		draw_block();
		continue;
	}
@End(run loop)
```
* go to next block

```c++
@def(do next cmd)
	int next = (curBlock - curInput->second.blocks.begin()) + 1;
	while (next >= static_cast<int>(curInput->second.blocks.size())) {
		--next;
	}
	@Mul(do block range);
	curBlock = curInput->second.blocks.begin() + next;
@end(do next cmd)
```
* go to next block

```c++
@Add(run loop)
	if (cmd == "p" || cmd == "prev") {
		@put(do prev cmd);
		draw_block();
		continue;
	}
@End(run loop)
```
* go to previous block

```c++
@def(do prev cmd)
	int next =curBlock - curInput->second.blocks.begin();
	if (next > 0) {
		--next;
	}
	@Mul(do block range);
	curBlock = curInput->second.blocks.begin() + next;
@end(do prev cmd)
```
* go to previous block

```c++
@def(draw block)
	if (curBlock->state == RS::header) {
		@put(draw header);
	}
@end(draw block)
```
* draw header block

```c++
@def(draw header)
	int i = 0;
	for (const auto &l : curBlock->value) {
		std::cout << ++i << ": ";
		@put(draw header level);
		std::cout << ' ' << l << "\n\n";
	}
@end(draw header)
```
* draw header block

```c++
@def(draw header level)
	for (int i = 0; i < curBlock->level; ++i) {
		std::cout << '#';
	}
@end(draw header level)
```
* draw header indent in Markdown syntax

```c++
@add(draw block)
	if (curBlock->state == RS::code) {
		@put(draw code);
	}
@end(draw block)
```
* draw code block

```c++
@def(draw code)
	std::cout << "```\n";
	int i = 0;
	for (const auto &l : curBlock->value) {
		std::cout << ++i << ": " << l << '\n';
	}
	std::cout << "```\n\n";
@end(draw code)
```
* draw code block

```c++
@add(draw block)
	if (curBlock->state == RS::para) {
		@put(draw para);
	}
@end(draw block)
```
* draw paragraph block

```c++
@def(draw para)
	int i = 0;
	for (const auto &l : curBlock->value) {
		std::cout << ++i << ": " << l << "\n\n";
	}
@end(draw para)
```
* draw paragraph block

```c++
@add(draw block)
	int j = 0;
	for (const auto &l : curBlock->notes) {
		std::cout << ++j << ": * " << l << '\n';
	}
	std::cout << '\n';
@end(draw block)
```
* draw notes

```c++
@def(draw position)
	auto &bs { curInput->second.blocks };
	std::cout << curInput->first << ':';
	int idx = (curBlock - bs.begin()) + 1;
	std::cout << idx;
	if (idx == static_cast<int>(bs.size())) {
		std::cout << " = $";
	}
@end(draw position)
```
* draw current input file number
* draw current input file name
* draw current block number

```c++
@Add(run loop)
	if (cmd == "f" || cmd == "forward") {
		@put(do forward cmd);
		draw_block();
		continue;
	}
@End(run loop)
```
* go to next input file

```c++
@def(do forward cmd)
	int next = 1;
	for (const auto &xx : inputs) {
		if (xx.first == curInput->first) { break; }
		++next;
	}
	while (next >= static_cast<int>(inputs.size())) {
		--next;
	}
	@Mul(do inputs range);
	curInput = inputs.begin();
	for (; next > 0 && curInput != inputs.end(); --next) {
		++curInput;
	}
	curBlock = curInput->second.blocks.begin();
@end(do forward cmd)
```
* go to next input file

```c++
@Add(run loop)
	if (cmd == "b" || cmd == "backward") {
		@put(do backward cmd);
		draw_block();
		continue;
	}
@End(run loop)
```
* go to previous input file

```c++
@def(do backward cmd)
	int next = 0;
	for (const auto &xx : inputs) {
		if (xx.first == curInput->first) { break; }
		++next;
	}
	if (next) {
		--next;
	}
	@Mul(do inputs range);
	curInput = inputs.begin();
	for (; next > 0 && curInput != inputs.end(); --next) {
		++curInput;
	}
	curBlock = curInput->second.blocks.begin();
@end(do backward cmd)
```
* go to previous input file
