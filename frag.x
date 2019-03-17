# Fragmente
* Fragmente bilden einen gerichteten azyklischen Graph
* Die Infix-Traversierung dieses Graphen bildet die generierten
  Source-Code Dateien

```
@Add(input prereqs)
	@Put(frag prereqs);
	@put(define frag);
@end(input prereqs)
```
* Fragmente sind global sichtbare Strukturen

```
@def(define frag)
	class Frag;

	class FragEntry {
		std::string _str;
		std::string _file;
		int _first_line;
		int _last_line;
	public:
		const Frag *frag;
		@put(entry methods);
	};
@end(define frag)
```
* Ein Eintrag kann entweder auf ein anderes Fragment verweisen (wenn
  dieses an der aktuellen Stelle expandiert werden soll)
* Oder er enthält Bytes, die beim Expandieren direkt expandiert werden
* Wenn ein Eintrag sowohl Daten als auch ein Fragment enthält, so wird
  zuerst das Fragment ausgegeben
* So können die Bytes leicht erweitert werden

```
@def(entry methods)
	FragEntry(
		Frag *frag = nullptr
	):
		frag { frag }
	{}
@end(entry methods)
```
* Ein Eintrag kann direkt mit einem Fragment initialisiert werden
* Dies kann auch leer sein
* Die Zeichenkette ist immer leer

```
@add(entry methods)
	const std::string &str() const {
		return _str;
	}
@end(entry methods)
```
* Auf die Bytes kann mit `str()` nur lesend zugegriffen werden

```
@add(entry methods)
	void add(
		char ch, const std::string &file,
		int line
	) {
		@mul(copy file and line);
		_str += ch;
	}
@end(entry methods)
```
* Beim Anfügen eines Zeichens werden Dateiname und erste Zeile
  aktualisiert
* Wenn sie noch nicht gesetzt sind

```
@def(copy file and line)
	if (
		_file.empty() || _first_line <= 0
	) {
		_file = file;
		_first_line = line;
	}
	_last_line = line;
@end(copy file and line)
```
* Wenn Dateiname oder erste Zeile nicht gesetzt sind, dann setzt die
  Methode beide
* Die letzte Zeile wird immer gesetzt

```
@add(entry methods)
	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		@mul(copy file and line);
		_str += value;
	}
@end(entry methods)
```
* Bytes können direkt hinzugefügt werden

```
@add(entry methods)
	bool canAdd(
		const std::string &file,
		int line
	) {
		@put(can add);
		return false;
	}
@end(entry methods)
```
* Prüft ob Zeichen aus einer bestimmten Datei und Zeile an den Eintrag
  angefügt werden können

```
@def(can add)
	if (
		! _file.empty() && file != _file
	) {
		return false;
	}
@end(can add)
```
* Wenn sich die Dateinamen unterscheiden, können die Zeichen nicht
  angefügt werden

```
@add(can add)
	if (
		_last_line > 0 &&
		_last_line != line &&
		_last_line + 1 != line
	) {
		return false;
	}
@end(can add)
```
* Wenn die letzte Zeile nicht zur neuen Zeile passt, können die Zeichen
  nicht angefügt werden

```
@add(can add)
	return true;
@end(can add)
```
* Ansonsten können die Zeichen angefügt werden

```
@Add(includes)
	#include <vector>
@end(includes)
```
* Das Fragment legt die Einträge in einem Vektor ab

```
@add(define frag)
	class Frag {
		std::vector<FragEntry> _entries;
		int _expands;
		int _multiples;
	public:
		const std::string name;
		@put(frag methods);
	};
@end(define frag)
```
* Die Einträge eines Fragments werden in einem Vektor gesammelt
* Das Fragment zählt wie häufig es mit `@put`, `@globexpand` und
  `@multiple`, `@globmult` aufgerufen wurde, um Struktur-Fehler zu
  erkennen

# Neues Fragment anlegen

```
@def(frag methods)
	bool isFile() const {
		static const std::string prefix {
			"file: "
		};
		std::string p {
			name.substr(0, prefix.size())
		};
		return p == prefix;
	}
@end(frag methods)
```
* Beschreibt das Fragment eine Datei?
* Nur wenn sein Name mit der Zeichenkette `@s(file: )` beginnt

```
@add(frag methods)
	std::string cmd() const {
		static const std::string prefix {
			"| "
		};
		std::string p {
			name.substr(0, prefix.size())
		};
		return p == prefix ?
			name.substr(prefix.size()) :
			std::string {};
	}
@end(frag methods)
```

```
@add(frag methods)
	Frag(
		const std::string &name
	):
		_entries {},
		_expands { 0 },
		_multiples { 0 },
		name { name }
	{
		if (isFile()) { ++_expands; }
		if (cmd().size()) { ++_expands; }
	}
@end(frag methods)
```
* Dateien gelten als einmal expandiert

