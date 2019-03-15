# Read the input files
* Processes the input files line by line

```
@Add(global elements)
	@put(globals);
@End(global elements)
```

```
@def(globals)
	@put(inputs prereqs);
	@put(inputs);
@end(globals)
```
* Some elements are used by the `Inputs` and `Input` classes
* and must be defined before its definition
* These can be grouped in the fragment `@s(inputs prereqs)`

## `Inputs` class
* This class manages multiple files
* Some are open, because of include hierarchies
* Some are waiting to be processed
* All is abstracted away, so that the interface provides lines until
  everything is processed

```
@def(inputs)
	class Inputs {
		public:
			@Put(inputs elements);
		private:
			@put(private inputs elements);
	};
@end(inputs)
```
* Enthält alle verarbeiteten Dateien
* `@k(@Put)` inserts a fragment from the global scope

```
@Def(includes)
	#include @s(<string>)
@End(includes)
```
* Defines `std::string`
* `@k(@Add)` extends a globally defined fragment

```
@Def(inputs elements)
	void read_line(std::string &l);
@End(inputs elements)
```

```
@add(inputs)
	void Inputs::read_line(
		std::string &line
	) {
		@put(inputs read line);
	}
@end(inputs)
```

```
@def(open input prereqs)
	struct no_more_lines {};
@end(open input prereqs)
```

```
@def(inputs read line)
	throw no_more_lines {};
@end(inputs read line)
```

## Reading everything

```
@add(globals)
	Inputs inputs;
@end(globals)
```
* `inputs` enthält neben der gerade offenen Datei auch alle Dateien, die
  noch prozessiert werden müssen
* Und alle bereits gelesenen Dateien

```
@Def(read source files) {
	@Put(additional read vars);
	std::string line;
	try { for (;;) {
		inputs.read_line(line);
		@Put(process line);
	} }
	catch (const no_more_lines &) {}
} @End(read source files)
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
@add(open input prereqs)
	@Put(input prereqs);
@end(open input prereqs)
```

```
@add(open input prereqs)
	class Input {
		public:
			@Put(input elements);
		private:
			std::string _path;
	};
@end(open input prereqs)
```
* A bunch of fragments make room for later extensions
* They are declared global, so they can be modified in different
  `x`-files


```
@def(inputs prereqs)
	@put(open input prereqs);
	class OpenInput {
		public:
			@Put(open input elements);
		private:
			@Put(private open input els);
	};
@end(inputs prereqs)
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
@End(private open input els)
```

```
@Def(open input elements)
	OpenInput(const std::string &path):
		_input { path },
		_file { path.c_str() }
	{}
@End(open input elements)
```

```
@Add(open input elements)
	OpenInput(
		const OpenInput &
	) = delete;
	OpenInput(
		OpenInput &&
	) = default;
@End(open input elements)
```

```
@Add(open input elements)
	OpenInput &operator=(
		const OpenInput &
	) = delete;
	OpenInput &operator=(
		OpenInput &&
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
		throw no_more_lines {};
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
@def(private inputs elements)
	std::vector<OpenInput> _open;
	std::vector<Input> _used;
@end(private inputs elements)
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
	while (! _open.empty()) {
		try {
			_open.back().read_line(line);
			return;
		} catch (const no_more_lines &) {}
		_used.push_back(std::move(
			_open.back().input()
		));
		_open.pop_back();
	}
	throw no_more_lines {};
@end(inputs read line)
```
* Probiert aus aktueller Datei eine Zeile zu lesen
* Wandert bei Misserfolg durch andere offenen Dateien

