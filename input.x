# Eingabe-Dateien

```
@Add(includes)
	#include <fstream>
	#include <iostream>
	#include <memory>
	#include <vector>
@end(includes)
```
* Aus `memory` wird `unique_ptr` verwendet
* `vector` ist ein Container für Source-Dateien

```
@Add(global elements)
	class Input {
		private:
			std::ifstream file;
			@put(private elements);
		public:
			const std::string name;
			@put(additional elements);
			@put(input methods);
	};
@end(global elements)
```
* Die `Input`-Klasse enthält den Dateinamen der Eingabe-Dateien
* Zusätzlich kann sie eine offene Datei enthalten
* Alle Eingabe-Dateien, die während einer Inkludierungs-Kaskade
  eingebunden werden bleiben offen, damit an der richtigen Stelle weiter
  gearbeitet werden kann
* Weiter Attribute und Methoden können später definiert werden

```
@def(input methods)
	Input(
		const std::string &name
	):
		file { name.c_str() },
		@put(private input constr)
		name { name }
	{
	}
@end(input methods)
```
* Im Konstruktor werden Datei und Name gesetzt
* Später eingeführte Attribute können auch dann initialisiert werden

```
@add(input methods)
	bool getLine(std::string &line) {
		if (file.is_open()) {
			@put(get line);
		}
		return false;
	}
@end(input methods)
```
* Liest Zeile aus der offenen Datei

```
@def(get line)
	if (std::getline(file, line)) {
		@put(line read);
		return true;
	} else {
		file.close();
	}
@end(get line)
```
* Wenn Zeile gelesen wurde, passt die Funktion weitere Parameter an
* die erst später definiert werden

```
@Add(global elements)
	FragMap root;
	FragMap *frags { &root };
@end(global elements)
```
* Kollektion mit allen Fragmenten wird für folgende Schritte sichtbar
  angelegt


```
@Add(global elements)
	class Inputs {
			@put(inputs attributes);
		public:
			@put(inputs methods);
	};
@end(global elements)
```
* Enthält alle verarbeiteten Dateien

```
@def(inputs attributes)
	std::unique_ptr<Input> _input;
	std::vector<std::unique_ptr<Input>>
		_pending;
	std::vector<std::unique_ptr<Input>>
		_used;
@end(inputs attributes)
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
@def(inputs methods)
	auto &cur() {
		return _input;
	}
@end(inputs methods)
```
* Liefert zuletzt geöffnete Datei

```
@add(inputs methods)
	auto begin() const {
		return _used.cbegin();
	}
@end(inputs methods)
```
* Liefert erste benutzte Datei

```
@add(inputs methods)
	auto end() const {
		return _used.cend();
	}
@end(inputs methods)
```
* Liefert zuletzt benutzte Datei

```
@add(inputs methods)
	void push(const std::string &path) {
		std::unique_ptr<Input> i {
			std::make_unique<Input>(path)
		};
		@put(init additional fields);
		@put(push to pending);
		_input = std::move(i);
	}
@end(inputs methods)
```
* Dateien werden über ihren Pfad identifiziert
* Dieser wird als Name gespeichert
* Durch `unique_ptr` ist keine direkte Speicherverwaltung notwendig
* Das Verschieben der Ownership muss aber explizit erfolgen

```
@def(push to pending)
	if (_input) {
		_pending.push_back(
			std::move(_input)
		);
	}
@end(push to pending)
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
@add(inputs methods)
	bool getLine(std::string &line) {
		while (_input) {
			if (_input->getLine(line)) {
				return true;
			}
			@put(get next input file);
		}
		return false;
	}
@end(inputs methods)
```
* Probiert aus aktueller Datei eine Zeile zu lesen
* Wandert bei Misserfolg durch andere offenen Dateien

```
@def(get next input file)
	_used.push_back(std::move(_input));
	if (! _pending.empty()) {
		_input =
			std::move(_pending.back());
		_pending.pop_back();
	}
	frags = frags->setLink(nullptr);
@end(get next input file)
```
* Die aktuelle Datei wird geschlossen und in die Liste der bereits
  verarbeiteten Dateien eingereiht
* Dann wird der Vorgänger zur aktuellen Datei erklärt
* Der Vorgänger wird aus dem globalen Namensraum wieder entfernt

```
@add(inputs methods)
	bool has(
		const std::string &name
	) const {
		@put(has checks);
		return false;
	}
@end(inputs methods)
```
* Prüft ob eine Datei bereits verwendet wurde
* Alle bearbeiteten Dateien werden inspiziert
* Dadurch wird bei Einbettungen verhindert, dass eine Datei mehrfach
  verarbeitet wird

```
@def(has checks)
	if (_input && _input->name == name) {
		return true;
	}
@end(has checks)
```
* Die aktuelle Datei wird geprüft

```
@add(has checks)
	for (const auto &j : _pending) {
		if (j->name == name) {
			return true;
		}
	}
@end(has checks)
```
* Noch ausstehende Dateien werden geprüft

```
@add(has checks)
	for (const auto &j : _used) {
		if (j->name == name) {
			return true;
		}
	}
@end(has checks)
```
* Bereits verarbeitete Dateien werden geprüft

# Lokale Fragmente
* Jede Datei hat eine eigene Fragment-Kollektion
* Die Kollektionen bereits offener Dateien werden hierarchisch integriert

```
@def(additional elements)
	FragMap frags;
@end(additional elements)
```
* Jede Source-Datei hat eine eigene Fragment-Map mit lokalen
  Definitionen

```
@def(init additional fields)
	if (_input) {
		_input->frags.setLink(frags);
		frags = &_input->frags;
	}
@end(init additional fields)
```
* Wenn es bereits eine offene Input-Datei gibt, dann wird deren lokale
  Fragmente in den globalen Namensraum aufgenommen

# Zeilennummern
* Jede Datei führt die aktuelle Zeiennummer mit

```
@def(private elements)
	int _line;
@end(private elements)
```
* Pro Datei wird die aktuelle Zeile festgehalten

```
@def(private input constr)
	_line { 0 },
@end(private input constr)
```
* Wenn keine Zeile prozessiert wurde, steht die Zeilennummer noch auf
  `0`

```
@add(input methods)
	int line() const {
		return _line;
	}
@end(input methods)
```
* Liefert Zeilennummer

```
@def(line read)
	++_line;
@end(line read)
```
* Zeilennummer wird erhöht

