
	
	
	#include <string>

	#include <fstream>

	#include <vector>

	#include <iostream>
	#include <exception>

	#include <vector>

	#include <sstream>

	#include <map>

	#include <iostream>
	#include <vector>

	#include <algorithm>

	#include <functional>
	#include <sstream>

	#include <string>

	#include <cctype>

	#include <set>

	#include <limits>
;

	
	
	
	struct No_More_Lines {};

	
	
	enum class Read_State {
		new_element
		,
	header
,
	code,
	after_code
,
	notes
,
	para

	};
	using RS = Read_State;

	struct Block {
		Read_State state;
		std::vector<std::string> value;
		std::vector<std::string> notes;
		int level;
	};


	
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
;
	
	class Frag;

	class FragEntry {
		std::string _str;
		std::string _file;
		int _first_line;
		int _last_line;
	public:
		const Frag *frag;
		
	FragEntry(
		Frag *frag = nullptr
	):
		frag { frag }
	{}

	const std::string &str() const {
		return _str;
	}

	void add(
		char ch, const std::string &file,
		int line
	) {
		
	if (
		_file.empty() || _first_line <= 0
	) {
		_file = file;
		_first_line = line;
	}
	_last_line = line;
;
		_str += ch;
	}

	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		
	if (
		_file.empty() || _first_line <= 0
	) {
		_file = file;
		_first_line = line;
	}
	_last_line = line;
;
		_str += value;
	}

	bool canAdd(
		const std::string &file,
		int line
	) {
		
	if (
		! _file.empty() && file != _file
	) {
		return false;
	}

	if (
		_last_line > 0 &&
		_last_line != line &&
		_last_line + 1 != line
	) {
		return false;
	}

	return true;
;
		return false;
	}
;
	};

	class Frag {
		std::vector<FragEntry> _entries;
		int _expands;
		int _multiples;
	public:
		const std::string name;
		
	bool isFile() const {
		static const std::string prefix {
			"file: "
		};
		std::string p {
			name.substr(0, prefix.size())
		};
		return p == prefix;
	}

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

	void clear() {
		_entries.clear();
	}

	bool empty() const {
		return _entries.empty();
	}

	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		if (value.empty()) { return; }
		
	if (_entries.empty()) {
		_entries.push_back(FragEntry {});
	} else if (
		! _entries.back().canAdd(
			file, line
		)
	) {
		_entries.push_back(FragEntry {});
	}
;
		_entries.back().add(
			value, file, line
		);
	}

	void add(
		char ch,
		const std::string &file,
		int line
	) {
		
	if (_entries.empty()) {
		_entries.push_back(FragEntry {});
	} else if (
		! _entries.back().canAdd(
			file, line
		)
	) {
		_entries.push_back(FragEntry {});
	}
;
		_entries.back().add(
			ch, file, line
		);
	}

	Frag &add(Frag *child);

	auto begin() const {
		return _entries.cbegin();
	}

	auto end() const {
		return _entries.cend();
	}

	int expands() const {
		return _expands;
	}

	void addExpand() {
		++_expands;
	}

	int multiples() const {
		return _multiples;
	}

	void addMultiple() {
		++_multiples;
	}
;
	};

	void testFragName(
		const std::string &name
	) {
		Frag f(name);
		ASSERT(f.name == name);
	}

	bool isPopulatedFrag(
		const Frag *f
	) {
		return f && ! f->empty();
	}

	
	bool isFragInFrag(
		const Frag *needle,
		const Frag *haystack
	) {
		ASSERT(needle);
		ASSERT(haystack);
		
	if (needle == haystack) {
		return true;
	}
;
		
	for (const auto &i : *haystack)  {
		if (! i.frag) { continue; }
		if (isFragInFrag(
			needle, i.frag
		)) {
			return true;
		}
	}
;
		return false;
	}

	Frag &Frag::add(Frag *child) {
		ASSERT(child);
		
	ASSERT(! isFragInFrag(
		this, child
	));
;
		
	_entries.push_back(
		FragEntry { child }
	);
;
		return *this;
	}

	void serializeFrag(
		const Frag &frag,
		std::ostream &out,
		bool writeLineMacros
	) {
		
	for (const auto &entry : frag) {
		if (entry.frag) {
			serializeFrag(
				*entry.frag, out,
				writeLineMacros
			);
		}
		out << entry.str();
	}
;
	}

	void testFrag(
		const Frag &frag,
		const std::string &expected
	) {
		
	std::ostringstream buffer;
	serializeFrag(frag, buffer, false);
	ASSERT(buffer.str() == expected);
;
	}

	void addStringToFrag(
		Frag *frag,
		const std::string &str
	) {
		frag->add(
			str, std::string {}, 0
		);
	}

	using FragMap =
		std::map<std::string, Frag>;
;

	FragMap root;
