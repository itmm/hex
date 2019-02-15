
	
	
	#include <iostream>
	#include <exception>

	#include <list>
	#include <map>
	#include <sstream>

	#include <fstream>
	#include <iostream>
	#include <memory>
	#include <vector>

	#include <functional>
	#include <sstream>

	#include <set>
	#include <string>

	#include <cctype>
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

	
	class Buf {
		std::string _value;
		std::string _file;
		int _startLine;
		int _endLine;
		bool _active;
		
	bool _canContinue(
		const std::string &file,
		int line
	) {
		if (_file != file) {
			return false;
		}
		return 
			line == _endLine ||
			line == _endLine + 1;
	}

	void assertCont(
		const std::string &file,
		int line
	) {
		if (_file.empty()) {
			_file = file;
			_startLine = line;
			_endLine = line;
		}
		ASSERT(_canContinue(file, line));
	}
;
	public:
		
	Buf(): _active(false) {}

	bool active() const {
		return _active;
	}

	bool empty() const {
		return _value.empty();
	}

	void activate() {
		_active = true;
	}

	void clear(bool active = false) {
		_value.clear();
		_file.clear();
		_startLine = 0;
		_endLine = 0;
		_active = active;
	}

	const std::string &str() const {
		return _value;
	}

	const std::string &file() const {
		return _file;
	}

	int startLine() const {
		return _startLine;
	}

	int endLine() const {
		return _endLine;
	}

	bool canContinue(
		const std::string &file,
		int line
	) {
		if (_file.empty()) {
			return true;
		}
		return _canContinue(
			file, line
		);
	}

	void add(
		const std::string &value,
		const std::string &file =
			"/dev/null",
		int line = 1
	) {
		activate();
		if (value.empty()) { return; }
		assertCont(file, line);
		_value += value;
		_endLine = line;
	}

	void add(
		int ch,
		const std::string &file =
			"/dev/null",
		int line = 1
	) {
		char c = static_cast<char>(ch);
		add(
			std::string(&c, &c + 1),
			file, line
		);
	}

	void add(const Buf &b) {
		activate();
		if (b.empty()) { return; }
		assertCont(
			b._file, b._startLine
		);
		_value += b._value;
		_endLine = b._endLine;
	}
;
	};
;

	
	class Frag;

	class FragEntry {
		Buf _buf;
	public:
		const Frag *frag;
		
	FragEntry(
		Frag *frag
	):
		frag { frag }
	{}

	FragEntry(
		const std::string &value,
		const std::string &file = "/dev/null",
		int line = 1
	): 
		frag { nullptr}
	{
		_buf.add(value, file, line);
	}

	FragEntry(): 
		frag { nullptr}
	{}

	const std::string &str() const {
		return _buf.str();
	}

	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		_buf.add(value, file, line);
	}

	void add(
		const Buf &b
	) {
		_buf.add(b);
	}

	bool canAdd(
		const std::string &file,
		int line
	) {
		return _buf.canContinue(file, line);
	}
;
	};

	class Frag {
		std::list<FragEntry> _entries;
		int _expands;
		int _multiples;
	public:
		const std::string name;
		
	bool isFile() const {
		static const std::string prefix { "file: " };
		return name.substr(0, prefix.size()) == prefix;
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

	Frag &add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		if (value.empty()) { return *this; }
		if (_entries.empty()) {
			_entries.push_back(FragEntry {
				value, file, line
			});
		} else if (! _entries.back().canAdd(
			file, line
		)) {
			_entries.push_back(FragEntry {
				value, file, line
			});
		} else {
			_entries.back().add(
				value, file, line
			);
		}
		return *this;
	}

	void add(const Buf &b) {
		if (b.empty()) { return; }
		if (_entries.empty()) {
			_entries.push_back(FragEntry {});
		} else if (! _entries.back().canAdd(
			b.file(), b.startLine()
		)) {
			_entries.push_back(FragEntry {});
		}
		_entries.back().add(b);
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
		const Frag &frag, std::ostream &out,
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
		std::map<std::string, Frag> map;
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

	Frag &get(const std::string &name, FragMap &insert) {
		Frag *found { find(name) };
		if (found) { return *found; }
		auto created { insert.map.insert(
			std::pair<std::string, Frag>(name, name)
		) };
		return created.first->second;
	}

	Frag &operator[](const std::string &name) {
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
		_input = std::move(_pending.back());
		_pending.pop_back();
	}
	frags = frags->setLink(nullptr);
;
		}
		return false;
	}

	bool has(const std::string &name) const {
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
		return false;
	}
