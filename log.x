# Logging

```
D{includes}
	#include <iostream>
	#include <exception>
x{includes}
```

```
D{define logging}
	class LogContainer {
		public:
			bool log;
			bool first;
			LogContainer(const char *file, int line, const char *cond): log { true }, first { true } {
				std::cerr << '[' << file << ':' << line << "] " << cond << " FAILED";
			}
			LogContainer(bool l, bool f):
				log {l}, first {f}
			{
			}
			LogContainer(LogContainer &&l):
				log {l.log}, first {l.first}
			{
				l.log = false;
			}
			~LogContainer() noexcept(false) {
				if (log) {
					std::cerr << std::endl;
					throw std::exception();
				}
			}
	};

	template<typename T>
	inline LogContainer operator<<(
		LogContainer lc, T t
	) {
		if (lc.log) {
			if (lc.first) { std::cerr << ": "; }
			std::cerr << t;
			return LogContainer(true, false);
		} else {
			return LogContainer(false, false);
		}
	}
x{define logging}
```

```
A{define logging}
	#define ASSERT(COND) \
		((COND) ? LogContainer { false, true } : \
			LogContainer { __FILE__, __LINE__, #COND } \
		)
x{define logging}
```
* Wenn Bedingung falsch ist, wird Fehlermeldung ausgegeben
* Und das Programm beendet
* Datei und Zeile des Tests wird ausgegeben
* Eine variable Anzahl von weiteren Parametern kann mit ausgegeben
  werden

