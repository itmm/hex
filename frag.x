# Fragmente
* Fragmente bilden einen gerichteten azyklischen Graph
* Die Infix-Traversierung dieses Graphen bildet die generierten
  Source-Code Dateien

```
i{hash.x}
```
* Hash-Funktion wird benötigt
* Und in der Datei als `calcHash` definiiert

```
A{global elements}
	e{define frag};
x{global elements}
```
* Fragmente sind global sichtbare Strukturen

```
d{define frag}
	struct Frag;

	struct FragEntry {
		Frag *frag;
		std::string value;
		e{additional entry attributes};
		FragEntry(
			const std::string &value = std::string()
		):
			frag(nullptr),
			value(value)
		{}
	};
x{define frag}
```
* Ein Eintrag kann entweder auf ein anderes Fragment verweisen (wenn
  dieses an der aktuellen Stelle expandiert werden soll)
* Oder er enthält Bytes, die beim Expandieren direkt expandiert werden
* Die Länge des Byte-Arrays wird über einen Zeiger angezeigt (damit auch
  Null-Bytes verwendet werden können)
* Wenn ein Eintrag sowohl Daten als auch ein Fragment enthält, so wird
  zuerst der Text ausgegeben

```
A{includes}
	#include <list>
	#include <map>
x{includes}
```

```
a{define frag}
	struct Frag {
		std::list<FragEntry> entries;
		int expands;
		int multiples;
		std::string name;
		e{frag methods};
	};
x{define frag}
```
* Fragmente erden in einfach verketteten Listen gesammelt
* Genauso werden die Einträge eines Fragments in einer einfach
  verketteten Liste organisiert
* Um schnell Einträge einfügen zu können, gibt es auch einen Verweis auf
  das letzte Element
* Je nach Namen werden für ein Fragment unterschiedlich viele Bytes im
  Heap angefordert

# Neues Fragment anlegen

```
d{frag methods}
	Frag(
		const std::string &name
	):
		entries(),
		expands(0),
		multiples(0),
		name(name)
	{ }
x{frag methods}
```
* Die Zeiger werden mit `nullptr` initialisiert

```
a{frag methods}
	void clear() {
		entries.clear();
	}
x{frag methods}
```

```
A{includes}
	#include <string.h>
x{includes}

# Unit Tests

```
D{perform unit-tests}
	e{frag unit tests};
x{perform unit-tests}

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
d{frag unit tests}
	testFragName("abc");
	testFragName("");
	testFragName("A c");
	{
		Frag f("ab");
		ASSERT(f.entries.empty());
	}
x{frag unit tests}
```
* Zum einen wird getestet, ob die Namen korrekt kopiert werden
* Zum anderen wird sichergestellt, dass die Verweise `nullptr` sind

```
a{define frag}
	bool isPopulatedFrag(
		const Frag *f
	) {
		return f && f->entries.size();
	}
x{define frag}
```

# Auf Attribute zugreifen

```
a{define frag}
	int getFragEntryValueSize(
		const FragEntry &e
	) {
		return e.value.size();
	}
x{define frag}
```
* Liefert die Anzahl der enthaltenen Bytes

# Unit Tests

```
a{frag unit tests}
	{
		FragEntry entry;
		ASSERT(! entry.frag);
	}
x{frag unit tests}
```
* Ein leerer Eintrag hat keinen Nachfolger
* Und kein Fragment

```
a{frag unit tests}
	{
		FragEntry entry;
		ASSERT(
			getFragEntryValueSize(entry) == 0
		);
	}
x{frag unit tests}
```
* Ein leerer Eintrag hat keine Bytes

```
a{frag unit tests}
	{
		FragEntry entry("abc");
		ASSERT(
			getFragEntryValueSize(entry) == 3
		);
	}
x{frag unit tests}
```
* Ein Eintrag hat die korrekte Anzahl an Bytes

```
a{frag unit tests}
	{
		FragEntry entry("abc");
		ASSERT(entry.value == "abc");
	}
x{frag unit tests}
```
* Die Bytes eines Eintrags stimmen überein

# Einträge zu Fragmenten hinzufügen

```
a{frag methods}
	Frag &add(
		const std::string &value,
		const std::string &source,
		int line
	) {
		entries.push_back(FragEntry());
		FragEntry &entry = entries.back();
		entry.value += value;
		e{populate additional entry fields};
		return *this;
	}
x{frag methods}
```

```
a{frag methods}
	Frag &add(Frag *child);
