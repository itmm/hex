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
			@Put(private inputs elements);
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
		@Put(inputs read line);
	}
@end(inputs)
```

```
@def(inputs prereqs)
	struct no_more_lines {};
@end(inputs prereqs)
```

```
@Def(inputs read line)
	throw no_more_lines {};
@End(inputs read line)
```

## What is a file?
* C++ represent open files as `std::ifstream`
* Also a file is represented by its path
* `hx` also store more information on a file by file basis
* For example the local fragments
* So a special class `Input` will represent a file

```
@Add(includes)
	#include @s(<fstream>)
@End(includes)
```
* Defines `std::ifstream`

## `Input` class
* Defines the `Input` class

```
@add(inputs prereqs)
	@Put(input prereqs);
@end(inputs prereqs)
```

```
@add(inputs prereqs)
	class Input {
		public:
			std::string path;
			@Put(input elements);
		private:
			std::ifstream _file;
			@Put(private input elements);
	};
@end(inputs prereqs)
```
* The `path` can be public, because it is `const`
* But the `_file` should only be modified via methods
* so it is private
* A bunch of fragments make room for later extensions
* They are declared global, so they can be modified in different
  `x`-files


```
@Def(input elements)
	Input(const std::string &path):
		path { path },
		_file { path.c_str() }
		@Put(private input constructor)
	{}
@End(input elements)
```
* In the constructor the `name` will be saved
* and the file with that path will be opened
* Additional elements can be initialized later

```
@Add(input elements)
	Input(const Input &) = delete;
	Input(Input &&) = default;
	Input &operator=(const Input &) = delete;
	Input &operator=(Input &&) = default;
@End(input elements)
```
