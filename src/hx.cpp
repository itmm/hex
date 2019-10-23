
#line 38 "index.md"

	
#line 169 "index.md"

	
#line 36 "read.md"

	#include <string>

#line 179 "read.md"

	#include <fstream>

#line 96 "blocks.md"

	#include <vector>

#line 9 "log.md"

	#include <iostream>
	#include <exception>

#line 245 "frag.md"

	#include <vector>

#line 754 "frag.md"

	#include <sstream>

#line 5 "input.md"

	#include <iostream>
	#include <vector>
	#include <filesystem>

#line 540 "index.md"

	#include <algorithm>

#line 994 "index.md"

	#include <functional>
	#include <sstream>

#line 84 "html.md"

	#include <string>

#line 732 "html.md"

	#include <cctype>

#line 778 "html.md"

	#include <set>

#line 111 "line.md"

	#include <limits>

#line 5 "ncurses.md"

	#if defined HAVE_CONFIG_H
		#include "config.h"
	#endif

#line 14 "ncurses.md"

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

#line 31 "ncurses.md"

	#include <locale.h>

#line 170 "index.md"
;

#line 181 "index.md"

	class Frag;
	class Input;
	class Frag_State;

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

	void split_frag(Frag *meta, std::map<std::string, std::string> &&values);
	void clear_frags();
	void eval_metas();

#line 5 "read.md"

	
#line 20 "read.md"

	
#line 64 "read.md"

	struct No_More_Lines {};

#line 148 "read.md"

	
#line 162 "read.md"

	
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

#line 203 "blocks.md"
,
	notes

#line 270 "blocks.md"
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


#line 6 "frag.md"

	class Frag;

	struct Write_State {
		std::string source_name = {};
		bool in_macro = false;
		bool c_style;

		Write_State(const Frag &f);
	};

#line 25 "frag.md"

	
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
		std::cerr << __FILE__ << ':' << \
			__LINE__ << ' '; \
		std::cerr << MSG; \
		 \
	std::cerr << '\n'; \
;
;
	
	class Frag_Entry {
		std::string _str;
		std::string _file;
		int _first_line;
		int _last_line;
	public:
		const Frag *frag;
		
#line 55 "frag.md"

	Frag_Entry(
		Frag *frag = nullptr
	):
		_first_line { -1 },
		frag { frag }
	{}

#line 69 "frag.md"

	void update_state(
		Write_State &state
	) const {
		
#line 82 "frag.md"

	auto c { _str.end() };
	auto b { _str.begin() };
	bool some_nl { false };
	while (b != c) {
		--c;
		
#line 100 "frag.md"

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

#line 88 "frag.md"
;
		break;
	}
	if (b != c && *c > ' ') {
		state.in_macro = false;
	}

#line 73 "frag.md"
;
	}

#line 119 "frag.md"

	std::string str(
		Write_State &state
	) const {
		
#line 140 "frag.md"

	bool old { state.in_macro };
	update_state(state);
	if (old) { return _str; }
	if (! state.c_style) { return _str; }
	if (_first_line < 1) { return _str; }
	if (_str.empty()) { return _str; };

#line 123 "frag.md"
;
		std::ostringstream oss;
		oss << "\n#line " <<
			_first_line << " \"" <<
			_file << "\"\n" << _str;
		return oss.str();
	}

#line 156 "frag.md"

	void add(
		char ch, const std::string &file,
		int line
	) {
		
#line 170 "frag.md"

	if (
		_file.empty() || _first_line <= 0
	) {
		_file = file;
		_first_line = line;
	}
	_last_line = line;

#line 161 "frag.md"
;
		_str += ch;
	}

#line 184 "frag.md"

	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		
#line 170 "frag.md"

	if (
		_file.empty() || _first_line <= 0
	) {
		_file = file;
		_first_line = line;
	}
	_last_line = line;

#line 190 "frag.md"
;
		_str += value;
	}

#line 198 "frag.md"

	bool canAdd(
		const std::string &file,
		int line
	) {
		
#line 212 "frag.md"

	if (
		! _file.empty() && file != _file
	) {
		return false;
	}

#line 224 "frag.md"

	if (
		_last_line > 0 &&
		_last_line != line &&
		_last_line + 1 != line
	) {
		return false;
	}

#line 238 "frag.md"

	return true;

#line 203 "frag.md"
;
		return false;
	}

#line 41 "frag.md"
;
	};

#line 252 "frag.md"

	class Frag {
		std::vector<Frag_Entry> _entries;
		int _expands;
		int _multiples;
		Frag *_prefix;
		const bool _is_meta;
	public:
		const std::string name;
		
#line 272 "frag.md"

	bool isFile() const {
		static const std::string prefix {
			"file: "
		};
		std::string p {
			name.substr(0, prefix.size())
		};
		return p == prefix;
	}

#line 287 "frag.md"

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

#line 304 "frag.md"

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

#line 325 "frag.md"

	const Frag *prefix() const {
		return _prefix;
	}
	Frag *prefix() {
		return _prefix;
	}

#line 337 "frag.md"

	bool is_meta() {
		return _is_meta;
	}

#line 345 "frag.md"

	void clear() {
		if (_prefix) {
			_prefix->clear();
		}
		_entries.clear();
	}

#line 358 "frag.md"

	bool empty() const {
		if (
			_prefix && ! _prefix->empty()
		) {
			return false;
		}
		return _entries.empty();
	}

#line 451 "frag.md"

	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		if (value.empty()) { return; }
		
#line 471 "frag.md"

	if (_entries.empty()) {
		_entries.emplace_back();
	} else if (
		! _entries.back().canAdd(
			file, line
		)
	) {
		_entries.emplace_back();
	}

#line 458 "frag.md"
;
		_entries.back().add(
			value, file, line
		);
	}

#line 487 "frag.md"

	void add(
		char ch,
		const std::string &file,
		int line
	) {
		
#line 471 "frag.md"

	if (_entries.empty()) {
		_entries.emplace_back();
	} else if (
		! _entries.back().canAdd(
			file, line
		)
	) {
		_entries.emplace_back();
	}

#line 493 "frag.md"
;
		_entries.back().add(
			ch, file, line
		);
	}

#line 503 "frag.md"

	Frag &add(Frag *child);

#line 533 "frag.md"

	auto begin() const {
		return _entries.cbegin();
	}

#line 542 "frag.md"

	auto end() const {
		return _entries.cend();
	}

#line 551 "frag.md"

	int expands() const {
		return _expands + (_prefix ? _prefix->expands() : 0);
	}

#line 560 "frag.md"

	void addExpand() {
		++_expands;
	}

#line 569 "frag.md"

	int multiples() const {
		return _multiples + (_prefix ? _prefix->multiples() : 0);
	}

#line 578 "frag.md"

	void addMultiple() {
		++_multiples;
	}

#line 587 "frag.md"

	bool is_c_style() const {
		
#line 597 "frag.md"

	static const std::string exts[] = {
		".c", ".h", ".cpp"
	};
	const std::string *end =
		exts + sizeof(exts)/sizeof(*exts);

#line 608 "frag.md"

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

#line 589 "frag.md"
;
		return false;
	}

#line 261 "frag.md"
;
	};

#line 372 "frag.md"

	Write_State::Write_State(
		const Frag &f
	):
		c_style { f.is_c_style() }
	{ }

#line 390 "frag.md"

	void test_frag_name(
		const std::string &name
	) {
		Frag f(name, nullptr);
		ASSERT(f.name == name);
	}

#line 419 "frag.md"

	bool isPopulatedFrag(
		const Frag *f
	) {
		return f && ! f->empty();
	}

#line 511 "frag.md"

	
#line 811 "frag.md"

	bool isFragInFrag(
		const Frag *needle,
		const Frag *haystack
	) {
		ASSERT(needle);
		ASSERT(haystack);
		
#line 837 "frag.md"

	if (needle == haystack) {
		return true;
	}

#line 818 "frag.md"
;
		
#line 846 "frag.md"

	if (haystack->prefix() && isFragInFrag(needle, haystack->prefix())) {
		return true;
	}
	for (const auto &i : *haystack)  {
		if (! i.frag) { continue; }
		if (isFragInFrag(
			needle, i.frag
		)) {
			return true;
		}
	}

#line 819 "frag.md"
;
		return false;
	}

#line 512 "frag.md"

	Frag &Frag::add(Frag *child) {
		ASSERT(child);
		
#line 828 "frag.md"

	ASSERT(! isFragInFrag(
		this, child
	));

#line 515 "frag.md"
;
		
#line 524 "frag.md"

	_entries.push_back(
		Frag_Entry { child }
	);

#line 516 "frag.md"
;
		return *this;
	}

#line 628 "frag.md"

	void serializeFrag(
		const Frag &frag,
		std::ostream &out,
		Write_State &state
	) {
		
#line 656 "frag.md"

	if (frag.prefix()) {
		serializeFrag(
			*frag.prefix(), out, state
		);
	}
	for (const auto &entry : frag) {
		if (entry.frag) {
			serializeFrag(
				*entry.frag, out,
				state
			);
		}
		out << entry.str(state);
	}

#line 634 "frag.md"
;
	}

#line 641 "frag.md"

	void serializeFrag(
		const Frag &f,
		std::ostream &out
	) {
		Write_State state { f };
		return serializeFrag(
			f, out, state
		);
	}

#line 677 "frag.md"

	bool check_frag(
		const Frag &f,
		std::istream &in,
		Write_State &state
	) {
		
#line 706 "frag.md"

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
		
#line 729 "frag.md"

	for (
		const auto &i : entry.str(state)
	) {
		if (in.get() != i) {
			return false;
		}
	}

#line 721 "frag.md"
;
	}

#line 683 "frag.md"
;
		return true;
	}

#line 691 "frag.md"

	bool check_frag(
		const Frag &f,
		std::istream &in
	) {
		Write_State state { f };
		return check_frag(
			f, in, state
		);
	}

#line 742 "frag.md"

	void testFrag(
		const Frag &frag,
		const std::string &expected
	) {
		
#line 761 "frag.md"

	std::ostringstream buffer;
	serializeFrag(frag, buffer);
	ASSERT(buffer.str() == expected);

#line 747 "frag.md"
;
	}

#line 771 "frag.md"

	void addStringToFrag(
		Frag *frag,
		const std::string &str
	) {
		frag->add(
			str, std::string {}, 0
		);
	}

#line 27 "frag.md"
;

