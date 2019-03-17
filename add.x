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
* The `@f(add_block)` function adds a new block after the current block
* It sets its value to a default text
* And makes the new block the current block

```
@Add(run loop)
	if (cmd == "H" || cmd == "Header") {
		add_block(RS::header);
		continue;
	}
@End(run loop)
```
* Generates a new header block

```
@Add(run loop)
	if (cmd == "C" || cmd == "Code") {
		add_block(RS::code);
		continue;
	}
@End(run loop)
```
* Generates a new code block

```
@Add(run loop)
	if (cmd == "P" || cmd == "Para") {
		add_block(RS::para);
		continue;
	}
@End(run loop)
```
* Generates a new paragraph blocks
* Multilpe paragraph blocks will be merged into one paragraph block on
  read

```
@def(add block)
	if (curInput != inputs.end()) {
		@put(increase block);
		int i = curBlock -
			curInput->blocks.begin();
		@put(insert block);
		curBlock =
			curInput->blocks.begin() + i;
	} else {
		std::cerr << "! no file\n";
	}
	draw_block();
@end(add block)
```
* `@f(add_block)` increases the current block if possible
* so it can insert the new block before the iterator
* As the insert can invalidate the iterators, it saves the distance
* and restores it afterwards

```
@def(increase block)
	if (
		curBlock !=
			curInput->blocks.end()
	) {
		++curBlock;
	}
@end(increase block)
```
* `@f(add_block)` does not advance beyond the end of the container

```
@def(insert block)
	curInput->blocks.insert(
		curBlock,
		{ state, { "REPLACE" }, {} }
	);
@end(insert block)
```
* `@f(add_block)` creates a new block with the requested `state`
* and a default text
