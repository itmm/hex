# Description of the `Frag` `class`
* `Frag`s form a directed acyclic graph (`DAG`)
* the infix traversal of this `DAG` generates the source code files

```
@Add(input prereqs)
	class Frag;

	struct Write_State {
		std::string source_name = {};
		bool in_macro = false;
		bool c_style;

		Write_State(const Frag &f);
	};
@End(input prereqs)
```
* the `Write_State` `class` is important for writing line macros in
  C-like files
* the lines must only be written in C-like files
* and only if the current output is not in a C-like macro definition
* the state is initialized later from a `Frag`

```
@Add(input prereqs)
	@Put(frag prereqs);
	@put(define frag);
@end(input prereqs)
```
* `Frag`s are global elements

```
@def(define frag)
	class Frag_Ref {
	public:
		const std::string name;
		const bool local;
		Frag_Ref(
			const std::string &name = {},
			bool local = true
		):
			name { name },
			local { local }
		{ }
	};
	class Frag_Entry {
		std::string _str;
		std::string _file;
		int _first_line;
		int _last_line;
		Frag_Ref _sub;
	public:
		const Frag *frag;
		@put(entry methods);
		const Frag_Ref &sub() const {
			return _sub;
		}
	};
@end(define frag)
```
* a `Frag` contains a collection of entries
* each entry describes a range in the input file marked by `_file`,
  `_first_line` and `_last_line`
* each entry contains a string value or a reference to another `Frag`
* if both are available the `Frag` expands the referenced `frag` before
  the `_str`
* so a `_frag` can easily concatenate bytes to the `_str` of its last
  entry

```
@def(entry methods)
	Frag_Entry(): frag { nullptr } { }
	Frag_Entry(Frag *frag, bool local);
@end(entry methods)
```
* an entry can be initialized with a sub `Frag`
* no range is provided in this case
* the range information of the sub `Frag` will be used

```
@Add(input prereqs)
	inline Frag_Entry::Frag_Entry(
		Frag *frag, bool local
	):
		_first_line { -1 },
		_sub { frag ? frag->name : std::string { }, local },
		frag { frag }
	{}
@End(input prereqs)
```

```
@add(entry methods)
	void update_state(
		Write_State &state
	) const {
		@put(update state);
	}
@end(entry methods)
```
* this method checks, if the input is in a multi-line macro definition
* that is the case, if the line ends with a backslash followed by
  newline characters

```
@def(update state)
	auto c { _str.end() };
	auto b { _str.begin() };
	bool some_nl { false };
	while (b != c) {
		--c;
		@put(update state checks);
		break;
	}
	if (b != c && *c > ' ') {
		state.in_macro = false;
	}
@end(update state)
```
* if some characters are found in the line, it is not in a macro, if
  it was not recognized in the loop

```
@def(update state checks)
	if (*c == '\n' || *c == '\r') {
		some_nl = true;
		continue;
	}
	if (*c <= ' ') { continue; }
	if (*c == '\\') {
		if (some_nl) {
			state.in_macro = true;
			return;
		}
	}
@end(update state checks)
```
* this method checks, if the input is in a multi-line macro definition
* that is the case, if the line ends with a backslash followed by
  newline characters

```
@add(entry methods)
	std::string str(
		Write_State &state
	) const {
		@put(check c-like);
		std::ostringstream oss;
		oss << "\n#line " <<
			_first_line << " \"" <<
			_file << "\"\n" << _str;
		return oss.str();
	}
@end(entry methods)
```
* generates an output line
* if the current source file is C-like, a `#line` macro is written before
  the content
* so that a C compiler refers warnings and errors to the position in the
  input file
* not in the generated source file

```
@def(check c-like)
	bool old { state.in_macro };
	update_state(state);
	if (old) { return _str; }
	if (! state.c_style) { return _str; }
	if (_first_line < 1) { return _str; }
	if (_str.empty()) { return _str; };
@end(check c-like)
```
* C-like macros are not added, if the current line is part of a
  multi-line macro
* or if the file does not support line-macros
* or the line is not set
* or the string is empty

```
@add(entry methods)
	void add(
		char ch, const std::string &file,
		int line
	) {
		@mul(copy file and line);
		_str += ch;
	}
@end(entry methods)
```
* adds a character to an entry
* also the range of the entry is updated

```
@def(copy file and line)
	if (
		_file.empty() || _first_line <= 0
	) {
		_file = file;
		_first_line = line;
	}
	_last_line = line;
@end(copy file and line)
```
* if the entry is empty, the beginning of the range is updated
* also the end of the range is updated

