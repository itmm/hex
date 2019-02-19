
	
	
	#include <iostream>
	#include <exception>

	#include <vector>

	#include <sstream>

	#include <map>

	#include <fstream>
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
			std::cerr << __FILE__ << \
				':' << __LINE__ << \
				"] " << #COND << \
				" FAILED\n"; \
			throw std::exception(); \
		}

	#define ASSERT_MSG(COND, MSG) \
		if (! (COND)) { \
			std::cerr << __FILE__ << \
				':' << __LINE__ << \
				"] " << #COND << \
				" FAILED: " << MSG << \
				'\n'; \
			throw std::exception(); \
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

	Frag(
		const std::string &name
	):
		_entries {},
		_expands { 0 },
		_multiples { 0 },
		name { name }
	{
		if (isFile()) { ++_expands; }
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

	class Input {
		private:
			std::ifstream file;
			
	int _line;
;
		public:
			const std::string name;
			
	FragMap frags;
;
			
	Input(
		const std::string &name
	):
		file { name.c_str() },
		
	_line { 0 },

		name { name }
	{
	}

	bool getLine(std::string &line) {
		if (file.is_open()) {
			
	if (std::getline(file, line)) {
		
	++_line;
;
		return true;
	} else {
		file.close();
	}
;
		}
		return false;
	}

	int line() const {
		return _line;
	}
;
	};

	FragMap root;
	FragMap *frags { &root };

	class Inputs {
			
	std::unique_ptr<Input> _input;
	std::vector<std::unique_ptr<Input>>
		_pending;
	std::vector<std::unique_ptr<Input>>
		_used;
;
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
		
	if (_input && _input->name == name) {
		return true;
	}

	for (const auto &j : _pending) {
		if (j->name == name) {
			return true;
		}
	}

	for (const auto &j : _used) {
		if (j->name == name) {
			return true;
		}
	}
;
		return false;
	}
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
			str, inputs.cur()->name,
			inputs.cur()->line()
		);
	}
