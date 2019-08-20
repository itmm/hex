
#line 38 "index.x"

	
#line 165 "index.x"

	
#line 36 "read.x"

	#include <string>

#line 176 "read.x"

	#include <fstream>

#line 91 "blocks.x"

	#include <vector>

#line 9 "log.x"

	#include <iostream>
	#include <exception>

#line 197 "frag.x"

	#include <vector>

#line 645 "frag.x"

	#include <sstream>

#line 761 "frag.x"

	#include <map>

#line 4 "input.x"

	#include <iostream>
	#include <vector>

#line 461 "index.x"

	#include <algorithm>

#line 816 "index.x"

	#include <functional>
	#include <sstream>

#line 82 "html.x"

	#include <string>

#line 691 "html.x"

	#include <cctype>

#line 741 "html.x"

	#include <set>

#line 94 "line.x"

	#include <limits>

#line 4 "ncurses.x"

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

#line 23 "ncurses.x"

	#include <locale.h>

#line 166 "index.x"
;

#line 5 "read.x"

	
#line 20 "read.x"

	
#line 64 "read.x"

	struct No_More_Lines {};

#line 146 "read.x"

	
#line 160 "read.x"

	
#line 7 "blocks.x"

	
#line 14 "blocks.x"

	enum class Read_State {
		new_element
		
#line 66 "blocks.x"
,
	header

#line 136 "blocks.x"
,
	code,
	after_code

#line 185 "blocks.x"
,
	notes

#line 241 "blocks.x"
,
	para

#line 17 "blocks.x"

	};

#line 26 "blocks.x"

	using RS = Read_State;

#line 97 "blocks.x"

	struct Block {
		Read_State state;
		std::vector<std::string> value;
		std::vector<std::string> notes;
		int level;
	};

#line 8 "blocks.x"


#line 7 "frag.x"

	class Frag;

	struct Write_State {
		std::string source_name = {};
		bool in_macro = false;
		bool c_style;

		Write_State(const Frag &f);
	};

#line 21 "frag.x"

	
#line 19 "log.x"

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

#line 36 "log.x"

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

#line 51 "log.x"

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
		
#line 49 "frag.x"

	FragEntry(
		Frag *frag = nullptr
	):
		_first_line { -1 },
		frag { frag }
	{}

#line 63 "frag.x"

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

#line 106 "frag.x"

	void add(
		char ch, const std::string &file,
		int line
	) {
		
#line 121 "frag.x"

	if (
		_file.empty() || _first_line <= 0
	) {
		_file = file;
		_first_line = line;
	}
	_last_line = line;

#line 111 "frag.x"
;
		_str += ch;
	}

#line 136 "frag.x"

	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		
#line 121 "frag.x"

	if (
		_file.empty() || _first_line <= 0
	) {
		_file = file;
		_first_line = line;
	}
	_last_line = line;

#line 142 "frag.x"
;
		_str += value;
	}

#line 150 "frag.x"

	bool canAdd(
		const std::string &file,
		int line
	) {
		
#line 164 "frag.x"

	if (
		! _file.empty() && file != _file
	) {
		return false;
	}

#line 176 "frag.x"

	if (
		_last_line > 0 &&
		_last_line != line &&
		_last_line + 1 != line
	) {
		return false;
	}

#line 190 "frag.x"

	return true;

#line 155 "frag.x"
;
		return false;
	}

#line 37 "frag.x"
;
	};

#line 204 "frag.x"

	class Frag {
		std::vector<FragEntry> _entries;
		int _expands;
		int _multiples;
	public:
		const std::string name;
		
#line 223 "frag.x"

	bool isFile() const {
		static const std::string prefix {
			"file: "
		};
		std::string p {
			name.substr(0, prefix.size())
		};
		return p == prefix;
	}

#line 239 "frag.x"

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

#line 255 "frag.x"

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

#line 272 "frag.x"

	void clear() {
		_entries.clear();
	}

#line 282 "frag.x"

	bool empty() const {
		return _entries.empty();
	}

#line 375 "frag.x"

	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		if (value.empty()) { return; }
		
#line 395 "frag.x"

	if (_entries.empty()) {
		_entries.push_back(FragEntry {});
	} else if (
		! _entries.back().canAdd(
			file, line
		)
	) {
		_entries.push_back(FragEntry {});
	}

#line 382 "frag.x"
;
		_entries.back().add(
			value, file, line
		);
	}

#line 412 "frag.x"

	void add(
		char ch,
		const std::string &file,
		int line
	) {
		
#line 395 "frag.x"

	if (_entries.empty()) {
		_entries.push_back(FragEntry {});
	} else if (
		! _entries.back().canAdd(
			file, line
		)
	) {
		_entries.push_back(FragEntry {});
	}

#line 418 "frag.x"
;
		_entries.back().add(
			ch, file, line
		);
	}

#line 429 "frag.x"

	Frag &add(Frag *child);

#line 465 "frag.x"

	auto begin() const {
		return _entries.cbegin();
	}

#line 474 "frag.x"

	auto end() const {
		return _entries.cend();
	}

#line 483 "frag.x"

	int expands() const {
		return _expands;
	}

#line 492 "frag.x"

	void addExpand() {
		++_expands;
	}

#line 501 "frag.x"

	int multiples() const {
		return _multiples;
	}

#line 510 "frag.x"

	void addMultiple() {
		++_multiples;
	}

#line 519 "frag.x"

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

#line 211 "frag.x"
;
	};

#line 291 "frag.x"

	Write_State::Write_State(const Frag &f):
		c_style { f.is_c_style() }
	{ }

#line 308 "frag.x"

	void testFragName(
		const std::string &name
	) {
		Frag f(name);
		ASSERT(f.name == name);
	}

#line 335 "frag.x"

	bool isPopulatedFrag(
		const Frag *f
	) {
		return f && ! f->empty();
	}

#line 438 "frag.x"

	
#line 703 "frag.x"

	bool isFragInFrag(
		const Frag *needle,
		const Frag *haystack
	) {
		ASSERT(needle);
		ASSERT(haystack);
		
#line 732 "frag.x"

	if (needle == haystack) {
		return true;
	}

#line 710 "frag.x"
;
		
#line 742 "frag.x"

	for (const auto &i : *haystack)  {
		if (! i.frag) { continue; }
		if (isFragInFrag(
			needle, i.frag
		)) {
			return true;
		}
	}

#line 711 "frag.x"
;
		return false;
	}

#line 439 "frag.x"

	Frag &Frag::add(Frag *child) {
		ASSERT(child);
		
#line 722 "frag.x"

	ASSERT(! isFragInFrag(
		this, child
	));

#line 442 "frag.x"
;
		
#line 456 "frag.x"

	_entries.push_back(
		FragEntry { child }
	);

#line 443 "frag.x"
;
		return *this;
	}

#line 541 "frag.x"

	void serializeFrag(
		const Frag &frag,
		std::ostream &out,
		Write_State &state
	) {
		
#line 569 "frag.x"

	for (const auto &entry : frag) {
		if (entry.frag) {
			serializeFrag(
				*entry.frag, out,
				state
			);
		}
		out << entry.str(state);
	}

#line 547 "frag.x"
;
	}

#line 555 "frag.x"

	void serializeFrag(
		const Frag &f,
		std::ostream &out
	) {
		Write_State state { f };
		return serializeFrag(
			f, out, state
		);
	}