;

	class Input {
		public:
			
	Input(const std::string &path):
		_path { path }
	{}

	Input(
		const Input &
	) = delete;
	Input(
		Input &&
	) = default;

	Input &operator=(
		const Input &
	) = delete;
	Input &operator=(
		Input &&
	) = default;

	const std::string &path() const {
		return _path;
	}

	std::vector<Block> blocks;

	FragMap frags;
;
		private:
			std::string _path;
	};
;
	class Open_Input {
		public:
			
	Open_Input(const std::string &path):
		_input { path },
		_file { path.c_str() }
	{}

	Open_Input(
		const Open_Input &
	) = delete;
	Open_Input(
		Open_Input &&
	) = default;

	Open_Input &operator=(
		const Open_Input &
	) = delete;
	Open_Input &operator=(
		Open_Input &&
	) = default;

	Input &input() { return _input; }

	const Input &input() const {
		return _input;
	}

	void read_line(std::string &line) {
		if (_file.is_open()) {
			
	if (std::getline(_file, line)) {
		
	++_line;
;
		return;
	}
	_file.close();
;
		}
		throw No_More_Lines {};
	}

	Read_State state = RS::new_element;

	int line() const {
		return _line;
	}
;
		private:
			
	Input _input;
	std::ifstream _file;

	int _line = 0;
;
	};
;
	
	class Inputs {
		public:
			
	void read_line(std::string &line);

	auto &cur() {
		ASSERT (! _open.empty());
		return _open.back();
	}

	auto begin() {
		return _used.begin();
	}

	auto end() {
		return _used.end();
	}

	void push(const std::string &path) {
		_used.push_back({ path });
		_open.push_back({ path });
	}

	bool has(
		const std::string &name
	) const {
		
	for (const auto &j : _used) {
		if (j.path() == name) {
			return true;
		}
	}
;
		return false;
	}

	Frag *find_local(const std::string &name) {
		ASSERT(! _open.empty());
		Input &i = _open.back().input();
		auto f = i.frags.find(name);
		if (f == i.frags.end()) { return nullptr; }
		return &f->second;
	}

	Frag *add_local(const std::string &name) {
		ASSERT(! _open.empty());
		Input &i = _open.back().input();
		return &i.frags.insert({ name, name }).first->second;
	}

	Frag *get_local(const std::string &name) {
		Frag *result = find_local(name);
		if (! result) {
			result = add_local(name);
		}
		return result;
	}

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
		auto f = root.find(name);
		if (f == root.end()) { return nullptr; }
		return &f->second;

	}

	Frag *add_global(const std::string &name) {
		return &root.insert({ name, name }).first->second;
	}

	Frag *get_global(const std::string &name) {
		Frag *result = find_global(name);
		if (! result) {
			result = add_global(name);
		}
		return result;
	}
;
		private:
			
	std::vector<Open_Input> _open;
	std::vector<Input> _used;
;
	};

	void Inputs::read_line(
		std::string &line
	) {
		
	while (! _open.empty()) {
		try {
			_open.back().read_line(line);
			return;
		}
		catch (const No_More_Lines &) {}
		
	for (auto &i : _used) {
		if (i.path() == _open.back().input().path()) {
			i = std::move(_open.back().input());
			break;
		}
	}
;
		_open.pop_back();
	}
	throw No_More_Lines {};
;
	}
;

	Inputs inputs;
