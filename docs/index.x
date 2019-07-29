# HTML Extractor
* this presentation is the program **HTML Extractor** (`hx`)
* it contains all source code that is needed to build the executable
* but the code is spread over multiple slides
* to form an extreme form of Literate Programming
* that may be called **Slideware-Programming** (SWP)
* Have fun!

## What is `hx`?
* `hx` is a program that parses a `x`-document and extracts source  code
  or an executable program out of it
* think of it as a very powerful macro processor that combines, extends
  and orders small fragments of code
* but also it generates a HTML documentation like the one you are
  currently reading

## What is a `x`-document?
* `x`-documents are text documents with a markdown-like syntax
* it contains sections at different levels, paragraphs and code snippets
* the sections and code snippets are automatically formatted as slides
  of a very big slide show
* these slides can be decorated with notes

## SWP ≠ Literate Programming
* SWP should not document a finished program
* but it should document the process of creating a program instead
* after every slide you can generate the code from all the slides that
  you have read
* and without peeking in the future a runnable program must result
* so it does not contain features that are described the later slides

## A very top-down view of `hx`
* in the first code slide contains the highest view of the `hx` program
* while not very interesting, it contains a lot of commands that show
  how `hx` works

```
@Def(file: ../src/hx.cpp)
	@put(global elements)
	int main(
		int argc,
		const char **argv
	) {
		@put(main body)
	}
@End(file: ../src/hx.cpp)
```
* `hx` is written in C++ and consists of one source file `@s(hx.cpp)`
* global elements like types, macros and functions are defined before
  the central `@f(main)` function is defined
* these elements and the body of the `@f(main)` function will be defined
  and refined in later slides

### Commands
* the first code slide has three `hx` commands: `@k(@Def)`, `@k(@put)`,
  and `@k(@End)`
* each command starts with an ampersand (`@`), some letters and an
  argument block
* the argument block consists of an opening parenthesis, the argument
  value, and a closing parenthesis
* for now assume, that no closing parenthesis or ampersand is part of
  the argument value

### Fragments
* `hx` processes code fragments
* a fragment starts with an opening command like `@k(@Def)`
* and ends with a matching `@k(@End)` or `@k(@end)` closing command
* the **name** of the fragment is the argument value of the opening
  command
* and must be the same as the argument value of the closing command

### Defined Fragments
* three fragments are defined on the first code page
* they have the names `@s(file: hx.cpp)`, `@s(global elements)`, and
  `@s(main body)`
* not all `hx` commands use their argument value as fragment name
* but the three presented so far all do

### Including fragments
* a fragment can be included into another fragment with the `@k(@put)`
  command
* of course you must not include a fragment that directly or indirectly
  includes the current fragment
* the inclusion will happen after all slides are processed
* so it is possible to include a fragment that is not defined yet

### Naming files
* fragments whose name start with `@s(file: )` are special
* `hx` writes them into files
* the file name is the rest of the argument value after the `@s(file: )`
  prefix
* the first code slide creates one fragment that is written into the
  file named `@s(hx.cpp)`

### Generating the first source code
* if we stop here and run `hx`, the following code will be generated

```
int main(
	int argc,
	const char **argv
) {
}
```
* the unknown fragments are noted in the output of `hx`
* but the file compiles without errors
* so it does not do much

### Steps in `@f(main)`
* the next code slides identify multiple steps to perform in the
  `@f(main)` function

```
@def(main body)
	#if ! NDEBUG
		@put(perform unit-tests);
	#endif
@end(main body)
```
* the unit-tests are performed at every start of the program
* unless a release version is build
* the use of `@k(@def)` instead of `@k(@Def)` is no mistake
* `@k(@Def)` defines a global fragment that is visible in all `x`-files
* `@k(@def)` defines a local fragment that is only visible in the
  current `x`-file and in all included `x`-files

```
@add(main body)
	@put(process arguments);
@end(main body)
```
* after the unit-tests `@f(main)` parses the arguments passed to it from
  the command line
* `@k(@add)` extends an existing local fragment
* the ability of macros to grow with time is borrowed from Literate
  Programming
* but is is far more important in SWP, as a slide provides only limited
  space

```
@add(main body)
	@put(read source files);
@end(main body)
```
* next the source files are read into a fragment tree
* here a lot of interesting things are happening but right now it will
  expand to nothing as nothing is described yet