#line 163 "read.md"
;
	class Input {
		public:
			
#line 247 "read.md"

	Input(const std::string &path, const Input *prev):
		prev { prev },
		_path { path }
	{}

#line 258 "read.md"

	Input(const Input &) = delete;
	Input(Input &&) = default;

#line 267 "read.md"

	Input &operator=(
		const Input &
	) = delete;
	Input &operator=(
		Input &&
	) = default;

#line 280 "read.md"

	const std::string &path() const {
		return _path;
	}

#line 115 "blocks.md"

	std::vector<Block> blocks;

#line 166 "read.md"
;
			const Input *prev;
		private:
			std::string _path;
	};

#line 149 "read.md"
;
	class Open_Input {
		public:
			
#line 195 "read.md"

	Open_Input(const std::string &path, const Input *prev):
		_input { path, prev },
		_file { path.c_str() }
	{}

#line 205 "read.md"

	Open_Input(
		const Open_Input &
	) = delete;
	Open_Input(
		Open_Input &&
	) = default;

#line 218 "read.md"

	Open_Input &operator=(
		const Open_Input &
	) = delete;
	Open_Input &operator=(
		Open_Input &&
	) = default;

#line 231 "read.md"

	Input &input() { return _input; }

#line 238 "read.md"

	const Input &input() const {
		return _input;
	}

#line 289 "read.md"

	void read_line(std::string &line) {
		if (_file.is_open()) {
			
#line 302 "read.md"

	if (std::getline(_file, line)) {
		
#line 135 "input.md"

	++_line;

#line 304 "read.md"
;
		return;
	}
	_file.close();

#line 292 "read.md"
;
		}
		throw No_More_Lines {};
	}

#line 33 "blocks.md"

	Read_State state = RS::new_element;

#line 126 "input.md"

	int line() const {
		return _line;
	}

#line 152 "read.md"
;
		private:
			
#line 186 "read.md"

	Input _input;
	std::ifstream _file;

#line 119 "input.md"

	int _line = 0;

#line 154 "read.md"
;
	};

#line 338 "index.md"

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
		
#line 195 "input.md"

	_used.clear();
	_open.clear();
	if (_paths.empty()) {
		
#line 208 "input.md"

	if (std::filesystem::exists(
		"index.md"
	)) {
		_paths.push_back("index.md");
	} else if (std::filesystem::exists(
		"index.x"
	)) {
		_paths.push_back("index.x");
	} else {
		std::cerr << "no input paths\n";
		_paths.push_back("index.md");
	}

#line 199 "input.md"
;
	}
	_current_path = _paths.begin();

#line 111 "read.md"
;
	}

#line 14 "input.md"

	auto &cur() {
		ASSERT (! _open.empty());
		return _open.back();
	}

#line 24 "input.md"

	auto begin() {
		return _used.begin();
	}

#line 33 "input.md"

	auto end() {
		return _used.end();
	}

#line 42 "input.md"

	auto size() const {
		return _used.size();
	}

#line 51 "input.md"

	void push(const std::string &path) {
		const Input *prev = _used.size() ? &_used.back(): nullptr;
		_used.push_back({ path, prev });
		_open.push_back({ path, prev });
	}
	const std::string open_head() const {
		ASSERT(! _open.empty());
		return _open.back().input().path();
	}
	Input *get(const std::string &name) {
		for (auto &i: _used) {
			if (i.path() == name) {
				return &i;
			}
		}
		return nullptr;
	}

#line 75 "input.md"

	void add(const std::string &path) {
		_paths.push_back(path);
		push(path);
	}

#line 90 "input.md"

	bool has(
		const std::string &name
	) const {
		
#line 102 "input.md"

	for (const auto &j : _used) {
		if (j.path() == name) {
			return true;
		}
	}

#line 94 "input.md"
;
		return false;
	}

#line 142 "input.md"

	Frag *get_local(
		Input &i, const std::string &name
	) {
		Frag *got { find_frag(i.path(), name) };
		return got ?: &add_frag(i, name);
	}

#line 155 "input.md"

	Frag *find_global(
		const std::string &name
	) {
		
#line 168 "input.md"

	if (_open.size() > 1) {
		auto i = _open.end() - 2;
		Frag *f { find_frag(
			i->input(), name
		) };
		if (f) { return f; }
	}

#line 159 "input.md"
;
		return find_frag(name);
	}

#line 181 "input.md"

	Frag *get_global(
		const std::string &name
	) {
		Frag *result = find_global(name);
		return result ?: &add_frag(name);
	}

#line 24 "read.md"
;
		private:
			
#line 315 "read.md"

	std::vector<std::string> _paths;
	std::vector<std::string>::
		const_iterator _current_path;

#line 325 "read.md"

	std::vector<Open_Input> _open;
	std::vector<Input> _used;

#line 26 "read.md"
;
	};