#line 585 "frag.x"

	bool check_frag(
		const Frag &f,
		std::istream &in,
		Write_State &state
	) {
		
#line 612 "frag.x"

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

#line 591 "frag.x"
;
		return true;
	}

#line 598 "frag.x"

	bool check_frag(
		const Frag &f,
		std::istream &in
	) {
		Write_State state { f };
		return check_frag(
			f, in, state
		);
	}

#line 632 "frag.x"

	void testFrag(
		const Frag &frag,
		const std::string &expected
	) {
		
#line 652 "frag.x"

	std::ostringstream buffer;
	serializeFrag(frag, buffer);
	ASSERT(buffer.str() == expected);

#line 637 "frag.x"
;
	}

#line 662 "frag.x"

	void addStringToFrag(
		Frag *frag,
		const std::string &str
	) {
		frag->add(
			str, std::string {}, 0
		);
	}

#line 768 "frag.x"

	using FragMap =
		std::map<std::string, Frag>;

#line 23 "frag.x"
;

#line 161 "read.x"
;
	class Input {
		public:
			
#line 246 "read.x"

	Input(const std::string &path):
		_path { path }
	{}

#line 256 "read.x"

	Input(const Input &) = delete;
	Input(Input &&) = default;

#line 265 "read.x"

	Input &operator=(
		const Input &
	) = delete;
	Input &operator=(
		Input &&
	) = default;

#line 278 "read.x"

	const std::string &path() const {
		return _path;
	}

#line 108 "blocks.x"

	std::vector<Block> blocks;

#line 116 "input.x"

	FragMap frags;

#line 164 "read.x"
;
		private:
			std::string _path;
	};

#line 147 "read.x"
;
	class Open_Input {
		public:
			
#line 194 "read.x"

	Open_Input(const std::string &path):
		_input { path },
		_file { path.c_str() }
	{}

#line 204 "read.x"

	Open_Input(
		const Open_Input &
	) = delete;
	Open_Input(
		Open_Input &&
	) = default;

#line 217 "read.x"

	Open_Input &operator=(
		const Open_Input &
	) = delete;
	Open_Input &operator=(
		Open_Input &&
	) = default;

#line 230 "read.x"

	Input &input() { return _input; }

#line 237 "read.x"

	const Input &input() const {
		return _input;
	}

#line 287 "read.x"

	void read_line(std::string &line) {
		if (_file.is_open()) {
			
#line 300 "read.x"

	if (std::getline(_file, line)) {
		
#line 143 "input.x"

	++_line;

#line 302 "read.x"
;
		return;
	}
	_file.close();

#line 290 "read.x"
;
		}
		throw No_More_Lines {};
	}

#line 33 "blocks.x"

	Read_State state = RS::new_element;

#line 134 "input.x"

	int line() const {
		return _line;
	}

#line 150 "read.x"
;
		private:
			
#line 183 "read.x"

	Input _input;
	std::ifstream _file;
	// char _last;

#line 127 "input.x"

	int _line = 0;

#line 152 "read.x"
;
	};

#line 307 "index.x"

	using SI =
		std::string::const_iterator;

#line 21 "read.x"
;
	class Inputs {
		public:
			
#line 43 "read.x"

	void read_line(std::string &line);

#line 109 "read.x"

	void clear() {
		
#line 234 "input.x"

	_used.clear();
	_open.clear();
	if (_paths.empty()) {
		_paths.push_back("index.x");
	}
	_current_path = _paths.begin();

#line 111 "read.x"
;
	}

#line 20 "input.x"

	auto &cur() {
		ASSERT (! _open.empty());
		return _open.back();
	}

#line 30 "input.x"

	auto begin() {
		return _used.begin();
	}

#line 39 "input.x"

	auto end() {
		return _used.end();
	}

#line 48 "input.x"

	auto size() const {
		return _used.size();
	}

#line 56 "input.x"

	void push(const std::string &path) {
		_used.push_back({ path });
		_open.push_back({ path });
	}

#line 68 "input.x"

	void add(const std::string &path) {
		_paths.push_back(path);
		push(path);
	}

#line 85 "input.x"

	bool has(
		const std::string &name
	) const {
		
#line 100 "input.x"

	for (const auto &j : _used) {
		if (j.path() == name) {
			return true;
		}
	}

#line 89 "input.x"
;
		return false;
	}

#line 150 "input.x"

	Frag *find_local(const std::string &name) {
		ASSERT(! _open.empty());
		Input &i = _open.back().input();
		auto f = i.frags.find(name);
		if (f == i.frags.end()) { return nullptr; }
		return &f->second;
	}

#line 162 "input.x"

	Frag *add_local(const std::string &name) {
		ASSERT(! _open.empty());
		Input &i = _open.back().input();
		return &i.frags.insert({ name, name }).first->second;
	}

#line 172 "input.x"

	Frag *get_local(const std::string &name) {
		Frag *result = find_local(name);
		if (! result) {
			result = add_local(name);
		}
		return result;
	}

#line 184 "input.x"

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

#line 206 "input.x"

	Frag *add_global(const std::string &name) {
		return &_root.insert({ name, name }).first->second;
	}

#line 214 "input.x"

	const FragMap &root() const {
		return _root;
	}

#line 222 "input.x"

	Frag *get_global(const std::string &name) {
		Frag *result = find_global(name);
		if (! result) {
			result = add_global(name);
		}
		return result;
	}

#line 24 "read.x"
;
		private:
			
#line 313 "read.x"

	std::vector<std::string> _paths;
	std::vector<std::string>::
		const_iterator _current_path;

#line 323 "read.x"

	std::vector<Open_Input> _open;
	std::vector<Input> _used;

#line 12 "input.x"

	FragMap _root;

#line 26 "read.x"
;
	};

#line 51 "read.x"

	void Inputs::read_line(
		std::string &line
	) {
		
#line 339 "read.x"

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
		
#line 363 "read.x"

	auto &f { _open.back().input() };
	
#line 376 "read.x"

	if (f.blocks.empty()) {
		f.blocks.push_back({
			RS::header,
			{ "EMPTY FILE" }, {}
		});
	}

#line 365 "read.x"
;
	for (auto &i : _used) {
		if (i.path() == f.path()) {
			i = std::move(f);
			break;
		}
	}

#line 353 "read.x"
;
		_open.pop_back();
	}
	throw No_More_Lines {};

#line 55 "read.x"
;
	}

#line 80 "read.x"

	Inputs inputs;

#line 91 "read.x"

	
#line 268 "index.x"

	int blockLimit = -1;

#line 316 "index.x"

	void process_char(
		Frag *frag, char ch
	) {
		
#line 355 "index.x"

	if (frag) {
		frag->add(
			ch, inputs.cur().input().path(),
			inputs.cur().line()
		);
	}

#line 320 "index.x"
;
	}

#line 438 "index.x"

	inline void expand_cmd_arg(
		Frag *f, const std::string &arg
	) {
		auto b = arg.begin();
		auto e = arg.end();
		
#line 468 "index.x"

	while (b != e) {
		auto x = std::find(b, e, '@');
		
#line 486 "index.x"

	f->add(
		std::string { b, x },
		inputs.cur().input().path(),
		inputs.cur().line()
	);

#line 471 "index.x"
;
		if (x != e) {
			b = x + 1;
			
#line 497 "index.x"

	if (b != e) {
		f->add(
			*b,
			inputs.cur().input().path(),
			inputs.cur().line()
		);
		++b;
	}

#line 474 "index.x"
;
		} else {
			b = e;
		}
	}

#line 444 "index.x"
;
	}

