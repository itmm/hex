
	
	
	#include <stdio.h>
	#include <memory>
	#include <vector>

	#include <list>
	#include <map>

	#include <string.h>

	#include <set>
	#include <string>

	#include <ctype.h>
;
	
	#define ASSERT(COND, ...) \
		if (! (COND)) { \
			fprintf(stderr, \
				"%s:%d", \
				__FILE__, __LINE__); \
			fprintf(stderr, \
				" FAILED: " __VA_ARGS__); \
			fprintf(stderr, "\n"); \
			exit(EXIT_FAILURE); \
		}
;

	
	class Hash {
		private:
			unsigned _hash;
		public:
			
	Hash(): _hash(0x3d9a73b5) {}

	int hash() const {
		return static_cast<int>(
			_hash & 0x7fffffff
		);
	}

	unsigned add(const std::string &s);
;
	};

	unsigned Hash::add(
		const std::string &s
	) {
		for (auto &ch : s) {
			
	_hash ^= ch;

	_hash = (_hash << 3) | (_hash >> 29);
;
		}
		return hash();
	}

	int calcHash(
		const std::string &name
	) {
		Hash h;
		return h.add(name);
	}
;

	
	struct Frag;

	struct FragEntry {
		Frag *frag;
		std::string value;
		
	std::string source;
	int line;
;
		FragEntry(
			const std::string &value = std::string()
		):
			frag(nullptr),
			value(value)
		{}
	};

	struct Frag {
		std::list<FragEntry> entries;
		int expands;
		int multiples;
		std::string name;
		
	Frag(
		const std::string &name
	):
		entries(),
		expands(0),
		multiples(0),
		name(name)
	{ }

	void clear() {
		entries.clear();
	}

	Frag &add(
		const std::string &value,
		const std::string &source,
		int line
	) {
		entries.push_back(FragEntry());
		FragEntry &entry = entries.back();
		entry.value += value;
		
	entry.source = source;
	entry.line = line;
;
		return *this;
	}

	Frag &add(Frag *child);
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
		return f && f->entries.size();
	}

	int getFragEntryValueSize(
		const FragEntry &e
	) {
		return e.value.size();
	}

	
	bool isFragInFrag(
		Frag *needle, Frag *haystack
	) {
		ASSERT(needle);
		ASSERT(haystack);
		
	if (needle == haystack) {
		return true;
	}
;
		
	for (
		auto i = haystack->entries.begin();
		i != haystack->entries.end(); ++i
	) {
		if (! i->frag) { continue; }
		if (isFragInFrag(
			needle, i->frag
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
		
	if (! entries.empty() &&
		! entries.back().frag
	) {
		entries.back().frag = child;
		return *this;
	}
;
		
	entries.push_back(FragEntry());
	FragEntry &entry = entries.back();
	entry.frag = child;
;
		return *this;
	}

	
	std::string *fragTestBufferCur = nullptr;
;
	void serializeFrag(
		const Frag &frag, FILE *out,
		bool writeLineMacros
	) {
		ASSERT(out);
		
	auto entry = frag.entries.begin();
	for (; entry != frag.entries.end(); ++entry) {
		
	if (getFragEntryValueSize(*entry)) {
		if (! fragTestBufferCur) {
			ASSERT(fwrite(
				entry->value.data(), 1, entry->value.size(),
				out
			) == entry->value.size());
		} else {
			*fragTestBufferCur += entry->value;
		}
	}
;
		if (entry->frag) {
			serializeFrag(
				*entry->frag, out,
				writeLineMacros
			);
		}
	}
;
	}

	void testFrag(
		const Frag &frag,
		const std::string &expected
	) {
		
	std::string buffer;
	fragTestBufferCur = &buffer;
	serializeFrag(frag, (FILE *) &buffer, false);
	ASSERT(buffer == expected);
	fragTestBufferCur = nullptr;
;
	}

	void addStringToFrag(
		Frag *frag,
		const std::string &str
	) {
		frag->add(
			str, std::string(), 0
		);
	}

	struct FragMap {
		FragMap *link;
		std::map<std::string, Frag> map;
		
	FragMap(): link(nullptr) {}

	Frag *find(const std::string &name) {
		auto found = map.find(name);
		if (found != map.end()) {
			return &found->second;
		}
		if (link) {
			return link->find(name);
		}
		return nullptr;
	}

	Frag &get(const std::string &name, FragMap &insert) {
		Frag *found = find(name);
		if (found) { return *found; }
		auto created = insert.map.insert(
			std::pair<std::string, Frag>(name, name)
		);
		return created.first->second;
	}

	Frag &operator[](const std::string &name) {
		return get(name, *this);
	}
;
	};
;

	class Input {
		private:
			FILE *file;
		public:
			const std::string name;
			
	FragMap frags;

	int line;
;
			
	Input(
		FILE *file,
		const std::string &name
	): file(file), name(name)
		
	, line(1)

	{
	}

	int next() {
		int ch = EOF;
		if (file) {
			ch = fgetc(file);
			if (ch == EOF) {
				fclose(file);
				file = nullptr;
			}
		}
		return ch;
	}
;
	};

	std::unique_ptr<Input> input;
	std::vector<std::unique_ptr<Input>>
		pending;
	std::vector<std::unique_ptr<Input>>
		used;

	FragMap root;
	FragMap *frags = &root;

	void pushPath(const std::string &path) {
		FILE *f = fopen(path.c_str(), "r");
		
	ASSERT(
		f, "can't open [%s]", path.c_str()
	);
;
		std::unique_ptr<Input> i =
			std::make_unique<Input>(f, path);
		
	if (input) {
		input->frags.link = frags;
		frags = &input->frags;
	}
;
		if (input) {
			pending.push_back(
				std::move(input)
			);
		}
		input = std::move(i);
	}

	std::string stylesheet { "slides/slides.css" };

	int nextCh() {
		int ch = EOF;
		while (input) {
			ch = input->next();
			
	if (ch == '\n') {
		++input->line;
	}
;
			if (ch != EOF) { break; }
			
	used.push_back(std::move(input));
	if (! pending.empty()) {
		input = std::move(pending.back());
		pending.pop_back();
	}
	FragMap *nxt = frags->link;
	frags->link = nullptr;
	frags = nxt;
;
		}
		return ch;
	}

	bool alreadyUsed(const std::string &name) {
		if (input && input->name == name) {
			return true;
		}
		for (auto &j : pending) {
			if (j->name == name) {
				return true;
			}
		}
		for (auto &j : used) {
			if (j->name == name) {
				return true;
			}
		}
		return false;
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
	char codeName[100];
	char *codeNameEnd;

	bool noteInCode;
	bool noteInBold;

	};

	inline HtmlStatus::HtmlStatus():
		state(HtmlState::nothingWritten)
		
	, headerLevel(0)
	, headerName()

	, codeOpening(0)
	, codeIndent(0)
	, codeSpecial('\0')
	, codeNameEnd(nullptr)

	, noteInCode(false)
	, noteInBold(false)

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
 
	template <typename T>
	void writeEscaped(
		FILE *out, T str, T end
	) {
		ASSERT(out);
		for (; *str && str != end; ++str) {
			switch (*str) {
				
	case '<':
		fprintf(out, "&lt;");
		break;
	case '>':
		fprintf(out, "&gt;");
		break;
	case '&':
		fprintf(out, "&amp;");
		break;

				default:
					fputc(*str, out);
			}
		}
	}

	void escapeIdent(
		FILE *out,
		const char *cls,
		const std::string &s
	) {
		fprintf(
			out,
			"<span class=\"%s\">%.*s</span>",
			cls,
			(int) s.size(),
			s.data()
		);
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

	int main(
		int argc,
		const char **argv
	) {
		
	
	
	testFragName("abc");
	testFragName("");
	testFragName("A c");
	{
		Frag f("ab");
		ASSERT(f.entries.empty());
	}

	{
		FragEntry entry;
		ASSERT(! entry.frag);
	}

	{
		FragEntry entry;
		ASSERT(
			getFragEntryValueSize(entry) == 0
		);
	}

	{
		FragEntry entry("abc");
		ASSERT(
			getFragEntryValueSize(entry) == 3
		);
	}

	{
		FragEntry entry("abc");
		ASSERT(entry.value == "abc");
	}
 {
	Frag frag("");
	addStringToFrag(&frag, "abc");
	addStringToFrag(&frag, "def");
	testFrag(frag, "abcdef");
}  {
	Frag a("");
	Frag b("");
	addStringToFrag(&a, "abc");
	b.add(&a);
	addStringToFrag(&b, "def");
	b.add(&a);
	testFrag(b, "abcdefabc");
} ;
;
	
	bool someFile = false;
	for (int i = 1; i < argc; ++i) {
		 {
	static const std::string prefix { "--css=" };
	std::string arg { argv[i] };
	if (arg.substr(0, prefix.length()) == prefix) {
		stylesheet = arg.substr(prefix.length());
		continue;
	}
} 
	if (! someFile) {
		pushPath(argv[1]);
		someFile = true;
		continue;
	}
;
		ASSERT(
			false,
			"unknown argument [%s]",
			argv[i]
		);
	}

	if (! someFile) {
		pushPath("index.x");
	}
;
	 {
	
	Frag *frag = nullptr;
	std::string buffer;
	int bufferLine = 0;

	char openCh = '\0';

	std::string name;
	bool useName = false;
	int nameLine = 0;
;
	int last = nextCh();
	int ch = last != EOF ? nextCh() : EOF;
	while (ch != EOF) {
		
	switch (ch) {
		case '{':
			 {
	if (! frag) {
		static const char valids[] = "aAdDirR";
		if (strchr(valids, last)) {
			openCh = last;
			useName = true;
			break;
		}
	}
}  {
	if (frag) {
		bool valid = false;
		
	static const char valids[] =
		"fvsntkxeEgGpmb";
	if (strchr(valids, last)) {
		valid = true;
	}
;
		if (valid) {
			openCh = last;
			nameLine = input->line;
			useName = true;
			break;
		}
	}
} 
	if (frag) {
		if (buffer.empty()) {
			bufferLine = input->line;
		}
		buffer.push_back(last);
	}
;
			break;
		case '}': {
			bool processed = false;
			 {
	if (useName || ! name.empty()) {
		
	if (openCh == 'd') {
		ASSERT(! frag, "def in frag");
		FragMap *fm = &input->frags;
		
	frag = fm->find(name);
	if (isPopulatedFrag(frag)) {
		printf(
			"frag [%s] already defined\n",
			name.c_str()
		);
	}
;
		if (! frag) {
			frag = &(*fm)[name];
		}
		processed = true;
	}

	if (openCh == 'D') {
		ASSERT(! frag, "def in frag");
		FragMap *fm = frags;
		
	frag = fm->find(name);
	if (isPopulatedFrag(frag)) {
		printf(
			"frag [%s] already defined\n",
			name.c_str()
		);
	}
;
		if (! frag) {
			frag = &root[name];
		}
		processed = true;
	}

	if (openCh == 'a') {
		ASSERT(! frag, "add in frag");
		FragMap *fm = &input->frags;
		FragMap *ins = fm;
		frag = fm->find(name);
		
	if (! isPopulatedFrag(frag)) {
		printf(
			"frag [%s] not defined\n",
			name.c_str()
		);
		frag = &fm->get(name, *ins);
	}
;
		processed = true;
	}

	if (openCh == 'A') {
		ASSERT(! frag, "add in frag");
		FragMap *fm = frags;
		FragMap *ins = &root;
		frag = fm->find(name);
		
	if (! isPopulatedFrag(frag)) {
		printf(
			"frag [%s] not defined\n",
			name.c_str()
		);
		frag = &fm->get(name, *ins);
	}
;
		processed = true;
	}

	if (openCh == 'r') {
		ASSERT(! frag, "replace in frag");
		frag = &(input->frags[name]);
		ASSERT(
			frag, "frag %s not defined",
			name.c_str()
		);
		frag->clear();
		processed = true;
	}

	if (openCh == 'R') {
		ASSERT(! frag, "replace in frag");
		frag = &frags->get(name, root);
		ASSERT(
			frag, "frag %s not defined",
			name.c_str()
		);
		frag->clear();
		processed = true;
	}

	if (openCh == 'x') {
		ASSERT(frag, "end not in frag");
		
	ASSERT(
		frag->name == name,
		"closing [%s] != [%s]",
		name.c_str(), frag->name.c_str()
	);
;
		
	if (! buffer.empty()) {
		frag->add(
			buffer,
			input->name, bufferLine
		);
		buffer.clear();
	}
;
		frag = nullptr;
		processed = true;
	}

	if (openCh == 'i') {
		ASSERT(! frag, "include in frag");
		if (! alreadyUsed(name.c_str())) {
			pushPath(name);
		}
		processed = true;
	}

	if (openCh == 'e') {
		ASSERT(frag, "expand not in frag");
		
	if (! buffer.empty()) {
		frag->add(
			buffer,
			input->name, bufferLine
		);
		buffer.clear();
	}
;
		Frag &sub = input->frags[name];
		
	if (sub.expands) {
		printf(
			"multiple expands of [%s]\n",
			sub.name.c_str()
		);
	}
	if (sub.multiples) {
		printf(
			"expand after mult of [%s]\n",
			sub.name.c_str()
		);
	}
;
		++sub.expands;
		frag->add(&sub);
		processed = true;
	}

	if (openCh == 'g') {
		ASSERT(frag, "expand not in frag");
		
	if (! buffer.empty()) {
		frag->add(
			buffer,
			input->name, bufferLine
		);
		buffer.clear();
	}
;
		Frag &sub = frags->get(name, root);
		
	if (sub.expands) {
		printf(
			"multiple expands of [%s]\n",
			sub.name.c_str()
		);
	}
	if (sub.multiples) {
		printf(
			"expand after mult of [%s]\n",
			sub.name.c_str()
		);
	}
;
		++sub.expands;
		frag->add(&sub);
		processed = true;
	}

	if (openCh == 'E') {
		ASSERT(frag, "multiple not in frag");
		
	if (! buffer.empty()) {
		frag->add(
			buffer,
			input->name, bufferLine
		);
		buffer.clear();
	}
;
		Frag &sub = input->frags[name];
		
	if (sub.expands) {
		printf(
			"multiple after expand "
				"of [%s]\n",
			sub.name.c_str()
		);
	}
;
		++sub.multiples;
		frag->add(&sub);
		processed = true;
	}

	if (openCh == 'G') {
		ASSERT(frag, "multiple not in frag");
		
	if (! buffer.empty()) {
		frag->add(
			buffer,
			input->name, bufferLine
		);
		buffer.clear();
	}
;
		Frag &sub = frags->get(name, root);
		
	if (sub.expands) {
		printf(
			"multiple after expand "
				"of [%s]\n",
			sub.name.c_str()
		);
	}
;
		++sub.multiples;
		frag->add(&sub);
		processed = true;
	}

	if (openCh == 'p') {
		ASSERT(frag, "private not in frag");
		
	Hash h;
	h.add(input->name);
	unsigned cur = h.add(name);

	static char hash[12];
	char *end = hash + sizeof(hash);
	char *head = end;
	*--head = '_';
	for (;;) {
		ASSERT(head > hash);
		*--head = (cur % 10) + '0';
		cur /= 10;
		if (! cur) { break; }
	}

	
	if (! buffer.empty()) {
		frag->add(
			buffer,
			input->name, bufferLine
		);
		buffer.clear();
	}
;
	static char prefix[] = "_private_";
	frag->add(
		prefix, input->name, nameLine
	);
	frag->add(
		std::string(head, end),
		input->name, nameLine
	);
	frag->add(
		name,
		input->name, nameLine
	);
;
		processed = true;
	}

	if (openCh == 'm') {
		ASSERT(frag, "magic not in frag");
		
	Hash h;
	h.add(input->name);
	unsigned cur = h.add(name);

	static char magic[12];
	char *end = magic + sizeof(magic);
	char *head = end;
	for (;;) {
		ASSERT(head > magic);
		*--head = (cur % 10) + '0';
		cur /= 10;
		if (! cur) { break; }
	}
	
	if (! buffer.empty()) {
		frag->add(
			buffer,
			input->name, bufferLine
		);
		buffer.clear();
	}
;
	frag->add(
		std::string(head, end),
		input->name, nameLine
	);
;
		processed = true;
	}

	if (! processed) {
		ASSERT(
			frag, "unknown frag %s",
			name.c_str()
		);
		for (auto &c : name) {
			if (buffer.empty()) {
				bufferLine = input->line;
			}
			buffer.push_back(c);
		}
		processed = true;
	}
;
		name.clear(); useName = false;
		last = ch;
		ch = nextCh();
	}
} 
	if (frag && ! processed) {
		if (buffer.empty()) {
			bufferLine = input->line;
		}
		buffer.push_back(last);
	}
;
			break;
		}
		default:
			 {
	if (useName || ! name.empty()) {
		name.push_back(ch);
		break;
	}
}  {
	if (frag) {
		if (buffer.empty()) {
			bufferLine = input->line;
		}
		buffer.push_back(last);
	}
} ;
	}
;
		last = ch; ch = nextCh();
	}
} ;
	 {
	for (auto &i : root.map) {
		Frag *frag = &i.second;
		 {
	static const std::string prefix { "file: " };
	if (frag->name.substr(0, prefix.size()) == prefix) {
		++frag->expands;
		
	FILE *f =
		fopen(frag->name.substr(6).c_str(), "w");
	ASSERT(
		f, "can't open %s",
		frag->name.substr(6).c_str()
	);
	serializeFrag(*frag, f, false);
	fclose(f);
;
	}
}  {
	int sum =
		frag->expands + frag->multiples;
	if (sum <= 0) {
		printf(
			"frag [%s] not called\n",
			frag->name.c_str()
		);
	}
} 
	if (frag->multiples == 1) {
		printf(
			"multiple frag [%s] only "
				"used once\n",
			frag->name.c_str()
		);
	}

	if (! isPopulatedFrag(frag)) {
		printf(
			"frag [%s] not populated\n",
			frag->name.c_str()
		);
	}
;
	}
}  {
	for (auto &j : used)
	{
		for (auto &i : j->frags.map) {
			Frag *frag = &i.second;
			 {
	static const std::string prefix { "file: " };
	if (frag->name.substr(0, prefix.size()) == prefix) {
		++frag->expands;
		
	FILE *f =
		fopen(frag->name.substr(6).c_str(), "w");
	ASSERT(
		f, "can't open %s",
		frag->name.substr(6).c_str()
	);
	serializeFrag(*frag, f, false);
	fclose(f);
;
	}
}  {
	int sum =
		frag->expands + frag->multiples;
	if (sum <= 0) {
		printf(
			"frag [%s] not called\n",
			frag->name.c_str()
		);
	}
} 
	if (frag->multiples == 1) {
		printf(
			"multiple frag [%s] only "
				"used once\n",
			frag->name.c_str()
		);
	}

	if (! isPopulatedFrag(frag)) {
		printf(
			"frag [%s] not populated\n",
			frag->name.c_str()
		);
	}
;
		}
	}
} ;
	
	for (auto &cur : used) {
		
	const std::string &name = cur->name;
	std::string outPath =
		name.substr(0, name.size() - 2) +
		".html";
	FILE *out = fopen(outPath.c_str(), "w");
	ASSERT(out);
	 
	FILE *in = fopen(cur->name.c_str(), "r");
	ASSERT(in);
	 {
	HtmlStatus status;
	bool newline = true;
	std::string ident;
	for (;;) {
		int ch = fgetc(in);
		 
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
			 
	fprintf(out, "<!doctype html>\n");
	fprintf(out, "<html lang=\"de\"l>\n");
	fprintf(out, "<head>\n");
	 
	fprintf(
		out, "<meta charset=\"utf-8\">\n"
	);
	fprintf(out, "<title>");
	writeEscaped(
		out,
		status.headerName.begin(),
		status.headerName.end()
	);
	fprintf(out, "</title>");
	fprintf(
		out, "<link rel=\"stylesheet\" "
		"type=\"text/css\" href=\"%s\">",
		stylesheet.c_str()
	);
;
	fprintf(out, "</head>\n");
	fprintf(out, "<body>\n");
;
			break;
		}
		case HtmlState::inSlide: {
			fprintf(out, "</div>\n");
			fprintf(out, "</div>\n");
			break;
		}
		default: {
			fprintf(out, "</div>\n");
		}
	}
;
	 
	fprintf(out, "<h%d>", status.headerLevel);
	writeEscaped(
		out,
		status.headerName.begin(),
		status.headerName.end()
	);
	fprintf(out, "</h%d>\n", status.headerLevel);
;
	fprintf(out, "<div class=\"slides\">\n");
	fprintf(out, "<div><div>\n");
	 
	fprintf(out, "<h%d>", status.headerLevel);
	writeEscaped(
		out,
		status.headerName.begin(),
		status.headerName.end()
	);
	fprintf(out, "</h%d>\n", status.headerLevel);
;
	fprintf(out, "</div>\n");
;
			 
	status.state = HtmlState::inSlide;
	status.headerLevel = 0;
	status.headerName.clear();
	status.headerState = HtmlState::inSlide;
;
			
	newline = ch == '\n';
	if (status.state != HtmlState::inHeader) {
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
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
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
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
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
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
		fprintf(out, "</div>\n");
	}
	fprintf(out, "<div><div>\n");
	fprintf(out, "<code>\n");
	status.state = HtmlState::inCode;
	
	newline = ch == '\n';
	if (status.state != HtmlState::inHeader) {
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
	}
;
;
			continue;
		} else if (
			status.state == HtmlState::inCode
		) {
			
	fprintf(out, "</code>\n");
	fprintf(out, "</div>\n");
	status.state = HtmlState::inSlide;
	status.codeIndent = 0;
	status.codeSpecial = '\0';
	
	newline = ch == '\n';
	if (status.state != HtmlState::inHeader) {
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
	}
;
;
			continue;
		}
	}

	if (status.codeOpening == 1) {
		if (! status.codeSpecial && status.state == HtmlState::inCode) {
			status.codeSpecial = '`';
			status.codeNameEnd = status.codeName;
			
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
				fprintf(
					out,
					"<span class=\"in%d\"></span>",
					status.codeIndent
				);
				status.codeIndent = 0;
			}
			fprintf(out, "<span class=\"str\">`");
		} else if (
			status.codeSpecial == '`' && status.codeNameEnd[-1] != '\x5c'
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
			writeEscaped(out, status.codeName, status.codeNameEnd);
			fprintf(out, "`</span>");
			status.codeSpecial = 0;
			status.codeNameEnd = nullptr;
		}
	}

	status.codeOpening = 0;

	if (status.state == HtmlState::inCode) {
		if (ch == EOF) {
			fprintf(
				stderr,
				"unterminated code block\n"
			);
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
		fprintf(out, "<br/>\n");
		
	newline = ch == '\n';
	if (status.state != HtmlState::inHeader) {
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
	}
;
		continue;
	}
 
	if (newline && ch == '\t') {
		++status.codeIndent;
		continue;
	}
 
	if (status.codeIndent) {
		fprintf(
			out,
			"<span class=\"in%d\"></span>",
			status.codeIndent
		);
		status.codeIndent = 0;
	}

	if (! status.codeSpecial && (ch == '\'' || ch == '"' || ch == '`')) {
		status.codeSpecial = ch;
		status.codeNameEnd = status.codeName;
		fprintf(out, "<span class=\"str\">%c", ch);
		continue;
	}

	
	if (ch == '{' && ident.size() == 1) {
		char lc = ident.front();
		switch (lc) {
			
	case 'd':
		fprintf(out,
			"<span class=\"add\">@def("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'D':
		fprintf(out,
			"<span class=\"add\">@globdef("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'a':
		fprintf(out,
			"<span class=\"add\">@add("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'A':
		fprintf(out,
			"<span class=\"add\">@globadd("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'r':
		fprintf(out,
			"<span class=\"add\">@replace("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'R':
		fprintf(out,
			"<span class=\"add\">@globreplace("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'x':
		fprintf(out,
			"<span class=\"end\">@end("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'e':
		fprintf(out,
			"<span class=\"expand\">@expand("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'E':
		fprintf(out,
			"<span class=\"expand\">@multiple("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'g':
		fprintf(out,
			"<span class=\"expand\">"
				"@globexpand("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'G':
		fprintf(out,
			"<span class=\"expand\">"
				"@globmult("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'i':
		fprintf(out,
			"<span class=\"include\">@include("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 't':
		fprintf(out, "<span class=\"type\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'v':
		fprintf(out, "<span class=\"var\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'f':
		fprintf(out, "<span class=\"fn\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'k':
		fprintf(out, "<span class=\"keyword\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 's':
		fprintf(out, "<span class=\"str\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'n':
		fprintf(out, "<span class=\"num\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'p':
		fprintf(out,
			"<span class=\"type\">"
				"@priv(<span>"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'm':
		fprintf(
			out,
			"<span class=\"num\">"
				"@magic(<span>"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'b':
		fprintf(out, "<span class=\"virt\"></span><br/>");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
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
			ch == status.codeSpecial &&
			status.codeNameEnd[-1] != '\x5c'
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
			writeEscaped(out, status.codeName, status.codeNameEnd);
		}
		switch (status.codeSpecial) {
			
	case 'i': {
		
	ASSERT(
		status.codeNameEnd <
		status.codeName +
			sizeof(status.codeName)
	);
	*status.codeNameEnd = '\0';
	while (
		status.codeNameEnd >= status.codeName
		&& *status.codeNameEnd != '.'
	) {
		--status.codeNameEnd;
	}

	ASSERT(
		status.codeNameEnd >= status.codeName,
		"no period"
	);
	*status.codeNameEnd = '\0';
	fprintf(out,
		"<a href=\"%s.html\">",
		status.codeName
	);
	*status.codeNameEnd = '.';
	fprintf(out,
		"%s</a>)</span>", status.codeName
	);
	status.codeNameEnd = nullptr;
;
		break;
	}
	case 'a': case 'e': case 'E': case 'x':
	case 'g': case 'G': case 'A': case 'D':
	case 'R':
	case 'r': case 'd': case 'p': case 'm': {
		fprintf(out, "</span>)");
		break;
	}
	case '\'': case '"': case '`': {
		fputc(status.codeSpecial, out);
		break;
	}

		}
		if (status.codeSpecial != 'b') {
			fprintf(out, "</span>");
		}
		status.codeSpecial = 0;
		status.codeNameEnd = nullptr;
		continue;
	}
;

	if (ch != EOF && status.codeNameEnd) {
		ASSERT(
			status.codeNameEnd <
				status.codeName +
					sizeof(status.codeName),
			" [%c], [%.*s]", status.codeSpecial,
			(int) sizeof(status.codeName), status.codeName
		);
		*status.codeNameEnd++ = ch;
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
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
	}
;
		continue;
	}
 
	if (
		newline &&
		status.state == HtmlState::inNotes
	) {
		if (ch == '*') {
			fprintf(out, "</li><li>\n");
			ident.clear();
			newline = false;
			continue;
		} else if (ch != ' ' && ch != '\t') {
			fprintf(out, "</li></ul></div>\n");
			status.state = HtmlState::afterSlide;
			
	newline = ch == '\n';
	if (status.state != HtmlState::inHeader) {
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
	}
;
			continue;
		}
	}
 
	if (newline && ch == '*') {
		if (isOutOfHtmlSpecial(&status)) {
			if (status.state != HtmlState::inSlide) {
				fprintf(out, "<div>\n");
			}
			status.state = HtmlState::inNotes;
			fprintf(out, "<ul><li>\n");
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
			fprintf(out, "</code>");
		} else {
			fprintf(out, "<code>");
		}
		status.noteInCode = ! status.noteInCode;
		ident.clear();
		newline = false;
		continue;
	}
 
	if (status.state == HtmlState::inNotes &&
	status.noteInCode) {
		
	if (ch == '{' && ident.size() == 1) {
		char lc = ident.front();
		switch (lc) {
			
	case 'd':
		fprintf(out,
			"<span class=\"add\">@def("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'D':
		fprintf(out,
			"<span class=\"add\">@globdef("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'a':
		fprintf(out,
			"<span class=\"add\">@add("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'A':
		fprintf(out,
			"<span class=\"add\">@globadd("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'r':
		fprintf(out,
			"<span class=\"add\">@replace("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'R':
		fprintf(out,
			"<span class=\"add\">@globreplace("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'x':
		fprintf(out,
			"<span class=\"end\">@end("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'e':
		fprintf(out,
			"<span class=\"expand\">@expand("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'E':
		fprintf(out,
			"<span class=\"expand\">@multiple("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'g':
		fprintf(out,
			"<span class=\"expand\">"
				"@globexpand("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'G':
		fprintf(out,
			"<span class=\"expand\">"
				"@globmult("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'i':
		fprintf(out,
			"<span class=\"include\">@include("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 't':
		fprintf(out, "<span class=\"type\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'v':
		fprintf(out, "<span class=\"var\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'f':
		fprintf(out, "<span class=\"fn\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'k':
		fprintf(out, "<span class=\"keyword\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 's':
		fprintf(out, "<span class=\"str\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'n':
		fprintf(out, "<span class=\"num\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'p':
		fprintf(out,
			"<span class=\"type\">"
				"@priv(<span>"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'm':
		fprintf(
			out,
			"<span class=\"num\">"
				"@magic(<span>"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

	case 'b':
		fprintf(out, "<span class=\"virt\"></span><br/>");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
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
			ch == status.codeSpecial &&
			status.codeNameEnd[-1] != '\x5c'
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
			writeEscaped(out, status.codeName, status.codeNameEnd);
		}
		switch (status.codeSpecial) {
			
	case 'i': {
		
	ASSERT(
		status.codeNameEnd <
		status.codeName +
			sizeof(status.codeName)
	);
	*status.codeNameEnd = '\0';
	while (
		status.codeNameEnd >= status.codeName
		&& *status.codeNameEnd != '.'
	) {
		--status.codeNameEnd;
	}

	ASSERT(
		status.codeNameEnd >= status.codeName,
		"no period"
	);
	*status.codeNameEnd = '\0';
	fprintf(out,
		"<a href=\"%s.html\">",
		status.codeName
	);
	*status.codeNameEnd = '.';
	fprintf(out,
		"%s</a>)</span>", status.codeName
	);
	status.codeNameEnd = nullptr;
;
		break;
	}
	case 'a': case 'e': case 'E': case 'x':
	case 'g': case 'G': case 'A': case 'D':
	case 'R':
	case 'r': case 'd': case 'p': case 'm': {
		fprintf(out, "</span>)");
		break;
	}
	case '\'': case '"': case '`': {
		fputc(status.codeSpecial, out);
		break;
	}

		}
		if (status.codeSpecial != 'b') {
			fprintf(out, "</span>");
		}
		status.codeSpecial = 0;
		status.codeNameEnd = nullptr;
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
			fprintf(out, "</b>");
		} else {
			fprintf(out, "<b>");
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
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
	}
;
		continue;
	}
;
		if (ch == EOF) { break; }
		
	newline = ch == '\n';
	if (status.state != HtmlState::inHeader) {
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
	}
;
	}
} ;
	fclose(in);
;
	fclose(out);
;
	}
	used.clear();
;

	}
