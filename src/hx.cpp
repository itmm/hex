
#line 38 "index.md"

	
#line 169 "index.md"

	
#line 36 "read.md"

	#include <string>

#line 176 "read.md"

	#include <fstream>

#line 96 "blocks.md"

	#include <vector>

#line 9 "log.md"

	#include <iostream>
	#include <exception>

#line 197 "frag.md"

	#include <vector>

#line 645 "frag.md"

	#include <sstream>

#line 761 "frag.md"

	#include <map>

#line 4 "input.md"

	#include <iostream>
	#include <vector>
	#include <filesystem>

#line 465 "index.md"

	#include <algorithm>

#line 820 "index.md"

	#include <functional>
	#include <sstream>

#line 86 "html.md"

	#include <string>

#line 695 "html.md"

	#include <cctype>

#line 745 "html.md"

	#include <set>

#line 94 "line.md"

	#include <limits>

#line 4 "ncurses.md"

	#if defined HAVE_CONFIG_H
		#include "config.h"
	#endif
	#if defined HAVE_NCURSESW_CURSES_H
		#include <ncursesw/curses.h>
	#elif defined HAVE_NCURSESW_H
		#include <ncursesw.h>
	#elif defined HAVE_NCURSES_CURSES_H
		#include <ncurses/curses.h>
	#elif defined HAVE_NCURSES_H
		#include <ncurses.h>
	#elif defined HAVE_CURSES_H
		#include <curses.h>
	#endif

#line 23 "ncurses.md"

	#include <locale.h>

#line 170 "index.md"
;

#line 5 "read.md"

	
#line 20 "read.md"

	
#line 64 "read.md"

	struct No_More_Lines {};

#line 146 "read.md"

	
#line 160 "read.md"

	
#line 7 "blocks.md"

	
#line 14 "blocks.md"

	enum class Read_State {
		new_element
		
#line 67 "blocks.md"
,
	header

#line 148 "blocks.md"
,
	code,
	after_code

#line 198 "blocks.md"
,
	notes

#line 254 "blocks.md"
,
	para

#line 17 "blocks.md"

	};

#line 26 "blocks.md"

	using RS = Read_State;

#line 103 "blocks.md"

	struct Block {
		Read_State state;
		std::vector<std::string> value;
		std::vector<std::string> notes;
		int level;
	};

#line 8 "blocks.md"


#line 7 "frag.md"

	class Frag;

	struct Write_State {
		std::string source_name = {};
		bool in_macro = false;
		bool c_style;

		Write_State(const Frag &f);
	};

#line 21 "frag.md"

	
#line 19 "log.md"

	#define ASSERT(COND) \
		if (! (COND)) { \
			 \
	std::cerr << \
		__FILE__ << ':' << __LINE__ << \
		' ' << #COND << " FAILED"; \
; \
			 \
	std::cerr << '\n'; \
; \
			 \
	throw std::exception(); \
; \
		}

#line 36 "log.md"

	#define ASSERT_MSG(COND, MSG) \
		if (! (COND)) { \
			 \
	std::cerr << \
		__FILE__ << ':' << __LINE__ << \
		' ' << #COND << " FAILED"; \
; \
			std::cerr << ": " << MSG; \
			 \
	std::cerr << '\n'; \
; \
			 \
	throw std::exception(); \
; \
		}

#line 51 "log.md"

	#define WARN_MSG(MSG) \
		std::cerr << __FILE__ << ':' << __LINE__ << ' '; \
		std::cerr << MSG; \
		 \
	std::cerr << '\n'; \
;
;
	
	class FragEntry {
		std::string _str;
		std::string _file;
		int _first_line;
		int _last_line;
	public:
		const Frag *frag;
		
#line 49 "frag.md"

	FragEntry(
		Frag *frag = nullptr
	):
		_first_line { -1 },
		frag { frag }
	{}

#line 63 "frag.md"

	void update_state(Write_State &state) const {

		auto c { _str.end() };
		auto b { _str.begin() };
		bool some_nl { false };
		while (b != c) {
			--c;
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
			break;
		}
		if (b != c && *c > ' ') {
			state.in_macro = false;
		}
	}

	std::string str(Write_State &state) const {
		bool old { state.in_macro };
		update_state(state);
		if (old) { return _str; }
		if (! state.c_style) { return _str; }
		if (_first_line < 1) { return _str; }
		if (_str.empty()) { return _str; };
		std::ostringstream oss;
		oss << "\n#line " <<
			_first_line << " \"" << _file << "\"\n" << _str;
		return oss.str();
	}

#line 106 "frag.md"

	void add(
		char ch, const std::string &file,
		int line
	) {
		
#line 121 "frag.md"

	if (
		_file.empty() || _first_line <= 0
	) {
		_file = file;
		_first_line = line;
	}
	_last_line = line;

#line 111 "frag.md"
;
		_str += ch;
	}

#line 136 "frag.md"

	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		
#line 121 "frag.md"

	if (
		_file.empty() || _first_line <= 0
	) {
		_file = file;
		_first_line = line;
	}
	_last_line = line;

#line 142 "frag.md"
;
		_str += value;
	}

#line 150 "frag.md"

	bool canAdd(
		const std::string &file,
		int line
	) {
		
#line 164 "frag.md"

	if (
		! _file.empty() && file != _file
	) {
		return false;
	}

#line 176 "frag.md"

	if (
		_last_line > 0 &&
		_last_line != line &&
		_last_line + 1 != line
	) {
		return false;
	}

#line 190 "frag.md"

	return true;

#line 155 "frag.md"
;
		return false;
	}

#line 37 "frag.md"
;
	};

#line 204 "frag.md"

	class Frag {
		std::vector<FragEntry> _entries;
		int _expands;
		int _multiples;
	public:
		const std::string name;
		
#line 223 "frag.md"

	bool isFile() const {
		static const std::string prefix {
			"file: "
		};
		std::string p {
			name.substr(0, prefix.size())
		};
		return p == prefix;
	}

#line 239 "frag.md"

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

#line 255 "frag.md"

	Frag(
		const std::string &name
	):
		_entries {},
		_expands { 0 },
		_multiples { 0 },
		name { name }
	{
		if (isFile()) { ++_expands; }
		if (cmd().size()) { ++_expands; }
	}

#line 272 "frag.md"

	void clear() {
		_entries.clear();
	}

#line 282 "frag.md"

	bool empty() const {
		return _entries.empty();
	}

#line 375 "frag.md"

	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		if (value.empty()) { return; }
		
#line 395 "frag.md"

	if (_entries.empty()) {
		_entries.push_back(FragEntry {});
	} else if (
		! _entries.back().canAdd(
			file, line
		)
	) {
		_entries.push_back(FragEntry {});
	}

#line 382 "frag.md"
;
		_entries.back().add(
			value, file, line
		);
	}

#line 412 "frag.md"

	void add(
		char ch,
		const std::string &file,
		int line
	) {
		
#line 395 "frag.md"

	if (_entries.empty()) {
		_entries.push_back(FragEntry {});
	} else if (
		! _entries.back().canAdd(
			file, line
		)
	) {
		_entries.push_back(FragEntry {});
	}

#line 418 "frag.md"
;
		_entries.back().add(
			ch, file, line
		);
	}

#line 429 "frag.md"

	Frag &add(Frag *child);

#line 465 "frag.md"

	auto begin() const {
		return _entries.cbegin();
	}

#line 474 "frag.md"

	auto end() const {
		return _entries.cend();
	}

#line 483 "frag.md"

	int expands() const {
		return _expands;
	}

#line 492 "frag.md"

	void addExpand() {
		++_expands;
	}

#line 501 "frag.md"

	int multiples() const {
		return _multiples;
	}

#line 510 "frag.md"

	void addMultiple() {
		++_multiples;
	}

#line 519 "frag.md"

	bool is_c_style() const {
		static const std::string extensions[] = {
			".c", ".h", ".cpp"
		};
		const std::string *end = extensions + sizeof(extensions)/sizeof(*extensions);
		for (auto i = extensions; i != end; ++i) {
			if (name.length() > i->length()) {
				if (name.substr(name.length() - i->length()) == *i) {
					return true;
				}
			}
		}
		return false;
	}

#line 211 "frag.md"
;
	};

#line 291 "frag.md"

	Write_State::Write_State(const Frag &f):
		c_style { f.is_c_style() }
	{ }

#line 308 "frag.md"

	void testFragName(
		const std::string &name
	) {
		Frag f(name);
		ASSERT(f.name == name);
	}

#line 335 "frag.md"

	bool isPopulatedFrag(
		const Frag *f
	) {
		return f && ! f->empty();
	}

#line 438 "frag.md"

	
#line 703 "frag.md"

	bool isFragInFrag(
		const Frag *needle,
		const Frag *haystack
	) {
		ASSERT(needle);
		ASSERT(haystack);
		
#line 732 "frag.md"

	if (needle == haystack) {
		return true;
	}

#line 710 "frag.md"
;
		
#line 742 "frag.md"

	for (const auto &i : *haystack)  {
		if (! i.frag) { continue; }
		if (isFragInFrag(
			needle, i.frag
		)) {
			return true;
		}
	}

#line 711 "frag.md"
;
		return false;
	}