#line 511 "index.x"

	#define ASSERT_NOT_FRAG() \
		ASSERT_MSG(! frag, '@' << \
			name << "(" << arg << \
			") in frag [" << \
			frag->name << ']' \
		)

#line 523 "index.x"

	#define CHECK_NOT_DEFINED() \
		if (isPopulatedFrag(frag)) { \
			WARN_MSG("frag [" << arg << \
				"] already defined" \
			); \
		}

#line 548 "index.x"

	#define ASSERT_FRAG() \
		ASSERT_MSG(frag, '@' << \
			name << "(" << arg << \
			") in frag [" << \
			frag->name << ']' \
		)

#line 582 "index.x"

	#define CHECK_DEFINED() \
		if (! isPopulatedFrag(frag)) { \
			WARN_MSG("frag [" << arg << \
				"] not defined" \
			); \
		}

#line 92 "read.x"
;
	void read_sources() {
		
#line 119 "read.x"
 {
	inputs.clear();
	
#line 345 "index.x"

	Frag *frag { nullptr };

#line 121 "read.x"
;
	std::string line;
	try { for (;;) {
		inputs.read_line(line);
		
#line 41 "blocks.x"

	do {
		auto &state = inputs.cur().state;
		
#line 114 "blocks.x"

	auto &blocks =
		inputs.cur().input().blocks;

#line 44 "blocks.x"
;
		
#line 143 "blocks.x"

	if (
		line == "```" &&
		state == RS::new_element
	) {
		state = RS::code;
		
#line 169 "blocks.x"

	blocks.push_back({
		RS::code, {}, {}, 0
	});

#line 149 "blocks.x"
;
		break;
	}

#line 156 "blocks.x"

	if (state == RS::code) {
		if (line == "```") {
			state = RS::after_code;
		} else {
			
#line 177 "blocks.x"

	blocks.back().value.push_back(
		line
	);

#line 161 "blocks.x"
;
		}
		break;
	}

#line 45 "blocks.x"
;
		
#line 57 "blocks.x"

	if (line.empty()) {
		state = RS::new_element;
		break;
	}

#line 46 "blocks.x"
;
		
#line 72 "blocks.x"

	if (
		line[0] == '#' &&
		state == RS::new_element
	) {
		state = RS::header;
		
#line 121 "blocks.x"

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

#line 78 "blocks.x"
;
		break;
	}

#line 191 "blocks.x"

	if (line[0] == '*') {
		if (
			state == RS::header ||
			state == RS::after_code ||
			state == RS::notes
		) {
			state = RS::notes;
			
#line 219 "blocks.x"

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

#line 199 "blocks.x"
;
			break;
		}
	}

#line 207 "blocks.x"

	if (
		line[0] == ' ' &&
		state == RS::notes
	) {
		
#line 234 "blocks.x"

	blocks.back().notes.back() +=
		line;

#line 212 "blocks.x"
;
		break;
	}

#line 247 "blocks.x"

	if (line[0] != ' ') {
		if (
			state == RS::new_element ||
			state == RS::para
		) {
			
#line 263 "blocks.x"

	if (state == RS::new_element) {
		if (blocks.empty() || blocks.back().state != RS::para) {
			blocks.push_back({
				RS::para, {}, {}, 0
			});
		}
		blocks.back().value.push_back(line);
	}

#line 253 "blocks.x"
;
			
#line 276 "blocks.x"

	if (state == RS::para) {
		blocks.back().value.back() +=
			" " + line;
	}

#line 254 "blocks.x"
;
			state = RS::para;
			break;
		}
	}

#line 47 "blocks.x"
;
		
#line 85 "blocks.x"

	std::cerr << "!! " << line << '\n';

#line 48 "blocks.x"
;
	} while (false);

#line 327 "index.x"

	auto end = line.cend();
	for (
		auto i = line.cbegin();
		i != end; ++i
	) {
		
#line 370 "index.x"

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
			
#line 394 "index.x"

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
		
#line 415 "index.x"

	i = ae;
	bool outside = ! frag;
	do {
		if (outside && ! blockLimit) {
			break;
		}
		
#line 535 "index.x"

	if (name == "def") {
		ASSERT_NOT_FRAG();
		frag = inputs.get_local(arg);
		CHECK_NOT_DEFINED();
		break;
	}

#line 560 "index.x"

	if (name == "end" || name == "End") {
		ASSERT_FRAG();
		
#line 572 "index.x"

	ASSERT_MSG(frag->name == arg,
		"closing [" << arg <<
		"] != [" << frag->name << ']'
	);

#line 563 "index.x"
;
		frag = nullptr;
		break;
	}

#line 594 "index.x"

	if (name == "add") {
		ASSERT_NOT_FRAG();
		frag = inputs.get_local(arg);
		CHECK_DEFINED();
		break;
	}

#line 607 "index.x"

	if (name == "put") {
		ASSERT_MSG(frag,
			"@put" << "(" << arg << ") not in frag"
		);
		Frag *sub = inputs.get_local(arg);
		if (sub) {
			
#line 627 "index.x"

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

#line 614 "index.x"
;
			sub->addExpand();
			frag->add(sub);
		}
		break;
	}

#line 646 "index.x"

	if (name == "inc") {
		ASSERT_MSG(! frag,
			"include in frag [" << frag->name << ']'
		);
		if (! inputs.has(arg)) {
			inputs.push(arg);
		}
		break;
	}

#line 663 "index.x"

	if (name == "mul") {
		ASSERT_MSG(frag,
			"@mul not in frag"
		);
		Frag *sub = inputs.get_local(arg);
		if (sub) {
			
#line 682 "index.x"

	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}

#line 670 "index.x"
;
			sub->addMultiple();
			frag->add(sub);
		}
		break;
	}

#line 695 "index.x"

	if (name == "Def") {
		ASSERT_MSG(! frag, "@Def in frag [" << frag->name << ']');
		frag = inputs.get_global(arg);
		if (isPopulatedFrag(frag)) {
			std::cerr << "Frag [" << arg << "] already defined\n";
		}
		break;
	}

#line 710 "index.x"

	if (name == "Add") {
		ASSERT_MSG(! frag, "@Add in frag [" << frag->name << ']');
		frag = inputs.get_global(arg);
		if (! isPopulatedFrag(frag)) {
			std::cerr << "Frag [" << arg << "] not defined\n";
		}
		break;
	}

#line 724 "index.x"

	if (name == "rep") {
		ASSERT_MSG(! frag,
			"@rep in frag [" << frag->name << ']'
		);
		frag = inputs.get_local(arg);
		
#line 754 "index.x"

	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();

#line 730 "index.x"
;
		break;
	}

#line 739 "index.x"

	if (name == "Rep") {
		ASSERT_MSG(! frag,
			"@Rep in frag [" << frag->name << ']'
		);
		frag = inputs.get_global(arg);
		
#line 754 "index.x"

	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();

#line 745 "index.x"
;
		break;
	}

#line 765 "index.x"

	if (name == "Put") {
		ASSERT_MSG(frag,
			"@Put not in frag"
		);
		Frag *sub = inputs.get_global(arg);
		if (sub) {
			
#line 627 "index.x"

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

#line 772 "index.x"
;
			sub->addExpand();
			frag->add(sub);
		}
		break;
	}

#line 784 "index.x"

	if (name == "Mul") {
		ASSERT_MSG(frag,
			"@Mul not in frag"
		);
		Frag *sub = inputs.get_global(arg);
		if (sub) {
			
#line 682 "index.x"

	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}

#line 791 "index.x"
;
			sub->addMultiple();
			frag->add(sub);
		}
		break;
	}

#line 802 "index.x"

	if (name == "priv") {
		ASSERT_MSG(frag,
			"@priv not in frag"
		);
		
#line 824 "index.x"

	std::hash<std::string> h;
	auto cur {
		h(inputs.cur().input().path() +
			':' + arg) &
				0x7fffffff
	};

#line 838 "index.x"

	std::ostringstream hashed;
	hashed << "_private_" <<
		cur << '_' <<
		arg;
	frag->add(
		hashed.str(),
		inputs.cur().input().path(),
		inputs.cur().line()
	);

#line 807 "index.x"
;
		break;
	}

#line 857 "index.x"

	if (name == "magic") {
		ASSERT_MSG(frag,
			"@magic not in frag"
		);
		
#line 872 "index.x"

	std::hash<std::string> h;
	auto cur {
		h(inputs.cur().input().path() +
			':' + arg) &
				0x7fffffff
	};

#line 884 "index.x"

	std::ostringstream value;
	value << cur;
	frag->add(
		value.str(),
		inputs.cur().input().path(),
		inputs.cur().line()
	);

#line 862 "index.x"
;
		break;
	}

#line 422 "index.x"
;
		
#line 451 "index.x"

	if (frag) {
		expand_cmd_arg(frag, arg);
	}

#line 423 "index.x"
;
	} while (false);
	if (blockLimit && outside && frag) {
		--blockLimit;
	}

#line 405 "index.x"
;
		continue;
	}

#line 383 "index.x"
;
		}
	}

