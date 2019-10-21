# HTML Extractor
* this presentation is the program **HTML Extractor** (`hx`)
* it contains all source code that is needed to build the executable
* but the code is spread over multiple slides
* to form an extreme form of Literate Programming
* that may be called **Slideware-Programming** (`SWP`)
* Have fun!

## What is `hx`?
* `hx` is a program that parses a Markdown document and extracts source
  code or an executable program out of it
* think of it as a very powerful macro processor that combines, extends
  and orders small fragments of code
* but also it generates a HTML documentation like the one you are
  currently reading

## Why use Markdown document?
* Markdown documents are structured text documents
* it contains sections at different levels, paragraphs and code snippets
* `hx` formats the sections and code snippets as slides of a very big
  slide show
* these slides should be decorated with notes

## `SWP` ≠ Literate Programming
* `SWP` should not document a finished program
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
* you must not include a fragment that directly or indirectly
  includes the current fragment, because that will result in a cycle
* `hx` prevents cyclic includes
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
* `@k(@Def)` defines a global fragment that is visible in all input
  files
* `@k(@def)` defines a local fragment that is only visible in the
  current input file
* and in all input files that are included directly or indirectly by
  this input file for the first time

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
* but is is far more important in `SWP`, as a slide provides only limited
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
@add(global elements)
	class Frag;
	class Input;

	#include <map>
	using Frag_Map = std::map<std::string, Frag>;

	Frag *find_frag(const std::string &in, const std::string &key);
	Frag *find_frag(const Input &in, const std::string &key);
	Frag *find_frag(const std::string &key);

	Frag &add_frag(const std::string &in, const std::string &key);
	Frag &add_frag(const Input &in, const std::string &key);
	Frag &add_frag(const std::string &key);

	Frag_Map &frag_map(const std::string &in);
	Frag_Map &frag_map(const Input &in);
	Frag_Map &frag_map();

	void clear_frags();
