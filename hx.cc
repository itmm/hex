
	
	
	#include <stdio.h>
	#include <stdlib.h>

	#include <stdlib.h>

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
	
	struct FragEntry;
	void freeFragEntry(
		struct FragEntry *entry
	);
;

	
	static inline unsigned initHash() {
		return 0x3d9a73b5;
	}

	template <typename T>
	unsigned addToHash(
		unsigned hash,
		T begin, T end
	) {
		for (; begin < end; ++begin) {
			
	hash ^= *begin;
	hash = (hash << 3) | (hash >> 29);
;
		}
		return hash;
	}

	int calcHash(
		const char *begin,
		const char *end
	) {
		unsigned hash = initHash();
		hash = addToHash(
			hash, begin, end
		);
		return hash & 0x7ffffff;
	}
;

	
	struct FragEntry;

	struct Frag {
		struct Frag *link;
		struct FragEntry *firstEntry;
		struct FragEntry *lastEntry;
		int expands;
		int multiples;
		std::string name;
	};

	struct Frag *allocFrag(
		const char *nameBegin,
		const char *nameEnd
	) {
		struct Frag *result = nullptr;
		
	ASSERT(nameBegin);
	ASSERT(nameBegin <= nameEnd);
	result = new Frag();
	ASSERT(result);
;
		result->link = nullptr;
		result->firstEntry = nullptr;
		result->expands = 0;
		result->multiples = 0;
		
	result->name = std::string(nameBegin, nameEnd);
;
		return result;
	}

	void freeFragEntries(
		struct Frag *frag
	) {
		if (frag) {
			
	freeFragEntry(frag->firstEntry);
;
			frag->firstEntry = nullptr;
			frag->lastEntry = nullptr;
		}
	}

	void freeFrag(
		struct Frag *f
	) {
		while (f) {
			struct Frag *l =
				f->link;
			freeFragEntries(f);
			delete(f);
			f = l;
		}
	}

	struct Frag *allocTestFrag(
		const char *name
	) {
		return allocFrag(
			name, name + strlen(name)
		);
	}

	void testFragName(
		const char *name
	) {
		struct Frag *f =
			allocTestFrag(name);
		ASSERT(f);
		ASSERT(f->name == name);
		freeFrag(f);
	}

	bool isPopulatedFrag(
		const struct Frag *f
	) {
		return f && f->firstEntry;
	}

	struct FragEntry {
		struct FragEntry *link;
		struct Frag *frag;
		std::string value;
		
	std::string source;
	int line;
;
	};

	struct FragEntry *allocFragEntry(
		struct Frag *frag,
		const char *valueBegin,
		const char *valueEnd
	) {
		struct FragEntry *result = nullptr;
		
	result = new FragEntry();
;
		result->link = nullptr;
		
	if (valueBegin) {
		result->value = std::string(valueBegin, valueEnd);
	}
	result->frag = frag;
;
		return result;
	}

	struct FragEntry *
		allocEmptyFragEntry() {
			return allocFragEntry(
				nullptr, nullptr, nullptr
			);
		}

	void freeFragEntry(
		struct FragEntry *e
	) {
		while (e) {
			struct FragEntry *l =
				e->link;
			delete(e);
			e = l;
		}
	}

	int getFragEntryValueSize(
		struct FragEntry *e
	) {
		if (! e) {
			return 0;
		}
		return e->value.size();
	}

	struct FragEntry *
		allocTestFragEntry(
			const char *v
		) {
			const char *e = v +
				strlen(v);

			return allocFragEntry(
				nullptr, v, e
			);
		}

	void addEntryToFrag(
		struct Frag *frag,
		struct FragEntry *entry
	) {
		
	ASSERT(frag);
	ASSERT(entry);
	ASSERT(! entry->link);
;
		if (frag->firstEntry) {
			
	frag->lastEntry->link = entry;
	frag->lastEntry = entry;
;
		} else {
			
	frag->firstEntry = entry;
	frag->lastEntry = entry;
;
		}
	}

	void addBytesToFrag(
		struct Frag *frag,
		const char *value,
		const char *valueEnd,
		const std::string &source,
		int line
	) {
		struct FragEntry *entry =
			allocFragEntry(
				nullptr, value, valueEnd
			);
		
	entry->source = source;
	entry->line = line;
;
		addEntryToFrag(frag, entry);
	}

	
	bool isFragInFrag(
		struct Frag *needle,
		struct Frag *haystack
	) {
		ASSERT(needle);
		ASSERT(haystack);
		
	if (needle == haystack) {
		return true;
	}
;
		
	struct FragEntry *entry =
		haystack->firstEntry;
	for (; entry; entry = entry->link) {
		if (! entry->frag) { continue; }
		if (isFragInFrag(
			needle, entry->frag
		)) {
			return true;
		}
	}
;
		return false;
	}

	void addFragToFrag(
		struct Frag *frag,
		struct Frag *child
	) {
		ASSERT(frag);
		ASSERT(child);
		
	ASSERT(! isFragInFrag(
		frag, child
	));
;
		
	if (frag->firstEntry &&
		! frag->lastEntry->frag
	) {
		frag->lastEntry->frag = child;
		return;
	}
;
		
	struct FragEntry *entry =
		allocFragEntry(
			child, nullptr, nullptr
		);
	addEntryToFrag(frag, entry);
;
	}

	
	std::string *fragTestBufferCur = nullptr;