;

	std::string stylesheet {
		"slides/slides.css"
	};

	int blockLimit = -1;

	using SI =
		std::string::const_iterator;

	void process_chars(
		Frag *frag, SI i, SI e
	) {
		
	if (frag) {
		std::string str {i, e};
		frag->add(
			str, inputs.cur().input().path(),
			inputs.cur().line()
		);
	}
;
	}

	void process_char(Frag *frag, char ch) {
		
	if (frag) {
		frag->add(
			ch, inputs.cur().input().path(),
			inputs.cur().line()
		);
	}
;
	}

	void expand_macro_arg(
		Frag *f, const std::string &arg
	) {
		auto b = arg.begin();
		auto e = arg.end();
		
	while (b != e) {
		auto x = std::find(b, e, '@');
		if (x != e) {
			
	f->add(
		std::string { b, x },
		inputs.cur().input().path(),
		inputs.cur().line()
	);
;
			b = x + 1;
			if (b != e) {
				f->add(
					*b,
					inputs.cur().input().path(),
					inputs.cur().line()
				);
				++b;
			}
		} else {
			
	f->add(
		std::string { b, e },
		inputs.cur().input().path(),
		inputs.cur().line()
	);
	b = e;
;
			b = e;
		}
	}
;
	}

	enum class HtmlState {
		nothing,
		inSlide,
		afterSlide,
		afterSlides
		
	, inCode

	, inNotes

	, inPara

	};

	struct HtmlStatus {
		
	HtmlStatus();
	HtmlState state;

	};

	inline HtmlStatus::HtmlStatus():
		state {
			HtmlState::nothing
		}
	{}
 
	void writeOneEscaped(
		std::ostream &out, char ch
	) {
		switch (ch) {
			
	case '<':
		out << "&lt;";
		break;
	case '>':
		out << "&gt;";
		break;
	case '&':
		out << "&amp;";
		break;

			default:
				out << ch;
		}
	}

	void writeEscaped(
		std::ostream &out,
		const std::string &str
	) {
		for (char ch : str) {
			writeOneEscaped(out, ch);
		}
	}

	
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

	using Set = std::set<std::string>;

	bool isKeyword(const std::string &s) {
		static Set reserved {
			
	"break", "case", "catch", "continue",
	"default", "delete", "else", "for",
	"if", "in", "new", "return", "static",
	"switch", "try", "typeof", "while",
	"class", "public", "private",
	"template", "typename", "using",
	"function", "throw"

		};
		return
			reserved.find(s) !=
				reserved.end() ||
					(s.size() && s[0] == '#');
	}

	bool isType(const std::string &s) {
		
	static Set reserved {
		
	"FILE", "auto", "bool", "char",
	"const", "enum", "extern", "int",
	"let", "long", "signed", "struct",
	"union", "unsigned", "void", "double",
	"string", "std", "ifstream",
	"istream", "ofstream", "ostream",
	"vector", "map", "list"

	};
	if (reserved.find(s) !=
		reserved.end()
	) {
		return true;
	}

	if (s.size() >= 2) {
		if (isupper(s[0]) &&
			islower(s[1])
		) {
			return true;
		}
	}
;
		return false;
	}

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

	void process_ident(
		std::ostream &out,
		const std::string ident,
		char w
	) {
		if (isKeyword(ident)) {
			span_str(out, "keyword", ident);
		} else if (w == '(') {
			span_str(out, "fn", ident);
		
	} else if (isType(ident)) {
		span_str(out, "type", ident);
	} else if (isNum(ident)) {
		span_str(out, "num", ident);

		} else {
			span_str(out, "var", ident);
		}
	}

	void writeMacroClass(
		std::ostream &out,
		const char *name
	) {
		out << "<span class=\"" <<
			name << "\">";
	}

	void writeMacroHeader(
		std::ostream &out,
		const std::string &name
	) {
		writeMacroClass(out, "macro");
		out << '@' << name << "(<span class=\"name\">";
	}
