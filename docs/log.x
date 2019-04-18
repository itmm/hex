# Logging
* Only if the condition is `false` (or `0`, or `NULL`) something will be
  logged
* There are no log levels
* The macro will throw an `std::exception` if the condition was not met
* This normally terminates the running program

```
@Add(includes)
	#include @s(<iostream>)
	#include @s(<exception>)
@End(includes)
```
* `std::cerr` from `@s(<iostream>)` is used to write log messages
* A standard exception from `@s(<exception>)` is used to indicate failed
  conditions

```
@Def(frag prereqs)
	#define ASSERT(COND) \
		if (! (COND)) { \
			@mul(log preamble); \
			@mul(log newline); \
			@mul(throw); \
		}
@End(frag prereqs)
```
* The plain `@f(ASSERT)` macro does nothing if the condition evaluates
  to `true`
* Otherwise the position of the line with the assertion will be written
* and a small message
* No details are given and the line is terminated with a newline
* Then an exception is raised

```
@Add(frag prereqs)
	#define ASSERT_MSG(COND, MSG) \
		if (! (COND)) { \
			@mul(log preamble); \
			std::cerr << ": " << MSG; \
			@mul(log newline); \
			@mul(throw); \
		}
@End(frag prereqs)
```
* The macro `@f(ASSERT_MSG)` has an additional argument that the macro
  writes after the preamble
* `MSG` can be multiple parameters concatenated by `<<`

```
@def(log preamble) \
	std::cerr << \
		__FILE__ << ':' << __LINE__ << \
		' ' << #COND << " FAILED"; \
@end(log preamble)
```
* The preamble starts with the position in the format `filename:line`
* This is recognized by a number of editors
* Afterwards a short error message is written

```
@def(log newline) \
	std::cerr << '\n'; \
@end(log newline)
```
* A simple newline terminates an error message

```
@def(throw) \
	throw std::exception(); \
@end(throw)
```
* The macros throw a generic `std::exception` if the condition was not
  met