```
@add(entry methods)
	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		@mul(copy file and line);
		_str += value;
	}
@end(entry methods)
```
* adds a whole `std::string` to an entry

```
@add(entry methods)
	bool canAdd(
		const std::string &file,
		int line
	) {
		@put(can add);
		return false;
	}
@end(entry methods)
```
* checks if a character at the specified position can be added to the
  fragment

```
@def(can add)
	if (
		! _file.empty() && file != _file
	) {
		return false;
	}
@end(can add)
```
* if the entry has a different file name, the character can not be added
  to this fragment

```
@add(can add)
	if (
		_last_line > 0 &&
		_last_line != line &&
		_last_line + 1 != line
	) {
		return false;
	}
@end(can add)
```
* if the last line does not match to the position, the character can not
  be added

```
@add(can add)
	return true;
@end(can add)
```
* otherwise the character can be added

```
@Add(includes)
	#include <vector>
@end(includes)
```
* needs `std::vector`

```
@add(define frag)
	class Frag {
		std::vector<Frag_Entry> _entries;
		int _expands;
		int _multiples;
		Frag *_prefix;
		const bool _is_meta;
	public:
		const std::string name;
		Frag *super = nullptr;
		@put(frag methods);
	};
@end(define frag)
```
* a `Frag` contains a collection of `Frag_Entry`s
* also a count is kept, how often the `Frag` was expanded in single and
  multiple style

## add a `Frag`

```
@def(frag methods)
	bool isFile() const {
		static const std::string prefix {
			"file: "
		};
		std::string p {
			name.substr(0, prefix.size())
		};
		return p == prefix;
	}
@end(frag methods)
```
* a `Frag` describes a file if its name has the prefix `@s(file: )`

```
@add(frag methods)
	std::string cmd() const {
		static const std::string prefix {
			"| "
		};
		std::string p {
			name.substr(0, prefix.size())
		};
		return p == prefix ?
			name.substr(prefix.size()) :
			std::string {};
	}
@end(frag methods)
```
* a `Frag` describes a command if its name has the prefix `@s(| )`

```
@add(frag methods)
	Frag(
		const std::string &name,
		Frag *prefix
	):
		_entries {},
		_expands { 0 },
		_multiples { 0 },
		_prefix { prefix },
		_is_meta { name.find('@') != std::string::npos },
		name { name }
	{
		if (isFile()) { ++_expands; }
		if (cmd().size()) { ++_expands; }
	}
@end(frag methods)
```
* initializes as `Frag`
* if the `Frag` is a name or command, it is counted as a single expand

```
@add(frag methods)
	const Frag *prefix() const {
		return _prefix;
	}
	Frag *prefix() {
		return _prefix;
	}
@end(frag methods)
```
* get prefix fragment

```
@add(frag methods)
	bool is_meta() {
		return _is_meta;
	}
@end(frag methods)
```

```
@add(frag methods)
	void clear() {
		if (_prefix) {
			_prefix->clear();
		}
		_entries.clear();
	}
@end(frag methods)
```
* deletes all entries
* will be used by `@rep` and `@Rep`

```
@add(frag methods)
	bool empty() const {
		if (
			_prefix && ! _prefix->empty()
		) {
			return false;
		}
		return _entries.empty();
	}
@end(frag methods)
```
* a fragment is empty, if it does not have any entries

```
@add(define frag)
	Write_State::Write_State(
		const Frag &f
	):
		c_style { f.is_c_style() }
	{ }
@end(define frag)
```
* the C-like property is copied from the fragment

```
@Def(perform unit-tests)
	@put(unit-tests);
@end(perform unit-tests)
```
* fragments have their own unit-test fragment

```
@add(define frag)
	void test_frag_name(
		const std::string &name
	) {
		Frag f(name, nullptr);
		ASSERT(f.name == name);
	}
@end(define frag)
```
* checks, if the fragment name is copied correctly

```
@def(unit-tests)
	test_frag_name("abc");
	test_frag_name("");
	test_frag_name("A c");
@end(unit-tests)
```
* verify that names are copied

```
@add(unit-tests) {
	Frag f { "ab", nullptr };
	ASSERT(f.empty());
} @end(unit-tests)
```
* check that a new `Frag` has no entries

```
@add(define frag)
	bool isPopulatedFrag(
		const Frag *f
	) {
		return f && ! f->empty();
	}
@end(define frag)
```
* check that a fragment is not empty

