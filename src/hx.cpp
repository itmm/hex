
#line 38 "index.md"

	
#line 169 "index.md"

	
#line 36 "read.md"

	#include <string>

#line 177 "read.md"

	#include <fstream>

#line 98 "blocks.md"

	#include <vector>

#line 9 "log.md"

	#include <iostream>
	#include <exception>

#line 261 "frag.md"

	#include <vector>

#line 783 "frag.md"

	#include <sstream>

#line 5 "input.md"

	#include <iostream>
	#include <vector>
	#include <filesystem>

#line 535 "index.md"

	#include <algorithm>

#line 992 "index.md"

	#include <functional>
	#include <sstream>

#line 84 "html.md"

	#include <string>

#line 739 "html.md"

	#include <cctype>

#line 785 "html.md"

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
	class Frag_Ref;

	Frag *find_frag(const std::string &path, const std::string &key, bool local, std::string *got_path = nullptr);
	Frag *find_frag(const Frag_Ref &ref, std::string *got_path = nullptr);

	Frag &get_frag(const std::string &path, const std::string &key, bool local);
	Frag &get_frag(const Frag_Ref &ref);

	#include <map>
	using Frag_Map = std::map<std::string, Frag>;

	Frag_Map &frag_map(const std::string &in);
	Frag_Map &frag_map();

	void split_frag(const std::string &name, Frag *meta, std::map<std::string, std::string> &&values);
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

#line 158 "blocks.md"
,
	code,
	after_code

#line 213 "blocks.md"
,
	notes

#line 281 "blocks.md"
,
	para

#line 345 "blocks.md"
,
	img

#line 17 "blocks.md"

	};

#line 26 "blocks.md"

	using RS = Read_State;

#line 105 "blocks.md"

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

		Write_State(const std::string &f);
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
	
	class Frag_Ref {
	public:
		const std::string path;
		const std::string name;
		const bool local;
		Frag_Ref(
			const std::string &p,
			const std::string &n,
			bool l
		):
			path { p },
			name { n },
			local { l }
		{ }
	};
	class Frag_Entry {
		std::string str_;
		std::string file_;
		int first_line_ { -1 };
		int last_line_;
		Frag_Ref sub_ = { std::string {}, std::string {}, true };
	public:
		
#line 73 "frag.md"

	Frag_Entry() { }
	Frag_Entry(Frag_Ref sub):
		sub_ { sub }
	{ }

#line 85 "frag.md"

	void update_state(
		Write_State &state
	) const {
		
#line 98 "frag.md"

	auto c { str_.end() };
	auto b { str_.begin() };
	bool some_nl { false };
	while (b != c) {
		--c;
		
#line 116 "frag.md"

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

#line 104 "frag.md"
;
		break;
	}
	if (b != c && *c > ' ') {
		state.in_macro = false;
	}

#line 89 "frag.md"
;
	}

#line 135 "frag.md"

	std::string str(
		Write_State &state
	) const {
		
#line 156 "frag.md"

	bool old { state.in_macro };
	update_state(state);
	if (old) { return str_; }
	if (! state.c_style) { return str_; }
	if (first_line_ < 1) { return str_; }
	if (str_.empty()) { return str_; };

#line 139 "frag.md"
;
		std::ostringstream oss;
		oss << "\n#line " <<
			first_line_ << " \"" <<
			file_ << "\"\n" << str_;
		return oss.str();
	}

#line 172 "frag.md"

	void add(
		char ch, const std::string &file,
		int line
	) {
		
#line 186 "frag.md"

	if (
		file_.empty() || first_line_ <= 0
	) {
		file_ = file;
		first_line_ = line;
	}
	last_line_ = line;

#line 177 "frag.md"
;
		str_ += ch;
	}

#line 200 "frag.md"

	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		
#line 186 "frag.md"

	if (
		file_.empty() || first_line_ <= 0
	) {
		file_ = file;
		first_line_ = line;
	}
	last_line_ = line;

#line 206 "frag.md"
;
		str_ += value;
	}

#line 214 "frag.md"

	bool canAdd(
		const std::string &file,
		int line
	) {
		
#line 228 "frag.md"

	if (
		! file_.empty() && file != file_
	) {
		return false;
	}

#line 240 "frag.md"

	if (
		last_line_ > 0 &&
		last_line_ != line &&
		last_line_ + 1 != line
	) {
		return false;
	}

#line 254 "frag.md"

	return true;

#line 219 "frag.md"
;
		return false;
	}

#line 56 "frag.md"
;
		const Frag_Ref &sub() const {
			return sub_;
		}
	};

#line 268 "frag.md"

	class Frag {
		std::vector<Frag_Entry> entries_;
		int expands_;
		int multiples_;
		Frag *prefix_;
		const bool is_meta_;
	public:
		const std::string name;
		
#line 288 "frag.md"

	static bool isFile(const std::string &name) {
		static const std::string prefix {
			"file: "
		};
		std::string p {
			name.substr(0, prefix.size())
		};
		return p == prefix;
	}

#line 303 "frag.md"

	static std::string cmd(const std::string &name) {
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

#line 320 "frag.md"

	Frag(
		const std::string &name,
		Frag *prefix
	):
		entries_ {},
		expands_ { 0 },
		multiples_ { 0 },
		prefix_ { prefix },
		is_meta_ { name.find('@') != std::string::npos },
		name { name }
	{
		if (isFile(name)) { ++expands_; }
		if (cmd(name).size()) { ++expands_; }
	}

#line 341 "frag.md"

	const Frag *prefix() const {
		return prefix_;
	}
	Frag *prefix() {
		return prefix_;
	}

#line 353 "frag.md"

	bool is_meta() {
		return is_meta_;
	}

#line 361 "frag.md"

	void clear() {
		if (prefix_) {
			prefix_->clear();
		}
		entries_.clear();
	}

#line 374 "frag.md"

	bool empty() const {
		if (
			prefix_ && ! prefix_->empty()
		) {
			return false;
		}
		return entries_.empty();
	}

#line 466 "frag.md"

	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		if (value.empty()) { return; }
		
#line 486 "frag.md"

	if (entries_.empty()) {
		entries_.emplace_back();
	} else if (
		! entries_.back().canAdd(
			file, line
		)
	) {
		entries_.emplace_back();
	}

#line 473 "frag.md"
;
		entries_.back().add(
			value, file, line
		);
	}

#line 502 "frag.md"

	void add(
		char ch,
		const std::string &file,
		int line
	) {
		
#line 486 "frag.md"

	if (entries_.empty()) {
		entries_.emplace_back();
	} else if (
		! entries_.back().canAdd(
			file, line
		)
	) {
		entries_.emplace_back();
	}

#line 508 "frag.md"
;
		entries_.back().add(
			ch, file, line
		);
	}

#line 518 "frag.md"

	Frag &add(const Frag_Ref &sub);

#line 547 "frag.md"

	auto begin() const {
		return entries_.cbegin();
	}

#line 556 "frag.md"

	auto end() const {
		return entries_.cend();
	}

#line 565 "frag.md"

	int expands() const {
		return expands_ + (prefix_ ? prefix_->expands() : 0);
	}

#line 574 "frag.md"

	void addExpand() {
		++expands_;
	}

#line 583 "frag.md"

	int multiples() const {
		return multiples_ + (prefix_ ? prefix_->multiples() : 0);
	}

#line 592 "frag.md"

	void addMultiple() {
		++multiples_;
	}

#line 601 "frag.md"

	static bool is_c_style(const std::string &name) {
		
#line 611 "frag.md"

	static const std::string exts[] = {
		".c", ".h", ".cpp"
	};
	const std::string *end =
		exts + sizeof(exts)/sizeof(*exts);

#line 622 "frag.md"

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

#line 603 "frag.md"
;
		return false;
	}

#line 277 "frag.md"
;
	};

#line 388 "frag.md"

	Write_State::Write_State(
		const std::string &f
	):
		c_style { Frag::is_c_style(f) }
	{ }

#line 406 "frag.md"

	void test_frag_name(
		const std::string &name
	) {
		Frag f(name, nullptr);
		ASSERT(f.name == name);
	}

#line 435 "frag.md"

	bool isPopulatedFrag(
		const Frag *f
	) {
		return f && ! f->empty();
	}

#line 526 "frag.md"

	
#line 856 "frag.md"

	bool isFragInFrag(
		const std::string &path,
		const Frag *needle,
		const Frag *haystack
	) {
		ASSERT(needle);
		ASSERT(haystack);
		
#line 884 "frag.md"

	if (needle == haystack) {
		return true;
	}

#line 864 "frag.md"
;
		
#line 893 "frag.md"

	if (haystack->prefix() && isFragInFrag(path, needle, haystack->prefix())) {
		return true;
	}
	for (const auto &i : *haystack)  {
		if (i.sub().name.empty()) { continue; }
		std::string new_path { path };
		Frag *f { find_frag(i.sub(), &new_path) };
		if (! f) { continue; }
		if (isFragInFrag(
			new_path, needle, f
		)) {
			return true;
		}
	}

#line 865 "frag.md"
;
		return false;
	}

