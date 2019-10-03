# Adding blocks in editor
* define commands to add a header, code block or paragraph block after
  the current slide
* when reading again `hx` will merge continuous paragraphs into one
  block

```
@Add(global elements)
	void add_block(Read_State state) {
		@put(add block);
	}
@End(global elements);
```
* the `@f(add_block)` function adds a new block after the current block
* it sets its value to a default text
* and makes the new block the current block

```
@Add(run loop)
	if (cmd == "h" || cmd == "header") {
		add_block(RS::header);
		continue;
	}
@End(run loop)
```
* generates a new header block

```
@Add(run loop)
	if (cmd == "c" || cmd == "code") {
		add_block(RS::code);
		continue;
	}
@End(run loop)
```
* generates a new code block

```
@Add(run loop)
	if (cmd == "o" || cmd == "other") {
		add_block(RS::para);
		continue;
	}
@End(run loop)
```
* generates a new paragraph blocks
* multiple paragraph blocks will be merged into one paragraph block on
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
* as the insert can invalidate the iterators, it saves the distance
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
		{ state, { "REPLACE" }, {}, state == RS::header ? 1 : 0 }
	);
@end(insert block)
```
* `@f(add_block)` creates a new block with the requested `state`
* and a default text

## Duplicating the current block
* duplicates the current block
* and sets the current block to the new copy

```
@Add(run loop)
	if (cmd == "d" || cmd == "dup") {
		if (curInput != inputs.end()) {
			if (curBlock !=
				curInput->blocks.end()
			) {
				@put(duplicate);
			}
		} else {
			std::cerr << "! no file\n";
		}
		continue;
	}
@End(run loop)
```
* duplicates the current block
* if the file is valid
* and the current block is valid

```
@def(duplicate)
	int i = curBlock -
		curInput->blocks.begin();
	curInput->blocks.insert(
		curBlock, *curBlock
	);
	curBlock = curInput->blocks.begin() +
		i + 1;
@end(duplicate)
```
* the insert invalidates the iterator
* so the offset is saved
* a copy of the current block is inserted before the current block
* that is the wrong way around but easier
* and doesn't matter in the end
* the new current block is set to the old current block
* which is now the second of the two duplicates
