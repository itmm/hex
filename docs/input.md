# Input Files
* structure for handling input files

```
@Add(includes)
	#include <iostream>
	#include <vector>
	#include <filesystem>
@End(includes)
```
* needed includes

```
@Add(inputs elements)
	auto &cur() {
		ASSERT (! open_.empty());
		return open_.back();
	}
	auto &cur_input() {
		return used_.find(cur().path())->second;
	}
@End(inputs elements)
```
* last opened open input file

```
@Add(inputs elements)
	auto begin() {
		return used_.begin();
	}
@End(inputs elements)
```
* begin iterator for used input files

```
@Add(inputs elements)
	auto end() {
		return used_.end();
	}
@End(inputs elements)
```
* end iterator for used input files

```
@Add(inputs elements)
	auto size() const {
		return used_.size();
	}
@End(inputs elements)
```
* number of used input files

```
@Add(inputs elements)
	void push(const std::string &path) {
		std::string prev;
		if (open_.size()) {
			auto got { used_.find(open_.back().path()) };
			if (got != used_.end()) {
				prev = open_.back().path();
			}
		}
		used_.insert(std::move(std::map<std::string, Input>::value_type(path, Input(prev))));
		open_.emplace_back(path);
	}
	const std::string open_head() const {
		ASSERT(! open_.empty());
		return open_.back().path();
	}
	Input &operator[](
		const std::string &name
	) {
		return used_[name];
	}
@End(inputs elements)
```
* open new input file
* is recorded as used input file

```
@Add(inputs elements)
	void add(const std::string &path) {
		roots_.push_back(path);
		push(path);
	}
@End(inputs elements)
```
* add a file to be processed

# Get next line
* reads next line from the current input file
* if the end is reached, the current input file in popped
* and the line is read from the previous input file

```
@Add(inputs elements)
	bool has(
		const std::string &name
	) const {
		@put(has checks);
		return false;
	}
@End(inputs elements)
```
* checks if the file is already used

```
@def(has checks)
	if (used_.find(name) != used_.end()) {
		return true;
	}
@end(has checks)
```
* iterate over `_used` collection

## Local `Frags`
* each input file has a `Frag` collection

## Line Numbers
* for each open input file the current line number is recorded

```
@Add(private open input els)
	int line_ = 0;
@end(private open input els)
```
* current line number

```
@Add(open input elements)
	int line() const {
		return line_;
	}
@end(open input elements)
```
* getter for current line number

```
@Def(line read)
	++line_;
@End(line read)
```
* increase line number for each line

```
@Def(clear inputs)
	used_.clear();
	open_.clear();
	if (roots_.empty()) {
		@put(populate default file);
	}
	current_path_ = roots_.begin();
@End(clear inputs)
```
* resets all open and used files
* if the `_path`s are empty the default input file names will be used

```
@def(populate default file)
	if (std::filesystem::exists(
		"index.md"
	)) {
		roots_.push_back("index.md");
	} else if (std::filesystem::exists(
		"index.x"
	)) {
		roots_.push_back("index.x");
	} else {
		std::cerr << "no input paths\n";
		roots_.push_back("index.md");
	}
@end(populate default file)
```
* sets a default file