;
	void process_code(
		std::ostream &out,
		SI begin, SI end
	) {
		
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

	for (; begin != end; ++begin) {
		
	if (
		*begin == '`' ||
		*begin == '\'' ||
		*begin == '"'
	) {
		
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

	std::string name {begin, w + 1};
	span_str(out, "str", name);
	begin = w;
;
		continue;
	}

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
			
	while (ae != end && *ae != ')') {
		if (*ae == '@') {
			++ae;
			if (ae == end) { break; }
		}
		++ae;
	}
	if (ae != end) {
		std::string arg {ab, ae};
		
	do {
		
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

	if (name == "inc") {
		
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
;
		break;
	}

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

	if (name == "f" || name == "fn") {
		writeMacroClass(out, "fn");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

	if (name == "v" || name == "var") {
		writeMacroClass(out, "var");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

	if (name == "k" || name == "key") {
		writeMacroClass(out, "keyword");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

	if (name == "n" || name == "num") {
		writeMacroClass(out, "num");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

	if (name == "t" || name == "typ") {
		writeMacroClass(out, "type");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

	if (name == "b" || name == "br") {
		writeMacroClass(out, "virt");
		out << "</span><br/>";
		break;
	}

	if (name == "priv") {
		writeMacroClass(out, "var");
		out << "@priv(<span>";
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}

	if (name == "magic") {
		writeMacroClass(out, "num");
		out << "@magic(<span>";
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}
;
		
	std::cerr << "unknown macro @" <<
		name << '(' << arg << ")\n";
	writeOneEscaped(out, '@');
	writeEscaped(out, name);
	writeOneEscaped(out, '(');
	writeEscaped(out, arg);
	writeOneEscaped(out, ')');
;
	} while (false);
	begin = ae;
;
		continue;
	}
;
		}
	}

	auto w = begin;
	
	while (w != end && (
		std::isalnum(*w) ||
			*w == '_' || *w == '$' ||
			*w == '#'
	)) {
		++w;
	}
;
	if (w != begin) {
		
	std::string ident {begin, w};
	begin = w - 1;
	process_ident(
		out, ident,
		w != end ? *w : ' '
	);
;
		continue;
	}
;
		writeOneEscaped(out, *begin);
	}
;
	}

	void process_content(
		std::ostream &out,
		SI begin, SI end
	) {
		
	for(; begin != end; ++begin) {
		
	if (*begin == '`') {
		
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
;
	}

	if (
		*begin == '*' &&
		(begin + 1) != end &&
		*(begin + 1) == '*'
	) {
		
	auto w = begin + 2;
	while (
		w != end && (w + 1) != end &&
		(*w != '*' || *(w + 1) != '*')
	) {
		++w;
	}

	if (
		w != end && (w + 1 ) != end &&
		*w == '*' && *(w + 1) == '*'
	) {
		
	out << "<b>";
	writeEscaped(
		out, std::string {begin + 2, w}
	);
	out << "</b>";
	begin = w + 1;
;
		continue;
	}
;
	}
;
		writeOneEscaped(out, *begin);
	}
;
	}

	bool interactive = false;
	std::vector<Input>::iterator curInput;
	std::vector<Block>::iterator curBlock;

	void draw_block() {
		
	if (curInput == inputs.end()) {
		std::cerr << "! no file\n";
		return;
	}

	if (curBlock == curInput->blocks.end()) {
		std::cerr << "! end\n";
		return;
	}

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

	if (curBlock->state == RS::code) {
		std::cout << "```\n";
		int i = 0;
		for (const auto &l : curBlock->value) {
			std::cout << ++i << ": " << l << '\n';
		}
		std::cout << "```\n\n";
	}

	if (curBlock->state == RS::para) {
		int i = 0;
		for (const auto &l : curBlock->value) {
			std::cout << ++i << ": " << l << "\n\n";
		}
	}

	int j = 0;
	for (const auto &l : curBlock->notes) {
		std::cout << ++j << ": * " << l << '\n';
	}
	std::cout << '\n';
;
	}

	void draw_position() {
		
	if (curInput == inputs.end()) {
		std::cout << "no file:end\n";
		return;
	}

	std::cout << curInput->path() << ':';
	if (curBlock == curInput->blocks.end()) {
		std::cout << "end";
	} else {
		std::cout << (curBlock - curInput->blocks.begin() + 1);
	}
;
	}

	void trim(std::string &s) {
		while (! s.empty() && (s[0] & 0xff) <= ' ') {
			s.erase(0, 1);
		}
	}

	class Line {
		public:
			
	int operator()(
		int cur, int end
	) const {
		int res {};
		
	if (*this) {
		res = cur;
	} else {
		res = _line;
		if (_relative) { res += cur; }
	}
	if (res < 0) { res = 0; }
	if (res > end) { res = end; }
;
		return res;
	}

	operator bool() const {
		return _line < 0 && ! _relative;
	}

	Line() = default;

	static Line relative(int line) {
		return Line { line, true };
	}

	static Line line(int line) {
		return Line { line, false };
	}

	static Line begin() {
		return line(0);
	}

	static const int max =
		std::numeric_limits<int>::max();
		
	static Line end() {
		return line(max);
	}
;
		private:
			
	int _line = -1;
	bool _relative = false;

	Line(int line, bool relative):
		_line { line },
		_relative { relative }
	{}
;
	};

	
	class Range {
		public:
			
	Line prev() {
		return _prev ? _last: _prev;
	}

	Line last() {
		return _last;
	}

	Range &operator<<(const Line &l) {
		_prev = _last;
		_last = l;
		return *this;
	}

	operator bool() {
		return _last;
	}
;
		private:
			Line _prev;
			Line _last;
	};
;
	
	Range range;
;

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

	Line get_line(std::string &s) {
		Line line {};
		do {
			
	if (s[0] == '.') {
		s.erase(0, 1);
		line = Line::relative(0);
		break;
	}

	if (s[0] == '+') {
		s.erase(0, 1);
		int n = get_number(s);
		line = Line::relative(n);
		break;
	}

	if (s[0] == '-') {
		s.erase(0, 1);
		int n = -get_number(s);
		line = Line::relative(n);
		break;
	}

	if (s[0] == '$') {
		line = Line::end();
		s.erase(0, 1);
		continue;
	}

	if (isdigit(s[0])) {
		int n = get_number(s);
		line = Line::line(n);
		continue;
	}
;
		} while (false);
		trim(s);
		return line;
	}

	void insert_before(
		const std::string &prefix,
		std::vector<std::string> &c
	) {
		
	int next = c.size();
	
	if (! range) {
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
		c.insert(c.begin() + next, t);
		++next;
	}
	draw_block();
;
	}

	bool valid_cur() {
		if (curInput == inputs.end()) {
			std::cerr << "! no file\n";
			return false;
		}
		if (curBlock == curInput->blocks.end()) {
			std::cerr << "! end\n";
			return false;
		}
		return true;
	}

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

	void add_block(Read_State state) {
		
	if (curInput != inputs.end()) {
		if (curBlock != curInput->blocks.end()) {
			++curBlock;
		}
		int i = curBlock - curInput->blocks.begin();
		curInput->blocks.insert(curBlock, { state, { "REPLACE" }, {} });
		curBlock = curInput->blocks.begin() + i;
	} else {
		std::cerr << "! no file\n";
	}
	draw_block();
;
	}

	int main(
		int argc,
		const char **argv
	) {
		
	#if ! NDEBUG
		
	
	testFragName("abc");
	testFragName("");
	testFragName("A c");
	{
		Frag f { "ab" };
		ASSERT(f.empty());
	}

	{
		FragEntry entry;
		ASSERT(! entry.frag);
	}

	{
		FragEntry entry;
		ASSERT(entry.str().empty());
	}
 {
	Frag frag { "" };
	addStringToFrag(&frag, "abc");
	addStringToFrag(&frag, "def");
	testFrag(frag, "abcdef");
}  {
	Frag a { "" };
	Frag b { "" };
	addStringToFrag(&a, "abc");
	b.add(&a);
	addStringToFrag(&b, "def");
	b.add(&a);
	testFrag(b, "abcdefabc");
} ;

	
	ASSERT(Line {});
	ASSERT(! Line::begin());
	ASSERT(! Line::end());
	ASSERT(! Line::end());
	ASSERT(! Line::line(0));
	ASSERT(! Line::relative(0));
	ASSERT(! Line::relative(-2));

	ASSERT(Line {}(5, 10) == 5);
	ASSERT(Line::begin()(5, 10) == 0);
	ASSERT(Line::end()(5, 10) == 10);

	ASSERT(Line::line(0)(5, 10) == 0);
	ASSERT(Line::line(6)(5, 10) == 6);
	ASSERT(Line::line(20)(5, 10) == 10);

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
 {
	std::string f = "+3";
	ASSERT(
		get_line(f)(5, 10) == 8
	);
}  {
	std::string f = ".";
	ASSERT(
		get_line(f)(5, 10) == 5
	);
}  {
	std::string f = "$";
	ASSERT(
		get_line(f)(5, 10) == 10 
	);
} ;
;
	#endif

	
	bool someFile { false };
	for (int i { 1 }; i < argc; ++i) {
		std::string arg { argv[i] };
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
}  {
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
	if (
		arg == "-i" ||
		arg == "--interactive"
	) {
		interactive = true;
		continue;
	}
;
		
	if (! someFile) {
		inputs.push(argv[1]);
		someFile = true;
		continue;
	}
;
		ASSERT_MSG(false,
			"unknown argument [" <<
			argv[i] << ']'
		);
	}

	if (! someFile) {
		inputs.push("index.x");
	}
;

	 {
	
	Frag *frag { nullptr };
;
	std::string line;
	try { for (;;) {
		inputs.read_line(line);
		
	do {
		auto &state = inputs.cur().state;
		
	auto &blocks =
		inputs.cur().input().blocks;
;
		
	if (
		line == "```" &&
		state == RS::new_element
	) {
		state = RS::code;
		
	blocks.push_back({
		RS::code, {}, {}, 0
	});
;
		break;
	}

	if (state == RS::code) {
		if (line == "```") {
			state = RS::after_code;
		} else {
			
	blocks.back().value.push_back(
		line
	);
;
		}
		break;
	}
;
		
	if (line.empty()) {
		state = RS::new_element;
		break;
	}
;
		
	if (
		line[0] == '#' &&
		state == RS::new_element
	) {
		state = RS::header;
		
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
;
		break;
	}

	if (line[0] == '*') {
		if (
			state == RS::header ||
			state == RS::after_code ||
			state == RS::notes
		) {
			state = RS::notes;
			
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
;
			break;
		}
	}

	if (
		line[0] == ' ' &&
		state == RS::notes
	) {
		
	blocks.back().notes.back() +=
		line;
;
		break;
	}

	if (line[0] != ' ') {
		if (
			state == RS::new_element ||
			state == RS::para
		) {
			
	if (state == RS::new_element) {
		if (blocks.empty() || blocks.back().state != RS::para) {
			blocks.push_back({
				RS::para, {}, {}, 0
			});
		}
		blocks.back().value.push_back(line);
	}
;
			
	if (state == RS::para) {
		blocks.back().value.back() +=
			" " + line;
	}
;
			state = RS::para;
			break;
		}
	}
;
		
	std::cerr << "!! " << line << '\n';
;
	} while (false);

	auto end = line.cend();
	for (
		auto i = line.cbegin();
		i != end; ++i
	) {
		
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
			std::string name {nb, ne};
			
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
		
	i = ae;
	bool outside = !frag;
	do {
		if (! frag && ! blockLimit) { break; }
		
	if (name == "def") {
		ASSERT_MSG(! frag, "@def(" << arg << ") in frag [" << frag->name << ']');
		frag = inputs.get_local(arg);
		if (isPopulatedFrag(frag)) {
			std::cerr << "frag [" << arg << "] already defined\n";
		}
		break;
	}

	if (name == "end" || name == "End") {
		ASSERT_MSG(frag,
			'@' << name << "(" << arg <<
			") not in frag"
		);
		
	ASSERT_MSG(frag->name == arg,
		"closing [" << arg <<
		"] != [" << frag->name << ']'
	);
;
		frag = nullptr;
		break;
	}

	if (name == "add") {
		ASSERT_MSG(! frag, "add in frag " << frag->name);
		frag = inputs.get_local(arg);
		if (! isPopulatedFrag(frag)) {
			std::cerr << "frag [" << arg << "] not defined\n";
		}
		break;
	}

	if (name == "put") {
		ASSERT_MSG(frag,
			"@put not in frag"
		);
		Frag *sub = inputs.get_local(arg);
		if (sub) {
			
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
;
			sub->addExpand();
			frag->add(sub);
		}
		break;
	}

	if (name == "inc") {
		ASSERT_MSG(! frag,
			"include in frag [" << frag->name << ']'
		);
		if (! inputs.has(arg)) {
			inputs.push(arg);
		}
		break;
	}

	if (name == "mul") {
		ASSERT_MSG(frag,
			"@mul not in frag"
		);
		Frag *sub = inputs.get_local(arg);
		if (sub) {
			
	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}
;
			sub->addMultiple();
			frag->add(sub);
		}
		break;
	}

	if (name == "Def") {
		ASSERT_MSG(! frag, "@Def in frag [" << frag->name << ']');
		frag = inputs.get_global(arg);
		if (isPopulatedFrag(frag)) {
			std::cerr << "Frag [" << arg << "] already defined\n";
		}
		break;
	}

	if (name == "Add") {
		ASSERT_MSG(! frag, "@Add in frag [" << frag->name << ']');
		frag = inputs.get_global(arg);
		if (! isPopulatedFrag(frag)) {
			std::cerr << "{{" << line << "}}\n";
			std::cerr << "Frag [" << arg << "] not defined\n";
			std::cerr << inputs.cur().input().path() << ':' << inputs.cur().line() << '\n';
		}
		break;
	}

	if (name == "rep") {
		ASSERT_MSG(! frag,
			"@rep in frag [" << frag->name << ']'
		);
		frag = inputs.get_local(arg);
		
	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();
;
		break;
	}

	if (name == "Rep") {
		ASSERT_MSG(! frag,
			"@Rep in frag [" << frag->name << ']'
		);
		frag = inputs.get_global(arg);
		
	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();
;
		break;
	}

	if (name == "Put") {
		ASSERT_MSG(frag,
			"@Put not in frag"
		);
		Frag *sub = inputs.get_global(arg);
		if (sub) {
			
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
;
			sub->addExpand();
			frag->add(sub);
		}
		break;
	}

	if (name == "Mul") {
		ASSERT_MSG(frag,
			"@Mul not in frag"
		);
		Frag *sub = inputs.get_global(arg);
		if (sub) {
			
	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}
;
			sub->addMultiple();
			frag->add(sub);
		}
		break;
	}

	if (name == "priv") {
		ASSERT_MSG(frag,
			"@priv not in frag"
		);
		
	std::hash<std::string> h;
	unsigned cur {
		h(inputs.cur().input().path() +
			':' + arg) &
				0x7fffffff
	};

	std::ostringstream hashed;
	hashed << "_private_" <<
		cur << '_' <<
		name;
	frag->add(
		hashed.str(),
		inputs.cur().input().path(),
		inputs.cur().line()
	);
;
		break;
	}

	if (name == "magic") {
		ASSERT_MSG(frag,
			"@magic not in frag"
		);
		
	std::hash<std::string> h;
	unsigned cur {
		h(inputs.cur().input().path() +
			':' + arg) &
				0x7fffffff
	};

	std::ostringstream value;
	value << cur;
	frag->add(
		value.str(),
		inputs.cur().input().path(),
		inputs.cur().line()
	);
;
		break;
	}
;
		
	if (frag) {
		expand_macro_arg(frag, arg);
	}
;
	} while (false);
	if (blockLimit && outside && frag) {
		--blockLimit;
	}
;
		continue;
	}
;
		}
	}
;
		process_chars(frag, i, i + 1);
	}
	process_char(frag, '\n');
;
	} }
	catch (const No_More_Lines &) {}
} ;

	
	for (auto &i : root) {
		const Frag *frag { &i.second };
		 {
	if (frag->isFile()) {
		
	std::ofstream out(
		frag->name.substr(6).c_str()
	);
	serializeFrag(*frag, out, false);
	out.close();
;
	}
}  {
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
	if (frag->multiples() == 1) {
		std::cerr <<
			"multiple frag [" <<
			frag->name <<
			"] only used once\n";
	}

	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not populated\n";
	}
;
	}

	for (auto &j : inputs) {
		for (auto &i : j.frags) {
			const Frag *frag {
				&i.second
			};
			 {
	if (frag->isFile()) {
		
	std::ofstream out(
		frag->name.substr(6).c_str()
	);
	serializeFrag(*frag, out, false);
	out.close();
;
	}
}  {
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
	if (frag->multiples() == 1) {
		std::cerr <<
			"multiple frag [" <<
			frag->name <<
			"] only used once\n";
	}

	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not populated\n";
	}
;
		}
	}

	for (auto &i : root) {
		const Frag *frag { &i.second };
		 {
	const std::string cmd { frag->cmd() };
	if (cmd.size()) {
		
	std::ostringstream out {};
	serializeFrag(*frag, out, false);
	std::string o { out.str() };
	std::FILE *f {
		popen(cmd.c_str(), "w")
	};
	if (f) {
		std::fwrite(o.c_str(), o.size(), 1, f);
		pclose(f);
	}
;
	}
} ;
	}

	for (auto &j : inputs) {
		for (auto &i : j.frags) {
			const Frag *frag {
				&i.second
			};
			 {
	const std::string cmd { frag->cmd() };
	if (cmd.size()) {
		
	std::ostringstream out {};
	serializeFrag(*frag, out, false);
	std::string o { out.str() };
	std::FILE *f {
		popen(cmd.c_str(), "w")
	};
	if (f) {
		std::fwrite(o.c_str(), o.size(), 1, f);
		pclose(f);
	}
;
	}
} ;
		}
	}