;
	};

	std::string stylesheet {
		"slides/slides.css"
	};

	int blockLimit = -1;

	Inputs inputs;

	void process_char(Frag *frag, char ch) {
		if (frag) {
			Buf buffer;
			buffer.add(
				ch,
				inputs.cur()->name,
				inputs.cur()->line()
			);
			frag->add(buffer);
		}
	}

	using SI =
		std::string::const_iterator;

	void process_chars(Frag *frag, SI i, SI e) {
		if (frag) {
			Buf buffer;
			buffer.add(std::string {i, e}, inputs.cur()->name, inputs.cur()->line());
			frag->add(buffer);
		}
	}

	bool is_macro_start(const Frag *frag, SI i, SI e) {
		auto n = i + 1;
		if (n >= e) { return false; }
		if (*n != '{') { return false; }
		
	if (! frag) {
		
	static const std::string valids {
		"aAdDirR"
	};
	bool found {
		valids.find(*i) !=
			std::string::npos
	};
	if (found && blockLimit > 0) {
		
	--blockLimit;
	return true;
;
	}
;
	}
 {
	if (frag) {
		
	static const std::string valids { 
		"fvsntkxeEgGpmb"
	};
	bool found {
		valids.find(
			static_cast<char>(*i)
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

	std::string::const_iterator find_macro_end(SI i, SI e) {
		while (i != e && *i != '}') {
			++i;
		}
		return i;
	}

	void process_macro(Frag *&frag, SI i, SI e) {
		char openCh{*i};
		i += 2;
		std::string name {i, e};
		
	if (openCh == 'd') {
		ASSERT_MSG(! frag, "def in frag");
		FragMap *fm {
			&inputs.cur()->frags
		};
		
	frag = fm->find(name);
	if (isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			name <<
			"] already defined\n";
	}
;
		if (! frag) {
			frag = &(*fm)[name];
		}
		return;
	}

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
		return;
	}

	if (openCh == 'a') {
		ASSERT_MSG(! frag, "add in frag");
		FragMap *fm {
			&inputs.cur()->frags
		};
		FragMap *ins { fm };
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
		return;
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
		return;
	}

	if (openCh == 'r') {
		ASSERT_MSG(! frag,
			"replace in frag"
		);
		frag = &(inputs.cur()->frags[
			name
		]);
		ASSERT_MSG(frag, "frag " <<
			name << " not defined"
		);
		frag->clear();
		return;
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
		return;
	}

	if (openCh == 'x') {
		ASSERT_MSG(frag,
			"end not in frag"
		);
		
	ASSERT_MSG(frag->name == name,
		"closing [" << name <<
		"] != [" << frag->name << ']'
	);
;
		frag = nullptr;
		return;
	}

	if (openCh == 'i') {
		ASSERT_MSG(! frag,
			"include in frag"
		);
		if (! inputs.has(name)) {
			inputs.push(name);
		}
		return;
	}

	if (openCh == 'e') {
		ASSERT_MSG(frag,
			"expand not in frag"
		);
		Frag &sub = inputs.cur()->frags[
			name
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
		return;
	}

	if (openCh == 'g') {
		ASSERT_MSG(frag,
			"expand not in frag"
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
		return;
	}

	if (openCh == 'E') {
		ASSERT_MSG(frag,
			"multiple not in frag"
		);
		Frag &sub { inputs.cur()->frags[
			name
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
		return;
	}

	if (openCh == 'G') {
		ASSERT_MSG(frag,
			"multiple not in frag"
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
		return;
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
		return;
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
		return;
	}

	ASSERT_MSG(frag,
		"unknown frag " << name
	);
	process_chars(frag, name.begin(), name.end());
	return;
;
	}

	enum class HtmlState {
		nothingWritten,
		inSlide,
		afterSlide
		
	, inHeader

	, inCode

	, inNotes

	};

	struct HtmlStatus {
		
	HtmlStatus();
	HtmlState state;

	int headerLevel;
	std::string headerName;
	HtmlState headerState;

	int codeOpening;

	int codeIndent;

	char codeSpecial;
	Buf name;

	bool noteInCode;
	bool noteInBold;

	};

	inline HtmlStatus::HtmlStatus():
		state {
			HtmlState::nothingWritten
		}
		
	, headerLevel { 0 }
	, headerName {}

	, codeOpening { 0 }
	, codeIndent { 0 }
	, codeSpecial { '\0' }
	, name {}

	, noteInCode { false }
	, noteInBold { false }

	{ }

	bool isOutOfHtmlSpecial(
		HtmlStatus *s
	) {
		
	if (
		s->state ==
			HtmlState::inHeader
	) {
		return false;
	}

	if (s->state == HtmlState::inCode) {
		return false;
	}

	if (s->state == HtmlState::inNotes) {
		return false;
	}
;
		return true;
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

	void escapeIdent(
		std::ostream &out,
		const char *cls,
		const std::string &s
	) {
		out << "<span class=\"" <<
			cls << "\">" << s <<
			"</span>";
	}

	bool isKeyword(const std::string &s) {
		static std::set<std::string>
			reserved {
				
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
		
	static std::set<std::string>
		reserved {
			
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
		static std::set<std::string> reserved {
			"EOF", "NULL", "nullptr",
			"false", "null", "true", "undefined"
		};
		if (std::isdigit(s[0])) { return true; }
		return reserved.find(s) != reserved.end();
	}

	void writeMacroClass(
		std::ostream &out,
		HtmlStatus &status,
		const char *name,
		char special
	) {
		out << "<span class=\"" << name << "\">";
		status.codeSpecial = special;
		status.name.clear(true);
	}

	void writeMacroHeader(
		std::ostream &out,
		HtmlStatus &status,
		const char *name,
		char special
	) {
		writeMacroClass(out, status, "macro", special);
		out << '@' << name << "(<span class=\"name\">";
	}

	int main(
		int argc,
		const char **argv
	) {
		
	
	 {
	Buf b;
	ASSERT(! b.active());
}  {
	Buf b;
	b.activate();
	ASSERT(b.active());
}  {
	Buf b;
	b.activate();
	b.clear();
	ASSERT(! b.active());
}  {
	
	Buf b;
	b.add("abc", "foo.x", 1);
;
	ASSERT(b.active());
}  {
	
	Buf b;
	b.add("abc", "foo.x", 1);
;
	ASSERT(b.str() == "abc");
}  {
	
	Buf b;
	b.add("abc", "foo.x", 1);
;
	ASSERT(b.file() == "foo.x");
}  {
	
	Buf b;
	b.add("abc", "foo.x", 1);
;
	ASSERT(b.startLine() == 1);
}  {
	
	Buf b;
	b.add("abc", "foo.x", 1);
;
	ASSERT(b.endLine() == 1);
}  {
	
	
	Buf b;
	b.add("abc", "foo.x", 1);
;
	b.add("\n", "foo.x", 1);
	b.add("def", "foo.x", 2);
;
	ASSERT(b.str() == "abc\ndef");
}  {
	
	
	Buf b;
	b.add("abc", "foo.x", 1);
;
	b.add("\n", "foo.x", 1);
	b.add("def", "foo.x", 2);
;
	ASSERT(b.startLine() == 1);
}  {
	
	
	Buf b;
	b.add("abc", "foo.x", 1);
;
	b.add("\n", "foo.x", 1);
	b.add("def", "foo.x", 2);
;
	ASSERT(b.endLine() == 2);
}  {
	
	
	
	Buf b;
	b.add("abc", "foo.x", 1);
;
	b.add("\n", "foo.x", 1);
	b.add("def", "foo.x", 2);
;
	Buf c;
	c.add(b);
;
	ASSERT(c.str() == b.str());
}  {
	
	
	
	Buf b;
	b.add("abc", "foo.x", 1);
;
	b.add("\n", "foo.x", 1);
	b.add("def", "foo.x", 2);
;
	Buf c;
	c.add(b);
;
	ASSERT(
		c.startLine() ==
			b.startLine()
	);
}  {
	
	
	
	Buf b;
	b.add("abc", "foo.x", 1);
;
	b.add("\n", "foo.x", 1);
	b.add("def", "foo.x", 2);
;
	Buf c;
	c.add(b);
;
	ASSERT(
		c.endLine() ==
			b.endLine()
	);
}  {
	
	
	
	
	Buf b;
	b.add("abc", "foo.x", 1);
;
	b.add("\n", "foo.x", 1);
	b.add("def", "foo.x", 2);
;
	Buf c;
	c.add(b);
;
	Buf d;
	d.add("\n", "foo.x", 2);
	d.add("ghi", "foo.x", 3);
	c.add(d);

	ASSERT(
		c.str() ==
			"abc\ndef\nghi"
	);
}  {
	
	
	
	
	Buf b;
	b.add("abc", "foo.x", 1);
;
	b.add("\n", "foo.x", 1);
	b.add("def", "foo.x", 2);
;
	Buf c;
	c.add(b);
;
	Buf d;
	d.add("\n", "foo.x", 2);
	d.add("ghi", "foo.x", 3);
	c.add(d);

	ASSERT(c.startLine() == 1);
}  {
	
	
	
	
	Buf b;
	b.add("abc", "foo.x", 1);
;
	b.add("\n", "foo.x", 1);
	b.add("def", "foo.x", 2);
;
	Buf c;
	c.add(b);
;
	Buf d;
	d.add("\n", "foo.x", 2);
	d.add("ghi", "foo.x", 3);
	c.add(d);

	ASSERT(c.endLine() == 3);
} ;

	
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
		FragEntry entry { "abc" };
		ASSERT(entry.str() == "abc");
	}
 {
	Frag frag {""};
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
		auto end = line.end();
		for (auto i = line.begin(); i != end; ++i) {
			if (is_macro_start(frag, i, end)) {
				auto j = find_macro_end(i, end);
				if (j != end) {
					process_chars(frag, line.begin(), i);
					process_macro(frag, i, j);
					process_chars(frag, j, end);
					break;
				}
			} else {
				process_chars(frag, i, i + 1);
			}
		}
		process_char(frag, '\n');
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
	 {
	HtmlStatus status;
	bool newline { true };
	std::string ident;
	for (;;) {
		int ch { in.get() };
		 
	if (ch == '#' && newline) {
		if (
			isOutOfHtmlSpecial(&status) ||
				status.state ==
					HtmlState::inHeader
		) {
			
	++status.headerLevel;
	if (
		status.state !=
			HtmlState::inHeader
	) {
		status.headerState =
			status.state;
	}
	status.state =
		HtmlState::inHeader;
;
			continue;
		}
	}
 
	if (
		status.state ==
			HtmlState::inHeader
	) {
		if (ch == '\n') {
			 
	ASSERT(! status.headerName.empty());
	 
	switch (status.headerState) {
		case HtmlState::nothingWritten: {
			 
	out << "<!doctype html>\n";
	out << "<html lang=\"de\">\n";
	out << "<head>\n";
	 
	out << "<meta charset=\"utf-8\">\n";
	out << "<title>";
	writeEscaped(
		out, status.headerName
	);
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
		}
		default: {
			out << "</div>\n";
		}
	}
;
	 
	out << "<h" <<
		status.headerLevel << '>';
	writeEscaped(
		out, status.headerName
	);
	out << "</h" <<
		status.headerLevel <<
		">\n";
;
	out << "<div class=\"slides\">\n";
	out << "<div><div>\n";
	 
	out << "<h" <<
		status.headerLevel << '>';
	writeEscaped(
		out, status.headerName
	);
	out << "</h" <<
		status.headerLevel <<
		">\n";
;
	out << "</div>\n";
;
			 
	status.state =
		HtmlState::inSlide;
	status.headerLevel = 0;
	status.headerName.clear();
	status.headerState =
		HtmlState::inSlide;
;
			
	newline = ch == '\n';
	if (
		status.state !=
			HtmlState::inHeader
	) {
		writeOneEscaped(out, ch);
	}
;
			continue;
		}
	}
 
	if (
		status.state ==
			HtmlState::inHeader
	) {
		auto &hn { status.headerName };
		if (! hn.empty()) {
			hn.push_back(ch);
			
	newline = ch == '\n';
	if (
		status.state !=
			HtmlState::inHeader
	) {
		writeOneEscaped(out, ch);
	}
;
			continue;
		}
	}
 
	if (
		status.state ==
			HtmlState::inHeader
	) {
		auto &hn { status.headerName };
		if (ch > ' ' && hn.empty()) {
			hn.push_back(ch);
			
	newline = ch == '\n';
	if (
		status.state !=
			HtmlState::inHeader
	) {
		writeOneEscaped(out, ch);
	}
;
			continue;
		}
	}
 
	if (newline && ch == '`') {
		if (isOutOfHtmlSpecial(&status) ||
			status.state ==
				HtmlState::inCode
		) {
			++status.codeOpening;
			continue;
		}
	}
 
	if (
		ch == '\n' &&
		status.codeOpening == 3
	) {
		status.codeOpening = 0;
		
	if (isOutOfHtmlSpecial(&status)) {
		
	if (
		status.state == HtmlState::inSlide
	) {
		out << "</div>\n";
	}
	out << "<div><div>\n";
	out << "<code>\n";
	status.state = HtmlState::inCode;
	
	newline = ch == '\n';
	if (
		status.state !=
			HtmlState::inHeader
	) {
		writeOneEscaped(out, ch);
	}
;
;
		continue;
	} else if (status.state ==
		HtmlState::inCode
	) {
		
	out << "</code>\n";
	out << "</div>\n";
	status.state = HtmlState::inSlide;
	status.codeIndent = 0;
	status.codeSpecial = '\0';
	
	newline = ch == '\n';
	if (
		status.state !=
			HtmlState::inHeader
	) {
		writeOneEscaped(out, ch);
	}
;
;
		continue;
	}
;
	}

	if (status.codeOpening == 1) {
		
	const auto &s { status.name.str() };
	if (! status.codeSpecial &&
		status.state == HtmlState::inCode
	) {
		
	status.codeSpecial = '`';
	status.name.clear(true);
	
	if (status.codeIndent) {
		out << "<span class=\"in"
			<< status.codeIndent
			<< "\"></span>";
		status.codeIndent = 0;
	}
	if (! ident.empty()) {
		if (ch == '(') {
			escapeIdent(out, "fn", ident);
		} else if (isKeyword(ident)) {
			escapeIdent(out, "keyword", ident);
		} else if (isType(ident)) {
			escapeIdent(out, "type", ident);
		} else if (isNum(ident)) {
			escapeIdent(out, "num", ident);
		} else {
			escapeIdent(out, "var", ident);
		}
		ident.clear();
	}
;
	
	if (status.codeIndent) {
		out << "<span class=\"in"
			<< status.codeIndent
			<< "\"></span>";
		status.codeIndent = 0;
	}
;
	out << "<span class=\"str\">`";
;
	} else if (
		status.codeSpecial == '`' && (
			s.empty() ||
			s.back() != '\x5c'
		)
	) { 
	
	if (status.codeIndent) {
		out << "<span class=\"in"
			<< status.codeIndent
			<< "\"></span>";
		status.codeIndent = 0;
	}
	if (! ident.empty()) {
		if (ch == '(') {
			escapeIdent(out, "fn", ident);
		} else if (isKeyword(ident)) {
			escapeIdent(out, "keyword", ident);
		} else if (isType(ident)) {
			escapeIdent(out, "type", ident);
		} else if (isNum(ident)) {
			escapeIdent(out, "num", ident);
		} else {
			escapeIdent(out, "var", ident);
		}
		ident.clear();
	}
;
	writeEscaped(
		out, status.name.str()
	);
	out << "`</span>";
	status.codeSpecial = 0;
	status.name.clear();
; }
;
	}

	status.codeOpening = 0;

	if (status.state == HtmlState::inCode) {
		if (ch == EOF) {
			std::cerr <<
				"unterminated code " << 
				"block\n";
			break;
		}
	}

	if (
		status.state == HtmlState::inCode
	) {
		
	if (
		! status.codeSpecial && (
			std::isalnum(ch) || ch == '_'
		)
	) {
		ident.push_back(ch);
		continue;
	}
;
	}

	if (status.state == HtmlState::inCode) {
		 
	if (ch == '\n') {
		
	if (status.codeIndent) {
		out << "<span class=\"in"
			<< status.codeIndent
			<< "\"></span>";
		status.codeIndent = 0;
	}
	if (! ident.empty()) {
		if (ch == '(') {
			escapeIdent(out, "fn", ident);
		} else if (isKeyword(ident)) {
			escapeIdent(out, "keyword", ident);
		} else if (isType(ident)) {
			escapeIdent(out, "type", ident);
		} else if (isNum(ident)) {
			escapeIdent(out, "num", ident);
		} else {
			escapeIdent(out, "var", ident);
		}
		ident.clear();
	}
;
		out << "<br/>\n";
		
	newline = ch == '\n';
	if (
		status.state !=
			HtmlState::inHeader
	) {
		writeOneEscaped(out, ch);
	}
;
		continue;
	}
 
	if (newline && ch == '\t') {
		++status.codeIndent;
		continue;
	}
 
	if (status.codeIndent) {
		out << "<span class=\"in"
			<< status.codeIndent
			<< "\"></span>";
		status.codeIndent = 0;
	}

	if (! status.codeSpecial && (ch == '\'' || ch == '"' || ch == '`')) {
		status.codeSpecial = ch;
		status.name.clear(true);
		out << "<span class=\"str\">" << static_cast<char>(ch);
		continue;
	}

	
	if (ch == '{' && ident.size() == 1) {
		char lc { ident.front() };
		switch (lc) {
			
	case 'd':
		writeMacroHeader(out, status, "def", lc);
		break;

	case 'D':
		writeMacroHeader(out, status, "globdef", lc);
		break;

	case 'a':
		writeMacroHeader(out, status, "add", lc);
		break;

	case 'A':
		writeMacroHeader(out, status, "globadd", lc);
		break;

	case 'r':
		writeMacroHeader(out, status, "replace", lc);
		break;

	case 'R':
		writeMacroHeader(out, status, "globreplace", lc);
		break;

	case 'x':
		writeMacroHeader(out, status, "end", lc);
		break;

	case 'e':
		writeMacroHeader(out, status, "expand", lc);
		break;

	case 'E':
		writeMacroHeader(out, status, "multiple", lc);
		break;

	case 'g':
		writeMacroHeader(out, status, "globexpand", lc);
		break;

	case 'G':
		writeMacroHeader(out, status, "globmult", lc);
		break;

	case 'i':
		writeMacroHeader(out, status, "include", lc);
		break;

	case 't':
		writeMacroClass(out, status, "type", lc);
		break;

	case 'v':
		writeMacroClass(out, status, "var", lc);
		break;

	case 'f':
		writeMacroClass(out, status, "fn", lc);
		break;

	case 'k':
		writeMacroClass(out, status, "keyword", lc);
		break;

	case 's':
		writeMacroClass(out, status, "str", lc);
		break;

	case 'n':
		writeMacroClass(out, status, "num", lc);
		break;

	case 'p':
		writeMacroClass(out, status, "var", lc);
		out << "@priv(<span>";
		break;

	case 'm':
		writeMacroClass(out, status, "var", lc);
		out << "@magic(<span>";
		break;

	case 'b':
		writeMacroClass(out, status, "virt", lc);
		out << "</span><br/>";
		break;

			default: break;
		}
		if (status.codeSpecial) {
			ident.clear();
			newline = false;
			continue;
		}
	}

	if (
		(
			ch == '}' &&
			status.codeSpecial &&
			status.codeSpecial != '\'' &&
			status.codeSpecial != '"' &&
			status.codeSpecial != '`'
		) 
	||
		(
			(
				status.codeSpecial == '\'' ||
				status.codeSpecial == '"' ||
				status.codeSpecial == '`'
			) &&
			ch == status.codeSpecial && (
				status.name.str().empty() ||
				status.name.str().back() != '\x5c'
			)
		)
	) {
		
	if (status.codeIndent) {
		out << "<span class=\"in"
			<< status.codeIndent
			<< "\"></span>";
		status.codeIndent = 0;
	}
	if (! ident.empty()) {
		if (ch == '(') {
			escapeIdent(out, "fn", ident);
		} else if (isKeyword(ident)) {
			escapeIdent(out, "keyword", ident);
		} else if (isType(ident)) {
			escapeIdent(out, "type", ident);
		} else if (isNum(ident)) {
			escapeIdent(out, "num", ident);
		} else {
			escapeIdent(out, "var", ident);
		}
		ident.clear();
	}
;
		ident.clear();
		newline = false;
		if (status.codeSpecial != 'i') {
			writeEscaped(out, status.name.str());
		}
		switch (status.codeSpecial) {
			
	case 'i': {
		
	auto ext = status.name.str().find_last_of('.');
	ASSERT_MSG(ext != std::string::npos,
		"no period"
	);
	out << "<a href=\"" 
		<< status.name.str().substr(0, ext) << ".html\">";
	out << status.name.str() << "</a>)</span>";
	status.name.clear();
;
		break;
	}
	case 'a': case 'e': case 'E': case 'x':
	case 'g': case 'G': case 'A': case 'D':
	case 'R':
	case 'r': case 'd': case 'p': case 'm': {
		out << "</span>)";
		break;
	}
	case '\'': case '"': case '`': {
		out << status.codeSpecial;
		break;
	}

		}
		if (status.codeSpecial != 'b') {
			out << "</span>";
		}
		status.codeSpecial = 0;
		status.name.clear();
		continue;
	}
;

	if (ch != EOF && status.name.active()) {
		status.name.add(ch);
		continue;
	}
;
		
	if (status.codeIndent) {
		out << "<span class=\"in"
			<< status.codeIndent
			<< "\"></span>";
		status.codeIndent = 0;
	}
	if (! ident.empty()) {
		if (ch == '(') {
			escapeIdent(out, "fn", ident);
		} else if (isKeyword(ident)) {
			escapeIdent(out, "keyword", ident);
		} else if (isType(ident)) {
			escapeIdent(out, "type", ident);
		} else if (isNum(ident)) {
			escapeIdent(out, "num", ident);
		} else {
			escapeIdent(out, "var", ident);
		}
		ident.clear();
	}
;
		
	newline = ch == '\n';
	if (
		status.state !=
			HtmlState::inHeader
	) {
		writeOneEscaped(out, ch);
	}
;
		continue;
	}
 
	if (
		newline &&
		status.state == HtmlState::inNotes
	) {
		if (ch == '*') {
			out << "</li><li>\n";
			ident.clear();
			newline = false;
			continue;
		} else if (ch != ' ' && ch != '\t') {
			out << "</li></ul></div>\n";
			status.state = HtmlState::afterSlide;
			
	newline = ch == '\n';
	if (
		status.state !=
			HtmlState::inHeader
	) {
		writeOneEscaped(out, ch);
	}
;
			continue;
		}
	}
 
	if (newline && ch == '*') {
		if (isOutOfHtmlSpecial(&status)) {
			if (status.state != HtmlState::inSlide) {
				out << "<div>\n";
			}
			status.state = HtmlState::inNotes;
			out << "<ul><li>\n";
			ident.clear();
			newline = false;
			continue;
		}
	}
 
	if (
		ch == '`' &&
		status.state == HtmlState::inNotes
	) {
		
	if (status.codeIndent) {
		out << "<span class=\"in"
			<< status.codeIndent
			<< "\"></span>";
		status.codeIndent = 0;
	}
	if (! ident.empty()) {
		if (ch == '(') {
			escapeIdent(out, "fn", ident);
		} else if (isKeyword(ident)) {
			escapeIdent(out, "keyword", ident);
		} else if (isType(ident)) {
			escapeIdent(out, "type", ident);
		} else if (isNum(ident)) {
			escapeIdent(out, "num", ident);
		} else {
			escapeIdent(out, "var", ident);
		}
		ident.clear();
	}
;
		if (status.noteInCode) {
			out << "</code>";
		} else {
			out << "<code>";
		}
		status.noteInCode = ! status.noteInCode;
		ident.clear();
		newline = false;
		continue;
	}
 
	if (status.state == HtmlState::inNotes &&
	status.noteInCode) {
		
	if (ch == '{' && ident.size() == 1) {
		char lc { ident.front() };
		switch (lc) {
			
	case 'd':
		writeMacroHeader(out, status, "def", lc);
		break;

	case 'D':
		writeMacroHeader(out, status, "globdef", lc);
		break;

	case 'a':
		writeMacroHeader(out, status, "add", lc);
		break;

	case 'A':
		writeMacroHeader(out, status, "globadd", lc);
		break;

	case 'r':
		writeMacroHeader(out, status, "replace", lc);
		break;

	case 'R':
		writeMacroHeader(out, status, "globreplace", lc);
		break;

	case 'x':
		writeMacroHeader(out, status, "end", lc);
		break;

	case 'e':
		writeMacroHeader(out, status, "expand", lc);
		break;

	case 'E':
		writeMacroHeader(out, status, "multiple", lc);
		break;

	case 'g':
		writeMacroHeader(out, status, "globexpand", lc);
		break;

	case 'G':
		writeMacroHeader(out, status, "globmult", lc);
		break;

	case 'i':
		writeMacroHeader(out, status, "include", lc);
		break;

	case 't':
		writeMacroClass(out, status, "type", lc);
		break;

	case 'v':
		writeMacroClass(out, status, "var", lc);
		break;

	case 'f':
		writeMacroClass(out, status, "fn", lc);
		break;

	case 'k':
		writeMacroClass(out, status, "keyword", lc);
		break;

	case 's':
		writeMacroClass(out, status, "str", lc);
		break;

	case 'n':
		writeMacroClass(out, status, "num", lc);
		break;

	case 'p':
		writeMacroClass(out, status, "var", lc);
		out << "@priv(<span>";
		break;

	case 'm':
		writeMacroClass(out, status, "var", lc);
		out << "@magic(<span>";
		break;

	case 'b':
		writeMacroClass(out, status, "virt", lc);
		out << "</span><br/>";
		break;

			default: break;
		}
		if (status.codeSpecial) {
			ident.clear();
			newline = false;
			continue;
		}
	}

	if (
		(
			ch == '}' &&
			status.codeSpecial &&
			status.codeSpecial != '\'' &&
			status.codeSpecial != '"' &&
			status.codeSpecial != '`'
		) 
	||
		(
			(
				status.codeSpecial == '\'' ||
				status.codeSpecial == '"' ||
				status.codeSpecial == '`'
			) &&
			ch == status.codeSpecial && (
				status.name.str().empty() ||
				status.name.str().back() != '\x5c'
			)
		)
	) {
		
	if (status.codeIndent) {
		out << "<span class=\"in"
			<< status.codeIndent
			<< "\"></span>";
		status.codeIndent = 0;
	}
	if (! ident.empty()) {
		if (ch == '(') {
			escapeIdent(out, "fn", ident);
		} else if (isKeyword(ident)) {
			escapeIdent(out, "keyword", ident);
		} else if (isType(ident)) {
			escapeIdent(out, "type", ident);
		} else if (isNum(ident)) {
			escapeIdent(out, "num", ident);
		} else {
			escapeIdent(out, "var", ident);
		}
		ident.clear();
	}
;
		ident.clear();
		newline = false;
		if (status.codeSpecial != 'i') {
			writeEscaped(out, status.name.str());
		}
		switch (status.codeSpecial) {
			
	case 'i': {
		
	auto ext = status.name.str().find_last_of('.');
	ASSERT_MSG(ext != std::string::npos,
		"no period"
	);
	out << "<a href=\"" 
		<< status.name.str().substr(0, ext) << ".html\">";
	out << status.name.str() << "</a>)</span>";
	status.name.clear();
;
		break;
	}
	case 'a': case 'e': case 'E': case 'x':
	case 'g': case 'G': case 'A': case 'D':
	case 'R':
	case 'r': case 'd': case 'p': case 'm': {
		out << "</span>)";
		break;
	}
	case '\'': case '"': case '`': {
		out << status.codeSpecial;
		break;
	}

		}
		if (status.codeSpecial != 'b') {
			out << "</span>";
		}
		status.codeSpecial = 0;
		status.name.clear();
		continue;
	}
;
	}
 
	if (
		false &&
		ch == '*' && /*last == '*' && */
		status.state == HtmlState::inNotes
	) {
		if (status.noteInBold) {
			out << "</b>";
		} else {
			out << "<b>";
		}
		status.noteInBold =
			! status.noteInBold;
		newline = false;
		continue;
	}
 
	if (status.state == HtmlState::inNotes) {
		
	if (status.codeIndent) {
		out << "<span class=\"in"
			<< status.codeIndent
			<< "\"></span>";
		status.codeIndent = 0;
	}
	if (! ident.empty()) {
		if (ch == '(') {
			escapeIdent(out, "fn", ident);
		} else if (isKeyword(ident)) {
			escapeIdent(out, "keyword", ident);
		} else if (isType(ident)) {
			escapeIdent(out, "type", ident);
		} else if (isNum(ident)) {
			escapeIdent(out, "num", ident);
		} else {
			escapeIdent(out, "var", ident);
		}
		ident.clear();
	}
;
		
	newline = ch == '\n';
	if (
		status.state !=
			HtmlState::inHeader
	) {
		writeOneEscaped(out, ch);
	}
;
		continue;
	}
;
		if (ch == EOF) { break; }
		
	newline = ch == '\n';
	if (
		status.state !=
			HtmlState::inHeader
	) {
		writeOneEscaped(out, ch);
	}
;
	}
} ;
	in.close();
;
	out.close();
;
	}
;

	}
