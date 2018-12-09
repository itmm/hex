
	
	
	#include <stdio.h>
	#include <stdlib.h>

	#include <stdlib.h>

	#include <string.h>

	#include <stdbool.h>

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

	
	#define INIT_BUFFER_SIZE 16
	struct Buffer {
		char initial[INIT_BUFFER_SIZE];
		char *buffer;
		char *current;
		const char *end;
	};

	static inline void bufferInvariant(
		const struct Buffer *b
	) {
		ASSERT(b);
		if (b->current) {
			ASSERT(b->buffer <= b->current);
			ASSERT(b->current <= b->end);
		}
	}

	static inline bool isActiveBuffer(
		const struct Buffer *b
	) {
		bufferInvariant(b);
		return b->current;
	}

	void activateBuffer(struct Buffer *b) {
		bufferInvariant(b);
		if (! b->buffer) {
			b->buffer = b->initial;
			b->end = b->initial +
				sizeof(b->initial);
		}
		if (! b->current) {
			b->current = b->buffer;
		}
	}

	void addToBuffer(
		struct Buffer *buffer, char ch
	) {
		bufferInvariant(buffer);
		activateBuffer(buffer);
		
	if (buffer->current >= buffer->end) {
		int size = buffer->current -
			buffer->buffer;
		int newSize = 2 * size;
		
	char *newBuffer;
	if (buffer->buffer == buffer->initial) {
		newBuffer = malloc(newSize);
		
	ASSERT(newBuffer);
	memcpy(
		newBuffer, buffer->buffer, size
	);
;
	} else {
		newBuffer = realloc(
			buffer->buffer, newSize
		);
	}
	
	ASSERT(newBuffer);
	buffer->buffer = newBuffer;
	buffer->current = newBuffer + size;
	buffer->end = newBuffer + newSize;
;
;
	}

		*buffer->current++ = ch;
	}

	void resetBuffer(
		struct Buffer *buffer
	) {
		bufferInvariant(buffer);
		buffer->current = NULL;
	}

	void eraseBuffer(
		struct Buffer *buffer
	) {
		bufferInvariant(buffer);
		
	if (buffer->buffer &&
		buffer->buffer != buffer->initial
	) {
		free(buffer->buffer);
	}
;
		buffer->buffer = NULL;
		buffer->current = NULL;
	}

	void addCharsToBuffer(
		struct Buffer *buffer,
		char ch, int count
	) {
		bufferInvariant(buffer);
		ASSERT(count >= 0);
		for (; count; --count) {
			addToBuffer(buffer, ch);
		}
	}