```
@add(main body)
	@put(serialize fragments);
@end(main body)
```
* in the next step `hx` writes the content of every file fragment in its
  designated file

```
@add(main body)
	@put(write HTML file);
@end(main body)
```
* and lastly the HTML documentation is generated

```
@def(global elements)
	@put(includes);
@end(global elements)
```
* `hx` defines a global fragment for included files

## Next steps
* the following slides refer to documents that document the next steps
  of `hx`
* follow them in order so that you do not miss important concepts

```
@inc(read.x)
```
* defines the mechanisms of reading files line by line
* the `@k(@inc)` command includes a different `x`-file at the current
  position
* the file is read only once, no matter how often it is included
* you can click on the argument value in the HTML documentation to
  navigate to the documentation from this file

```
@inc(blocks.x)
```
* the input is split into blocks
* a block has a type, a value and optional notes
* blocks are separated by newlines

```
@inc(log.x)
```
* defines logging mechanism

```
@inc(frag.x)
```
* fragments are flexible macro definitions
* they can be extended or replaced in later parts of the `.x`-file
* fragments can be used before they are defined
* if they are not defined in the end, they will be expanded to nothing
* so even a partial program can be generated

```
@inc(input.x)
```
* the input class contains all the blocks and fragments that the input
  file consists of
* the fragments are used to generate the code
* the blocks are used to generate the documentation
* the blocks are mostly needed for the interactive editor that is
  block-based

## Parsing command line arguments
* parses the command line arguments element by element

```
@def(process arguments)
	for (int i { 1 }; i < argc; ++i) {
		std::string arg { argv[i] };
		@put(process argument);
		@put(process file argument);
		ASSERT_MSG(false,
			"unknown argument [" <<
			argv[i] << ']'
		);
	}
@end(process arguments)
```
* the arguments are processed one by one
* if an argument is processed successfully, the program short-cuts the
  loop
* so the end of the loop is an indicator that an unknown argument
  occurred.
* if it is not a known argument it may be a file name.

```
@add(global elements)
	std::string stylesheet {
		"slides/slides.css"
	};
@end(global elements)
```
* for the HTML-output a CSS-stylesheet is used
* an argument can specify the used file
* but a default is presented here

```
@def(process argument) {
	static const std::string prefix {
		"--css="
	};
	if (arg.substr(
		0, prefix.length()
	) == prefix) {
		stylesheet =
			arg.substr(prefix.length());
		continue;
	}
} @end(process argument)
```
* sets a new stylesheet path

```
@Def(needed by read_sources)
	int blockLimit = -1;
@End(needed by read_sources)
```
* limits the number of blocks that will be processed

```
@add(process argument) {
	static const std::string prefix {
		"--limit="
	};
	if (arg.substr(
		0, prefix.length()
	) == prefix) {
		blockLimit = std::stoi(
			arg.substr(prefix.length())
		);
		continue;
	}
} @end(process argument)
```
* the user can limit the number of blocks that will be processed
* so the user can verify that subsets are working properly

```
@def(process file argument)
	inputs.add(arg);
	continue;
@end(process file argument)
```
* if no other argument pattern matches the argument is treated as input
  file
* from these files slides and source code are generated

# Read input files
* the code of this section reads input files, extracts the fragments
  and blocks and establishes their interdependencies

```
@Add(inputs prereqs)
	using SI =
		std::string::const_iterator;
@End(inputs prereqs)
```
* the code uses string iterators at a lot of places
* so it defines a shorthand to reduce clutter on the slides

```
@Add(needed by read_sources)
	void process_char(
		Frag *frag, char ch
	) {
		@put(process char);
	}
@End(needed by read_sources)
```
* adds a single character to the content of a fragment

```
@Add(process line)
	auto end = line.cend();
	for (
		auto i = line.cbegin();
		i != end; ++i
	) {
		@put(process special chars);
		process_char(frag, *i);
	}
	process_char(frag, '\n');
@End(process line)
```
* reads each character in the current line
* special characters are processed first
* if the character was not special, it will be added to the current
  active fragment

```
@Def(additional read vars)
	Frag *frag { nullptr };
@End(additional read vars)
```
* the `frag` pointer signals if the current line is inside a fragment in
  a code sequence (not `nullptr`)
* at the beginning there is no active fragment
* so the pointer is `nullptr`