#line 333 "index.x"
;
		process_char(frag, *i);
	}
	process_char(frag, '\n');

#line 125 "read.x"
;
	} }
	catch (const No_More_Lines &) {}
} 
#line 94 "read.x"
;
	}

#line 6 "read.x"
;

#line 241 "index.x"

	std::string stylesheet {
		"slides/slides.css"
	};

#line 902 "index.x"

	
#line 1003 "index.x"

	std::string file_name(const Frag &f) {
		return f.name.substr(6);
	}

#line 1012 "index.x"

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

#line 903 "index.x"

	void files_write() {
		
#line 921 "index.x"

	for (auto &i : inputs.root()) {
		const Frag *frag {
			&i.second
		};
		
#line 950 "index.x"
 {
	if (frag->isFile()) {
		
#line 1033 "index.x"

	if (file_changed(*frag)) {
		std::ofstream out(
			file_name(*frag).c_str()
		);
		serializeFrag(*frag, out);
	}

#line 952 "index.x"
;
	}
} 
#line 961 "index.x"
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
#line 977 "index.x"

	if (frag->multiples() == 1) {
		std::cerr <<
			"multiple frag [" <<
			frag->name <<
			"] only used once\n";
	}

#line 991 "index.x"

	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not populated\n";
	}

#line 926 "index.x"
;
	}

#line 936 "index.x"

	for (auto &j : inputs) {
		for (auto &i : j.frags) {
			const Frag *frag {
				&i.second
			};
			
#line 950 "index.x"
 {
	if (frag->isFile()) {
		
#line 1033 "index.x"

	if (file_changed(*frag)) {
		std::ofstream out(
			file_name(*frag).c_str()
		);
		serializeFrag(*frag, out);
	}

#line 952 "index.x"
;
	}
} 
#line 961 "index.x"
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
#line 977 "index.x"

	if (frag->multiples() == 1) {
		std::cerr <<
			"multiple frag [" <<
			frag->name <<
			"] only used once\n";
	}

#line 991 "index.x"

	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not populated\n";
	}

#line 942 "index.x"
;
		}
	}

#line 905 "index.x"
;
	}

#line 1045 "index.x"

	
#line 1092 "index.x"

	bool no_cmds = false;

#line 1046 "index.x"
;
	void files_process() {
		
#line 1065 "index.x"

	for (auto &i : inputs.root()) {
		const Frag *frag {
			&i.second
		};
		
#line 1099 "index.x"
 {
	const std::string cmd { frag->cmd() };
	if (cmd.size()) {
		
#line 1109 "index.x"

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

#line 1102 "index.x"
;
	}
} 
#line 1070 "index.x"
;
	}

#line 1077 "index.x"

	for (auto &j : inputs) {
		for (auto &i : j.frags) {
			const Frag *frag {
				&i.second
			};
			
#line 1099 "index.x"
 {
	const std::string cmd { frag->cmd() };
	if (cmd.size()) {
		
#line 1109 "index.x"

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

#line 1102 "index.x"
;
	}
} 
#line 1083 "index.x"
;
		}
	}

#line 1048 "index.x"
;
	}

#line 6 "html.x"

	
#line 48 "html.x"

	enum class HtmlState {
		nothing,
		inSlide,
		afterSlide,
		afterSlides
		
#line 309 "html.x"

	, inCode

#line 915 "html.x"

	, inNotes

#line 1067 "html.x"

	, inPara

#line 54 "html.x"

	};

#line 65 "html.x"

	struct HtmlStatus {
		
#line 74 "html.x"

	HtmlState state = HtmlState::nothing;

#line 67 "html.x"

	};

#line 171 "html.x"
 
	void writeOneEscaped(
		std::ostream &out, char ch
	) {
		switch (ch) {
			
#line 204 "html.x"

	case '<':
		out << "&lt;";
		break;
	case '>':
		out << "&gt;";
		break;
	case '&':
		out << "&amp;";
		break;

#line 176 "html.x"

			default:
				out << ch;
		}
	}

#line 188 "html.x"

	void writeEscaped(
		std::ostream &out,
		const std::string &str
	) {
		for (char ch : str) {
			writeOneEscaped(out, ch);
		}
	}

#line 219 "html.x"

	
#line 444 "html.x"

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

#line 748 "html.x"

	using Set = std::set<std::string>;

	bool isKeyword(const std::string &s) {
		static Set reserved {
			
#line 765 "html.x"

	"break", "case", "catch", "continue",
	"default", "delete", "else", "for",
	"if", "in", "new", "return", "static",
	"switch", "try", "typeof", "while",
	"class", "public", "private",
	"template", "typename", "using",
	"function", "throw", "namespace",
	"once", "constexpr", "volatile"

#line 753 "html.x"

		};
		return
			reserved.find(s) !=
				reserved.end() ||
					(s.size() && s[0] == '#');
	}

#line 779 "html.x"

	bool isType(const std::string &s) {
		
#line 789 "html.x"

	static Set reserved {
		
#line 804 "html.x"

	"FILE", "auto", "bool", "char",
	"const", "enum", "extern", "int",
	"let", "long", "signed", "struct",
	"union", "unsigned", "void", "double",
	"string", "std", "ifstream",
	"istream", "ofstream", "ostream",
	"vector", "map", "list"

#line 791 "html.x"

	};
	if (reserved.find(s) !=
		reserved.end()
	) {
		return true;
	}

#line 817 "html.x"

	if (s.size() >= 2) {
		if (isupper(s[0]) &&
			islower(s[1])
		) {
			return true;
		}
	}

#line 781 "html.x"
;
		return false;
	}

#line 832 "html.x"

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