;
	void serializeFrag(
		struct Frag *frag,
		FILE *out,
		bool writeLineMacros
	) {
		ASSERT(frag);
		ASSERT(out);
		
	struct FragEntry *entry =
		frag->firstEntry;
	for (; entry; entry = entry->link) {
		
	if (getFragEntryValueSize(entry)) {
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
				entry->frag, out,
				writeLineMacros
			);
		}
	}
;
	}

	void testFrag(struct Frag *
		frag, const char *expected
	) {
		
	std::string buffer;
	fragTestBufferCur = &buffer;
	serializeFrag(frag, (FILE *) &buffer, false);
	ASSERT(buffer == expected);
	fragTestBufferCur = nullptr;
;
	}

	void addStringToFrag(
		struct Frag *frag,
		const char *str
	) {
		int size = strlen(str);
		addBytesToFrag(
			frag, str, str + size,
			std::string(), 0
		);
	}

	#define FRAG_SLOTS 128

	struct FragMap {
		struct FragMap *link;
		struct Frag *frags[
			FRAG_SLOTS
		];
	};

	void clearFragMap(
		struct FragMap *map
	) {
		struct Frag **cur = map->frags;
		struct Frag **end =
			cur + FRAG_SLOTS;
		for (; cur < end; ++cur) {
			freeFrag(*cur); *cur = nullptr;
		}
		map->link = nullptr;
	}

	int calcFragHash(
		const char *begin, const char *end
	) {
		int hash = calcHash(begin, end);
		return hash % FRAG_SLOTS;
	}

	struct Frag *allocFragInMap(
		struct FragMap *map,
		const char *begin,
		const char *end
	) {
		ASSERT(map);
		struct Frag *frag =
			allocFrag(begin, end);
		
	int hash = calcFragHash(begin, end);
	frag->link = map->frags[hash];
	map->frags[hash] = frag;
;
		return frag;
	}

	struct Frag *findFragInMap(
		struct FragMap *map,
		const char *begin,
		const char *end
	) {
		ASSERT(map);
		 {
	int hash = calcFragHash(begin, end);
	struct Frag *frag = map->frags[hash];
	std::string s(begin, end);
	for (; frag; frag = frag->link) {
		if (s == frag->name) {
			return frag;
		}
	}
} ;
		
	if (map->link) {
		return findFragInMap(
			map->link, begin, end
		);
	}
;
		return nullptr;
	}

	struct Frag *getFragInMap(
		struct FragMap *map,
		const char *begin,
		const char *end,
		struct FragMap *insert
	) {
		struct Frag *frag = nullptr;
		
	frag = findFragInMap(
		map, begin, end
	);
	if (frag) {
		return frag;
	}
;
		
	frag = allocFragInMap(
		map, begin, end
	);
;
		return frag;
	}
