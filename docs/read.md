# Read the input files
* processes the input files line by line

```
@Add(global elements)
	@put(globals);
@End(global elements)
```
* shorthand for global fragment
* groups all code from this input file together

## The `Inputs` class
* this class manages multiple files
* some are open, because of include hierarchies
* some are waiting to be processed
* all is abstracted away, so that the interface provides lines until
  everything is processed

```
@def(globals)
	@Put(inputs prereqs);
	class Inputs {
		public:
			@Put(inputs elements);
		private:
			@Put(private inputs elements);
	};
@end(globals)
```
* some elements are used by the `Inputs` and `Input` classes
* and must be defined before its definition
* these can be grouped in the fragment `@s(inputs prereqs)`
* `@k(@Put)` inserts a fragment from the global scope

```
@Def(includes)
	#include @s(<string>)
@End(includes)
```
* needs `std::string`

```
@Def(inputs elements)
	void read_line(std::string &line);
@End(inputs elements)
```
* defines method to read a line
* to avoid creating too much objects the storage is passed as argument

```
@add(globals)
	void Inputs::read_line(
		std::string &line
	) {
		@put(inputs read line);
	}
@end(globals)
```
* method is not defined inline in the class because it is used multiple
  times
* and it is somewhat big

```
@Def(inputs prereqs)
	struct No_More_Lines {};
@End(inputs prereqs)
```
* the code throws this exception, when no more lines are available

```
@def(inputs read line)
	throw No_More_Lines {};
@end(inputs read line)
```
* a trivial first implementation always throws the exception

## Reading everything

```
@add(globals)
	Inputs inputs;
@end(globals)
```
* `inputs` is a central point for managing a stack of open files
* if one file includes another file the old file is kept open
* to continue the processing after the processing of the included file
  finishes
* also all read files are kept

```
@add(globals)
	@Put(needed by read_sources);
	void read_sources() {
		@put(read sources);
	}
@end(globals)
```
* read all sources that are registered in the global `inputs` variable

```
@Def(read source files)
	read_sources();
@End(read source files)
```
* the `@f(main)` calls this function to read the sources
* in the interactive environment this function may also be called

```
@Add(inputs elements)
	void clear() {
		@Put(clear inputs);
	}
@End(inputs elements)
```
* remove all state from the `inputs` variable
* all that is kept is the list of initial source files

```
@def(read sources) {
	inputs.clear();
	clear_frags();
	@Put(additional read vars);
	std::string line;
	try { for (;;) {
		inputs.read_line(line);
		@Put(process line);
	} }
	catch (const No_More_Lines &) {}
	eval_metas();
} @end(read sources)
```
* first this function resets `inputs` to a clean state
* then it processes each input file and all included files line by line
* all lines are processed in the order they occur
* end of input is signaled with an exception

## What is a file?
* C++ represent open files as `std::ifstream`
* also a file is represented by its path
* `hx` also store more information on a file by file basis
* for example the local fragments
* so a special class `Input` will represent a file

## `Input` class
* defines the `Input` class

```
@Add(inputs prereqs)
	@Put(open input prereqs);
	class Open_Input {
		public:
			@Put(open input elements);
		private:
			@Put(private open input els);
	};
@End(inputs prereqs)
```
* the `Open_Input` class represents an `Input` file that is also
  currently open for reading

```
@Def(open input prereqs)
	@Put(input prereqs);
	class Input {
		public:
			@Put(input elements);
			const std::string prev;
	};
@End(open input prereqs)
```
* an input file has the list of all blocks associated with it that were
  defined in them
* but these are defined later
* for just now the path of the file is stored

```
@Add(includes)
	#include @s(<fstream>)
@End(includes)
```
* needs `std::ifstream`

```
@Def(private open input els)
	std::string path_;
	std::ifstream file_;
@End(private open input els)
```
* open file contains a file
* and an input stream

```
@Def(open input elements)
	Open_Input(const std::string &path):
		path_ { path },
		file_ { path.c_str() }
	{}
@End(open input elements)
```
* open file on construction

```
@Add(open input elements)
	const std::string &path() const {
		return path_; 
	}
@End(open input elements)
```

```
@Add(open input elements)
	Open_Input(
		const Open_Input &
	) = delete;
	Open_Input(
		Open_Input &&
	) = default;
@End(open input elements)
```
* no copy construction
* no move construction

```
@Add(open input elements)
	Open_Input &operator=(
		const Open_Input &
	) = delete;
	Open_Input &operator=(
		Open_Input &&
	) = default;
@End(open input elements)
```
* no copy assignment
* no move assignment

```
@Def(input elements)
	Input(const std::string &prev = {}):
		prev { prev }
	{}
@End(input elements)
```
* in the constructor the `name` will be saved
* additional elements can be initialized later

```
@Add(input elements)
	Input(const Input &) = delete;
	Input(Input &&) = default;
@End(input elements)
```
* no copy constructor
* no move constructor

```
@Add(input elements)
	Input &operator=(
		const Input &
	) = delete;
	Input &operator=(
		Input &&
	) = default;
@End(input elements)
```
* no copy assignment
* no move assignment

```
@Add(open input elements)
	void read_line(std::string &line) {
		if (file_.is_open()) {
			@put(get line);
		}
		throw No_More_Lines {};
	}
@end(open input elements)
```
* if file is open, return next line from this file
* if that fails or file was closed, then throw an exception

```
@def(get line)
	if (std::getline(file_, line)) {
		@Put(line read);
		return;
	}
	file_.close();
@end(get line)
```
* if could read line, return
* otherwise close file
* and fall through to the exception throwing code

```
@Def(private inputs elements)
	std::vector<std::string> roots_;
	std::vector<std::string>::
		const_iterator current_path_;
@End(private inputs elements)
```
* has a list of input files
* and an iterator for the current path

```
@Add(private inputs elements)
	std::vector<Open_Input> open_;
	std::map<std::string, Input> used_;
@End(private inputs elements)
```
* `_open` contains all files open for reading
* the last file in this collection is the current input file
* more files are pushed on it with include commands
* `_used` contains all the files that were opened
* this is used to avoid including files multiple times

```
@rep(inputs read line)
	for (;;) {
		@put(push next path);
		try {
			open_.back().read_line(line);
			return;
		}
		catch (const No_More_Lines &) {}
		@put(save open input);
		open_.pop_back();
	}
	throw No_More_Lines {};
@end(inputs read line)
```
* reads a line from the current file
* when the end is reached, the file is popped and the line is read from
  the previous file

```
@def(push next path)
	if (open_.empty()) {
		if (
			current_path_ != roots_.end()
		) {
			push(*current_path_++);
		} else {
			break;
		}
	}
@end(push next path)
```
* if no files are available, try to open a new one from the paht list

```
@def(save open input)
	auto &f { used_.find(open_.back().path())->second };
	if (f.blocks.empty()) {
		f.blocks.push_back({
			RS::header,
			{ "EMPTY FILE" }, {}
		});
	}
@end(save open input)
```
* if the file was empty, one dummy block is added

