
	
	
	#include <iostream>
	#include <cstdlib>

	#include <list>
	#include <map>
	#include <sstream>

	#include <string.h>

	#include <fstream>
	#include <iostream>
	#include <memory>
	#include <vector>

	#include <functional>
	#include <sstream>

	#include <set>
	#include <string>

	#include <ctype.h>
;
	
	#define ASSERT(COND, ...) \
		if (! (COND)) { \
			std::cerr << __FILE__ << ':' \
				<< __LINE__ << ' ' \
				<< #COND << " FAILED: "; \
			failSuffix(__VA_ARGS__); \
			exit(EXIT_FAILURE); \
		}

	inline void failSuffix() {
		std::cerr << std::endl;
	}

	template<typename T, typename... Args>
	inline void failSuffix(
		const T& a, Args... args
	) {
		std::cerr << a;
		failSuffix(args...);
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
		const std::string &file = "/dev/null",
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
		const std::string &file = "/dev/null",
		int line = 1
	) {
		char c = static_cast<char>(ch);
		add(std::string(&c, &c + 1), file, line);
	}

	void add(const Buf &b) {
		activate();
		if (b.empty()) { return; }
		assertCont(b._file, b._startLine);
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
	bool _shouldAdd;
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
	_shouldAdd { true },

		name { name }
	{
	}

	int next() {
		int ch { EOF };
		if (file.is_open()) {
			ch = file.get();
			
	if (_shouldAdd) { ++_line; }
	_shouldAdd = (ch == '\n');
;
			if (! file.good()) {
				file.close();
			}
		}
		return ch;
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

	int get() {
		int ch { EOF };
		while (_input) {
			ch = _input->next();
			if (ch != EOF) { break; }
			
	_used.push_back(std::move(_input));
	if (! _pending.empty()) {
		_input = std::move(_pending.back());
		_pending.pop_back();
	}
	frags = frags->setLink(nullptr);
;
		}
		return ch;
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

	Inputs inputs;

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
		state { HtmlState::nothingWritten }
		
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
		
	if (s->state == HtmlState::inHeader) {
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
		out << "<span class=\"" << cls << "\">"
			<< s << "</span>";
	}

	bool isKeyword(const std::string &s) {
		static std::set<std::string> reserved {
			"break", "case", "catch", "continue",
			"default", "delete", "else", "for",
			"if", "in", "new", "return", "static",
			"switch", "try", "typeof", "while"
		};
		return reserved.find(s) != reserved.end();
	}

	bool isType(const std::string &s) {
		static std::set<std::string> reserved {
			"FILE",
			"bool", "char", "const", "enum",
			"int", "let", "long", "signed", "struct",
			"union", "unsigned", "void"
		};
		if (reserved.find(s) != reserved.end()) {
			return true;
		}
		if (s.size() >= 2) {
			if (isupper(s[0]) && islower(s[1])) {
				return true;
			}
		}
		return false;
	}

	bool isNum(const std::string &s) {
		static std::set<std::string> reserved {
			"EOF", "NULL", "nullptr",
			"false", "null", "true", "undefined"
		};
		if (isdigit(s[0])) { return true; }
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
	ASSERT(c.startLine() == b.startLine());
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
	ASSERT(c.endLine() == b.endLine());
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

	ASSERT(c.str() == "abc\ndef\nghi");
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
} 
	if (! someFile) {
		inputs.push(argv[1]);
		someFile = true;
		continue;
	}
;
		ASSERT(
			false,
			"unknown argument [", argv[i], ']'
		);
	}

	if (! someFile) {
		inputs.push("index.x");
	}
;
	 {
	
	Frag *frag { nullptr };
	Buf buffer;

	char openCh { '\0' };

	Buf name;
;
	int last { inputs.get() };
	int ch { last != EOF ? inputs.get() : EOF };
	while (ch != EOF) {
		
	switch (ch) {
		case '{':
			 {
	if (! frag) {
		static const char valids[] { "aAdDirR" };
		if (strchr(valids, last)) {
			openCh = last;
			name.activate();
			break;
		}
	}
}  {
	if (frag) {
		bool valid { false };
		
	static const char valids[] { 
		"fvsntkxeEgGpmb"
	};
	if (strchr(valids, last)) {
		valid = true;
	}
;
		if (valid) {
			openCh = last;
			name.activate();
			break;
		}
	}
} 
	if (frag) {
		buffer.add(last, inputs.cur()->name, inputs.cur()->line());
	}
;
			break;
		case '}': {
			bool processed { false };
			 {
	if (name.active()) {
		
	if (openCh == 'd') {
		ASSERT(! frag, "def in frag");
		FragMap *fm { &inputs.cur()->frags };
		
	frag = fm->find(name.str());
	if (isPopulatedFrag(frag)) {
		std::cerr << "frag [" << name.str() <<
			"] already defined" <<
			std::endl;
	}
;
		if (! frag) {
			frag = &(*fm)[name.str()];
		}
		processed = true;
	}

	if (openCh == 'D') {
		ASSERT(! frag, "def in frag");
		FragMap *fm { frags };
		
	frag = fm->find(name.str());
	if (isPopulatedFrag(frag)) {
		std::cerr << "frag [" << name.str() <<
			"] already defined" <<
			std::endl;
	}
;
		if (! frag) {
			frag = &root[name.str()];
		}
		processed = true;
	}

	if (openCh == 'a') {
		ASSERT(! frag, "add in frag");
		FragMap *fm { &inputs.cur()->frags };
		FragMap *ins { fm };
		frag = fm->find(name.str());
		
	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" << name.str() <<
			"] not defined" << std::endl;
		frag = &fm->get(name.str(), *ins);
	}
;
		processed = true;
	}

	if (openCh == 'A') {
		ASSERT(! frag, "add in frag");
		FragMap *fm { frags };
		FragMap *ins { &root };
		frag = fm->find(name.str());
		
	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" << name.str() <<
			"] not defined" << std::endl;
		frag = &fm->get(name.str(), *ins);
	}
;
		processed = true;
	}

	if (openCh == 'r') {
		ASSERT(! frag, "replace in frag");
		frag = &(inputs.cur()->frags[name.str()]);
		ASSERT(
			frag, "frag ", name.str(), " not defined"
		);
		frag->clear();
		processed = true;
	}

	if (openCh == 'R') {
		ASSERT(! frag, "replace in frag");
		frag = &frags->get(name.str(), root);
		ASSERT(
			frag, "frag ", name.str(), " not defined"
		);
		frag->clear();
		processed = true;
	}

	if (openCh == 'x') {
		ASSERT(frag, "end not in frag");
		
	ASSERT(
		frag->name == name.str(),
		"closing [", name.str(), "] != [",
			frag->name, ']'
	);
;
		
	if (! buffer.empty()) {
		frag->add(buffer);
		buffer.clear();
	}
;
		frag = nullptr;
		processed = true;
	}

	if (openCh == 'i') {
		ASSERT(! frag, "include in frag");
		if (! inputs.has(name.str())) {
			inputs.push(name.str());
		}
		processed = true;
	}

	if (openCh == 'e') {
		ASSERT(frag, "expand not in frag");
		
	if (! buffer.empty()) {
		frag->add(buffer);
		buffer.clear();
	}
;
		Frag &sub = inputs.cur()->frags[name.str()];
		
	if (sub.expands()) {
		std::cerr << "multiple expands of ["
			<< sub.name << "]" << std::endl;
	}
	if (sub.multiples()) {
		std::cerr << "expand after mult of ["
			<< sub.name << "]" << std::endl;
	}
;
		sub.addExpand();
		frag->add(&sub);
		processed = true;
	}

	if (openCh == 'g') {
		ASSERT(frag, "expand not in frag");
		
	if (! buffer.empty()) {
		frag->add(buffer);
		buffer.clear();
	}
;
		Frag &sub = frags->get(name.str(), root);
		
	if (sub.expands()) {
		std::cerr << "multiple expands of ["
			<< sub.name << "]" << std::endl;
	}
	if (sub.multiples()) {
		std::cerr << "expand after mult of ["
			<< sub.name << "]" << std::endl;
	}
;
		sub.addExpand();
		frag->add(&sub);
		processed = true;
	}

	if (openCh == 'E') {
		ASSERT(frag, "multiple not in frag");
		
	if (! buffer.empty()) {
		frag->add(buffer);
		buffer.clear();
	}
;
		Frag &sub { inputs.cur()->frags[name.str()] };
		
	if (sub.expands()) {
		std::cerr
			<< "multiple after expand of ["
			<< sub.name << "]" << std::endl;
	}
;
		sub.addMultiple();
		frag->add(&sub);
		processed = true;
	}

	if (openCh == 'G') {
		ASSERT(frag, "multiple not in frag");
		
	if (! buffer.empty()) {
		frag->add(buffer);
		buffer.clear();
	}
;
		Frag &sub { frags->get(name.str(), root) };
		
	if (sub.expands()) {
		std::cerr
			<< "multiple after expand of ["
			<< sub.name << "]" << std::endl;
	}
;
		sub.addMultiple();
		frag->add(&sub);
		processed = true;
	}

	if (openCh == 'p') {
		ASSERT(frag, "private not in frag");
		
	std::hash<std::string> h;
	unsigned cur {
		h(inputs.cur()->name + ':' + name.str())
			& 0x7fffffff
	};

	
	if (! buffer.empty()) {
		frag->add(buffer);
		buffer.clear();
	}
;
	std::ostringstream hashed;
	hashed << "_private_"
		<< cur << '_' << name.str();
	frag->add(
		hashed.str(), inputs.cur()->name,
		name.startLine()
	);
;
		processed = true;
	}

	if (openCh == 'm') {
		ASSERT(frag, "magic not in frag");
		
	std::hash<std::string> h;
	unsigned cur {
		h(inputs.cur()->name + ':' + name.str())
			& 0x7fffffff
	};

	
	if (! buffer.empty()) {
		frag->add(buffer);
		buffer.clear();
	}
;
	std::ostringstream value;
	value << cur;
	
	if (! buffer.empty()) {
		frag->add(buffer);
		buffer.clear();
	}
;
	frag->add(
		value.str(),
		inputs.cur()->name, inputs.cur()->line()
	);
;
		processed = true;
	}

	if (! processed) {
		ASSERT(
			frag, "unknown frag ",
			name.str()
		);
		buffer.add(name);
		processed = true;
	}
;
		name.clear();
		last = ch;
		ch = inputs.get();
	}
} 
	if (frag && ! processed) {
		buffer.add(last, inputs.cur()->name, inputs.cur()->line());
	}
;
			break;
		}
		default:
			 {
	if (name.active()) {
		name.add(ch, inputs.cur()->name, inputs.cur()->line());
		break;
	}
}  {
	if (frag) {
		buffer.add(last, inputs.cur()->name, inputs.cur()->line());
	}
} ;
	}
;
		last = ch; ch = inputs.get();
	}
} ;
	 {
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
		std::cerr << "frag ["
			<< frag->name
			<< "] not called"
			<< std::endl;
	}
} 
	if (frag->multiples() == 1) {
		std::cerr << "multiple frag ["
			<< frag->name
			<< "] only used once"
			<< std::endl;
	}

	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag ["
			<< frag->name
			<< "] not populated"
			<< std::endl;
	}
