# Fragmente
* Fragmente bilden einen gerichteten azyklischen Graph
* Die Infix-Traversierung dieses Graphen bildet die generierten
  Source-Code Dateien

```
A{global elements}
	e{define frag};
x{global elements}
```
* Fragmente sind global sichtbare Strukturen

```
d{define frag}
	class Frag;

	class FragEntry {
		std::string _str;
		std::string _file;
		int _first_line;
		int _last_line;
	public:
		const Frag *frag;
		e{entry methods};
	};
x{define frag}
```
* Ein Eintrag kann entweder auf ein anderes Fragment verweisen (wenn
  dieses an der aktuellen Stelle expandiert werden soll)
* Oder er enthält Bytes, die beim Expandieren direkt expandiert werden
* Wenn ein Eintrag sowohl Daten als auch ein Fragment enthält, so wird
  zuerst das Fragment ausgegeben
* So können die Bytes leicht erweitert werden

```
d{entry methods}
	FragEntry(
		Frag *frag = nullptr
	):
		frag { frag }
	{}
x{entry methods}
```
* Ein Eintrag kann direkt mit einem Fragment initialisiert werden
* Dies kann auch leer sein
* Die Zeichenkette ist immer leer

```
a{entry methods}
	const std::string &str() const {
		return _str;
	}
x{entry methods}
```
* Auf die Bytes kann mit `str()` nur lesend zugegriffen werden

```
a{entry methods}
	void add(
		char ch, const std::string &file,
		int line
	) {
		E{copy file and line};
		_str += ch;
	}
x{entry methods}
```
* Beim Anfügen eines Zeichens werden Dateiname und erste Zeile
  aktualisiert
* Wenn sie noch nicht gesetzt sind

```
d{copy file and line}
	if (
		_file.empty() || _first_line <= 0
	) {
		_file = file;
		_first_line = line;
	}
	_last_line = line;
x{copy file and line}
```
* Wenn Dateiname oder erste Zeile nicht gesetzt sind, dann setzt die
  Methode beide
* Die letzte Zeile wird immer gesetzt

```
a{entry methods}
	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		E{copy file and line};
		_str += value;
	}
x{entry methods}
```
* Bytes können direkt hinzugefügt werden

```
a{entry methods}
	bool canAdd(
		const std::string &file,
		int line
	) {
		e{can add};
		return false;
	}
x{entry methods}
```
* Prüft ob Zeichen aus einer bestimmten Datei und Zeile an den Eintrag
  angefügt werden können

```
d{can add}
	if (
		! _file.empty() && file != _file
	) {
		return false;
	}
x{can add}
```
* Wenn sich die Dateinamen unterscheiden, können die Zeichen nicht
  angefügt werden

```
a{can add}
	if (
		_last_line > 0 &&
		_last_line != line &&
		_last_line + 1 != line
	) {
		return false;
	}
x{can add}
```
* Wenn die letzte Zeile nicht zur neuen Zeile passt, können die Zeichen
  nicht angefügt werden

```
a{can add}
	return true;
x{can add}
```
* Ansonsten können die Zeichen angefügt werden

```
A{includes}
	#include <vector>
x{includes}
```
* Das Fragment legt die Einträge in einem Vektor ab

```
a{define frag}
	class Frag {
		std::vector<FragEntry> _entries;
		int _expands;
		int _multiples;
	public:
		const std::string name;
		e{frag methods};
	};
x{define frag}
```
* Die Einträge eines Fragments werden in einem Vektor gesammelt
* Das Fragment zählt wie häufig es mit `@expand`, `@globexpand` und
  `@multiple`, `@globmult` aufgerufen wurde, um Struktur-Fehler zu
  erkennen

# Neues Fragment anlegen

```
d{frag methods}
	bool isFile() const {
		static const std::string prefix {
			"file: "
		};
		std::string p {
			name.substr(0, prefix.size())
		};
		return p == prefix;
	}
x{frag methods}
```
* Beschreibt das Fragment eine Datei?
* Nur wenn sein Name mit der Zeichenkette `s{file: }` beginnt