```
@add(unit-tests) {
	Frag_Entry entry;
	ASSERT(! entry.frag);
} @end(unit-tests)
```
* verify that an empty fragment has no sub `Frag`

```
@add(unit-tests) {
	Frag f { "", nullptr };
	Write_State s { f };
	Frag_Entry entry;
	ASSERT(entry.str(s).empty());
} @end(unit-tests)
```
* an empty entry will not return any bytes

## Add entries to `Frag`s
* add sub `Frag`s or text to a `Frag`

```
@add(frag methods)
	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		if (value.empty()) { return; }
		@mul(assure frag entry);
		_entries.back().add(
			value, file, line
		);
	}
@end(frag methods)
```
* add some text
* if the text is empty, nothing is added
* otherwise the method assures that there is an entry
* and adds text to this entry

```
@def(assure frag entry)
	if (_entries.empty()) {
		_entries.emplace_back();
	} else if (
		! _entries.back().canAdd(
			file, line
		)
	) {
		_entries.emplace_back();
	}
@end(assure frag entry)
```
* if there are no entries, a new one is added
* otherwise if the text is at the wrong position, a new one is also added

```
@add(frag methods)
	void add(
		char ch,
		const std::string &file,
		int line
	) {
		@mul(assure frag entry);
		_entries.back().add(
			ch, file, line
		);
	}
@end(frag methods)
```
* adds a single character to the `Frag`

```
@add(frag methods)
	Frag &add(const std::string &path, Frag *child, bool local);
@end(frag methods)
```
* adds a sub `Frag` to a `Frag`
* assures that no cycle will result

```
@add(define frag)
	@put(define cycle check)
	Frag &Frag::add(const std::string &path, Frag *child, bool local) {
		ASSERT(child);
		@put(avoid frag cycles);
		@put(add frag entry);
		return *this;
	}
@end(define frag)
```
* checks, that the sub `Frag` is valid and that no cycles will result

```
@def(add frag entry)
	_entries.push_back(
		Frag_Entry { child, local }
	);
@end(add frag entry)
```
* creates a new entry for the sub `Frag`

```
@add(frag methods)
	auto begin() const {
		return _entries.cbegin();
	}
@end(frag methods)
```
* getter for the begin entries iterator

```
@add(frag methods)
	auto end() const {
		return _entries.cend();
	}
@end(frag methods)
```
* getter for the end entries iterator

```
@add(frag methods)
	int expands() const {
		return _expands + (_prefix ? _prefix->expands() : 0);
	}
@end(frag methods)
```
* how often was the `Frag` `@put` or `@Put`

```
@add(frag methods)
	void addExpand() {
		++_expands;
	}
@end(frag methods)
```
* increases the `@put` or `@Put` count

```
@add(frag methods)
	int multiples() const {
		return _multiples + (_prefix ? _prefix->multiples() : 0);
	}
@end(frag methods)
```
* how often was the `Frag` `@mul` or `@Mul`

```
@add(frag methods)
	void addMultiple() {
		++_multiples;
	}
@end(frag methods)
```
* increases the `@mul` or `@Mul` count

```
@add(frag methods)
	bool is_c_style() const {
		@put(is c-style);
		return false;
	}
@end(frag methods)
```
* check if a fragment supports C-like line number macros

```
@def(is c-style)
	static const std::string exts[] = {
		".c", ".h", ".cpp"
	};
	const std::string *end =
		exts + sizeof(exts)/sizeof(*exts);
@end(is c-style)
```
* array of valid extensions

```
@add(is c-style)
	for (auto i = exts; i != end; ++i) {
		if (name.length() > i->length()) {
			if (name.substr(
				name.length() -
					i->length()) == *i
			) {
				return true;
			}
		}
	}
@end(is c-style)
```
* a fragment is of C style, if its name ends in one of the C/C++ file
  extensions

## Serialize `Frag`s
* write `Frag` traversal to a `std::ostream`

```
@add(define frag)
	void serializeFrag(
		const Frag &frag,
		std::ostream &out,
		Write_State &state
	) {
		@put(iterate entries);
	}
@end(define frag)
```
* iterate over the entries

```
@add(define frag)
	void serializeFrag(
		const Frag &f,
		std::ostream &out
	) {
		Write_State state { f };
		return serializeFrag(
			f, out, state
		);
	}
@end(define frag)
```
* estimate a `Write_State` first

