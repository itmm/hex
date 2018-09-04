// @expand(global elements)
	// @expand(includes);
		#include <stdlib.h>
		#include <stdio.h>
		#include <string.h>

		struct MacroEntry;
		void freeMacroEntry(
			struct MacroEntry *entry
		);

	// @expand(define logging);
		#define ASSERT(COND) \
			if (! (COND)) { \
				fprintf(stderr, \
					"%s:%d FAILED\n", \
					__FILE__, __LINE__ \
				); \
				exit(EXIT_FAILURE); \
			}

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

	// @expand(process arguments);
	// @expand(process HTML file);
	// @expand(serialize fragments);
	// @expand(compile program);
}