#line 439 "frag.md"

	Frag &Frag::add(Frag *child) {
		ASSERT(child);
		
#line 722 "frag.md"

	ASSERT(! isFragInFrag(
		this, child
	));

#line 442 "frag.md"
;
		
#line 456 "frag.md"

	_entries.push_back(
		FragEntry { child }
	);

#line 443 "frag.md"
;
		return *this;
	}

#line 541 "frag.md"

	void serializeFrag(
		const Frag &frag,
		std::ostream &out,
		Write_State &state
	) {
		
#line 569 "frag.md"

	for (const auto &entry : frag) {
		if (entry.frag) {
			serializeFrag(
				*entry.frag, out,
				state
			);
		}
		out << entry.str(state);
	}

#line 547 "frag.md"
;
	}

#line 555 "frag.md"

	void serializeFrag(
		const Frag &f,
		std::ostream &out
	) {
		Write_State state { f };
		return serializeFrag(
			f, out, state
		);
	}

#line 585 "frag.md"

	bool check_frag(
		const Frag &f,
		std::istream &in,
		Write_State &state
	) {
		
#line 612 "frag.md"

	for (const auto &entry : f) {
		if (entry.frag) {
			if (!check_frag(
				*entry.frag, in,
				state
			)) {
				return false;
			}
		}
		for (const auto &i : entry.str(state)) {
			if (in.get() != i) {
				return false;
			}
		}
	}

#line 591 "frag.md"
;
		return true;
	}

#line 598 "frag.md"

	bool check_frag(
		const Frag &f,
		std::istream &in
	) {
		Write_State state { f };
		return check_frag(
			f, in, state
		);
	}

#line 632 "frag.md"

	void testFrag(
		const Frag &frag,
		const std::string &expected
	) {
		
#line 652 "frag.md"

	std::ostringstream buffer;
	serializeFrag(frag, buffer);
	ASSERT(buffer.str() == expected);

#line 637 "frag.md"
;
	}

#line 662 "frag.md"

	void addStringToFrag(
		Frag *frag,
		const std::string &str
	) {
		frag->add(
			str, std::string {}, 0
		);
	}

#line 768 "frag.md"

	using FragMap =
		std::map<std::string, Frag>;

#line 23 "frag.md"
;

#line 161 "read.md"
;
	class Input {
		public:
			
#line 246 "read.md"

	Input(const std::string &path):
		_path { path }
	{}

#line 256 "read.md"

	Input(const Input &) = delete;
	Input(Input &&) = default;

#line 265 "read.md"

	Input &operator=(
		const Input &
	) = delete;
	Input &operator=(
		Input &&
	) = default;

#line 278 "read.md"

	const std::string &path() const {
		return _path;
	}

#line 115 "blocks.md"

	std::vector<Block> blocks;

#line 117 "input.md"

	FragMap frags;

#line 164 "read.md"
;
		private:
			std::string _path;
	};

#line 147 "read.md"
;
	class Open_Input {
		public:
			
#line 194 "read.md"

	Open_Input(const std::string &path):
		_input { path },
		_file { path.c_str() }
	{}

#line 204 "read.md"

	Open_Input(
		const Open_Input &
	) = delete;
	Open_Input(
		Open_Input &&
	) = default;

#line 217 "read.md"

	Open_Input &operator=(
		const Open_Input &
	) = delete;
	Open_Input &operator=(
		Open_Input &&
	) = default;

#line 230 "read.md"

	Input &input() { return _input; }

#line 237 "read.md"

	const Input &input() const {
		return _input;
	}

#line 287 "read.md"

	void read_line(std::string &line) {
		if (_file.is_open()) {
			
#line 300 "read.md"

	if (std::getline(_file, line)) {
		
#line 144 "input.md"

	++_line;

#line 302 "read.md"
;
		return;
	}
	_file.close();

#line 290 "read.md"
;
		}
		throw No_More_Lines {};
	}

#line 33 "blocks.md"

	Read_State state = RS::new_element;

#line 135 "input.md"

	int line() const {
		return _line;
	}

#line 150 "read.md"
;
		private:
			
#line 183 "read.md"

	Input _input;
	std::ifstream _file;
	// char _last;

#line 128 "input.md"

	int _line = 0;

#line 152 "read.md"
;
	};

#line 311 "index.md"

	using SI =
		std::string::const_iterator;

#line 21 "read.md"
;
	class Inputs {
		public:
			
#line 43 "read.md"

	void read_line(std::string &line);

#line 109 "read.md"

	void clear() {
		
#line 235 "input.md"

	_used.clear();
	_open.clear();
	if (_paths.empty()) {
		if (std::filesystem::exists("index.md")) {
			_paths.push_back("index.md");
		} else if (std::filesystem::exists("index.x")) {
			_paths.push_back("index.x");
		}
	}
	_current_path = _paths.begin();

#line 111 "read.md"
;
	}

#line 21 "input.md"

	auto &cur() {
		ASSERT (! _open.empty());
		return _open.back();
	}

#line 31 "input.md"

	auto begin() {
		return _used.begin();
	}

#line 40 "input.md"

	auto end() {
		return _used.end();
	}

#line 49 "input.md"

	auto size() const {
		return _used.size();
	}

#line 57 "input.md"

	void push(const std::string &path) {
		_used.push_back({ path });
		_open.push_back({ path });
	}

#line 69 "input.md"

	void add(const std::string &path) {
		_paths.push_back(path);
		push(path);
	}

#line 86 "input.md"

	bool has(
		const std::string &name
	) const {
		
#line 101 "input.md"

	for (const auto &j : _used) {
		if (j.path() == name) {
			return true;
		}
	}

#line 90 "input.md"
;
		return false;
	}

#line 151 "input.md"

	Frag *find_local(const std::string &name) {
		ASSERT(! _open.empty());
		Input &i = _open.back().input();
		auto f = i.frags.find(name);
		if (f == i.frags.end()) { return nullptr; }
		return &f->second;
	}

#line 163 "input.md"

	Frag *add_local(const std::string &name) {
		ASSERT(! _open.empty());
		Input &i = _open.back().input();
		return &i.frags.insert({ name, name }).first->second;
	}

#line 173 "input.md"

	Frag *get_local(const std::string &name) {
		Frag *result = find_local(name);
		if (! result) {
			result = add_local(name);
		}
		return result;
	}

#line 185 "input.md"

	Frag *find_global(const std::string &name) {
		if (_open.size() > 1) {
			auto i = _open.end() - 2;
			for (;; --i) {
				auto &fs = i->input().frags;
				auto f = fs.find(name);
				if (f != fs.end()) {
					return &f->second;
				}
				if (i == _open.begin()) { break; }
			}
		}
		auto f = _root.find(name);
		if (f == _root.end()) { return nullptr; }
		return &f->second;

	}

#line 207 "input.md"

	Frag *add_global(const std::string &name) {
		return &_root.insert({ name, name }).first->second;
	}

#line 215 "input.md"

	const FragMap &root() const {
		return _root;
	}

#line 223 "input.md"

	Frag *get_global(const std::string &name) {
		Frag *result = find_global(name);
		if (! result) {
			result = add_global(name);
		}
		return result;
	}

#line 24 "read.md"
;
		private:
			
#line 313 "read.md"

	std::vector<std::string> _paths;
	std::vector<std::string>::
		const_iterator _current_path;

#line 323 "read.md"

	std::vector<Open_Input> _open;
	std::vector<Input> _used;

#line 13 "input.md"

	FragMap _root;

#line 26 "read.md"
;
	};

#line 51 "read.md"

	void Inputs::read_line(
		std::string &line
	) {
		
#line 339 "read.md"

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
		
#line 363 "read.md"

	auto &f { _open.back().input() };
	
#line 376 "read.md"

	if (f.blocks.empty()) {
		f.blocks.push_back({
			RS::header,
			{ "EMPTY FILE" }, {}
		});
	}

#line 365 "read.md"
;
	for (auto &i : _used) {
		if (i.path() == f.path()) {
			i = std::move(f);
			break;
		}
	}

#line 353 "read.md"
;
		_open.pop_back();
	}
	throw No_More_Lines {};

#line 55 "read.md"
;
	}

#line 80 "read.md"

	Inputs inputs;

#line 91 "read.md"

	
#line 272 "index.md"

	int blockLimit = -1;

#line 320 "index.md"

	void process_char(
		Frag *frag, char ch
	) {
		
#line 359 "index.md"

	if (frag) {
		frag->add(
			ch, inputs.cur().input().path(),
			inputs.cur().line()
		);
	}

#line 324 "index.md"
;
	}

#line 442 "index.md"

	inline void expand_cmd_arg(
		Frag *f, const std::string &arg
	) {
		auto b = arg.begin();
		auto e = arg.end();
		
#line 472 "index.md"

	while (b != e) {
		auto x = std::find(b, e, '@');
		
#line 490 "index.md"

	f->add(
		std::string { b, x },
		inputs.cur().input().path(),
		inputs.cur().line()
	);

#line 475 "index.md"
;
		if (x != e) {
			b = x + 1;
			
#line 501 "index.md"

	if (b != e) {
		f->add(
			*b,
			inputs.cur().input().path(),
			inputs.cur().line()
		);
		++b;
	}

#line 478 "index.md"
;
		} else {
			b = e;
		}
	}

#line 448 "index.md"
;
	}