```
@def(process char)
	if (frag) {
		frag->add(
			ch, inputs.cur().input().path(),
			inputs.cur().line()
		);
	}
@end(process char)
```
* if there is an active fragment, the character will be added to its
  contents
* based on the current input file and line number the fragment may open
  a new block

```
@def(process special chars)
	if (*i == '@') {
		auto nb = i + 1;
		auto ne = nb;
		while (ne != end && *ne != '(') {
			if (! isalpha(*ne)) {
				ne = end;
				break;
			}
			++ne;
		}
		if (ne != end && ne != nb) {
			std::string name { nb, ne };
			@put(cmd argument);
		}
	}
@end(process special chars)
```
* special sequences start with `@`
* it follows a name and an argument in parenthesis
* if the input is not matching it is not treated as a syntax error
* but as normal characters that are not treated special

```
@def(cmd argument)
	auto ab = ne + 1;
	auto ae = ab;
	while (ae != end && *ae != ')') {
		if (*ae == '@') {
			if (++ae == end) { break; }
		}
		++ae;
	}
	if (ae != end) {
		std::string arg {ab, ae};
		@put(cmd found);
		continue;
	}
@end(cmd argument)
```
* arguments are read until the closing parenthesis
* characters in the argument can be escaped by prefixing them with `@`
* that is needed to allow for the closing parenthesis in the argument

```
@def(cmd found)
	i = ae;
	bool outside = ! frag;
	do {
		if (outside && ! blockLimit) {
			break;
		}
		@put(do special cmd);
		@put(do default cmd);
	} while (false);
	if (blockLimit && outside && frag) {
		--blockLimit;
	}
@end(cmd found)
```
* if the block limit is reached, nothing will be processed
* if the command was not a functional command, it will be replaced by its
  argument in source code
* for example formatting hints are only used for syntax highlighting
* and can be ignored in generated source code
* after processing a block the code adjusts the limit

```
@Add(needed by read_sources)
	inline void expand_cmd_arg(
		Frag *f, const std::string &arg
	) {
		auto b = arg.begin();
		auto e = arg.end();
		@put(expand loop);
	}
@End(needed by read_sources)
```
* performs the default expansion of an unknown command

```
@def(do default cmd)
	if (frag) {
		expand_cmd_arg(frag, arg);
	}
@end(do default cmd)
```
* if the command was not a functional command, it will be replaced by its
  argument in source code

```
@add(includes)
	#include <algorithm>
@end(includes)
```
* `std::find` is used on the next slide

```
@def(expand loop)
	while (b != e) {
		auto x = std::find(b, e, '@');
		@put(expand before);
		if (x != e) {
			b = x + 1;
			@put(expand escaped);
		} else {
			b = e;
		}
	}
@end(expand loop)
```
* the code can not copy the argument directly
* the argument may contain escaped sequences starting with `@`
* the code will unescape them

```
@def(expand before)
	f->add(
		std::string { b, x },
		inputs.cur().input().path(),
		inputs.cur().line()
	);
@end(expand before)
```
* copy everything that is before the current iterator

```
@def(expand escaped)
	if (b != e) {
		f->add(
			*b,
			inputs.cur().input().path(),
			inputs.cur().line()
		);
		++b;
	}
@end(expand escaped)
```
* add escaped char, if there is one

```
@Add(needed by read_sources)
	#define ASSERT_NOT_FRAG() \
		ASSERT_MSG(! frag, '@' << \
			name << "(" << arg << \
			") in frag [" << \
			frag->name << ']' \
		)
@End(needed by read_sources)
```
* raise error, if command is in an active fragment

```
@Add(needed by read_sources)
	#define CHECK_NOT_DEFINED() \
		if (isPopulatedFrag(frag)) { \
			WARN_MSG("frag [" << arg << \
				"] already defined" \
			); \
		}
@End(needed by read_sources)
```
* warn, if fragment is already filled with some content

```
@def(do special cmd)
	if (name == "def") {
		ASSERT_NOT_FRAG();
		frag = inputs.get_local(arg);
		CHECK_NOT_DEFINED();
		break;
	}
@end(do special cmd)
```
* creates a new fragment
* fragment must not be created multiple times

```
@Add(needed by read_sources)
	#define ASSERT_FRAG() \
		ASSERT_MSG(frag, '@' << \
			name << "(" << arg << \
			") in frag [" << \
			frag->name << ']' \
		)
@End(needed by read_sources)
```
* raise error, if command is not in an active fragment

