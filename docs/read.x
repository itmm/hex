# Read the input files
* Processes the input files line by line

```
@Add(global elements)
	@put(globals);
@End(global elements)
```
* shorthand for global fragment
* groups all code from this `x`-file together

```
@def(globals)
	@Put(inputs prereqs);
	@put(inputs);
@end(globals)
```
* some elements are used by the `Inputs` and `Input` classes
* and must be defined before its definition
* these can be grouped in the fragment `@s(inputs prereqs)`

## The `Inputs` class
* this class manages multiple files
* some are open, because of include hierarchies
* some are waiting to be processed
* all is abstracted away, so that the interface provides lines until
  everything is processed

```
@def(inputs)
	class Inputs {
		public:
			@Put(inputs elements);
		private:
			@Put(private inputs elements);
	};
@end(inputs)
```
* contains all files that are processed
* `@k(@Put)` inserts a fragment from the global scope

```
@Def(includes)
	#include @s(<string>)
@End(includes)
```
* defines `std::string`
* `@k(@Add)` extends a globally defined fragment

```
@Def(inputs elements)
	void read_line(std::string &line);
@End(inputs elements)
```
* defines method to read a line

```
@add(inputs)
	void Inputs::read_line(
		std::string &line
	) {
		@put(inputs read line);
	}
@end(inputs)
```
* method is not defined inline in the class because it is used multiple
  times

```
@Def(open input prereqs)
	struct No_More_Lines {};
@End(open input prereqs)
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
* `inputs` enthält neben der gerade offenen Datei auch alle Dateien,
  die  noch prozessiert werden müssen
* Und alle bereits gelesenen Dateien

```
@add(globals)
	@Put(needed by read_sources);
	void read_sources() {
		@put(read sources);
	}
@end(globals)
```

```
@Def(read source files)
	read_sources();
@End(read source files)
```

```
@Add(inputs elements)
	void clear() {
		@Put(clear inputs);
	}
@End(inputs elements)
```

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
* `hx` liest die Eingabe-Dateien zeilenweise
* Inkludierungen werden transparent in `inputs` behandelt

## What is a file?
* C++ represent open files as `std::ifstream`
* Also a file is represented by its path
* `hx` also store more information on a file by file basis
* For example the local fragments
* So a special class `Input` will represent a file

## `Input` class
* Defines the `Input` class

```
@Add(open input prereqs)
	@Put(input prereqs);
@End(open input prereqs)
```

```
@Add(open input prereqs)
	class Input {
		public:
			@Put(input elements);
		private:
			std::string _path;
	};
@End(open input prereqs)
```
* A bunch of fragments make room for later extensions
* They are declared global, so they can be modified in different
  `x`-files

```
@Def(inputs prereqs)
	@Put(open input prereqs);
	class Open_Input {
		public:
			@Put(open input elements);
		private:
			@Put(private open input els);
	};
@End(inputs prereqs)
```

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
@Add(private inputs elements)
	std::vector<std::string> _paths;
	std::vector<std::string>::
		const_iterator _current_path;
@End(private inputs elements)
```

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