```
@add(frag methods)
	void clear() {
		_entries.clear();
	}
@end(frag methods)
```
* Löscht alle Einträge
* wird von `@replace`, `@globrep` verwendet

```
@add(frag methods)
	bool empty() const {
		return _entries.empty();
	}
@end(frag methods)
```
* Ein Fragment ist leer, wenn es keine Einträge enthält

# Unit Tests

```
@Def(perform unit-tests)
	@put(unit tests);
@end(perform unit-tests)
```
* Fragmente haben einen eigenes Unit-Test Fragment

```
@add(define frag)
	void testFragName(
		const std::string &name
	) {
		Frag f(name);
		ASSERT(f.name == name);
	}
@end(define frag)
```
* `testFragName` prüft, ob der Name korrekt in ein Fragment kopiert
  wurde

```
@def(unit tests)
	testFragName("abc");
	testFragName("");
	testFragName("A c");
	{
		Frag f { "ab" };
		ASSERT(f.empty());
	}
@end(unit tests)
```
* Zum einen wird getestet, ob die Namen korrekt kopiert werden
* Zum anderen wird sichergestellt, dass die Verweise `nullptr` sind

```
@add(define frag)
	bool isPopulatedFrag(
		const Frag *f
	) {
		return f && ! f->empty();
	}
@end(define frag)
```
* Prüft, ob ein Fragment nicht leer ist

# Unit Tests
* Testet Fragmente

```
@add(unit tests)
	{
		FragEntry entry;
		ASSERT(! entry.frag);
	}
@end(unit tests)
```
* Ein leerer Eintrag hat keinen Nachfolger
* Und kein Fragment

```
@add(unit tests)
	{
		FragEntry entry;
		ASSERT(entry.str().empty());
	}
@end(unit tests)
```
* Ein leerer Eintrag hat keine Bytes

# Einträge zu Fragmenten hinzufügen
* fügt Text oder Fragmente an ein Fragment an

```
@add(frag methods)
	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		if (value.empty()) { return; }
		@mul(assure frag entry);
		_entries.back().add(
			value, file, line
		);
	}
@end(frag methods)
```
* Wenn der Text leer ist, fügt die Methode nichts an
* Ansonsten stellt die Methode sicher, dass der letzte Eintrag valide
  ist
* An diesen wird der Text angefügt

```
@def(assure frag entry)
	if (_entries.empty()) {
		_entries.push_back(FragEntry {});
	} else if (
		! _entries.back().canAdd(
			file, line
		)
	) {
		_entries.push_back(FragEntry {});
	}
@end(assure frag entry)
```
* Wenn es noch keine Einträge gibt, legt die Methode einen an
* Wenn der letzte Eintrag unpassende Dateinamen oder Zeilennummern hat,
  dann legt die Methode einen neuen Eintrag an

```
@add(frag methods)
	void add(
		char ch,
		const std::string &file,
		int line
	) {
		@mul(assure frag entry);
		_entries.back().add(
			ch, file, line
		);
	}
@end(frag methods)
```
* Die Methode stellt sicher, dass der letzte Eintrag valide ist
* An diesen wird das Zeichen angefügt

```
@add(frag methods)
	Frag &add(Frag *child);
@end(frag methods)
```
* Die `@f(add)`-Methode muss sicherstellen, dass keine Zykel entstehen
* Da es die notwendigen Methoden noch nicht gibt, wird die Methode
  außerhalb der Klasse definiert

```
@add(define frag)
	@put(define cycle check)
	Frag &Frag::add(Frag *child) {
		ASSERT(child);
		@put(avoid frag cycles);
		@put(add frag entry);
		return *this;
	}
@end(define frag)
```
* Bevor ein Fragment hinzugefügt werden kann, muss sichergestellt
  werden,  dass kein Zykel entsteht
* Ein Zykel liegt vor, wenn `frag` gleich `child` ist
* Oder bereits direkt oder indirekt zu `child` hinzugefügt wurde
* Falls der letzte Eintrag noch kein Fragment hat, wird dieser Eintrag
  verwendet

```
@def(add frag entry)
	_entries.push_back(
		FragEntry { child }
	);
@end(add frag entry)
```
* Für das Kind wird ein neuer Eintrag an das Fragment angefügt

```
@add(frag methods)
	auto begin() const {
		return _entries.cbegin();
	}
@end(frag methods)
```
* Beginn eines konstanten Iterators auf den Einträgen

```
@add(frag methods)
	auto end() const {
		return _entries.cend();
	}
@end(frag methods)
```
* Ende eines konstanten Iterators auf den Einträgen

```
@add(frag methods)
	int expands() const {
		return _expands;
	}
@end(frag methods)
```
* Anzahl der `@put` und `@globexpand` Aufrufe

