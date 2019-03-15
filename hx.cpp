
	
	
	#include <fstream>

	#include <string>

	#include <iostream>
	#include <exception>

	#include <vector>

	#include <sstream>

	#include <map>

	#include <iostream>
	#include <memory>
	#include <vector>

	#include <algorithm>

	#include <functional>
	#include <sstream>

	#include <string>

	#include <cctype>

	#include <set>
;

	
	
	
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

	class FragMap {
		FragMap *_link;
		using Map =
			std::map<std::string, Frag>;
		Map map;
	public:
		
	FragMap(): _link { nullptr } {}

	FragMap *setLink(FragMap *link) {
		FragMap *old { _link };
		_link = link;
		return old;
	}

	Frag *find(const std::string &name) {
		auto found { map.find(name) };
		if (found != map.end()) {
			return &found->second;
		}
		if (_link) {
			return _link->find(name);
		}
		return nullptr;
	}

	Frag &get(
		const std::string &name,
		FragMap &insert
	) {
		Frag *found { find(name) };
		if (found) { return *found; }
		auto created { insert.map.insert(
			Map::value_type { name, name }
		) };
		return created.first->second;
	}

	Frag &operator[](
		const std::string &name
	) {
		return get(name, *this);
	}

	auto begin() const {
		return map.cbegin();
	}

	auto end() const {
		return map.cend();
	}
;
	};
;

	FragMap root;
	FragMap *frags { &root };
;

	class Input {
		public:
			const std::string path;
			
	Input(const std::string &path):
		path { path },
		_file { path.c_str() }
		,
	_line { 0 }

	{}

	bool getLine(std::string &line) {
		if (_file.is_open()) {
			
	if (std::getline(_file, line)) {
		
	++_line;
;
		return true;
	} else {
		_file.close();
	}
;
		}
		return false;
	}

	FragMap frags;

	int line() const {
		return _line;
	}
;
		private:
			std::ifstream _file;
			
	int _line;
;
	};
