# Logging
* only if the condition is `false` (or `0`, or `NULL`) something will be
  logged
* there are no log levels
* the macro will throw an `std::exception` if the condition was not met
* this normally terminates the running program

```
@Add(includes)
	#include @s(<iostream>)
	#include @s(<exception>)
@End(includes)
```
* `std::cerr` from `@s(<iostream>)` is used to write log messages
* a standard exception from `@s(<exception>)` is used to indicate failed
  conditions

```
@Def(frag prereqs)
	#define ASSERT(COND) \
		if (! (COND)) { \
			@mul(log preamble) \
			@mul(log newline) \
			@mul(throw) \
		}
@End(frag prereqs)
```
* the plain `@f(ASSERT)` macro does nothing if the condition evaluates
  to `true`
* otherwise the position of the line with the assertion will be written
* and a small message
* no details are given and the line is terminated with a newline
* then an exception is raised

```
@Add(frag prereqs)
	#define ASSERT_MSG(COND, MSG) \
		if (! (COND)) { \
			@mul(log preamble) \
			std::cerr << ": " << MSG; \
			@mul(log newline) \
			@mul(throw) \
		}
@End(frag prereqs)
```
* the macro `@f(ASSERT_MSG)` has an additional argument that the macro
  writes after the preamble
* `MSG` can be multiple parameters concatenated by `<<`

```
@Add(frag prereqs)
	#define WARN_MSG(MSG) \
		std::cerr << __FILE__ << ':' << \
			__LINE__ << ' '; \
		std::cerr << MSG; \
		@mul(log newline)
@end(frag prereqs)
```
* a warning writes the same message, as an assertion
* but it will not terminate the program

```
@def(log preamble) \
	std::cerr << \
		__FILE__ << ':' << __LINE__ << \
		' ' << #COND << " FAILED"; \
@end(log preamble)
```
* the preamble starts with the position in the format `filename:line`
* this is recognized by a number of editors
* afterwards a short error message is written

```
@def(log newline) \
	std::cerr << '\n'; \
@end(log newline)
```
* a simple newline terminates an error message

```
@def(throw) \
	throw std::exception(); \
@end(throw)
```
* the assert macros throw a generic `std::exception` if the condition
  was not met