x{frag methods}
```

```
a{define frag}
	e{define cycle check}
	Frag &Frag::add(Frag *child) {
		ASSERT(child);
		e{avoid frag cycles};
		e{reuse last entry};
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
d{reuse last entry}
	if (! entries.empty() &&
		! entries.back().frag
	) {
		entries.back().frag = child;
		return *this;
	}
x{reuse last entry}
```
* Wenn das Fragment-Attribut im letzten Eintrag noch nicht benutzt wird,
  kann dieses verwendet werden

```
d{add frag entry}
	entries.push_back(FragEntry());
	FragEntry &entry = entries.back();
	entry.frag = child;
x{add frag entry}
```
* Sonst muss ein neuer Eintrag in dem Fragment angelegt werden

# Fragmente serialisieren

```
a{define frag}
	e{serialize test defines};
	void serializeFrag(
		const Frag &frag, FILE *out,
		bool writeLineMacros
	) {
		ASSERT(out);
		e{iterate entries};
	}
x{define frag}
```
* Jeder Eintrag wird nacheinander bearbeitet
* Fragmente in Einträgen werden rekursiv ausgegeben

```
d{iterate entries}
	auto entry = frag.entries.begin();
	for (; entry != frag.entries.end(); ++entry) {
		e{serialize bytes};
		if (entry->frag) {
			serializeFrag(
				*entry->frag, out,
				writeLineMacros
			);
		}
	}
x{iterate entries}
```
* Für jeden Eintrag werden zuerst die Bytes ausgegeben
* Dann wird rekursiv das Fragment ausgegeben, falls vorhanden

```
d{serialize test defines}
	std::string *fragTestBufferCur = nullptr;
x{serialize test defines}
```

```
d{serialize bytes}
	if (getFragEntryValueSize(*entry)) {
		if (! fragTestBufferCur) {
			ASSERT(fwrite(
				entry->value.data(), 1, entry->value.size(),
				out
			) == entry->value.size());
		} else {
			*fragTestBufferCur += entry->value;
		}
	}
x{serialize bytes}
```
* Wenn es Bytes in dem Eintrag gibt, wird der `consumer` mit ihnen
  aufgerufen

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
d{serialize test frag}
	std::string buffer;
	fragTestBufferCur = &buffer;
	serializeFrag(frag, (FILE *) &buffer, false);
	ASSERT(buffer == expected);
	fragTestBufferCur = nullptr;
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
			str, std::string(), 0
		);
	}
x{define frag}
```
* Zu Testzwecken kann eine Null-terminierte Zeichenkette hinzugefügt
  werden

```
a{frag unit tests} {
	Frag frag("");
	addStringToFrag(&frag, "abc");
	addStringToFrag(&frag, "def");
	testFrag(frag, "abcdef");
} x{frag unit tests}
```
* Prüft, ob zwei Strings richtig serialisiert werden

```
a{frag unit tests} {
	Frag a("");
	Frag b("");
	addStringToFrag(&a, "abc");
	b.add(&a);
	addStringToFrag(&b, "def");
	b.add(&a);
	testFrag(b, "abcdefabc");
} x{frag unit tests}
```
* Prüft, ob Fragmente expandiert werden

# Zykel im Fragment-Graph finden

```
d{define cycle check}
	bool isFragInFrag(
		Frag *needle, Frag *haystack
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
	for (
		auto i = haystack->entries.begin();
		i != haystack->entries.end(); ++i
	) {
		if (! i->frag) { continue; }
		if (isFragInFrag(
			needle, i->frag
		)) {
			return true;
		}
	}
x{check cycle entries}
```
* Alle Fragment in den Einträgen werden rekursiv untersucht
* Damit wird der ganze Graph durchsucht

# Fragment-Kollektion

```
a{define frag}
	struct FragMap {
		FragMap *link;
		std::map<std::string, Frag> map;
		e{frag map methods};
	};
x{define frag}
```
* Eine Kollektion von Fragmenten ist ein Array von Fragment-Ketten
* Alle Felder müssen mit `nullptr` initialisiert werden

```
d{frag map methods}
	FragMap(): link(nullptr) {}
x{frag map methods}
```

```
a{frag map methods}
	Frag *find(const std::string &name) {
		auto found = map.find(name);
		if (found != map.end()) {
			return &found->second;
		}
		if (link) {
			return link->find(name);
		}
		return nullptr;
	}
x{frag map methods}
```

```
a{frag map methods}
	Frag &get(const std::string &name, FragMap &insert) {
		Frag *found = find(name);
		if (found) { return *found; }
		auto created = insert.map.insert(
			std::pair<std::string, Frag>(name, name)
		);
		return created.first->second;
	}
x{frag map methods}
```

```
a{frag map methods}
	Frag &operator[](const std::string &name) {
		return get(name, *this);
	}
x{frag map methods}
```

# Position im Original merken

```
d{additional entry attributes}
	std::string source;
	int line;
x{additional entry attributes}
```
* Jedes Fragment hält einen Zeiger auf die Datei aus der das Fragment
  generiert wurde
* Und die Zeile in dieser Datei
* So kann durch spezielle `#line` Makros im generierten Source-Code auf
  die ursprüngliche Datei verwiesen werden

```
d{populate additional entry fields}
	entry.source = source;
	entry.line = line;
x{populate additional entry fields}
```