;

	struct Input {
		struct Input *link;
		FILE *file;
		
	struct FragMap frags;

	int line;
;
		std::string name;
	};

	struct Input *input = nullptr;
	struct Input *used = nullptr;

	struct FragMap root = {};
	struct FragMap *frags = &root;

	void pushPath(const char *path) {
		FILE *f = fopen(path, "r");
		
	ASSERT(
		f, "can't open [%s]", path
	);
;
		struct Input *i = new Input();
		i->link = input;
		i->file = f;
		i->name = path;
		
	if (input) {
		input->frags.link = frags;
		frags = &input->frags;
	}

	memset(
		&i->frags, 0,
		sizeof(i->frags)
	);

	i->line = 1;
;
		input = i;
	}

	const char *stylesheet =
		"slides/slides.css";

	int nextCh() {
		int ch = EOF;
		while (input) {
			ch = fgetc(input->file);
			
	if (ch == '\n') {
		++input->line;
	}
;
			if (ch != EOF) { break; }
			
	struct Input *n = input->link;
	fclose(input->file);
	input->link = used;
	used = input;
	input = n;
	struct FragMap *nxt = frags->link;
	frags->link = nullptr;
	frags = nxt;
;
		}
		return ch;
	}

	bool alreadyUsed(const char *name) {
		struct Input *i = input;
		for (; i; i = i->link) {
			if (i->name == name) {
				return true;
			}
		}
		for (i = used; i; i = i->link) {
			if (i->name == name) {
				return true;
			}
		}
		return false;
	}

	enum HtmlState {
		hs_NOTHING_WRITTEN,
		hs_IN_SLIDE,
		hs_AFTER_SLIDE
		
	, hs_IN_HEADER

	, hs_IN_CODE

	, hs_IN_NOTES

	};

	struct HtmlStatus {
		
	HtmlStatus();
	enum HtmlState state;

	int headerLevel;
	std::string headerName;
	enum HtmlState headerState;

	int codeOpening;

	int codeIndent;

	char codeSpecial;
	char codeName[100];
	char *codeNameEnd;

	bool noteInCode;
	bool noteInBold;

	};

	inline HtmlStatus::HtmlStatus():
		state(hs_NOTHING_WRITTEN)
		
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
		struct HtmlStatus *s
	) {
		
	if (s->state == hs_IN_HEADER) {
		return false;
	}

	if (s->state == hs_IN_CODE) {
		return false;
	}

	if (s->state == hs_IN_NOTES) {
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
			s.size(),
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
		int argc, const char **argv
	) {
		
	
	
	testFragName("abc");
	testFragName("");
	testFragName("A c");
	{
		struct Frag *f =
			allocTestFrag("ab");
		ASSERT(f);
		ASSERT(! f->link);
		ASSERT(! f->firstEntry);
		freeFrag(f);
	}

	{
		struct FragEntry *entry =
			allocEmptyFragEntry();

		ASSERT(entry);
		ASSERT(! entry->link);
		ASSERT(! entry->frag);

		freeFragEntry(entry);
	}

	{
		struct FragEntry *entry =
			allocEmptyFragEntry();

		ASSERT(entry);
		ASSERT(
			getFragEntryValueSize(
				entry) == 0
		);

		freeFragEntry(entry);
	}

	{
		struct FragEntry *entry =
			allocTestFragEntry("abc");

		ASSERT(entry);
		ASSERT(
			getFragEntryValueSize(
				entry) == 3
		);

		freeFragEntry(entry);
	}

	{
		struct FragEntry *entry =
			allocTestFragEntry("abc");

		ASSERT(entry);
		ASSERT(entry->value == "abc");

		freeFragEntry(entry);
	}

	{
		struct Frag *f =
			allocTestFrag("");
		struct FragEntry *e =
			allocEmptyFragEntry();
		addEntryToFrag(f, e);
		ASSERT(
			f->firstEntry == e
		);
		freeFrag(f);
	}

	{
		struct Frag *f =
			allocTestFrag("");
		struct FragEntry *e =
			allocEmptyFragEntry();
		addEntryToFrag(f, e);
		ASSERT(
			f->lastEntry == e
		);
		freeFrag(f);
	}

	{
		struct Frag *frag = nullptr;
		struct FragEntry *first;
		struct FragEntry *second;
		
	frag = allocTestFrag("");
	first = allocEmptyFragEntry();
	second = allocEmptyFragEntry();

	addEntryToFrag(frag, first);
	addEntryToFrag(frag, second);
;
		
	ASSERT(frag->firstEntry == first);
;
		freeFrag(frag);
	}

	{
		struct Frag *frag = nullptr;
		struct FragEntry *first;
		struct FragEntry *second;
		
	frag = allocTestFrag("");
	first = allocEmptyFragEntry();
	second = allocEmptyFragEntry();

	addEntryToFrag(frag, first);
	addEntryToFrag(frag, second);
;
		ASSERT(
			frag->lastEntry == second
		);
		freeFrag(frag);
	}

	{
		struct Frag *frag =
			allocTestFrag("");
		addStringToFrag(frag, "abc");
		addStringToFrag(frag, "def");
		testFrag(frag, "abcdef");
		freeFrag(frag);
	}
 {
	struct Frag *a =
		allocTestFrag("");
	struct Frag *b =
		allocTestFrag("");
	addStringToFrag(a, "abc");
	addFragToFrag(b, a);
	addStringToFrag(b, "def");
	addFragToFrag(b, a);
	testFrag(b, "abcdefabc");
	freeFrag(a); freeFrag(b);
} ;
;
	
	bool someFile = false;
	for (int i = 1; i < argc; ++i) {
		 {
	const char prefix[] = "--css=";
	int len = sizeof(prefix) - 1;
	if (memcmp(argv[i], prefix, len) == 0) {
		stylesheet = argv[i] + len;
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
		
	struct Frag *frag = nullptr;
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
		struct FragMap *fm = &input->frags;
		
	frag = findFragInMap(
		fm, name.data(),
		name.data() + name.size()
	);
	if (isPopulatedFrag(frag)) {
		printf(
			"frag [%s] already defined\n",
			name.c_str()
		);
	}
;
		if (! frag) {
			frag = allocFragInMap(
				fm, name.data(),
				name.data() + name.size()
			);
		}
		processed = true;
	}

	if (openCh == 'D') {
		ASSERT(! frag, "def in frag");
		struct FragMap *fm = frags;
		
	frag = findFragInMap(
		fm, name.data(),
		name.data() + name.size()
	);
	if (isPopulatedFrag(frag)) {
		printf(
			"frag [%s] already defined\n",
			name.c_str()
		);
	}
;
		if (! frag) {
			frag = allocFragInMap(
				&root, name.data(),
				name.data() + name.size()
			);
		}
		processed = true;
	}

	if (openCh == 'a') {
		ASSERT(! frag, "add in frag");
		struct FragMap *fm = &input->frags;
		struct FragMap *ins = fm;
		frag = findFragInMap(
			fm, name.data(),
			name.data() + name.size()
		);
		
	if (! isPopulatedFrag(frag)) {
		printf(
			"frag [%s] not defined\n",
			name.c_str()
		);
		frag = getFragInMap(
			fm, name.data(),
			name.data() + name.size(),
			ins
		);
	}
;
		processed = true;
	}

	if (openCh == 'A') {
		ASSERT(! frag, "add in frag");
		struct FragMap *fm = frags;
		struct FragMap *ins = &root;
		frag = findFragInMap(
			fm, name.data(),
			name.data() + name.size()
		);
		
	if (! isPopulatedFrag(frag)) {
		printf(
			"frag [%s] not defined\n",
			name.c_str()
		);
		frag = getFragInMap(
			fm, name.data(),
			name.data() + name.size(),
			ins
		);
	}
;
		processed = true;
	}

	if (openCh == 'r') {
		ASSERT(! frag, "replace in frag");
		frag = getFragInMap(
			&input->frags, name.data(),
			name.data() + name.size(), &input->frags
		);
		ASSERT(
			frag, "frag %s not defined",
			name.c_str()
		);
		freeFragEntries(frag);
		processed = true;
	}

	if (openCh == 'R') {
		ASSERT(! frag, "replace in frag");
		frag = getFragInMap(
			frags, name.data(),
			name.data() + name.size(), &root
		);
		ASSERT(
			frag, "frag %s not defined",
			name.c_str()
		);
		freeFragEntries(frag);
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
		addBytesToFrag(
			frag, buffer.data(),
			buffer.data() + buffer.size(),
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
			pushPath(name.c_str());
		}
		processed = true;
	}

	if (openCh == 'e') {
		ASSERT(frag, "expand not in frag");
		
	if (! buffer.empty()) {
		addBytesToFrag(
			frag, buffer.data(),
			buffer.data() + buffer.size(),
			input->name, bufferLine
		);
		buffer.clear();
	}
;
		struct Frag *sub = getFragInMap(
			&input->frags, name.data(),
			name.data() + name.size(), &input->frags
		);
		
	if (sub->expands) {
		printf(
			"multiple expands of [%s]\n",
			sub->name.c_str()
		);
	}
	if (sub->multiples) {
		printf(
			"expand after mult of [%s]\n",
			sub->name.c_str()
		);
	}
;
		++sub->expands;
		addFragToFrag(frag, sub);
		processed = true;
	}

	if (openCh == 'g') {
		ASSERT(frag, "expand not in frag");
		
	if (! buffer.empty()) {
		addBytesToFrag(
			frag, buffer.data(),
			buffer.data() + buffer.size(),
			input->name, bufferLine
		);
		buffer.clear();
	}
;
		struct Frag *sub = getFragInMap(
			frags, name.data(),
			name.data() + name.size(), &root
		);
		
	if (sub->expands) {
		printf(
			"multiple expands of [%s]\n",
			sub->name.c_str()
		);
	}
	if (sub->multiples) {
		printf(
			"expand after mult of [%s]\n",
			sub->name.c_str()
		);
	}
;
		++sub->expands;
		addFragToFrag(frag, sub);
		processed = true;
	}

	if (openCh == 'E') {
		ASSERT(frag, "multiple not in frag");
		
	if (! buffer.empty()) {
		addBytesToFrag(
			frag, buffer.data(),
			buffer.data() + buffer.size(),
			input->name, bufferLine
		);
		buffer.clear();
	}
;
		struct Frag *sub =
			getFragInMap(
				&input->frags, name.data(),
				name.data() + name.size(), &input->frags
			);
		
	if (sub->expands) {
		printf(
			"multiple after expand "
				"of [%s]\n",
			sub->name.c_str()
		);
	}
;
		++sub->multiples;
		addFragToFrag(
			frag, sub);
		processed = true;
	}

	if (openCh == 'G') {
		ASSERT(frag, "multiple not in frag");
		
	if (! buffer.empty()) {
		addBytesToFrag(
			frag, buffer.data(),
			buffer.data() + buffer.size(),
			input->name, bufferLine
		);
		buffer.clear();
	}
;
		struct Frag *sub =
			getFragInMap(
				frags, name.data(),
				name.data() + name.size(), &root
			);
		
	if (sub->expands) {
		printf(
			"multiple after expand "
				"of [%s]\n",
			sub->name.c_str()
		);
	}
;
		++sub->multiples;
		addFragToFrag(
			frag, sub);
		processed = true;
	}

	if (openCh == 'p') {
		ASSERT(frag, "private not in frag");
		
	unsigned cur = initHash();
	cur = addToHash(
		cur, input->name.begin(), input->name.end()
	);
	cur = addToHash(
		cur, name.begin(), name.end()
	);
	cur &= 0x7fffffff;

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
		addBytesToFrag(
			frag, buffer.data(),
			buffer.data() + buffer.size(),
			input->name, bufferLine
		);
		buffer.clear();
	}
;
	static char prefix[] = "_private_";
	addBytesToFrag(
		frag, prefix,
		prefix + sizeof(prefix) - 1,
		input->name, nameLine
	);
	addBytesToFrag(
		frag, head, end,
		input->name, nameLine
	);
	addBytesToFrag(
		frag, name.data(), name.data() + name.size(),
		input->name, nameLine
	);
;
		processed = true;
	}

	if (openCh == 'm') {
		ASSERT(frag, "magic not in frag");
		
	unsigned cur = initHash();
	cur = addToHash(
		cur, input->name.begin(), input->name.end()
	);
	cur = addToHash(
		cur, name.begin(), name.end()
	);
	cur &= 0x7fffffff;

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
		addBytesToFrag(
			frag, buffer.data(),
			buffer.data() + buffer.size(),
			input->name, bufferLine
		);
		buffer.clear();
	}
;
	addBytesToFrag(
		frag, head, end,
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
		const char *c = name.data();
		const char *e = c + name.size();
		for (; c != e; ++c) {
			if (buffer.empty()) {
				bufferLine = input->line;
			}
			buffer.push_back(*c);
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
	}
;
	 {
	struct Frag **cur = root.frags;
	struct Frag **end =
		cur + FRAG_SLOTS;
	for (; cur < end; ++cur) {
		struct Frag *frag = *cur;
		for (; frag; frag = frag->link) {
			
	if (! memcmp(
		"file: ", frag->name.data(), 6
	)) {
		++frag->expands;
		
	FILE *f =
		fopen(frag->name.substr(6).c_str(), "w");
	ASSERT(
		f, "can't open %s",
		frag->name.substr(6).c_str()
	);
	serializeFrag(frag, f, false);
	fclose(f);
;
	}
 {
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
}  {
	input = used;
	for (; input; input = input->link)
	{
		struct Frag **cur =
			input->frags.frags;
		struct Frag **end =
			cur + FRAG_SLOTS;
		for (; cur < end; ++cur) {
			struct Frag *frag = *cur;
			while (frag) {
				
	if (! memcmp(
		"file: ", frag->name.data(), 6
	)) {
		++frag->expands;
		
	FILE *f =
		fopen(frag->name.substr(6).c_str(), "w");
	ASSERT(
		f, "can't open %s",
		frag->name.substr(6).c_str()
	);
	serializeFrag(frag, f, false);
	fclose(f);
;
	}
 {
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
				frag = frag->link;
			}
		}
	}
} ;
	
	struct Input *cur = used;
	while (cur) {
		
	std::string &name = cur->name;
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
			status.state == hs_IN_HEADER
		) {
			++status.headerLevel;
			if (status.state != hs_IN_HEADER) {
				status.headerState =
					status.state;
			}
			status.state = hs_IN_HEADER;
			continue;
		}
	}
 
	if (status.state == hs_IN_HEADER) {
		if (ch == '\n') {
			 
	ASSERT(! status.headerName.empty());
	 
	switch (status.headerState) {
		case hs_NOTHING_WRITTEN: {
			 
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
		stylesheet
	);
;
	fprintf(out, "</head>\n");
	fprintf(out, "<body>\n");
;
			break;
		}
		case hs_IN_SLIDE: {
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
			 
	status.state = hs_IN_SLIDE;
	status.headerLevel = 0;
	status.headerName.clear();
	status.headerState = hs_IN_SLIDE;
;
			
	newline = ch == '\n';
	if (status.state != hs_IN_HEADER) {
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
	}
;
			continue;
		}
	}
 
	if (status.state == hs_IN_HEADER) {
		if (! status.headerName.empty()) {
			status.headerName.push_back(ch);
			
	newline = ch == '\n';
	if (status.state != hs_IN_HEADER) {
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
	}
;
			continue;
		}
	}
 
	if (status.state == hs_IN_HEADER) {
		if (ch > ' ' && status.headerName.empty()) {
			status.headerName.push_back(ch);
			
	newline = ch == '\n';
	if (status.state != hs_IN_HEADER) {
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
	}
;
			continue;
		}
	}
 
	if (newline && ch == '`') {
		if (isOutOfHtmlSpecial(&status) ||
			status.state == hs_IN_CODE
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
			
	if (status.state == hs_IN_SLIDE) {
		fprintf(out, "</div>\n");
	}
	fprintf(out, "<div><div>\n");
	fprintf(out, "<code>\n");
	status.state = hs_IN_CODE;
	
	newline = ch == '\n';
	if (status.state != hs_IN_HEADER) {
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
	}
;
;
			continue;
		} else if (
			status.state == hs_IN_CODE
		) {
			
	fprintf(out, "</code>\n");
	fprintf(out, "</div>\n");
	status.state = hs_IN_SLIDE;
	status.codeIndent = 0;
	status.codeSpecial = '\0';
	
	newline = ch == '\n';
	if (status.state != hs_IN_HEADER) {
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
	}
;
;
			continue;
		}
	}

	if (status.codeOpening == 1) {
		if (! status.codeSpecial && status.state == hs_IN_CODE) {
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

	if (status.state == hs_IN_CODE) {
		if (ch == EOF) {
			fprintf(
				stderr,
				"unterminated code block\n"
			);
			break;
		}
	}

	if (status.state == hs_IN_CODE) {
		if (! status.codeSpecial && (isalnum(ch) || ch == '_')) {
			ident.push_back(ch);
			continue;
		}
	}

	if (status.state == hs_IN_CODE) {
		 
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
	if (status.state != hs_IN_HEADER) {
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
			sizeof(status.codeName), status.codeName
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
	if (status.state != hs_IN_HEADER) {
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
	}
;
		continue;
	}
 
	if (
		newline &&
		status.state == hs_IN_NOTES
	) {
		if (ch == '*') {
			fprintf(out, "</li><li>\n");
			ident.clear();
			newline = false;
			continue;
		} else if (ch != ' ' && ch != '\t') {
			fprintf(out, "</li></ul></div>\n");
			status.state = hs_AFTER_SLIDE;
			
	newline = ch == '\n';
	if (status.state != hs_IN_HEADER) {
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
	}
;
			continue;
		}
	}
 
	if (newline && ch == '*') {
		if (isOutOfHtmlSpecial(&status)) {
			if (status.state != hs_IN_SLIDE) {
				fprintf(out, "<div>\n");
			}
			status.state = hs_IN_NOTES;
			fprintf(out, "<ul><li>\n");
			ident.clear();
			newline = false;
			continue;
		}
	}
 
	if (
		ch == '`' &&
		status.state == hs_IN_NOTES
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
 
	if (status.state == hs_IN_NOTES &&
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
		status.state == hs_IN_NOTES
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
 
	if (status.state == hs_IN_NOTES) {
		
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
	if (status.state != hs_IN_HEADER) {
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
	}
;
		continue;
	}
;
		if (ch == EOF) { break; }
		
	newline = ch == '\n';
	if (status.state != hs_IN_HEADER) {
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
		struct Input *next = cur->link;
		free(cur);
		cur = next;
	}
;

	}
