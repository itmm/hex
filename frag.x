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
	struct FragEntry;

	struct Frag {
		struct Frag *link;
		struct FragEntry *firstEntry;
		struct FragEntry *lastEntry;
		int expands;
		int multiples;
		char name[];
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
* `stdlib.h` wird für die Definition von `NULL` benötigt

# Neues Fragment anlegen

```
a{define frag}
	struct Frag *allocFrag(
		const char *nameBegin,
		const char *nameEnd
	) {
		struct Frag *result = NULL;
		e{allocate frag on heap};
		result->link = NULL;
		result->firstEntry = NULL;
		result->expands = 0;
		result->multiples = 0;
		e{copy frag name};
		return result;
	}
x{define frag}
```
* Die Zeiger werden mit `NULL` initialisiert
* `lastEntry` wird erst initialisiert, wenn `firstEntry` gesetzt
  wird
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
	int nameLength =
		nameEnd - nameBegin;
	int size = sizeof(struct Frag)
		+ nameLength + 1;
	result = malloc(size);
	ASSERT(result);
x{allocate frag on heap}
```
* Die Zeiger werden mit `NULL` initialisiert
* `lastEntry` wird erst initialisiert, wenn `firstEntry` gesetzt
  wird
* Der Name wird über zwei Zeiger übergeben, muss also nicht mit einem
  Null-Byte terminiert sein

```
A{includes}
	#include <string.h>
x{includes}

d{copy frag name}
	memcpy(
		result->name, nameBegin,
		nameLength
	);
	result->name[nameLength] = '\0';
x{copy frag name}
```
* Der Name wird direkt in das Fragment kopiert
* Der Name wird mit einem Null-Byte abgeschlossen

# Fragmente freigeben

```
a{define frag}
	void freeFragEntries(
		struct Frag *frag
	) {
		if (frag) {
			e{free frag entries};
			frag->firstEntry = NULL;
			frag->lastEntry = NULL;
		}
	}
x{define frag}
```

```
a{define frag}
	void freeFrag(
		struct Frag *f
	) {
		while (f) {
			struct Frag *l =
				f->link;
			freeFragEntries(f);
			free(f);
			f = l;
		}
	}
x{define frag}
```
* Mit einem Fragment werden auch alle verketteten Fragmente freigegeben
* Die Freigabe der einzelnen Einträge wird später definiert
		
# Unit Tests

```
A{perform unit-tests}
	e{frag unit tests};
x{perform unit-tests}

a{define frag}
	struct Frag *allocTestFrag(
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
		struct Frag *f =
			allocTestFrag(name);
		ASSERT(f);
		ASSERT(
			strcmp(f->name, name) == 0
		);
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
		struct Frag *f =
			allocTestFrag("ab");
		ASSERT(f);
		ASSERT(! f->link);
		ASSERT(! f->firstEntry);
		freeFrag(f);
	}
x{frag unit tests}
```
* Zum einen wird getestet, ob die Namen korrekt kopiert werden
* Zum anderen wird sichergestellt, dass die Verweise `NULL` sind

```
a{define frag}
	bool isPopulatedFrag(
		const struct Frag *f
	) {
		return f && f->firstEntry;
	}
x{define frag}
```

# Fragment-Einträge

```
a{define frag}
	struct FragEntry {
		struct FragEntry *link;
		struct Frag *frag;
		const char *valueEnd;
		e{additional entry attributes};
		char value[];
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
		
# Fragment-Eintrag anlegen

```
a{define frag}
	struct FragEntry *allocFragEntry(
		struct Frag *frag,
		const char *valueBegin,
		const char *valueEnd
	) {
		struct FragEntry *result = NULL;
		e{allocate entry on heap};
		result->link = NULL;
		e{copy entry values};
		return result;
	}
x{define frag}
```
* Wie bei einem Fragment werden die `link` Verweise auf `NULL`
  initialisiert
* Sowohl `frag` als auch `valueBegin` sind optional
* Die Größe des Eintrags hängt von der Anzahl der Bytes ab, die kopiert
  werden

```
d{allocate entry on heap}
	int valueLength = 0;
	if (valueBegin) {
		ASSERT(valueBegin <= valueEnd);
		valueLength =
			valueEnd - valueBegin;
	}
	int entrySize = valueLength +
		sizeof(struct FragEntry);
	result = malloc(entrySize);
	ASSERT(result);
x{allocate entry on heap}
```
* Die Größe der Struktur wird um die Anzahl der zu kopierenden Bytes
  vergrößert
* Wenn nicht genug Speicher vorhanden ist, dann wird das Programm beendet

```
d{copy entry values}
	if (valueBegin) {
		memcpy(
			result->value, valueBegin,
			valueLength
		);
	}
	result->valueEnd =
		result->value + valueLength;
	result->frag = frag;
x{copy entry values}
```
* Die Bytes werden nur kopiert, wenn welche übergeben wurden

```
a{define frag}
	struct FragEntry *
		allocEmptyFragEntry() {
			return allocFragEntry(
				NULL, NULL, NULL
			);
		}
x{define frag}
```
* Für Tests ist es praktisch, leere Einträge anzulegen

# Fragment-Einträge freigeben

```
a{define frag}
	void freeFragEntry(
		struct FragEntry *e
	) {
		while (e) {
			struct FragEntry *l =
				e->link;
			free(e);
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
		struct FragEntry *entry
	);
x{forward declarations}
```

```
d{free frag entries}
	freeFragEntry(frag->firstEntry);
x{free frag entries}
```
* Wenn ein Fragment freigegeben wird, so werden auch die anhängenden
  Einträge freigegeben
* Damit die Funktion im `freeFrag` sichtbar ist, wird sie in der
  Include-Sektion definiert

# Auf Attribute zugreifen

```
a{define frag}
	int getFragEntryValueSize(
		struct FragEntry *e
	) {
		if (! e) {
			return 0;
		}
		return e->valueEnd -
			e->value;
	}
x{define frag}
```
* Liefert die Anzahl der enthaltenen Bytes

# Unit Tests

```
a{frag unit tests}
	{
		struct FragEntry *entry =
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
		struct FragEntry *entry =
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
	struct FragEntry *
		allocTestFragEntry(
			const char *v
		) {
			const char *e = v +
				strlen(v);

			return allocFragEntry(
				NULL, v, e
			);
		}
x{define frag}
```
* Der Test-Konstruktor bekommt eine Null-terminierte Zeichenkette als
  Argument

```
a{frag unit tests}
	{
		struct FragEntry *entry =
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
		struct FragEntry *entry =
			allocTestFragEntry("abc");

		ASSERT(entry);
		ASSERT(
			memcmp(entry->value,
				"abc", 3) == 0
		);

		freeFragEntry(entry);
	}
x{frag unit tests}
```
* Die Bytes eines Eintrags stimmen überein

# Einträge zu Fragmenten hinzufügen

```
a{define frag}
	void addEntryToFrag(
		struct Frag *frag,
		struct FragEntry *entry
	) {
		e{assert add entry};
		if (frag->firstEntry) {
			e{append entry};
		} else {
			e{set first entry};
		}
	}
x{define frag}
```
* Ein Eintrag wird entweder an das Ende der Liste der Einträge angehängt
* Oder als neuer Kopf der Liste gesetzt

```
d{assert add entry}
	ASSERT(frag);
	ASSERT(entry);
	ASSERT(! entry->link);
x{assert add entry}
```
* Fragment darf nicht `NULL` sein
* Eintrag darf nicht `NULL` sein
* Eintrag darf noch nicht in einer anderen Liste hängen

```
d{append entry}
	frag->lastEntry->link = entry;
	frag->lastEntry = entry;
x{append entry}
```
* Da es schon Einträge gibt, muss es bereits einen letzten geben
* Dessen neuer Nachfolger ist der neue Eintrag
* Der neue Eintrag wird zum neuen letzten Eintrag
* Der Nachfolger von `entry` ist bereits `NULL`

```
d{set first entry}
	frag->firstEntry = entry;
	frag->lastEntry = entry;
x{set first entry}
```
* Der erste Eintrag wird auch als letzter Eintrag gesetzt
* Der Nachfolger von `entry` ist bereits `NULL`

```
a{frag unit tests}
	{
		struct Frag *f =
			allocTestFrag("");
		struct FragEntry *e =
			allocEmptyFragEntry();
		addEntryToFrag(f, e);
		ASSERT(
			f->firstEntry == e
		);
		freeFrag(f);
	}
x{frag unit tests}
```
* Der erste Eintrag im Fragment wird gesetzt

```
a{frag unit tests}
	{
		struct Frag *f =
			allocTestFrag("");
		struct FragEntry *e =
			allocEmptyFragEntry();
		addEntryToFrag(f, e);
		ASSERT(
			f->lastEntry == e
		);
		freeFrag(f);
	}
x{frag unit tests}
```
* Der letzte Eintrag im Fragment wird gesetzt

```
a{frag unit tests}
	{
		struct Frag *frag = NULL;
		struct FragEntry *first;
		struct FragEntry *second;
		E{add two entries};
		e{check first of 2};
		freeFrag(frag);
	}
x{frag unit tests}
```
* Zwei Einträge werden an ein Fragment angehängt
* Der erste Eintrag muss gesetzt bleiben

```
d{add two entries}
	frag = allocTestFrag("");
	first = allocEmptyFragEntry();
	second = allocEmptyFragEntry();

	addEntryToFrag(frag, first);
	addEntryToFrag(frag, second);
x{add two entries}

d{check first of 2}
	ASSERT(frag->firstEntry == first);
x{check first of 2}
```
* Zwei Einträge werden an ein Fragment angehängt
* Der erste Eintrag muss gesetzt bleiben

```
a{frag unit tests}
	{
		struct Frag *frag = NULL;
		struct FragEntry *first;
		struct FragEntry *second;
		E{add two entries};
		ASSERT(
			frag->lastEntry == second
		);
		freeFrag(frag);
	}
x{frag unit tests}
```
* Zwei Einträge werden an ein Fragment angehängt
* Der letzte Eintrag muss gesetzt werden

```
a{define frag}
	void addBytesToFrag(
		struct Frag *frag,
		const char *value,
		const char *valueEnd,
		const char *source,
		int line
	) {
		struct FragEntry *entry =
			allocFragEntry(
				NULL, value, valueEnd
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
		struct Frag *frag,
		struct Frag *child
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
	struct FragEntry *entry =
		allocFragEntry(
			child, NULL, NULL
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
		struct Frag *frag,
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
	struct FragEntry *entry =
		frag->firstEntry;
	for (; entry; entry = entry->link) {
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
	char *fragTestBufferCur = NULL;
	const char *fragTestBufferEnd = NULL;
x{serialize test defines}
```

```
d{serialize bytes}
	if (getFragEntryValueSize(entry)) {
		const char *cur = entry->value;
		const char *end = entry->valueEnd;
		int len = end - cur;
		if (! fragTestBufferCur) {
			ASSERT(fwrite(cur, 1, len, out) == len);
		} else {
			ASSERT(fragTestBufferCur + len < fragTestBufferEnd);
			memcpy(fragTestBufferCur, cur, len);
			fragTestBufferCur += len;
			*fragTestBufferCur = '\0';
		}
	}
x{serialize bytes}
```
* Wenn es Bytes in dem Eintrag gibt, wird der `consumer` mit ihnen
  aufgerufen

```
a{define frag}
	void testFrag(struct Frag *
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
	char buffer[100];
	fragTestBufferCur = buffer;
	fragTestBufferEnd = buffer + sizeof(buffer);
	serializeFrag(frag, (void *) buffer, false);
	ASSERT(strcmp(
		expected, buffer
	) == 0);
	fragTestBufferCur = NULL;
	fragTestBufferEnd = NULL;
x{serialize test frag}
```
* Serialisiert das Fragment
* Der Buffer muss die erwarteten Werte enthalten

```
a{define frag}
	void addStringToFrag(
		struct Frag *frag,
		const char *str
	) {
		int size = strlen(str);
		addBytesToFrag(
			frag, str, str + size,
			NULL, 0
		);
	}
x{define frag}
```
* Zu Testzwecken kann eine Null-terminierte Zeichenkette hinzugefügt
  werden

```
a{frag unit tests}
	{
		struct Frag *frag =
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
	struct Frag *a =
		allocTestFrag("");
	struct Frag *b =
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
A{includes}
	#include <stdbool.h>
x{includes}
```
* Boolesche Datenwerte werden benötigt

```
d{define cycle check}
	bool isFragInFrag(
		struct Frag *needle,
		struct Frag *haystack
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
	struct FragEntry *entry =
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
		struct FragMap *link;
		struct Frag *frags[
			FRAG_SLOTS
		];
	};
x{define frag}
```
* Eine Kollektion von Fragmenten ist ein Array von Fragment-Ketten
* Alle Felder müssen mit `NULL` initialisiert werden

```
a{define frag}
	void clearFragMap(
		struct FragMap *map
	) {
		struct Frag **cur = map->frags;
		struct Frag **end =
			cur + FRAG_SLOTS;
		for (; cur < end; ++cur) {
			freeFrag(*cur); *cur = NULL;
		}
		map->link = NULL;
	}
x{define frag}
```
* Um den Speicher freizugeben, wird jeder Slot gelöscht
* und auf <code class="keyword">NULL</code> gesetzt um wieder verwendet
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
	struct Frag *allocFragInMap(
		struct FragMap *map,
		const char *begin,
		const char *end
	) {
		ASSERT(map);
		struct Frag *frag =
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
	struct Frag *findFragInMap(
		struct FragMap *map,
		const char *begin,
		const char *end
	) {
		ASSERT(map);
		e{find frag in slot};
		e{find frag in linked map};
		return NULL;
	}
x{define frag}
```
* Liefert das erste Fragment mit dem übergebenen Namen

```
d{find frag in slot} {
	int hash = calcFragHash(begin, end);
	struct Frag *frag = map->frags[hash];
	for (; frag; frag = frag->link) {
		const char *a = begin;
		const char *b = frag->name;
		while (a != end) {
			if (*a++ != *b++) { break; }
		}
		if (a == end && ! *b) {
			return frag; }
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
	struct Frag *getFragInMap(
		struct FragMap *map,
		const char *begin,
		const char *end,
		struct FragMap *insert
	) {
		struct Frag *frag = NULL;
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
	const char *source;
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
