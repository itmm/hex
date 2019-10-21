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
		ASSERT (! _open.empty());
		return _open.back();
	}
@End(inputs elements)
```
* last opened open input file

```
@Add(inputs elements)
	auto begin() {
		return _used.begin();
	}
@End(inputs elements)
```
* begin iterator for used input files

```
@Add(inputs elements)
	auto end() {
		return _used.end();
	}
@End(inputs elements)
```
* end iterator for used input files

```
@Add(inputs elements)
	auto size() const {
		return _used.size();
	}
@End(inputs elements)
```
* number of used input files

```
@Add(inputs elements)
	void push(const std::string &path) {
		const Input *prev = _used.size() ? &_used.back(): nullptr;
		_used.push_back({ path, prev });
		_open.push_back({ path, prev });
	}
@End(inputs elements)
```
* open new input file
* is recorded as used input file

```
@Add(inputs elements)
	void add(const std::string &path) {
		_paths.push_back(path);
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
	for (const auto &j : _used) {
		if (j.path() == name) {
			return true;
		}
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
	int _line = 0;
@end(private open input els)
```
* current line number

```
@Add(open input elements)
	int line() const {
		return _line;
	}
@end(open input elements)
```
* getter for current line number

```
@Def(line read)
	++_line;
@End(line read)
```
* increase line number for each line

```
@Add(inputs elements)
	Frag *get_local(
		const std::string &name
	) {
		ASSERT(! _open.empty());
		Input &i = _open.back().input();
		Frag *got { find_frag(i.path(), name) };
		return got ?: &add_frag(i, name);
	}
@End(inputs elements)
```
* return a fragment in the current open input file
* if it is not present, it is created

```
@Add(inputs elements)
	Frag *find_global(
		const std::string &name
	) {
		@put(find global);
		return find_frag(name);
	}
@End(inputs elements)
```
* find a fragment in all but the current open input files or the `_root`
  collection

```
@def(find global)
	if (_open.size() > 1) {
		auto i = _open.end() - 2;
		Frag *f { find_frag(
			i->input(), name
		) };
		if (f) { return f; }
	}
@end(find global)
```
* walk through all open input files and return, if the fragment is found

```
@Add(inputs elements)
	Frag *add_global(
		const std::string &name
	) {
		return &add_frag(name);
	}
@End(inputs elements)
```
* add a fragment in the `_root` collection

```
@Add(inputs elements)
	Frag *get_global(
		const std::string &name
	) {
		Frag *result = find_global(name);
		if (! result) {
			result = add_global(name);
		}
		return result;
	}
@End(inputs elements)
```
* returns a global fragment
* if it is not found, the function creates a fragment in the `_root`
  collection

```
@Def(clear inputs)
	_used.clear();
	_open.clear();
	if (_paths.empty()) {
		@put(populate default file);
	}
	_current_path = _paths.begin();
@End(clear inputs)
```
* resets all open and used files
* if the `_path`s are empty the default input file names will be used

```
@def(populate default file)
	if (std::filesystem::exists(
		"index.md"
	)) {
		_paths.push_back("index.md");
	} else if (std::filesystem::exists(
		"index.x"
	)) {
		_paths.push_back("index.x");
	} else {
		std::cerr << "no input paths\n";
		_paths.push_back("index.md");
	}
@end(populate default file)
```
* sets a default file