#line 51 "read.md"

	void Inputs::read_line(
		std::string &line
	) {
		
#line 337 "read.md"

	for (;;) {
		
#line 356 "read.md"

	if (_open.empty()) {
		if (
			_current_path != _paths.end()
		) {
			push(*_current_path++);
		} else {
			break;
		}
	}

#line 339 "read.md"
;
		try {
			_open.back().read_line(line);
			return;
		}
		catch (const No_More_Lines &) {}
		
#line 371 "read.md"

	auto &f { _open.back().input() };
	
#line 385 "read.md"

	if (f.blocks.empty()) {
		f.blocks.push_back({
			RS::header,
			{ "EMPTY FILE" }, {}
		});
	}

#line 373 "read.md"
;
	for (auto &i : _used) {
		if (i.path() == f.path()) {
			i = std::move(f);
			break;
		}
	}

#line 345 "read.md"
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

	
#line 299 "index.md"

	int blockLimit = -1;

#line 347 "index.md"

	void process_char(
		Frag *frag, char ch, const std::string &cur_path, int cur_line
	) {
		
#line 390 "index.md"

	if (frag) {
		frag->add(ch, cur_path, cur_line);
	}

#line 351 "index.md"
;
	}

#line 506 "index.md"

	inline void expand_cmd_arg(
		Frag *f, const std::string &arg,
		const std::string cur_path, int cur_line
	) {
		auto b = arg.begin();
		auto e = arg.end();
		
#line 547 "index.md"

	while (b != e) {
		auto x = std::find(b, e, '@');
		
#line 565 "index.md"

	f->add(
		std::string { b, x },
		cur_path, cur_line
	);

#line 550 "index.md"
;
		if (x != e) {
			b = x + 1;
			
#line 575 "index.md"

	if (b != e) {
		f->add(*b, cur_path, cur_line);
		++b;
	}

#line 553 "index.md"
;
		} else {
			b = e;
		}
	}

#line 513 "index.md"
;
	}

#line 585 "index.md"

	#define ASSERT_NOT_FRAG() \
		ASSERT_MSG(! frag, '@' << \
			name << "(" << arg << \
			") in frag [" << \
			frag->name << ']' \
		)

#line 597 "index.md"

	#define CHECK_NOT_DEFINED() \
		if (isPopulatedFrag(frag)) { \
			WARN_MSG("frag [" << arg << \
				"] already defined" \
			); \
		}

#line 622 "index.md"

	#define ASSERT_FRAG() \
		ASSERT_MSG(frag, '@' << \
			name << "(" << arg << \
			") in frag [" << \
			frag->name << ']' \
		)

#line 680 "index.md"

	#define CHECK_DEFINED() \
		if (! isPopulatedFrag(frag)) { \
			WARN_MSG("frag [" << arg << \
				"] not defined" \
			); \
		}

#line 715 "index.md"

	void parse_args(const std::string &arg, std::string &pattern, std::map<std::string, std::string> &values) {
		for (unsigned i = 0; i < arg.size(); ++i) {
			if (arg[i] == '@') {
				unsigned j = i + 1;
				while (j < arg.size() && isalpha(arg[j])) { ++j; }
				if (j > i + 1 && j < arg.size() && arg[j] == '(') {
					int cnt = 1;
					unsigned k = j + 1;
					for (; k < arg.size() && cnt; ++k) {
						if (arg[k] == '(') { ++cnt; }
						if (arg[k] == ')') { --cnt; }
					}
					if (! cnt) {
						std::string key { arg.substr(i + 1, j - i - 1) };
						std::string value { arg.substr(j + 1, k - j - 1) };
						values[key] = value;
						pattern += '@';
						pattern += key;
						pattern += '(';
						i = k - 1;
					}
				}
			}
			pattern += arg[i];
		}
	}

#line 92 "read.md"
;
	void read_sources() {
		
#line 119 "read.md"
 {
	inputs.clear();
	clear_frags();
	
#line 380 "index.md"

	Frag *frag { nullptr };

#line 122 "read.md"
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
		
#line 185 "blocks.md"

	blocks.push_back({
		RS::code, {}, {}, 0
	});

#line 162 "blocks.md"
;
		break;
	}

#line 170 "blocks.md"

	if (state == RS::code) {
		if (line == "```") {
			state = RS::after_code;
		} else {
			
#line 194 "blocks.md"

	blocks.back().value.push_back(
		line
	);

#line 175 "blocks.md"
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

#line 210 "blocks.md"

	if (line[0] == '*') {
		if (
			state == RS::header ||
			state == RS::after_code ||
			state == RS::notes
		) {
			state = RS::notes;
			
#line 243 "blocks.md"

	auto b { line.begin() };
	auto e { line.end() };
	for (;
		b != e &&
			(*b == '*' || *b == ' ');
		++b
	) {}
	blocks.back().notes.emplace_back(
		b, e
	);

#line 218 "blocks.md"
;
			break;
		}
	}

#line 229 "blocks.md"

	if (
		line[0] == ' ' &&
		state == RS::notes
	) {
		
#line 261 "blocks.md"

	blocks.back().notes.back() +=
		line;

#line 234 "blocks.md"
;
		break;
	}

#line 278 "blocks.md"

	if (line[0] != ' ') {
		if (
			state == RS::new_element ||
			state == RS::para
		) {
			
#line 295 "blocks.md"

	if (state == RS::new_element) {
		
#line 308 "blocks.md"

	if (blocks.empty() ||
		blocks.back().state != RS::para
	) {
		blocks.push_back({
			RS::para, {}, {}, 0
		});
	}

#line 297 "blocks.md"
;
		blocks.back().value.push_back(
			line
		);
	}

#line 284 "blocks.md"
;
			
#line 321 "blocks.md"

	if (state == RS::para) {
		blocks.back().value.back() +=
			" " + line;
	}

#line 285 "blocks.md"
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

#line 358 "index.md"

	auto end = line.cend();
	Input &input { *inputs.get(inputs.open_head()) };
	std::string cur_path = inputs.cur().input().path();
	int cur_line = inputs.cur().line();
	std::map<std::string, std::string> cmd_values;
	for (
		auto i = line.cbegin();
		i != end; ++i
	) {
		
#line 402 "index.md"

	if (*i == '@') {
		auto nb = i + 1;
		auto ne = nb;
		
#line 420 "index.md"

	while (ne != end && *ne != '(') {
		if (! isalpha(*ne)) {
			ne = end;
			break;
		}
		++ne;
	}

#line 406 "index.md"
;
		if (ne != end && ne != nb) {
			std::string name { nb, ne };
			
#line 434 "index.md"

	auto ab = ne + 1; auto ae = ab;
	while (ae != end && *ae != ')') {
		if (*ae == '@') {
			
#line 454 "index.md"

	if (++ae == end) { break; }
	if (isalpha(*ae)) {
		
#line 463 "index.md"

	auto ac { ae };
	while (isalpha(*ac)) {
		++ac; if (ac == end) { break; }
	}
	if (ac != end && *ac == '(') {
		int cnt = 1; ++ac;
		while (ac != end && cnt != 0) {
			if (*ac == '(') { ++cnt; }
			if (*ac == ')') { --cnt; }
			++ac;
		}
		if (cnt == 0) {
			ae = ac - 1;
		}
	}

#line 457 "index.md"
;
	}

#line 438 "index.md"
;
		}
		++ae;
	}
	if (ae != end) {
		std::string arg {ab, ae};
		
#line 483 "index.md"

	i = ae;
	bool outside = ! frag;
	do {
		if (outside && ! blockLimit) {
			break;
		}
		
#line 609 "index.md"

	if (name == "def") {
		ASSERT_NOT_FRAG();
		frag = inputs.get_local(input, arg);
		CHECK_NOT_DEFINED();
		break;
	}

#line 634 "index.md"
 {
	auto i { cmd_values.find(name) };
	if (i != cmd_values.end()) {
		frag->add(i->second, cur_path, cur_line);
		break;
	}
} 
#line 644 "index.md"

	if (name == "end" || name == "End") {
		ASSERT_FRAG();
		if (frag->is_meta()) {
			if (frag->name == arg) {
				frag = nullptr;
			} else {
				auto f { cur_path };
				auto l { cur_line };
				frag->add('@', f, l);
				frag->add(name, f, l);
				frag->add('(', f, l);
				frag->add(arg, f, l);
				frag->add(')', f, l);
			}
		} else {
			
#line 670 "index.md"

	ASSERT_MSG(frag->name == arg,
		"closing [" << arg <<
		"] != [" << frag->name << ']'
	);

#line 660 "index.md"
;
			frag = nullptr;
		}
		break;
	}

#line 692 "index.md"

	if (name == "add") {
		if (frag && frag->is_meta()) {
			auto f { cur_path };
			auto l { cur_line };
			frag->add('@', f, l);
			frag->add(name, f, l);
			frag->add('(', f, l);
			frag->add(arg, f, l);
			frag->add(')', f, l);
		} else {
			ASSERT_NOT_FRAG();
			frag = inputs.get_local(input, arg);
			CHECK_DEFINED();
		}
		break;
	}

#line 746 "index.md"

	if (name == "put") {
		if (! frag && arg.find('@') != std::string::npos) {
			std::string pattern;
			std::map<std::string, std::string> values;
			parse_args(arg, pattern, values);
			Frag *sub = inputs.get_local(input, pattern);
			split_frag(sub, std::move(values));
		} else {
			ASSERT_MSG(frag, "@put" << "(" <<
				arg << ") not in frag"
			);
			Frag *sub = inputs.get_local(input, arg);
			if (sub) {
				
#line 774 "index.md"

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

#line 760 "index.md"
;
				sub->addExpand();
				frag->add(sub);
			}
		}
		break;
	}

#line 791 "index.md"

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

#line 809 "index.md"

	if (name == "mul") {
		ASSERT_MSG(frag,
			"@mul not in frag"
		);
		Frag *sub = inputs.get_local(input, arg);
		if (sub) {
			
#line 827 "index.md"

	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}

#line 816 "index.md"
;
			sub->addMultiple();
			frag->add(sub);
		}
		break;
	}

#line 840 "index.md"

	if (name == "Def") {
		
#line 850 "index.md"

	ASSERT_MSG(! frag,
		"@Def in frag [" <<
		frag->name << ']'
	);
	frag = inputs.get_global(arg);
	if (isPopulatedFrag(frag)) {
		std::cerr << "Frag [" <<
			arg << "] already defined\n";
	}

#line 842 "index.md"
;
		break;
	}

#line 865 "index.md"

	if (name == "Add") {
		
#line 875 "index.md"

	ASSERT_MSG(! frag, "@Add in frag [" <<
		frag->name << ']'
	);
	frag = inputs.get_global(arg);
	if (! isPopulatedFrag(frag)) {
		std::cerr << "Frag [" << arg <<
			"] not defined\n";
	}

#line 867 "index.md"
;
		break;
	}

#line 891 "index.md"

	if (name == "rep") {
		ASSERT_MSG(! frag,
			"@rep in frag [" <<
				frag->name << ']'
		);
		frag = inputs.get_local(input, arg);
		
#line 921 "index.md"

	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();

#line 898 "index.md"
;
		break;
	}

#line 906 "index.md"

	if (name == "Rep") {
		ASSERT_MSG(! frag,
			"@Rep in frag [" <<
				frag->name << ']'
		);
		frag = inputs.get_global(arg);
		
#line 921 "index.md"

	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();

#line 913 "index.md"
;
		break;
	}

#line 933 "index.md"

	if (name == "Put") {
		
#line 943 "index.md"

	ASSERT_MSG(frag, "@Put not in frag");
	Frag *sub = inputs.get_global(arg);
	if (sub) {
		
#line 774 "index.md"

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

#line 947 "index.md"
;
		sub->addExpand();
		frag->add(sub);
	}

#line 935 "index.md"
;
		break;
	}

#line 956 "index.md"

	if (name == "Mul") {
		
#line 966 "index.md"

	ASSERT_MSG(frag, "@Mul not in frag");
	Frag *sub = inputs.get_global(arg);
	if (sub) {
		
#line 827 "index.md"

	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}

#line 970 "index.md"
;
		sub->addMultiple();
		frag->add(sub);
	}

#line 958 "index.md"
;
		break;
	}

#line 979 "index.md"

	if (name == "priv") {
		ASSERT_MSG(frag,
			"@priv not in frag"
		);
		
#line 1002 "index.md"

	std::hash<std::string> h;
	auto cur {
		h(cur_path + ':' + arg) &
			0x7fffffff
	};

#line 1014 "index.md"

	std::ostringstream hashed;
	hashed << "_private_" <<
		cur << '_' <<
		arg;
	frag->add(
		hashed.str(), cur_path, cur_line
	);

#line 984 "index.md"
;
		break;
	}

#line 1029 "index.md"

	if (name == "magic") {
		ASSERT_MSG(frag,
			"@magic not in frag"
		);
		
#line 1042 "index.md"

	std::hash<std::string> h;
	auto cur {
		h(cur_path + ':' + arg) &
			0x7fffffff
	};

#line 1053 "index.md"

	std::ostringstream value;
	value << cur;
	frag->add(
		value.str(), cur_path, cur_line
	);

#line 1034 "index.md"
;
		break;
	}

#line 490 "index.md"
;
		
#line 520 "index.md"

	if (frag) {
		if (frag->is_meta()) {
			auto f { cur_path };
			auto l { cur_line };
			frag->add('@', f, l);
			frag->add(name, f, l);
			frag->add('(', f, l);
			frag->add(arg, f, l);
			frag->add(')', f, l);
		} else {
			expand_cmd_arg(frag, arg, cur_path, cur_line);
		}
	}

#line 491 "index.md"
;
	} while (false);
	if (blockLimit && outside && frag) {
		--blockLimit;
	}

#line 444 "index.md"
;
		continue;
	}

#line 409 "index.md"
;
		}
	}

#line 368 "index.md"
;
		process_char(frag, *i, cur_path, cur_line);
	}
	process_char(frag, '\n', cur_path, cur_line);

#line 126 "read.md"
;
	} }
	catch (const No_More_Lines &) {}
	eval_metas();
} 
#line 94 "read.md"
;
	}

#line 6 "read.md"
;

#line 272 "index.md"

	std::string stylesheet {
		"slides/slides.css"
	};

#line 1069 "index.md"

	
#line 1167 "index.md"

	std::string file_name(const Frag &f) {
		return f.name.substr(6);
	}

#line 1176 "index.md"

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

#line 1070 "index.md"

	void files_write() {
		
#line 1088 "index.md"

	for (auto &i : frag_map()) {
		const Frag *frag {
			&i.second
		};
		
#line 1117 "index.md"
 {
	if (frag->isFile()) {
		
#line 1197 "index.md"

	if (file_changed(*frag)) {
		std::ofstream out(
			file_name(*frag).c_str()
		);
		serializeFrag(*frag, out);
	}

#line 1119 "index.md"
;
	}
} 
#line 1127 "index.md"
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
#line 1142 "index.md"

	if (frag->multiples() == 1) {
		std::cerr <<
			"multiple frag [" <<
			frag->name <<
			"] only used once\n";
	}

#line 1155 "index.md"

	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not populated\n";
	}

#line 1093 "index.md"
;
	}

#line 1101 "index.md"

	for (auto &j : inputs) {
		for (auto &i : frag_map(j)) {
			const Frag *frag {
				&i.second
			};
			
#line 1117 "index.md"
 {
	if (frag->isFile()) {
		
#line 1197 "index.md"

	if (file_changed(*frag)) {
		std::ofstream out(
			file_name(*frag).c_str()
		);
		serializeFrag(*frag, out);
	}

#line 1119 "index.md"
;
	}
} 
#line 1127 "index.md"
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
#line 1142 "index.md"

	if (frag->multiples() == 1) {
		std::cerr <<
			"multiple frag [" <<
			frag->name <<
			"] only used once\n";
	}

#line 1155 "index.md"

	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not populated\n";
	}

#line 1107 "index.md"
;
		}
	}

#line 1072 "index.md"
;
	}

#line 1209 "index.md"

	
#line 1256 "index.md"

	bool no_cmds = false;