;
	}

	void process_char(Frag *frag, char ch) {
		
	if (frag) {
		frag->add(
			ch, inputs.cur()->name,
			inputs.cur()->line()
		);
	}
;
	}

	bool is_macro_start(
		const Frag *frag, SI begin, SI end
	) {
		
	auto n = begin + 1;
	if (n >= end) { return false; }
	if (*n != '{') { return false; }

	if (! frag) {
		
	static const std::string valids {
		"ADrR"
	};
	bool found {
		valids.find(*begin) !=
			std::string::npos
	};
	if (found && blockLimit != 0) {
		
	--blockLimit;
	return true;
;
	}
;
	}
 {
	if (frag) {
		
	static const std::string valids { 
		"fvsntkgGpmb"
	};
	bool found {
		valids.find(
			static_cast<char>(*begin)
		) != std::string::npos
	};
	if (found) {
		return true;
	}
;
	}
} ;
		return false;
	}

	SI find_macro_end(SI i, SI e) {
		
	return std::find(i, e, '}');
;
		return i;
	}

	enum class HtmlState {
		nothing,
		inSlide,
		afterSlide
		
	, inCode

	, inNotes

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
	"switch", "try", "typeof", "while"

		};
		return
			reserved.find(s) !=
				reserved.end();
	}

	bool isType(const std::string &s) {
		
	static Set reserved {
		
	"FILE", "auto", "bool", "char",
	"const", "enum", "extern", "int",
	"let", "long", "signed", "struct",
	"union", "unsigned", "void"

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
		if (w == '(') {
			span_str(out, "fn", ident);
		
	} else if (isKeyword(ident)) {
		span_str(out, "keyword", ident);
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
		auto ne = std::find(nb, end, '(');
		if (ne != end) {
			std::string name {nb, ne};
			auto ab = ne + 1;
			auto ae =
				std::find(ab, end, ')');
			if (ae != end) {
				std::string arg {ab, ae};
				
	do {
		
	static Set macros = {
		"def", "end", "add", "put", "mul",
		"Def", "Add", "Mul", "rep", "Rep"
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
		}
	}

	auto w = begin;
	
	while (w != end && (
		std::isalnum(*w) ||
			*w == '_' || *w == '$'
	)) {
		++w;
	}
;
	if (w != begin) {
		
	std::string ident {begin, w};
	begin = w - 1;
	if (w != end && *w == '{') {
		
	auto q = std::find(w + 1, end, '}');
	if (q == end) {
		writeEscaped(out, ident);
		writeOneEscaped(out, '{');
		begin = w;
	} else {
		std::string name {w + 1, q};
		
	if (ident == "v") {
		writeMacroClass(out, "var");
		writeEscaped(out, name);
		out << "</span>";
	}

	else if (ident == "f") {
		writeMacroClass(out, "fn");
		writeEscaped(out, name);
		out << "</span>";
	}

	else if (ident == "k") {
		writeMacroClass(out, "keyword");
		writeEscaped(out, name);
		out << "</span>";
	}

	else if (ident == "s") {
		writeMacroClass(out, "str");
		writeEscaped(out, name);
		out << "</span>";
	}

	else if (ident == "n") {
		writeMacroClass(out, "num");
		writeEscaped(out, name);
		out << "</span>";
	}

	else if (ident == "p") {
		writeMacroClass(out, "var");
		out << "@priv(<span>";
		writeEscaped(out, name);
		out << "</span>)</span>";
	}

	else if (ident == "m") {
		writeMacroClass(out, "var");
		out << "@magic(<span>";
		writeEscaped(out, name);
		out << "</span>)</span>";
	}

	else if (ident == "b") {
		writeMacroClass(out, "virt");
		out << "</span><br/>";
	}

	else {
		process_ident(out, ident, '{');
		writeOneEscaped(out, '{');
		q = w + 1;
	}
;
		begin = q;
	}
;
	} else {
		process_ident(
			out, ident,
			w != end ? *w : ' '
		);
	}
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
	out << '\n';
;
	}

	int main(
		int argc,
		const char **argv
	) {
		
	
	
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
		
	if (is_macro_start(frag, i, end)) {
		auto j = find_macro_end(i, end);
		if (j != end) {
			do {
				
	char openCh {*i};
	i += 2;
	std::string name {i, j};

	if (openCh == 'D') {
		ASSERT_MSG(! frag, "def in frag");
		FragMap *fm { frags };
		
	frag = fm->find(name);
	if (isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			name <<
			"] already defined\n";
	}
;
		if (! frag) {
			frag = &root[name];
		}
		break;
	}

	if (openCh == 'A') {
		ASSERT_MSG(! frag, "add in frag");
		FragMap *fm { frags };
		FragMap *ins { &root };
		frag = fm->find(name);
		
	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			name <<
			"] not defined\n";
		frag = &fm->get(
			name, *ins
		);
	}
;
		break;
	}

	if (openCh == 'r') {
		ASSERT_MSG(! frag,
			"replace in frag"
		);
		frag = &(inputs.cur()->frags[
			name
		]);
		
	ASSERT_MSG(frag, "frag " <<
		name <<
		" not defined"
	);
	frag->clear();
;
		break;
	}

	if (openCh == 'R') {
		ASSERT_MSG(! frag,
			"replace in frag"
		);
		frag = &frags->get(
			name, root
		);
		
	ASSERT_MSG(frag, "frag " <<
		name <<
		" not defined"
	);
	frag->clear();
;
		break;
	}

	if (openCh == 'g') {
		ASSERT_MSG(frag,
			"globexpand not in frag"
		);
		Frag &sub = frags->get(
			name, root
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

	if (openCh == 'G') {
		ASSERT_MSG(frag,
			"globmult not in frag"
		);
		Frag &sub { frags->get(
			name, root
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

	if (openCh == 'p') {
		ASSERT_MSG(frag,
			"private not in frag"
		);
		
	std::hash<std::string> h;
	unsigned cur {
		h(inputs.cur()->name +
			':' + name) &
				0x7fffffff
	};

	std::ostringstream hashed;
	hashed << "_private_" <<
		cur << '_' <<
		name;
	frag->add(
		hashed.str(),
		inputs.cur()->name,
		inputs.cur()->line()
	);
;
		break;
	}

	if (openCh == 'm') {
		ASSERT_MSG(frag,
			"magic not in frag"
		);
		
	std::hash<std::string> h;
	unsigned cur {
		h(inputs.cur()->name +
			':' + name) &
				0x7fffffff
	};

	std::ostringstream value;
	value << cur;
	frag->add(
		value.str(),
		inputs.cur()->name,
		inputs.cur()->line()
	);
;
		break;
	}

	ASSERT_MSG(frag,
		"must be in frag " << openCh <<
		 	'{' << name << '}'
	);
	frag->add(
		name,
		inputs.cur()->name,
		inputs.cur()->line()
	);
;
			} while (false); 
			i += (j - i);
			continue;
		}
	}

	if (*i == '@') {
		auto nb = i + 1;
		auto ne = nb;
		while (ne != end && *ne != '(') {
			++ne;
		}
		if (ne != end && ne != nb) {
			std::string name {nb, ne};
			
	auto ab = ne + 1;
	auto ae = ab;
	while (ae != end && *ae != ')') {
		++ae;
	}
	if (ae != end) {
		std::string arg {ab, ae};
		
	i = ae;
	do {
		
	if (name == "def") {
		ASSERT_MSG(! frag, "def in frag");
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

	if (name == "end") {
		ASSERT_MSG(frag,
			"end not in frag"
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
		ASSERT_MSG(! frag, "add in frag");
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
;
		
	ASSERT_MSG(frag,
		"must be in frag @" << name <<
		 	'(' << arg << ')'
	);
	frag->add(
		arg,
		inputs.cur()->name,
		inputs.cur()->line()
	);
;
	} while (false);
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
;
	
	for (auto &cur : inputs) {
		
	const std::string &name { cur->name };
	std::string outPath {
		name.substr(0, name.size() - 2) +
		".html"
	};
	std::ofstream out { outPath.c_str() };
	
	std::ifstream in {
		cur->name.c_str()
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
;
		switch (status.state) {
			case HtmlState::afterSlide:
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
	writeEscaped(out, name);
	out << "</h" << level << ">\n";
;
	out << "<div class=\"slides\">\n";
	out << "<div><div>\n";
	
	out << "<h" << level << '>';
	writeEscaped(out, name);
	out << "</h" << level << ">\n";
;
	out << "</div>\n";
;
		status.state = HtmlState::inSlide;
		continue;
	}

	if (line == "```") {
		
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
;
	} else {
		process_content(
			out, line.begin(), line.end()
		);
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
	in.close();
;
	out.close();
;
	}
;

	}
