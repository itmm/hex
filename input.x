# Eingabe-Dateien

```
A{includes}
	#include <fstream>
	#include <iostream>
	#include <memory>
	#include <vector>
x{includes}
```
* Aus `memory` wird `unique_ptr` verwendet
* `vector` ist ein Container für Source-Dateien

```
A{global elements}
	class Input {
		private:
			std::ifstream file;
			e{private input elements};
		public:
			const std::string name;
			e{additional input elements};
			e{input methods};
	};
x{global elements}
```
* Die `Input`-Klasse enthält den Dateinamen der Eingabe-Dateien
* Zusätzlich kann sie eine offene Datei enthalten
* Alle Eingabe-Dateien, die während einer Inkludierungs-Kaskade
  eingebunden werden bleiben offen, damit an der richtigen Stelle weiter
  gearbeitet werden kann
* Weiter Attribute und Methoden können später definiert werden

```
d{input methods}
	Input(
		const std::string &name
	):
		file { name.c_str() },
		e{private input constr}
		name { name }
	{
	}
x{input methods}
```
* Im Konstruktor werden Datei und Name gesetzt
* Später eingeführte Attribute können auch dann initialisiert werden

```
a{input methods}
	bool getLine(std::string &line) {
		if (file.is_open()) {
			if (std::getline(file, line)) {
				e{line read};
				return true;
			} else {
				file.close();
			}
		}
		return false;
	}
x{input methods}
```

```
A{global elements}
	FragMap root;
	FragMap *frags { &root };
x{global elements}
```
* Kollektion mit allen Fragmenten wird für folgende Schritte sichtbar
  angelegt


```
A{global elements}
	class Inputs {
			e{inputs attributes};
		public:
			e{inputs methods};
	};
x{global elements}
```

```
d{inputs attributes}
	std::unique_ptr<Input> _input;
	std::vector<std::unique_ptr<Input>>
		_pending;
	std::vector<std::unique_ptr<Input>>
		_used;
x{inputs attributes}
```
* Es gibt immer eine aktuelle Datei, die gerade gelesen wird
* Mitten während des Lesens können andere Dateien eingelesen
  (inkludiert) werden
* Daher gibt es einen Stapel offener Dateien
* Aus der letzten wird aktuell gelesen
* Eine Liste aller gelesenen Dateien wird in `used` verwaltet
* Damit wird verhindert, dass eine Datei mehrfach gelesen wird
* Auch signalisiert es der HTML-Ausgabe, welche Dateien generiert
  werden müssen

```
d{inputs methods}
	auto &cur() {
		return _input;
	}
x{inputs methods}
```

```
a{inputs methods}
	auto begin() const {
		return _used.cbegin();
	}
x{inputs methods}
```

```
a{inputs methods}
	auto end() const {
		return _used.cend();
	}
x{inputs methods}
```

```
a{inputs methods}
	void push(const std::string &path) {
		std::unique_ptr<Input> i {
			std::make_unique<Input>(path)
		};
		e{init additional input fields};
		e{push to pending};
		_input = std::move(i);
	}
x{inputs methods}
```
* Dateien werden über ihren Pfad identifiziert
* Dieser wird als Name gespeichert
* Durch `unique_ptr` ist keine direkte Speicherverwaltung notwendig
* Das Verschieben der Ownership muss aber explizit erfolgen

```
d{push to pending}
	if (_input) {
		_pending.push_back(
			std::move(_input)
		);
	}
x{push to pending}
```
* Falls schon eine Datei offen ist, wird sie nach `pending` verschoben

# Nächstes Zeichen
* Die Funktion `get` liest das nächste Zeichen aus der aktuellen
  Datei
* Wenn das Dateiende erreicht ist, wird die nächste Datei aus dem
  Stapel der offenen Dateien geholt
* Erst wenn die letzte Datei fertig gelesen wurde, wird ein `EOF`
  zurück geliefert

```
a{inputs methods}
	bool getLine(std::string &line) {
		while (_input) {
			if (_input->getLine(line)) {
				return true;
			}
			e{get next input file};
		}
		return false;
	}
x{inputs methods}
```

```
d{get next input file}
	_used.push_back(std::move(_input));
	if (! _pending.empty()) {
		_input = std::move(_pending.back());
		_pending.pop_back();
	}
	frags = frags->setLink(nullptr);
x{get next input file}
```
* Die aktuelle Datei wird geschlossen und in die Liste der bereits
  verarbeiteten Dateien eingereiht
* Dann wird der Vorgänger zur aktuellen Datei erklärt
* Der Vorgänger wird aus dem globalen Namensraum wieder entfernt

```
a{inputs methods}
	bool has(const std::string &name) const {
		if (_input && _input->name == name) {
			return true;
		}
		for (const auto &j : _pending) {
			if (j->name == name) {
				return true;
			}
		}
		for (const auto &j : _used) {
			if (j->name == name) {
				return true;
			}
		}
		return false;
	}
x{inputs methods}
```
* Prüft ob eine Datei bereits verwendet wurde
* Sowohl die offenen als auch die bereits prozessierten Dateien werden
  durchgegangen
* Dadurch wird bei Einbettungen verhindert, dass eine Datei mehrfach
  verarbeitet wird

# Lokale Fragmente

```
d{additional input elements}
	FragMap frags;
x{additional input elements}
```
* Jede Source-Datei hat eine eigene Fragment-Map mit lokalen
  Definitionen

```
d{init additional input fields}
	if (_input) {
		_input->frags.setLink(frags);
		frags = &_input->frags;
	}
x{init additional input fields}
```
* Wenn es bereits eine offene Input-Datei gibt, dann wird deren lokale
  Fragmente in den globalen Namensraum aufgenommen

# Zeilennummern

```
d{private input elements}
	int _line;
x{private input elements}
```
* Pro Datei wird die aktuelle Zeile festgehalten
* `_shouldAdd` signalisiert, dass das nächse Zeichen in einer neuen Zeile
  ist

```
d{private input constr}
	_line { 0 },
x{private input constr}
```

```
a{input methods}
	int line() const {
		return _line;
	}
x{input methods}
```

```
d{line read}
	++_line;
x{line read}
```

