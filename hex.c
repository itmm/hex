// @expand(global elements)
	// @expand(includes);
		#include <stdlib.h>
		#include <stdio.h>
		#include <string.h>
		#include <stdbool.h>

	// @expand(define logging);
		#define ASSERT(COND) \
			if (! (COND)) { \
				fprintf(stderr, \
					"%s:%d FAILED\n", \
					__FILE__, __LINE__ \
				); \
				exit(EXIT_FAILURE); \
			}

	// @expand(forward declarations)
		struct MacroEntry;
		void freeMacroEntry(
			struct MacroEntry *entry
		);

	
	// @expand(define macro);
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
			// @expand(allocate macro on heap);
				ASSERT(nameBegin);
				ASSERT(nameBegin <= nameEnd);
				int nameLength =
					nameEnd - nameBegin;
				int macroSize = sizeof(struct Macro)
					+ nameLength + 1;
				result = malloc(macroSize);
				ASSERT(result);

			result->link = NULL;
			result->firstEntry = NULL;
			//@expand(copy macro name);
				memcpy(
					result->name, nameBegin,
					nameLength
				);
				result->name[nameLength] = '\0';

			return result;
		}

		void freeMacro(
			struct Macro *macro
		) {
			while (macro) {
				struct Macro *link =
					macro->link;
				// @expand(free macros entries);
					freeMacroEntry(macro->firstEntry);

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
			// @expand(allocate entry on heap);
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

			result->link = NULL;
			// @expand(copy entry value);
				if (valueBegin) {
					memcpy(
						result->value, valueBegin,
						valueLength
					  );
				}
				result->valueEnd =
					result->value + valueLength;
				result->macro = macro;

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
			// @expand(assert add entry);
				ASSERT(macro);
				ASSERT(entry);
				ASSERT(! entry->link);

			if (macro->firstEntry) {
				// @expand(append entry);
					macro->lastEntry->link = entry;
					macro->lastEntry = entry;

			} else {
				// @expand(set first entry);
					macro->firstEntry = entry;
					macro->lastEntry = entry;
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

		// @expand(define cycle check);
			bool isMacroInMacro(
				struct Macro *needle,
				struct Macro *haystack
			) {
				ASSERT(needle);
				ASSERT(haystack);
				// @expand(check cycle macro);
					if (needle == haystack) {
						return true;
					}

				// @expand(check cycle entries);
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

				return false;
			}

		void addMacroToMacro(
			struct Macro *macro,
			struct Macro *child
		) {
			ASSERT(macro);
			ASSERT(child);
			// @expand(avoid macro cycles);
				ASSERT(! isMacroInMacro(
					macro, child
				));

			// @expand(reuse last entry);
				if (macro->firstEntry &&
					! macro->lastEntry->macro
				) {
					macro->lastEntry->macro = child;
					return;
				}

			// @expand(add macro entry);
				struct MacroEntry *entry =
					allocMacroEntry(
						child, NULL, NULL
					);
				addEntryToMacro(macro, entry);

		}

		typedef void Consumer(
			const char *begin,
			const char *end,
			void *context
		);

		void serializeMacro(
			struct Macro *macro,
			Consumer consumer,
			void *context
		) {
			ASSERT(macro);
			ASSERT(consumer);
			// @expand(iterate entries);
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

					if (entry->macro) {
						serializeMacro(entry->macro,
							consumer, context
						);
					}
				}
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
			// @expand(add to test consumer);
				struct TestConsumerContext *ctx =
					context;
				int length = end - begin;
				ASSERT(ctx->current + length <= ctx->end);
				memcpy(ctx->current, begin, length);
				ctx->current += length;
		}

		struct TestConsumerContext *
		allocTestConsumerContext(int size) {
			ASSERT(size >= 0);
			struct TestConsumerContext *
				context = malloc(size + sizeof(
							struct TestConsumerContext));
			ASSERT(context);
			// @expand(init test consumer);
				 context->current = context->buffer;
				 context->end = context->buffer + size;

			return context;
		}

		void testMacro(struct Macro *macro,
			const char *expected
		) {
			int size = strlen(expected);
			struct TestConsumerContext *context =
				allocTestConsumerContext(size);
			serializeMacro(macro,
				testConsumer, context);
			ASSERT(context->current -
					context->buffer == size);
			ASSERT(memcmp(expected,
				context->buffer, size) == 0);
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
			// @expand(insert in slot);
				int hash = calcHash(begin, end);
				macro->link = map->macros[hash];
				map->macros[hash] = macro; 

			return macro;
		}

		struct Macro *findMacroInMap(
			struct MacroMap *map,
			const char *begin,
			const char *end
		) {
			ASSERT(map);
			struct Macro *macro = NULL;
			// @expand(find macro in slot);
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

			return macro;
		}


int main(
	int argc,
	const char **argv
) {
	// @expand(perform unit-tests);
		// @expand(macro unit tests);
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
				// @expand(add two entries);
					macro = allocTestMacro("");
					first = allocEmptyMacroEntry();
					second = allocEmptyMacroEntry();

					addEntryToMacro(macro, first);
					addEntryToMacro(macro, second);

				// @expand(check first of 2);
					ASSERT(macro->firstEntry == first);

				freeMacro(macro);
			}
			{
				struct Macro *macro = NULL;
				struct MacroEntry *first;
				struct MacroEntry *second;
				// @expand(add two entries);
					macro = allocTestMacro("");
					first = allocEmptyMacroEntry();
					second = allocEmptyMacroEntry();

					addEntryToMacro(macro, first);
					addEntryToMacro(macro, second);

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
			}


	// @expand(process arguments);
		FILE *input = stdin;

	// @expand(process source file);
		{
			// @expand(additional read vars);
				struct Macro *macro = NULL;
				char openCh = '\0';
				char name[128];
				char *nameCur = NULL;
				const char *nameEnd = name +
					sizeof(name);

			int last = fgetc(input);
			int ch = fgetc(input);
			while (ch != EOF) {
				// @expand(process current char);
					switch (ch) {
						case '{':
							// @expand(process open brace);
							{
								if (! macro) {
									if (last == 'a') {
										openCh = last;
										nameCur = name;
									}
								}
							}
							break;
						case '}':
							// @expand(process close brace);
							{
								if (nameCur) {
									// @expand(process macro name);
									nameCur = NULL;
								}
							}
							break;
						default:
							// @expand(process other char)
							{
								if (nameCur) {
									ASSERT(nameCur < nameEnd);
									*nameCur++ = ch;
									break;
								}
							}
							{
								if (macro) {
									putchar(last);
								}
							}
							;
					}

				last = ch; ch = fgetc(input);
			}
		}

	// @expand(write HTML file);
	// @expand(serialize fragments);
	// @expand(compile program);
}