#line 527 "frag.md"

	Frag &Frag::add(const Frag_Ref &sub) {
		
#line 874 "frag.md"

	Frag &f { get_frag(sub) };
	ASSERT(! isFragInFrag(
		sub.path, this, &f
	));

#line 529 "frag.md"
;
		
#line 538 "frag.md"

	entries_.push_back(
		Frag_Entry { sub }
	);

#line 530 "frag.md"
;
		return *this;
	}

#line 642 "frag.md"

	void serializeFrag(
		const Frag &frag,
		std::ostream &out,
		Write_State &state,
		const std::string &path
	) {
		
#line 673 "frag.md"

	if (frag.prefix()) {
		serializeFrag(
			*frag.prefix(), out, state, path
		);
	}
	for (const auto &entry : frag) {
		if (! entry.sub().name.empty()) {
			std::string new_path = path;
			const Frag *f { find_frag(entry.sub(), &new_path) };
			if (f) {
				serializeFrag(
					*f, out, state, new_path
				);
			} else {
				std::cerr << "no frag [" << entry.sub().name << "], " << (entry.sub().local ? "local" : "global" ) << ", [" << path << "]\n";
			}
		}
		out << entry.str(state);
	}

#line 649 "frag.md"
;
	}

#line 656 "frag.md"

	void serializeFrag(
		const std::string &name,
		const Frag &f,
		std::ostream &out,
		const std::string &path
	) {
		Write_State state { name };
		return serializeFrag(
			f, out, state, path
		);
	}

#line 699 "frag.md"

	bool check_frag(
		const Frag &f,
		std::istream &in,
		Write_State &state,
		const std::string &path
	) {
		
#line 731 "frag.md"

	if (f.prefix()) {
		if (! check_frag(*f.prefix(), in, state, path)) {
			return false;
		}
	}
	for (const auto &entry : f) {
		if (! entry.sub().name.empty()) {
			std::string new_path = path;
			const Frag *f { find_frag(entry.sub(), &new_path) };
			if (f) {
				if (! check_frag(
					*f, in, state, new_path
				)) {
					return false;
				}
			}
		}
		
#line 757 "frag.md"

	for (
		const auto &i : entry.str(state)
	) {
		if (in.get() != i) {
			return false;
		}
	}

#line 749 "frag.md"
;
	}

#line 706 "frag.md"
;
		return true;
	}

#line 714 "frag.md"

	bool check_frag(
		const std::string &name,
		const Frag &f,
		std::istream &in,
		const std::string &path
	) {
		Write_State state { name };
		return check_frag(
			f, in, state, path
		);
	}

#line 770 "frag.md"

	void testFrag(
		const std::string &name,
		const Frag &frag,
		const std::string &expected
	) {
		
#line 790 "frag.md"

	std::ostringstream buffer;
	serializeFrag(name, frag, buffer, "");
	ASSERT(buffer.str() == expected);

#line 776 "frag.md"
;
	}

#line 800 "frag.md"

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
			
#line 237 "read.md"

	Input(const std::string &prev = {}):
		prev { prev }
	{}

#line 247 "read.md"

	Input(const Input &) = delete;
	Input(Input &&) = default;

#line 256 "read.md"

	Input &operator=(
		const Input &
	) = delete;
	Input &operator=(
		Input &&
	) = default;

#line 117 "blocks.md"

	std::vector<Block> blocks;

#line 166 "read.md"
;
			const std::string prev;
	};

#line 149 "read.md"
;
	class Open_Input {
		public:
			
#line 193 "read.md"

	Open_Input(const std::string &path):
		path_ { path },
		file_ { path.c_str() }
	{}

#line 203 "read.md"

	const std::string &path() const {
		return path_; 
	}

#line 211 "read.md"

	Open_Input(
		const Open_Input &
	) = delete;
	Open_Input(
		Open_Input &&
	) = default;

#line 224 "read.md"

	Open_Input &operator=(
		const Open_Input &
	) = delete;
	Open_Input &operator=(
		Open_Input &&
	) = default;

#line 269 "read.md"

	void read_line(std::string &line) {
		if (file_.is_open()) {
			
#line 282 "read.md"

	if (std::getline(file_, line)) {
		
#line 139 "input.md"

	++line_;

#line 284 "read.md"
;
		return;
	}
	file_.close();

#line 272 "read.md"
;
		}
		throw No_More_Lines {};
	}

#line 33 "blocks.md"

	Read_State state = RS::new_element;

#line 130 "input.md"

	int line() const {
		return line_;
	}

#line 152 "read.md"
;
		private:
			
#line 184 "read.md"

	std::string path_;
	std::ifstream file_;

#line 123 "input.md"

	int line_ = 0;

#line 154 "read.md"
;
	};

#line 334 "index.md"

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
		
#line 146 "input.md"

	used_.clear();
	open_.clear();
	if (roots_.empty()) {
		
#line 159 "input.md"

	if (std::filesystem::exists(
		"index.md"
	)) {
		roots_.push_back("index.md");
	} else if (std::filesystem::exists(
		"index.x"
	)) {
		roots_.push_back("index.x");
	} else {
		std::cerr << "no input paths\n";
		roots_.push_back("index.md");
	}

#line 150 "input.md"
;
	}
	current_path_ = roots_.begin();

#line 111 "read.md"
;
	}

#line 14 "input.md"

	auto &cur() {
		ASSERT (! open_.empty());
		return open_.back();
	}
	auto &cur_input() {
		return used_.find(cur().path())->second;
	}

#line 27 "input.md"

	auto begin() {
		return used_.begin();
	}

#line 36 "input.md"

	auto end() {
		return used_.end();
	}

#line 45 "input.md"

	auto size() const {
		return used_.size();
	}

#line 54 "input.md"

	void push(const std::string &path) {
		std::string prev;
		if (open_.size()) {
			auto got { used_.find(open_.back().path()) };
			if (got != used_.end()) {
				prev = open_.back().path();
			}
		}
		used_.insert(std::move(std::map<std::string, Input>::value_type(path, Input(prev))));
		open_.emplace_back(path);
	}
	const std::string open_head() const {
		ASSERT(! open_.empty());
		return open_.back().path();
	}
	Input &operator[](
		const std::string &name
	) {
		return used_[name];
	}

#line 81 "input.md"

	void add(const std::string &path) {
		roots_.push_back(path);
		push(path);
	}

#line 96 "input.md"

	bool has(
		const std::string &name
	) const {
		
#line 108 "input.md"

	if (used_.find(name) != used_.end()) {
		return true;
	}

#line 100 "input.md"
;
		return false;
	}

#line 24 "read.md"
;
		private:
			
#line 295 "read.md"

	std::vector<std::string> roots_;
	std::vector<std::string>::
		const_iterator current_path_;

#line 305 "read.md"

	std::vector<Open_Input> open_;
	std::map<std::string, Input> used_;

#line 26 "read.md"
;
	};

#line 51 "read.md"

	void Inputs::read_line(
		std::string &line
	) {
		
#line 317 "read.md"

	for (;;) {
		
#line 336 "read.md"

	if (open_.empty()) {
		if (
			current_path_ != roots_.end()
		) {
			push(*current_path_++);
		} else {
			break;
		}
	}

#line 319 "read.md"
;
		try {
			open_.back().read_line(line);
			return;
		}
		catch (const No_More_Lines &) {}
		
#line 351 "read.md"

	auto &f { used_.find(open_.back().path())->second };
	if (f.blocks.empty()) {
		f.blocks.push_back({
			RS::header,
			{ "EMPTY FILE" }, {}
		});
	}

#line 325 "read.md"
;
		open_.pop_back();
	}
	throw No_More_Lines {};

#line 55 "read.md"
;
	}

#line 80 "read.md"

	Inputs inputs;

#line 91 "read.md"

	
#line 295 "index.md"

	int blockLimit = -1;

#line 343 "index.md"

	void process_char(
		Frag *frag, char ch, const std::string &cur_path, int cur_line
	) {
		
#line 385 "index.md"

	if (frag) {
		frag->add(ch, cur_path, cur_line);
	}

#line 347 "index.md"
;
	}

#line 501 "index.md"

	inline void expand_cmd_arg(
		Frag *f, const std::string &arg,
		const std::string cur_path, int cur_line
	) {
		auto b = arg.begin();
		auto e = arg.end();
		
#line 542 "index.md"

	while (b != e) {
		auto x = std::find(b, e, '@');
		
#line 560 "index.md"

	f->add(
		std::string { b, x },
		cur_path, cur_line
	);

#line 545 "index.md"
;
		if (x != e) {
			b = x + 1;
			
#line 570 "index.md"

	if (b != e) {
		f->add(*b, cur_path, cur_line);
		++b;
	}

#line 548 "index.md"
;
		} else {
			b = e;
		}
	}

#line 508 "index.md"
;
	}

#line 580 "index.md"

	#define ASSERT_NOT_FRAG() \
		ASSERT_MSG(! frag, '@' << \
			name << "(" << arg << \
			") in frag [" << \
			frag->name << ']' \
		)

#line 592 "index.md"

	#define CHECK_NOT_DEFINED() \
		if (isPopulatedFrag(frag)) { \
			WARN_MSG("frag [" << arg << \
				"] already defined" \
			); \
		}

#line 617 "index.md"

	#define ASSERT_FRAG() \
		ASSERT_MSG(frag, '@' << \
			name << "(" << arg << \
			") in frag [" << \
			frag->name << ']' \
		)

