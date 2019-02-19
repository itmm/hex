# Logging
* In dieser Datei wird der Logging-Mechanismus beschrieben
* Es wird nur etwas geloggt, wenn ein Fehler aufgetreten ist

```
@Def(includes)
	#include <iostream>
	#include <exception>
@end(includes)
```
* Geschrieben wird nach `std::cerr`, das in `<iostream>` definiert ist
* Nach dem Herausschreiben der Nachricht wird eine Exception generiert

```
@Def(define logging)
	#define ASSERT(COND) \
		if (! (COND)) { \
			std::cerr << __FILE__ << \
				':' << __LINE__ << \
				"] " << #COND << \
				" FAILED\n"; \
			throw std::exception(); \
		}
@end(define logging)
```
* Loggen wird über ein Makro realisiert
* Nur so kann die aktuelle Position im Source-Code ermittelt werden
* Wenn die Bedingung wahr ist, wird nicht geloggt
* Ansonsten wird eine Fehlermeldung ausgegeben
* Und das Programm beendet
* Datei und Zeile des Tests wird ausgegeben

```
@Add(define logging)
	#define ASSERT_MSG(COND, MSG) \
		if (! (COND)) { \
			std::cerr << __FILE__ << \
				':' << __LINE__ << \
				"] " << #COND << \
				" FAILED: " << MSG << \
				'\n'; \
			throw std::exception(); \
		}
@end(define logging)
```
* Zusätzlich kann eine Nachricht mitgegegben werden

