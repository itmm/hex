// @expand(global elements)
	// @expand(includes);
		#include <stdlib.h>
		#include <stdio.h>

	// @expand(define logging);
		#define ASSERT_MEM(PTR) \
			if (! (PTR)) { \
				fprintf(stderr, \
					"%s:%d OUT OF MEM\n", \
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
				int nameLength =
					nameEnd - nameBegin;
				int macroSize = sizeof(struct Macro)
					+ nameLength + 1;
				result = malloc(macroSize);
				ASSERT_MEM(result);

			result->link = NULL;
			result->firstEntry = NULL;
			//@expand(copy macro name);
			return result;
		}

int main(
	int argc,
	const char **argv
) {
	// @expand(perform unit-tests);
	// @expand(process arguments);
	// @expand(process HTML file);
	// @expand(serialize fragments);
	// @expand(compile program);
}