;

	
	for (auto &cur : inputs) {
		
	const std::string &name { cur.path() };
	std::string outPath {
		name.substr(0, name.size() - 2) +
		".html"
	};
	std::ofstream out { outPath.c_str() };
	
	
	HtmlStatus status;
	for (const auto &b : cur.blocks) {
		
	if (b.state == RS::header) {
		
	
	switch (status.state) {
		case HtmlState::nothing: {
			
	out << "<!doctype html>\n";
	out << "<html lang=\"de\">\n";
	out << "<head>\n";
	
	out << "<meta charset=\"utf-8\">\n";
	out << "<title>";
	writeEscaped(out, b.value[0]);
	out << "</title>\n";
	out << "<link rel=\"stylesheet\" "
		"type=\"text/css\" href=\""
		<< stylesheet << "\">";
;
	out << "</head>\n";
	out << "<body>\n";
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
;
	 {
	out << "<h" << b.level << '>';
	const auto &n = b.value[0];
	process_content(out, n.begin(), n.end());
	out << "</h" << b.level << ">\n";
} ;
	out << "<div class=\"slides\">\n";
	out << "<div>\n";
	out << "<div>\n";
	 {
	out << "<h" << b.level << '>';
	const auto &n = b.value[0];
	process_content(out, n.begin(), n.end());
	out << "</h" << b.level << ">\n";
} ;
	out << "</div>\n";
	status.state = HtmlState::inSlide;
	for (const auto &note : b.notes) {
		
	auto end = note.end();
	auto begin = note.begin();

	if (
		status.state != HtmlState::inNotes
	) {
		
	if (
		status.state != HtmlState::inSlide
	) {
		out << "<div>\n";
	}
	status.state = HtmlState::inNotes;
	out << "<ul><li>\n";
;
	} else {
		out << "</li><li>\n";
	}
	process_content(out, begin, end);
	out << '\n';
;
	}
	
	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}
;
	
	out << "</div>\n";
	status.state = HtmlState::afterSlide;
;
;
	}

	if (b.state == RS::code) {
		
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
;
		for (const auto &code : b.value) {
			
	process_code(
		out, code.begin(), code.end()
	);
	out << "<br/>\n";
;
		}
		
	out << "</code></div>\n";
	status.state = HtmlState::inSlide;
;
		for (const auto &note : b.notes) {
			
	auto end = note.end();
	auto begin = note.begin();

	if (
		status.state != HtmlState::inNotes
	) {
		
	if (
		status.state != HtmlState::inSlide
	) {
		out << "<div>\n";
	}
	status.state = HtmlState::inNotes;
	out << "<ul><li>\n";
;
	} else {
		out << "</li><li>\n";
	}
	process_content(out, begin, end);
	out << '\n';
;
		}
		
	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}
;
		
	out << "</div>\n";
	status.state = HtmlState::afterSlide;
;
	}

	if (b.state == RS::para) {
		for (const auto &para : b.value) {
			
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
;
			
	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}
;
		}
		
	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}

	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}