#line 678 "index.md"

	#define CHECK_DEFINED() \
		if (! isPopulatedFrag(frag)) { \
			WARN_MSG("frag [" << arg << \
				"] not defined" \
			); \
		}

#line 713 "index.md"

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
						std::string value { arg.substr(j + 1, k - j - 2) };
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
	
#line 375 "index.md"

	Frag *frag { nullptr };

#line 122 "read.md"
;
	std::string line;
	try { for (;;) {
		inputs.read_line(line);
		
#line 41 "blocks.md"

	do {
		auto &state = inputs.cur().state;
		
#line 124 "blocks.md"

	auto &blocks =
		inputs.cur_input().blocks;

#line 44 "blocks.md"
;
		
#line 166 "blocks.md"

	if (
		line == "```" &&
		state == RS::new_element
	) {
		state = RS::code;
		
#line 195 "blocks.md"

	blocks.push_back({
		RS::code, {}, {}, 0
	});

#line 172 "blocks.md"
;
		break;
	}

#line 180 "blocks.md"

	if (state == RS::code) {
		if (line == "```") {
			state = RS::after_code;
		} else {
			
#line 204 "blocks.md"

	blocks.back().value.push_back(
		line
	);

#line 185 "blocks.md"
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
		(state == RS::new_element ||
			state == RS::header)
	) {
		bool was_new { state == RS::new_element };
		state = RS::header;
		
#line 132 "blocks.md"

	auto b { line.begin() };
	auto e { line.end() };
	int l { 0 };
	for (
		; b != e && *b == '#'; ++b, ++l
	) {}
	for (; b != e && *b == ' '; ++b) {}
	if (was_new || blocks.empty() ||
		blocks.back().state != RS::header ||
		blocks.back().notes.size()
	) {
		blocks.push_back({
			RS::header, {}, {}, l
		});
	}
	blocks.back().value.emplace_back(
		b, e
	);

#line 82 "blocks.md"
;
		break;
	}

#line 220 "blocks.md"

	if (line[0] == '*') {
		if (
			state == RS::header ||
			state == RS::after_code ||
			state == RS::notes
			
#line 352 "blocks.md"
 ||
	state == RS::img

#line 226 "blocks.md"

		) {
			state = RS::notes;
			
#line 254 "blocks.md"

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

#line 229 "blocks.md"
;
			break;
		}
	}

#line 240 "blocks.md"

	if (
		line[0] == ' ' &&
		state == RS::notes
	) {
		
#line 272 "blocks.md"

	blocks.back().notes.back() +=
		line;

#line 245 "blocks.md"
;
		break;
	}

#line 289 "blocks.md"

	
#line 358 "blocks.md"

	if (line[0] == '!') {
		if (
			state == RS::new_element ||
			state == RS::img
		) {
			
#line 374 "blocks.md"

	if (state == RS::new_element) {
		
#line 382 "blocks.md"

	if (blocks.empty() ||
		blocks.back().state != RS::img
	) {
		blocks.push_back({
			RS::img, {}, {}, 0
		});
	}

#line 376 "blocks.md"
;
	}

#line 364 "blocks.md"
;
			
#line 394 "blocks.md"

	if (line.size() < 3 || line[1] != '(' || line[line.size() - 1] != ')') {
		std::cerr << "wrong line " << line << "\n";
	}
	blocks.back().value.push_back(line.substr(2, line.size() - 3));

#line 365 "blocks.md"
;
			state = RS::img;
			break;
		}
	}

#line 290 "blocks.md"
;
	if (line[0] != ' ') {
		if (
			state == RS::new_element ||
			state == RS::para
		) {
			
#line 307 "blocks.md"

	if (state == RS::new_element) {
		
#line 320 "blocks.md"

	if (blocks.empty() ||
		blocks.back().state != RS::para
	) {
		blocks.push_back({
			RS::para, {}, {}, 0
		});
	}

#line 309 "blocks.md"
;
		blocks.back().value.push_back(
			line
		);
	}

#line 296 "blocks.md"
;
			
#line 333 "blocks.md"

	if (state == RS::para) {
		blocks.back().value.back() +=
			" " + line;
	}


#line 297 "blocks.md"
;
			state = RS::para;
			break;
		}
	}

#line 47 "blocks.md"
;
		
#line 91 "blocks.md"

	std::cerr << "!! " << line << '\n';

#line 48 "blocks.md"
;
	} while (false);

#line 354 "index.md"

	auto end = line.cend();
	std::string cur_path = inputs.cur().path();
	int cur_line = inputs.cur().line();
	std::map<std::string, std::string> cmd_values;
	for (
		auto i = line.cbegin();
		i != end; ++i
	) {
		
#line 397 "index.md"

	if (*i == '@') {
		auto nb = i + 1;
		auto ne = nb;
		
#line 415 "index.md"

	while (ne != end && *ne != '(') {
		if (! isalpha(*ne)) {
			ne = end;
			break;
		}
		++ne;
	}

#line 401 "index.md"
;
		if (ne != end && ne != nb) {
			std::string name { nb, ne };
			
#line 429 "index.md"

	auto ab = ne + 1; auto ae = ab;
	while (ae != end && *ae != ')') {
		if (*ae == '@') {
			
#line 449 "index.md"

	if (++ae == end) { break; }
	if (isalpha(*ae)) {
		
#line 458 "index.md"

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

#line 452 "index.md"
;
	}

#line 433 "index.md"
;
		}
		++ae;
	}
	if (ae != end) {
		std::string arg {ab, ae};
		
#line 478 "index.md"

	i = ae;
	bool outside = ! frag;
	do {
		if (outside && ! blockLimit) {
			break;
		}
		
#line 604 "index.md"

	if (name == "def") {
		ASSERT_NOT_FRAG();
		frag = &get_frag(cur_path, arg, true);
		CHECK_NOT_DEFINED();
		break;
	}

#line 629 "index.md"
 {
	auto i { cmd_values.find(name) };
	if (i != cmd_values.end()) {
		frag->add(i->second, cur_path, cur_line);
		break;
	}
} 
#line 639 "index.md"

	if (name == "end" || name == "End") {
		ASSERT_FRAG();
		if (frag->is_meta()) {
			std::string pattern;
			std::map<std::string, std::string> values;
			parse_args(arg, pattern, values);
			if (frag->name == pattern) {
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
			
#line 668 "index.md"

	ASSERT_MSG(frag->name == arg,
		"closing [" << arg <<
		"] != [" << frag->name << ']'
	);

#line 658 "index.md"
;
			frag = nullptr;
		}
		break;
	}

#line 690 "index.md"

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
			frag = &get_frag(cur_path, arg, true);
			CHECK_DEFINED();
		}
		break;
	}

#line 744 "index.md"

	if (name == "put") {
		if (! frag && arg.find('@') != std::string::npos) {
			std::string pattern;
			std::map<std::string, std::string> values;
			parse_args(arg, pattern, values);
			Frag *sub = &get_frag(cur_path, pattern, true);
			sub->addMultiple();
			split_frag(pattern, sub, std::move(values));
		} else {
			ASSERT_MSG(frag, "@put" << "(" <<
				arg << ") not in frag"
			);
			Frag *sub = &get_frag(cur_path, arg, true);
			ASSERT(sub);
			
#line 772 "index.md"

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

#line 759 "index.md"
;
			sub->addExpand();
			frag->add(Frag_Ref { cur_path, arg, true });
		}
		break;
	}

#line 789 "index.md"

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

#line 807 "index.md"

	if (name == "mul") {
		ASSERT_MSG(frag,
			"@mul not in frag"
		);
		Frag *sub = &get_frag(cur_path, arg, true);
		if (sub) {
			
#line 825 "index.md"

	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}

#line 814 "index.md"
;
			sub->addMultiple();
			frag->add(Frag_Ref { cur_path, arg, true });
		}
		break;
	}

#line 838 "index.md"

	if (name == "Def") {
		
#line 848 "index.md"

	ASSERT_MSG(! frag,
		"@Def in frag [" <<
		frag->name << ']'
	);
	frag = &get_frag(cur_path, arg, false);
	if (isPopulatedFrag(frag)) {
		std::cerr << "Frag [" <<
			arg << "] already defined\n";
	}

#line 840 "index.md"
;
		break;
	}

#line 863 "index.md"

	if (name == "Add") {
		
#line 873 "index.md"

	ASSERT_MSG(! frag, "@Add in frag [" <<
		frag->name << ']'
	);
	frag = &get_frag(cur_path, arg, false);
	if (! isPopulatedFrag(frag)) {
		std::cerr << "Frag [" << arg <<
			"] not defined\n";
	}

#line 865 "index.md"
;
		break;
	}

#line 889 "index.md"

	if (name == "rep") {
		ASSERT_MSG(! frag,
			"@rep in frag [" <<
				frag->name << ']'
		);
		frag = &get_frag(cur_path, arg, true);
		
#line 919 "index.md"

	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();

#line 896 "index.md"
;
		break;
	}

#line 904 "index.md"

	if (name == "Rep") {
		ASSERT_MSG(! frag,
			"@Rep in frag [" <<
				frag->name << ']'
		);
		frag = &get_frag(cur_path, arg, false);
		
#line 919 "index.md"

	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();

#line 911 "index.md"
;
		break;
	}

#line 931 "index.md"

	if (name == "Put") {
		
#line 941 "index.md"

	ASSERT_MSG(frag, "@Put not in frag");
	Frag *sub = &get_frag(cur_path, arg, false);
	if (sub) {
		
#line 772 "index.md"

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

#line 945 "index.md"
;
		sub->addExpand();
		frag->add(Frag_Ref { cur_path, arg, false });
	}

#line 933 "index.md"
;
		break;
	}

#line 954 "index.md"

	if (name == "Mul") {
		
#line 964 "index.md"

	ASSERT_MSG(frag, "@Mul not in frag");
	Frag *sub = &get_frag(cur_path, arg, false);
	if (sub) {
		
#line 825 "index.md"

	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}

#line 968 "index.md"
;
		sub->addMultiple();
		frag->add(Frag_Ref { cur_path, arg, false });
	}

#line 956 "index.md"
;
		break;
	}

#line 977 "index.md"

	if (name == "priv") {
		ASSERT_MSG(frag,
			"@priv not in frag"
		);
		
#line 1000 "index.md"

	std::hash<std::string> h;
	auto cur {
		h(cur_path + ':' + arg) &
			0x7fffffff
	};

#line 1012 "index.md"

	std::ostringstream hashed;
	hashed << "_private_" <<
		cur << '_' <<
		arg;
	frag->add(
		hashed.str(), cur_path, cur_line
	);

#line 982 "index.md"
;
		break;
	}

#line 1027 "index.md"

	if (name == "magic") {
		ASSERT_MSG(frag,
			"@magic not in frag"
		);
		
#line 1040 "index.md"

	std::hash<std::string> h;
	auto cur {
		h(cur_path + ':' + arg) &
			0x7fffffff
	};

#line 1051 "index.md"

	std::ostringstream value;
	value << cur;
	frag->add(
		value.str(), cur_path, cur_line
	);

#line 1032 "index.md"
;
		break;
	}

#line 485 "index.md"
;
		
#line 515 "index.md"

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

#line 486 "index.md"
;
	} while (false);
	if (blockLimit && outside && frag) {
		--blockLimit;
	}

#line 439 "index.md"
;
		continue;
	}

#line 404 "index.md"
;
		}
	}

#line 363 "index.md"
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

#line 268 "index.md"

	std::string stylesheet {
		"slides/slides.css"
	};

#line 1067 "index.md"

	
#line 1156 "index.md"

	std::string file_name(const std::string &name) {
		return name.substr(6);
	}

#line 1165 "index.md"

	bool file_changed(const std::string &name, const Frag &f, std::string cur_path) {
		std::ifstream in(
			file_name(name).c_str()
		);
		if (! check_frag(name, f, in, cur_path)) {
			return true;
		}
		if (in.get() != EOF) {
			return true;
		}
		return false;
	}

#line 1068 "index.md"

	void files_write() {
		
#line 1086 "index.md"

	for (auto &i : frag_map()) {
		const Frag *frag {
			&i.second
		};
		std::string cur_path { };
		std::string cur_name { i.first };
		
#line 1119 "index.md"
 {
	if (frag->isFile(cur_name)) {
		
#line 1186 "index.md"

	if (file_changed(cur_name, *frag, cur_path)) {
		std::ofstream out(
			file_name(cur_name).c_str()
		);
		serializeFrag(cur_name, *frag, out, cur_path);
	}

#line 1121 "index.md"
;
	}
} 
#line 1129 "index.md"
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
#line 1144 "index.md"

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
		std::string cur_path { j.first };
		for (auto &i : frag_map(cur_path)) {
			const std::string cur_name { i.first };
			const Frag *frag {
				&i.second
			};
			
#line 1119 "index.md"
 {
	if (frag->isFile(cur_name)) {
		
#line 1186 "index.md"

	if (file_changed(cur_name, *frag, cur_path)) {
		std::ofstream out(
			file_name(cur_name).c_str()
		);
		serializeFrag(cur_name, *frag, out, cur_path);
	}

#line 1121 "index.md"
;
	}
} 
#line 1129 "index.md"
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
#line 1144 "index.md"

	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not populated\n";
	}

#line 1109 "index.md"
;
		}
	}

#line 1070 "index.md"
;
	}