#line 515 "index.md"

	#define ASSERT_NOT_FRAG() \
		ASSERT_MSG(! frag, '@' << \
			name << "(" << arg << \
			") in frag [" << \
			frag->name << ']' \
		)

#line 527 "index.md"

	#define CHECK_NOT_DEFINED() \
		if (isPopulatedFrag(frag)) { \
			WARN_MSG("frag [" << arg << \
				"] already defined" \
			); \
		}

#line 552 "index.md"

	#define ASSERT_FRAG() \
		ASSERT_MSG(frag, '@' << \
			name << "(" << arg << \
			") in frag [" << \
			frag->name << ']' \
		)

#line 586 "index.md"

	#define CHECK_DEFINED() \
		if (! isPopulatedFrag(frag)) { \
			WARN_MSG("frag [" << arg << \
				"] not defined" \
			); \
		}

#line 92 "read.md"
;
	void read_sources() {
		
#line 119 "read.md"
 {
	inputs.clear();
	
#line 349 "index.md"

	Frag *frag { nullptr };

#line 121 "read.md"
;
	std::string line;
	try { for (;;) {
		inputs.read_line(line);
		
#line 41 "blocks.md"

	do {
		auto &state = inputs.cur().state;
		
#line 122 "blocks.md"

	auto &blocks =
		inputs.cur().input().blocks;

#line 44 "blocks.md"
;
		
#line 156 "blocks.md"

	if (
		line == "```" &&
		state == RS::new_element
	) {
		state = RS::code;
		
#line 182 "blocks.md"

	blocks.push_back({
		RS::code, {}, {}, 0
	});

#line 162 "blocks.md"
;
		break;
	}

#line 169 "blocks.md"

	if (state == RS::code) {
		if (line == "```") {
			state = RS::after_code;
		} else {
			
#line 190 "blocks.md"

	blocks.back().value.push_back(
		line
	);

#line 174 "blocks.md"
;
		}
		break;
	}

#line 45 "blocks.md"
;
		
#line 57 "blocks.md"

	if (line.empty()) {
		state = RS::new_element;
		break;
	}

#line 46 "blocks.md"
;
		
#line 74 "blocks.md"

	if (
		line[0] == '#' &&
		state == RS::new_element
	) {
		state = RS::header;
		
#line 130 "blocks.md"

	auto b { line.begin() };
	auto e { line.end() };
	int l { 0 };
	for (
		; b != e && *b == '#'; ++b, ++l
	) {}
	for (; b != e && *b == ' '; ++b) {}
	blocks.push_back({
		RS::header, {{ b, e }}, {}, l
	});

#line 80 "blocks.md"
;
		break;
	}

#line 204 "blocks.md"

	if (line[0] == '*') {
		if (
			state == RS::header ||
			state == RS::after_code ||
			state == RS::notes
		) {
			state = RS::notes;
			
#line 232 "blocks.md"

	auto b { line.begin() };
	auto e { line.end() };
	for (;
		b != e &&
			(*b == '*' || *b == ' ');
		++b
	) {}
	blocks.back().notes.push_back(
		{ b, e }
	);

#line 212 "blocks.md"
;
			break;
		}
	}

#line 220 "blocks.md"

	if (
		line[0] == ' ' &&
		state == RS::notes
	) {
		
#line 247 "blocks.md"

	blocks.back().notes.back() +=
		line;

#line 225 "blocks.md"
;
		break;
	}

#line 260 "blocks.md"

	if (line[0] != ' ') {
		if (
			state == RS::new_element ||
			state == RS::para
		) {
			
#line 276 "blocks.md"

	if (state == RS::new_element) {
		if (blocks.empty() || blocks.back().state != RS::para) {
			blocks.push_back({
				RS::para, {}, {}, 0
			});
		}
		blocks.back().value.push_back(line);
	}

#line 266 "blocks.md"
;
			
#line 289 "blocks.md"

	if (state == RS::para) {
		blocks.back().value.back() +=
			" " + line;
	}

#line 267 "blocks.md"
;
			state = RS::para;
			break;
		}
	}

#line 47 "blocks.md"
;
		
#line 89 "blocks.md"

	std::cerr << "!! " << line << '\n';

#line 48 "blocks.md"
;
	} while (false);

#line 331 "index.md"

	auto end = line.cend();
	for (
		auto i = line.cbegin();
		i != end; ++i
	) {
		
#line 374 "index.md"

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
			
#line 398 "index.md"

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
		
#line 419 "index.md"

	i = ae;
	bool outside = ! frag;
	do {
		if (outside && ! blockLimit) {
			break;
		}
		
#line 539 "index.md"

	if (name == "def") {
		ASSERT_NOT_FRAG();
		frag = inputs.get_local(arg);
		CHECK_NOT_DEFINED();
		break;
	}

#line 564 "index.md"

	if (name == "end" || name == "End") {
		ASSERT_FRAG();
		
#line 576 "index.md"

	ASSERT_MSG(frag->name == arg,
		"closing [" << arg <<
		"] != [" << frag->name << ']'
	);

#line 567 "index.md"
;
		frag = nullptr;
		break;
	}

#line 598 "index.md"

	if (name == "add") {
		ASSERT_NOT_FRAG();
		frag = inputs.get_local(arg);
		CHECK_DEFINED();
		break;
	}

#line 611 "index.md"

	if (name == "put") {
		ASSERT_MSG(frag,
			"@put" << "(" << arg <<
				") not in frag"
		);
		Frag *sub = inputs.get_local(arg);
		if (sub) {
			
#line 632 "index.md"

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

#line 619 "index.md"
;
			sub->addExpand();
			frag->add(sub);
		}
		break;
	}

#line 649 "index.md"

	if (name == "inc") {
		ASSERT_MSG(! frag,
			"include in frag [" << frag->name << ']'
		);
		if (! inputs.has(arg)) {
			inputs.push(arg);
		}
		break;
	}

#line 666 "index.md"

	if (name == "mul") {
		ASSERT_MSG(frag,
			"@mul not in frag"
		);
		Frag *sub = inputs.get_local(arg);
		if (sub) {
			
#line 684 "index.md"

	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}

#line 673 "index.md"
;
			sub->addMultiple();
			frag->add(sub);
		}
		break;
	}

#line 697 "index.md"

	if (name == "Def") {
		ASSERT_MSG(! frag, "@Def in frag [" << frag->name << ']');
		frag = inputs.get_global(arg);
		if (isPopulatedFrag(frag)) {
			std::cerr << "Frag [" << arg << "] already defined\n";
		}
		break;
	}

#line 712 "index.md"

	if (name == "Add") {
		ASSERT_MSG(! frag, "@Add in frag [" << frag->name << ']');
		frag = inputs.get_global(arg);
		if (! isPopulatedFrag(frag)) {
			std::cerr << "Frag [" << arg << "] not defined\n";
		}
		break;
	}

#line 729 "index.md"

	if (name == "rep") {
		ASSERT_MSG(! frag,
			"@rep in frag [" << frag->name << ']'
		);
		frag = inputs.get_local(arg);
		
#line 757 "index.md"

	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();

#line 735 "index.md"
;
		break;
	}

#line 743 "index.md"

	if (name == "Rep") {
		ASSERT_MSG(! frag,
			"@Rep in frag [" << frag->name << ']'
		);
		frag = inputs.get_global(arg);
		
#line 757 "index.md"

	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();

#line 749 "index.md"
;
		break;
	}

#line 769 "index.md"

	if (name == "Put") {
		ASSERT_MSG(frag,
			"@Put not in frag"
		);
		Frag *sub = inputs.get_global(arg);
		if (sub) {
			
#line 632 "index.md"

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

#line 776 "index.md"
;
			sub->addExpand();
			frag->add(sub);
		}
		break;
	}

#line 787 "index.md"

	if (name == "Mul") {
		ASSERT_MSG(frag,
			"@Mul not in frag"
		);
		Frag *sub = inputs.get_global(arg);
		if (sub) {
			
#line 684 "index.md"

	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}

#line 794 "index.md"
;
			sub->addMultiple();
			frag->add(sub);
		}
		break;
	}

#line 805 "index.md"

	if (name == "priv") {
		ASSERT_MSG(frag,
			"@priv not in frag"
		);
		
#line 828 "index.md"

	std::hash<std::string> h;
	auto cur {
		h(inputs.cur().input().path() +
			':' + arg) &
				0x7fffffff
	};

#line 841 "index.md"

	std::ostringstream hashed;
	hashed << "_private_" <<
		cur << '_' <<
		arg;
	frag->add(
		hashed.str(),
		inputs.cur().input().path(),
		inputs.cur().line()
	);

#line 810 "index.md"
;
		break;
	}

#line 858 "index.md"

	if (name == "magic") {
		ASSERT_MSG(frag,
			"@magic not in frag"
		);
		
#line 871 "index.md"

	std::hash<std::string> h;
	auto cur {
		h(inputs.cur().input().path() +
			':' + arg) &
				0x7fffffff
	};

#line 883 "index.md"

	std::ostringstream value;
	value << cur;
	frag->add(
		value.str(),
		inputs.cur().input().path(),
		inputs.cur().line()
	);

#line 863 "index.md"
;
		break;
	}

#line 426 "index.md"
;
		
#line 455 "index.md"

	if (frag) {
		expand_cmd_arg(frag, arg);
	}

#line 427 "index.md"
;
	} while (false);
	if (blockLimit && outside && frag) {
		--blockLimit;
	}

#line 409 "index.md"
;
		continue;
	}

#line 387 "index.md"
;
		}
	}

