# Eingabe-Dateien

```
@Add(includes)
	#include <iostream>
	#include <vector>
@end(includes)
```
* `vector` ist ein Container für Source-Dateien


```
@Add(input elements)
	void read_line(std::string &line) {
		if (_file.is_open()) {
			@put(get line);
		}
		throw no_more_lines {};
	}
@end(input elements)
```
* Liest Zeile aus der offenen Datei

```
@def(get line)
	if (std::getline(_file, line)) {
		@put(line read);
		return;
	}
	_file.close();
@end(get line)
```
* Wenn Zeile gelesen wurde, passt die Funktion weitere Parameter an
* die erst später definiert werden

```
@Add(input prereqs)
	FragMap root;
@End(input prereqs)
```
* Kollektion mit allen Fragmenten wird für folgende Schritte sichtbar
  angelegt


```
@Def(private inputs elements)
	std::vector<Input> _open;
	std::vector<Input> _used;
@End(private inputs elements)
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
@Add(inputs elements)
	auto &cur() {
		ASSERT (! _open.empty());
		return _open.back();
	}
@End(inputs elements)
```
* Liefert zuletzt geöffnete Datei

```
@Add(inputs elements)
	auto begin() const {
		return _used.cbegin();
	}
@End(inputs elements)
```
* Liefert erste benutzte Datei

```
@Add(inputs elements)
	auto end() const {
		return _used.cend();
	}
@End(inputs elements)
```
* Liefert zuletzt benutzte Datei

```
@Add(inputs elements)
	void push(const std::string &path) {
		_open.push_back({ path });
	}
@End(inputs elements)
```
* Dateien werden über ihren Pfad identifiziert
* Dieser wird als Name gespeichert
* Das Verschieben der Ownership muss aber explizit erfolgen

# Nächste Zeile
* Die Funktion `@f(read_line)` liest die nächste Zeile aus der aktuellen
  Datei
* Wenn das Dateiende erreicht ist, wird die nächste Datei aus dem
  Stapel der offenen Dateien geholt
* Erst wenn die letzte Datei fertig gelesen wurde, wird `false`
  zurück geliefert

```
@Rep(inputs read line)
	while (! _open.empty()) {
		try {
			_open.back().read_line(line);
			return;
		} catch (const no_more_lines &) {}
		@put(get next input file);
	}
	throw no_more_lines {};
@End(inputs read line)
```
* Probiert aus aktueller Datei eine Zeile zu lesen
* Wandert bei Misserfolg durch andere offenen Dateien

```
@def(get next input file)
	_used.push_back(std::move(_open.back()));
	_open.pop_back();
@end(get next input file)
```
* Die aktuelle Datei wird geschlossen und in die Liste der bereits
  verarbeiteten Dateien eingereiht
* Dann wird der Vorgänger zur aktuellen Datei erklärt
* Der Vorgänger wird aus dem globalen Namensraum wieder entfernt

```
@Add(inputs elements)
	bool has(
		const std::string &name
	) const {
		@put(has checks);
		return false;
	}
@End(inputs elements)
```
* Prüft ob eine Datei bereits verwendet wurde
* Alle bearbeiteten Dateien werden inspiziert
* Dadurch wird bei Einbettungen verhindert, dass eine Datei mehrfach
  verarbeitet wird

```
@def(has checks)
	for (const auto &j : _open) {
		if (j.path() == name) {
			return true;
		}
	}
@end(has checks)
```
* Noch ausstehende Dateien werden geprüft

```
@add(has checks)
	for (const auto &j : _used) {
		if (j.path() == name) {
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
@Add(input elements)
	FragMap frags;
@end(input elements)
```
* Jede Source-Datei hat eine eigene Fragment-Map mit lokalen
  Definitionen

# Zeilennummern
* Jede Datei führt die aktuelle Zeiennummer mit

```
@Def(private input elements)
	int _line;
@end(private input elements)
```
* Pro Datei wird die aktuelle Zeile festgehalten

```
@Def(private input constructor),
	_line { 0 }
@end(private input constructor)
```
* Wenn keine Zeile prozessiert wurde, steht die Zeilennummer noch auf
  `0`

```
@Add(input elements)
	int line() const {
		return _line;
	}
@end(input elements)
```
* Liefert Zeilennummer

```
@def(line read)
	++_line;
@end(line read)
```
* Zeilennummer wird erhöht

```
@Add(inputs elements)
	Frag *find_local(const std::string &name) {
		if (_open.empty()) { return nullptr; }
		Input &i = _open.back();
		auto f = i.frags.find(name);
		if (f == i.frags.end()) { return nullptr; }
		return &f->second;
	}
@End(inputs elements)
```

```
@Add(inputs elements)
	Frag *add_local(const std::string &name) {
		if (_open.empty()) { return nullptr; }
		Input &i = _open.back();
		return &i.frags.insert({ name, name }).first->second;
	}
@End(inputs elements)
```

```
@Add(inputs elements)
	Frag *get_local(const std::string &name) {
		Frag *result = find_local(name);
		if (! result) {
			result = add_local(name);
		}
		return result;
	}
@End(inputs elements)
```

```
@Add(inputs elements)
	Frag *find_global(const std::string &name) {
		if (_open.size() > 1) {
			auto i = _open.end() - 2;
			for (;; --i) {
				auto f = i->frags.find(name);
				if (f != i->frags.end()) {
					return &f->second;
				}
				if (i == _open.begin()) { break; }
			}
		}
		auto f = root.find(name);
		if (f == root.end()) { return nullptr; }
		return &f->second;

	}
@End(inputs elements)
```

```
@Add(inputs elements)
	Frag *add_global(const std::string &name) {
		return &root.insert({ name, name }).first->second;
	}
@End(inputs elements)
```

```
@Add(inputs elements)
	Frag *get_global(const std::string &name) {
		Frag *result = find_global(name);
		if (! result) {
			result = add_global(name);
		}
		return result;
	}
@End(inputs elements)
```
