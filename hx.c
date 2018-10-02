
						
						
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
						
						void pushPath(const char *path) {
						FILE *f = fopen(path, "r");
						;
						pushFile(f);
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
						
						typedef void (* Consumer)(
						const char *begin,
						const char *end,
						void *context
						);
						
						void serializeMacro(
						struct Macro *macro,
						Consumer consumer,
						void * context
						) {
						ASSERT(macro);
						ASSERT(consumer);
						
						struct MacroEntry *entry =
						macro->firstEntry;
						for (; entry; entry = entry->link) {
						
						if (getMacroEntryValueSize(entry)) {
						consumer(
						entry->value,
						entry->valueEnd,
						context
						);
						}
						;
						if (entry->macro) {
						serializeMacro(entry->macro,
						consumer, context
						);
						}
						}
						;
						}
						
						struct TestConsumerContext {
						char *current;
						const char *end;
						char buffer[];
						};
						
						void testConsumer (
						const char *begin,
						const char *end,
						void *context
						) {
						ASSERT(begin);
						ASSERT(begin <= end);
						ASSERT(context);
						
						struct TestConsumerContext *ctx =
						context;
						int length = end - begin;
						ASSERT(
						ctx->current + length <= ctx->end
						);
						memcpy(ctx->current, begin, length);
						ctx->current += length;
						;
						}
						
						struct TestConsumerContext *
						allocTestConsumerContext(int size) {
						ASSERT(size >= 0);
						struct TestConsumerContext *
						context = malloc(size + sizeof(
						struct TestConsumerContext));
						ASSERT(context);
						
						context->current =
						context->buffer;
						context->end =
						context->buffer + size;
						;
						return context;
						}
						
						void testMacro(struct Macro *
						macro, const char *expected
						) {
						int size = strlen(expected);
						struct TestConsumerContext *
						context =
						allocTestConsumerContext(
						size);
						
						serializeMacro(macro,
						testConsumer, context);
						ASSERT(context->current -
						context->buffer == size);
						ASSERT(memcmp(expected,
						context->buffer, size) == 0);
						;
						free(context);
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
							
						
						struct EntityContext {
						char prefix[6];
						Consumer subConsumer;
						void *subContext;
						};
						
						void entityConsumer(
						const char *begin,
						const char *end,
						void *context
						) {
						struct EntityContext *ec =
						(void *) context;
						for (; begin < end; ++begin) {
						
						if (*ec->prefix) {
						if (*begin == ';') {
						;
						
						memset(
						ec->prefix, 0,
						sizeof(ec->prefix)
						);
						;
						} else {
						;
						}
						break;
						}
						
						if (*begin == '&') {
						*ec->prefix = *begin;
						break;
						}
						
						ec->subConsumer(
						begin, begin + 1, ec->subContext
						);
						;
						}
						}
						
						
						
						void sourceConsumer(
						const char *begin,
						const char *end,
						void *context
						) {
						printf(
						"%.*s",
						(int) (end - begin),
						begin
						);
						}
						
						
						int main(
						int argc, const char **argv
						) {
						
						;
						
						pushFile(stdin);
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
						"12345bfvsntkxe";
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
						struct Macro * macro =
						getMacroInMap(
						&macros, name,
						name + sizeof(name) - 1
						);
						serializeMacro(
						macro, sourceConsumer, NULL
						);
						} ;
						;
						
						}
						