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
		FragEntry *link;
		Frag *frag;
		std::string value;
		e{additional entry attributes};
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
x{includes}
```

```
a{define frag}
	struct Frag {
		Frag *link;
		std::list<FragEntry> entries;
		FragEntry *firstEntry;
		FragEntry *lastEntry;
		int expands;
		int multiples;
		std::string name;
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

```
A{includes}
	#include <stdlib.h>
x{includes}
```
* `stdlib.h` wird für die Definition von `nullptr` benötigt

# Neues Fragment anlegen

```
a{define frag}
	struct Frag *allocFrag(
		const char *nameBegin,
		const char *nameEnd
	) {
		Frag *result = nullptr;
		e{allocate frag on heap};
		result->link = nullptr;
		result->expands = 0;
		result->multiples = 0;
		e{copy frag name};
		return result;
	}
x{define frag}
```
* Die Zeiger werden mit `nullptr` initialisiert
* Der Name wird über zwei Zeiger übergeben, muss also nicht mit einem
  Null-Byte terminiert sein

```
D{define logging}
	#define ASSERT(COND, ...) \
		if (! (COND)) { \
			fprintf(stderr, \
				"%s:%d", \
				__FILE__, __LINE__); \
			fprintf(stderr, \
				" FAILED: " __VA_ARGS__); \
			fprintf(stderr, "\n"); \
			exit(EXIT_FAILURE); \
		}
x{define logging}
```
* Wenn Bedingung falsch ist, wird Fehlermeldung ausgegeben
* Und das Programm beendet
* Datei und Zeile des Tests wird ausgegeben

```
d{allocate frag on heap}
	ASSERT(nameBegin);
	ASSERT(nameBegin <= nameEnd);
	result = new Frag();
	ASSERT(result);
x{allocate frag on heap}
```
* Die Zeiger werden mit `nullptr` initialisiert
* Der Name wird über zwei Zeiger übergeben, muss also nicht mit einem
  Null-Byte terminiert sein

```
A{includes}
	#include <string.h>
x{includes}

d{copy frag name}
	result->name = std::string(nameBegin, nameEnd);
x{copy frag name}
```
* Der Name wird direkt in das Fragment kopiert

# Fragmente freigeben

```
a{define frag}
	void freeFragEntries(
		Frag *frag
	) {
		if (frag) {
			frag->entries.clear();
		}
	}
x{define frag}
```

```
a{define frag}
	void freeFrag(
		Frag *f
	) {
		while (f) {
			Frag *l =
				f->link;
			delete(f);
			f = l;
		}
	}
x{define frag}
```
* Mit einem Fragment werden auch alle verketteten Fragmente freigegeben
* Die Freigabe der einzelnen Einträge wird später definiert
		
# Unit Tests

```
D{perform unit-tests}
	e{frag unit tests};
x{perform unit-tests}

a{define frag}
	Frag *allocTestFrag(
		const char *name
	) {
		return allocFrag(
			name, name + strlen(name)
		);
	}
x{define frag}
```
* Für Unit-Tests gibt es einen einfacheren Konstruktor
* Die Länge des Namens wird anhand des Null-Bytes berechnet

```
a{define frag}
	void testFragName(
		const char *name
	) {
		Frag *f =
			allocTestFrag(name);
		ASSERT(f);
		ASSERT(f->name == name);
		freeFrag(f);
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
		Frag *f =
			allocTestFrag("ab");
		ASSERT(f);
		ASSERT(! f->link);
		ASSERT(! f->firstEntry);
		freeFrag(f);
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

# Fragment-Eintrag anlegen

```
a{define frag}
	FragEntry *allocFragEntry(
		Frag *frag,
		const char *valueBegin,
		const char *valueEnd
	) {
		FragEntry *result = nullptr;
		e{allocate entry on heap};
		result->link = nullptr;
		e{copy entry values};
		return result;
	}
x{define frag}
```
* Wie bei einem Fragment werden die `link` Verweise auf `nullptr`
  initialisiert
* Sowohl `frag` als auch `valueBegin` sind optional
* Die Größe des Eintrags hängt von der Anzahl der Bytes ab, die kopiert
  werden

```
d{allocate entry on heap}
	result = new FragEntry();
x{allocate entry on heap}
```
* Die Größe der Struktur wird um die Anzahl der zu kopierenden Bytes
  vergrößert
* Wenn nicht genug Speicher vorhanden ist, dann wird das Programm beendet

```
d{copy entry values}
	if (valueBegin) {
		result->value = std::string(valueBegin, valueEnd);
	}
	result->frag = frag;
x{copy entry values}
```
* Die Bytes werden nur kopiert, wenn welche übergeben wurden

```
a{define frag}
	FragEntry *
		allocEmptyFragEntry() {
			return allocFragEntry(
				nullptr, nullptr, nullptr
			);
		}
x{define frag}
```
* Für Tests ist es praktisch, leere Einträge anzulegen

# Fragment-Einträge freigeben

```
a{define frag}
	void freeFragEntry(
		FragEntry *e
	) {
		while (e) {
			FragEntry *l =
				e->link;
			delete(e);
			e = l;
		}
	}
x{define frag}
```
* Wenn ein Eintrag freigegeben wird, so werden auch alle verlinkten
  Einträge freigegeben
* Referenzierte Fragmente werden nicht mit freigegeben

```
D{forward declarations}
	struct FragEntry;
	void freeFragEntry(
		FragEntry *entry
	);
x{forward declarations}
```

# Auf Attribute zugreifen

```
a{define frag}
	int getFragEntryValueSize(
		FragEntry *e
	) {
		if (! e) {
			return 0;
		}
		return e->value.size();
	}
x{define frag}
```
* Liefert die Anzahl der enthaltenen Bytes

# Unit Tests

```
a{frag unit tests}
	{
		FragEntry *entry =
			allocEmptyFragEntry();

		ASSERT(entry);
		ASSERT(! entry->link);
		ASSERT(! entry->frag);

		freeFragEntry(entry);
	}
x{frag unit tests}
```
* Ein leerer Eintrag hat keinen Nachfolger
* Und kein Fragment

```
a{frag unit tests}
	{
		FragEntry *entry =
			allocEmptyFragEntry();

		ASSERT(entry);
		ASSERT(
			getFragEntryValueSize(
				entry) == 0
		);

		freeFragEntry(entry);
	}
x{frag unit tests}
```
* Ein leerer Eintrag hat keine Bytes

```
a{define frag}
	FragEntry *
		allocTestFragEntry(
			const char *v
		) {
			const char *e = v +
				strlen(v);

			return allocFragEntry(
				nullptr, v, e
			);
		}
x{define frag}
```
* Der Test-Konstruktor bekommt eine Null-terminierte Zeichenkette als
  Argument

```
a{frag unit tests}
	{
		FragEntry *entry =
			allocTestFragEntry("abc");

		ASSERT(entry);
		ASSERT(
			getFragEntryValueSize(
				entry) == 3
		);

		freeFragEntry(entry);
	}
x{frag unit tests}
```
* Ein Eintrag hat die korrekte Anzahl an Bytes

```
a{frag unit tests}
	{
		FragEntry *entry =
			allocTestFragEntry("abc");

		ASSERT(entry);
		ASSERT(entry->value == "abc");

		freeFragEntry(entry);
	}
x{frag unit tests}
```
* Die Bytes eines Eintrags stimmen überein

# Einträge zu Fragmenten hinzufügen

```
a{define frag}
	void addEntryToFrag(
		Frag *frag,
		FragEntry *entry
	) {
		e{assert add entry};
		frag->entries.push_back(*entry);
	}
x{define frag}
```
* Ein Eintrag wird entweder an das Ende der Liste der Einträge angehängt
* Oder als neuer Kopf der Liste gesetzt

```
d{assert add entry}
	ASSERT(frag);
	ASSERT(entry);
x{assert add entry}
```
* Fragment darf nicht `nullptr` sein
* Eintrag darf nicht `nullptr` sein
* Eintrag darf noch nicht in einer anderen Liste hängen

```
a{define frag}
	void addBytesToFrag(
		Frag *frag,
		const char *value,
		const char *valueEnd,
		const std::string &source,
		int line
	) {
		FragEntry *entry =
			allocFragEntry(
				nullptr, value, valueEnd
			);
		e{populate additional entry fields};
		addEntryToFrag(frag, entry);
	}
x{define frag}
```
* Für die Daten wird ein neuer Eintrag angelegt
* Dieser wird an das Fragment angehängt

```
a{define frag}
	e{define cycle check}
	void addFragToFrag(
		Frag *frag,
		Frag *child
	) {
		ASSERT(frag);
		ASSERT(child);
		e{avoid frag cycles};
		e{reuse last entry};
		e{add frag entry};
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
	if (frag->firstEntry &&
		! frag->lastEntry->frag
	) {
		frag->lastEntry->frag = child;
		return;
	}
x{reuse last entry}
```
* Wenn das Fragment-Attribut im letzten Eintrag noch nicht benutzt wird,
  kann dieses verwendet werden

```
d{add frag entry}
	FragEntry *entry =
		allocFragEntry(
			child, nullptr, nullptr
		);
	addEntryToFrag(frag, entry);
x{add frag entry}
```
* Sonst muss ein neuer Eintrag in dem Fragment angelegt werden

# Fragmente serialisieren

```
a{define frag}
	e{serialize test defines};
	void serializeFrag(
		Frag *frag,
		FILE *out,
		bool writeLineMacros
	) {
		ASSERT(frag);
		ASSERT(out);
		e{iterate entries};
	}
x{define frag}
```
* Jeder Eintrag wird nacheinander bearbeitet
* Fragmente in Einträgen werden rekursiv ausgegeben

```
d{iterate entries}
	auto entry = frag->entries.begin();
	for (; entry != frag->entries.end(); ++entry) {
		e{serialize bytes};
		if (entry->frag) {
			serializeFrag(
				entry->frag, out,
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
	if (getFragEntryValueSize(&*entry)) {
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
	void testFrag(Frag *
		frag, const char *expected
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
		const char *str
	) {
		int size = strlen(str);
		addBytesToFrag(
			frag, str, str + size,
			std::string(), 0
		);
	}
x{define frag}
```
* Zu Testzwecken kann eine Null-terminierte Zeichenkette hinzugefügt
  werden

```
a{frag unit tests}
	{
		Frag *frag =
			allocTestFrag("");
		addStringToFrag(frag, "abc");
		addStringToFrag(frag, "def");
		testFrag(frag, "abcdef");
		freeFrag(frag);
	}
x{frag unit tests}
```
* Prüft, ob zwei Strings richtig serialisiert werden

```
a{frag unit tests} {
	Frag *a =
		allocTestFrag("");
	Frag *b =
		allocTestFrag("");
	addStringToFrag(a, "abc");
	addFragToFrag(b, a);
	addStringToFrag(b, "def");
	addFragToFrag(b, a);
	testFrag(b, "abcdefabc");
	freeFrag(a); freeFrag(b);
} x{frag unit tests}
```
* Prüft, ob Fragmente expandiert werden

# Zykel im Fragment-Graph finden

```
d{define cycle check}
	bool isFragInFrag(
		Frag *needle,
		Frag *haystack
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
		frag, child
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
	FragEntry *entry =
		haystack->firstEntry;
	for (; entry; entry = entry->link) {
		if (! entry->frag) { continue; }
		if (isFragInFrag(
			needle, entry->frag
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
	#define FRAG_SLOTS 128

	struct FragMap {
		FragMap *link;
		Frag *frags[
			FRAG_SLOTS
		];
	};
x{define frag}
```
* Eine Kollektion von Fragmenten ist ein Array von Fragment-Ketten
* Alle Felder müssen mit `nullptr` initialisiert werden

```
a{define frag}
	void clearFragMap(
		FragMap *map
	) {
		Frag **cur = map->frags;
		Frag **end =
			cur + FRAG_SLOTS;
		for (; cur < end; ++cur) {
			freeFrag(*cur); *cur = nullptr;
		}
		map->link = nullptr;
	}
x{define frag}
```
* Um den Speicher freizugeben, wird jeder Slot gelöscht
* und auf `nullptr` gesetzt um wieder verwendet
  zu werden
* Wenn es einen Link auf ein andere Map gibt, wird diese zurückgesetzt
* Die referenzierte Map wird jedoch nicht gelöscht

```
a{define frag}
	int calcFragHash(
		const char *begin, const char *end
	) {
		int hash = calcHash(begin, end);
		return hash % FRAG_SLOTS;
	}
x{define frag}
```
* Der Hash wird über den Namen des Fragments erstellt
* Und auf die Anzahl der möglichen Slots beschränkt

```
a{define frag}
	Frag *allocFragInMap(
		FragMap *map,
		const char *begin,
		const char *end
	) {
		ASSERT(map);
		Frag *frag =
			allocFrag(begin, end);
		e{insert in slot};
		return frag;
	}
x{define frag}
```
* Ein neues Fragment wird erstellt
* Und in der Hash-Map abgelegt

```
d{insert in slot}
	int hash = calcFragHash(begin, end);
	frag->link = map->frags[hash];
	map->frags[hash] = frag;
x{insert in slot}
```
* Fragment wird im Slot eingefügt
* Neue Fragmente überlagern alte Fragmente mit gleichem Namen

```
a{define frag}
	Frag *findFragInMap(
		FragMap *map,
		const char *begin,
		const char *end
	) {
		ASSERT(map);
		e{find frag in slot};
		e{find frag in linked map};
		return nullptr;
	}
x{define frag}
```
* Liefert das erste Fragment mit dem übergebenen Namen

```
d{find frag in slot} {
	int hash = calcFragHash(begin, end);
	Frag *frag = map->frags[hash];
	std::string s(begin, end);
	for (; frag; frag = frag->link) {
		if (s == frag->name) {
			return frag;
		}
	}
} x{find frag in slot}
```
* Im passenden Hash-Slot werden die Namen der Fragmente verglichen

```
d{find frag in linked map}
	if (map->link) {
		return findFragInMap(
			map->link, begin, end
		);
	}
x{find frag in linked map}
```
* Wenn es einen verlinkten Kontext gibt, so kann das Element auch in
  diesem liegen

```
a{define frag}
	Frag *getFragInMap(
		FragMap *map,
		const char *begin,
		const char *end,
		FragMap *insert
	) {
		Frag *frag = nullptr;
		e{get frag find};
		e{get frag alloc};
		return frag;
	}
x{define frag}
```
* Liefert Fragment mit angegebenen Namen oder legt ein neues Fragment
  mit diesem Namen an

```
d{get frag find}
	frag = findFragInMap(
		map, begin, end
	);
	if (frag) {
		return frag;
	}
x{get frag find}
```
* Wenn das Fragment in der Kollektion vorhanden ist, wird dieses
  verwendet

```
d{get frag alloc}
	frag = allocFragInMap(
		map, begin, end
	);
x{get frag alloc}
```
* Sonst wird ein neues Fragment angelegt

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
	entry->source = source;
	entry->line = line;
x{populate additional entry fields}
```
