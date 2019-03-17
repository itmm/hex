# Adding blocks in editor
* Define commands to add a header, code block or paragraph block after
  the current slide
* When reading again `hx` will merge continuous paragraphs into one
  block

```
@Add(global elements)
	void add_block(Read_State state) {
		@put(add block);
	}
@End(global elements);
```

```
@Add(run loop)
	if (cmd == "H" || cmd == "Header") {
		add_block(RS::header);
		continue;
	}
@End(run loop)
```

```
@Add(run loop)
	if (cmd == "C" || cmd == "Code") {
		add_block(RS::code);
		continue;
	}
@End(run loop)
```

```
@Add(run loop)
	if (cmd == "P" || cmd == "Para") {
		add_block(RS::para);
		continue;
	}
@End(run loop)
```

```
@def(add block)
	if (curInput != inputs.end()) {
		if (curBlock != curInput->blocks.end()) {
			++curBlock;
		}
		int i = curBlock - curInput->blocks.begin();
		curInput->blocks.insert(curBlock, { state, { "REPLACE" }, {} });
		curBlock = curInput->blocks.begin() + i;
	} else {
		std::cerr << "! no file\n";
	}
	draw_block();
@end(add block)
```