#line 1210 "index.md"
;
	void files_process() {
		
#line 1229 "index.md"

	for (auto &i : frag_map()) {
		const Frag *frag {
			&i.second
		};
		
#line 1263 "index.md"
 {
	const std::string cmd { frag->cmd() };
	if (cmd.size()) {
		
#line 1273 "index.md"

	std::ostringstream out {};
	serializeFrag(*frag, out);
	std::string o { out.str() };
	if (no_cmds) {
		std::cout << o;
	} else {
		
#line 1289 "index.md"

	std::FILE *f {
		popen(cmd.c_str(), "w")
	};
	if (f) {
		std::fwrite(
			o.c_str(), o.size(), 1, f
		);
		pclose(f);
	}

#line 1280 "index.md"
;
	}

#line 1266 "index.md"
;
	}
} 
#line 1234 "index.md"
;
	}

#line 1241 "index.md"

	for (auto &j : inputs) {
		for (auto &i : frag_map(j)) {
			const Frag *frag {
				&i.second
			};
			
#line 1263 "index.md"
 {
	const std::string cmd { frag->cmd() };
	if (cmd.size()) {
		
#line 1273 "index.md"

	std::ostringstream out {};
	serializeFrag(*frag, out);
	std::string o { out.str() };
	if (no_cmds) {
		std::cout << o;
	} else {
		
#line 1289 "index.md"

	std::FILE *f {
		popen(cmd.c_str(), "w")
	};
	if (f) {
		std::fwrite(
			o.c_str(), o.size(), 1, f
		);
		pclose(f);
	}

#line 1280 "index.md"
;
	}

#line 1266 "index.md"
;
	}
} 
#line 1247 "index.md"
;
		}
	}

#line 1212 "index.md"
;
	}

#line 5 "html.md"

	
#line 52 "html.md"

	enum class HtmlState {
		nothing,
		inSlide,
		afterSlide,
		afterSlides
		
#line 317 "html.md"

	, inCode

#line 963 "html.md"

	, inNotes

#line 1112 "html.md"

	, inPara

#line 58 "html.md"

	};

#line 68 "html.md"

	struct HtmlStatus {
		
#line 77 "html.md"

	HtmlState state = HtmlState::nothing;

#line 70 "html.md"

	};

#line 181 "html.md"
 
	void writeOneEscaped(
		std::ostream &out, char ch
	) {
		switch (ch) {
			
#line 212 "html.md"

	case '<':
		out << "&lt;";
		break;
	case '>':
		out << "&gt;";
		break;
	case '&':
		out << "&amp;";
		break;

#line 186 "html.md"

			default:
				out << ch;
		}
	}

#line 197 "html.md"

	void writeEscaped(
		std::ostream &out,
		const std::string &str
	) {
		for (char ch : str) {
			writeOneEscaped(out, ch);
		}
	}

#line 227 "html.md"

	
#line 458 "html.md"

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

#line 785 "html.md"

	using Set = std::set<std::string>;

#line 792 "html.md"

	bool isKeyword(const std::string &s) {
		static Set reserved {
			
#line 809 "html.md"

	"break", "case", "catch", "continue",
	"default", "delete", "else", "for",
	"if", "in", "new", "return", "static",
	"switch", "try", "typeof", "while",
	"class", "public", "private",
	"template", "typename", "using",
	"function", "throw", "namespace",
	"once", "constexpr", "volatile"

#line 795 "html.md"

		};
		return
			reserved.find(s) !=
				reserved.end() ||
					(s.size() &&
						s[0] == '#'
					);
	}

#line 823 "html.md"

	bool isType(const std::string &s) {
		
#line 833 "html.md"

	static Set reserved {
		
#line 847 "html.md"

	"FILE", "auto", "bool", "char",
	"const", "enum", "extern", "int",
	"let", "long", "signed", "struct",
	"union", "unsigned", "void", "double",
	"string", "std", "ifstream",
	"istream", "ofstream", "ostream",
	"vector", "map", "list"

#line 835 "html.md"

	};
	if (reserved.find(s) !=
		reserved.end()
	) {
		return true;
	}

#line 860 "html.md"

	if (s.size() >= 2) {
		if (isupper(s[0]) &&
			islower(s[1])
		) {
			return true;
		}
	}

#line 825 "html.md"
;
		return false;
	}

#line 874 "html.md"

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

#line 893 "html.md"

	void process_ident(
		std::ostream &out,
		const std::string ident,
		char w
	) {
		
#line 906 "html.md"

	if (isKeyword(ident)) {
		span_str(out, "keyword", ident);
	} else if (w == '(') {
		span_str(out, "fn", ident);
	
#line 922 "html.md"

	} else if (isType(ident)) {
		span_str(out, "type", ident);
	} else if (isNum(ident)) {
		span_str(out, "num", ident);

#line 911 "html.md"

	} else {
		span_str(out, "var", ident);
	}

#line 899 "html.md"
;
	}

#line 933 "html.md"

	void writeMacroClass(
		std::ostream &out,
		const char *name
	) {
		out << "<span class=\"" <<
			name << "\">";
	}

#line 946 "html.md"

	void writeMacroHeader(
		std::ostream &out,
		const std::string &name
	) {
		writeMacroClass(out, "macro");
		out << '@' << name <<
			"(<span class=\"name\">";
	}

