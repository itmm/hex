# Logging

```
D{includes}
	#include <iostream>
	#include <cstdlib>
x{includes}
```

```
D{define logging}
	#define ASSERT(COND, ...) \
		if (! (COND)) { \
			std::cerr << __FILE__ << ':' \
				<< __LINE__ << ' ' \
				<< #COND << " FAILED: "; \
			failSuffix(__VA_ARGS__); \
			std::exit(EXIT_FAILURE); \
		}
x{define logging}
```
* Wenn Bedingung falsch ist, wird Fehlermeldung ausgegeben
* Und das Programm beendet
* Datei und Zeile des Tests wird ausgegeben
* Eine variable Anzahl von weiteren Parametern kann mit ausgegeben
  werden

```
A{define logging}
	inline void failSuffix() {
		std::cerr << std::endl;
	}
x{define logging}
```
* Die Ausgabe der zusätzlichen Parameter erfolgt über variadic
  Funktionen
* Im einfachsten Fall wird nur ein Zeilenumbruch ausgegeben

```
A{define logging}
	template<typename T, typename... Args>
	inline void failSuffix(
		const T& a, Args... args
	) {
		std::cerr << a;
		failSuffix(args...);
	}
x{define logging}
```
* Bei einer variablen Anzahl von Parametern wird zuerst der erste
  Parameter ausgegeben
* Dann wird die Suffix-Funktion für den Rest mit weniger Parametern
  aufgerufen
* wenn es keine Parameter mehr gibt, bricht die Kette ab

