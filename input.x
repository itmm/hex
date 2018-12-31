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
		public:
			const std::string name;
			e{additional input elements};
			e{input methods};
	};
x{global elements}
```
* Die `Input`-Klasse enthält den Dateinamen der Eingabe-Dateien
* Zusätzlich kann sie eine offene Datei enthalten
* Alle Eingabe-Dateien, die während einer Inklude-Kaskade eingebunden
  werden bleiben offen, damit an der richtigen Stelle weiter gearbeitet
  werden kann
* Weiter Attribute und Methoden können später definiert werden

```
d{input methods}
	Input(
		const std::string &name
	):
		file { name.c_str() },
		name { name }
		e{direct input constr}
	{
	}
x{input methods}
```
* Im Konstruktor werden Datei und Name gesetzt
* Später eingeführte Attribute können auch dann initialisiert werden

```
a{input methods}
	int next() {
		int ch { EOF };
		if (file.is_open()) {
			ch = file.get();
			if (! file.good()) {
				file.close();
			}
		}
		return ch;
	}
x{input methods}
```
* `next` liefert das nächste Zeichen aus der Datei
* Wenn das Ende erreicht ist, wird `EOF` zurück gegeben
* Und die Datei geschlossen

```
A{global elements}
	std::unique_ptr<Input> input;
	std::vector<std::unique_ptr<Input>>
		pending;
	std::vector<std::unique_ptr<Input>>
		used;
x{global elements}
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
A{global elements}
	FragMap root;
	FragMap *frags { &root };
x{global elements}
```
* Kollektion mit allen Fragmenten wird für folgende Schritte sichtbar
  angelegt

```
A{global elements}
	void pushPath(const std::string &path) {
		std::unique_ptr<Input> i {
			std::make_unique<Input>(path)
		};
		e{init additional input fields};
		e{push to pending};
		input = std::move(i);
	}
x{global elements}
```
* Dateien werden über ihren Pfad identifiziert
* Dieser wird als Name gespeichert
* Durch `unique_ptr` ist keine direkte Speicherverwaltung notwendig
* Das Verschieben der Ownership muss aber explizit erfolgen

```
d{push to pending}
	if (input) {
		pending.push_back(
			std::move(input)
		);
	}
x{push to pending}
```
* Falls schon eine Datei offen ist, wird sie nach `pending` verschoben

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
	if (input) {
		input->frags.setLink(frags);
		frags = &input->frags;
	}
x{init additional input fields}
```
* Wenn es bereits eine offene Input-Datei gibt, dann wird deren lokale
  Fragmente in den globalen Namensraum aufgenommen

# Nächstes Zeichen
* Die Funktion `nextCh` liest das nächste Zeichen aus der aktuellen
  Datei
* Wenn das Dateiende erreicht ist, wird die nächste Datei aus dem
  Stapel der offenen Dateien geholt
* Erst wenn die letzte Datei fertig gelesen wurde, wird ein `EOF`
  zurück geliefert

```
A{global elements}
	int nextCh() {
		int ch { EOF };
		while (input) {
			ch = input->next();
			e{preprocess char};
			if (ch != EOF) { break; }
			e{get next input file};
		}
		return ch;
	}
x{global elements}
```
* Wenn kein `EOF` gelesen wurde, dann wird das Zeichen zurück
  geliefert
* Ansonsten wird aus der nächsten Datei ein Zeichen gelesen

```
d{get next input file}
	used.push_back(std::move(input));
	if (! pending.empty()) {
		input = std::move(pending.back());
		pending.pop_back();
	}
	frags = frags->setLink(nullptr);
x{get next input file}
```
* Die aktuelle Datei wird geschlossen und in die Liste der bereits
  verarbeiteten Dateien eingereiht
* Dann wird der Vorgänger zur aktuellen Datei erklärt
* Der Vorgänger wird aus dem globalen Namensraum wieder entfernt

```
A{global elements}
	bool alreadyUsed(const std::string &name) {
		if (input && input->name == name) {
			return true;
		}
		for (auto &j : pending) {
			if (j->name == name) {
				return true;
			}
		}
		for (auto &j : used) {
			if (j->name == name) {
				return true;
			}
		}
		return false;
	}
x{global elements}
```
* Prüft ob eine Datei bereits verwendet wurde
* Sowohl die offenen als auch die bereits prozessierten Dateien werden
  durchgegangen
* Dadurch wird bei Einbettungen verhindert, dass eine Datei mehrfach
  verarbeitet wird

```
a{additional input elements}
	int line;
x{additional input elements}
```
* Pro Datei wird die aktuelle Zeile festgehalten

```
d{direct input constr}
	, line(1)
x{direct input constr}
```
* Zeile wird mit `1` initialisiert

```
d{preprocess char}
	if (ch == '\n') {
		++input->line;
	}
x{preprocess char}
```
* Wenn ein Zeilenumbruch gelesen wird, wird die aktuelle Zeile
  erhöht