```
@add(frag methods)
	void addExpand() {
		++_expands;
	}
@end(frag methods)
```
* Fügt `@put` oder `@globexpand` hinzu

```
@add(frag methods)
	int multiples() const {
		return _multiples;
	}
@end(frag methods)
```
* Anzahl der `@multiple` und `@globmult` Aufrufe

```
@add(frag methods)
	void addMultiple() {
		++_multiples;
	}
@end(frag methods)
```
* Fügt `@multiple` oder `@globmult` hinzu

# Fragmente serialisieren
* Serialisiert Fragmente in einen `std::ostream`

```
@add(define frag)
	void serializeFrag(
		const Frag &frag,
		std::ostream &out,
		bool writeLineMacros
	) {
		@put(iterate entries);
	}
@end(define frag)
```
* Jeder Eintrag wird nacheinander bearbeitet
* Fragmente in Einträgen werden rekursiv ausgegeben

```
@def(iterate entries)
	for (const auto &entry : frag) {
		if (entry.frag) {
			serializeFrag(
				*entry.frag, out,
				writeLineMacros
			);
		}
		out << entry.str();
	}
@end(iterate entries)
```
* Rekursiv wird das Fragment ausgegeben, falls vorhanden
* Dann werden die Bytes des Eintrags ausgegeben

```
@add(define frag)
	void testFrag(
		const Frag &frag,
		const std::string &expected
	) {
		@put(serialize test frag);
	}
@end(define frag)
```
* Diese Hilfsfunktion prüft ob die Serialisierung eines Fragments der
  Erwartung entspricht

```
@Add(includes)
	#include <sstream>
@end(includes)
```
* `@f(testFrag)` benötigt `std::ostringstream`

```
@def(serialize test frag)
	std::ostringstream buffer;
	serializeFrag(frag, buffer, false);
	ASSERT(buffer.str() == expected);
@end(serialize test frag)
```
* Serialisiert das Fragment
* Der Buffer muss die erwarteten Werte enthalten

```
@add(define frag)
	void addStringToFrag(
		Frag *frag,
		const std::string &str
	) {
		frag->add(
			str, std::string {}, 0
		);
	}
@end(define frag)
```
* Zu Testzwecken kann eine Null-terminierte Zeichenkette hinzugefügt
  werden

```
@add(unit tests) {
	Frag frag { "" };
	addStringToFrag(&frag, "abc");
	addStringToFrag(&frag, "def");
	testFrag(frag, "abcdef");
} @end(unit tests)
```
* Prüft, ob zwei Strings richtig serialisiert werden

```
@add(unit tests) {
	Frag a { "" };
	Frag b { "" };
	addStringToFrag(&a, "abc");
	b.add(&a);
	addStringToFrag(&b, "def");
	b.add(&a);
	testFrag(b, "abcdefabc");
} @end(unit tests)
```
* Prüft, ob Fragmente expandiert werden

# Zykel im Fragment-Graph finden
* Prüft ob das Hinzufügen eines Fragments zu einem Zykel führt

```
@def(define cycle check)
	bool isFragInFrag(
		const Frag *needle,
		const Frag *haystack
	) {
		ASSERT(needle);
		ASSERT(haystack);
		@put(check cycle frag);
		@put(check cycle entries);
		return false;
	}
@end(define cycle check)
```
* Wenn das Fragment das gesuchte ist, dann wurde ein Zykel gefunden
* Danach wird über alle Einträge gesucht
* Wenn das Fragment dort nicht gefunden wurde, dann ist es nicht
  enthalten

```
@def(avoid frag cycles)
	ASSERT(! isFragInFrag(
		this, child
	));
@end(avoid frag cycles)
```
* Ein Fragment darf nur hinzugefügt werden, wenn es den Container nicht
  bereits enthält

```
@def(check cycle frag)
	if (needle == haystack) {
		return true;
	}
@end(check cycle frag)
```
* Wenn der Container selbst das gesuchte Fragment ist, liefert die
  Funktion `true` zurück

```
@def(check cycle entries)
	for (const auto &i : *haystack)  {
		if (! i.frag) { continue; }
		if (isFragInFrag(
			needle, i.frag
		)) {
			return true;
		}
	}
@end(check cycle entries)
```
* Alle Fragment in den Einträgen werden rekursiv untersucht
* Damit wird der ganze Graph durchsucht

# Fragment-Kollektion
* Kollektion von Fragmenten, die in mehreren Hierarchien organisiert
  werden

```
@Add(includes)
	#include <map>
@end(includes)
```
* `FragMap` enthält eine `std::map`

```
@add(define frag)
	using FragMap =
		std::map<std::string, Frag>;
@end(define frag)
```
* Eine Kollektion von Fragmenten ist ein Array von Fragment-Ketten
* Alle Felder müssen mit `nullptr` initialisiert werden
