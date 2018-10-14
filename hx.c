
						
						
						#include <stdio.h>
						#include <stdlib.h>
						
						#include <stdlib.h>
						
						#include <string.h>
						
						#include <stdbool.h>
						;
						
						#define ASSERT(COND) \
						if (! (COND)) { \
						fprintf(stderr, \
						"%s:%d FAILED\n", \
						__FILE__, __LINE__); \
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
						};
						
						struct Input *input = NULL;
						
						void pushFile (FILE *file) {
						struct Input *i =
						malloc(sizeof(struct Input));
						
						;
						i->link = input;
						i->file = file;
						input = i;
						}
						
						void pushPath(const char *path) {
						FILE *f = fopen(path, "r");
						;
						pushFile(f);
						}
						
						int nextCh() {
						int ch = EOF;
						while (input) {
						ch = fgetc(input->file);
						if (ch != EOF) { break; }
						struct Input *n = input->link;
						fclose(input->file);
						free(input);
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
							
						
						struct Consumer {
						int (* put)(
						struct Consumer *consumer, int ch
						);
						};
						
						void putToConsumer(
						struct Consumer *c, int ch
						) {
						ASSERT(c); ASSERT(c->put);
						c->put(c, ch);
						}
						
						struct FileConsumer {
						struct Consumer consumer;
						FILE * file;
						};
						
						int consumeInFile(
						struct Consumer *c, int ch
						) {
						struct FileConsumer *fc = (void *) c;
						ASSERT(fc); ASSERT(fc->file);
						if (ch != EOF) {
						ch = fputc(ch, fc->file);
						} else {
						fclose(fc->file);
						fc->file = NULL;
						}
						return ch;
						}
						
						void setupFileConsumer(
						struct FileConsumer *fc, FILE *f
						) {
						ASSERT(fc); ASSERT(f);
						fc->file = f;
						fc->consumer.put = consumeInFile;
						}
						
						struct BufferConsumer {
						struct Consumer consumer;
						struct Buffer buffer;
						};
						
						int consumeInBuffer(
						struct Consumer *c, int ch
						) {
						struct BufferConsumer *bc = (void *) c;
						ASSERT(bc);
						int cr = ch != EOF ? ch : '\0';
						addToBuffer(&bc->buffer, cr);
						return ch;
						}
						
						void setupBufferConsumer(
						struct BufferConsumer *bc
						) {
						ASSERT(bc);
						memset(
						&bc->buffer, 0,
						sizeof(bc->buffer)
						);
						bc->consumer.put = consumeInBuffer;
						}
						
						
						
						struct MacroEntry;
						
						struct Macro {
						struct Macro *link;
						struct MacroEntry *firstEntry;
						struct MacroEntry *lastEntry;
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
						
						memcpy(
						result->name, nameBegin,
						nameLength
						);
						result->name[nameLength] = '\0';
						;
						return result;
						}
						
						void freeMacro(
						struct Macro *macro
						) {
						while (macro) {
						struct Macro *link =
						macro->link;
						
						freeMacroEntry(macro->firstEntry);
						;
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
						
						void serializeMacro(
						struct Macro *macro,
						struct Consumer *consumer
						) {
						ASSERT(macro);
						ASSERT(consumer);
						
						struct MacroEntry *entry =
						macro->firstEntry;
						for (; entry; entry = entry->link) {
						
						if (getMacroEntryValueSize(entry)) {
						const char *cur = entry->value;
						const char *end = entry->valueEnd;
						for (; cur < end; ++cur) {
						putToConsumer(consumer, *cur);
						}
						}
						;
						if (entry->macro) {
						serializeMacro(
						entry->macro, consumer
						);
						}
						}
						;
						}
						
						void testMacro(struct Macro *
						macro, const char *expected
						) {
						struct BufferConsumer bc;
						setupBufferConsumer(&bc);
						
						serializeMacro(macro, &bc.consumer);
						putToConsumer(&bc.consumer, EOF);
						ASSERT(strcmp(
						expected, bc.buffer.buffer
						) == 0);
						;
						eraseBuffer(&bc.buffer);
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
						
						
						struct EntityConsumer {
						struct Consumer consumer;
						struct Consumer *subConsumer;
						char prefix[6];
						};
						
						int consumeEntities(
						struct Consumer *consumer, int ch
						) {
						struct EntityConsumer *ec =
						(void *) consumer;
						bool processed = false;
						
						if (! processed && ch == '&') {
						if (*ec->prefix) {
						 {
						char *cur = ec->prefix;
						for (; *cur; ++cur) {
						putToConsumer(ec->subConsumer, *cur);
						*cur = '\0';
						}
						} ;
						}
						*ec->prefix = ch;
						processed = true;
						}
						
						if (
						! processed && *ec->prefix && ch != EOF
						) {
						if (ch == ';') {
						 {
						bool expanded = false;
						
						if (strcmp("&gt", ec->prefix) == 0) {
						putToConsumer(ec->subConsumer, '>');
						expanded = true;
						}
						if (strcmp("&lt", ec->prefix) == 0) {
						putToConsumer(ec->subConsumer, '<');
						expanded = true;
						}
						
						if (strcmp("&amp", ec->prefix) == 0) {
						putToConsumer(ec->subConsumer, '&');
						expanded = true;
						}
						;
						if (! expanded) {
						 {
						char *cur = ec->prefix;
						for (; *cur; ++cur) {
						putToConsumer(ec->subConsumer, *cur);
						*cur = '\0';
						}
						} ;
						putToConsumer(ec->subConsumer, ch);
						}
						} ;
						
						memset(
						ec->prefix, 0, sizeof(ec->prefix)
						);
						;
						} else {
						
						char *cur = ec->prefix + 1;
						const char *end = ec->prefix +
						sizeof(ec->prefix) - 1;
						for (; cur < end && *cur; ++cur) {}
						if (cur < end) {
						*cur = ch;
						} else {
						 {
						char *cur = ec->prefix;
						for (; *cur; ++cur) {
						putToConsumer(ec->subConsumer, *cur);
						*cur = '\0';
						}
						} ;
						}
						processed = true;
						;
						}
						processed = true;
						}
						
						if (! processed) {
						if (ch == EOF) {
						 {
						char *cur = ec->prefix;
						for (; *cur; ++cur) {
						putToConsumer(ec->subConsumer, *cur);
						*cur = '\0';
						}
						} ;
						}
						putToConsumer(ec->subConsumer, ch);
						}
						;
						return processed ? ch : EOF;
						}
						
						void setupEntityConsumer(
						struct EntityConsumer *ec,
						struct Consumer *sc
						) {
						ASSERT(ec); ASSERT(sc);
						ec->subConsumer = sc;
						ec->consumer.put = consumeEntities;
						
						memset(
						ec->prefix, 0, sizeof(ec->prefix)
						);
						;
						}
						
						void testEntityConsumer(
						const char *source,
						const char *expected
						) {
						ASSERT(source); ASSERT(expected);
						struct BufferConsumer bc;
						struct EntityConsumer ec;
						setupBufferConsumer(&bc);
						setupEntityConsumer(&ec, &bc.consumer);
						
						for (; *source; ++source) {
						putToConsumer(&ec.consumer, *source);
						}
						putToConsumer(&ec.consumer, EOF);
						;
						ASSERT(strcmp(
						expected, bc.buffer.buffer
						) == 0);
						}
						
						
						int main(
						int argc, const char **argv
						) {
						
						 {
						 {
						struct BufferConsumer bc;
						setupBufferConsumer(&bc);
						struct Consumer *c = &bc.consumer;
						putToConsumer(c, 'a');
						putToConsumer(c, 'b');
						putToConsumer(c, EOF);
						ASSERT(
						strcmp("ab", bc.buffer.buffer) == 0
						);
						} 
						}  {
						
						testEntityConsumer("a bc", "a bc");
						
						testEntityConsumer("a&lt;b", "a<b");
						
						testEntityConsumer("a&gt;b", "a>b");
						
						testEntityConsumer("a&amp;b", "a&b");
						
						testEntityConsumer("a&copy;b", "a&copy;b");
						
						testEntityConsumer("a&l&lt;b", "a&l<b");
						
						testEntityConsumer("a&amp;&amp;b", "a&&b");
						
						testEntityConsumer("a&b", "a&b");
						
						} ;
						
						if(argc > 1) {
						pushPath(argv[1]);
						} else {
						pushFile(stdin);
						}
						;
						
						
						struct MacroMap macros = {};
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
						if (last == 'a' || last == 'i') {
						openCh = last;
						nameCur = name;
						break;
						}
						}
						}  {
						if (macro) {
						bool valid = false;
						
						static const char valids[] =
						"123456bfvsntkxe";
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
						
						if (openCh == 'a') {
						ASSERT(! macro);
						macro = getMacroInMap(
							&macros, name, nameCur
						);
						processed = true;
						}
						
						if (openCh == 'x') {
						ASSERT(macro);
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
						ASSERT(! macro);
						pushPath(name);
						processed = true;
						}
						
						if (openCh == 'e') {
						ASSERT(macro);
						
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
						addMacroToMacro(
						macro, sub);
						processed = true;
						}
						
						if (! processed) {
						ASSERT(macro);
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
						ASSERT(nameCur < nameEnd);
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
						;
						 {
						const char name[] = "MAIN";
						struct Macro * macro = getMacroInMap(
						&macros, name,
						name + sizeof(name) - 1
						);
						struct FileConsumer fc;
						setupFileConsumer(&fc, stdout);
						struct EntityConsumer ec;
						setupEntityConsumer(&ec, &fc.consumer);
						serializeMacro(
						macro, &ec.consumer
						);
						} ;
						;
						
						}
						