```
@def(iterate entries)
	if (frag.prefix()) {
		serializeFrag(
			*frag.prefix(), out, state
		);
	}
	for (const auto &entry : frag) {
		if (entry.frag) {
			const Frag *f { entry.frag };
			while (f->super) {
				f = f->super;
			}
			serializeFrag(
				*f, out, state
			);
		}
		out << entry.str(state);
	}
@end(iterate entries)
```
* recursively visit sub `Frag`s
* then write the string value

```
@add(define frag)
	bool check_frag(
		const Frag &f,
		std::istream &in,
		Write_State &state
	) {
		@put(check entries);
		return true;
	}
@end(define frag)
```
* checks if the traversal results in the same value as a `std::istream`

```
@add(define frag)
	bool check_frag(
		const Frag &f,
		std::istream &in
	) {
		Write_State state { f };
		return check_frag(
			f, in, state
		);
	}
@end(define frag)
```
* estimate a `Write_State` first

```
@def(check entries)
	if (f.prefix()) {
		if (! check_frag(*f.prefix(), in, state)) {
			return false;
		}
	}
	for (const auto &entry : f) {
		if (entry.frag) {
			if (!check_frag(
				*entry.frag, in,
				state
			)) {
				return false;
			}
		}
		@put(check entry str);
	}
@end(check entries)
```
* recursively visit sub `Frag`s
* then compare the string value

```
@def(check entry str)
	for (
		const auto &i : entry.str(state)
	) {
		if (in.get() != i) {
			return false;
		}
	}
@end(check entry str)
```
* compare string value character by character

```
@add(define frag)
	void testFrag(
		const Frag &frag,
		const std::string &expected
	) {
		@put(serialize test frag);
	}
@end(define frag)
```
* verifies that a `Frag` serializes as expected

```
@Add(includes)
	#include <sstream>
@end(includes)
```
* needs `std::ostringstream`

```
@def(serialize test frag)
	std::ostringstream buffer;
	serializeFrag(frag, buffer);
	ASSERT(buffer.str() == expected);
@end(serialize test frag)
```
* serializes the `Frag`
* and compares resulting value

```
@add(define frag)
	void addStringToFrag(
		Frag *frag,
		const std::string &str
	) {
		frag->add(
			str, std::string {}, 0
		);
	}
@end(define frag)
```
* adds a zero-terminated string to a `Frag`

```
@add(unit-tests) {
	Frag frag { "", nullptr };
	addStringToFrag(&frag, "abc");
	addStringToFrag(&frag, "def");
	testFrag(frag, "abcdef");
} @end(unit-tests)
```
* checks that two strings are correctly serialized

```
@add(unit-tests) {
	Frag a { "", nullptr };
	Frag b { "", nullptr };
	addStringToFrag(&a, "abc");
	b.add("", &a, true);
	addStringToFrag(&b, "def");
	b.add("", &a, true);
	testFrag(b, "abcdefabc");
} @end(unit-tests)
```
* checks that sub `Frag`s are serialized correctly

## Cycle detection
* checks if the addition of a sub `Frag` would result in a cycle

```
@def(define cycle check)
	bool isFragInFrag(
		const std::string &path,
		const Frag *needle,
		const Frag *haystack
	) {
		ASSERT(needle);
		ASSERT(haystack);
		@put(check cycle frag);
		@put(check cycle entries);
		return false;
	}
@end(define cycle check)
```
* checks, if the parent `Frag` `needle` is already present in the `DAG`
  starting at the sub `Frag` `haystack`

```
@def(avoid frag cycles)
	ASSERT(! isFragInFrag(
		path, this, child
	));
@end(avoid frag cycles)
```
* a sub `Frag` can not be added, if a cycle would result

```
@def(check cycle frag)
	if (needle == haystack) {
		return true;
	}
@end(check cycle frag)
```
* if the container is itself the searched `Frag`, a cycle would result

```
@def(check cycle entries)
	if (haystack->prefix() && isFragInFrag(path, needle, haystack->prefix())) {
		return true;
	}
	for (const auto &i : *haystack)  {
		if (i.sub().name.empty()) { continue; }
		std::string new_path { path };
		Frag *f { find_frag(path, i.sub(), &new_path) };
		if (! f) { continue; }
		if (isFragInFrag(
			new_path, needle, f
		)) {
			return true;
		}
	}
@end(check cycle entries)
```
* otherwise all sub `Frag`s will be searched
* as soon as a cycle is found, the search can be aborted