```
a{frag methods}
	Frag(
		const std::string &name
	):
		_entries {},
		_expands { 0 },
		_multiples { 0 },
		name { name }
	{
		if (isFile()) { ++_expands; }
	}
x{frag methods}
```
* Dateien gelten als einmal expandiert

```
a{frag methods}
	void clear() {
		_entries.clear();
	}
x{frag methods}
```
* Löscht alle Einträge
* wird von `@replace`, `@globrep` verwendet

```
a{frag methods}
	bool empty() const {
		return _entries.empty();
	}
x{frag methods}
```
* Ein Fragment ist leer, wenn es keine Einträge enthält

# Unit Tests

```
D{perform unit-tests}
	e{unit tests};
x{perform unit-tests}
```
* Fragmente haben einen eigenes Unit-Test Fragment

```
a{define frag}
	void testFragName(
		const std::string &name
	) {
		Frag f(name);
		ASSERT(f.name == name);
	}
x{define frag}
```
* `testFragName` prüft, ob der Name korrekt in ein Fragment kopiert
  wurde

```
d{unit tests}
	testFragName("abc");
	testFragName("");
	testFragName("A c");
	{
		Frag f { "ab" };
		ASSERT(f.empty());
	}
x{unit tests}
```
* Zum einen wird getestet, ob die Namen korrekt kopiert werden
* Zum anderen wird sichergestellt, dass die Verweise `nullptr` sind

```
a{define frag}
	bool isPopulatedFrag(
		const Frag *f
	) {
		return f && ! f->empty();
	}
x{define frag}
```
* Prüft, ob ein Fragment nicht leer ist

# Unit Tests
* Testet Fragmente

```
a{unit tests}
	{
		FragEntry entry;
		ASSERT(! entry.frag);
	}
x{unit tests}
```
* Ein leerer Eintrag hat keinen Nachfolger
* Und kein Fragment

```
a{unit tests}
	{
		FragEntry entry;
		ASSERT(entry.str().empty());
	}
x{unit tests}
```
* Ein leerer Eintrag hat keine Bytes

# Einträge zu Fragmenten hinzufügen
* fügt Text oder Fragmente an ein Fragment an

```
a{frag methods}
	void add(
		const std::string &value,
		const std::string &file,
		int line
	) {
		if (value.empty()) { return; }
		E{assure frag entry};
		_entries.back().add(
			value, file, line
		);
	}
x{frag methods}
```
* Wenn der Text leer ist, fügt die Methode nichts an
* Ansonsten stellt die Methode sicher, dass der letzte Eintrag valide ist
* An diesen wird der Text angefügt

```
d{assure frag entry}
	if (_entries.empty()) {
		_entries.push_back(FragEntry {});
	} else if (
		! _entries.back().canAdd(
			file, line
		)
	) {
		_entries.push_back(FragEntry {});
	}
x{assure frag entry}
```
* Wenn es noch keine Einträge gibt, legt die Methode einen an
* Wenn der letzte Eintrag unpassende Dateinamen oder Zeilennummern hat,
  dann legt die Methode einen neuen Eintrag an

```
a{frag methods}
	void add(
		char ch,
		const std::string &file,
		int line
	) {
		E{assure frag entry};
		_entries.back().add(
			ch, file, line
		);
	}
x{frag methods}
```
* Die Methode stellt sicher, dass der letzte Eintrag valide ist
* An diesen wird das Zeichen angefügt

```
a{frag methods}
	Frag &add(Frag *child);
x{frag methods}
```
* Die `f{add}`-Methode muss sicherstellen, dass keine Zykel entstehen
* Da es die notwendigen Methoden noch nicht gibt, wird die Methode
  außerhalb der Klasse definiert