#line 337 "index.md"
;
		process_char(frag, *i);
	}
	process_char(frag, '\n');

#line 125 "read.md"
;
	} }
	catch (const No_More_Lines &) {}
} 
#line 94 "read.md"
;
	}

#line 6 "read.md"
;

#line 245 "index.md"

	std::string stylesheet {
		"slides/slides.css"
	};

#line 901 "index.md"

	
#line 999 "index.md"

	std::string file_name(const Frag &f) {
		return f.name.substr(6);
	}

#line 1008 "index.md"

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

#line 902 "index.md"

	void files_write() {
		
#line 920 "index.md"

	for (auto &i : inputs.root()) {
		const Frag *frag {
			&i.second
		};
		
#line 949 "index.md"
 {
	if (frag->isFile()) {
		
#line 1029 "index.md"

	if (file_changed(*frag)) {
		std::ofstream out(
			file_name(*frag).c_str()
		);
		serializeFrag(*frag, out);
	}

#line 951 "index.md"
;
	}
} 
#line 959 "index.md"
 {
	int sum {
		frag->expands()
			+ frag->multiples()
	};
	if (sum <= 0) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not called\n";
	}
} 
#line 974 "index.md"

	if (frag->multiples() == 1) {
		std::cerr <<
			"multiple frag [" <<
			frag->name <<
			"] only used once\n";
	}

#line 987 "index.md"

	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not populated\n";
	}

#line 925 "index.md"
;
	}

#line 933 "index.md"

	for (auto &j : inputs) {
		for (auto &i : j.frags) {
			const Frag *frag {
				&i.second
			};
			
#line 949 "index.md"
 {
	if (frag->isFile()) {
		
#line 1029 "index.md"

	if (file_changed(*frag)) {
		std::ofstream out(
			file_name(*frag).c_str()
		);
		serializeFrag(*frag, out);
	}

#line 951 "index.md"
;
	}
} 
#line 959 "index.md"
 {
	int sum {
		frag->expands()
			+ frag->multiples()
	};
	if (sum <= 0) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not called\n";
	}
} 
#line 974 "index.md"

	if (frag->multiples() == 1) {
		std::cerr <<
			"multiple frag [" <<
			frag->name <<
			"] only used once\n";
	}

#line 987 "index.md"

	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not populated\n";
	}

#line 939 "index.md"
;
		}
	}

#line 904 "index.md"
;
	}

#line 1041 "index.md"

	
#line 1088 "index.md"

	bool no_cmds = false;

#line 1042 "index.md"
;
	void files_process() {
		
#line 1061 "index.md"

	for (auto &i : inputs.root()) {
		const Frag *frag {
			&i.second
		};
		
#line 1095 "index.md"
 {
	const std::string cmd { frag->cmd() };
	if (cmd.size()) {
		
#line 1105 "index.md"

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

#line 1098 "index.md"
;
	}
} 
#line 1066 "index.md"
;
	}

#line 1073 "index.md"

	for (auto &j : inputs) {
		for (auto &i : j.frags) {
			const Frag *frag {
				&i.second
			};
			
#line 1095 "index.md"
 {
	const std::string cmd { frag->cmd() };
	if (cmd.size()) {
		
#line 1105 "index.md"

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

#line 1098 "index.md"
;
	}
} 
#line 1079 "index.md"
;
		}
	}

#line 1044 "index.md"
;
	}

#line 6 "html.md"

	
#line 52 "html.md"

	enum class HtmlState {
		nothing,
		inSlide,
		afterSlide,
		afterSlides
		
#line 313 "html.md"

	, inCode

#line 919 "html.md"

	, inNotes

#line 1071 "html.md"

	, inPara

#line 58 "html.md"

	};

#line 69 "html.md"

	struct HtmlStatus {
		
#line 78 "html.md"

	HtmlState state = HtmlState::nothing;

#line 71 "html.md"

	};

#line 175 "html.md"
 
	void writeOneEscaped(
		std::ostream &out, char ch
	) {
		switch (ch) {
			
#line 208 "html.md"

	case '<':
		out << "&lt;";
		break;
	case '>':
		out << "&gt;";
		break;
	case '&':
		out << "&amp;";
		break;

#line 180 "html.md"

			default:
				out << ch;
		}
	}

#line 192 "html.md"

	void writeEscaped(
		std::ostream &out,
		const std::string &str
	) {
		for (char ch : str) {
			writeOneEscaped(out, ch);
		}
	}

#line 223 "html.md"

	
#line 448 "html.md"

	void span_str(
		std::ostream &out,
		const char *cls,
		const std::string &s
	) {
		out << "<span class=\"" <<
			cls << "\">";
		writeEscaped(out, s);
		out << "</span>";
	}

#line 752 "html.md"

	using Set = std::set<std::string>;

	bool isKeyword(const std::string &s) {
		static Set reserved {
			
#line 769 "html.md"

	"break", "case", "catch", "continue",
	"default", "delete", "else", "for",
	"if", "in", "new", "return", "static",
	"switch", "try", "typeof", "while",
	"class", "public", "private",
	"template", "typename", "using",
	"function", "throw", "namespace",
	"once", "constexpr", "volatile"

#line 757 "html.md"

		};
		return
			reserved.find(s) !=
				reserved.end() ||
					(s.size() && s[0] == '#');
	}

#line 783 "html.md"

	bool isType(const std::string &s) {
		
#line 793 "html.md"

	static Set reserved {
		
#line 808 "html.md"

	"FILE", "auto", "bool", "char",
	"const", "enum", "extern", "int",
	"let", "long", "signed", "struct",
	"union", "unsigned", "void", "double",
	"string", "std", "ifstream",
	"istream", "ofstream", "ostream",
	"vector", "map", "list"

#line 795 "html.md"

	};
	if (reserved.find(s) !=
		reserved.end()
	) {
		return true;
	}

#line 821 "html.md"

	if (s.size() >= 2) {
		if (isupper(s[0]) &&
			islower(s[1])
		) {
			return true;
		}
	}

#line 785 "html.md"
;
		return false;
	}

#line 836 "html.md"

	bool isNum(const std::string &s) {
		static Set reserved {
			"EOF", "NULL", "nullptr",
			"false", "null", "true",
			"undefined"
		};
		if (std::isdigit(s[0])) {
			return true;
		}
		return reserved.find(s) !=
			reserved.end();
	}

#line 855 "html.md"

	void process_ident(
		std::ostream &out,
		const std::string ident,
		char w
	) {
		if (isKeyword(ident)) {
			span_str(out, "keyword", ident);
		} else if (w == '(') {
			span_str(out, "fn", ident);
		
#line 878 "html.md"

	} else if (isType(ident)) {
		span_str(out, "type", ident);
	} else if (isNum(ident)) {
		span_str(out, "num", ident);

#line 865 "html.md"

		} else {
			span_str(out, "var", ident);
		}
	}

#line 889 "html.md"

	void writeMacroClass(
		std::ostream &out,
		const char *name
	) {
		out << "<span class=\"" <<
			name << "\">";
	}

#line 902 "html.md"

	void writeMacroHeader(
		std::ostream &out,
		const std::string &name
	) {
		writeMacroClass(out, "macro");
		out << '@' << name << "(<span class=\"name\">";
	}