@end(global elements)
```

```
@inc(read.md)
```
* defines the mechanisms of reading files line by line
* the `@k(@inc)` command includes a different input file at the current
  position
* the file is read only once, no matter how often it is included
* you can click on the argument value in the HTML documentation to
  navigate to the documentation from this file

```
@inc(blocks.md)
```
* the input is split into blocks
* a block has a type, a value and optional notes
* blocks are separated by newlines

```
@inc(log.md)
```
* defines logging mechanism

```
@inc(frag.md)
```
* fragments are flexible macro definitions
* they can be extended or replaced in later parts of the input file
* fragments can be used before they are defined
* if they are not defined in the end, they will be expanded to nothing
* so even a partial program can be generated

```
@inc(input.md)
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
		frag->add(ch,
			inputs.cur().input().path(),
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
		@put(cmd prefix);
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
@def(cmd prefix)
	while (ne != end && *ne != '(') {
		if (! isalpha(*ne)) {
			ne = end;
			break;
		}
		++ne;
	}
@end(cmd prefix)
```
* check that the name contains only letters
* and is followed by an opening parenthesis

```
@def(cmd argument)
	auto ab = ne + 1; auto ae = ab;
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
		ASSERT_MSG(frag, "@put" << "(" <<
			arg << ") not in frag"
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
* searches the fragment and integrates it
* if the fragment is not found, a new one is created to be populated
  later

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
* if the fragment was already expanded, an error message is generated
* also if the fragment was expanded in multiple mode

```
@add(do special cmd)
	if (name == "inc") {
		ASSERT_MSG(! frag,
			"include in frag [" <<
				frag->name << ']'
		);
		if (! inputs.has(arg)) {
			inputs.push(arg);
		}
		break;
	}
@end(do special cmd)
```
* includes another input file
* opens the file and pushes it on the stack of open file
* files that were are already included are ignored

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
* expands a fragment multiple times

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
* when a multiple expanded fragment was already expanded with a normal
  `@put` an error message is printed

```
@add(do special cmd)
	if (name == "Def") {
		@put(do Def);
		break;
	}
@end(do special cmd)
```
* creates a new fragment in global namespace

```
@def(do Def)
	ASSERT_MSG(! frag,
		"@Def in frag [" <<
		frag->name << ']'
	);
	frag = inputs.get_global(arg);
	if (isPopulatedFrag(frag)) {
		std::cerr << "Frag [" <<
			arg << "] already defined\n";
	}
@end(do Def)
```
* if the fragment already exists, an error message is printed

```
@add(do special cmd)
	if (name == "Add") {
		@put(do Add);
		break;
	}
@end(do special cmd)
```
* extends a global fragment

```
@def(do Add)
	ASSERT_MSG(! frag, "@Add in frag [" <<
		frag->name << ']'
	);
	frag = inputs.get_global(arg);
	if (! isPopulatedFrag(frag)) {
		std::cerr << "Frag [" << arg <<
			"] not defined\n";
	}
@end(do Add)
```
* a global fragment is any fragment that is defined in the global
  namespace or in an input file that includes the current input file
* if the fragment is not defined, an error message is printed

```
@add(do special cmd)
	if (name == "rep") {
		ASSERT_MSG(! frag,
			"@rep in frag [" <<
				frag->name << ']'
		);
		frag = inputs.get_local(arg);
		@mul(clear frag);
		break;
	}
@end(do special cmd)
```
* replaces the content of a local fragment

```
@add(do special cmd)
	if (name == "Rep") {
		ASSERT_MSG(! frag,
			"@Rep in frag [" <<
				frag->name << ']'
		);
		frag = inputs.get_global(arg);
		@mul(clear frag);
		break;
	}
@end(do special cmd)
```
* replace the content of a global fragment

```
@def(clear frag)
	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();
@end(clear frag)
```
* clears the content of a fragment
* if the fragment is not defined, an error message is printed

```
@add(do special cmd)
	if (name == "Put") {
		@put(do Put);
		break;
	}
@end(do special cmd)
```
* inserts a global fragment in the current fragment

```
@def(do Put)
	ASSERT_MSG(frag, "@Put not in frag");
	Frag *sub = inputs.get_global(arg);
	if (sub) {
		@mul(check frag ex. count);
		sub->addExpand();
		frag->add(sub);
	}
@end(do Put)
```
* check that the fragment is not already expanded

```
@add(do special cmd)
	if (name == "Mul") {
		@put(do Mul);
		break;
	}
@end(do special cmd)
```
* inserts a global fragment multiple times

```
@def(do Mul)
	ASSERT_MSG(frag, "@Mul not in frag");
	Frag *sub = inputs.get_global(arg);
	if (sub) {
		@mul(check for prev expands);
		sub->addMultiple();
		frag->add(sub);
	}
@end(do Mul)
```
* check that the fragment is not already used in a single expand

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
* creates a private identifier
* the identifier is based on the argument, but extended with a
  hash-code based on the name and current input file name

```
@add(includes)
	#include <functional>
	#include <sstream>
@end(includes)
```
* needed for hashing 

```
@def(process private frag)
	std::hash<std::string> h;
	auto cur {
		h(inputs.cur().input().path() +
			':' + arg) &
				0x7fffffff
	};
@end(process private frag)
```
* hash the current path and argument
* mask the hash to a positive integer

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
* new identifier has a common prefix,
* the hash,
* and the original argument

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
* create a hash value based on the argument and the current path

```
@def(process magic frag)
	std::hash<std::string> h;
	auto cur {
		h(inputs.cur().input().path() +
			':' + arg) &
				0x7fffffff
	};
@end(process magic frag)
```
* calculate hash value

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
* add the hash value to the fragment

# Serialize Fragments
* generate source code by traversing the tree of all fragments that
  specify files
* and write the contents into the files

```
@add(global elements)
	@put(needed by files write)
	void files_write() {
		@put(files write);
	}
@end(global elements)
```
* write all files generated from fragments

```
@def(serialize fragments)
	if (write_files) {
		files_write();
	}
@end(serialize fragments)
```
* write files, if requested

```
@def(files write)
	for (auto &i : frag_map()) {
		const Frag *frag {
			&i.second
		};
		@mul(serialize frag);
	}
@end(files write)
```
* fragments that start with `file:` represent files
* additional the code checks if fragments were expanded not often enough

```
@add(files write)
	for (auto &j : inputs) {
		for (auto &i : frag_map(j)) {
			const Frag *frag {
				&i.second
			};
			@mul(serialize frag);
		}
	}
@end(files write)
```
* also look for `file:` fragments in the local fragments
* so you should define file fragments as global to avoid overwriting of
  files

```
@def(serialize frag) {
	if (frag->isFile()) {
		@put(write in file);
	}
} @end(serialize frag)
```
* fragments that start with `file:` represent files
* additional the code checks if fragments were expanded not often enough

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
* if a fragment was not expanded, an error message is be written

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
* if a multiple fragment is only expanded once, an error message is
  written

```
@add(serialize frag)
	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not populated\n";
	}
@end(serialize frag)
```
* if a fragment was expanded, but not defined, an error message is
  written

```
@def(needed by files write)
	std::string file_name(const Frag &f) {
		return f.name.substr(6);
	}
@end(needed by files write)
```
* command argument without `"file:"` prefix is the file name

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
* check if the file will be changed
* if the fragments serialize to the content of the file, it must not be
  rewritten
* and the modification date can stay the same

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
* write fragment to the specified file

```
@add(global elements)
	@put(needed by files process);
	void files_process() {
		@put(files process);
	}
@end(global elements)
```
* serialize all files that are processed by external programs
* instead of directly written out

```
@add(serialize fragments)
	if (process_files) {
		files_process();
	}
@end(serialize fragments)
```
* only process files if requested

```
@def(files process)
	for (auto &i : frag_map()) {
		const Frag *frag {
			&i.second
		};
		@mul(serialize cmd);
	}
@end(files process)
```
* process commands that are key at root level

```
@add(files process)
	for (auto &j : inputs) {
		for (auto &i : frag_map(j)) {
			const Frag *frag {
				&i.second
			};
			@mul(serialize cmd);
		}
	}
@end(files process)
```
* process commands that are defined at the top of source files
* this behavior is deprecated

```
@def(needed by files process)
	bool no_cmds = false;
@end(needed by files process)
```
* there is an option to disable command processing

```
@def(serialize cmd) {
	const std::string cmd { frag->cmd() };
	if (cmd.size()) {
		@put(write cmd in file);
	}
} @end(serialize cmd)
```
* a fragment is only processed, if its name matches a command invocation

```
@def(write cmd in file)
	std::ostringstream out {};
	serializeFrag(*frag, out);
	std::string o { out.str() };
	if (no_cmds) {
		std::cout << o;
	} else {
		@put(do write cmd);
	}
@end(write cmd in file)
```
* pipe serialized fragment directly to the command
* if debug mode is activated the fragment is written to `std::cout`
  instead

```
@def(do write cmd)
	std::FILE *f {
		popen(cmd.c_str(), "w")
	};
	if (f) {
		std::fwrite(
			o.c_str(), o.size(), 1, f
		);
		pclose(f);
	}
@end(do write cmd)
```
* open pipe and send fragment to it

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
* disable command execution with a command switch
* the file will be serialized to `std::cout` instead

```
@inc(html.md)
```
* generate HTML slide show

```
@inc(view.md)
```
* Interactive display of slides

```
@inc(line.md)
```
* parsing lines with commands entered by the user

```
@inc(edit.md)
```
* edit slides in place

```
@inc(range.md)
```
* handle range requests in the editor

```
@inc(write.md)
```
* handle write commands in the editor

```
@inc(add.md)
```
* handle adding of new elements in the editor

```
@inc(ncurses.md)
```
* `ncurses` interface for the editor

```
@inc(todos.md)
```
* list of open issues

```
@add(global elements)
	using Inputs_Frag_Map = std::map<std::string, Frag_Map>;
	Inputs_Frag_Map _all_frags;

	Frag *find_frag(const std::string &in, const std::string &key) {
		auto got { _all_frags[in].find(key) };
		return got != _all_frags[in].end() ? &got->second : nullptr;
	}
	Frag *find_frag(const Input &in, const std::string &key) {
		const Input *i { &in };
		for (;;) {
			Frag *f { find_frag(i->path(), key) };
			if (f) { return f; }
			if (! i->prev) { return nullptr; }
			i = i->prev;
		}
	}
	Frag *find_frag(const std::string &key) {
		return find_frag(std::string { }, key);
	}

	Frag &add_frag(const std::string &in, const std::string &key) {
		return _all_frags[in].insert({ key, { key } }).first->second;
	}
	Frag &add_frag(const Input &in, const std::string &key) {
		return add_frag(in.path(), key);
	}
	Frag &add_frag(const std::string &key) {
		return add_frag(std::string { }, key);
	}

	Frag_Map &frag_map(const std::string &in) {
		return _all_frags[in];
	}

	Frag_Map &frag_map(const Input &in) {
		return frag_map(in.path());
	}
	Frag_Map &frag_map() {
		return frag_map(std::string { });
	}

	void clear_frags() { _all_frags.clear(); }
@end(global elements)