```
a{define frag}
	e{define cycle check}
	Frag &Frag::add(Frag *child) {
		ASSERT(child);
		e{avoid frag cycles};
		e{add frag entry};
		return *this;
	}
x{define frag}
```
* Bevor ein Fragment hinzugefügt werden kann, muss sichergestellt werden,
  dass kein Zykel entsteht
* Ein Zykel liegt vor, wenn `frag` gleich `child` ist
* Oder bereits direkt oder indirekt zu `child` hinzugefügt wurde
* Falls der letzte Eintrag noch kein Fragment hat, wird dieser Eintrag
  verwendet

```
d{add frag entry}
	_entries.push_back(
		FragEntry { child }
	);
x{add frag entry}
```
* Für das Kind wird ein neuer Eintrag an das Fragment angefügt

```
a{frag methods}
	auto begin() const {
		return _entries.cbegin();
	}
x{frag methods}
```
* Beginn eines konstanten Iterators auf den Einträgen

```
a{frag methods}
	auto end() const {
		return _entries.cend();
	}
x{frag methods}
```
* Ende eines konstanten Iterators auf den Einträgen

```
a{frag methods}
	int expands() const {
		return _expands;
	}
x{frag methods}
```
* Anzahl der `@expand` und `@globexpand` Aufrufe

```
a{frag methods}
	void addExpand() {
		++_expands;
	}
x{frag methods}
```
* Fügt `@expand` oder `@globexpand` hinzu

```
a{frag methods}
	int multiples() const {
		return _multiples;
	}
x{frag methods}
```
* Anzahl der `@multiple` und `@globmult` Aufrufe

```
a{frag methods}
	void addMultiple() {
		++_multiples;
	}
x{frag methods}
```
* Fügt `@multiple` oder `@globmult` hinzu

# Fragmente serialisieren
* Serialisiert Fragmente in einen `std::ostream`

```
a{define frag}
	void serializeFrag(
		const Frag &frag,
		std::ostream &out,
		bool writeLineMacros
	) {
		e{iterate entries};
	}
x{define frag}
```
* Jeder Eintrag wird nacheinander bearbeitet
* Fragmente in Einträgen werden rekursiv ausgegeben

```
d{iterate entries}
	for (const auto &entry : frag) {
		if (entry.frag) {
			serializeFrag(
				*entry.frag, out,
				writeLineMacros
			);
		}
		out << entry.str();
	}
x{iterate entries}
```
* Rekursiv wird das Fragment ausgegeben, falls vorhanden
* Dann werden die Bytes des Eintrags ausgegeben

```
a{define frag}
	void testFrag(
		const Frag &frag,
		const std::string &expected
	) {
		e{serialize test frag};
	}
x{define frag}
```
* Diese Hilfsfunktion prüft ob die Serialisierung eines Fragments der
  Erwartung entspricht

```
A{includes}
	#include <sstream>
x{includes}
```
* `f{testFrag}` benötigt `std::ostringstream`

```
d{serialize test frag}
	std::ostringstream buffer;
	serializeFrag(frag, buffer, false);
	ASSERT(buffer.str() == expected);
x{serialize test frag}
```
* Serialisiert das Fragment
* Der Buffer muss die erwarteten Werte enthalten

```
a{define frag}
	void addStringToFrag(
		Frag *frag,
		const std::string &str
	) {
		frag->add(
			str, std::string {}, 0
		);
	}
x{define frag}
```
* Zu Testzwecken kann eine Null-terminierte Zeichenkette hinzugefügt
  werden

```
a{unit tests} {
	Frag frag { "" };
	addStringToFrag(&frag, "abc");
	addStringToFrag(&frag, "def");
	testFrag(frag, "abcdef");
} x{unit tests}
```
* Prüft, ob zwei Strings richtig serialisiert werden

```
a{unit tests} {
	Frag a { "" };
	Frag b { "" };
	addStringToFrag(&a, "abc");
	b.add(&a);
	addStringToFrag(&b, "def");
	b.add(&a);
	testFrag(b, "abcdefabc");
} x{unit tests}
```
* Prüft, ob Fragmente expandiert werden