;
	}
}  {
	for (auto &j : inputs)
	{
		for (auto &i : j->frags) {
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
		std::cerr << "frag ["
			<< frag->name
			<< "] not called"
			<< std::endl;
	}
} 
	if (frag->multiples() == 1) {
		std::cerr << "multiple frag ["
			<< frag->name
			<< "] only used once"
			<< std::endl;
	}

	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag ["
			<< frag->name
			<< "] not populated"
			<< std::endl;
	}
;
		}
	}
} ;
	
	for (auto &cur : inputs) {
		
	const std::string &name { cur->name };
	std::string outPath {
		name.substr(0, name.size() - 2) +
		".html"
	};
	std::ofstream out { outPath.c_str() };
	 
	std::ifstream in { cur->name.c_str() };
	 {
	HtmlStatus status;
	bool newline { true };
	std::string ident;
	for (;;) {
		int ch { in.get() };
		 
	if (ch == '#' && newline) {
		if (isOutOfHtmlSpecial(&status) ||
			status.state == HtmlState::inHeader
		) {
			++status.headerLevel;
			if (status.state != HtmlState::inHeader) {
				status.headerState =
					status.state;
			}
			status.state = HtmlState::inHeader;
			continue;
		}
	}
 
	if (status.state == HtmlState::inHeader) {
		if (ch == '\n') {
			 
	ASSERT(! status.headerName.empty());
	 
	switch (status.headerState) {
		case HtmlState::nothingWritten: {
			 
	out << "<!doctype html>" << std::endl;
	out << "<html lang=\"de\"l>" << std::endl;
	out << "<head>" << std::endl;
	 
	out << "<meta charset=\"utf-8\">" << std::endl;
	out << "<title>";
	writeEscaped(
		out, status.headerName
	);
	out << "</title>" << std::endl;
	out << "<link rel=\"stylesheet\" "
		"type=\"text/css\" href=\""
		<< stylesheet << "\">";
;
	out << "</head>" << std::endl;
	out << "<body>" << std::endl;
;
			break;
		}
		case HtmlState::inSlide: {
			out << "</div>" << std::endl;
			out << "</div>" << std::endl;
			break;
		}
		default: {
			out << "</div>" << std::endl;
		}
	}
;
	 
	out << "<h" << status.headerLevel << '>';
	writeEscaped(
		out, status.headerName
	);
	out << "</h" << status.headerLevel << '>' << std::endl;
;
	out << "<div class=\"slides\">" << std::endl;
	out << "<div><div>" << std::endl;
	 
	out << "<h" << status.headerLevel << '>';
	writeEscaped(
		out, status.headerName
	);
	out << "</h" << status.headerLevel << '>' << std::endl;
;
	out << "</div>" << std::endl;
;
			 
	status.state = HtmlState::inSlide;
	status.headerLevel = 0;
	status.headerName.clear();
	status.headerState = HtmlState::inSlide;
;
			
	newline = ch == '\n';
	if (status.state != HtmlState::inHeader) {
		writeOneEscaped(out, ch);
	}
;
			continue;
		}
	}
 
	if (status.state == HtmlState::inHeader) {
		if (! status.headerName.empty()) {
			status.headerName.push_back(ch);
			
	newline = ch == '\n';
	if (status.state != HtmlState::inHeader) {
		writeOneEscaped(out, ch);
	}
;
			continue;
		}
	}
 
	if (status.state == HtmlState::inHeader) {
		if (ch > ' ' && status.headerName.empty()) {
			status.headerName.push_back(ch);
			
	newline = ch == '\n';
	if (status.state != HtmlState::inHeader) {
		writeOneEscaped(out, ch);
	}
;
			continue;
		}
	}
 
	if (newline && ch == '`') {
		if (isOutOfHtmlSpecial(&status) ||
			status.state == HtmlState::inCode
		) {
			++status.codeOpening;
			continue;
		}
	}
 
	if (
		ch == '\n' && status.codeOpening == 3
	) {
		status.codeOpening = 0;
		if (isOutOfHtmlSpecial(&status)) {
			
	if (status.state == HtmlState::inSlide) {
		out << "</div>" << std::endl;
	}
	out << "<div><div>" << std::endl;
	out << "<code>" << std::endl;
	status.state = HtmlState::inCode;
	
	newline = ch == '\n';
	if (status.state != HtmlState::inHeader) {
		writeOneEscaped(out, ch);
	}
;
;
			continue;
		} else if (
			status.state == HtmlState::inCode
		) {
			
	out << "</code>" << std::endl;
	out << "</div>" << std::endl;
	status.state = HtmlState::inSlide;
	status.codeIndent = 0;
	status.codeSpecial = '\0';
	
	newline = ch == '\n';
	if (status.state != HtmlState::inHeader) {
		writeOneEscaped(out, ch);
	}
;
;
			continue;
		}
	}

	if (status.codeOpening == 1) {
		if (! status.codeSpecial && status.state == HtmlState::inCode) {
			status.codeSpecial = '`';
			status.name.clear(true);
			
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
			out << "<span class=\"str\">`";
		} else if (
			status.codeSpecial == '`' && (
				status.name.str().empty() ||
				status.name.str().back() != '\x5c'
			)
		) {
			
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
			writeEscaped(out, status.name.str());
			out << "`</span>";
			status.codeSpecial = 0;
			status.name.clear();
		}
	}

	status.codeOpening = 0;

	if (status.state == HtmlState::inCode) {
		if (ch == EOF) {
			std::cerr << "unterminated code block" << std::endl;
			break;
		}
	}

	if (status.state == HtmlState::inCode) {
		if (! status.codeSpecial && (isalnum(ch) || ch == '_')) {
			ident.push_back(ch);
			continue;
		}
	}

	if (status.state == HtmlState::inCode) {
		 
	if (ch == '\n') {
		
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
		out << "<br/>" << std::endl;
		
	newline = ch == '\n';
	if (status.state != HtmlState::inHeader) {
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
		writeMacroClass(out, status, "type", lc);
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
	ASSERT(
		ext != std::string::npos,
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
	if (status.state != HtmlState::inHeader) {
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
			out << "</li><li>" << std::endl;
			ident.clear();
			newline = false;
			continue;
		} else if (ch != ' ' && ch != '\t') {
			out << "</li></ul></div>" << std::endl;
			status.state = HtmlState::afterSlide;
			
	newline = ch == '\n';
	if (status.state != HtmlState::inHeader) {
		writeOneEscaped(out, ch);
	}
;
			continue;
		}
	}
 
	if (newline && ch == '*') {
		if (isOutOfHtmlSpecial(&status)) {
			if (status.state != HtmlState::inSlide) {
				out << "<div>" << std::endl;
			}
			status.state = HtmlState::inNotes;
			out << "<ul><li>" << std::endl;
			ident.clear();
			newline = false;
			continue;
		}
	}
 
	if (
		ch == '`' &&
		status.state == HtmlState::inNotes
	) {
		
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
		writeMacroClass(out, status, "type", lc);
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
	ASSERT(
		ext != std::string::npos,
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
	if (status.state != HtmlState::inHeader) {
		writeOneEscaped(out, ch);
	}
;
		continue;
	}
;
		if (ch == EOF) { break; }
		
	newline = ch == '\n';
	if (status.state != HtmlState::inHeader) {
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