;
	}
;
	}

	if (
		status.state == HtmlState::inCode
	) {
		std::cerr <<
			"unterminated code block\n";
	}

	if (
		status.state != HtmlState::nothing
	) {
		out << "</body>\n</html>\n";
	}
;
;
	out.close();
;
	}
;

	if (interactive) {
		
	curInput = inputs.begin();
	curBlock = curInput != inputs.end() ?
		curInput->blocks.begin() :
		std::vector<Block>::iterator {};

	draw_block();
	for (;;) {
		
	std::string cmd;
	draw_position();
	std::cout << "> ";
	std::getline(std::cin, cmd);
	trim(cmd);
	if (cmd.empty()) { continue; }
	
	range = Range {};
	range << get_line(cmd);
	if (! cmd.empty() && cmd[0] == ',') {
		cmd.erase(0, 1);
		range << get_line(cmd);
	}
;

	if (cmd == "q" || cmd == "quit") {
		break;
	}

	if (cmd == "n" || cmd == "next") {
		if (curInput != inputs.end()) {
			int next = curBlock - curInput->blocks.begin();
			if (curBlock != curInput->blocks.end()) {
				++next;
			}
			
	if (! range) {
		next = range.last()(
			(curBlock - curInput->blocks.begin()) + 1,
			curInput->blocks.size() + 1
		) - 1;
		if (next < 0) { next = 0; }
	}
;
			curBlock = curInput->blocks.begin() + next;
			draw_block();
			continue;
		}
		std::cerr << "! end\n";
	}

	if (cmd == "p" || cmd == "prev") {
		if (curInput != inputs.end()) {
			int next = curBlock - curInput->blocks.begin();
			if (next > 0) {
				--next;
			}
			
	if (! range) {
		next = range.last()(
			(curBlock - curInput->blocks.begin()) + 1,
			curInput->blocks.size() + 1
		) - 1;
		if (next < 0) { next = 0; }
	}
;
			curBlock = curInput->blocks.begin() + next;
			draw_block();
			continue;
		}
		std::cerr << "! start\n";
	}

	if (cmd == "f" || cmd == "forward") {
		int next = curInput - inputs.begin();
		if (curInput != inputs.end()) {
			++next;
		}
		
	if (! range) {
		next = range.last()(
			(curInput - inputs.begin()) + 1,
			(inputs.end() - inputs.begin()) + 1
		) - 1;
		if (next < 0) { next = 0; }
	}
;
		curInput = inputs.begin() + next;
		curBlock = curInput != inputs.end() ?
			curInput->blocks.begin() :
			std::vector<Block>::iterator {};
		draw_block();
		continue;
	}

	if (cmd == "b" || cmd == "backward") {
		int next = curInput - inputs.begin();
		if (next) {
			--next;
		}
		
	if (! range) {
		next = range.last()(
			(curInput - inputs.begin()) + 1,
			(inputs.end() - inputs.begin()) + 1
		) - 1;
		if (next < 0) { next = 0; }
	}
;
		curInput = inputs.begin() + next;
		curBlock = curInput != inputs.end() ?
			curInput->blocks.begin() :
			std::vector<Block>::iterator {};
		draw_block();
		continue;
	}

	if (cmd == "N" || cmd == "Note") {
		if (valid_cur()) {
			insert_before(
				"n",
				curBlock->notes
			);
		}
		continue;
	}

	if (cmd == "A" || cmd == "Add") {
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
		if (valid_cur()) {
			insert_before(
				prefix,
				curBlock->value
			);
		}
		continue;
	}

	if (cmd == "W" || cmd == "Write") {
		for (const auto &cur : inputs) {
			
	std::ofstream out {
		cur.path().c_str()
	};
	bool first = true;
	for (const auto &b : cur.blocks) {
		if (first) {
			first = false;
		} else { out << '\n'; }
		switch (b.state) {
			
	case RS::header: {
		
	for (const auto &n : b.value) {
		for (
			int i = 0; i < b.level; ++i
		) {
			out << '#';
		}
		out << ' ';
		out << n << '\n';
	}
;
		break;
	}

	case RS::code: {
		
	out << "```\n";
	for (const auto &n: b.value) {
		out << n << '\n';
	}
	out << "```\n";
;
		break;
	}

	case RS::para: {
		
	bool first = true;
	for (const auto &n: b.value) {
		if (first) {
			first = false;
		} else { out << '\n'; }
		multi_write(out, n, {}, {});
	}
;
		break;
	}
;
			default: ;
		}
		
	for (const auto &n: b.notes) {
		multi_write(out, n, "* ", "  ");
	}
;
	}
;
		}
		continue;
	}

	if (cmd == "H" || cmd == "Header") {
		add_block(RS::header);
		continue;
	}

	if (cmd == "C" || cmd == "Code") {
		add_block(RS::code);
		continue;
	}

	if (cmd == "P" || cmd == "Para") {
		add_block(RS::para);
		continue;
	}
;
	}
;
	}

	}