```
@add(do special cmd)
	if (name == "end" || name == "End") {
		ASSERT_FRAG();
		@put(frag names must match);
		frag = nullptr;
		break;
	}
@end(do special cmd)
```
* interrupts the active fragment

```
@def(frag names must match)
	ASSERT_MSG(frag->name == arg,
		"closing [" << arg <<
		"] != [" << frag->name << ']'
	);
@end(frag names must match)
```
* the name of the fragment must match the command argument

```
@Add(needed by read_sources)
	#define CHECK_DEFINED() \
		if (! isPopulatedFrag(frag)) { \
			WARN_MSG("frag [" << arg << \
				"] not defined" \
			); \
		}
@End(needed by read_sources)
```
* warn, if fragment is not filled with some content

```
@add(do special cmd)
	if (name == "add") {
		ASSERT_NOT_FRAG();
		frag = inputs.get_local(arg);
		CHECK_DEFINED();
		break;
	}
@end(do special cmd)
```
* reopens an existing fragment
* more content can be added to it

```
@add(do special cmd)
	if (name == "put") {
		ASSERT_MSG(frag,
			"@put" << "(" << arg << ") not in frag"
		);
		Frag *sub = inputs.get_local(arg);
		if (sub) {
			@mul(check frag ex. count);
			sub->addExpand();
			frag->add(sub);
		}
		break;
	}
@end(do special cmd)
```
* Bei einem `@put` wird das Fragment gesucht und eingebunden
* Ggf. wird das Fragment dabei auch erzeugt, um später befüllt zu
  werden

```
@def(check frag ex. count)
	if (sub->expands()) {
		std::cerr <<
			"multiple expands of [" <<
			sub->name << "]\n";
	}
	if (sub->multiples()) {
		std::cerr <<
			"expand after mult of ["
			<< sub->name << "]\n";
	}
@end(check frag ex. count)
```
* Wenn das Fragment bereits expandiert wurde, dann wird eine Meldung
  ausgegeben
* Wenn das Fragment bereits im Mehrfach-Modus ausgegeben wurde, wird
  ebenfalls eine Meldung ausgegeben

```
@add(do special cmd)
	if (name == "inc") {
		ASSERT_MSG(! frag,
			"include in frag [" << frag->name << ']'
		);
		if (! inputs.has(arg)) {
			inputs.push(arg);
		}
		break;
	}
@end(do special cmd)
```
* Wenn eine Datei eingebunden werden soll, dann wird sie geöffnet und
  auf den Stapel der offenen Dateien gelegt
* Wenn die Datei bereits geöffnet wurde, dann wird sie ignoriert

```
@add(do special cmd)
	if (name == "mul") {
		ASSERT_MSG(frag,
			"@mul not in frag"
		);
		Frag *sub = inputs.get_local(arg);
		if (sub) {
			@mul(check for prev expands);
			sub->addMultiple();
			frag->add(sub);
		}
		break;
	}
@end(do special cmd)
```
* Mit einem `@mul` Befehl kann ein Fragment an mehreren Stellen
  expandiert werden

```
@def(check for prev expands)
	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}
@end(check for prev expands)
```
* Es ist ein Fehler, wenn das Fragment bereits mit `@put` eingebunden
  wurde

```
@add(do special cmd)
	if (name == "Def") {
		ASSERT_MSG(! frag, "@Def in frag [" << frag->name << ']');
		frag = inputs.get_global(arg);
		if (isPopulatedFrag(frag)) {
			std::cerr << "Frag [" << arg << "] already defined\n";
		}
		break;
	}
@end(do special cmd)
```
* Erzeugt ein neues Fragment im globalen Namensraum
* Das Fragment darf nicht mehrfach definiert werden

```
@add(do special cmd)
	if (name == "Add") {
		ASSERT_MSG(! frag, "@Add in frag [" << frag->name << ']');
		frag = inputs.get_global(arg);
		if (! isPopulatedFrag(frag)) {
			std::cerr << "Frag [" << arg << "] not defined\n";
		}
		break;
	}
@end(do special cmd)
```
* Erweitert ein global definiertes Fragment