# Zykel im Fragment-Graph finden
* Prüft ob das Hinzufügen eines Fragments zu einem Zykel führt

```
d{define cycle check}
	bool isFragInFrag(
		const Frag *needle,
		const Frag *haystack
	) {
		ASSERT(needle);
		ASSERT(haystack);
		e{check cycle frag};
		e{check cycle entries};
		return false;
	}
x{define cycle check}
```
* Wenn das Fragment das gesuchte ist, dann wurde ein Zykel gefunden
* Danach wird über alle Einträge gesucht
* Wenn das Fragment dort nicht gefunden wurde, dann ist es nicht
  enthalten

```
d{avoid frag cycles}
	ASSERT(! isFragInFrag(
		this, child
	));
x{avoid frag cycles}
```
* Ein Fragment darf nur hinzugefügt werden, wenn es den Container nicht
  bereits enthält

```
d{check cycle frag}
	if (needle == haystack) {
		return true;
	}
x{check cycle frag}
```
* Wenn der Container selbst das gesuchte Fragment ist, liefert die
  Funktion `true` zurück

```
d{check cycle entries}
	for (const auto &i : *haystack)  {
		if (! i.frag) { continue; }
		if (isFragInFrag(
			needle, i.frag
		)) {
			return true;
		}
	}
x{check cycle entries}
```
* Alle Fragment in den Einträgen werden rekursiv untersucht
* Damit wird der ganze Graph durchsucht

# Fragment-Kollektion
* Kollektion von Fragmenten, die in mehreren Hierarchien organisiert
  werden

```
A{includes}
	#include <map>
x{includes}
```
* `FragMap` enthält eine `std::map`

```
a{define frag}
	class FragMap {
		FragMap *_link;
		using Map =
			std::map<std::string, Frag>;
		Map map;
	public:
		e{frag map methods};
	};
x{define frag}
```
* Eine Kollektion von Fragmenten ist ein Array von Fragment-Ketten
* Alle Felder müssen mit `nullptr` initialisiert werden

```
d{frag map methods}
	FragMap(): _link { nullptr } {}
x{frag map methods}
```
* Map wird ohne Verknüpfung und Elementen als leere Struktur angelegt

```
a{frag map methods}
	FragMap *setLink(FragMap *link) {
		FragMap *old { _link };
		_link = link;
		return old;
	}
x{frag map methods}
```
* Der Link kann dynamisch geändert werden
* Der vorher aktive Link wird zurück geliefert

```
a{frag map methods}
	Frag *find(const std::string &name) {
		auto found { map.find(name) };
		if (found != map.end()) {
			return &found->second;
		}
		if (_link) {
			return _link->find(name);
		}
		return nullptr;
	}
x{frag map methods}
```
* Sucht ein Element in der Map
* Oder in verlinkten Maps
* Wenn es nicht gefunden wird, wird `nullptr` zurück geliefert

```
a{frag map methods}
	Frag &get(
		const std::string &name,
		FragMap &insert
	) {
		Frag *found { find(name) };
		if (found) { return *found; }
		auto created { insert.map.insert(
			Map::value_type { name, name }
		) };
		return created.first->second;
	}
x{frag map methods}
```
* Sucht ein Element in der Map
* Oder in verlinkten Map
* Wenn es nicht gefunden wird, wird es in der Map `insert` angelegt
* Und zurück geliefert

```
a{frag map methods}
	Frag &operator[](
		const std::string &name
	) {
		return get(name, *this);
	}
x{frag map methods}
```
* Kurzform für `get`

```
a{frag map methods}
	auto begin() const {
		return map.cbegin();
	}
x{frag map methods}
```
* Beginn eines konstanten Iterators über Elemente der Map

```
a{frag map methods}
	auto end() const {
		return map.cend();
	}
x{frag map methods}
```
* Ende eines kostanten Iterators über Elemente der Map