;

	class Inputs {
		public:
			
	auto &cur() {
		return _input;
	}

	auto begin() const {
		return _used.cbegin();
	}

	auto end() const {
		return _used.cend();
	}

	void push(const std::string &path) {
		std::unique_ptr<Input> i {
			std::make_unique<Input>(path)
		};
		
	if (_input) {
		_input->frags.setLink(frags);
		frags = &_input->frags;
	}
;
		
	if (_input) {
		_pending.push_back(
			std::move(_input)
		);
	}
;
		_input = std::move(i);
	}

	bool getLine(std::string &line) {
		while (_input) {
			if (_input->getLine(line)) {
				return true;
			}
			
	_used.push_back(std::move(_input));
	if (! _pending.empty()) {
		_input =
			std::move(_pending.back());
		_pending.pop_back();
	}
	frags = frags->setLink(nullptr);
;
		}
		return false;
	}

	bool has(
		const std::string &name
	) const {
		
	if (_input && _input->path == name) {
		return true;
	}

	for (const auto &j : _pending) {
		if (j->path == name) {
			return true;
		}
	}

	for (const auto &j : _used) {
		if (j->path == name) {
			return true;
		}
	}
;
		return false;
	}
;
		private:
			
	std::unique_ptr<Input> _input;
	std::vector<std::unique_ptr<Input>>
		_pending;
	std::vector<std::unique_ptr<Input>>
		_used;
;
	};

	std::string stylesheet {
		"slides/slides.css"
	};

	int blockLimit = -1;

	Inputs inputs;

	using SI =
		std::string::const_iterator;

	void process_chars(
		Frag *frag, SI i, SI e
	) {
		
	if (frag) {
		std::string str {i, e};
		frag->add(
			str, inputs.cur()->path,
			inputs.cur()->line()
		);
	}
;
	}

	void process_char(Frag *frag, char ch) {
		
	if (frag) {
		frag->add(
			ch, inputs.cur()->path,
			inputs.cur()->line()
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
		inputs.cur()->path,
		inputs.cur()->line()
	);
;
			b = x + 1;
			if (b != e) {
				f->add(
					*b,
					inputs.cur()->path,
					inputs.cur()->line()
				);
				++b;
			}
		} else {
			
	f->add(
		std::string { b, e },
		inputs.cur()->path,
		inputs.cur()->line()
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
	{ }

	bool in_code(
		HtmlStatus *s
	) {
		
	if (s->state == HtmlState::inCode) {
		return true;
	}
;
		return false;
	}
 
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
	"function"

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
	"string", "std", "ifstream", "istream",
	"ofstream", "ostream"

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
;
	#endif

	
	bool someFile { false };
	for (int i { 1 }; i < argc; ++i) {
		 {
	static const std::string prefix {
		"--css="
	};
	std::string arg { argv[i] };
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
	std::string arg { argv[i] };
	if (arg.substr(
		0, prefix.length()
	) == prefix) {
		
	std::istringstream iss {
		arg.substr(prefix.length())
	};
	iss >> blockLimit;
	continue;
;
	}
} 
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
	while (inputs.getLine(line)) {
		
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
		ASSERT_MSG(! frag, "def(" << arg << ") in frag");
		FragMap *fm {
			&inputs.cur()->frags
		};
		
	frag = fm->find(arg);
	if (isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			arg <<
			"] already defined\n";
	}
;
		if (! frag) {
			frag = &(*fm)[arg];
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
		FragMap *fm {
			&inputs.cur()->frags
		};
		FragMap *ins { fm };
		frag = fm->find(arg);
		
	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			arg <<
			"] not defined\n";
		frag = &fm->get(
			arg, *ins
		);
	}
;
		break;
	}

	if (name == "put") {
		ASSERT_MSG(frag,
			"expand not in frag"
		);
		Frag &sub = inputs.cur()->frags[
			arg
		];
		
	if (sub.expands()) {
		std::cerr <<
			"multiple expands of [" <<
			sub.name << "]\n";
	}
	if (sub.multiples()) {
		std::cerr <<
			"expand after mult of ["
			<< sub.name << "]\n";
	}
;
		sub.addExpand();
		frag->add(&sub);
		break;
	}

	if (name == "inc") {
		ASSERT_MSG(! frag,
			"include in frag"
		);
		if (! inputs.has(arg)) {
			inputs.push(arg);
		}
		break;
	}

	if (name == "mul") {
		ASSERT_MSG(frag,
			"mul not in frag"
		);
		Frag &sub { inputs.cur()->frags[
			arg
		] };
		
	if (sub.expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub.name << "]\n";
	}
;
		sub.addMultiple();
		frag->add(&sub);
		break;
	}

	if (name == "Def") {
		ASSERT_MSG(! frag, "Def in frag");
		FragMap *fm { frags };
		
	frag = fm->find(arg);
	if (isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			arg <<
			"] already defined\n";
	}
;
		if (! frag) {
			frag = &root[arg];
		}
		break;
	}

	if (name == "Add") {
		ASSERT_MSG(! frag, "Add in frag");
		FragMap *fm { frags };
		FragMap *ins { &root };
		frag = fm->find(arg);
		
	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			arg <<
			"] not defined\n";
		frag = &fm->get(
			arg, *ins
		);
	}
;
		break;
	}

	if (name == "rep") {
		ASSERT_MSG(! frag,
			"rep in frag"
		);
		frag = &(inputs.cur()->frags[
			arg
		]);
		
	ASSERT_MSG(frag, "frag " <<
		name <<
		" not defined"
	);
	frag->clear();
;
		break;
	}

	if (name == "Rep") {
		ASSERT_MSG(! frag,
			"Rep in frag"
		);
		FragMap *fm { frags };
		frag = fm->find(arg);
		
	ASSERT_MSG(frag, "frag " <<
		name <<
		" not defined"
	);
	frag->clear();
;
		break;
	}

	if (name == "rep") {
		ASSERT_MSG(! frag,
			"replace in frag"
		);
		frag = &frags->get(
			arg, root
		);
		
	ASSERT_MSG(frag, "frag " <<
		name <<
		" not defined"
	);
	frag->clear();
;
		break;
	}

	if (name == "Put") {
		ASSERT_MSG(frag,
			"Put not in frag"
		);
		Frag &sub = frags->get(
			arg, root
		);
		
	if (sub.expands()) {
		std::cerr <<
			"multiple expands of [" <<
			sub.name << "]\n";
	}
	if (sub.multiples()) {
		std::cerr <<
			"expand after mult of ["
			<< sub.name << "]\n";
	}
;
		sub.addExpand();
		frag->add(&sub);
		break;
	}

	if (name == "Mul") {
		ASSERT_MSG(frag,
			"globmult not in frag"
		);
		Frag &sub { frags->get(
			arg, root
		) };
		
	if (sub.expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub.name << "]\n";
	}
;
		sub.addMultiple();
		frag->add(&sub);
		break;
	}

	if (name == "priv") {
		ASSERT_MSG(frag,
			"@priv not in frag"
		);
		
	std::hash<std::string> h;
	unsigned cur {
		h(inputs.cur()->path +
			':' + arg) &
				0x7fffffff
	};

	std::ostringstream hashed;
	hashed << "_private_" <<
		cur << '_' <<
		name;
	frag->add(
		hashed.str(),
		inputs.cur()->path,
		inputs.cur()->line()
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
		h(inputs.cur()->path +
			':' + arg) &
				0x7fffffff
	};

	std::ostringstream value;
	value << cur;
	frag->add(
		value.str(),
		inputs.cur()->path,
		inputs.cur()->line()
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
	}
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
		for (auto &i : j->frags) {
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
		for (auto &i : j->frags) {
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
		
	const std::string &name { cur->path };
	std::string outPath {
		name.substr(0, name.size() - 2) +
		".html"
	};
	std::ofstream out { outPath.c_str() };
	
	std::ifstream in {
		cur->path.c_str()
	};
	
	HtmlStatus status;
	std::string ident;
	std::string line;
	while (std::getline(in, line)) {
		
	if (in_code(&status)) {
		
	if (line == "```") {
		
	out << "</code></div>\n";
	status.state = HtmlState::inSlide;
;
		continue;
	}

	process_code(
		out, line.begin(), line.end()
	);
	out << "<br/>\n";
;
		continue;
	}

	if (line == "") {
		
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
		switch (status.state) {
			case HtmlState::afterSlide:
			case HtmlState::afterSlides:
			case HtmlState::nothing:
				break;
			default:
				
	out << "</div>\n";
	status.state = HtmlState::afterSlide;
;
		}
		continue;
	}

	if (line[0] == '#') {
		
	int level = 1;
	while (
		level < (int) line.size() &&
			line[level] == '#'
	) {
		++level;
	}

	auto e = line.end();
	auto b = line.begin() + level;
	while (b != e && *b <= ' ') {
		++b;
	}

	ASSERT(b != e);
	std::string name {b, e};
	
	switch (status.state) {
		case HtmlState::nothing: {
			
	out << "<!doctype html>\n";
	out << "<html lang=\"de\">\n";
	out << "<head>\n";
	
	out << "<meta charset=\"utf-8\">\n";
	out << "<title>";
	writeEscaped(out, name);
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
	
	out << "<h" << level << '>';
	process_content(out, name.begin(), name.end());
	out << "</h" << level << ">\n";
;
	out << "<div class=\"slides\">\n";
	out << "<div><div>\n";
	
	out << "<h" << level << '>';
	process_content(out, name.begin(), name.end());
	out << "</h" << level << ">\n";
;
	out << "</div>\n";
;
		status.state = HtmlState::inSlide;
		continue;
	}

	if (line == "```") {
		
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
		continue;
	}

	if (
		line[0] == '*' ||
		status.state == HtmlState::inNotes
	) {
		
	if (line[0] == '*') {
		
	auto end = line.end();
	auto begin = line.begin() + 1;
	while (
		begin != end && *begin == ' '
	) {
		++begin;
	}

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
	} else {
		process_content(
			out, line.begin(), line.end()
		);
		out << '\n';
	}
;
		continue;
	}

	
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
		out, line.begin(), line.end()
	);
	out << '\n';
;
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
	in.close();
;
	out.close();
;
	}
;

	}