```
@add(do special cmd)
	if (name == "rep") {
		ASSERT_MSG(! frag,
			"@rep in frag [" << frag->name << ']'
		);
		frag = inputs.get_local(arg);
		@mul(clear frag);
		break;
	}
@end(do special cmd)
```
* Bei einem `@rep` wird der Inhalt eines Fragments zurückgesetzt
* Das Fragment muss bereits vorhanden sein

```
@add(do special cmd)
	if (name == "Rep") {
		ASSERT_MSG(! frag,
			"@Rep in frag [" << frag->name << ']'
		);
		frag = inputs.get_global(arg);
		@mul(clear frag);
		break;
	}
@end(do special cmd)
```
* Bei einem `@rep` wird der Inhalt eines Fragments zurückgesetzt
* Das Fragment muss bereits vorhanden sein

```
@def(clear frag)
	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();
@end(clear frag)
```
* Löscht das aktuelle Fragment

```
@add(do special cmd)
	if (name == "Put") {
		ASSERT_MSG(frag,
			"@Put not in frag"
		);
		Frag *sub = inputs.get_global(arg);
		if (sub) {
			@mul(check frag ex. count);
			sub->addExpand();
			frag->add(sub);
		}
		break;
	}
@end(do special cmd)
```
* Bei einem `@Put` wird das Fragment in den umschließenden  `FragMap`s
  gesucht

```
@add(do special cmd)
	if (name == "Mul") {
		ASSERT_MSG(frag,
			"@Mul not in frag"
		);
		Frag *sub = inputs.get_global(arg);
		if (sub) {
			@mul(check for prev expands);
			sub->addMultiple();
			frag->add(sub);
		}
		break;
	}
@end(do special cmd)
```
* `@Mul` expandiert ein globales Fragment an mehreren Stellen

```
@add(do special cmd)
	if (name == "priv") {
		ASSERT_MSG(frag,
			"@priv not in frag"
		);
		@put(process private frag);
		break;
	}
@end(do special cmd)
```
* Private Bezeichner werden durch einen Hash erweitert
* Um sie global unique zu machen

```
@add(includes)
	#include <functional>
	#include <sstream>
@end(includes)
```
* Enthält Hash-Funktion

```
@def(process private frag)
	std::hash<std::string> h;
	unsigned cur {
		h(inputs.cur().input().path() +
			':' + arg) &
				0x7fffffff
	};
@end(process private frag)
```
* Der Hash wird aus dem aktuellen Dateinamen
* Und dem aktuellen Bezeichner berechnet
* Zum Schluss wird er auf eine positive Zahl maskiert

```
@add(process private frag)
	std::ostringstream hashed;
	hashed << "_private_" <<
		cur << '_' <<
		arg;
	frag->add(
		hashed.str(),
		inputs.cur().input().path(),
		inputs.cur().line()
	);
@end(process private frag)
```
* Zuerst werden eventuell zwischengespeicherte Zeichen ausgegeben
* Dann kommt der neue Bezeichner
* Dieser besteht aus einem konstanten Präfix
* Dem Hash-Wert
* Und dem alten Bezeichner

```
@add(do special cmd)
	if (name == "magic") {
		ASSERT_MSG(frag,
			"@magic not in frag"
		);
		@put(process magic frag);
		break;
	}
@end(do special cmd)
```
* Der `@magic`-Befehl erzeugt einen Hash-Wert
* Der sich aus dem Dateinamen und dem Argument des Befehls  zusammen
  setzt

```
@def(process magic frag)
	std::hash<std::string> h;
	unsigned cur {
		h(inputs.cur().input().path() +
			':' + arg) &
				0x7fffffff
	};
@end(process magic frag)
```
* Berechnet Hash-Wert

```
@add(process magic frag)
	std::ostringstream value;
	value << cur;
	frag->add(
		value.str(),
		inputs.cur().input().path(),
		inputs.cur().line()
	);
@end(process magic frag)
```
* Gibt den Hash-Wert aus
* Vorher wird noch eventuell gespeicherte Zeichen ausgegeben

# Fragmente serialisieren
* Fragmente, die Dateien spezifizieren werden in diese Dateien
  rausgeschrieben

```
@add(global elements)
	@put(needed by files write)
	void files_write() {
		@put(files write);
	}
@end(global elements)
```

```
@def(serialize fragments)
	if (write_files) {
		files_write();
	}
@end(serialize fragments)
```