;

	
	static inline unsigned initHash() {
		return 0x3d9a73b5;
	}

	unsigned addRangeToHash(
		unsigned hash,
		const char *begin,
		const char *end
	) {
		ASSERT(begin);
		for (; begin < end; ++begin) {
			
	hash ^= *begin;
	hash = (hash << 3) | (hash >> 29);
;
		}
		return hash;
	}

	unsigned addTerminatedToHash(
		unsigned hash,
		const char *begin
	) {
		ASSERT(begin);
		for (; *begin; ++begin) {
			
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
		hash = addRangeToHash(
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
		char name[];
	};

	struct Frag *allocFrag(
		const char *nameBegin,
		const char *nameEnd
	) {
		struct Frag *result = NULL;
		
	ASSERT(nameBegin);
	ASSERT(nameBegin <= nameEnd);
	int nameLength =
		nameEnd - nameBegin;
	int size = sizeof(struct Frag)
		+ nameLength + 1;
	result = malloc(size);
	ASSERT(result);
;
		result->link = NULL;
		result->firstEntry = NULL;
		result->expands = 0;
		result->multiples = 0;
		
	memcpy(
		result->name, nameBegin,
		nameLength
	);
	result->name[nameLength] = '\0';
;
		return result;
	}

	void freeFragEntries(
		struct Frag *frag
	) {
		if (frag) {
			
	freeFragEntry(frag->firstEntry);
;
			frag->firstEntry = NULL;
			frag->lastEntry = NULL;
		}
	}

	void freeFrag(
		struct Frag *f
	) {
		while (f) {
			struct Frag *l =
				f->link;
			freeFragEntries(f);
			free(f);
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
		ASSERT(
			strcmp(f->name, name) == 0
		);
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
		const char *valueEnd;
		
	const char *source;
	int line;
;
		char value[];
	};

	struct FragEntry *allocFragEntry(
		struct Frag *frag,
		const char *valueBegin,
		const char *valueEnd
	) {
		struct FragEntry *result = NULL;
		
	int valueLength = 0;
	if (valueBegin) {
		ASSERT(valueBegin <= valueEnd);
		valueLength =
			valueEnd - valueBegin;
	}
	int entrySize = valueLength +
		sizeof(struct FragEntry);
	result = malloc(entrySize);
	ASSERT(result);
;
		result->link = NULL;
		
	if (valueBegin) {
		memcpy(
			result->value, valueBegin,
			valueLength
		);
	}
	result->valueEnd =
		result->value + valueLength;
	result->frag = frag;
;
		return result;
	}

	struct FragEntry *
		allocEmptyFragEntry() {
			return allocFragEntry(
				NULL, NULL, NULL
			);
		}

	void freeFragEntry(
		struct FragEntry *e
	) {
		while (e) {
			struct FragEntry *l =
				e->link;
			free(e);
			e = l;
		}
	}

	int getFragEntryValueSize(
		struct FragEntry *e
	) {
		if (! e) {
			return 0;
		}
		return e->valueEnd -
			e->value;
	}

	struct FragEntry *
		allocTestFragEntry(
			const char *v
		) {
			const char *e = v +
				strlen(v);

			return allocFragEntry(
				NULL, v, e
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
		const char *source,
		int line
	) {
		struct FragEntry *entry =
			allocFragEntry(
				NULL, value, valueEnd
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
			child, NULL, NULL
		);
	addEntryToFrag(frag, entry);
;
	}

	
	char *fragTestBufferCur = NULL;
	const char *fragTestBufferEnd = NULL;
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
		const char *cur = entry->value;
		const char *end = entry->valueEnd;
		int len = end - cur;
		if (! fragTestBufferCur) {
			ASSERT(fwrite(cur, 1, len, out) == len);
		} else {
			ASSERT(fragTestBufferCur + len < fragTestBufferEnd);
			memcpy(fragTestBufferCur, cur, len);
			fragTestBufferCur += len;
			*fragTestBufferCur = '\0';
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
		
	char buffer[100];
	fragTestBufferCur = buffer;
	fragTestBufferEnd = buffer + sizeof(buffer);
	serializeFrag(frag, (void *) buffer, false);
	ASSERT(strcmp(
		expected, buffer
	) == 0);
	fragTestBufferCur = NULL;
	fragTestBufferEnd = NULL;
;
	}

	void addStringToFrag(
		struct Frag *frag,
		const char *str
	) {
		int size = strlen(str);
		addBytesToFrag(
			frag, str, str + size,
			NULL, 0
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
			freeFrag(*cur); *cur = NULL;
		}
		map->link = NULL;
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
	for (; frag; frag = frag->link) {
		const char *a = begin;
		const char *b = frag->name;
		while (a != end) {
			if (*a++ != *b++) { break; }
		}
		if (a == end && ! *b) {
			return frag; }
	}
} ;
		
	if (map->link) {
		return findFragInMap(
			map->link, begin, end
		);
	}
;
		return NULL;
	}

	struct Frag *getFragInMap(
		struct FragMap *map,
		const char *begin,
		const char *end,
		struct FragMap *insert
	) {
		struct Frag *frag = NULL;
		
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
		char name[];
	};

	struct Input *input = NULL;
	struct Input *used = NULL;

	struct FragMap root = {};
	struct FragMap *frags = &root;

	void pushPath(const char *path) {
		FILE *f = fopen(path, "r");
		
	ASSERT(
		f, "can't open [%s]", path
	);
;
		int len = strlen(path) + 1;
		struct Input *i = malloc(
			sizeof(struct Input) + len
		);
		
	ASSERT(
		i,
		"no memory for input"
	);
;
		i->link = input;
		i->file = f;
		memcpy(i->name, path, len);
		
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
	frags->link = NULL;
	frags = nxt;
;
		}
		return ch;
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
		
	enum HtmlState state;

	int headerLevel;
	struct Buffer headerName;
	enum HtmlState headerState;

	int codeOpening;

	int codeIndent;

	char codeSpecial;
	char codeName[100];
	char *codeNameEnd;

	bool noteInCode;
	bool noteInBold;

	};

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
 
	void writeEscaped(
		FILE *out, const char *str,
		const char *end
	) {
		ASSERT(out); ASSERT(str);
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

	int compareBuffer(const char *s, const struct Buffer *b) {
		const char *x = b->buffer;
		while (*s && x != b->current) {
			if (*s == *x) {
				++s; ++x;
			} else if (*s < *x) {
				return -1;
			} else {
				return 1;
			}
		}
		if (*s) { return 1; }
		if (x != b->current) { return -1; }
		return 0;
	}

	void escapeIdent(
		FILE *out,
		const char *cls,
		const struct Buffer *b
	) {
		fprintf(
			out,
			"<span class=\"%s\">%.*s</span>",
			cls,
			(int) (b->current - b->buffer),
			b->buffer
		);
	}

	bool contains(
		const char **begin,
		const char **end,
		const struct Buffer *b
	) {
		for (; begin != end; ++begin) {
			if (compareBuffer(*begin, b) == 0) {
				return true;
			}
		}
		return false;
	}

	bool isKeyword(const struct Buffer *b) {
		static const char *begin[] = {
			"break", "case", "catch", "continue",
			"default", "delete", "else", "for",
			"if", "in", "new", "return", "static",
			"switch", "try", "typeof", "while"
		};
		static const char **end = (void *) begin + sizeof(begin);
		return contains(begin, end, b);
	}

	bool isType(const struct Buffer *b) {
		static const char *begin[] = {
			"FILE",
			"bool", "char", "const", "enum",
			"int", "let", "long", "signed", "struct",
			"union", "unsigned", "void"
		};
		static const char **end = (void *) begin + sizeof(begin);
		if (contains(begin, end, b)) {
			return true;
		}
		if (b->current - b->buffer >= 2) {
			if (isupper(b->buffer[0]) && islower(b->buffer[1])) {
				return true;
			}
		}
		return false;
	}

	bool isNum(const struct Buffer *b) {
		static const char *begin[] = {
			"EOF", "NULL",
			"false", "null", "true", "undefined"
		};
		static const char **end = (void *) begin + sizeof(begin);
		if (isdigit(*b->buffer)) { return true; }
		return contains(begin, end, b);
	}

	int main(
		int argc, const char **argv
	) {
		
	
	 {
	struct Buffer b = {};
	bufferInvariant(&b);
}  {
	struct Buffer b = {};
	ASSERT(! isActiveBuffer(&b));
}  {
	struct Buffer b = {};
	activateBuffer(&b);
	ASSERT(isActiveBuffer(&b));
}  {
	struct Buffer b = {};
	addToBuffer(&b, 'x');
	ASSERT(*b.buffer == 'x');
	ASSERT(b.buffer + 1 == b.current);
	ASSERT(b.buffer == b.initial);
}  {
	struct Buffer b = {};
	addCharsToBuffer(&b, 'x', 1000);
	ASSERT(isActiveBuffer(&b));
	ASSERT(*b.buffer == 'x');
	ASSERT(b.buffer + 1000 == b.current);
	ASSERT(b.buffer != b.initial);
	eraseBuffer(&b);
	ASSERT(! isActiveBuffer(&b));
} ;

	
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
		ASSERT(
			memcmp(entry->value,
				"abc", 3) == 0
		);

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
		struct Frag *frag = NULL;
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
		struct Frag *frag = NULL;
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
	
	
	bool alreadyUsed(const char *name) {
		struct Input *i = input;
		for (; i; i = i->link) {
			if (strcmp(i->name, name) == 0) {
				return true;
			}
		}
		for (i = used; i; i = i->link) {
			if (strcmp(i->name, name) == 0) {
				return true;
			}
		}
		return false;
	}
;
	{
		
	struct Frag *frag = NULL;
	struct Buffer buffer = {};
	int bufferLine = 0;

	char openCh = '\0';

	struct Buffer name = {};
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
			activateBuffer(&name);
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
			activateBuffer(&name);
			break;
		}
	}
} 
	if (frag) {
		if (! isActiveBuffer(&buffer)) {
			bufferLine = input->line;
		}
		addToBuffer(&buffer, last);
	}
;
			break;
		case '}': {
			bool processed = false;
			 {
	if (isActiveBuffer(&name)) {
		addToBuffer(&name, '\0');
		
	if (openCh == 'd') {
		ASSERT(! frag, "def in frag");
		struct FragMap *fm = &input->frags;
		
	frag = findFragInMap(
		fm, name.buffer,
		name.current - 1
	);
	if (isPopulatedFrag(frag)) {
		printf(
			"frag [%s] already defined\n",
			name.buffer
		);
	}
;
		if (! frag) {
			frag = allocFragInMap(
				fm, name.buffer,
				name.current - 1
			);
		}
		processed = true;
	}

	if (openCh == 'D') {
		ASSERT(! frag, "def in frag");
		struct FragMap *fm = frags;
		
	frag = findFragInMap(
		fm, name.buffer,
		name.current - 1
	);
	if (isPopulatedFrag(frag)) {
		printf(
			"frag [%s] already defined\n",
			name.buffer
		);
	}
;
		if (! frag) {
			frag = allocFragInMap(
				&root, name.buffer,
				name.current - 1
			);
		}
		processed = true;
	}

	if (openCh == 'a') {
		ASSERT(! frag, "add in frag");
		struct FragMap *fm = &input->frags;
		struct FragMap *ins = fm;
		frag = findFragInMap(
			fm, name.buffer,
			name.current - 1
		);
		
	if (! isPopulatedFrag(frag)) {
		printf(
			"frag [%s] not defined\n",
			name.buffer
		);
		frag = getFragInMap(
			fm, name.buffer,
			name.current - 1,
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
			fm, name.buffer,
			name.current - 1
		);
		
	if (! isPopulatedFrag(frag)) {
		printf(
			"frag [%s] not defined\n",
			name.buffer
		);
		frag = getFragInMap(
			fm, name.buffer,
			name.current - 1,
			ins
		);
	}
;
		processed = true;
	}

	if (openCh == 'r') {
		ASSERT(! frag, "replace in frag");
		frag = getFragInMap(
			&input->frags, name.buffer,
			name.current - 1, &input->frags
		);
		ASSERT(
			frag, "frag %s not defined",
			name.buffer
		);
		freeFragEntries(frag);
		processed = true;
	}

	if (openCh == 'R') {
		ASSERT(! frag, "replace in frag");
		frag = getFragInMap(
			frags, name.buffer,
			name.current - 1, &root
		);
		ASSERT(
			frag, "frag %s not defined",
			name.buffer
		);
		freeFragEntries(frag);
		processed = true;
	}

	if (openCh == 'x') {
		ASSERT(frag, "end not in frag");
		
	ASSERT(
		! strcmp(frag->name, name.buffer),
		"closing [%s] != [%s]",
		name.buffer, frag->name
	);
;
		
	if (
		buffer.buffer != buffer.current
	) {
		addBytesToFrag(
			frag, buffer.buffer,
			buffer.current,
			input->name, bufferLine
		);
		resetBuffer(&buffer);
	}
;
		frag = NULL;
		processed = true;
	}

	if (openCh == 'i') {
		ASSERT(! frag, "include in frag");
		if (! alreadyUsed(name.buffer)) {
			pushPath(name.buffer);
		}
		processed = true;
	}

	if (openCh == 'e') {
		ASSERT(frag, "expand not in frag");
		
	if (
		buffer.buffer != buffer.current
	) {
		addBytesToFrag(
			frag, buffer.buffer,
			buffer.current,
			input->name, bufferLine
		);
		resetBuffer(&buffer);
	}
;
		struct Frag *sub = getFragInMap(
			&input->frags, name.buffer,
			name.current - 1, &input->frags
		);
		
	if (sub->expands) {
		printf(
			"multiple expands of [%s]\n",
			sub->name
		);
	}
	if (sub->multiples) {
		printf(
			"expand after mult of [%s]\n",
			sub->name
		);
	}
;
		++sub->expands;
		addFragToFrag(frag, sub);
		processed = true;
	}

	if (openCh == 'g') {
		ASSERT(frag, "expand not in frag");
		
	if (
		buffer.buffer != buffer.current
	) {
		addBytesToFrag(
			frag, buffer.buffer,
			buffer.current,
			input->name, bufferLine
		);
		resetBuffer(&buffer);
	}
;
		struct Frag *sub = getFragInMap(
			frags, name.buffer,
			name.current - 1, &root
		);
		
	if (sub->expands) {
		printf(
			"multiple expands of [%s]\n",
			sub->name
		);
	}
	if (sub->multiples) {
		printf(
			"expand after mult of [%s]\n",
			sub->name
		);
	}
;
		++sub->expands;
		addFragToFrag(frag, sub);
		processed = true;
	}

	if (openCh == 'E') {
		ASSERT(frag, "multiple not in frag");
		
	if (
		buffer.buffer != buffer.current
	) {
		addBytesToFrag(
			frag, buffer.buffer,
			buffer.current,
			input->name, bufferLine
		);
		resetBuffer(&buffer);
	}
;
		struct Frag *sub =
			getFragInMap(
				&input->frags, name.buffer,
				name.current - 1, &input->frags
			);
		
	if (sub->expands) {
		printf(
			"multiple after expand "
				"of [%s]\n",
			sub->name
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
		
	if (
		buffer.buffer != buffer.current
	) {
		addBytesToFrag(
			frag, buffer.buffer,
			buffer.current,
			input->name, bufferLine
		);
		resetBuffer(&buffer);
	}
;
		struct Frag *sub =
			getFragInMap(
				frags, name.buffer,
				name.current - 1, &root
			);
		
	if (sub->expands) {
		printf(
			"multiple after expand "
				"of [%s]\n",
			sub->name
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
	cur = addTerminatedToHash(
		cur, input->name
	);
	cur = addRangeToHash(
		cur, name.buffer, name.current
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

	
	if (
		buffer.buffer != buffer.current
	) {
		addBytesToFrag(
			frag, buffer.buffer,
			buffer.current,
			input->name, bufferLine
		);
		resetBuffer(&buffer);
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
		frag, name.buffer, name.current - 1,
		input->name, nameLine
	);
;
		processed = true;
	}

	if (openCh == 'm') {
		ASSERT(frag, "magic not in frag");
		
	unsigned cur = initHash();
	cur = addTerminatedToHash(
		cur, input->name
	);
	cur = addRangeToHash(
		cur, name.buffer, name.current
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
	
	if (
		buffer.buffer != buffer.current
	) {
		addBytesToFrag(
			frag, buffer.buffer,
			buffer.current,
			input->name, bufferLine
		);
		resetBuffer(&buffer);
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
			name.buffer
		);
		const char *c = name.buffer;
		for (; c != name.current - 1; ++c) {
			if (! isActiveBuffer(&buffer)) {
				bufferLine = input->line;
			}
			addToBuffer(&buffer, *c);
		}
		processed = true;
	}
;
		resetBuffer(&name);
		last = ch;
		ch = nextCh();
	}
} 
	if (frag && ! processed) {
		if (! isActiveBuffer(&buffer)) {
			bufferLine = input->line;
		}
		addToBuffer(&buffer, last);
	}
;
			break;
		}
		default:
			 {
	if (isActiveBuffer(&name)) {
		addToBuffer(&name, ch);
		break;
	}
}  {
	if (frag) {
		if (! isActiveBuffer(&buffer)) {
			bufferLine = input->line;
		}
		addToBuffer(&buffer, last);
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
		"file: ", frag->name, 6
	)) {
		++frag->expands;
		
	FILE *f =
		fopen(frag->name + 6, "w");
	ASSERT(
		f, "can't open %s",
		frag->name + 6
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
			frag->name
		);
	}
} 
	if (frag->multiples == 1) {
		printf(
			"multiple frag [%s] only "
				"used once\n",
			frag->name
		);
	}

	if (! isPopulatedFrag(frag)) {
		printf(
			"frag [%s] not populated\n",
			frag->name
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
		"file: ", frag->name, 6
	)) {
		++frag->expands;
		
	FILE *f =
		fopen(frag->name + 6, "w");
	ASSERT(
		f, "can't open %s",
		frag->name + 6
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
			frag->name
		);
	}
} 
	if (frag->multiples == 1) {
		printf(
			"multiple frag [%s] only "
				"used once\n",
			frag->name
		);
	}

	if (! isPopulatedFrag(frag)) {
		printf(
			"frag [%s] not populated\n",
			frag->name
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
		
	int len = strlen(cur->name) + 4;
	char *outPath = malloc(len);
	ASSERT(outPath);
	memcpy(outPath, cur->name, len - 6);
	strcpy(outPath + len - 6, ".html");
	FILE *out = fopen(outPath, "w");
	ASSERT(out);
	 
	FILE *in = fopen(cur->name, "r");
	ASSERT(in);
	 {
	struct HtmlStatus status = {
		.state = hs_NOTHING_WRITTEN
		
	, .headerLevel = 0
	, .headerName = {}

	, .codeOpening = 0
	, .codeIndent = 0
	, .codeSpecial = '\0'
	, .codeNameEnd = NULL

	, .noteInCode = false
	, .noteInBold = false

	};
	bool newline = true;
	struct Buffer ident = {};
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
			 
	ASSERT(isActiveBuffer(&status.headerName));
	 
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
		status.headerName.buffer,
		status.headerName.current
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
		status.headerName.buffer,
		status.headerName.current
	);
	fprintf(out, "</h%d>\n", status.headerLevel);
;
	fprintf(out, "<div class=\"slides\">\n");
	fprintf(out, "<div><div>\n");
	 
	fprintf(out, "<h%d>", status.headerLevel);
	writeEscaped(
		out,
		status.headerName.buffer,
		status.headerName.current
	);
	fprintf(out, "</h%d>\n", status.headerLevel);
;
	fprintf(out, "</div>\n");
;
			 
	status.state = hs_IN_SLIDE;
	status.headerLevel = 0;
	resetBuffer(&status.headerName);
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
		if (
			isActiveBuffer(&status.headerName)
		) {
			addToBuffer(
				&status.headerName, ch
			);
			
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
		if (ch > ' ' && ! isActiveBuffer(
			&status.headerName
		)) {
			addToBuffer(
				&status.headerName, ch
			);
			
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
			
	if (isActiveBuffer(&ident)) {
		if (ch == '(') {
			escapeIdent(out, "fn", &ident);
		} else if (isKeyword(&ident)) {
			escapeIdent(out, "keyword", &ident);
		} else if (isType(&ident)) {
			escapeIdent(out, "type", &ident);
		} else if (isNum(&ident)) {
			escapeIdent(out, "num", &ident);
		} else {
			escapeIdent(out, "var", &ident);
		}
		resetBuffer(&ident);
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
			
	if (isActiveBuffer(&ident)) {
		if (ch == '(') {
			escapeIdent(out, "fn", &ident);
		} else if (isKeyword(&ident)) {
			escapeIdent(out, "keyword", &ident);
		} else if (isType(&ident)) {
			escapeIdent(out, "type", &ident);
		} else if (isNum(&ident)) {
			escapeIdent(out, "num", &ident);
		} else {
			escapeIdent(out, "var", &ident);
		}
		resetBuffer(&ident);
	}
;
			writeEscaped(out, status.codeName, status.codeNameEnd);
			fprintf(out, "`</span>");
			status.codeSpecial = 0;
			status.codeNameEnd = NULL;
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
			addToBuffer(&ident, ch);
			continue;
		}
	}

	if (status.state == hs_IN_CODE) {
		 
	if (ch == '\n') {
		
	if (isActiveBuffer(&ident)) {
		if (ch == '(') {
			escapeIdent(out, "fn", &ident);
		} else if (isKeyword(&ident)) {
			escapeIdent(out, "keyword", &ident);
		} else if (isType(&ident)) {
			escapeIdent(out, "type", &ident);
		} else if (isNum(&ident)) {
			escapeIdent(out, "num", &ident);
		} else {
			escapeIdent(out, "var", &ident);
		}
		resetBuffer(&ident);
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

	
	if (ch == '{' && ident.current - ident.buffer == 1) {
		char lc = *ident.buffer;
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
		fprintf(out, "<br/>");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

			default: break;
		}
		if (status.codeSpecial) {
			resetBuffer(&ident);
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
		
	if (isActiveBuffer(&ident)) {
		if (ch == '(') {
			escapeIdent(out, "fn", &ident);
		} else if (isKeyword(&ident)) {
			escapeIdent(out, "keyword", &ident);
		} else if (isType(&ident)) {
			escapeIdent(out, "type", &ident);
		} else if (isNum(&ident)) {
			escapeIdent(out, "num", &ident);
		} else {
			escapeIdent(out, "var", &ident);
		}
		resetBuffer(&ident);
	}
;
		resetBuffer(&ident);
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
	status.codeNameEnd = NULL;
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
		status.codeNameEnd = NULL;
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
		
	if (isActiveBuffer(&ident)) {
		if (ch == '(') {
			escapeIdent(out, "fn", &ident);
		} else if (isKeyword(&ident)) {
			escapeIdent(out, "keyword", &ident);
		} else if (isType(&ident)) {
			escapeIdent(out, "type", &ident);
		} else if (isNum(&ident)) {
			escapeIdent(out, "num", &ident);
		} else {
			escapeIdent(out, "var", &ident);
		}
		resetBuffer(&ident);
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
			resetBuffer(&ident);
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
			resetBuffer(&ident);
			newline = false;
			continue;
		}
	}
 
	if (
		ch == '`' &&
		status.state == hs_IN_NOTES
	) {
		
	if (isActiveBuffer(&ident)) {
		if (ch == '(') {
			escapeIdent(out, "fn", &ident);
		} else if (isKeyword(&ident)) {
			escapeIdent(out, "keyword", &ident);
		} else if (isType(&ident)) {
			escapeIdent(out, "type", &ident);
		} else if (isNum(&ident)) {
			escapeIdent(out, "num", &ident);
		} else {
			escapeIdent(out, "var", &ident);
		}
		resetBuffer(&ident);
	}
;
		if (status.noteInCode) {
			fprintf(out, "</code>");
		} else {
			fprintf(out, "<code>");
		}
		status.noteInCode = ! status.noteInCode;
		resetBuffer(&ident);
		newline = false;
		continue;
	}
 
	if (status.state == hs_IN_NOTES &&
	status.noteInCode) {
		
	if (ch == '{' && ident.current - ident.buffer == 1) {
		char lc = *ident.buffer;
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
		fprintf(out, "<br/>");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;

			default: break;
		}
		if (status.codeSpecial) {
			resetBuffer(&ident);
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
		
	if (isActiveBuffer(&ident)) {
		if (ch == '(') {
			escapeIdent(out, "fn", &ident);
		} else if (isKeyword(&ident)) {
			escapeIdent(out, "keyword", &ident);
		} else if (isType(&ident)) {
			escapeIdent(out, "type", &ident);
		} else if (isNum(&ident)) {
			escapeIdent(out, "num", &ident);
		} else {
			escapeIdent(out, "var", &ident);
		}
		resetBuffer(&ident);
	}
;
		resetBuffer(&ident);
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
	status.codeNameEnd = NULL;
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
		status.codeNameEnd = NULL;
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
		
	if (isActiveBuffer(&ident)) {
		if (ch == '(') {
			escapeIdent(out, "fn", &ident);
		} else if (isKeyword(&ident)) {
			escapeIdent(out, "keyword", &ident);
		} else if (isType(&ident)) {
			escapeIdent(out, "type", &ident);
		} else if (isNum(&ident)) {
			escapeIdent(out, "num", &ident);
		} else {
			escapeIdent(out, "var", &ident);
		}
		resetBuffer(&ident);
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
	free(outPath);
;
		struct Input *next = cur->link;
		free(cur);
		cur = next;
	}
;

	}
