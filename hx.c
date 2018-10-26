
	
	
	#include <stdio.h>
	#include <stdlib.h>

	#include <stdlib.h>

	#include <string.h>

	#include <stdbool.h>
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
	
struct MacroEntry;
void freeMacroEntry(
	struct MacroEntry *entry
);
;

	struct Input {
		struct Input *link;
		FILE *file;
		char name[];
	};

	struct Input *input = NULL;
	struct Input *used = NULL;

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
		input = i;
	}

	int nextCh() {
		int ch = EOF;
		while (input) {
			ch = fgetc(input->file);
			if (ch != EOF) { break; }
			struct Input *n = input->link;
			fclose(input->file);
			input->link = used;
			used = input;
			input = n;
		}
		return ch;
	}

	
	#define INIT_BUFFER_SIZE 16
	
	struct Buffer {
		char initial[INIT_BUFFER_SIZE];
		char *buffer;
		char *current;
		const char *end;
	};

	void addToBuffer(
		struct Buffer *buffer, char ch
	) {
		ASSERT(buffer);
		
	if (! buffer->buffer) {
		buffer->buffer =
			buffer->initial;
		buffer->current =
			buffer->buffer;
		buffer->end =
			buffer->initial +
				INIT_BUFFER_SIZE;
	}

		
	if (
		buffer->current >= buffer->end
	) {
		int size = buffer->current -
			buffer->buffer;
		int newSize = 2 * size;
		
	char *newBuffer;
	if (
		buffer->buffer == buffer->initial
	) {
		newBuffer = malloc(newSize);
		
	ASSERT(newBuffer);
	memcpy(
		newBuffer, buffer->buffer, size
	);
;
	} else {
		newBuffer = realloc(
			buffer->buffer, newSize);
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
		ASSERT(buffer);
		buffer->current = buffer->buffer;
	}

	void eraseBuffer(
		struct Buffer *buffer
	) {
		ASSERT(buffer);
		
	if (buffer->buffer &&
		buffer->buffer != buffer->initial
	) {
		free(buffer->buffer);
		buffer->buffer = buffer->initial;
	}
;
		buffer->current = buffer->buffer;
	}

	void addCharsToBuffer(
		struct Buffer *buffer,
		char ch, int count
	) {
		ASSERT(buffer);
		ASSERT(count >= 0);
		for (; count; --count) {
			addToBuffer(buffer, ch);
		}
	}
;

	
	struct MacroEntry;

	struct Macro {
		struct Macro *link;
		struct MacroEntry *firstEntry;
		struct MacroEntry *lastEntry;
		int expands;
		int multiples;
		char name[];
	};

	struct Macro *allocMacro(
		const char *nameBegin,
		const char *nameEnd
	) {
		struct Macro *result = NULL;
		
	ASSERT(nameBegin);
	ASSERT(nameBegin <= nameEnd);
	int nameLength =
		nameEnd - nameBegin;
	int macroSize = sizeof(struct Macro)
		+ nameLength + 1;
	result = malloc(macroSize);
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

	void freeMacrosEntries(
		struct Macro *macro
	) {
		if (macro) {
			
	freeMacroEntry(macro->firstEntry);
;
			macro->firstEntry = NULL;
			macro->lastEntry = NULL;
		}
	}

	void freeMacro(
		struct Macro *macro
	) {
		while (macro) {
			struct Macro *link =
				macro->link;
			freeMacrosEntries(macro);
			free(macro);
			macro = link;
		}
	}

	struct Macro *allocTestMacro(
		const char *name
	) {
		return allocMacro(
			name, name + strlen(name)
		);
	}

	void testMacroName(
		const char *name
	) {
		struct Macro *macro =
			allocTestMacro(name);
		ASSERT(macro);
		ASSERT(
			strcmp(macro->name, name) == 0
		);
		freeMacro(macro);
	}

	bool isPopulatedMacro(
		const struct Macro *macro
	) {
		return macro && macro->firstEntry;
	}

	struct MacroEntry {
		struct MacroEntry *link;
		struct Macro *macro;
		const char *valueEnd;
		char value[];
	};

	struct MacroEntry *allocMacroEntry(
		struct Macro *macro,
		const char *valueBegin,
		const char *valueEnd
	) {
		struct MacroEntry *result = NULL;
		
	int valueLength = 0;
	if (valueBegin) {
		ASSERT(valueBegin <= valueEnd);
		valueLength =
			valueEnd - valueBegin;
	}
	int entrySize = valueLength +
		sizeof(struct MacroEntry);
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
	result->macro = macro;
;
		return result;
	}

struct MacroEntry *
allocEmptyMacroEntry() {
	return allocMacroEntry(
		NULL, NULL, NULL
	);
}

void freeMacroEntry(
	struct MacroEntry *entry
) {
	while (entry) {
		struct MacroEntry *link =
			entry->link;
		free(entry);
		entry = link;
	}
}

	int getMacroEntryValueSize(
		struct MacroEntry *entry
	) {
		if (! entry) {
			return 0;
		}
		return entry->valueEnd -
			entry->value;
	}

	struct MacroEntry *
	allocTestMacroEntry(
		const char *value
	) {
		const char *end = value +
			strlen(value);

		return allocMacroEntry(
			NULL, value, end
		);
	}

	void addEntryToMacro(
		struct Macro *macro,
		struct MacroEntry *entry
	) {
		
	ASSERT(macro);
	ASSERT(entry);
	ASSERT(! entry->link);
;
		if (macro->firstEntry) {
			
	macro->lastEntry->link = entry;
	macro->lastEntry = entry;
;
		} else {
			
	macro->firstEntry = entry;
	macro->lastEntry = entry;
;
		}
	}

	void addBytesToMacro(
		struct Macro *macro,
		const char *value,
		const char *valueEnd
	) {
		struct MacroEntry *entry =
			allocMacroEntry(
				NULL, value, valueEnd
			);
		addEntryToMacro(macro, entry);
	}

	
	bool isMacroInMacro(
		struct Macro *needle,
		struct Macro *haystack
	) {
		ASSERT(needle);
		ASSERT(haystack);
		
	if (needle == haystack) {
		return true;
	}
;
		
	struct MacroEntry *entry =
		haystack->firstEntry;
	for (; entry; entry = entry->link) {
		if (! entry->macro) { continue; }
		if (isMacroInMacro(
			needle, entry->macro
		)) {
			return true;
		}
	}
;
		return false;
	}

	void addMacroToMacro(
		struct Macro *macro,
		struct Macro *child
	) {
		ASSERT(macro);
		ASSERT(child);
		
	ASSERT(! isMacroInMacro(
		macro, child
	));
;
		
	if (macro->firstEntry &&
		! macro->lastEntry->macro
	) {
		macro->lastEntry->macro = child;
		return;
	}
;
		
	struct MacroEntry *entry =
		allocMacroEntry(
			child, NULL, NULL
		);
	addEntryToMacro(macro, entry);
;
	}

	
	char *macroTestBufferCur = NULL;
	const char *macroTestBufferEnd = NULL;
;
	void serializeMacro(
		struct Macro *macro,
		FILE *out
	) {
		ASSERT(macro);
		ASSERT(out);
		
	struct MacroEntry *entry =
		macro->firstEntry;
	for (; entry; entry = entry->link) {
		
	if (getMacroEntryValueSize(entry)) {
		const char *cur = entry->value;
		const char *end = entry->valueEnd;
		int len = end - cur;
		if (! macroTestBufferCur) {
			ASSERT(fwrite(cur, 1, len, out) == len);
		} else {
			ASSERT(macroTestBufferCur + len < macroTestBufferEnd);
			memcpy(macroTestBufferCur, cur, len);
			macroTestBufferCur += len;
			*macroTestBufferCur = '\0';
		}
	}
;
		if (entry->macro) {
			serializeMacro(
				entry->macro, out
			);
		}
	}
;
	}

	void testMacro(struct Macro *
		macro, const char *expected
	) {
		
	char buffer[100];
	macroTestBufferCur = buffer;
	macroTestBufferEnd = buffer + sizeof(buffer);
	serializeMacro(macro, (void *) buffer);
	ASSERT(strcmp(
		expected, buffer
	) == 0);
	macroTestBufferCur = NULL;
	macroTestBufferEnd = NULL;
;
	}

	void addStringToMacro(
		struct Macro *macro,
		const char *str
	) {
		int size = strlen(str);
		addBytesToMacro(
			macro, str, str + size
		);
	}

	#define MACRO_SLOTS 128

	struct MacroMap {
		struct Macro *macros[
			MACRO_SLOTS
		];
	};

	void clearMacroMap(
		struct MacroMap *map
	) {
		struct Macro **cur = map->macros;
		struct Macro **end =
			cur + MACRO_SLOTS;
		for (; cur < end; ++cur) {
			freeMacro(*cur); *cur = NULL;
		}
	}

	int calcHash(const char *begin,
		const char *end) {
		ASSERT(begin);
		unsigned hash = 0xf1e2d3c4;
		while (*begin && begin != end) {
			hash ^= *begin++;
			hash = (hash << 3) |
				(hash >> 29);
		}
		return hash % MACRO_SLOTS;
	}

	struct Macro *allocMacroInMap(
		struct MacroMap *map,
		const char *begin,
		const char *end
	) {
		ASSERT(map);
		struct Macro *macro =
			allocMacro(begin, end);
		
	int hash = calcHash(begin, end);
	macro->link = map->macros[hash];
	map->macros[hash] = macro;
;
		return macro;
	}

	struct Macro *findMacroInMap(
		struct MacroMap *map,
		const char *begin,
		const char *end
	) {
		ASSERT(map);
		struct Macro *macro = NULL;
		
	int hash = calcHash(begin, end);
	macro = map->macros[hash];
	for (; macro; macro = macro->link) {
		const char *a = begin;
		const char *b = macro->name;
		while (a != end) {
			if (*a++ != *b++) { break; }
		}
		if (a == end && ! *b) {
			return macro; }
	}
;
		return macro;
	}

	struct Macro *getMacroInMap(
		struct MacroMap *map,
		const char *begin,
		const char *end
	) {
		struct Macro *macro = NULL;
		
	macro = findMacroInMap(
		map, begin, end
	);
	if (macro) {
		return macro;
	}
;
		
	macro = allocMacroInMap(
		map, begin, end
	);
;
		return macro;
	}
;

	struct SourceElement {
		struct SourceElement *_private_link;
		unsigned _private_magic;
		char path[];
	};

	struct SourceElement *createSourceElement(
		const char *path
	) {
		ASSERT(path);
		int len = strlen(path) + 1;
		int size =
			len + sizeof(struct SourceElement);
		struct SourceElement *se =
			malloc(size);
		ASSERT(se);
		se->_private_link = NULL;
		memcpy(se->path, path, len);
		se->_private_magic = 2478325;
		return se;
	}

	bool hasSuffix(
		const char *str,
		const char *suff
	) {
		ASSERT(str); ASSERT(suff);
		int sl = strlen(str);
		int su = strlen(suff);
		return sl >= su && 0 == memcmp(
			str + sl - su, suff, su
		);
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
		char headerName[100];
		char * headerNameEnd;
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
		FILE *out, const char *str, const char *end
	) {
		ASSERT(out); ASSERT(str);
		for (; *str && str != end; ++str) switch (*str) {
			case '<':
				fprintf(out, "&lt;"); break;
			case '>':
				fprintf(out, "&gt;"); break;
			case '&':
				fprintf(out, "&"); break;
			default:
				fputc(*str, out);
		}
	}

	int main(
		int argc, const char **argv
	) {
		
	
	 {
	struct Buffer buffer = {};
	addToBuffer(&buffer, 'x');
	ASSERT(*buffer.buffer == 'x');
	ASSERT(buffer.buffer + 1 ==
		buffer.current);
	ASSERT(buffer.buffer ==
		buffer.initial);
}  {
	struct Buffer buffer = {};
	addCharsToBuffer(&buffer, 'x',
		1000);
	ASSERT(*buffer.buffer == 'x');
	ASSERT(buffer.buffer + 1000 ==
		buffer.current);
	ASSERT(buffer.buffer !=
		buffer.initial);
	eraseBuffer(&buffer);
	ASSERT(buffer.buffer ==
		buffer.initial);
} ;

	
	testMacroName("abc");
	testMacroName("");
	testMacroName("A c");
	{
		struct Macro *macro =
			allocTestMacro("ab");
		ASSERT(macro);
		ASSERT(! macro->link);
		ASSERT(! macro->firstEntry);
		freeMacro(macro);
	}

	{
		struct MacroEntry *entry =
			allocEmptyMacroEntry();

		ASSERT(entry);
		ASSERT(! entry->link);
		ASSERT(! entry->macro);

		freeMacroEntry(entry);
	}

	{
		struct MacroEntry *entry =
			allocEmptyMacroEntry();

		ASSERT(entry);
		ASSERT(
			getMacroEntryValueSize(
				entry) == 0);

		freeMacroEntry(entry);
	}

	{
		struct MacroEntry *entry =
			allocTestMacroEntry("abc");

		ASSERT(entry);
		ASSERT(
			getMacroEntryValueSize(
				entry) == 3);

		freeMacroEntry(entry);
	}

	{
		struct MacroEntry *entry =
			allocTestMacroEntry("abc");

		ASSERT(entry);
		ASSERT(
			memcmp(entry->value,
				"abc", 3) == 0);

		freeMacroEntry(entry);
	}

	{
		struct Macro *macro =
			allocTestMacro("");
		struct MacroEntry *entry =
			allocEmptyMacroEntry();
		addEntryToMacro(macro, entry);
		ASSERT(
			macro->firstEntry == entry
		);
		freeMacro(macro);
	}

	{
		struct Macro *macro =
			allocTestMacro("");
		struct MacroEntry *entry =
			allocEmptyMacroEntry();
		addEntryToMacro(macro, entry);
		ASSERT(
			macro->lastEntry == entry
		);
		freeMacro(macro);
	}

	{
		struct Macro *macro = NULL;
		struct MacroEntry *first;
		struct MacroEntry *second;
		
	macro = allocTestMacro("");
	first = allocEmptyMacroEntry();
	second = allocEmptyMacroEntry();

	addEntryToMacro(macro, first);
	addEntryToMacro(macro, second);
;
		
	ASSERT(macro->firstEntry == first);
;
		freeMacro(macro);
	}

	{
		struct Macro *macro = NULL;
		struct MacroEntry *first;
		struct MacroEntry *second;
		
	macro = allocTestMacro("");
	first = allocEmptyMacroEntry();
	second = allocEmptyMacroEntry();

	addEntryToMacro(macro, first);
	addEntryToMacro(macro, second);
;
		ASSERT(
			macro->lastEntry == second
		);
		freeMacro(macro);
	}

	{
		struct Macro *macro =
			allocTestMacro("");
		addStringToMacro(macro, "abc");
		addStringToMacro(macro, "def");
		testMacro(macro, "abcdef");
		freeMacro(macro);
	}
 {
	struct Macro *a =
		allocTestMacro("");
	struct Macro *b =
		allocTestMacro("");
	addStringToMacro(a, "abc");
	addMacroToMacro(b, a);
	addStringToMacro(b, "def");
	addMacroToMacro(b, a);
	testMacro(b, "abcdefabc");
	freeMacro(a); freeMacro(b);
} ;
;
	
	if(argc > 1) {
		pushPath(argv[1]);
	} else {
		pushPath("index.x");
	}
;
	
	
	struct MacroMap macros = {};

	bool alreadyUsed(const char *name) {
		for (struct Input *u = used; u; u =
		u->link) {
			if (strcmp(u->name, name) == 0) {
				return true;
			}
		}
		return false;
	}
;
	{
		
	struct Macro * macro = NULL;
	struct Buffer buffer = {};

	char openCh = '\0';

	char name[128];
	char *nameCur = NULL;	
	const char *nameEnd = name +
		sizeof(name);
;
		int last = nextCh();
		int ch = nextCh();
		while (ch != EOF) {
			
	switch (ch) {
		case '{':
			 {
	if (! macro) {
		static const char valids[] = "adir";
		if (strchr(valids, last)) {
			openCh = last;
			nameCur = name;
			break;
		}
	}
}  {
	if (macro) {
		bool valid = false;
		
	static const char valids[] =
		"123456fvsntkxeEpm";
	if (strchr(valids, last)) {
		valid = true;
	}
;
		if (valid) {
			openCh = last;
			nameCur = name;
			break;
		}
	}
} 
	if (macro) {
		addToBuffer(&buffer, last);
	}
;
			break;
		case '}': {
			bool processed = false;
			 {
	if (nameCur) {
		*nameCur = '\0';
		
	if (openCh == 'd') {
		ASSERT(! macro, "def in macro");
		macro = findMacroInMap(
			&macros, name, nameCur
		);
		if (isPopulatedMacro(macro)) {
			printf("macro [%.*s] already defined\n", (int) (nameCur - name), name);
		}
		if (! macro) {
			macro = allocMacroInMap(
				&macros, name, nameCur
			);
		}
		processed = true;
	}

	if (openCh == 'a') {
		ASSERT(! macro, "add in macro");
		macro = findMacroInMap(
			&macros, name, nameCur
		);
		if (! isPopulatedMacro(macro)) {
			printf("macro [%.*s] not defined\n", (int) (nameCur - name), name);
			macro = getMacroInMap(&macros, name, nameCur);
		}
		processed = true;
	}

	if (openCh == 'r') {
		ASSERT(! macro, "replace in macro");
		macro = getMacroInMap(
			&macros, name, nameCur
		);
		ASSERT(macro, "macro %.*s not defined", (int) (nameCur - name), name);
		freeMacrosEntries(macro);
		processed = true;
	}

	if (openCh == 'x') {
		ASSERT(macro, "end not in macro");
		
	ASSERT(
		! strcmp(macro->name, name),
		"closing [%s] != [%s]",
		name, macro->name
	);
;
		
	if (
		buffer.buffer != buffer.current
	) {
		addBytesToMacro(
			macro, buffer.buffer,
			buffer.current
		);
		resetBuffer(&buffer);
	}
;
		macro = NULL;
		processed = true;
	}

	if (openCh == 'i') {
		ASSERT(! macro, "include in macro");
		if (! alreadyUsed(name)) {
			pushPath(name);
		}
		processed = true;
	}

	if (openCh == 'e') {
		ASSERT(macro, "expand not in macro");
		
	if (
		buffer.buffer != buffer.current
	) {
		addBytesToMacro(
			macro, buffer.buffer,
			buffer.current
		);
		resetBuffer(&buffer);
	}
;
		struct Macro *sub =
			getMacroInMap(
				&macros, name, nameCur);
		if (sub->expands) {
			printf("multiple expands of [%s]\n", sub->name);
		}
		if (sub->multiples) {
			printf("expand after mult of [%s]\n", sub->name);
		}
		++sub->expands;
		addMacroToMacro(
			macro, sub);
		processed = true;
	}

	if (openCh == 'E') {
		ASSERT(macro, "multiple not in macro");
		
	if (
		buffer.buffer != buffer.current
	) {
		addBytesToMacro(
			macro, buffer.buffer,
			buffer.current
		);
		resetBuffer(&buffer);
	}
;
		struct Macro *sub =
			getMacroInMap(
				&macros, name, nameCur);
		if (sub->expands) {
			printf("multiple after expand of [%s]\n", sub->name);
		}
		++sub->multiples;
		addMacroToMacro(
			macro, sub);
		processed = true;
	}

	if (openCh == 'p') {
		ASSERT(macro, "private not in macro");
		
	static char prefix[] = "_private_";
	
	if (
		buffer.buffer != buffer.current
	) {
		addBytesToMacro(
			macro, buffer.buffer,
			buffer.current
		);
		resetBuffer(&buffer);
	}
;
	addBytesToMacro(
		macro, prefix, prefix + sizeof(prefix) - 1
	);
	addBytesToMacro(
		macro, name, nameCur
	);
;
		processed = true;
	}

	if (openCh == 'm') {
		ASSERT(macro, "magic not in macro");
		
	static char magic[] = "2478325";
	
	if (
		buffer.buffer != buffer.current
	) {
		addBytesToMacro(
			macro, buffer.buffer,
			buffer.current
		);
		resetBuffer(&buffer);
	}
;
	addBytesToMacro(
		macro, magic, magic + sizeof(magic) - 1
	);
;
		processed = true;
	}

	if (! processed) {
		ASSERT(macro, "unknown macro %.*s", (int) (nameCur - name), name);
		const char *c = name;
		for (; c != nameCur; ++c) {
			addToBuffer(&buffer, *c);
		}
		processed = true;
	}
;
		nameCur = NULL;
		last = ch;
		ch = nextCh();
	}
} 
	if (macro && ! processed) {
		addToBuffer(&buffer, last);
	}
;
			break;
		}
		default:
			 {
	if (nameCur) {
		ASSERT(nameCur < nameEnd, "name too long");
		*nameCur++ = ch;
		break;
	}
}  {
	if (macro) {
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
	struct Macro **cur = macros.macros;
	struct Macro **end =
		cur + MACRO_SLOTS;
	for (; cur < end; ++cur) {
		struct Macro *macro = *cur;
		for (; macro; macro = macro->link) {
			
	if (! memcmp(
		"file: ", macro->name, 6
	)) {
		++macro->expands;
		
	FILE *f = fopen(macro->name + 6, "w");
	ASSERT(f, "can't open %s", macro->name + 6);
	serializeMacro(macro, f);
	fclose(f);
;
	}

	if (macro->expands + macro->multiples <= 0) {
		printf("macro [%s] not called\n", macro->name);
	}

	if (macro->multiples == 1) {
		printf("multiple macro [%s] only used once\n", macro->name);
	}

	if (! isPopulatedMacro(macro)) {
		printf("macro [%s] not populated\n", macro->name);
	}
;
		}
	}
} ;
	
	struct SourceElement *cur;
	if (argc > 1) {
		cur = createSourceElement(argv[1]);
	} else {
		cur = createSourceElement("index.x");
	}
	struct SourceElement *end = cur;
	while (cur) {
		
	if (hasSuffix(cur->path, ".x")) {
		int len = strlen(cur->path) + 4;
		char *outPath = malloc(len);
		ASSERT(outPath);
		memcpy(outPath, cur->path, len - 6);
		strcpy(outPath + len - 6, ".html");
		FILE *out = fopen(outPath, "w");
		ASSERT(out);
		 
	FILE *in = fopen(cur->path, "r");
	ASSERT(in);
	 {
	struct HtmlStatus status = {
		.state = hs_NOTHING_WRITTEN
		
	, .headerLevel = 0
	, .headerNameEnd = NULL

	, .codeOpening = 0
	, .codeIndent = 0
	, .codeSpecial = '\0'
	, .codeNameEnd = NULL

	, .noteInCode = false
	, .noteInBold = false

	};
	char last = '\n';
	for (;;) {
		int ch = fgetc(in);
		 
	if (ch == '#' && last == '\n') {
		if (isOutOfHtmlSpecial(&status) ||
			status.state == hs_IN_HEADER
		) {
			++status.headerLevel;
			if (status.state != hs_IN_HEADER) {
				status.headerState = status.state;
			}
			status.state = hs_IN_HEADER;
			continue;
		}
	}
 
	if (status.state == hs_IN_HEADER) {
		if (ch == '\n') {
			 
	ASSERT(status.headerNameEnd);
	 
	switch (status.headerState) {
		case hs_NOTHING_WRITTEN: {
			 
	fprintf(out, "<!doctype html>\n");
	fprintf(out, "<html lang=\"de\"l>\n");
	fprintf(out, "<head>\n");
	 
	fprintf(
		out, "<meta charset=\"utf-8\">\n"
	);
	fprintf(out, "<title>");
	writeEscaped(out, status.headerName, status.headerNameEnd);
	fprintf(out, "</title>");
	fprintf(
		out, "<link rel=\"stylesheet\" "
		"type=\"text/css\" "
		"href=\"slides/slides.css\">"
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
	writeEscaped(out, status.headerName, status.headerNameEnd);
	fprintf(out, "</h%d>\n", status.headerLevel);
;
	fprintf(out, "<div class=\"slides\">\n");
	fprintf(out, "<div><div>\n");
	 
	fprintf(out, "<h%d>", status.headerLevel);
	writeEscaped(out, status.headerName, status.headerNameEnd);
	fprintf(out, "</h%d>\n", status.headerLevel);
;
	fprintf(out, "</div>\n");
;
			 
	status.state = hs_IN_SLIDE;
	status.headerLevel = 0;
	status.headerNameEnd = NULL;
	status.headerState = hs_IN_SLIDE;
;
			last = ch;
			continue;
		}
	}
 
	if (status.state == hs_IN_HEADER) {
		if (status.headerNameEnd) {
			ASSERT(
				status.headerNameEnd <
					status.headerName + sizeof(
						status.headerName
					) - 1
			);
			*status.headerNameEnd++ = ch;
			last = ch;
			continue;
		}
	}
 
	if (status.state == hs_IN_HEADER) {
		if (! status.headerNameEnd &&
			ch > ' '
		) {
			status.headerNameEnd =
				status.headerName;
			*status.headerNameEnd++ = ch;
			last = ch;
			continue;
		}
	}
 
	if (last == '\n' && ch == '`') {
		if (isOutOfHtmlSpecial(&status) ||
			status.state == hs_IN_CODE
		) {
			++status.codeOpening;
			continue;
		}
	}
 
	if (ch == '\n' && status.codeOpening == 3) {
		status.codeOpening = 0;
		if (isOutOfHtmlSpecial(&status)) {
			if (status.state == hs_IN_SLIDE) {
				fprintf(out, "</div>\n");
			}
			fprintf(out, "<div><div>\n");
			fprintf(out, "<code>\n");
			status.state = hs_IN_CODE;
			last = ch;
			continue;
		} else if (status.state == hs_IN_CODE) {
			fprintf(out, "</code>\n");
			fprintf(out, "</div>\n");
			status.state = hs_IN_SLIDE;
			status.codeIndent = 0;
			status.codeSpecial = '\0';
			last = ch;
			continue;
		}
	}
	status.codeOpening = 0;

	if (status.state == hs_IN_CODE) {
		 
	if (ch == '\n') {
		if (last) {
			writeEscaped(out, &last, &last + 1);
		}
		fprintf(out, "<br/>\n");
			last = ch;
		continue;
	}
 
	if (last == '\n' && ch == '\t') {
		++status.codeIndent;
		continue;
	}
	if (status.codeIndent) {
		fprintf(
			out, "<span class=\"in%d\"></span>", status.codeIndent
		);
		status.codeIndent = 0;
	}

	
	if (ch == '{') {
		switch (last) {
			
	case 'd':
		fprintf(out, "<span class=\"add\">@def(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;
	case 'a':
		fprintf(out, "<span class=\"add\">@add(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;
	case 'r':
		fprintf(out, "<span class=\"add\">@replace(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;

	case 'x':
		fprintf(out, "<span class=\"end\">@end(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;

	case 'e':
		fprintf(out, "<span class=\"expand\">@expand(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;
	case 'E':
		fprintf(out, "<span class=\"expand\">@multiple(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;

	case 'i':
		fprintf(out, "<span class=\"include\">@include(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		status.codeNameEnd = status.codeName;
		break;

	case 't':
		fprintf(out, "<span class=\"type\">");
		status.codeSpecial = last;
		break;

	case 'v':
		fprintf(out, "<span class=\"var\">");
		status.codeSpecial = last;
		break;

	case 'f':
		fprintf(out, "<span class=\"fn\">");
		status.codeSpecial = last;
		break;

	case 'k':
		fprintf(out, "<span class=\"keyword\">");
		status.codeSpecial = last;
		break;

	case 's':
		fprintf(out, "<span class=\"str\">");
		status.codeSpecial = last;
		break;

	case 'n':
		fprintf(out, "<span class=\"num\">");
		status.codeSpecial = last;
		break;

	case 'p':
		fprintf(out, "<span class=\"type\">@priv(<span>");
		status.codeSpecial = last;
		break;

	case 'm':
		fprintf(out, "<span class=\"num\">@magic(<span>");
		status.codeSpecial = last;
		break;

			default: break;
		}
		if (status.codeSpecial) {
			last = 0;
			continue;
		}
	}

	if (ch == '}' && status.codeSpecial) {
		if (last) {
			writeEscaped(out, &last, &last + 1);
			last = 0;
		}
		switch (status.codeSpecial) {
			case 'i': {
				
	ASSERT(status.codeNameEnd <
		status.codeName + sizeof(status.codeName)
	);
	*status.codeNameEnd = '\0';
	while (status.codeNameEnd >= status.codeName && *status.codeNameEnd
		!= '.') {
		--status.codeNameEnd;
	}
	ASSERT(status.codeNameEnd >= status.codeName, "no period");
	*status.codeNameEnd = '\0';
	fprintf(out, "<a href=\"%s.html\">", status.codeName);
	*status.codeNameEnd = '.';
	fprintf(out, "%s</a>)</span>", status.codeName);
	end->_private_link = createSourceElement(status.codeName);
	end = end->_private_link;
	status.codeNameEnd = NULL;

;
				break;
			}
			case 'a': case 'e': case 'E': case 'x':
			case 'r': case 'd': case 'p': case 'm': {
				fprintf(out, ")</span>");
			}
		}
		fprintf(out, "</span>");
		status.codeSpecial = 0;
		continue;
	}
;

	if (ch != EOF && status.codeNameEnd) {
		ASSERT(status.codeNameEnd <
			status.codeName + sizeof(status.codeName)
		);
		*status.codeNameEnd++ = ch;
		continue;
	}
;
		if (last) {
			writeEscaped(out, &last, &last + 1);
		}
		last = ch;
		continue;
	}
 
	if (last == '\n' && status.state == hs_IN_NOTES) {
		if (ch == '*') {
			fprintf(out, "</li><li>\n");
			last = 0;
			continue;
		} else if (ch != ' ' && ch != '\t') {
			fprintf(out, "</li></ul></div>\n");
			status.state = hs_AFTER_SLIDE;
			last = ch;
			continue;
		}
	}
 
	if (last == '\n' && ch == '*') {
		if (isOutOfHtmlSpecial(&status)) {
			if(status.state != hs_IN_SLIDE) {
				fprintf(out, "<div>\n");
			}
			status.state = hs_IN_NOTES;
			fprintf(out, "<ul><li>\n");
			last = '\0';
			continue;
		}
	}
 
	if (ch == '`' && status.state == hs_IN_NOTES) {
		if (last) {
			writeEscaped(out, &last, &last + 1);
		}
		if (status.noteInCode) {
			fprintf(out, "</code>");
		} else {
			fprintf(out, "<code>");
		}
		status.noteInCode = ! status.noteInCode;
		last = 0;
		continue;
	}
 
	if (status.state == hs_IN_NOTES &&
	status.noteInCode) {
		
	if (ch == '{') {
		switch (last) {
			
	case 'd':
		fprintf(out, "<span class=\"add\">@def(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;
	case 'a':
		fprintf(out, "<span class=\"add\">@add(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;
	case 'r':
		fprintf(out, "<span class=\"add\">@replace(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;

	case 'x':
		fprintf(out, "<span class=\"end\">@end(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;

	case 'e':
		fprintf(out, "<span class=\"expand\">@expand(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;
	case 'E':
		fprintf(out, "<span class=\"expand\">@multiple(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;

	case 'i':
		fprintf(out, "<span class=\"include\">@include(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		status.codeNameEnd = status.codeName;
		break;

	case 't':
		fprintf(out, "<span class=\"type\">");
		status.codeSpecial = last;
		break;

	case 'v':
		fprintf(out, "<span class=\"var\">");
		status.codeSpecial = last;
		break;

	case 'f':
		fprintf(out, "<span class=\"fn\">");
		status.codeSpecial = last;
		break;

	case 'k':
		fprintf(out, "<span class=\"keyword\">");
		status.codeSpecial = last;
		break;

	case 's':
		fprintf(out, "<span class=\"str\">");
		status.codeSpecial = last;
		break;

	case 'n':
		fprintf(out, "<span class=\"num\">");
		status.codeSpecial = last;
		break;

	case 'p':
		fprintf(out, "<span class=\"type\">@priv(<span>");
		status.codeSpecial = last;
		break;

	case 'm':
		fprintf(out, "<span class=\"num\">@magic(<span>");
		status.codeSpecial = last;
		break;

			default: break;
		}
		if (status.codeSpecial) {
			last = 0;
			continue;
		}
	}

	if (ch == '}' && status.codeSpecial) {
		if (last) {
			writeEscaped(out, &last, &last + 1);
			last = 0;
		}
		switch (status.codeSpecial) {
			case 'i': {
				
	ASSERT(status.codeNameEnd <
		status.codeName + sizeof(status.codeName)
	);
	*status.codeNameEnd = '\0';
	while (status.codeNameEnd >= status.codeName && *status.codeNameEnd
		!= '.') {
		--status.codeNameEnd;
	}
	ASSERT(status.codeNameEnd >= status.codeName, "no period");
	*status.codeNameEnd = '\0';
	fprintf(out, "<a href=\"%s.html\">", status.codeName);
	*status.codeNameEnd = '.';
	fprintf(out, "%s</a>)</span>", status.codeName);
	end->_private_link = createSourceElement(status.codeName);
	end = end->_private_link;
	status.codeNameEnd = NULL;

;
				break;
			}
			case 'a': case 'e': case 'E': case 'x':
			case 'r': case 'd': case 'p': case 'm': {
				fprintf(out, ")</span>");
			}
		}
		fprintf(out, "</span>");
		status.codeSpecial = 0;
		continue;
	}
;
	}
 
	if (ch == '*' && last == '*' && status.state == hs_IN_NOTES) {
		if (status.noteInBold) {
			fprintf(out, "</b>");
		} else {
			fprintf(out, "<b>");
		}
		status.noteInBold = ! status.noteInBold;
		last = 0;
		continue;
	}
 
	if (status.state == hs_IN_NOTES) {
		if (last) {
			writeEscaped(out, &last, &last + 1);
		}
		last = ch;
		continue;
	}
;
		if (ch == EOF) { break; }
		last = ch;
	}
} ;
	fclose(in);
;
		fclose(out);
		free(outPath);
	}
;
		struct SourceElement *next =
			cur->_private_link;
		free(cur);
		cur = next;
	}
;

	}