```
@def(files write)
	for (auto &i : inputs.root()) {
		const Frag *frag {
			&i.second
		};
		@mul(serialize frag);
	}
@end(files write)
```
* Fragmente, die mit `file:` beginnen, werden in die entsprechenden
  Dateien rausgeschrieben
* Zusätzlich wird geprüft, ob Fragmente zu selten oder zu oft
  expandiert  wurden

```
@add(files write)
	for (auto &j : inputs) {
		for (auto &i : j.frags) {
			const Frag *frag {
				&i.second
			};
			@mul(serialize frag);
		}
	}
@end(files write)
```
* Auch alle lokalen Fragmente bearbeiten

```
@def(serialize frag) {
	if (frag->isFile()) {
		@put(write in file);
	}
} @end(serialize frag)
```
* Wenn der Name eines Fragments mit `file: ` beginnt, dann wird es in
  die passende Datei geschrieben
* Zusätzlich zählt das als eine Expansion

```
@add(serialize frag) {
	int sum {
		frag->expands()
			+ frag->multiples()
	};
	if (sum <= 0) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not called\n";
	}
} @end(serialize frag)
```
* Ein Fragment wurde nicht aufgerufen
* Dies wird mit einer Meldung protokolliert

```
@add(serialize frag)
	if (frag->multiples() == 1) {
		std::cerr <<
			"multiple frag [" <<
			frag->name <<
			"] only used once\n";
	}
@end(serialize frag)
```
* Ein Fragment das zur mehrfachen Verwendung deklariert wurde, wird nur
  einmal verwendet
* Dies wird mit einer Meldung protokolliert

```
@add(serialize frag)
	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not populated\n";
	}
@end(serialize frag)
```
* Für jedes Fragment, das nicht befüllt wurde wird eine Meldung
  ausgegeben

```
@def(needed by files write)
	std::string file_name(const Frag &f) {
		return f.name.substr(6);
	}
@end(needed by files write)
```

```
@add(needed by files write)
	bool file_changed(const Frag &f) {
		std::ifstream in(
			file_name(f).c_str()
		);
		if (! check_frag(f, in)) {
			return true;
		}
		if (in.get() != EOF) {
			return true;
		}
		return false;
	}
@end(needed by files write)
```

```
@def(write in file)
	if (file_changed(*frag)) {
		std::ofstream out(
			file_name(*frag).c_str()
		);
		serializeFrag(*frag, out);
	}
@end(write in file)
```
* Das Fragment wird in die entsprechende Datei geschrieben

```
@add(global elements)
	@put(needed by files process);
	void files_process() {
		@put(files process);
	}
@end(global elements)
```

```
@add(serialize fragments)
	if (process_files) {
		files_process();
	}
@end(serialize fragments)
```

```
@def(files process)
	for (auto &i : inputs.root()) {
		const Frag *frag {
			&i.second
		};
		@mul(serialize cmd);
	}
@end(files process)
```

```
@add(files process)
	for (auto &j : inputs) {
		for (auto &i : j.frags) {
			const Frag *frag {
				&i.second
			};
			@mul(serialize cmd);
		}
	}
@end(files process)
```

```
@def(needed by files process)
	bool no_cmds = false;
@end(needed by files process)
```

```
@def(serialize cmd) {
	const std::string cmd { frag->cmd() };
	if (cmd.size()) {
		@put(write cmd in file);
	}
} @end(serialize cmd)
```

```
@def(write cmd in file)
	std::ostringstream out {};
	serializeFrag(*frag, out);
	std::string o { out.str() };
	if (no_cmds) {
		std::cout << o;
	} else {
		std::FILE *f {
			popen(cmd.c_str(), "w")
		};
		if (f) {
			std::fwrite(o.c_str(), o.size(), 1, f);
			pclose(f);
		}
	}
@end(write cmd in file)
```

```
@add(process argument) {
	static const std::string prefix {
		"--no-cmds"
	};
	if (arg == prefix) {
		no_cmds = true;
		continue;
	}
} @end(process argument)
```

```
@inc(html.x)
```
* Die Generierung liegt in einer eigenen Datei

```
@inc(view.x)
```
* Interactive display of slides

```
@inc(line.x)
```

```
@inc(edit.x)
```
* Edit slides in place

```
@inc(range.x)
```

```
@inc(write.x)
```

```
@inc(add.x)
```

```
@inc(ncurses.x)
```

```
@inc(todos.x)
```