#line 851 "html.x"

	void process_ident(
		std::ostream &out,
		const std::string ident,
		char w
	) {
		if (isKeyword(ident)) {
			span_str(out, "keyword", ident);
		} else if (w == '(') {
			span_str(out, "fn", ident);
		
#line 874 "html.x"

	} else if (isType(ident)) {
		span_str(out, "type", ident);
	} else if (isNum(ident)) {
		span_str(out, "num", ident);

#line 861 "html.x"

		} else {
			span_str(out, "var", ident);
		}
	}

#line 885 "html.x"

	void writeMacroClass(
		std::ostream &out,
		const char *name
	) {
		out << "<span class=\"" <<
			name << "\">";
	}

#line 898 "html.x"

	void writeMacroHeader(
		std::ostream &out,
		const std::string &name
	) {
		writeMacroClass(out, "macro");
		out << '@' << name << "(<span class=\"name\">";
	}

#line 220 "html.x"
;
	void process_code(
		std::ostream &out,
		SI begin, SI end
	) {
		
#line 380 "html.x"

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

#line 398 "html.x"

	for (; begin != end; ++begin) {
		
#line 410 "html.x"

	if (
		*begin == '`' ||
		*begin == '\'' ||
		*begin == '"'
	) {
		
#line 424 "html.x"

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

#line 461 "html.x"

	std::string name {begin, w + 1};
	span_str(out, "str", name);
	begin = w;

#line 416 "html.x"
;
		continue;
	}

#line 471 "html.x"

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
			
#line 493 "html.x"

	while (ae != end && *ae != ')') {
		if (*ae == '@') {
			++ae;
			if (ae == end) { break; }
		}
		++ae;
	}
	if (ae != end) {
		std::string arg {ab, ae};
		
#line 510 "html.x"

	do {
		
#line 532 "html.x"

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

#line 550 "html.x"

	if (name == "inc") {
		
#line 559 "html.x"

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

#line 552 "html.x"
;
		break;
	}

#line 577 "html.x"

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

#line 594 "html.x"

	if (name == "f" || name == "fn") {
		writeMacroClass(out, "fn");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 606 "html.x"

	if (name == "v" || name == "var") {
		writeMacroClass(out, "var");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 618 "html.x"

	if (name == "k" || name == "key") {
		writeMacroClass(out, "keyword");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 630 "html.x"

	if (name == "n" || name == "num") {
		writeMacroClass(out, "num");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 642 "html.x"

	if (name == "t" || name == "typ") {
		writeMacroClass(out, "type");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 654 "html.x"

	if (name == "b" || name == "br") {
		writeMacroClass(out, "virt");
		out << "</span><br/>";
		break;
	}

#line 665 "html.x"

	if (name == "priv") {
		writeMacroClass(out, "var");
		out << "@priv(<span>";
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}

#line 678 "html.x"

	if (name == "magic") {
		writeMacroClass(out, "num");
		out << "@magic(<span>";
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}

#line 512 "html.x"
;
		
#line 520 "html.x"

	std::cerr << "unknown macro @" <<
		name << '(' << arg << ")\n";
	writeOneEscaped(out, '@');
	writeEscaped(out, name);
	writeOneEscaped(out, '(');
	writeEscaped(out, arg);
	writeOneEscaped(out, ')');

#line 513 "html.x"
;
	} while (false);
	begin = ae;

#line 503 "html.x"
;
		continue;
	}

#line 486 "html.x"
;
		}
	}

#line 698 "html.x"

	auto w = begin;
	
#line 711 "html.x"

	while (w != end && (
		std::isalnum(*w) ||
			*w == '_' || *w == '$' ||
			*w == '#'
	)) {
		++w;
	}

#line 700 "html.x"
;
	if (w != begin) {
		
#line 725 "html.x"

	std::string ident {begin, w};
	begin = w - 1;
	process_ident(
		out, ident,
		w != end ? *w : ' '
	);

#line 702 "html.x"
;
		continue;
	}

#line 400 "html.x"
;
		writeOneEscaped(out, *begin);
	}

#line 225 "html.x"
;
	}

#line 234 "html.x"

	void process_content(
		std::ostream &out,
		SI begin, SI end
	) {
		
#line 974 "html.x"

	for(; begin != end; ++begin) {
		
#line 985 "html.x"

	if (*begin == '`') {
		
#line 994 "html.x"

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

#line 987 "html.x"
;
	}

#line 1012 "html.x"

	if (
		*begin == '*' &&
		(begin + 1) != end &&
		*(begin + 1) == '*'
	) {
		
#line 1025 "html.x"

	auto w = begin + 2;
	while (
		w != end && (w + 1) != end &&
		(*w != '*' || *(w + 1) != '*')
	) {
		++w;
	}

#line 1038 "html.x"

	if (
		w != end && (w + 1 ) != end &&
		*w == '*' && *(w + 1) == '*'
	) {
		
#line 1052 "html.x"

	out << "<b>";
	writeEscaped(
		out, std::string {begin + 2, w}
	);
	out << "</b>";
	begin = w + 1;

#line 1043 "html.x"
;
		continue;
	}

#line 1018 "html.x"
;
	}

#line 976 "html.x"
;
		writeOneEscaped(out, *begin);
	}

#line 239 "html.x"
;
	}

#line 7 "html.x"

	void write_html() {
		for (auto &cur : inputs) {
			
#line 27 "html.x"

	const std::string &name { cur.path() };
	std::string outPath {
		name.substr(0, name.size() - 2) +
		".html"
	};
	std::ofstream out { outPath.c_str() };
	
#line 41 "html.x"

	
#line 89 "html.x"

	HtmlStatus status;
	for (const auto &b : cur.blocks) {
		
#line 101 "html.x"

	if (b.state == RS::header) {
		
#line 150 "html.x"

	
#line 257 "html.x"

	switch (status.state) {
		case HtmlState::nothing: {
			
#line 277 "html.x"

	out << "<!doctype html>\n";
	out << "<html lang=\"de\">\n";
	out << "<head>\n";
	
#line 291 "html.x"

	out << "<meta charset=\"utf-8\">\n";
	out << "<title>";
	writeEscaped(out, b.value[0]);
	out << "</title>\n";
	out << "<link rel=\"stylesheet\" "
		"type=\"text/css\" href=\""
		<< stylesheet << "\">";

#line 281 "html.x"
;
	out << "</head>\n";
	out << "<body>\n";

#line 260 "html.x"
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

#line 151 "html.x"
;
	
#line 247 "html.x"
 {
	out << "<h" << b.level << '>';
	const auto &n = b.value[0];
	process_content(out, n.begin(), n.end());
	out << "</h" << b.level << ">\n";
} 
#line 152 "html.x"
;
	out << "<div class=\"slides\">\n";
	out << "<div>\n";
	out << "<div>\n";
	
#line 247 "html.x"
 {
	out << "<h" << b.level << '>';
	const auto &n = b.value[0];
	process_content(out, n.begin(), n.end());
	out << "</h" << b.level << ">\n";
} 
#line 156 "html.x"
;
	out << "</div>\n";
	status.state = HtmlState::inSlide;
	for (const auto &note : b.notes) {
		
#line 934 "html.x"

	auto end = note.end();
	auto begin = note.begin();

#line 942 "html.x"

	if (
		status.state != HtmlState::inNotes
	) {
		
#line 959 "html.x"

	if (
		status.state != HtmlState::inSlide
	) {
		out << "<div>\n";
	}
	status.state = HtmlState::inNotes;
	out << "<ul><li>\n";

#line 946 "html.x"
;
	} else {
		out << "</li><li>\n";
	}
	process_content(out, begin, end);
	out << '\n';

#line 160 "html.x"
;
	}
	
#line 923 "html.x"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1075 "html.x"

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}

#line 162 "html.x"
;
	
#line 138 "html.x"

	out << "</div>\n";
	status.state = HtmlState::afterSlide;

#line 163 "html.x"
;

#line 103 "html.x"
;
	}

#line 109 "html.x"

	if (b.state == RS::code) {
		
#line 316 "html.x"

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

#line 111 "html.x"
;
		for (const auto &code : b.value) {
			
#line 369 "html.x"

	process_code(
		out, code.begin(), code.end()
	);
	out << "<br/>\n";

#line 113 "html.x"
;
		}
		
#line 336 "html.x"

	out << "</code></div>\n";
	status.state = HtmlState::inSlide;

#line 115 "html.x"
;
		for (const auto &note : b.notes) {
			
#line 934 "html.x"

	auto end = note.end();
	auto begin = note.begin();

#line 942 "html.x"

	if (
		status.state != HtmlState::inNotes
	) {
		
#line 959 "html.x"

	if (
		status.state != HtmlState::inSlide
	) {
		out << "<div>\n";
	}
	status.state = HtmlState::inNotes;
	out << "<ul><li>\n";

#line 946 "html.x"
;
	} else {
		out << "</li><li>\n";
	}
	process_content(out, begin, end);
	out << '\n';

#line 117 "html.x"
;
		}
		
#line 923 "html.x"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1075 "html.x"

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}

#line 119 "html.x"
;
		
#line 138 "html.x"

	out << "</div>\n";
	status.state = HtmlState::afterSlide;

#line 120 "html.x"
;
	}

#line 126 "html.x"

	if (b.state == RS::para) {
		for (const auto &para : b.value) {
			
#line 1088 "html.x"

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

#line 129 "html.x"
;
			
#line 923 "html.x"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1075 "html.x"

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}

#line 130 "html.x"
;
		}
		
#line 923 "html.x"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1075 "html.x"

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}

#line 132 "html.x"
;
	}

#line 92 "html.x"
;
	}

#line 344 "html.x"

	if (
		status.state == HtmlState::inCode
	) {
		std::cerr <<
			"unterminated code block\n";
	}

#line 358 "html.x"

	if (
		status.state != HtmlState::nothing
	) {
		out << "</body>\n</html>\n";
	}

#line 42 "html.x"
;

#line 34 "html.x"
;
	out.close();

#line 10 "html.x"
;
		}
	}

#line 4 "view.x"

	bool interactive = false;
	std::vector<Input>::iterator curInput;
	std::vector<Block>::iterator curBlock;

#line 12 "view.x"

	bool write_files = true;

#line 18 "view.x"

	bool process_files = true;

#line 24 "view.x"

	bool html_files = true;

#line 60 "view.x"

	void draw_block() {
		
#line 145 "view.x"

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

#line 160 "view.x"

	if (curBlock->state == RS::code) {
		std::cout << "```\n";
		int i = 0;
		for (const auto &l : curBlock->value) {
			std::cout << ++i << ": " << l << '\n';
		}
		std::cout << "```\n\n";
	}

#line 173 "view.x"

	if (curBlock->state == RS::para) {
		int i = 0;
		for (const auto &l : curBlock->value) {
			std::cout << ++i << ": " << l << "\n\n";
		}
	}

#line 184 "view.x"

	int j = 0;
	for (const auto &l : curBlock->notes) {
		std::cout << ++j << ": * " << l << '\n';
	}
	std::cout << '\n';

#line 62 "view.x"
;
	}

#line 77 "view.x"

	void draw_position() {
		
#line 194 "view.x"

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

#line 79 "view.x"
;
	}

#line 85 "view.x"

	void trim(std::string &s) {
		while (! s.empty() && (s[0] & 0xff) <= ' ') {
			s.erase(0, 1);
		}
	}

#line 4 "line.x"

	class Line {
		public:
			
#line 15 "line.x"

	int operator()(
		int cur, int end
	) const {
		int res {};
		
#line 42 "line.x"

	if (! *this) {
		res = cur;
	} else {
		res = _line;
		if (_relative) { res += cur; }
	}
	if (res < 0) { res = 0; }
	if (res > end) { res = end; }

#line 20 "line.x"
;
		return res;
	}

#line 34 "line.x"

	operator bool() const {
		return _line >= 0 ||  _relative;
	}

#line 64 "line.x"

	Line() = default;

#line 70 "line.x"

	static Line relative(int line) {
		return Line { line, true };
	}

#line 78 "line.x"

	static Line line(int line) {
		return Line { line, false };
	}

#line 86 "line.x"

	static Line begin() {
		return line(0);
	}

#line 100 "line.x"

	static const int max =
		std::numeric_limits<int>::max();
		
	static Line end() {
		return line(max);
	}

#line 7 "line.x"
;
		private:
			
#line 27 "line.x"

	int _line = -1;
	bool _relative = false;

#line 55 "line.x"

	Line(int line, bool relative):
		_line { line },
		_relative { relative }
	{}

#line 9 "line.x"
;
	};

#line 111 "line.x"

	
#line 4 "range.x"

	class Range {
		public:
			
#line 16 "range.x"

	Line prev() {
		return _prev ?: _last;
	}

#line 24 "range.x"

	Line last() {
		return _last;
	}

#line 32 "range.x"

	Range &operator<<(const Line &l) {
		_prev = _last;
		_last = l;
		return *this;
	}

#line 42 "range.x"

	operator bool() {
		return _last;
	}

#line 7 "range.x"
;
		private:
			Line _prev;
			Line _last;
	};

#line 112 "line.x"
;
	
#line 113 "range.x"

	Range range;

#line 113 "line.x"
;

#line 124 "line.x"

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

#line 139 "line.x"

	Line get_line(std::string &s) {
		Line line {};
		do {
			
#line 152 "line.x"

	if (s[0] == '.') {
		s.erase(0, 1);
		line = Line::relative(0);
		break;
	}

#line 162 "line.x"

	if (s[0] == '+') {
		s.erase(0, 1);
		int n = get_number(s);
		line = Line::relative(n);
		break;
	}

#line 173 "line.x"

	if (s[0] == '-') {
		s.erase(0, 1);
		int n = -get_number(s);
		line = Line::relative(n);
		break;
	}

#line 184 "line.x"

	if (s[0] == '$') {
		line = Line::end();
		s.erase(0, 1);
		continue;
	}

#line 194 "line.x"

	if (isdigit(s[0])) {
		int n = get_number(s);
		line = Line::line(n);
		continue;
	}

#line 143 "line.x"
;
		} while (false);
		trim(s);
		return line;
	}

#line 4 "edit.x"

	void insert_before(
		const std::string &prefix,
		std::vector<std::string> &c
	) {
		
#line 49 "edit.x"

	int next = c.size();
	
#line 91 "range.x"

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

#line 51 "edit.x"
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

#line 9 "edit.x"
;
	}

#line 5 "write.x"

	
#line 100 "write.x"

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

#line 126 "write.x"

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

#line 6 "write.x"

	void write_x() {
		for (const auto &cur : inputs) {
			
#line 25 "write.x"

	std::ofstream out {
		cur.path().c_str()
	};

#line 33 "write.x"

	bool first = true;
	for (const auto &b : cur.blocks) {
		if (first) {
			first = false;
		} else { out << '\n'; }
		switch (b.state) {
			
#line 49 "write.x"

	case RS::header: {
		
#line 58 "write.x"

	for (const auto &n : b.value) {
		for (
			int i = 0; i < b.level; ++i
		) {
			out << '#';
		}
		out << ' ';
		out << n << '\n';
	}

#line 51 "write.x"
;
		break;
	}

#line 72 "write.x"

	case RS::code: {
		
#line 81 "write.x"

	out << "```\n";
	for (const auto &n: b.value) {
		out << n << '\n';
	}
	out << "```\n";

#line 74 "write.x"
;
		break;
	}

#line 91 "write.x"

	case RS::para: {
		
#line 143 "write.x"

	bool first = true;
	for (const auto &n: b.value) {
		if (first) {
			first = false;
		} else { out << '\n'; }
		multi_write(out, n, {}, {});
	}

#line 93 "write.x"
;
		break;
	}

#line 40 "write.x"
;
			default: ;
		}
		
#line 155 "write.x"

	for (const auto &n: b.notes) {
		multi_write(out, n, "* ", "  ");
	}

#line 43 "write.x"
;
	}

#line 9 "write.x"
;
		}
	}

#line 226 "write.x"

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

#line 8 "add.x"

	void add_block(Read_State state) {
		
#line 51 "add.x"

	if (curInput != inputs.end()) {
		
#line 71 "add.x"

	if (
		curBlock !=
			curInput->blocks.end()
	) {
		++curBlock;
	}

#line 53 "add.x"
;
		int i = curBlock -
			curInput->blocks.begin();
		
#line 83 "add.x"

	curInput->blocks.insert(
		curBlock,
		{ state, { "REPLACE" }, {}, state == RS::header ? 1 : 0 }
	);

#line 56 "add.x"
;
		curBlock =
			curInput->blocks.begin() + i;
	} else {
		std::cerr << "! no file\n";
	}
	draw_block();

#line 10 "add.x"
;
	}

#line 29 "ncurses.x"

	#if HAVE_CURSES
		
#line 37 "ncurses.x"

	
#line 43 "ncurses.x"

	bool with_ncurses = false;

#line 77 "ncurses.x"

	class Ncurses_Handler {
		public:
			Ncurses_Handler() {
				
#line 91 "ncurses.x"

	setlocale(LC_CTYPE, "");
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();

#line 81 "ncurses.x"
;
			}
			~Ncurses_Handler() {
				
#line 101 "ncurses.x"

	endwin();

#line 84 "ncurses.x"
;
			}
	};

#line 107 "ncurses.x"

	struct End_Of_Curses {};

#line 113 "ncurses.x"

	
#line 149 "ncurses.x"

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

#line 114 "ncurses.x"

	void draw_page() {
		clear();
		move(0, 0);
		
#line 167 "ncurses.x"

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

#line 184 "ncurses.x"

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

#line 199 "ncurses.x"

	if (curBlock->state == RS::para) {
		int i = 0;
		for (const auto &l : curBlock->value) {
			draw_line(++i);
			addstr(l.c_str());
			addstr("\n\n");
		}
	}

#line 212 "ncurses.x"

	int j = 0;
	for (const auto &l : curBlock->notes) {
		draw_line(++j);
		addstr("* ");
		addstr(l.c_str());
		addch('\n');
	}
	addch('\n');

#line 225 "ncurses.x"

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

#line 118 "ncurses.x"
;
		refresh();
	}

#line 38 "ncurses.x"


#line 31 "ncurses.x"

	#endif

#line 39 "index.x"

	int main(
		int argc,
		const char **argv
	) {
		
#line 114 "index.x"

	#if ! NDEBUG
		
#line 301 "frag.x"

	
#line 321 "frag.x"

	testFragName("abc");
	testFragName("");
	testFragName("A c");
	{
		Frag f { "ab" };
		ASSERT(f.empty());
	}

#line 349 "frag.x"

	{
		FragEntry entry;
		ASSERT(! entry.frag);
	}

#line 360 "frag.x"

	{
		Frag f { "" };
		Write_State s { f };
		FragEntry entry;
		ASSERT(entry.str(s).empty());
	}

#line 677 "frag.x"
 {
	Frag frag { "" };
	addStringToFrag(&frag, "abc");
	addStringToFrag(&frag, "def");
	testFrag(frag, "abcdef");
} 
#line 687 "frag.x"
 {
	Frag a { "" };
	Frag b { "" };
	addStringToFrag(&a, "abc");
	b.add(&a);
	addStringToFrag(&b, "def");
	b.add(&a);
	testFrag(b, "abcdefabc");
} 
#line 302 "frag.x"
;

#line 210 "line.x"

	
#line 216 "line.x"

	ASSERT(! Line {});
	ASSERT(Line::begin());
	ASSERT(Line::end());
	ASSERT(Line::end());
	ASSERT(Line::line(0));
	ASSERT(Line::relative(0));
	ASSERT(Line::relative(-2));

#line 228 "line.x"

	ASSERT(Line {}(5, 10) == 5);
	ASSERT(Line::begin()(5, 10) == 0);
	ASSERT(Line::end()(5, 10) == 10);

#line 236 "line.x"

	ASSERT(Line::line(0)(5, 10) == 0);
	ASSERT(Line::line(6)(5, 10) == 6);
	ASSERT(Line::line(20)(5, 10) == 10);

#line 244 "line.x"

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

#line 261 "line.x"
 {
	std::string f = "+3";
	ASSERT(
		get_line(f)(5, 10) == 8
	);
} 
#line 270 "line.x"
 {
	std::string f = ".";
	ASSERT(
		get_line(f)(5, 10) == 5
	);
} 
#line 279 "line.x"
 {
	std::string f = "$";
	ASSERT(
		get_line(f)(5, 10) == 10 
	);
} 
#line 211 "line.x"
;

#line 116 "index.x"
;
	#endif

#line 128 "index.x"

	
#line 221 "index.x"

	for (int i { 1 }; i < argc; ++i) {
		std::string arg { argv[i] };
		
#line 252 "index.x"
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
#line 275 "index.x"
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
#line 1129 "index.x"
 {
	static const std::string prefix {
		"--no-cmds"
	};
	if (arg == prefix) {
		no_cmds = true;
		continue;
	}
} 
#line 30 "view.x"

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

#line 49 "ncurses.x"

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

#line 224 "index.x"
;
		
#line 293 "index.x"

	inputs.add(arg);
	continue;

#line 225 "index.x"
;
		ASSERT_MSG(false,
			"unknown argument [" <<
			argv[i] << ']'
		);
	}

#line 129 "index.x"
;

#line 141 "index.x"

	
#line 101 "read.x"

	read_sources();

#line 142 "index.x"
;

#line 150 "index.x"

	
#line 912 "index.x"

	if (write_files) {
		files_write();
	}

#line 1056 "index.x"

	if (process_files) {
		files_process();
	}

#line 151 "index.x"
;

#line 158 "index.x"

	
#line 17 "html.x"

	if (html_files) {
		write_html();
	}

#line 159 "index.x"
;

#line 45 "view.x"

	if (interactive) {
		
#line 53 "view.x"

	curInput = inputs.begin();
	curBlock = curInput->blocks.begin();

#line 68 "view.x"

	draw_block();
	for (;;) {
		
#line 95 "view.x"

	std::string cmd;
	draw_position();
	std::cout << "> ";
	std::getline(std::cin, cmd);
	trim(cmd);
	if (cmd.empty()) { continue; }
	
#line 56 "range.x"

	range = Range {};
	range << get_line(cmd);
	if (! cmd.empty() && cmd[0] == ',') {
		cmd.erase(0, 1);
		range << get_line(cmd);
	}

#line 102 "view.x"
;

#line 107 "view.x"

	if (cmd == "q" || cmd == "quit") {
		break;
	}

#line 115 "view.x"

	if (cmd == "n" || cmd == "next") {
		int next = (curBlock - curInput->blocks.begin()) + 1;
		while (next >= static_cast<int>(curInput->blocks.size())) {
			--next;
		}
		
#line 67 "range.x"

	if (range) {
		next = range.last()(
			(curBlock - curInput->blocks.begin()) + 1,
			curInput->blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 121 "view.x"
;
		curBlock = curInput->blocks.begin() + next;
		draw_block();
		continue;
	}

#line 130 "view.x"

	if (cmd == "p" || cmd == "prev") {
		int next = curBlock - curInput->blocks.begin();
		if (next > 0) {
			--next;
		}
		
#line 67 "range.x"

	if (range) {
		next = range.last()(
			(curBlock - curInput->blocks.begin()) + 1,
			curInput->blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 136 "view.x"
;
		curBlock = curInput->blocks.begin() + next;
		draw_block();
		continue;
	}

#line 209 "view.x"

	if (cmd == "f" || cmd == "forward") {
		int next =(curInput - inputs.begin()) + 1;
		while (next >= static_cast<int>(inputs.size())) {
			--next;
		}
		
#line 79 "range.x"

	if (range) {
		next = range.last()(
			(curInput - inputs.begin()) + 1,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 215 "view.x"
;
		curInput = inputs.begin() + next;
		curBlock = curInput->blocks.begin();
		draw_block();
		continue;
	}

#line 225 "view.x"

	if (cmd == "b" || cmd == "backward") {
		int next = curInput - inputs.begin();
		if (next) {
			--next;
		}
		
#line 79 "range.x"

	if (range) {
		next = range.last()(
			(curInput - inputs.begin()) + 1,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 231 "view.x"
;
		curInput = inputs.begin() + next;
		curBlock = curInput->blocks.begin();
		draw_block();
		continue;
	}

#line 15 "edit.x"

	if (cmd == "l" || cmd == "list") {
		insert_before(
			"l", curBlock->notes
		);
		continue;
	}

#line 26 "edit.x"

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

#line 70 "edit.x"

	if (cmd == ">>") {
		if (curBlock->state == RS::header) {
			++curBlock->level;
			draw_block();
			continue;
		}
	}

#line 82 "edit.x"

	if (cmd == "<<") {
		if (curBlock->level > 1) {
			--curBlock->level;
			draw_block();
			continue;
		}
	}

#line 16 "write.x"

	if (cmd == "W" || cmd == "Write") {
		write_x();
		continue;
	}

#line 163 "write.x"

	if (cmd == "H" || cmd == "Html") {
		write_x();
		write_html();
		continue;
	}

#line 173 "write.x"

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

#line 194 "write.x"

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

#line 216 "write.x"

	if (cmd == "M" || cmd == "Make") {
		write_x();
		system("make");
		continue;
	}

#line 241 "write.x"
 {
	static const std::string p { "M " };
	if (is_prefix(cmd, p)) {
		write_x();
		system(("make " + cmd.substr(p.size())).c_str());
		continue;
	}
} 
#line 252 "write.x"
 {
	static const std::string p { "Make " };
	if (is_prefix(cmd, p)) {
		write_x();
		system(("make " + cmd.substr(p.size())).c_str());
		continue;
	}
} 
#line 263 "write.x"

	if (cmd == "G" || cmd == "Git") {
		write_x();
		system("git status");
		continue;
	}

#line 273 "write.x"
 {
	static const std::string p { "G " };
	if (is_prefix(cmd, p)) {
		write_x();
		system(("git " + cmd.substr(p.size())).c_str());
		continue;
	}
} 
#line 284 "write.x"
 {
	static const std::string p { "Git " };
	if (is_prefix(cmd, p)) {
		write_x();
		system(("git " + cmd.substr(p.size())).c_str());
		continue;
	}
} 
#line 19 "add.x"

	if (cmd == "h" || cmd == "header") {
		add_block(RS::header);
		continue;
	}

#line 29 "add.x"

	if (cmd == "c" || cmd == "code") {
		add_block(RS::code);
		continue;
	}

#line 39 "add.x"

	if (cmd == "o" || cmd == "other") {
		add_block(RS::para);
		continue;
	}

#line 98 "add.x"

	if (cmd == "d" || cmd == "dup") {
		if (curInput != inputs.end()) {
			if (curBlock !=
				curInput->blocks.end()
			) {
				
#line 118 "add.x"

	int i = curBlock -
		curInput->blocks.begin();
	curInput->blocks.insert(
		curBlock, *curBlock
	);
	curBlock = curInput->blocks.begin() +
		i + 1;

#line 104 "add.x"
;
			}
		} else {
			std::cerr << "! no file\n";
		}
		continue;
	}

#line 71 "view.x"
;
	}

#line 47 "view.x"
;
	}

#line 67 "ncurses.x"

	#if HAVE_CURSES
		if (with_ncurses) {
			
#line 125 "ncurses.x"

	Ncurses_Handler handler;
	curInput = inputs.begin();
	curBlock = curInput->blocks.begin();
	draw_page();
	int ch;
	try {
		for (;;) {
			switch (ch = getch()) {
				
#line 143 "ncurses.x"

	case 'q': throw End_Of_Curses {};

#line 247 "ncurses.x"

	case 'n': {
		int next = (curBlock - curInput->blocks.begin()) + 1;
		while (next >= static_cast<int>(curInput->blocks.size())) {
			--next;
		}
		
#line 67 "range.x"

	if (range) {
		next = range.last()(
			(curBlock - curInput->blocks.begin()) + 1,
			curInput->blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 253 "ncurses.x"
;
		curBlock = curInput->blocks.begin() + next;
		draw_page();
		break;
	}

#line 262 "ncurses.x"

	case 'p' : {
		int next = curBlock - curInput->blocks.begin();
		if (next > 0) {
			--next;
		}
		
#line 67 "range.x"

	if (range) {
		next = range.last()(
			(curBlock - curInput->blocks.begin()) + 1,
			curInput->blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 268 "ncurses.x"
;
		curBlock = curInput->blocks.begin() + next;
		draw_page();
		break;
	}

#line 277 "ncurses.x"

	case 'f': {
		int next =(curInput - inputs.begin()) + 1;
		while (next >= static_cast<int>(inputs.size())) {
			--next;
		}
		
#line 79 "range.x"

	if (range) {
		next = range.last()(
			(curInput - inputs.begin()) + 1,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 283 "ncurses.x"
;
		curInput = inputs.begin() + next;
		curBlock = curInput->blocks.begin();
		draw_page();
		continue;
	}

#line 293 "ncurses.x"

	case 'b': {
		int next = curInput - inputs.begin();
		if (next) {
			--next;
		}
		
#line 79 "range.x"

	if (range) {
		next = range.last()(
			(curInput - inputs.begin()) + 1,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 299 "ncurses.x"
;
		curInput = inputs.begin() + next;
		curBlock = curInput->blocks.begin();
		draw_page();
		continue;
	}

#line 134 "ncurses.x"

			}
			draw_page();
		}
	} catch (const End_Of_Curses &) {}

#line 70 "ncurses.x"
;
		}
	#endif

#line 44 "index.x"

	}