#line 1198 "index.md"

	
#line 1249 "index.md"

	bool no_cmds = false;

#line 1199 "index.md"
;
	void files_process() {
		
#line 1218 "index.md"

	for (auto &i : frag_map()) {
		const Frag *frag {
			&i.second
		};
		const std::string cur_path;
		const std::string cur_name = i.first;
		
#line 1256 "index.md"
 {
	const std::string cmd { Frag::cmd(cur_name) };
	if (cmd.size()) {
		
#line 1266 "index.md"

	std::ostringstream out {};
	serializeFrag(cur_name, *frag, out, cur_path);
	std::string o { out.str() };
	if (no_cmds) {
		std::cout << o;
	} else {
		
#line 1282 "index.md"

	std::FILE *f {
		popen(cmd.c_str(), "w")
	};
	if (f) {
		std::fwrite(
			o.c_str(), o.size(), 1, f
		);
		pclose(f);
	}

#line 1273 "index.md"
;
	}

#line 1259 "index.md"
;
	}
} 
#line 1225 "index.md"
;
	}

#line 1232 "index.md"

	for (auto &j : inputs) {
		for (auto &i : frag_map(j.first)) {
			const Frag *frag {
				&i.second
			};
			const std::string cur_path = j.first;
			const std::string cur_name = i.first;
			
#line 1256 "index.md"
 {
	const std::string cmd { Frag::cmd(cur_name) };
	if (cmd.size()) {
		
#line 1266 "index.md"

	std::ostringstream out {};
	serializeFrag(cur_name, *frag, out, cur_path);
	std::string o { out.str() };
	if (no_cmds) {
		std::cout << o;
	} else {
		
#line 1282 "index.md"

	std::FILE *f {
		popen(cmd.c_str(), "w")
	};
	if (f) {
		std::fwrite(
			o.c_str(), o.size(), 1, f
		);
		pclose(f);
	}

#line 1273 "index.md"
;
	}

#line 1259 "index.md"
;
	}
} 
#line 1240 "index.md"
;
		}
	}

#line 1201 "index.md"
;
	}

#line 5 "html.md"

	
#line 52 "html.md"

	enum class HtmlState {
		nothing,
		inSlide,
		afterSlide,
		afterSlides
		
#line 326 "html.md"

	, inCode

#line 971 "html.md"

	, inNotes

#line 1120 "html.md"

	, inPara

#line 58 "html.md"

	};

#line 68 "html.md"

	struct HtmlStatus {
		
#line 77 "html.md"

	HtmlState state = HtmlState::nothing;

#line 70 "html.md"

	};

#line 190 "html.md"
 
	void writeOneEscaped(
		std::ostream &out, char ch
	) {
		switch (ch) {
			
#line 221 "html.md"

	case '<':
		out << "&lt;";
		break;
	case '>':
		out << "&gt;";
		break;
	case '&':
		out << "&amp;";
		break;

#line 195 "html.md"

			default:
				out << ch;
		}
	}

#line 206 "html.md"

	void writeEscaped(
		std::ostream &out,
		const std::string &str
	) {
		for (char ch : str) {
			writeOneEscaped(out, ch);
		}
	}

#line 236 "html.md"

	
#line 467 "html.md"

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

#line 792 "html.md"

	using Set = std::set<std::string>;

#line 799 "html.md"

	bool isKeyword(const std::string &s) {
		static Set reserved {
			
#line 816 "html.md"

	"break", "case", "catch", "continue",
	"default", "delete", "else", "for",
	"if", "in", "inline", "new", "return",
	"static", "switch", "try", "typeof",
	"while", "class", "public", "private",
	"template", "typename", "using",
	"function", "throw", "namespace",
	"once", "constexpr", "volatile",
	"override"

#line 802 "html.md"

		};
		return
			reserved.find(s) !=
				reserved.end() ||
					(s.size() &&
						s[0] == '#'
					);
	}

#line 831 "html.md"

	bool isType(const std::string &s) {
		
#line 841 "html.md"

	static Set reserved {
		
#line 855 "html.md"

	"FILE", "auto", "bool", "char",
	"const", "enum", "extern", "int",
	"let", "long", "signed", "struct",
	"union", "unsigned", "void", "double",
	"string", "std", "ifstream",
	"istream", "ofstream", "ostream",
	"vector", "map", "list", "float"

#line 843 "html.md"

	};
	if (reserved.find(s) !=
		reserved.end()
	) {
		return true;
	}

#line 868 "html.md"

	if (s.size() >= 2) {
		if (isupper(s[0]) &&
			islower(s[1])
		) {
			return true;
		}
	}

#line 833 "html.md"
;
		return false;
	}

#line 882 "html.md"

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

#line 901 "html.md"

	void process_ident(
		std::ostream &out,
		const std::string ident,
		char w
	) {
		
#line 914 "html.md"

	if (isKeyword(ident)) {
		span_str(out, "keyword", ident);
	} else if (w == '(') {
		span_str(out, "fn", ident);
	
#line 930 "html.md"

	} else if (isType(ident)) {
		span_str(out, "type", ident);
	} else if (isNum(ident)) {
		span_str(out, "num", ident);

#line 919 "html.md"

	} else {
		span_str(out, "var", ident);
	}

#line 907 "html.md"
;
	}

