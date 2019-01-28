# Logging
* In dieser Datei wird der Logging-Mechanismus beschrieben
* Es wird nur etwas geloggt, wenn ein Fehler aufgetreten ist

```
D{includes}
	#include <iostream>
	#include <exception>
x{includes}
```
* Geschrieben wird nach `std::cerr`, das in `<iostream>` definiert ist
* Nach dem Herausschreiben der Nachricht wird eine Exception generiert

```
D{define logging}
	class LogContainer;
	#define ASSERT(COND) \
		((COND) ? \
			LogContainer { \
				false, true \
			} : \
			LogContainer { \
				__FILE__, __LINE__, \
				#COND \
			} \
		)
x{define logging}
```
* Loggen wird über ein Makro realisiert
* Nur so kann die aktuelle Position im Source-Code ermittelt werden
* Das Loggen selbst übernimmt der `LogContainer`, um ein
  Stream-Interface anzubieten
* Wenn die Bedingung wahr ist, wird nicht geloggt
* Ansonsten wird eine Fehlermeldung ausgegeben
* Und das Programm beendet
* Datei und Zeile des Tests wird ausgegeben

```
A{define logging}
	class LogContainer {
		public:
			bool log;
			bool first;
			e{log container};
	};
x{define logging}
```
* Der `LogContainer` enthält zwei Flags
* `log` signalisiert, ob eine Nachricht geschrieben werden soll
* `first` signalisiert, dass noch kein Parameter herausgeschrieben
  wurde
* Beim ersten Parameter wird noch ein Doppelpunkt ausgegeben

```
d{log container}
	LogContainer(bool l, bool f):
		log { l }, first { f }
	{
	}
x{log container}
```
* Die beiden Parameter können in einem Konstruktor direkt gesetzt
  werden

```
a{log container}
	LogContainer(
		const char *file, int line,
		const char *cond
	): log { true }, first { true } {
		std::cerr << '[' << file <<
			':' << line << "] " <<
			cond << " FAILED";
	}
x{log container}
```
* Alternativ kann der `LogContainer` mit den der Position eines
  Fehlers im Source-Code initialisiert werden
* In diesem Fall soll die Nachricht geloggt werden und es wurden
  noch keine Parameter ausgegeben

```
a{log container}
	LogContainer(LogContainer &&l):
		log { l.log },
		first { l.first }
	{
		l.log = false;
	}
x{log container}
```
* Durch den Move-Constructor wird sichergestellt, dass nur eine
  Instanz wirklich loggt
* Im Destruktor dieser Instanz wird ein Zeilenvorschub ausgegeben und
  eine Exception geworfen

```
a{log container}
	~LogContainer() noexcept(false) {
		if (log) {
			std::cerr << '\n';
			throw std::exception();
		}
	}
x{log container}
```
* Wenn Logging aktiv ist, wird ein Zeilenvorschub ausgegeben
* Und eine Exception geworfen
* Das muss extra angezeigt werden, da Exceptions im Destructor
  verboten sind

```
A{define logging}
	template<typename T>
	inline LogContainer operator<<(
		const LogContainer &lc, T t
	) {
		if (lc.log) {
			e{log t};
		}
		return LogContainer {
			false, false
		};
	}
x{define logging}
```
* Über diese Template-Funktion können weitere Parameter in die
  Fehlerausgabe geschoben werden
* Falls die Bedingung falsch war und Logging aktiviert ist
* Vor dem ersten Argument wird ein Doppelpunkt ausgegeben

```
d{log t}
	if (lc.first) {
		std::cerr << ": ";
	}
	std::cerr << t;
	return LogContainer {
		true, false 
	};
x{log t}
```

