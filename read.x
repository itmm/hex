# Read the input files
* Processes the input files line by line

```
@Def(includes)
	#include <fstream>
	#include <string>
@End(includes)
```

```
@Add(global elements)
	@Put(input prereqs);
	class Input {
		private:
			std::ifstream file;
			@Put(private elements);
		public:
			const std::string name;
			@Put(additional elements);
			@Put(input methods);
	};
@End(global elements)
```
* Die `Input`-Klasse enthält den Dateinamen der Eingabe-Dateien
* Zusätzlich kann sie eine offene Datei enthalten
* Alle Eingabe-Dateien, die während einer Inkludierungs-Kaskade
  eingebunden werden bleiben offen, damit an der richtigen Stelle weiter
  gearbeitet werden kann
* Weiter Attribute und Methoden können später definiert werden


```
@Def(input methods)
	Input(const std::string &name):
		file { name.c_str() },
		@Put(private input constr)
		name { name }
	{}
@End(input methods)
```
* Im Konstruktor werden Datei und Name gesetzt
* Später eingeführte Attribute können auch dann initialisiert werden