#line 941 "html.md"

	void writeMacroClass(
		std::ostream &out,
		const char *name
	) {
		out << "<span class=\"" <<
			name << "\">";
	}

#line 954 "html.md"

	void writeMacroHeader(
		std::ostream &out,
		const std::string &name
	) {
		writeMacroClass(out, "macro");
		out << '@' << name <<
			"(<span class=\"name\">";
	}

#line 237 "html.md"
;
	void process_code(
		std::ostream &out,
		SI begin, SI end
	) {
		
#line 403 "html.md"

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

#line 421 "html.md"

	for (; begin != end; ++begin) {
		
#line 432 "html.md"

	if (
		*begin == '`' ||
		*begin == '\'' ||
		*begin == '"'
	) {
		
#line 446 "html.md"

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

#line 483 "html.md"

	std::string name {begin, w + 1};
	span_str(out, "str", name);
	begin = w;

#line 438 "html.md"
;
		continue;
	}

#line 492 "html.md"

	if (*begin == '@') {
		auto nb = begin + 1;
		auto ne = nb;
		
#line 503 "html.md"

	while (ne != end && *ne != '(') {
		if (! isalpha(*ne)) {
			ne = end;
			break;
		}
		++ne;
	}

#line 517 "html.md"

	if (ne != end) {
		std::string name {nb, ne};
		auto ab = ne + 1;
		auto ae = ab;
		
#line 529 "html.md"

	while (ae != end && *ae != ')') {
		if (*ae == '@') {
			++ae;
			if (ae == end) { break; }
		}
		++ae;
	}
	if (ae != end) {
		std::string arg {ab, ae};
		
#line 547 "html.md"

	do {
		
#line 572 "html.md"

	static Set macros = {
		"def", "end", "add", "put", "mul",
		"Def", "Add", "Mul", "rep", "Rep",
		"Put", "End"
	};

#line 583 "html.md"

	if (
		macros.find(name) != macros.end()
	) {
		writeMacroHeader(out, name);
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}

#line 597 "html.md"

	if (name == "inc") {
		
#line 607 "html.md"

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

#line 599 "html.md"
;
		break;
	}

#line 623 "html.md"

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

#line 641 "html.md"

	if (name == "f" || name == "fn") {
		writeMacroClass(out, "fn");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 653 "html.md"

	if (name == "v" || name == "var") {
		writeMacroClass(out, "var");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 665 "html.md"

	if (name == "k" || name == "key") {
		writeMacroClass(out, "keyword");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 677 "html.md"

	if (name == "n" || name == "num") {
		writeMacroClass(out, "num");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 689 "html.md"

	if (name == "t" || name == "typ") {
		writeMacroClass(out, "type");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 701 "html.md"

	if (name == "b" || name == "br") {
		writeMacroClass(out, "virt");
		out << "</span><br/>";
		break;
	}

#line 713 "html.md"

	if (name == "priv") {
		writeMacroClass(out, "var");
		out << "@priv(<span>";
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}

#line 726 "html.md"

	if (name == "magic") {
		writeMacroClass(out, "num");
		out << "@magic(<span>";
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}

#line 549 "html.md"
;
		
#line 560 "html.md"

	writeOneEscaped(out, '@');
	writeEscaped(out, name);
	writeOneEscaped(out, '(');
	writeEscaped(out, arg);
	writeOneEscaped(out, ')');

#line 550 "html.md"
;
	} while (false);
	begin = ae;

#line 539 "html.md"
;
		continue;
	}

#line 522 "html.md"
;
	}

#line 496 "html.md"
;
	}

#line 746 "html.md"

	auto w = begin;
	
#line 758 "html.md"

	while (w != end && (
		std::isalnum(*w) ||
			*w == '_' || *w == '$' ||
			*w == '#'
	)) {
		++w;
	}

#line 748 "html.md"
;
	if (w != begin) {
		
#line 772 "html.md"

	std::string ident {begin, w};
	begin = w - 1;
	process_ident(
		out, ident,
		w != end ? *w : ' '
	);

#line 750 "html.md"
;
		continue;
	}

#line 423 "html.md"
;
		writeOneEscaped(out, *begin);
	}

#line 242 "html.md"
;
	}

#line 252 "html.md"

	void process_content(
		std::ostream &out,
		SI begin, SI end
	) {
		
#line 1028 "html.md"

	for(; begin != end; ++begin) {
		
#line 1040 "html.md"

	if (*begin == '`') {
		
#line 1049 "html.md"

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

#line 1042 "html.md"
;
	}

#line 1066 "html.md"

	if (
		*begin == '*' &&
		(begin + 1) != end &&
		*(begin + 1) == '*'
	) {
		
#line 1079 "html.md"

	auto w = begin + 2;
	while (
		w != end && (w + 1) != end &&
		(*w != '*' || *(w + 1) != '*')
	) {
		++w;
	}

#line 1092 "html.md"

	if (
		w != end && (w + 1 ) != end &&
		*w == '*' && *(w + 1) == '*'
	) {
		
#line 1105 "html.md"

	out << "<b>";
	writeEscaped(
		out, std::string {begin + 2, w}
	);
	out << "</b>";
	begin = w + 1;

#line 1097 "html.md"
;
		continue;
	}

#line 1072 "html.md"
;
	}

#line 1030 "html.md"
;
		writeOneEscaped(out, *begin);
	}

#line 257 "html.md"
;
	}

#line 6 "html.md"

	void write_html() {
		for (auto &cur : inputs) {
			
#line 26 "html.md"

	const std::string &name { cur.first };
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
	for (const auto &b : cur.second.blocks) {
		
#line 101 "html.md"

	if (b.state == RS::header) {
		
#line 153 "html.md"

	
#line 277 "html.md"

	switch (status.state) {
		case HtmlState::nothing:
			
#line 295 "html.md"

	out << "<!doctype html>\n";
	out << "<html lang=\"en\">\n";
	out << "<head>\n";
	
#line 307 "html.md"

	out << "<meta charset=\"utf-8\">\n";
	out << "<title>";
	writeEscaped(out, b.value[0]);
	out << "</title>\n";
	out << "<link rel=\"stylesheet\" "
		"type=\"text/css\" href=\""
		<< stylesheet << "\">";

#line 299 "html.md"
;
	out << "</head>\n";
	out << "<body>\n";

#line 280 "html.md"
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
	
#line 265 "html.md"
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
	out << "<div class=\"page\">\n";
	out << "<div class=\"slide\"><div class=\"headers\">\n";
	
#line 265 "html.md"
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
	for (unsigned i = 1; i < b.value.size(); ++i)
	{
		out << "<h" << (b.level + 1) << '>';
		const auto &n = b.value[i];
		process_content(
			out, n.begin(), n.end()
		);
		out << "</h" << (b.level + 1) << ">\n";
	}
	out << "</div></div>\n";
	status.state = HtmlState::inSlide;

#line 178 "html.md"

	for (const auto &note : b.notes) {
		
#line 989 "html.md"

	auto end = note.end();
	auto begin = note.begin();

#line 997 "html.md"

	if (
		status.state != HtmlState::inNotes
	) {
		
#line 1014 "html.md"

	if (
		status.state != HtmlState::inSlide
	) {
		out << "<div class=\"page\">\n";
	}
	status.state = HtmlState::inNotes;
	out << "<ul><li>\n";

#line 1001 "html.md"
;
	} else {
		out << "</li><li>\n";
	}
	process_content(out, begin, end);
	out << '\n';

#line 180 "html.md"
;
	}
	
#line 978 "html.md"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1127 "html.md"

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}

#line 182 "html.md"
;
	
#line 141 "html.md"

	out << "</div>\n";
	status.state = HtmlState::afterSlide;

#line 183 "html.md"
;

#line 103 "html.md"
;
	}

#line 110 "html.md"

	if (b.state == RS::code) {
		
#line 333 "html.md"

	if (
		status.state ==
			HtmlState::afterSlides
	) {
		out << "<div class=\"slides\">\n";
	}

#line 345 "html.md"

	if (
		status.state == HtmlState::inSlide
	) {
		out << "</div>\n";
	}
	out << "<div class=\"page\"><div class=\"slide\">\n";
	out << "<code>\n";
	status.state = HtmlState::inCode;

#line 112 "html.md"
;
		for (const auto &code : b.value) {
			
#line 392 "html.md"

	process_code(
		out, code.begin(), code.end()
	);
	out << "<br/>\n";

#line 114 "html.md"
;
		}
		
#line 360 "html.md"

	out << "</code></div>\n";
	status.state = HtmlState::inSlide;

#line 116 "html.md"
;
		for (const auto &note : b.notes) {
			
#line 989 "html.md"

	auto end = note.end();
	auto begin = note.begin();

#line 997 "html.md"

	if (
		status.state != HtmlState::inNotes
	) {
		
#line 1014 "html.md"

	if (
		status.state != HtmlState::inSlide
	) {
		out << "<div class=\"page\">\n";
	}
	status.state = HtmlState::inNotes;
	out << "<ul><li>\n";

#line 1001 "html.md"
;
	} else {
		out << "</li><li>\n";
	}
	process_content(out, begin, end);
	out << '\n';

#line 118 "html.md"
;
		}
		
#line 978 "html.md"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1127 "html.md"

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
			
#line 1140 "html.md"

	if (
		status.state ==
			HtmlState::afterSlide
	) {
		out << "</div>\n";
	}

#line 1152 "html.md"

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
			
#line 978 "html.md"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1127 "html.md"

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
		
#line 978 "html.md"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1127 "html.md"

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

#line 1169 "html.md"

	if (b.state == RS::img) {
		
#line 1186 "html.md"

	if (
		status.state ==
			HtmlState::afterSlides
	) {
		out << "<div class=\"slides\">\n";
	}
	status.state = HtmlState::inSlide;

#line 1171 "html.md"
;
		for (const auto &img : b.value) {
			
#line 1199 "html.md"

	out << "<div class=\"page\"><div class=\"slide\">\n";
	out	<< "<img src=\"" << img << "\">\n";
	out << "</div>\n";

#line 1173 "html.md"
;
		}
		for (const auto &note : b.notes) {
			
#line 989 "html.md"

	auto end = note.end();
	auto begin = note.begin();

#line 997 "html.md"

	if (
		status.state != HtmlState::inNotes
	) {
		
#line 1014 "html.md"

	if (
		status.state != HtmlState::inSlide
	) {
		out << "<div class=\"page\">\n";
	}
	status.state = HtmlState::inNotes;
	out << "<ul><li>\n";

#line 1001 "html.md"
;
	} else {
		out << "</li><li>\n";
	}
	process_content(out, begin, end);
	out << '\n';

#line 1176 "html.md"
;
		}
		
#line 978 "html.md"

	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

#line 1127 "html.md"

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}

#line 1178 "html.md"
;
		
#line 141 "html.md"

	out << "</div>\n";
	status.state = HtmlState::afterSlide;

#line 1179 "html.md"
;
	}

#line 94 "html.md"
;
	}

#line 368 "html.md"

	if (
		status.state == HtmlState::inCode
	) {
		std::cerr <<
			"unterminated code block\n";
	}

#line 380 "html.md"

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

	std::map<std::string, Input>::iterator curInput;
	std::vector<Block>::iterator curBlock;

#line 20 "view.md"

	bool write_files = true;

#line 27 "view.md"

	bool process_files = true;

#line 35 "view.md"

	bool html_files = true;

#line 77 "view.md"

	void draw_block() {
		
#line 195 "view.md"

	if (curBlock->state == RS::header) {
		
#line 204 "view.md"

	int i = 0;
	for (
		const auto &l : curBlock->value
	) {
		std::cout << ++i << ": ";
		
#line 218 "view.md"

	for (
		int i = 0;
		i < curBlock->level; ++i
	) {
		std::cout << '#';
	}

#line 210 "view.md"
;
		std::cout << ' ' << l << "\n\n";
	}

#line 197 "view.md"
;
	}

#line 230 "view.md"

	if (curBlock->state == RS::code) {
		
#line 239 "view.md"

	std::cout << "```\n";
	int i = 0;
	for (
		const auto &l : curBlock->value
	) {
		std::cout << ++i << ": " <<
			l << '\n';
	}
	std::cout << "```\n\n";

#line 232 "view.md"
;
	}

#line 254 "view.md"

	if (curBlock->state == RS::para) {
		
#line 263 "view.md"

	int i = 0;
	for (
		const auto &l : curBlock->value
	) {
		std::cout << ++i << ": " <<
			l << "\n\n";
	}

#line 256 "view.md"
;
	}

#line 276 "view.md"

	int j = 0;
	for (
		const auto &l : curBlock->notes
	) {
		std::cout << ++j << ": * " <<
			l << '\n';
	}
	std::cout << '\n';

#line 79 "view.md"
;
	}

#line 96 "view.md"

	void draw_position() {
		
#line 290 "view.md"

	auto &bs { curInput->second.blocks };
	std::cout << curInput->first << ':';
	int idx =
		(curBlock - bs.begin()) + 1;
	std::cout << idx;
	if (
		idx == static_cast<int>(bs.size())
	) {
		std::cout << " = $";
	}

#line 98 "view.md"
;
	}

#line 105 "view.md"

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
		res = line_;
		if (relative_) { res += cur; }
	}
	if (res < 0) { res = 0; }
	if (res > end) { res = end; }

#line 22 "line.md"
;
		return res;
	}

#line 40 "line.md"

	operator bool() const {
		return line_ >= 0 || relative_;
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

	int line_ { -1 };
	bool relative_ { false };

#line 67 "line.md"

	Line(int line, bool relative):
		line_ { line },
		relative_ { relative }
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
		return prev_ ?: last_;
	}

#line 29 "range.md"

	Line last() {
		return last_;
	}

#line 38 "range.md"

	Range &operator<<(const Line &l) {
		prev_ = last_;
		last_ = l;
		return *this;
	}

#line 49 "range.md"

	operator bool() {
		return last_;
	}

#line 8 "range.md"
;
		private:
			Line prev_;
			Line last_;
	};

#line 131 "line.md"
;
	
#line 139 "range.md"

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
	
#line 109 "range.md"

	if (range) {
		next = range.last()(
			Line::max, c.size() + 1
		) - 1;
		if (next < 0) { next = 0; }
		int p = range.prev()(
			Line::max, c.size() + 1
		) - 1;
		if (p < 0) { p = 0; }
		
#line 126 "range.md"

	if (p < next) {
		c.erase(
			c.begin() + p,
			c.begin() + next
		);
		next = p;
	}

#line 119 "range.md"
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
		cur.first.c_str()
	};

#line 36 "write.md"

	bool first = true;
	for (const auto &b : cur.second.blocks) {
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
			curInput->second.blocks.end()
	) {
		++curBlock;
	}

#line 53 "add.md"
;
		int i = curBlock -
			curInput->second.blocks.begin();
		
#line 83 "add.md"

	curInput->second.blocks.insert(
		curBlock,
		{
			state, { "REPLACE" }, {},
			state == RS::header ? 1 : 0
		}
	);

#line 56 "add.md"
;
		curBlock =
			curInput->second.blocks.begin() + i;
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

	int idx = 1;
	for (const auto &xx : inputs) {
		if (xx.first == curInput->first) { break; }
		++idx;
	}
	draw_number(idx);
	if (idx ==
		static_cast<int>(inputs.size())
	) {
		addstr(" = $");
	}
	addch(' ');
	addstr(curInput->first.c_str());
	addch(':');

#line 332 "ncurses.md"

	auto &bs { curInput->second.blocks };
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

#line 1356 "index.md"

	using Inputs_Frag_Map = std::map<std::string, Frag_Map>;

	class Frag_State {
		public:
			std::unique_ptr<Frag_State> parent;
			Inputs_Frag_Map state;
			Frag_State(std::unique_ptr<Frag_State> &&p): parent { std::move(p) } { }
			Frag *meta = nullptr;
			std::string meta_path;
			std::string meta_name;
			std::map<std::string, std::string> meta_values;
	};

	std::unique_ptr<Frag_State> all_frags_ = std::move(std::make_unique<Frag_State>(nullptr));
	Frag_State *cur_state_ = nullptr;

	Frag_State &cur_state() {
		return cur_state_ ? *cur_state_ : *all_frags_;
	}

	Frag *find_frag(Frag_State &state, const std::string &in, const std::string &key) {
		auto got { state.state[in].find(key) };
		if (got != state.state[in].end()) {
			return &got->second;
		}
		if (state.parent) {
			Frag *pg = find_frag(*state.parent, in, key);
			if (pg) {
				return &state.state[in].insert({ key, { key, pg } }).first->second;
			}
		}
		return nullptr;
	}

	Frag *find_frag(const std::string &in, const std::string &key) {
		return find_frag(cur_state(), in, key);
	}

	Frag *find_frag_in_files(const std::string &path, const std::string &key, std::string *got_path) {
		std::string p { path };
		for (;;) {
			Frag *f { find_frag(p, key) };
			if (f) {
				if (got_path) { *got_path = p; }
				return f;
			}
			const Input &i { inputs[p] };
			if (i.prev.empty()) { return nullptr; }
			p = i.prev;
		}
	}

	Frag *find_frag(const std::string &path, const std::string &key, bool local, std::string *got_path) {
		if (local) {
			if (got_path) { *got_path = path; }
			return find_frag(path, key);
		} else {
			Frag *f { nullptr };
			Input &i { inputs[path] };
			if (! i.prev.empty()) {
				f = find_frag_in_files(i.prev, key, got_path);
			}
			if (! f) {
				f = find_frag(std::string { }, key);
				if (f) {
					if (got_path) { *got_path = std::string { }; }
				}
			}
			return f;
		}
	}

	Frag *find_frag(const Frag_Ref &ref, std::string *got_path) {
		return find_frag(ref.path, ref.name, ref.local, got_path);
	}

	Frag &add_frag(Frag_State &state, const std::string &in, const std::string &key) {
		Frag *prev { nullptr };
		if (state.parent) {
			prev = &add_frag(*state.parent, in, key);
		}
		Frag &res { state.state[in].insert({ key, { key, prev } }).first->second };
		return res;
	}

	Frag &add_frag(const std::string &in, const std::string &key) {
		return add_frag(cur_state(), in, key);
	}

	Frag &get_frag(const std::string &path, const std::string &key, bool local) {
		Frag *f { find_frag(path, key, local) };
		if (f) { return *f; }
		const std::string new_path { local ? path : std::string { } };
		return add_frag(new_path, key);
	}

	Frag &get_frag(const Frag_Ref &ref) {
		return get_frag(ref.path, ref.name, ref.local);
	}

	Frag_Map &frag_map(Frag_State &state, const std::string &in) {
		Frag_Map &cur { state.state[in] };
		if (state.parent) {
			Frag_Map &prev { frag_map(*state.parent, in) };
			for (auto &f: prev) {
				if (cur.find(f.first) == cur.end()) {
					cur.insert({ f.first, { f.first, &f.second } });
				}
			}
		}
		return cur;
	}
	Frag_Map &frag_map(const std::string &in) {
		return frag_map(cur_state(), in);
	}

	Frag_Map &frag_map() {
		return frag_map(std::string { });
	}

	void split_frag(const std::string &name, Frag *meta, std::map<std::string, std::string> &&values) {
		Frag_State &current = *all_frags_;
		current.meta = meta;
		current.meta_path = inputs.open_head();
		current.meta_values = std::move(values);
		current.meta_name = name;
		std::unique_ptr<Frag_State> n { std::move(std::make_unique<Frag_State>(std::move(all_frags_))) };
		all_frags_ = std::move(n);
		cur_state_ = nullptr;
	}

	void clear_frags() { 
		all_frags_ = std::move(std::make_unique<Frag_State>(nullptr)); cur_state_ = nullptr;
	}

	void eval_meta(Frag_State &fs) {
		if (fs.parent) {
			eval_meta(*fs.parent);
		}
		if (fs.meta) {
			
#line 1508 "index.md"

	std::ostringstream out;
	serializeFrag(fs.meta_name, *fs.meta, out, fs.meta_path);
	std::istringstream in { out.str() };
	std::string line;
	Frag *frag = nullptr;
	std::string cur_path = fs.meta_path;
	int cur_line { 1 };
	auto &cmd_values = fs.meta_values;
	cur_state_ = &fs;
	while (std::getline(in, line)) {
		auto end = line.cend();
		for (
			auto i = line.cbegin();
			i != end; ++i
		) {
			
#line 397 "index.md"

	if (*i == '@') {
		auto nb = i + 1;
		auto ne = nb;
		
#line 415 "index.md"

	while (ne != end && *ne != '(') {
		if (! isalpha(*ne)) {
			ne = end;
			break;
		}
		++ne;
	}

#line 401 "index.md"
;
		if (ne != end && ne != nb) {
			std::string name { nb, ne };
			
#line 429 "index.md"

	auto ab = ne + 1; auto ae = ab;
	while (ae != end && *ae != ')') {
		if (*ae == '@') {
			
#line 449 "index.md"

	if (++ae == end) { break; }
	if (isalpha(*ae)) {
		
#line 458 "index.md"

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

#line 452 "index.md"
;
	}

#line 433 "index.md"
;
		}
		++ae;
	}
	if (ae != end) {
		std::string arg {ab, ae};
		
#line 478 "index.md"

	i = ae;
	bool outside = ! frag;
	do {
		if (outside && ! blockLimit) {
			break;
		}
		
#line 604 "index.md"

	if (name == "def") {
		ASSERT_NOT_FRAG();
		frag = &get_frag(cur_path, arg, true);
		CHECK_NOT_DEFINED();
		break;
	}

#line 629 "index.md"
 {
	auto i { cmd_values.find(name) };
	if (i != cmd_values.end()) {
		frag->add(i->second, cur_path, cur_line);
		break;
	}
} 
#line 639 "index.md"

	if (name == "end" || name == "End") {
		ASSERT_FRAG();
		if (frag->is_meta()) {
			std::string pattern;
			std::map<std::string, std::string> values;
			parse_args(arg, pattern, values);
			if (frag->name == pattern) {
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
			
#line 668 "index.md"

	ASSERT_MSG(frag->name == arg,
		"closing [" << arg <<
		"] != [" << frag->name << ']'
	);

#line 658 "index.md"
;
			frag = nullptr;
		}
		break;
	}

#line 690 "index.md"

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
			frag = &get_frag(cur_path, arg, true);
			CHECK_DEFINED();
		}
		break;
	}

#line 744 "index.md"

	if (name == "put") {
		if (! frag && arg.find('@') != std::string::npos) {
			std::string pattern;
			std::map<std::string, std::string> values;
			parse_args(arg, pattern, values);
			Frag *sub = &get_frag(cur_path, pattern, true);
			sub->addMultiple();
			split_frag(pattern, sub, std::move(values));
		} else {
			ASSERT_MSG(frag, "@put" << "(" <<
				arg << ") not in frag"
			);
			Frag *sub = &get_frag(cur_path, arg, true);
			ASSERT(sub);
			
#line 772 "index.md"

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

#line 759 "index.md"
;
			sub->addExpand();
			frag->add(Frag_Ref { cur_path, arg, true });
		}
		break;
	}

#line 789 "index.md"

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

#line 807 "index.md"

	if (name == "mul") {
		ASSERT_MSG(frag,
			"@mul not in frag"
		);
		Frag *sub = &get_frag(cur_path, arg, true);
		if (sub) {
			
#line 825 "index.md"

	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}

#line 814 "index.md"
;
			sub->addMultiple();
			frag->add(Frag_Ref { cur_path, arg, true });
		}
		break;
	}

#line 838 "index.md"

	if (name == "Def") {
		
#line 848 "index.md"

	ASSERT_MSG(! frag,
		"@Def in frag [" <<
		frag->name << ']'
	);
	frag = &get_frag(cur_path, arg, false);
	if (isPopulatedFrag(frag)) {
		std::cerr << "Frag [" <<
			arg << "] already defined\n";
	}

#line 840 "index.md"
;
		break;
	}

#line 863 "index.md"

	if (name == "Add") {
		
#line 873 "index.md"

	ASSERT_MSG(! frag, "@Add in frag [" <<
		frag->name << ']'
	);
	frag = &get_frag(cur_path, arg, false);
	if (! isPopulatedFrag(frag)) {
		std::cerr << "Frag [" << arg <<
			"] not defined\n";
	}

#line 865 "index.md"
;
		break;
	}

#line 889 "index.md"

	if (name == "rep") {
		ASSERT_MSG(! frag,
			"@rep in frag [" <<
				frag->name << ']'
		);
		frag = &get_frag(cur_path, arg, true);
		
#line 919 "index.md"

	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();

#line 896 "index.md"
;
		break;
	}

#line 904 "index.md"

	if (name == "Rep") {
		ASSERT_MSG(! frag,
			"@Rep in frag [" <<
				frag->name << ']'
		);
		frag = &get_frag(cur_path, arg, false);
		
#line 919 "index.md"

	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();

#line 911 "index.md"
;
		break;
	}

#line 931 "index.md"

	if (name == "Put") {
		
#line 941 "index.md"

	ASSERT_MSG(frag, "@Put not in frag");
	Frag *sub = &get_frag(cur_path, arg, false);
	if (sub) {
		
#line 772 "index.md"

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

#line 945 "index.md"
;
		sub->addExpand();
		frag->add(Frag_Ref { cur_path, arg, false });
	}

#line 933 "index.md"
;
		break;
	}

#line 954 "index.md"

	if (name == "Mul") {
		
#line 964 "index.md"

	ASSERT_MSG(frag, "@Mul not in frag");
	Frag *sub = &get_frag(cur_path, arg, false);
	if (sub) {
		
#line 825 "index.md"

	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}

#line 968 "index.md"
;
		sub->addMultiple();
		frag->add(Frag_Ref { cur_path, arg, false });
	}

#line 956 "index.md"
;
		break;
	}

#line 977 "index.md"

	if (name == "priv") {
		ASSERT_MSG(frag,
			"@priv not in frag"
		);
		
#line 1000 "index.md"

	std::hash<std::string> h;
	auto cur {
		h(cur_path + ':' + arg) &
			0x7fffffff
	};

#line 1012 "index.md"

	std::ostringstream hashed;
	hashed << "_private_" <<
		cur << '_' <<
		arg;
	frag->add(
		hashed.str(), cur_path, cur_line
	);

#line 982 "index.md"
;
		break;
	}

#line 1027 "index.md"

	if (name == "magic") {
		ASSERT_MSG(frag,
			"@magic not in frag"
		);
		
#line 1040 "index.md"

	std::hash<std::string> h;
	auto cur {
		h(cur_path + ':' + arg) &
			0x7fffffff
	};

#line 1051 "index.md"

	std::ostringstream value;
	value << cur;
	frag->add(
		value.str(), cur_path, cur_line
	);

#line 1032 "index.md"
;
		break;
	}

#line 485 "index.md"
;
		
#line 515 "index.md"

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

#line 486 "index.md"
;
	} while (false);
	if (blockLimit && outside && frag) {
		--blockLimit;
	}

#line 439 "index.md"
;
		continue;
	}

#line 404 "index.md"
;
		}
	}

#line 1524 "index.md"
;
			process_char(frag, *i, cur_path, cur_line);
		}
		process_char(frag, '\n', cur_path, cur_line);
	}
	cur_state_ = nullptr;

#line 1497 "index.md"
;
		}
	}

	void eval_metas() {
		eval_meta(*all_frags_);
	}

#line 39 "index.md"

	int main(
		int argc,
		const char **argv
	) {
		
#line 115 "index.md"

	#if ! NDEBUG
		
#line 399 "frag.md"

	
#line 418 "frag.md"

	test_frag_name("abc");
	test_frag_name("");
	test_frag_name("A c");

#line 427 "frag.md"
 {
	Frag f { "ab", nullptr };
	ASSERT(f.empty());
} 
#line 446 "frag.md"
 {
	Frag_Entry entry;
} 
#line 453 "frag.md"
 {
	Frag f { "", nullptr };
	Write_State s { "" };
	Frag_Entry entry;
	ASSERT(entry.str(s).empty());
} 
#line 814 "frag.md"
 {
	clear_frags();
	Frag frag { "a", nullptr };
	addStringToFrag(&frag, "abc");
	addStringToFrag(&frag, "def");
	testFrag("a", frag, "abcdef");
} 
#line 825 "frag.md"
 {
	clear_frags();
	Frag &a { get_frag("", "a", true) };
	Frag &b { get_frag("", "b", true) };
	addStringToFrag(&a, "abc");
	b.add(Frag_Ref { "", "a", true });
	addStringToFrag(&b, "def");
	b.add(Frag_Ref { "", "a", true });
	testFrag("b", b, "abcdefabc");
} 
#line 839 "frag.md"
 {
	clear_frags();
	Frag &a { get_frag("", "a", false) };
	Frag &b { get_frag("", "b", true) };
	addStringToFrag(&a, "abc");
	b.add(Frag_Ref { "", "a", true });
	addStringToFrag(&b, "def");
	b.add(Frag_Ref { "", "a", false });
	testFrag("b", b, "abcdefabc");
} 
#line 400 "frag.md"
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

	
#line 248 "index.md"

	for (int i { 1 }; i < argc; ++i) {
		std::string arg { argv[i] };
		
#line 279 "index.md"
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
#line 302 "index.md"
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
#line 1297 "index.md"
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

#line 251 "index.md"
;
		
#line 320 "index.md"

	inputs.add(arg);
	continue;

#line 252 "index.md"
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

	
#line 1077 "index.md"

	if (write_files) {
		files_write();
	}

#line 1209 "index.md"

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
	std::cerr << "curInput == " << curInput->first << "; is end == " << (curInput == inputs.end()) << "\n";
	curBlock = curInput->second.blocks.begin();
	std::cerr << "curBlock == " << &*curBlock << "; is end == " << (curBlock == curInput->second.blocks.end()) << "\n";

#line 86 "view.md"

	draw_block();
	for (;;) {
		
#line 119 "view.md"

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

#line 126 "view.md"
;

#line 134 "view.md"

	if (cmd == "q" || cmd == "quit") {
		break;
	}

#line 143 "view.md"

	if (cmd == "n" || cmd == "next") {
		
#line 154 "view.md"

	int next = (curBlock -
		curInput->second.blocks.begin()) + 1;
	while (next >= static_cast<int>(
		curInput->second.blocks.size()
	)) {
		--next;
	}
	
#line 78 "range.md"

	if (range) {
		next = range.last()((curBlock -
			curInput->second.blocks.begin()) + 1,
			curInput->second.blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 162 "view.md"
;
	curBlock =
		curInput->second.blocks.begin() + next;

#line 145 "view.md"
;
		draw_block();
		continue;
	}

#line 170 "view.md"

	if (cmd == "p" || cmd == "prev") {
		
#line 181 "view.md"

	int next =curBlock -
		curInput->second.blocks.begin();
	if (next > 0) {
		--next;
	}
	
#line 78 "range.md"

	if (range) {
		next = range.last()((curBlock -
			curInput->second.blocks.begin()) + 1,
			curInput->second.blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 187 "view.md"
;
	curBlock =
		curInput->second.blocks.begin() + next;

#line 172 "view.md"
;
		draw_block();
		continue;
	}

#line 308 "view.md"

	if (cmd == "f" || cmd == "forward") {
		
#line 319 "view.md"

	int next = 1;
	for (const auto &xx : inputs) {
		if (xx.first == curInput->first) { break; }
		++next;
	}
	while (next >= static_cast<int>(
		inputs.size()
	)) {
		--next;
	}
	
#line 91 "range.md"

	if (range) {
		int idx = 1;
		for (const auto &xx : inputs) {
			if (xx.first == curInput->first) { break; }
			++idx;
		}
		next = range.last()(
			idx,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 330 "view.md"
;
	curInput = inputs.begin();
	for (; next > 0 && curInput != inputs.end(); --next) {
		++curInput;
	}
	curBlock = curInput->second.blocks.begin();

#line 310 "view.md"
;
		draw_block();
		continue;
	}

#line 341 "view.md"

	if (cmd == "b" || cmd == "backward") {
		
#line 352 "view.md"

	int next = 0;
	for (const auto &xx : inputs) {
		if (xx.first == curInput->first) { break; }
		++next;
	}
	if (next) {
		--next;
	}
	
#line 91 "range.md"

	if (range) {
		int idx = 1;
		for (const auto &xx : inputs) {
			if (xx.first == curInput->first) { break; }
			++idx;
		}
		next = range.last()(
			idx,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 361 "view.md"
;
	curInput = inputs.begin();
	for (; next > 0 && curInput != inputs.end(); --next) {
		++curInput;
	}
	curBlock = curInput->second.blocks.begin();

#line 343 "view.md"
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
			curInput->second.blocks.begin();
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
			curInput->second.blocks.begin();
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
				curInput->second.blocks.end()
			) {
				
#line 121 "add.md"

	int i = curBlock -
		curInput->second.blocks.begin();
	curInput->second.blocks.insert(
		curBlock, *curBlock
	);
	curBlock = curInput->second.blocks.begin() +
		i + 1;

#line 107 "add.md"
;
			}
		} else {
			std::cerr << "! no file\n";
		}
		continue;
	}

#line 89 "view.md"
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
	curBlock = curInput->second.blocks.begin();
	draw_page();

#line 163 "ncurses.md"

	int ch;
	try {
		for (;;) {
			switch (ch = getch()) {
				
#line 178 "ncurses.md"

	case 'q': throw End_Of_Curses {};

#line 347 "ncurses.md"

	case 'n': {
		
#line 358 "ncurses.md"

	int next = (curBlock -
		curInput->second.blocks.begin()) + 1;
	while (next >= static_cast<int>(
		curInput->second.blocks.size()
	)) {
		--next;
	}
	
#line 78 "range.md"

	if (range) {
		next = range.last()((curBlock -
			curInput->second.blocks.begin()) + 1,
			curInput->second.blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 366 "ncurses.md"
;
	curBlock = curInput->second.blocks.begin() +
		next;

#line 349 "ncurses.md"
;
		draw_page();
		break;
	}

#line 374 "ncurses.md"

	case 'p' : {
		
#line 385 "ncurses.md"

	int next = curBlock -
		curInput->second.blocks.begin();
	if (next > 0) {
		--next;
	}
	
#line 78 "range.md"

	if (range) {
		next = range.last()((curBlock -
			curInput->second.blocks.begin()) + 1,
			curInput->second.blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 391 "ncurses.md"
;
	curBlock =
		curInput->second.blocks.begin() + next;

#line 376 "ncurses.md"
;
		draw_page();
		break;
	}

#line 399 "ncurses.md"

	case 'f': {
		
#line 410 "ncurses.md"

	int next = 1;
	for (const auto &xx : inputs) {
		if (xx.first == curInput->first) { break; }
		++next;
	}
	while (next >= static_cast<int>(
		inputs.size()
	)) {
		--next;
	}
	
#line 91 "range.md"

	if (range) {
		int idx = 1;
		for (const auto &xx : inputs) {
			if (xx.first == curInput->first) { break; }
			++idx;
		}
		next = range.last()(
			idx,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 421 "ncurses.md"
;
	curInput = inputs.begin();
	for (; next > 0 && curInput != inputs.end(); --next) {
		++curInput;
	}
	curBlock = curInput->second.blocks.begin();

#line 401 "ncurses.md"
;
		draw_page();
		continue;
	}

#line 432 "ncurses.md"

	case 'b': {
		
#line 443 "ncurses.md"

	int next = 0;
	for (const auto &xx : inputs) {
		if (xx.first == curInput->first) { break; }
		++next;
	}
	if (next) {
		--next;
	}
	
#line 91 "range.md"

	if (range) {
		int idx = 1;
		for (const auto &xx : inputs) {
			if (xx.first == curInput->first) { break; }
			++idx;
		}
		next = range.last()(
			idx,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 452 "ncurses.md"
;
	curInput = inputs.begin();
	for (; next > 0 && curInput != inputs.end(); --next) {
		++curInput;
	}
	curBlock = curInput->second.blocks.begin();

#line 434 "ncurses.md"
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
