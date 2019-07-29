# Read the input files
* Processes the input files line by line

```
@Add(global elements)
	@put(globals);
@End(global elements)
```
* shorthand for global fragment
* groups all code from this `x`-file together

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
	@Put(additional read vars);
	std::string line;
	try { for (;;) {
		inputs.read_line(line);
		@Put(process line);
	} }
	catch (const No_More_Lines &) {}
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
		private:
			std::string _path;
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
* Defines `std::ifstream`

```
@Def(private open input els)
	Input _input;
	std::ifstream _file;
	char _last;
@End(private open input els)
```

```
@Def(open input elements)
	Open_Input(const std::string &path):
		_input { path },
		_file { path.c_str() }
	{}
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

```
@Add(open input elements)
	Input &input() { return _input; }
@End(open input elements)
```

```
@Add(open input elements)
	const Input &input() const {
		return _input;
	}
@End(open input elements)
```

```
@Def(input elements)
	Input(const std::string &path):
		_path { path }
	{}
@End(input elements)
```
* In the constructor the `name` will be saved
* and the file with that path will be opened
* Additional elements can be initialized later

```
@Add(input elements)
	Input(
		const Input &
	) = delete;
	Input(
		Input &&
	) = default;
@End(input elements)
```

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
* `Input` instances can only be moved, not copied

```
@Add(input elements)
	const std::string &path() const {
		return _path;
	}
@End(input elements)
```
* Simple accessor

```
@Add(open input elements)
	void read_line(std::string &line) {
		if (_file.is_open()) {
			@put(get line);
		}
		throw No_More_Lines {};
	}
@end(open input elements)
```
* Liest Zeile aus der offenen Datei

```
@def(get line)
	if (std::getline(_file, line)) {
		@Put(line read);
		return;
	}
	_file.close();
@end(get line)
```
* Wenn Zeile gelesen wurde, passt die Funktion weitere Parameter an
* die erst später definiert werden

```
@Def(private inputs elements)
	std::vector<std::string> _paths;
	std::vector<std::string>::
		const_iterator _current_path;
@End(private inputs elements)
```

```
@Add(private inputs elements)
	std::vector<Open_Input> _open;
	std::vector<Input> _used;
@End(private inputs elements)
```
* Es gibt immer eine aktuelle Datei, die gerade gelesen wird
* Mitten während des Lesens können andere Dateien eingelesen
  (inkludiert) werden
* Daher gibt es einen Stapel offener Dateien
* Aus der letzten wird aktuell gelesen
* Eine Liste aller gelesenen Dateien wird in `used` verwaltet
* Damit wird verhindert, dass eine Datei mehrfach gelesen wird
* Auch signalisiert es der HTML-Ausgabe, welche Dateien generiert
  werden müssen

```
@rep(inputs read line)
	for (;;) {
		if (_open.empty()) {
			if (_current_path != _paths.end()) {
				push(*_current_path++);
			} else {
				break;
			}
		}
		try {
			_open.back().read_line(line);
			return;
		}
		catch (const No_More_Lines &) {}
		@put(save open input);
		_open.pop_back();
	}
	throw No_More_Lines {};
@end(inputs read line)
```
* Probiert aus aktueller Datei eine Zeile zu lesen
* Wandert bei Misserfolg durch andere offenen Dateien

```
@def(save open input)
	auto &f { _open.back().input() };
	@put(assure not empty);
	for (auto &i : _used) {
		if (i.path() == f.path()) {
			i = std::move(f);
			break;
		}
	}
@end(save open input)
```

```
@def(assure not empty)
	if (f.blocks.empty()) {
		f.blocks.push_back({
			RS::header,
			{ "EMPTY FILE" }, {}
		});
	}
@end(assure not empty)
```
