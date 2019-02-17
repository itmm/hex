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
			e{private elements};
		public:
			const std::string name;
			e{additional elements};
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
			e{get line};
		}
		return false;
	}
x{input methods}
```
* Liest Zeile aus der offenen Datei

```
d{get line}
	if (std::getline(file, line)) {
		e{line read};
		return true;
	} else {
		file.close();
	}
x{get line}
```
* Wenn Zeile gelesen wurde, passt die Funktion weitere Parameter an
* die erst später definiert werden

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
* Enthält alle verarbeiteten Dateien

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
* Liefert zuletzt geöffnete Datei

```
a{inputs methods}
	auto begin() const {
		return _used.cbegin();
	}
x{inputs methods}
```
* Liefert erste benutzte Datei

```
a{inputs methods}
	auto end() const {
		return _used.cend();
	}
x{inputs methods}
```
* Liefert zuletzt benutzte Datei

```
a{inputs methods}
	void push(const std::string &path) {
		std::unique_ptr<Input> i {
			std::make_unique<Input>(path)
		};
		e{init additional fields};
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

# Nächste Zeile
* Die Funktion `f{getLine}` liest die nächste Zeile aus der aktuellen
  Datei
* Wenn das Dateiende erreicht ist, wird die nächste Datei aus dem
  Stapel der offenen Dateien geholt
* Erst wenn die letzte Datei fertig gelesen wurde, wird `false`
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
* Probiert aus aktueller Datei eine Zeile zu lesen
* Wandert bei Misserfolg durch andere offenen Dateien

```
d{get next input file}
	_used.push_back(std::move(_input));
	if (! _pending.empty()) {
		_input =
			std::move(_pending.back());
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
	bool has(
		const std::string &name
	) const {
		e{has checks};
		return false;
	}
x{inputs methods}
```
* Prüft ob eine Datei bereits verwendet wurde
* Alle bearbeiteten Dateien werden inspiziert
* Dadurch wird bei Einbettungen verhindert, dass eine Datei mehrfach
  verarbeitet wird

```
d{has checks}
	if (_input && _input->name == name) {
		return true;
	}
x{has checks}
```
* Die aktuelle Datei wird geprüft

```
a{has checks}
	for (const auto &j : _pending) {
		if (j->name == name) {
			return true;
		}
	}
x{has checks}
```
* Noch ausstehende Dateien werden geprüft

```
a{has checks}
	for (const auto &j : _used) {
		if (j->name == name) {
			return true;
		}
	}
x{has checks}
```
* Bereits verarbeitete Dateien werden geprüft

# Lokale Fragmente
* Jede Datei hat eine eigene Fragment-Kollektion
* Die Kollektionen bereits offener Dateien werden hierarchisch integriert

```
d{additional elements}
	FragMap frags;
x{additional elements}
```
* Jede Source-Datei hat eine eigene Fragment-Map mit lokalen
  Definitionen

```
d{init additional fields}
	if (_input) {
		_input->frags.setLink(frags);
		frags = &_input->frags;
	}
x{init additional fields}
```
* Wenn es bereits eine offene Input-Datei gibt, dann wird deren lokale
  Fragmente in den globalen Namensraum aufgenommen

# Zeilennummern
* Jede Datei führt die aktuelle Zeiennummer mit

```
d{private elements}
	int _line;
x{private elements}
```
* Pro Datei wird die aktuelle Zeile festgehalten

```
d{private input constr}
	_line { 0 },
x{private input constr}
```
* Wenn keine Zeile prozessiert wurde, steht die Zeilennummer noch auf
  `0`

```
a{input methods}
	int line() const {
		return _line;
	}
x{input methods}
```
* Liefert Zeilennummer

```
d{line read}
	++_line;
x{line read}
```
* Zeilennummer wird erhöht