#line 228 "html.md"
;
	void process_code(
		std::ostream &out,
		SI begin, SI end
	) {
		
#line 394 "html.md"

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

#line 412 "html.md"

	for (; begin != end; ++begin) {
		
#line 423 "html.md"

	if (
		*begin == '`' ||
		*begin == '\'' ||
		*begin == '"'
	) {
		
#line 437 "html.md"

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

#line 474 "html.md"

	std::string name {begin, w + 1};
	span_str(out, "str", name);
	begin = w;

#line 429 "html.md"
;
		continue;
	}

#line 483 "html.md"

	if (*begin == '@') {
		auto nb = begin + 1;
		auto ne = nb;
		
#line 494 "html.md"

	while (ne != end && *ne != '(') {
		if (! isalpha(*ne)) {
			ne = end;
			break;
		}
		++ne;
	}

#line 508 "html.md"

	if (ne != end) {
		std::string name {nb, ne};
		auto ab = ne + 1;
		auto ae = ab;
		
#line 520 "html.md"

	while (ae != end && *ae != ')') {
		if (*ae == '@') {
			++ae;
			if (ae == end) { break; }
		}
		++ae;
	}
	if (ae != end) {
		std::string arg {ab, ae};
		
#line 538 "html.md"

	do {
		
#line 565 "html.md"

	static Set macros = {
		"def", "end", "add", "put", "mul",
		"Def", "Add", "Mul", "rep", "Rep",
		"Put", "End"
	};

#line 576 "html.md"

	if (
		macros.find(name) != macros.end()
	) {
		writeMacroHeader(out, name);
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}

#line 590 "html.md"

	if (name == "inc") {
		
#line 600 "html.md"

	auto ext = arg.find_last_of('.');
	if (ext == std::string::npos) {
		ext = arg.size();
	}
	writeMacroHeader(out, name);
	out << "<a href=\"" <<
		arg.substr(0, ext) <<
		".html\">";
	out << arg <<
		"</a></span>)</span>";

#line 592 "html.md"
;
		break;
	}

#line 616 "html.md"

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

#line 634 "html.md"

	if (name == "f" || name == "fn") {
		writeMacroClass(out, "fn");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 646 "html.md"

	if (name == "v" || name == "var") {
		writeMacroClass(out, "var");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 658 "html.md"

	if (name == "k" || name == "key") {
		writeMacroClass(out, "keyword");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 670 "html.md"

	if (name == "n" || name == "num") {
		writeMacroClass(out, "num");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 682 "html.md"

	if (name == "t" || name == "typ") {
		writeMacroClass(out, "type");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 694 "html.md"

	if (name == "b" || name == "br") {
		writeMacroClass(out, "virt");
		out << "</span><br/>";
		break;
	}

#line 706 "html.md"

	if (name == "priv") {
		writeMacroClass(out, "var");
		out << "@priv(<span>";
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}

#line 719 "html.md"

	if (name == "magic") {
		writeMacroClass(out, "num");
		out << "@magic(<span>";
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}

#line 540 "html.md"
;
		
#line 551 "html.md"

	std::cerr << "unknown macro @" <<
		name << '(' << arg << ")\n";
	writeOneEscaped(out, '@');
	writeEscaped(out, name);
	writeOneEscaped(out, '(');
	writeEscaped(out, arg);
	writeOneEscaped(out, ')');

#line 541 "html.md"
;
	} while (false);
	begin = ae;

#line 530 "html.md"
;
		continue;
	}

#line 513 "html.md"
;
	}

#line 487 "html.md"
;
	}

#line 739 "html.md"

	auto w = begin;
	
#line 751 "html.md"

	while (w != end && (
		std::isalnum(*w) ||
			*w == '_' || *w == '$' ||
			*w == '#'
	)) {
		++w;
	}

#line 741 "html.md"
;
	if (w != begin) {
		
#line 765 "html.md"

	std::string ident {begin, w};
	begin = w - 1;
	process_ident(
		out, ident,
		w != end ? *w : ' '
	);

#line 743 "html.md"
;
		continue;
	}

#line 414 "html.md"
;
		writeOneEscaped(out, *begin);
	}

#line 233 "html.md"
;
	}

#line 243 "html.md"

	void process_content(
		std::ostream &out,
		SI begin, SI end
	) {
		
#line 1020 "html.md"

	for(; begin != end; ++begin) {
		
#line 1032 "html.md"

	if (*begin == '`') {
		
#line 1041 "html.md"

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

#line 1034 "html.md"
;
	}

#line 1058 "html.md"

	if (
		*begin == '*' &&
		(begin + 1) != end &&
		*(begin + 1) == '*'
	) {
		
#line 1071 "html.md"

	auto w = begin + 2;
	while (
		w != end && (w + 1) != end &&
		(*w != '*' || *(w + 1) != '*')
	) {
		++w;
	}

#line 1084 "html.md"

	if (
		w != end && (w + 1 ) != end &&
		*w == '*' && *(w + 1) == '*'
	) {
		
#line 1097 "html.md"

	out << "<b>";
	writeEscaped(
		out, std::string {begin + 2, w}
	);
	out << "</b>";
	begin = w + 1;

#line 1089 "html.md"
;
		continue;
	}

#line 1064 "html.md"
;
	}

#line 1022 "html.md"
;
		writeOneEscaped(out, *begin);
	}

#line 248 "html.md"
;
	}

#line 6 "html.md"

	void write_html() {
		for (auto &cur : inputs) {
			
#line 26 "html.md"

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

	
#line 91 "html.md"

	HtmlStatus status;
	for (const auto &b : cur.blocks) {
		
#line 101 "html.md"

	if (b.state == RS::header) {
		
#line 153 "html.md"

	
#line 268 "html.md"

	switch (status.state) {
		case HtmlState::nothing:
			
#line 286 "html.md"

	out << "<!doctype html>\n";
	out << "<html lang=\"en\">\n";
	out << "<head>\n";
	
#line 298 "html.md"

	out << "<meta charset=\"utf-8\">\n";
	out << "<title>";
	writeEscaped(out, b.value[0]);
	out << "</title>\n";
	out << "<link rel=\"stylesheet\" "
		"type=\"text/css\" href=\""
		<< stylesheet << "\">";

#line 290 "html.md"
;
	out << "</head>\n";
	out << "<body>\n";

#line 271 "html.md"
;
			break;
		case HtmlState::inSlide:
			out << "</div>\n";
			// fallthrough
		default:
			out << "</div>\n";
	}

#line 154 "html.md"
;
	
#line 256 "html.md"
 {
	out << "<h" << b.level << '>';
	const auto &n = b.value[0];
	process_content(
		out, n.begin(), n.end()
	);
	out << "</h" << b.level << ">\n";
} 
#line 155 "html.md"
;
	out << "<div class=\"slides\">\n";
	out << "<div>\n";
	out << "<div>\n";
	
#line 256 "html.md"
 {
	out << "<h" << b.level << '>';
	const auto &n = b.value[0];
	process_content(
		out, n.begin(), n.end()
	);
	out << "</h" << b.level << ">\n";
} 
#line 159 "html.md"
;
	out << "</div>\n";
	status.state = HtmlState::inSlide;

#line 169 "html.md"

	for (const auto &note : b.notes) {
		
#line 981 "html.md"

	auto end = note.end();
	auto begin = note.begin();

#line 989 "html.md"

	if (
		status.state != HtmlState::inNotes
	) {
		
#line 1006 "html.md"

	if (
		status.state != HtmlState::inSlide
	) {
		out << "<div>\n";
	}
	status.state = HtmlState::inNotes;
	out << "<ul><li>\n";

#line 993 "html.md"
;
	} else {
		out << "</li><li>\n";
	}
	process_content(out, begin, end);
	out << '\n';

#line 171 "html.md"
;
	}
	
#line 970 "html.md"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1119 "html.md"

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}

#line 173 "html.md"
;
	
#line 141 "html.md"

	out << "</div>\n";
	status.state = HtmlState::afterSlide;

#line 174 "html.md"
;

#line 103 "html.md"
;
	}

#line 110 "html.md"

	if (b.state == RS::code) {
		
#line 324 "html.md"

	if (
		status.state ==
			HtmlState::afterSlides
	) {
		out << "<div class=\"slides\">\n";
	}

#line 336 "html.md"

	if (
		status.state == HtmlState::inSlide
	) {
		out << "</div>\n";
	}
	out << "<div><div>\n";
	out << "<code>\n";
	status.state = HtmlState::inCode;

#line 112 "html.md"
;
		for (const auto &code : b.value) {
			
#line 383 "html.md"

	process_code(
		out, code.begin(), code.end()
	);
	out << "<br/>\n";

#line 114 "html.md"
;
		}
		
#line 351 "html.md"

	out << "</code></div>\n";
	status.state = HtmlState::inSlide;

#line 116 "html.md"
;
		for (const auto &note : b.notes) {
			
#line 981 "html.md"

	auto end = note.end();
	auto begin = note.begin();

#line 989 "html.md"

	if (
		status.state != HtmlState::inNotes
	) {
		
#line 1006 "html.md"

	if (
		status.state != HtmlState::inSlide
	) {
		out << "<div>\n";
	}
	status.state = HtmlState::inNotes;
	out << "<ul><li>\n";

#line 993 "html.md"
;
	} else {
		out << "</li><li>\n";
	}
	process_content(out, begin, end);
	out << '\n';

#line 118 "html.md"
;
		}
		
#line 970 "html.md"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1119 "html.md"

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}

#line 120 "html.md"
;
		
#line 141 "html.md"

	out << "</div>\n";
	status.state = HtmlState::afterSlide;

#line 121 "html.md"
;
	}

#line 128 "html.md"

	if (b.state == RS::para) {
		for (const auto &para : b.value) {
			
#line 1132 "html.md"

	if (
		status.state ==
			HtmlState::afterSlide
	) {
		out << "</div>\n";
	}

#line 1144 "html.md"

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

#line 131 "html.md"
;
			
#line 970 "html.md"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1119 "html.md"

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}

#line 132 "html.md"
;
		}
		
#line 970 "html.md"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1119 "html.md"

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

#line 94 "html.md"
;
	}

#line 359 "html.md"

	if (
		status.state == HtmlState::inCode
	) {
		std::cerr <<
			"unterminated code block\n";
	}

#line 371 "html.md"

	if (
		status.state != HtmlState::nothing
	) {
		out << "</body>\n</html>\n";
	}

#line 46 "html.md"
;

#line 37 "html.md"
;
	out.close();

#line 9 "html.md"
;
		}
	}

#line 5 "view.md"

	bool interactive = false;

#line 12 "view.md"

	std::vector<Input>::iterator curInput;
	std::vector<Block>::iterator curBlock;

#line 20 "view.md"

	bool write_files = true;

#line 27 "view.md"

	bool process_files = true;

#line 35 "view.md"

	bool html_files = true;

#line 75 "view.md"

	void draw_block() {
		
#line 193 "view.md"

	if (curBlock->state == RS::header) {
		
#line 202 "view.md"

	int i = 0;
	for (
		const auto &l : curBlock->value
	) {
		std::cout << ++i << ": ";
		
#line 216 "view.md"

	for (
		int i = 0;
		i < curBlock->level; ++i
	) {
		std::cout << '#';
	}

#line 208 "view.md"
;
		std::cout << ' ' << l << "\n\n";
	}

#line 195 "view.md"
;
	}

#line 228 "view.md"

	if (curBlock->state == RS::code) {
		
#line 237 "view.md"

	std::cout << "```\n";
	int i = 0;
	for (
		const auto &l : curBlock->value
	) {
		std::cout << ++i << ": " <<
			l << '\n';
	}
	std::cout << "```\n\n";

#line 230 "view.md"
;
	}

#line 252 "view.md"

	if (curBlock->state == RS::para) {
		
#line 261 "view.md"

	int i = 0;
	for (
		const auto &l : curBlock->value
	) {
		std::cout << ++i << ": " <<
			l << "\n\n";
	}

#line 254 "view.md"
;
	}

#line 274 "view.md"

	int j = 0;
	for (
		const auto &l : curBlock->notes
	) {
		std::cout << ++j << ": * " <<
			l << '\n';
	}
	std::cout << '\n';

#line 77 "view.md"
;
	}

#line 94 "view.md"

	void draw_position() {
		
#line 288 "view.md"

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

#line 96 "view.md"
;
	}

#line 103 "view.md"

	void trim(std::string &s) {
		while (
			! s.empty() &&
				(s[0] & 0xff) <= ' '
		) {
			s.erase(0, 1);
		}
	}

#line 5 "line.md"

	class Line {
		public:
			
#line 17 "line.md"

	int operator()(
		int cur, int end
	) const {
		int res {};
		
#line 50 "line.md"

	if (! *this) {
		res = cur;
	} else {
		res = _line;
		if (_relative) { res += cur; }
	}
	if (res < 0) { res = 0; }
	if (res > end) { res = end; }

#line 22 "line.md"
;
		return res;
	}

#line 40 "line.md"

	operator bool() const {
		return _line >= 0 || _relative;
	}

#line 77 "line.md"

	Line() = default;

#line 84 "line.md"

	static Line relative(int line) {
		return Line { line, true };
	}

#line 93 "line.md"

	static Line line(int line) {
		return Line { line, false };
	}

#line 102 "line.md"

	static Line begin() {
		return line(0);
	}

#line 118 "line.md"

	static const int max =
		std::numeric_limits<int>::max();
		
	static Line end() {
		return line(max);
	}

#line 8 "line.md"
;
		private:
			
#line 31 "line.md"

	int _line { -1 };
	bool _relative { false };

#line 67 "line.md"

	Line(int line, bool relative):
		_line { line },
		_relative { relative }
	{}

#line 10 "line.md"
;
	};

#line 130 "line.md"

	
#line 5 "range.md"

	class Range {
		public:
			
#line 18 "range.md"

	Line prev() {
		return _prev ?: _last;
	}

#line 29 "range.md"

	Line last() {
		return _last;
	}

#line 38 "range.md"

	Range &operator<<(const Line &l) {
		_prev = _last;
		_last = l;
		return *this;
	}

#line 49 "range.md"

	operator bool() {
		return _last;
	}

#line 8 "range.md"
;
		private:
			Line _prev;
			Line _last;
	};

#line 131 "line.md"
;
	
#line 134 "range.md"

	Range range;

#line 132 "line.md"
;

#line 146 "line.md"

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

#line 162 "line.md"

	Line get_line(std::string &s) {
		Line line {};
		do {
			
#line 177 "line.md"

	if (s[0] == '.') {
		s.erase(0, 1);
		line = Line::relative(0);
		break;
	}

#line 188 "line.md"

	if (s[0] == '+') {
		s.erase(0, 1);
		int n = get_number(s);
		line = Line::relative(n);
		break;
	}

#line 200 "line.md"

	if (s[0] == '-') {
		s.erase(0, 1);
		int n = -get_number(s);
		line = Line::relative(n);
		break;
	}

#line 212 "line.md"

	if (s[0] == '$') {
		line = Line::end();
		s.erase(0, 1);
		continue;
	}

#line 223 "line.md"

	if (isdigit(s[0])) {
		int n = get_number(s);
		line = Line::line(n);
		continue;
	}

#line 166 "line.md"
;
		} while (false);
		trim(s);
		return line;
	}

#line 5 "edit.md"

	void insert_before(
		const std::string &prefix,
		std::vector<std::string> &c
	) {
		
#line 61 "edit.md"

	int next = c.size();
	
#line 104 "range.md"

	if (range) {
		next = range.last()(
			Line::max, c.size() + 1
		) - 1;
		if (next < 0) { next = 0; }
		int p = range.prev()(
			Line::max, c.size() + 1
		) - 1;
		if (p < 0) { p = 0; }
		
#line 121 "range.md"

	if (p < next) {
		c.erase(
			c.begin() + p,
			c.begin() + next
		);
		next = p;
	}

#line 114 "range.md"
;
	}

#line 63 "edit.md"
;
	std::string l;
	for (;;) {
		
#line 75 "edit.md"

	std::cout << prefix << ' ' <<
		(next + 1) << "? ";
	std::getline(std::cin, l);
	auto b = l.begin();
	auto e = l.end();
	while (b != e && *b <= ' ') { ++b; }
	std::string t { b, e };
	if (t.empty()) { continue; }
	if (t == ".") { break; }
	c.insert(c.begin() + next, l);
	++next;
	continue;

#line 66 "edit.md"
;
		break;
	}
	draw_block();

#line 10 "edit.md"
;
	}

#line 5 "write.md"

	
#line 109 "write.md"

	std::string split(
		std::string &s, int width
	) {
		auto b { s.begin() };
		auto e { s.end() };
		
#line 127 "write.md"

	while (b != e && *b == ' ') {
		++b;
	}

#line 115 "write.md"
;
		auto c = b;
		
#line 136 "write.md"

	while (c != e) {
		auto t = c;
		
#line 151 "write.md"

	while (t != e && *t == ' ') {
		++t;
	}
	while (t != e && *t != ' ') {
		++t;
	}

#line 139 "write.md"
;
		if (c == b || t - b <= width) {
			c = t;
		} else {
			break;
		}
	}

#line 117 "write.md"
;
		std::string res { b, c };
		s.erase(s.begin(), c);
		return res;
	}

#line 164 "write.md"

	void multi_write(
		std::ofstream &out,
		std::string str,
		std::string first_in,
		const std::string &other_in
	) {
		
#line 178 "write.md"

	while (! str.empty()) {
		std::string p = split(
			str, 72 - first_in.size()
		);
		out << first_in << p << '\n';
		first_in = other_in;
	}

#line 171 "write.md"
;
	}

#line 6 "write.md"

	void write_input() {
		for (const auto &cur : inputs) {
			
#line 27 "write.md"

	std::ofstream out {
		cur.path().c_str()
	};

#line 36 "write.md"

	bool first = true;
	for (const auto &b : cur.blocks) {
		if (first) {
			first = false;
		} else { out << '\n'; }
		switch (b.state) {
			
#line 53 "write.md"

	case RS::header: {
		
#line 63 "write.md"

	for (const auto &n : b.value) {
		for (
			int i = 0; i < b.level; ++i
		) {
			out << '#';
		}
		out << ' ';
		out << n << '\n';
	}

#line 55 "write.md"
;
		break;
	}

#line 78 "write.md"

	case RS::code: {
		
#line 88 "write.md"

	out << "```\n";
	for (const auto &n: b.value) {
		out << n << '\n';
	}
	out << "```\n";

#line 80 "write.md"
;
		break;
	}

#line 99 "write.md"

	case RS::para: {
		
#line 191 "write.md"

	bool first = true;
	for (const auto &n: b.value) {
		if (first) {
			first = false;
		} else { out << '\n'; }
		multi_write(out, n, {}, {});
	}

#line 101 "write.md"
;
		break;
	}

#line 43 "write.md"
;
			default: ;
		}
		
#line 204 "write.md"

	for (const auto &n: b.notes) {
		multi_write(out, n, "* ", "  ");
	}

#line 46 "write.md"
;
	}

#line 9 "write.md"
;
		}
	}

#line 302 "write.md"

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
		{
			state, { "REPLACE" }, {},
			state == RS::header ? 1 : 0
		}
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

#line 38 "ncurses.md"

	#if HAVE_CURSES
		
#line 47 "ncurses.md"

	
#line 54 "ncurses.md"

	bool with_ncurses = false;

#line 99 "ncurses.md"

	class Ncurses_Handler {
		public:
			Ncurses_Handler() {
				
#line 114 "ncurses.md"

	setlocale(LC_CTYPE, "");
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();

#line 103 "ncurses.md"
;
			}
			~Ncurses_Handler() {
				
#line 125 "ncurses.md"

	endwin();

#line 106 "ncurses.md"
;
			}
	};

#line 132 "ncurses.md"

	struct End_Of_Curses {};

#line 139 "ncurses.md"

	
#line 185 "ncurses.md"

	void draw_number(int l) {
		int r = l / 10;
		if (r) { draw_number(r); }
		addch((l % 10) + '0');
	}

#line 196 "ncurses.md"

	void draw_line(int l) {
		if (l <= 9) {
			addch(' ');
		}
		draw_number(l);
		addstr(": ");
	}

#line 140 "ncurses.md"

	void draw_page() {
		clear();
		move(0, 0);
		
#line 210 "ncurses.md"

	if (curBlock->state == RS::header) {
		
#line 219 "ncurses.md"

	int i = 0;
	for (
		const auto &l : curBlock->value
	) {
		
#line 231 "ncurses.md"

	draw_line(++i);
	for (
		int j = 0; j < curBlock->level;
		++j
	) {
		addch('#');
	}
	addch(' ');
	addstr(l.c_str());
	addstr("\n\n");

#line 224 "ncurses.md"
;
	}

#line 212 "ncurses.md"
;
	}

#line 247 "ncurses.md"

	if (curBlock->state == RS::code) {
		
#line 256 "ncurses.md"

	addstr("    ```\n");
	int i = 0;
	for (
		const auto &l : curBlock->value
	) {
		draw_line(++i);
		addstr(l.c_str());
		addch('\n');
	}
	addstr("    ```\n\n");

#line 249 "ncurses.md"
;
	}

#line 272 "ncurses.md"

	if (curBlock->state == RS::para) {
		
#line 281 "ncurses.md"

	int i = 0;
	for (
		const auto &l : curBlock->value
	) {
		draw_line(++i);
		addstr(l.c_str());
		addstr("\n\n");
	}

#line 274 "ncurses.md"
;
	}

#line 295 "ncurses.md"

	int j = 0;
	for (
		const auto &l : curBlock->notes
	) {
		draw_line(++j);
		addstr("* ");
		addstr(l.c_str());
		addch('\n');
	}
	addch('\n');

#line 311 "ncurses.md"

	int idx =
		(curInput - inputs.begin()) + 1;
	draw_number(idx);
	if (idx ==
		static_cast<int>(inputs.size())
	) {
		addstr(" = $");
	}
	addch(' ');
	addstr(curInput->path().c_str());
	addch(':');

#line 329 "ncurses.md"

	auto &bs { curInput->blocks };
	idx = (curBlock - bs.begin()) + 1;
	draw_number(idx);
	if (
		idx == static_cast<int>(bs.size())
	) {
		addstr(" = $");
	}

#line 144 "ncurses.md"
;
		refresh();
	}

#line 48 "ncurses.md"


#line 40 "ncurses.md"

	#endif

#line 1363 "index.md"

	using Inputs_Frag_Map = std::map<std::string, Frag_Map>;
	class Frag_State {
		public:
			std::unique_ptr<Frag_State> parent;
			Inputs_Frag_Map state;
			Frag_State(std::unique_ptr<Frag_State> &&parent): parent { std::move(parent) } { }
			Frag *meta = nullptr;
			std::string meta_path;
			std::map<std::string, std::string> meta_values;
	};
	std::unique_ptr<Frag_State> _all_frags = std::move(std::make_unique<Frag_State>(nullptr));

	Frag *find_frag(Frag_State &s, const std::string &in, const std::string &key) {
		auto got { s.state[in].find(key) };
		if (got != s.state[in].end()) {
			return &got->second;
		}
		return nullptr;
	}
	Frag *find_frag(const std::string &in, const std::string &key) {
		return find_frag(*_all_frags, in, key);
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

	Frag &add_frag(Frag_State &state, const std::string &in, const std::string &key) {
		Frag *prev { nullptr };
		if (state.parent) {
			prev = &add_frag(*state.parent, in, key);
		}
		return state.state[in].insert({ key, { key, prev } }).first->second;
	}

	Frag &add_frag(const std::string &in, const std::string &key) {
		return add_frag(*_all_frags, in, key);
	}
	Frag &add_frag(const Input &in, const std::string &key) {
		return add_frag(in.path(), key);
	}
	Frag &add_frag(const std::string &key) {
		return add_frag(std::string { }, key);
	}

	Frag_Map &frag_map(const std::string &in) {
		return _all_frags->state[in];
	}

	Frag_Map &frag_map(const Input &in) {
		return frag_map(in.path());
	}
	Frag_Map &frag_map() {
		return frag_map(std::string { });
	}

	void split_frag(Frag *meta, std::map<std::string, std::string> &&values) {
		Frag_State *current = &*_all_frags;
		current->meta = meta;
		current->meta_path = inputs.open_head();
		current->meta_values = std::move(values);
		auto n { std::make_unique<Frag_State>(std::move(_all_frags)) };
		for (auto &i : current->state) {
			for (auto &j : i.second) {
				n->state[i.first].insert({ j.first, { j.first, &j.second } });
			}
		}
		_all_frags = std::move(n);
	}

	void clear_frags() { _all_frags = std::move(std::make_unique<Frag_State>(nullptr)); }

	void eval_meta(Frag_State &fs) {
		if (fs.parent) {
			eval_meta(*fs.parent);
		}
		if (fs.meta) {
			
#line 1460 "index.md"

	std::ostringstream out;
	serializeFrag(*fs.meta, out);
	std::istringstream in { out.str() };
	std::string line;
	Frag *frag = nullptr;
	Input *ci { inputs.get(fs.meta_path) };
	ASSERT(ci);
	Input &input = *ci;
	std::string cur_path = fs.meta_path;
	int cur_line = -1;
	auto &cmd_values = fs.meta_values;
	while (std::getline(in, line)) {
		auto end = line.cend();
		for (
			auto i = line.cbegin();
			i != end; ++i
		) {
			
#line 402 "index.md"

	if (*i == '@') {
		auto nb = i + 1;
		auto ne = nb;
		
#line 420 "index.md"

	while (ne != end && *ne != '(') {
		if (! isalpha(*ne)) {
			ne = end;
			break;
		}
		++ne;
	}

#line 406 "index.md"
;
		if (ne != end && ne != nb) {
			std::string name { nb, ne };
			
#line 434 "index.md"

	auto ab = ne + 1; auto ae = ab;
	while (ae != end && *ae != ')') {
		if (*ae == '@') {
			
#line 454 "index.md"

	if (++ae == end) { break; }
	if (isalpha(*ae)) {
		
#line 463 "index.md"

	auto ac { ae };
	while (isalpha(*ac)) {
		++ac; if (ac == end) { break; }
	}
	if (ac != end && *ac == '(') {
		int cnt = 1; ++ac;
		while (ac != end && cnt != 0) {
			if (*ac == '(') { ++cnt; }
			if (*ac == ')') { --cnt; }
			++ac;
		}
		if (cnt == 0) {
			ae = ac - 1;
		}
	}

#line 457 "index.md"
;
	}

#line 438 "index.md"
;
		}
		++ae;
	}
	if (ae != end) {
		std::string arg {ab, ae};
		
#line 483 "index.md"

	i = ae;
	bool outside = ! frag;
	do {
		if (outside && ! blockLimit) {
			break;
		}
		
#line 609 "index.md"

	if (name == "def") {
		ASSERT_NOT_FRAG();
		frag = inputs.get_local(input, arg);
		CHECK_NOT_DEFINED();
		break;
	}

#line 634 "index.md"
 {
	auto i { cmd_values.find(name) };
	if (i != cmd_values.end()) {
		frag->add(i->second, cur_path, cur_line);
		break;
	}
} 
#line 644 "index.md"

	if (name == "end" || name == "End") {
		ASSERT_FRAG();
		if (frag->is_meta()) {
			if (frag->name == arg) {
				frag = nullptr;
			} else {
				auto f { cur_path };
				auto l { cur_line };
				frag->add('@', f, l);
				frag->add(name, f, l);
				frag->add('(', f, l);
				frag->add(arg, f, l);
				frag->add(')', f, l);
			}
		} else {
			
#line 670 "index.md"

	ASSERT_MSG(frag->name == arg,
		"closing [" << arg <<
		"] != [" << frag->name << ']'
	);

#line 660 "index.md"
;
			frag = nullptr;
		}
		break;
	}

#line 692 "index.md"

	if (name == "add") {
		if (frag && frag->is_meta()) {
			auto f { cur_path };
			auto l { cur_line };
			frag->add('@', f, l);
			frag->add(name, f, l);
			frag->add('(', f, l);
			frag->add(arg, f, l);
			frag->add(')', f, l);
		} else {
			ASSERT_NOT_FRAG();
			frag = inputs.get_local(input, arg);
			CHECK_DEFINED();
		}
		break;
	}

#line 746 "index.md"

	if (name == "put") {
		if (! frag && arg.find('@') != std::string::npos) {
			std::string pattern;
			std::map<std::string, std::string> values;
			parse_args(arg, pattern, values);
			Frag *sub = inputs.get_local(input, pattern);
			split_frag(sub, std::move(values));
		} else {
			ASSERT_MSG(frag, "@put" << "(" <<
				arg << ") not in frag"
			);
			Frag *sub = inputs.get_local(input, arg);
			if (sub) {
				
#line 774 "index.md"

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

#line 760 "index.md"
;
				sub->addExpand();
				frag->add(sub);
			}
		}
		break;
	}

#line 791 "index.md"

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

#line 809 "index.md"

	if (name == "mul") {
		ASSERT_MSG(frag,
			"@mul not in frag"
		);
		Frag *sub = inputs.get_local(input, arg);
		if (sub) {
			
#line 827 "index.md"

	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}

#line 816 "index.md"
;
			sub->addMultiple();
			frag->add(sub);
		}
		break;
	}

#line 840 "index.md"

	if (name == "Def") {
		
#line 850 "index.md"

	ASSERT_MSG(! frag,
		"@Def in frag [" <<
		frag->name << ']'
	);
	frag = inputs.get_global(arg);
	if (isPopulatedFrag(frag)) {
		std::cerr << "Frag [" <<
			arg << "] already defined\n";
	}

#line 842 "index.md"
;
		break;
	}

#line 865 "index.md"

	if (name == "Add") {
		
#line 875 "index.md"

	ASSERT_MSG(! frag, "@Add in frag [" <<
		frag->name << ']'
	);
	frag = inputs.get_global(arg);
	if (! isPopulatedFrag(frag)) {
		std::cerr << "Frag [" << arg <<
			"] not defined\n";
	}

#line 867 "index.md"
;
		break;
	}

#line 891 "index.md"

	if (name == "rep") {
		ASSERT_MSG(! frag,
			"@rep in frag [" <<
				frag->name << ']'
		);
		frag = inputs.get_local(input, arg);
		
#line 921 "index.md"

	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();

#line 898 "index.md"
;
		break;
	}

#line 906 "index.md"

	if (name == "Rep") {
		ASSERT_MSG(! frag,
			"@Rep in frag [" <<
				frag->name << ']'
		);
		frag = inputs.get_global(arg);
		
#line 921 "index.md"

	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();

#line 913 "index.md"
;
		break;
	}

#line 933 "index.md"

	if (name == "Put") {
		
#line 943 "index.md"

	ASSERT_MSG(frag, "@Put not in frag");
	Frag *sub = inputs.get_global(arg);
	if (sub) {
		
#line 774 "index.md"

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

#line 947 "index.md"
;
		sub->addExpand();
		frag->add(sub);
	}

#line 935 "index.md"
;
		break;
	}

#line 956 "index.md"

	if (name == "Mul") {
		
#line 966 "index.md"

	ASSERT_MSG(frag, "@Mul not in frag");
	Frag *sub = inputs.get_global(arg);
	if (sub) {
		
#line 827 "index.md"

	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}

#line 970 "index.md"
;
		sub->addMultiple();
		frag->add(sub);
	}

#line 958 "index.md"
;
		break;
	}

#line 979 "index.md"

	if (name == "priv") {
		ASSERT_MSG(frag,
			"@priv not in frag"
		);
		
#line 1002 "index.md"

	std::hash<std::string> h;
	auto cur {
		h(cur_path + ':' + arg) &
			0x7fffffff
	};

#line 1014 "index.md"

	std::ostringstream hashed;
	hashed << "_private_" <<
		cur << '_' <<
		arg;
	frag->add(
		hashed.str(), cur_path, cur_line
	);

#line 984 "index.md"
;
		break;
	}

#line 1029 "index.md"

	if (name == "magic") {
		ASSERT_MSG(frag,
			"@magic not in frag"
		);
		
#line 1042 "index.md"

	std::hash<std::string> h;
	auto cur {
		h(cur_path + ':' + arg) &
			0x7fffffff
	};

#line 1053 "index.md"

	std::ostringstream value;
	value << cur;
	frag->add(
		value.str(), cur_path, cur_line
	);

#line 1034 "index.md"
;
		break;
	}

#line 490 "index.md"
;
		
#line 520 "index.md"

	if (frag) {
		if (frag->is_meta()) {
			auto f { cur_path };
			auto l { cur_line };
			frag->add('@', f, l);
			frag->add(name, f, l);
			frag->add('(', f, l);
			frag->add(arg, f, l);
			frag->add(')', f, l);
		} else {
			expand_cmd_arg(frag, arg, cur_path, cur_line);
		}
	}

#line 491 "index.md"
;
	} while (false);
	if (blockLimit && outside && frag) {
		--blockLimit;
	}

#line 444 "index.md"
;
		continue;
	}

#line 409 "index.md"
;
		}
	}

#line 1478 "index.md"
;
			process_char(frag, *i, cur_path, cur_line);
		}
		process_char(frag, '\n', cur_path, cur_line);
	}

#line 1449 "index.md"
;
		}
	}

	void eval_metas() {
		// eval_meta(*_all_frags);
	}

#line 39 "index.md"

	int main(
		int argc,
		const char **argv
	) {
		
#line 115 "index.md"

	#if ! NDEBUG
		
#line 383 "frag.md"

	
#line 402 "frag.md"

	test_frag_name("abc");
	test_frag_name("");
	test_frag_name("A c");

#line 411 "frag.md"
 {
	Frag f { "ab", nullptr };
	ASSERT(f.empty());
} 
#line 430 "frag.md"
 {
	Frag_Entry entry;
	ASSERT(! entry.frag);
} 
#line 438 "frag.md"
 {
	Frag f { "", nullptr };
	Write_State s { f };
	Frag_Entry entry;
	ASSERT(entry.str(s).empty());
} 
#line 785 "frag.md"
 {
	Frag frag { "", nullptr };
	addStringToFrag(&frag, "abc");
	addStringToFrag(&frag, "def");
	testFrag(frag, "abcdef");
} 
#line 795 "frag.md"
 {
	Frag a { "", nullptr };
	Frag b { "", nullptr };
	addStringToFrag(&a, "abc");
	b.add(&a);
	addStringToFrag(&b, "def");
	b.add(&a);
	testFrag(b, "abcdefabc");
} 
#line 384 "frag.md"
;

#line 241 "line.md"

	
#line 248 "line.md"

	ASSERT(! Line {});

#line 255 "line.md"

	ASSERT(Line::begin());
	ASSERT(Line::end());

#line 263 "line.md"

	ASSERT(Line::line(0));

#line 270 "line.md"

	ASSERT(Line::relative(0));
	ASSERT(Line::relative(-2));

#line 278 "line.md"

	ASSERT(Line {}(5, 10) == 5);

#line 285 "line.md"

	ASSERT(Line::begin()(5, 10) == 0);
	ASSERT(Line::end()(5, 10) == 10);

#line 293 "line.md"

	ASSERT(Line::line(0)(5, 10) == 0);
	ASSERT(Line::line(6)(5, 10) == 6);
	ASSERT(Line::line(20)(5, 10) == 10);

#line 303 "line.md"

	ASSERT(
		Line::relative(2)(5, 10) == 7
	);

#line 312 "line.md"

	ASSERT(
		Line::relative(7)(5, 10) == 10
	);

#line 321 "line.md"

	ASSERT(
		Line::relative(-2)(5, 10) == 3
	);

#line 330 "line.md"

	ASSERT(
		Line::relative(-7)(5, 10) == 0
	);

#line 339 "line.md"
 {
	std::string f = "+3";
	ASSERT(
		get_line(f)(5, 10) == 8
	);
} 
#line 349 "line.md"
 {
	std::string f = ".";
	ASSERT(
		get_line(f)(5, 10) == 5
	);
} 
#line 359 "line.md"
 {
	std::string f = "$";
	ASSERT(
		get_line(f)(5, 10) == 10 
	);
} 
#line 242 "line.md"
;

#line 117 "index.md"
;
	#endif

#line 132 "index.md"

	
#line 252 "index.md"

	for (int i { 1 }; i < argc; ++i) {
		std::string arg { argv[i] };
		
#line 283 "index.md"
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
#line 306 "index.md"
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
#line 1304 "index.md"
 {
	static const std::string prefix {
		"--no-cmds"
	};
	if (arg == prefix) {
		no_cmds = true;
		continue;
	}
} 
#line 42 "view.md"

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

#line 61 "ncurses.md"

	#if HAVE_CURSES
		if (
			arg == "-c" ||
			arg == "--curses"
		) {
			
#line 76 "ncurses.md"

	with_ncurses = true;
	interactive = false;
	write_files = false;
	process_files = false;
	html_files = false;

#line 67 "ncurses.md"
;
			continue;
		}
	#endif

#line 255 "index.md"
;
		
#line 324 "index.md"

	inputs.add(arg);
	continue;

#line 256 "index.md"
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

	
#line 1079 "index.md"

	if (write_files) {
		files_write();
	}

#line 1220 "index.md"

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

#line 58 "view.md"

	if (interactive) {
		
#line 67 "view.md"

	curInput = inputs.begin();
	curBlock = curInput->blocks.begin();

#line 84 "view.md"

	draw_block();
	for (;;) {
		
#line 117 "view.md"

	std::string cmd;
	draw_position();
	std::cout << "> ";
	std::getline(std::cin, cmd);
	trim(cmd);
	if (cmd.empty()) { continue; }
	
#line 65 "range.md"

	range = Range {};
	range << get_line(cmd);
	if (! cmd.empty() && cmd[0] == ',') {
		cmd.erase(0, 1);
		range << get_line(cmd);
	}

#line 124 "view.md"
;

#line 132 "view.md"

	if (cmd == "q" || cmd == "quit") {
		break;
	}

#line 141 "view.md"

	if (cmd == "n" || cmd == "next") {
		
#line 152 "view.md"

	int next = (curBlock -
		curInput->blocks.begin()) + 1;
	while (next >= static_cast<int>(
		curInput->blocks.size()
	)) {
		--next;
	}
	
#line 78 "range.md"

	if (range) {
		next = range.last()((curBlock -
			curInput->blocks.begin()) + 1,
			curInput->blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 160 "view.md"
;
	curBlock =
		curInput->blocks.begin() + next;

#line 143 "view.md"
;
		draw_block();
		continue;
	}

#line 168 "view.md"

	if (cmd == "p" || cmd == "prev") {
		
#line 179 "view.md"

	int next =curBlock -
		curInput->blocks.begin();
	if (next > 0) {
		--next;
	}
	
#line 78 "range.md"

	if (range) {
		next = range.last()((curBlock -
			curInput->blocks.begin()) + 1,
			curInput->blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 185 "view.md"
;
	curBlock =
		curInput->blocks.begin() + next;

#line 170 "view.md"
;
		draw_block();
		continue;
	}

#line 306 "view.md"

	if (cmd == "f" || cmd == "forward") {
		
#line 317 "view.md"

	int next =
		(curInput - inputs.begin()) + 1;
	while (next >= static_cast<int>(
		inputs.size()
	)) {
		--next;
	}
	
#line 91 "range.md"

	if (range) {
		next = range.last()((curInput -
				inputs.begin()) + 1,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 325 "view.md"
;
	curInput = inputs.begin() + next;
	curBlock = curInput->blocks.begin();

#line 308 "view.md"
;
		draw_block();
		continue;
	}

#line 333 "view.md"

	if (cmd == "b" || cmd == "backward") {
		
#line 344 "view.md"

	int next = curInput - inputs.begin();
	if (next) {
		--next;
	}
	
#line 91 "range.md"

	if (range) {
		next = range.last()((curInput -
				inputs.begin()) + 1,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 349 "view.md"
;
	curInput = inputs.begin() + next;
	curBlock = curInput->blocks.begin();

#line 335 "view.md"
;
		draw_block();
		continue;
	}

#line 18 "edit.md"

	if (cmd == "l" || cmd == "list") {
		insert_before(
			"l", curBlock->notes
		);
		continue;
	}

#line 30 "edit.md"

	if (cmd == "a" || cmd == "add") {
		std::string prefix;
		
#line 45 "edit.md"

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

#line 33 "edit.md"
;
		insert_before(
			prefix,
			curBlock->value
		);
		continue;
	}

#line 94 "edit.md"

	if (cmd == ">>") {
		if (
			curBlock->state == RS::header
		) {
			++curBlock->level;
			draw_block();
			continue;
		}
	}

#line 109 "edit.md"

	if (cmd == "<<") {
		if (curBlock->level > 1) {
			--curBlock->level;
			draw_block();
			continue;
		}
	}

#line 17 "write.md"

	if (cmd == "W" || cmd == "Write") {
		write_input();
		continue;
	}

#line 213 "write.md"

	if (cmd == "H" || cmd == "Html") {
		write_input();
		write_html();
		continue;
	}

#line 225 "write.md"

	if (cmd == "F" || cmd == "Files") {
		write_input();
		write_html();
		Inputs old { std::move(inputs) };
		
#line 243 "write.md"

	try {
		read_sources();
		files_write();
	} catch (...) {
		std::cerr << "!! aborted\n";
		inputs = std::move(old);
	}

#line 230 "write.md"
;
		curInput = inputs.begin();
		curBlock =
			curInput->blocks.begin();
		continue;
	}

#line 256 "write.md"

	if (cmd == "P" || cmd == "Process") {
		write_input();
		write_html();
		Inputs old { std::move(inputs) };
		
#line 275 "write.md"

	try {
		read_sources();
		files_write();
		files_process();
	} catch (...) {
		std::cerr << "!! aborted\n";
		inputs = std::move(old);
	}

#line 261 "write.md"
;
		curInput = inputs.begin();
		curBlock =
			curInput->blocks.begin();
		continue;
	}

#line 290 "write.md"

	if (cmd == "M" || cmd == "Make") {
		write_input();
		system("make");
		continue;
	}

#line 318 "write.md"
 {
	static const std::string p { "M " };
	if (is_prefix(cmd, p)) {
		write_input();
		system(("make " +
			cmd.substr(p.size())).c_str()
		);
		continue;
	}
} 
#line 333 "write.md"
 {
	static const std::string p {
		"Make "
	};
	if (is_prefix(cmd, p)) {
		write_input();
		system(("make " + 
			cmd.substr(p.size())
		).c_str());
		continue;
	}
} 
#line 350 "write.md"

	if (cmd == "G" || cmd == "Git") {
		write_input();
		system("git status");
		continue;
	}

#line 362 "write.md"
 {
	static const std::string p { "G " };
	if (is_prefix(cmd, p)) {
		write_input();
		system(("git " +
			cmd.substr(p.size())
		).c_str());
		continue;
	}
} 
#line 377 "write.md"
 {
	static const std::string p { "Git " };
	if (is_prefix(cmd, p)) {
		write_input();
		system(("git " +
			cmd.substr(p.size())
		).c_str());
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

#line 101 "add.md"

	if (cmd == "d" || cmd == "dup") {
		if (curInput != inputs.end()) {
			if (curBlock !=
				curInput->blocks.end()
			) {
				
#line 121 "add.md"

	int i = curBlock -
		curInput->blocks.begin();
	curInput->blocks.insert(
		curBlock, *curBlock
	);
	curBlock = curInput->blocks.begin() +
		i + 1;

#line 107 "add.md"
;
			}
		} else {
			std::cerr << "! no file\n";
		}
		continue;
	}

#line 87 "view.md"
;
	}

#line 60 "view.md"
;
	}

#line 88 "ncurses.md"

	#if HAVE_CURSES
		if (with_ncurses) {
			
#line 152 "ncurses.md"

	Ncurses_Handler handler;
	curInput = inputs.begin();
	curBlock = curInput->blocks.begin();
	draw_page();

#line 163 "ncurses.md"

	int ch;
	try {
		for (;;) {
			switch (ch = getch()) {
				
#line 178 "ncurses.md"

	case 'q': throw End_Of_Curses {};

#line 344 "ncurses.md"

	case 'n': {
		
#line 355 "ncurses.md"

	int next = (curBlock -
		curInput->blocks.begin()) + 1;
	while (next >= static_cast<int>(
		curInput->blocks.size()
	)) {
		--next;
	}
	
#line 78 "range.md"

	if (range) {
		next = range.last()((curBlock -
			curInput->blocks.begin()) + 1,
			curInput->blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 363 "ncurses.md"
;
	curBlock = curInput->blocks.begin() +
		next;

#line 346 "ncurses.md"
;
		draw_page();
		break;
	}

#line 371 "ncurses.md"

	case 'p' : {
		
#line 382 "ncurses.md"

	int next = curBlock -
		curInput->blocks.begin();
	if (next > 0) {
		--next;
	}
	
#line 78 "range.md"

	if (range) {
		next = range.last()((curBlock -
			curInput->blocks.begin()) + 1,
			curInput->blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 388 "ncurses.md"
;
	curBlock =
		curInput->blocks.begin() + next;

#line 373 "ncurses.md"
;
		draw_page();
		break;
	}

#line 396 "ncurses.md"

	case 'f': {
		
#line 407 "ncurses.md"

	int next =
		(curInput - inputs.begin()) + 1;
	while (next >= static_cast<int>(
		inputs.size()
	)) {
		--next;
	}
	
#line 91 "range.md"

	if (range) {
		next = range.last()((curInput -
				inputs.begin()) + 1,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 415 "ncurses.md"
;
	curInput = inputs.begin() + next;
	curBlock = curInput->blocks.begin();

#line 398 "ncurses.md"
;
		draw_page();
		continue;
	}

#line 423 "ncurses.md"

	case 'b': {
		
#line 434 "ncurses.md"

	int next = curInput - inputs.begin();
	if (next) {
		--next;
	}
	
#line 91 "range.md"

	if (range) {
		next = range.last()((curInput -
				inputs.begin()) + 1,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 439 "ncurses.md"
;
	curInput = inputs.begin() + next;
	curBlock = curInput->blocks.begin();

#line 425 "ncurses.md"
;
		draw_page();
		continue;
	}

#line 168 "ncurses.md"

			}
			draw_page();
		}
	} catch (const End_Of_Curses &) {}

#line 91 "ncurses.md"
;
		}
	#endif

#line 44 "index.md"

	}
