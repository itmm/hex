// @expand(global elements)
	// @expand(includes);
		#include <stdlib.h>
		#include <stdio.h>
		#include <string.h>

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
				// @expand(assert macro name);
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

