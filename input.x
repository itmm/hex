# Eingabe-Dateien

```
@Add(includes)
	#include <iostream>
	#include <vector>
@end(includes)
```
* `vector` ist ein Container für Source-Dateien

```
@Add(input prereqs)
	FragMap root;
@End(input prereqs)
```
* Kollektion mit allen Fragmenten wird für folgende Schritte sichtbar
  angelegt

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
	auto begin() {
		return _used.begin();
	}
@End(inputs elements)
```
* Liefert erste benutzte Datei

```
@Add(inputs elements)
	auto end() {
		return _used.end();
	}
@End(inputs elements)
```
* Liefert zuletzt benutzte Datei

```
@Add(inputs elements)
	void push(const std::string &path) {
		_used.push_back({ path });
		_open.push_back({ path });
	}
@End(inputs elements)
```
* Dateien werden über ihren Pfad identifiziert
* Dieser wird als Name gespeichert
* Das Verschieben der Ownership muss aber explizit erfolgen

```
@Add(inputs elements)
	void add(const std::string &path) {
		_paths.push_back(path);
		push(path);
	}
@End(inputs elements)
```

# Nächste Zeile
* Die Funktion `@f(read_line)` liest die nächste Zeile aus der
  aktuellen  Datei
* Wenn das Dateiende erreicht ist, wird die nächste Datei aus dem
  Stapel der offenen Dateien geholt
* Erst wenn die letzte Datei fertig gelesen wurde, wird `false`  zurück
  geliefert

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
* Die Kollektionen bereits offener Dateien werden hierarchisch
  integriert

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
@Add(private open input els)
	int _line = 0;
@end(private open input els)
```
* Pro Datei wird die aktuelle Zeile festgehalten

```
@Add(open input elements)
	int line() const {
		return _line;
	}
@end(open input elements)
```
* Liefert Zeilennummer

```
@Def(line read)
	++_line;
@End(line read)
```
* Zeilennummer wird erhöht

```
@Add(inputs elements)
	Frag *find_local(const std::string &name) {
		ASSERT(! _open.empty());
		Input &i = _open.back().input();
		auto f = i.frags.find(name);
		if (f == i.frags.end()) { return nullptr; }
		return &f->second;
	}
@End(inputs elements)
```

```
@Add(inputs elements)
	Frag *add_local(const std::string &name) {
		ASSERT(! _open.empty());
		Input &i = _open.back().input();
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
				auto &fs = i->input().frags;
				auto f = fs.find(name);
				if (f != fs.end()) {
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

```
@Def(clear inputs)
	_used.clear();
	_open.clear();
	if (_paths.empty()) {
		_paths.push_back("index.x");
	}
	_current_path = _paths.begin();
@End(clear inputs)
```