#line 224 "html.md"
;
	void process_code(
		std::ostream &out,
		SI begin, SI end
	) {
		
#line 384 "html.md"

	int indent = 0;
	while (
		begin != end && *begin == '\t'
	) {
		++indent; ++begin;
	}
	if (indent) {
		out << "<span class=\"in"
			<< indent
			<< "\"></span>";
	}

#line 402 "html.md"

	for (; begin != end; ++begin) {
		
#line 414 "html.md"

	if (
		*begin == '`' ||
		*begin == '\'' ||
		*begin == '"'
	) {
		
#line 428 "html.md"

	auto w = begin + 1;
	while (w != end && *w != *begin) {
		if (*w == '\\') {
			++w;
			if (w == end) { break; }
		}
		++w;
	}
	if (w == end) {
		writeOneEscaped(out, *begin);
		continue;
	}

#line 465 "html.md"

	std::string name {begin, w + 1};
	span_str(out, "str", name);
	begin = w;

#line 420 "html.md"
;
		continue;
	}

#line 475 "html.md"

	if (*begin == '@') {
		auto nb = begin + 1;
		auto ne = nb;
		while (ne != end && *ne != '(') {
			if (! isalpha(*ne)) {
				ne = end;
				break;
			}
			++ne;
		}
		if (ne != end) {
			std::string name {nb, ne};
			auto ab = ne + 1;
			auto ae = ab;
			
#line 497 "html.md"

	while (ae != end && *ae != ')') {
		if (*ae == '@') {
			++ae;
			if (ae == end) { break; }
		}
		++ae;
	}
	if (ae != end) {
		std::string arg {ab, ae};
		
#line 514 "html.md"

	do {
		
#line 536 "html.md"

	static Set macros = {
		"def", "end", "add", "put", "mul",
		"Def", "Add", "Mul", "rep", "Rep",
		"Put", "End"
	};
	if (
		macros.find(name) != macros.end()
	) {
		writeMacroHeader(out, name);
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}

#line 554 "html.md"

	if (name == "inc") {
		
#line 563 "html.md"

	auto ext = arg.find_last_of('.');
	ASSERT_MSG(
		ext != std::string::npos,
		"no period"
	);
	writeMacroHeader(out, name);
	out << "<a href=\"" <<
		arg.substr(0, ext) <<
		".html\">";
	out << arg <<
		"</a></span>)</span>";

#line 556 "html.md"
;
		break;
	}

#line 581 "html.md"

	if (name == "s" || name == "str") {
		writeMacroClass(out, "str");
		for (auto x = arg.begin();; ++x) {
			if (*x == '@') {
				++x;
			}
			if (x == arg.end()) { break; }
			writeOneEscaped(out, *x);
		}
		out << "</span>";
		break;
	}

#line 598 "html.md"

	if (name == "f" || name == "fn") {
		writeMacroClass(out, "fn");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 610 "html.md"

	if (name == "v" || name == "var") {
		writeMacroClass(out, "var");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 622 "html.md"

	if (name == "k" || name == "key") {
		writeMacroClass(out, "keyword");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 634 "html.md"

	if (name == "n" || name == "num") {
		writeMacroClass(out, "num");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 646 "html.md"

	if (name == "t" || name == "typ") {
		writeMacroClass(out, "type");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 658 "html.md"

	if (name == "b" || name == "br") {
		writeMacroClass(out, "virt");
		out << "</span><br/>";
		break;
	}

#line 669 "html.md"

	if (name == "priv") {
		writeMacroClass(out, "var");
		out << "@priv(<span>";
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}

#line 682 "html.md"

	if (name == "magic") {
		writeMacroClass(out, "num");
		out << "@magic(<span>";
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}

#line 516 "html.md"
;
		
#line 524 "html.md"

	std::cerr << "unknown macro @" <<
		name << '(' << arg << ")\n";
	writeOneEscaped(out, '@');
	writeEscaped(out, name);
	writeOneEscaped(out, '(');
	writeEscaped(out, arg);
	writeOneEscaped(out, ')');

#line 517 "html.md"
;
	} while (false);
	begin = ae;

#line 507 "html.md"
;
		continue;
	}

#line 490 "html.md"
;
		}
	}

#line 702 "html.md"

	auto w = begin;
	
#line 715 "html.md"

	while (w != end && (
		std::isalnum(*w) ||
			*w == '_' || *w == '$' ||
			*w == '#'
	)) {
		++w;
	}

#line 704 "html.md"
;
	if (w != begin) {
		
#line 729 "html.md"

	std::string ident {begin, w};
	begin = w - 1;
	process_ident(
		out, ident,
		w != end ? *w : ' '
	);

#line 706 "html.md"
;
		continue;
	}

#line 404 "html.md"
;
		writeOneEscaped(out, *begin);
	}

#line 229 "html.md"
;
	}

#line 238 "html.md"

	void process_content(
		std::ostream &out,
		SI begin, SI end
	) {
		
#line 978 "html.md"

	for(; begin != end; ++begin) {
		
#line 989 "html.md"

	if (*begin == '`') {
		
#line 998 "html.md"

	auto w = begin + 1;
	while (w != end && *w != '`') {
		++w;
	}
	if (w != end) {
		out << "<code>";
		process_code(out, begin + 1, w);
		out << "</code>";
		begin = w;
		continue;
	}

#line 991 "html.md"
;
	}

#line 1016 "html.md"

	if (
		*begin == '*' &&
		(begin + 1) != end &&
		*(begin + 1) == '*'
	) {
		
#line 1029 "html.md"

	auto w = begin + 2;
	while (
		w != end && (w + 1) != end &&
		(*w != '*' || *(w + 1) != '*')
	) {
		++w;
	}

#line 1042 "html.md"

	if (
		w != end && (w + 1 ) != end &&
		*w == '*' && *(w + 1) == '*'
	) {
		
#line 1056 "html.md"

	out << "<b>";
	writeEscaped(
		out, std::string {begin + 2, w}
	);
	out << "</b>";
	begin = w + 1;

#line 1047 "html.md"
;
		continue;
	}

#line 1022 "html.md"
;
	}

#line 980 "html.md"
;
		writeOneEscaped(out, *begin);
	}

#line 243 "html.md"
;
	}

#line 7 "html.md"

	void write_html() {
		for (auto &cur : inputs) {
			
#line 27 "html.md"

	const std::string &name { cur.path() };
	auto ext { name.rfind('.') };
	if (ext == std::string::npos) {
		ext = name.size();
	}
	std::string outPath {
		name.substr(0, ext) +
		".html"
	};
	std::ofstream out { outPath.c_str() };
	
#line 45 "html.md"

	
#line 93 "html.md"

	HtmlStatus status;
	for (const auto &b : cur.blocks) {
		
#line 105 "html.md"

	if (b.state == RS::header) {
		
#line 154 "html.md"

	
#line 261 "html.md"

	switch (status.state) {
		case HtmlState::nothing: {
			
#line 281 "html.md"

	out << "<!doctype html>\n";
	out << "<html lang=\"de\">\n";
	out << "<head>\n";
	
#line 295 "html.md"

	out << "<meta charset=\"utf-8\">\n";
	out << "<title>";
	writeEscaped(out, b.value[0]);
	out << "</title>\n";
	out << "<link rel=\"stylesheet\" "
		"type=\"text/css\" href=\""
		<< stylesheet << "\">";

#line 285 "html.md"
;
	out << "</head>\n";
	out << "<body>\n";

#line 264 "html.md"
;
			break;
		}
		case HtmlState::inSlide: {
			out << "</div>\n";
			// fallthrough
		}
		default: {
			out << "</div>\n";
		}
	}

#line 155 "html.md"
;
	
#line 251 "html.md"
 {
	out << "<h" << b.level << '>';
	const auto &n = b.value[0];
	process_content(out, n.begin(), n.end());
	out << "</h" << b.level << ">\n";
} 
#line 156 "html.md"
;
	out << "<div class=\"slides\">\n";
	out << "<div>\n";
	out << "<div>\n";
	
#line 251 "html.md"
 {
	out << "<h" << b.level << '>';
	const auto &n = b.value[0];
	process_content(out, n.begin(), n.end());
	out << "</h" << b.level << ">\n";
} 
#line 160 "html.md"
;
	out << "</div>\n";
	status.state = HtmlState::inSlide;
	for (const auto &note : b.notes) {
		
#line 938 "html.md"

	auto end = note.end();
	auto begin = note.begin();

#line 946 "html.md"

	if (
		status.state != HtmlState::inNotes
	) {
		
#line 963 "html.md"

	if (
		status.state != HtmlState::inSlide
	) {
		out << "<div>\n";
	}
	status.state = HtmlState::inNotes;
	out << "<ul><li>\n";

#line 950 "html.md"
;
	} else {
		out << "</li><li>\n";
	}
	process_content(out, begin, end);
	out << '\n';

#line 164 "html.md"
;
	}
	
#line 927 "html.md"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1079 "html.md"

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}

#line 166 "html.md"
;
	
#line 142 "html.md"

	out << "</div>\n";
	status.state = HtmlState::afterSlide;

#line 167 "html.md"
;

#line 107 "html.md"
;
	}

#line 113 "html.md"

	if (b.state == RS::code) {
		
#line 320 "html.md"

	if (
		status.state == HtmlState::afterSlides
	) {
		out << "<div class=\"slides\">\n";
	}
	if (
		status.state == HtmlState::inSlide
	) {
		out << "</div>\n";
	}
	out << "<div><div>\n";
	out << "<code>\n";
	status.state = HtmlState::inCode;

#line 115 "html.md"
;
		for (const auto &code : b.value) {
			
#line 373 "html.md"

	process_code(
		out, code.begin(), code.end()
	);
	out << "<br/>\n";

#line 117 "html.md"
;
		}
		
#line 340 "html.md"

	out << "</code></div>\n";
	status.state = HtmlState::inSlide;

#line 119 "html.md"
;
		for (const auto &note : b.notes) {
			
#line 938 "html.md"

	auto end = note.end();
	auto begin = note.begin();

#line 946 "html.md"

	if (
		status.state != HtmlState::inNotes
	) {
		
#line 963 "html.md"

	if (
		status.state != HtmlState::inSlide
	) {
		out << "<div>\n";
	}
	status.state = HtmlState::inNotes;
	out << "<ul><li>\n";

#line 950 "html.md"
;
	} else {
		out << "</li><li>\n";
	}
	process_content(out, begin, end);
	out << '\n';

#line 121 "html.md"
;
		}
		
#line 927 "html.md"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1079 "html.md"

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}

#line 123 "html.md"
;
		
#line 142 "html.md"

	out << "</div>\n";
	status.state = HtmlState::afterSlide;

#line 124 "html.md"
;
	}

#line 130 "html.md"

	if (b.state == RS::para) {
		for (const auto &para : b.value) {
			
#line 1092 "html.md"

	if (status.state == HtmlState::afterSlide) {
		out << "</div>\n";
	}
	if (
		status.state != HtmlState::inPara
	) {
		out << "<p>";
		status.state = HtmlState::inPara;
	}
	process_content(
		out, para.begin(), para.end()
	);
	out << '\n';

#line 133 "html.md"
;
			
#line 927 "html.md"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1079 "html.md"

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}

#line 134 "html.md"
;
		}
		
#line 927 "html.md"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1079 "html.md"

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}

#line 136 "html.md"
;
	}

#line 96 "html.md"
;
	}

#line 348 "html.md"

	if (
		status.state == HtmlState::inCode
	) {
		std::cerr <<
			"unterminated code block\n";
	}

#line 362 "html.md"

	if (
		status.state != HtmlState::nothing
	) {
		out << "</body>\n</html>\n";
	}

#line 46 "html.md"
;

#line 38 "html.md"
;
	out.close();

#line 10 "html.md"
;
		}
	}

#line 4 "view.md"

	bool interactive = false;
	std::vector<Input>::iterator curInput;
	std::vector<Block>::iterator curBlock;

#line 12 "view.md"

	bool write_files = true;

#line 18 "view.md"

	bool process_files = true;

#line 24 "view.md"

	bool html_files = true;

#line 60 "view.md"

	void draw_block() {
		
#line 145 "view.md"

	if (curBlock->state == RS::header) {
		int i = 0;
		for (const auto &l : curBlock->value) {
			std::cout << ++i << ": ";
			for (int i = 0; i < curBlock->level; ++i) {
				std::cout << '#';
			}
			std::cout << ' ' << l << "\n\n";
		}
	}

#line 160 "view.md"

	if (curBlock->state == RS::code) {
		std::cout << "```\n";
		int i = 0;
		for (const auto &l : curBlock->value) {
			std::cout << ++i << ": " << l << '\n';
		}
		std::cout << "```\n\n";
	}

#line 173 "view.md"

	if (curBlock->state == RS::para) {
		int i = 0;
		for (const auto &l : curBlock->value) {
			std::cout << ++i << ": " << l << "\n\n";
		}
	}

#line 184 "view.md"

	int j = 0;
	for (const auto &l : curBlock->notes) {
		std::cout << ++j << ": * " << l << '\n';
	}
	std::cout << '\n';

#line 62 "view.md"
;
	}

#line 77 "view.md"

	void draw_position() {
		
#line 194 "view.md"

	auto &bs { curInput->blocks };
	std::cout << curInput->path() << ':';
	int idx =
		(curBlock - bs.begin()) + 1;
	std::cout << idx;
	if (
		idx == static_cast<int>(bs.size())
	) {
		std::cout << " = $";
	}

#line 79 "view.md"
;
	}

#line 85 "view.md"

	void trim(std::string &s) {
		while (! s.empty() && (s[0] & 0xff) <= ' ') {
			s.erase(0, 1);
		}
	}

#line 4 "line.md"

	class Line {
		public:
			
#line 15 "line.md"

	int operator()(
		int cur, int end
	) const {
		int res {};
		
#line 42 "line.md"

	if (! *this) {
		res = cur;
	} else {
		res = _line;
		if (_relative) { res += cur; }
	}
	if (res < 0) { res = 0; }
	if (res > end) { res = end; }

#line 20 "line.md"
;
		return res;
	}

#line 34 "line.md"

	operator bool() const {
		return _line >= 0 ||  _relative;
	}

#line 64 "line.md"

	Line() = default;

#line 70 "line.md"

	static Line relative(int line) {
		return Line { line, true };
	}

#line 78 "line.md"

	static Line line(int line) {
		return Line { line, false };
	}

#line 86 "line.md"

	static Line begin() {
		return line(0);
	}

#line 100 "line.md"

	static const int max =
		std::numeric_limits<int>::max();
		
	static Line end() {
		return line(max);
	}

#line 7 "line.md"
;
		private:
			
#line 27 "line.md"

	int _line = -1;
	bool _relative = false;

#line 55 "line.md"

	Line(int line, bool relative):
		_line { line },
		_relative { relative }
	{}

#line 9 "line.md"
;
	};

#line 111 "line.md"

	
#line 4 "range.md"

	class Range {
		public:
			
#line 16 "range.md"

	Line prev() {
		return _prev ?: _last;
	}

#line 24 "range.md"

	Line last() {
		return _last;
	}

#line 32 "range.md"

	Range &operator<<(const Line &l) {
		_prev = _last;
		_last = l;
		return *this;
	}

#line 42 "range.md"

	operator bool() {
		return _last;
	}

#line 7 "range.md"
;
		private:
			Line _prev;
			Line _last;
	};

#line 112 "line.md"
;
	
#line 113 "range.md"

	Range range;

#line 113 "line.md"
;

#line 124 "line.md"

	int get_number(std::string &s) {
		int res = 0;
		while (
			! s.empty() && isdigit(s[0])
		) {
			res = res * 10 + s[0] - '0';
			s.erase(0, 1);
		}
		return res;
	}

#line 139 "line.md"

	Line get_line(std::string &s) {
		Line line {};
		do {
			
#line 152 "line.md"

	if (s[0] == '.') {
		s.erase(0, 1);
		line = Line::relative(0);
		break;
	}

#line 162 "line.md"

	if (s[0] == '+') {
		s.erase(0, 1);
		int n = get_number(s);
		line = Line::relative(n);
		break;
	}

#line 173 "line.md"

	if (s[0] == '-') {
		s.erase(0, 1);
		int n = -get_number(s);
		line = Line::relative(n);
		break;
	}

#line 184 "line.md"

	if (s[0] == '$') {
		line = Line::end();
		s.erase(0, 1);
		continue;
	}

#line 194 "line.md"

	if (isdigit(s[0])) {
		int n = get_number(s);
		line = Line::line(n);
		continue;
	}

#line 143 "line.md"
;
		} while (false);
		trim(s);
		return line;
	}

#line 4 "edit.md"

	void insert_before(
		const std::string &prefix,
		std::vector<std::string> &c
	) {
		
#line 49 "edit.md"

	int next = c.size();
	
#line 91 "range.md"

	if (range) {
		next = range.last()(
			Line::max, c.size() + 1
		) - 1;
		if (next < 0) { next = 0; }
		int p = range.prev()(
			Line::max, c.size() + 1
		) - 1;
		if (p < 0) { p = 0; }
		if (p < next) {
			c.erase(
				c.begin() + p,
				c.begin() + next
			);
			next = p;
		}
	}

#line 51 "edit.md"
;
	std::string l;
	for (;;) {
		std::cout << prefix << ' ' << (next + 1) << "? ";
		std::getline(std::cin, l);
		auto b = l.begin();
		auto e = l.end();
		while (b != e && *b <= ' ') { ++b; }
		std::string t { b, e };
		if (t.empty()) { continue; }
		if (t == ".") { break; }
		c.insert(c.begin() + next, l);
		++next;
	}
	draw_block();

#line 9 "edit.md"
;
	}

#line 5 "write.md"

	
#line 100 "write.md"

	std::string split(
		std::string &s, int width
	) {
		auto b { s.begin() };
		auto e { s.end() };
		while (b != e && *b == ' ') { ++b; }
		auto c = b;
		while (c != e) {
			auto t = c;
			while (t != e && *t == ' ') { ++t; }
			while (t != e && *t != ' ') { ++t; }
			if (c == b || t - b <= width) {
				c = t;
			} else {
				break;
			}
		}
		std::string res { b, c };
		s.erase(s.begin(), c);
		return res;
	}

#line 126 "write.md"

	void multi_write(
		std::ofstream &out,
		std::string str,
		std::string first_in,
		const std::string &other_in
	) {
		while (! str.empty()) {
			std::string p = split(str, 72 - first_in.size());
			out << first_in << p << '\n';
			first_in = other_in;
		}
	}

#line 6 "write.md"

	void write_x() {
		for (const auto &cur : inputs) {
			
#line 25 "write.md"

	std::ofstream out {
		cur.path().c_str()
	};

#line 33 "write.md"

	bool first = true;
	for (const auto &b : cur.blocks) {
		if (first) {
			first = false;
		} else { out << '\n'; }
		switch (b.state) {
			
#line 49 "write.md"

	case RS::header: {
		
#line 58 "write.md"

	for (const auto &n : b.value) {
		for (
			int i = 0; i < b.level; ++i
		) {
			out << '#';
		}
		out << ' ';
		out << n << '\n';
	}

#line 51 "write.md"
;
		break;
	}

#line 72 "write.md"

	case RS::code: {
		
#line 81 "write.md"

	out << "```\n";
	for (const auto &n: b.value) {
		out << n << '\n';
	}
	out << "```\n";

#line 74 "write.md"
;
		break;
	}

#line 91 "write.md"

	case RS::para: {
		
#line 143 "write.md"

	bool first = true;
	for (const auto &n: b.value) {
		if (first) {
			first = false;
		} else { out << '\n'; }
		multi_write(out, n, {}, {});
	}

#line 93 "write.md"
;
		break;
	}

#line 40 "write.md"
;
			default: ;
		}
		
#line 155 "write.md"

	for (const auto &n: b.notes) {
		multi_write(out, n, "* ", "  ");
	}

#line 43 "write.md"
;
	}

#line 9 "write.md"
;
		}
	}

#line 226 "write.md"

	bool is_prefix(
		const std::string &s,
		const std::string &p
	) {
		return s.size() >= p.size() &&
			std::equal(
				p.begin(), p.end(),
				s.begin()
			);
	}

#line 8 "add.md"

	void add_block(Read_State state) {
		
#line 51 "add.md"

	if (curInput != inputs.end()) {
		
#line 71 "add.md"

	if (
		curBlock !=
			curInput->blocks.end()
	) {
		++curBlock;
	}

#line 53 "add.md"
;
		int i = curBlock -
			curInput->blocks.begin();
		
#line 83 "add.md"

	curInput->blocks.insert(
		curBlock,
		{ state, { "REPLACE" }, {}, state == RS::header ? 1 : 0 }
	);

#line 56 "add.md"
;
		curBlock =
			curInput->blocks.begin() + i;
	} else {
		std::cerr << "! no file\n";
	}
	draw_block();

#line 10 "add.md"
;
	}

#line 29 "ncurses.md"

	#if HAVE_CURSES
		
#line 37 "ncurses.md"

	
#line 43 "ncurses.md"

	bool with_ncurses = false;

#line 77 "ncurses.md"

	class Ncurses_Handler {
		public:
			Ncurses_Handler() {
				
#line 91 "ncurses.md"

	setlocale(LC_CTYPE, "");
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();

#line 81 "ncurses.md"
;
			}
			~Ncurses_Handler() {
				
#line 101 "ncurses.md"

	endwin();

#line 84 "ncurses.md"
;
			}
	};

#line 107 "ncurses.md"

	struct End_Of_Curses {};

#line 113 "ncurses.md"

	
#line 149 "ncurses.md"

	void draw_number(int l) {
		int r = l / 10;
		if (r) { draw_number(r); }
		addch((l % 10) + '0');
	}

	void draw_line(int l) {
		if (l <= 9) {
			addch(' ');
		}
		draw_number(l);
		addstr(": ");
	}

#line 114 "ncurses.md"

	void draw_page() {
		clear();
		move(0, 0);
		
#line 167 "ncurses.md"

	if (curBlock->state == RS::header) {
		int i = 0;
		for (const auto &l : curBlock->value) {
			draw_line(++i);
			for (int i = 0; i < curBlock->level; ++i) {
				addch('#');
			}
			addch(' ');
			addstr(l.c_str());
			addstr("\n\n");
		}
	}

#line 184 "ncurses.md"

	if (curBlock->state == RS::code) {
		addstr("    ```\n");
		int i = 0;
		for (const auto &l : curBlock->value) {
			draw_line(++i);
			addstr(l.c_str());
			addch('\n');
		}
		addstr("    ```\n\n");
	}

#line 199 "ncurses.md"

	if (curBlock->state == RS::para) {
		int i = 0;
		for (const auto &l : curBlock->value) {
			draw_line(++i);
			addstr(l.c_str());
			addstr("\n\n");
		}
	}

#line 212 "ncurses.md"

	int j = 0;
	for (const auto &l : curBlock->notes) {
		draw_line(++j);
		addstr("* ");
		addstr(l.c_str());
		addch('\n');
	}
	addch('\n');

#line 225 "ncurses.md"

	int idx = (curInput - inputs.begin()) + 1;
	draw_number(idx);
	if (idx == static_cast<int>(inputs.size())) {
		addstr(" = $");
	}
	addch(' ');
	addstr(curInput->path().c_str());
	addch(':');
	auto &bs { curInput->blocks };
	idx = (curBlock - bs.begin()) + 1;
	draw_number(idx);
	if (
		idx == static_cast<int>(bs.size())
	) {
		addstr(" = $");
	}

#line 118 "ncurses.md"
;
		refresh();
	}

#line 38 "ncurses.md"


#line 31 "ncurses.md"

	#endif

#line 39 "index.md"

	int main(
		int argc,
		const char **argv
	) {
		
#line 115 "index.md"

	#if ! NDEBUG
		
#line 301 "frag.md"

	
#line 321 "frag.md"

	testFragName("abc");
	testFragName("");
	testFragName("A c");
	{
		Frag f { "ab" };
		ASSERT(f.empty());
	}

#line 349 "frag.md"

	{
		FragEntry entry;
		ASSERT(! entry.frag);
	}

#line 360 "frag.md"

	{
		Frag f { "" };
		Write_State s { f };
		FragEntry entry;
		ASSERT(entry.str(s).empty());
	}

#line 677 "frag.md"
 {
	Frag frag { "" };
	addStringToFrag(&frag, "abc");
	addStringToFrag(&frag, "def");
	testFrag(frag, "abcdef");
} 
#line 687 "frag.md"
 {
	Frag a { "" };
	Frag b { "" };
	addStringToFrag(&a, "abc");
	b.add(&a);
	addStringToFrag(&b, "def");
	b.add(&a);
	testFrag(b, "abcdefabc");
} 
#line 302 "frag.md"
;

#line 210 "line.md"

	
#line 216 "line.md"

	ASSERT(! Line {});
	ASSERT(Line::begin());
	ASSERT(Line::end());
	ASSERT(Line::end());
	ASSERT(Line::line(0));
	ASSERT(Line::relative(0));
	ASSERT(Line::relative(-2));

#line 228 "line.md"

	ASSERT(Line {}(5, 10) == 5);
	ASSERT(Line::begin()(5, 10) == 0);
	ASSERT(Line::end()(5, 10) == 10);

#line 236 "line.md"

	ASSERT(Line::line(0)(5, 10) == 0);
	ASSERT(Line::line(6)(5, 10) == 6);
	ASSERT(Line::line(20)(5, 10) == 10);

#line 244 "line.md"

	ASSERT(
		Line::relative(2)(5, 10) == 7
	);
	ASSERT(
		Line::relative(7)(5, 10) == 10
	);
	ASSERT(
		Line::relative(-2)(5, 10) == 3
	);
	ASSERT(
		Line::relative(-7)(5, 10) == 0
	);

#line 261 "line.md"
 {
	std::string f = "+3";
	ASSERT(
		get_line(f)(5, 10) == 8
	);
} 
#line 270 "line.md"
 {
	std::string f = ".";
	ASSERT(
		get_line(f)(5, 10) == 5
	);
} 
#line 279 "line.md"
 {
	std::string f = "$";
	ASSERT(
		get_line(f)(5, 10) == 10 
	);
} 
#line 211 "line.md"
;

#line 117 "index.md"
;
	#endif

#line 132 "index.md"

	
#line 225 "index.md"

	for (int i { 1 }; i < argc; ++i) {
		std::string arg { argv[i] };
		
#line 256 "index.md"
 {
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
} 
#line 279 "index.md"
 {
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
} 
#line 1125 "index.md"
 {
	static const std::string prefix {
		"--no-cmds"
	};
	if (arg == prefix) {
		no_cmds = true;
		continue;
	}
} 
#line 30 "view.md"

	if (
		arg == "-i" ||
		arg == "--interactive"
	) {
		interactive = true;
		write_files = false;
		process_files = false;
		html_files = false;
		continue;
	}

#line 49 "ncurses.md"

	#if HAVE_CURSES
		if (
			arg == "-c" ||
			arg == "--curses"
		) {
			with_ncurses = true;
			interactive = false;
			write_files = false;
			process_files = false;
			html_files = false;
			continue;
		}
	#endif

#line 228 "index.md"
;
		
#line 297 "index.md"

	inputs.add(arg);
	continue;

#line 229 "index.md"
;
		ASSERT_MSG(false,
			"unknown argument [" <<
			argv[i] << ']'
		);
	}

#line 133 "index.md"
;

#line 145 "index.md"

	
#line 101 "read.md"

	read_sources();

#line 146 "index.md"
;

#line 154 "index.md"

	
#line 911 "index.md"

	if (write_files) {
		files_write();
	}

#line 1052 "index.md"

	if (process_files) {
		files_process();
	}

#line 155 "index.md"
;

#line 162 "index.md"

	
#line 17 "html.md"

	if (html_files) {
		write_html();
	}

#line 163 "index.md"
;

#line 45 "view.md"

	if (interactive) {
		
#line 53 "view.md"

	curInput = inputs.begin();
	curBlock = curInput->blocks.begin();

#line 68 "view.md"

	draw_block();
	for (;;) {
		
#line 95 "view.md"

	std::string cmd;
	draw_position();
	std::cout << "> ";
	std::getline(std::cin, cmd);
	trim(cmd);
	if (cmd.empty()) { continue; }
	
#line 56 "range.md"

	range = Range {};
	range << get_line(cmd);
	if (! cmd.empty() && cmd[0] == ',') {
		cmd.erase(0, 1);
		range << get_line(cmd);
	}

#line 102 "view.md"
;

#line 107 "view.md"

	if (cmd == "q" || cmd == "quit") {
		break;
	}

#line 115 "view.md"

	if (cmd == "n" || cmd == "next") {
		int next = (curBlock - curInput->blocks.begin()) + 1;
		while (next >= static_cast<int>(curInput->blocks.size())) {
			--next;
		}
		
#line 67 "range.md"

	if (range) {
		next = range.last()(
			(curBlock - curInput->blocks.begin()) + 1,
			curInput->blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 121 "view.md"
;
		curBlock = curInput->blocks.begin() + next;
		draw_block();
		continue;
	}

#line 130 "view.md"

	if (cmd == "p" || cmd == "prev") {
		int next = curBlock - curInput->blocks.begin();
		if (next > 0) {
			--next;
		}
		
#line 67 "range.md"

	if (range) {
		next = range.last()(
			(curBlock - curInput->blocks.begin()) + 1,
			curInput->blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 136 "view.md"
;
		curBlock = curInput->blocks.begin() + next;
		draw_block();
		continue;
	}

#line 209 "view.md"

	if (cmd == "f" || cmd == "forward") {
		int next =(curInput - inputs.begin()) + 1;
		while (next >= static_cast<int>(inputs.size())) {
			--next;
		}
		
#line 79 "range.md"

	if (range) {
		next = range.last()(
			(curInput - inputs.begin()) + 1,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 215 "view.md"
;
		curInput = inputs.begin() + next;
		curBlock = curInput->blocks.begin();
		draw_block();
		continue;
	}

#line 225 "view.md"

	if (cmd == "b" || cmd == "backward") {
		int next = curInput - inputs.begin();
		if (next) {
			--next;
		}
		
#line 79 "range.md"

	if (range) {
		next = range.last()(
			(curInput - inputs.begin()) + 1,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 231 "view.md"
;
		curInput = inputs.begin() + next;
		curBlock = curInput->blocks.begin();
		draw_block();
		continue;
	}

#line 15 "edit.md"

	if (cmd == "l" || cmd == "list") {
		insert_before(
			"l", curBlock->notes
		);
		continue;
	}

#line 26 "edit.md"

	if (cmd == "a" || cmd == "add") {
		std::string prefix;
		switch (curBlock->state) {
			case RS::header:
				prefix = "h"; break;
			case RS::code:
				prefix = "c"; break;
			case RS::para:
				prefix = "p"; break;
			default:
				prefix = "?"; break;
		}
		insert_before(
			prefix,
			curBlock->value
		);
		continue;
	}

#line 70 "edit.md"

	if (cmd == ">>") {
		if (curBlock->state == RS::header) {
			++curBlock->level;
			draw_block();
			continue;
		}
	}

#line 82 "edit.md"

	if (cmd == "<<") {
		if (curBlock->level > 1) {
			--curBlock->level;
			draw_block();
			continue;
		}
	}

#line 16 "write.md"

	if (cmd == "W" || cmd == "Write") {
		write_x();
		continue;
	}

#line 163 "write.md"

	if (cmd == "H" || cmd == "Html") {
		write_x();
		write_html();
		continue;
	}

#line 173 "write.md"

	if (cmd == "F" || cmd == "Files") {
		write_x();
		write_html();
		Inputs old { std::move(inputs) };
		try {
			read_sources();
			files_write();
		} catch (...) {
			std::cerr << "!! aborted\n";
			inputs = std::move(old);
		}
		curInput = inputs.begin();
		curBlock =
			curInput->blocks.begin();
		continue;
	}

#line 194 "write.md"

	if (cmd == "P" || cmd == "Process") {
		write_x();
		write_html();
		Inputs old { std::move(inputs) };
		try {
			read_sources();
			files_write();
			files_process();
		} catch (...) {
			std::cerr << "!! aborted\n";
			inputs = std::move(old);
		}
		curInput = inputs.begin();
		curBlock =
			curInput->blocks.begin();
		continue;
	}

#line 216 "write.md"

	if (cmd == "M" || cmd == "Make") {
		write_x();
		system("make");
		continue;
	}

#line 241 "write.md"
 {
	static const std::string p { "M " };
	if (is_prefix(cmd, p)) {
		write_x();
		system(("make " + cmd.substr(p.size())).c_str());
		continue;
	}
} 
#line 252 "write.md"
 {
	static const std::string p { "Make " };
	if (is_prefix(cmd, p)) {
		write_x();
		system(("make " + cmd.substr(p.size())).c_str());
		continue;
	}
} 
#line 263 "write.md"

	if (cmd == "G" || cmd == "Git") {
		write_x();
		system("git status");
		continue;
	}

#line 273 "write.md"
 {
	static const std::string p { "G " };
	if (is_prefix(cmd, p)) {
		write_x();
		system(("git " + cmd.substr(p.size())).c_str());
		continue;
	}
} 
#line 284 "write.md"
 {
	static const std::string p { "Git " };
	if (is_prefix(cmd, p)) {
		write_x();
		system(("git " + cmd.substr(p.size())).c_str());
		continue;
	}
} 
#line 19 "add.md"

	if (cmd == "h" || cmd == "header") {
		add_block(RS::header);
		continue;
	}

#line 29 "add.md"

	if (cmd == "c" || cmd == "code") {
		add_block(RS::code);
		continue;
	}

#line 39 "add.md"

	if (cmd == "o" || cmd == "other") {
		add_block(RS::para);
		continue;
	}

#line 98 "add.md"

	if (cmd == "d" || cmd == "dup") {
		if (curInput != inputs.end()) {
			if (curBlock !=
				curInput->blocks.end()
			) {
				
#line 118 "add.md"

	int i = curBlock -
		curInput->blocks.begin();
	curInput->blocks.insert(
		curBlock, *curBlock
	);
	curBlock = curInput->blocks.begin() +
		i + 1;

#line 104 "add.md"
;
			}
		} else {
			std::cerr << "! no file\n";
		}
		continue;
	}

#line 71 "view.md"
;
	}

#line 47 "view.md"
;
	}

#line 67 "ncurses.md"

	#if HAVE_CURSES
		if (with_ncurses) {
			
#line 125 "ncurses.md"

	Ncurses_Handler handler;
	curInput = inputs.begin();
	curBlock = curInput->blocks.begin();
	draw_page();
	int ch;
	try {
		for (;;) {
			switch (ch = getch()) {
				
#line 143 "ncurses.md"

	case 'q': throw End_Of_Curses {};

#line 247 "ncurses.md"

	case 'n': {
		int next = (curBlock - curInput->blocks.begin()) + 1;
		while (next >= static_cast<int>(curInput->blocks.size())) {
			--next;
		}
		
#line 67 "range.md"

	if (range) {
		next = range.last()(
			(curBlock - curInput->blocks.begin()) + 1,
			curInput->blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 253 "ncurses.md"
;
		curBlock = curInput->blocks.begin() + next;
		draw_page();
		break;
	}

#line 262 "ncurses.md"

	case 'p' : {
		int next = curBlock - curInput->blocks.begin();
		if (next > 0) {
			--next;
		}
		
#line 67 "range.md"

	if (range) {
		next = range.last()(
			(curBlock - curInput->blocks.begin()) + 1,
			curInput->blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 268 "ncurses.md"
;
		curBlock = curInput->blocks.begin() + next;
		draw_page();
		break;
	}

#line 277 "ncurses.md"

	case 'f': {
		int next =(curInput - inputs.begin()) + 1;
		while (next >= static_cast<int>(inputs.size())) {
			--next;
		}
		
#line 79 "range.md"

	if (range) {
		next = range.last()(
			(curInput - inputs.begin()) + 1,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 283 "ncurses.md"
;
		curInput = inputs.begin() + next;
		curBlock = curInput->blocks.begin();
		draw_page();
		continue;
	}

#line 293 "ncurses.md"

	case 'b': {
		int next = curInput - inputs.begin();
		if (next) {
			--next;
		}
		
#line 79 "range.md"

	if (range) {
		next = range.last()(
			(curInput - inputs.begin()) + 1,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 299 "ncurses.md"
;
		curInput = inputs.begin() + next;
		curBlock = curInput->blocks.begin();
		draw_page();
		continue;
	}

#line 134 "ncurses.md"

			}
			draw_page();
		}
	} catch (const End_Of_Curses &) {}

#line 70 "ncurses.md"
;
		}
	#endif

#line 44 "index.md"

	}
