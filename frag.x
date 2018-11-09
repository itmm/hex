# Fragmente
* Fragmente bilden einen gerichteten azyklischen Graph
* Die Infix-Traversierung dieses Graphen bildet die generierten
  Source-Code Dateien

```
a{global elements}
	e{define frag};
x{global elements}
```
* Fragmente sind global sichtbare Strukturen

```
d{define frag}
	t{struct FragEntry};

	t{struct Frag} {
		t{struct Frag *}v{link};
		t{struct FragEntry *}v{firstEntry};
		t{struct FragEntry *}v{lastEntry};
		t{int} v{expands};
		t{int} v{multiples};
		t{char }v{name}t{[]};
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
a{includes}
	#include <stdlib.h>
x{includes}
```
* `stdlib.h` wird für die Definition von `k{NULL}` benötigt

# Neues Fragment anlegen

```
a{define frag}
	t{struct Frag *}f{allocFrag}(
		t{const char *}v{nameBegin},
		t{const char *}v{nameEnd}
	) {
		t{struct Frag *}v{result} = k{NULL};
		e{allocate frag on heap};
		v{result}->v{link} = k{NULL};
		v{result}->v{firstEntry} = k{NULL};
		v{result}->v{expands} = 0;
		v{result}->v{multiples} = 0;
		e{copy frag name};
		k{return} v{result};
	}
x{define frag}
```
* Die Zeiger werden mit `k{NULL}` initialisiert
* `v{lastEntry}` wird erst initialisiert, wenn `v{firstEntry}` gesetzt
  wird
* Der Name wird über zwei Zeiger übergeben, muss also nicht mit einem
  Null-Byte terminiert sein

```
d{define logging}
	#define ASSERT(v{COND}, ...) \
		k{if} (! (v{COND})) { \
			f{fprintf}(v{stderr}, \
				s{"%s:%d"}, \
				__FILE__, __LINE__); \
			f{fprintf}(v{stderr}, \
				s{" FAILED: "} __VA_ARGS__); \
			f{fprintf}(v{stderr}, s{"\n"}); \
			f{exit}(v{EXIT_FAILURE}); \
		}
x{define logging}
```
* Wenn Bedingung falsch ist, wird Fehlermeldung ausgegeben
* Und das Programm beendet
* Datei und Zeile des Tests wird ausgegeben

```
d{allocate frag on heap}
	f{ASSERT}(v{nameBegin});
	f{ASSERT}(v{nameBegin} <= v{nameEnd});
	t{int} v{nameLength} =
		v{nameEnd} - v{nameBegin};
	t{int} v{size} = k{sizeof}(t{struct Frag})
		+ v{nameLength} + 1;
	v{result} = v{malloc}(v{size});
	f{ASSERT}(v{result});
x{allocate frag on heap}
```
* Die Zeiger werden mit `k{NULL}` initialisiert
* `v{lastEntry}` wird erst initialisiert, wenn `v{firstEntry}` gesetzt
  wird
* Der Name wird über zwei Zeiger übergeben, muss also nicht mit einem
  Null-Byte terminiert sein

```
a{includes}
	#include <string.h>
x{includes}

d{copy frag name}
	f{memcpy}(
		v{result}->v{name}, v{nameBegin},
		v{nameLength}
	);
	v{result}->v{name}[v{nameLength}] = s{'\0'};
x{copy frag name}
```
* Der Name wird direkt in das Fragment kopiert
* Der Name wird mit einem Null-Byte abgeschlossen

# Fragmente freigeben

```
a{define frag}
	t{void} f{freeFragEntries}(
		t{struct Frag *}v{frag}
	) {
		k{if} (v{frag}) {
			e{free frag entries};
			v{frag}->v{firstEntry} = k{NULL};
			v{frag}->v{lastEntry} = k{NULL};
		}
	}
x{define frag}
```

```
a{define frag}
	t{void} f{freeFrag}(
		t{struct Frag *}v{f}
	) {
		k{while} (v{f}) {
			t{struct Frag *}v{l} =
				v{f}->v{link};
			freeFragEntries(f);
			f{free}(v{f});
			v{f} = v{l};
		}
	}
x{define frag}
```
* Mit einem Fragment werden auch alle verketteten Fragmente freigegeben
* Die Freigabe der einzelnen Einträge wird später definiert
		
# Unit Tests

```
a{perform unit-tests}
	e{frag unit tests};
x{perform unit-tests}

a{define frag}
	t{struct Frag *}f{allocTestFrag}(
		t{const char *}v{name}
	) {
		k{return} f{allocFrag}(
			v{name}, v{name} + f{strlen}(v{name})
		);
	}
x{define frag}
```
* Für Unit-Tests gibt es einen einfacheren Konstruktor
* Die Länge des Namens wird anhand des Null-Bytes berechnet

```
a{define frag}
	k{void} f{testFragName}(
		t{const char *}v{name}
	) {
		t{struct Frag *}v{f} =
			f{allocTestFrag}(v{name});
		f{ASSERT}(v{f});
		f{ASSERT}(
			f{strcmp}(v{f}->v{name}, v{name}) == 0
		);
		f{freeFrag}(v{f});
	}
x{define frag}
```
* `f{testFragName}` prüft, ob der Name korrekt in ein Fragment kopiert
  wurde

```
d{frag unit tests}
	f{testFragName}(s{"abc"});
	f{testFragName}(s{""});
	f{testFragName}(s{"A c"});
	{
		t{struct Frag *}v{f} =
			f{allocTestFrag}(s{"ab"});
		f{ASSERT}(v{f});
		f{ASSERT}(! v{f}->v{link});
		f{ASSERT}(! v{f}->v{firstEntry});
		f{freeFrag}(v{f});
	}
x{frag unit tests}
```
* Zum einen wird getestet, ob die Namen korrekt kopiert werden
* Zum anderen wird sichergestellt, dass die Verweise `k{NULL}` sind

```
a{define frag}
	t{bool} f{isPopulatedFrag}(
		t{const struct Frag *}v{f}
	) {
		return f && f->firstEntry;
	}
x{define frag}
```

# Fragment-Einträge

```
a{define frag}
	t{struct FragEntry} {
		t{struct FragEntry *}v{link};
		t{struct Frag *}v{frag};
		t{const char *}v{valueEnd};
		e{additional entry attributes};
		t{char }v{value}t{[]};
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
	t{struct FragEntry *}f{allocFragEntry}(
		t{struct Frag *}v{frag},
		t{const char *}v{valueBegin},
		t{const char *}v{valueEnd}
	) {
		t{struct FragEntry *}v{result} = k{NULL};
		e{allocate entry on heap};
		v{result}->v{link} = k{NULL};
		e{copy entry values};
		k{return} v{result};
	}
x{define frag}
```
* Wie bei einem Fragment werden die `v{link}` Verweise auf `k{NULL}`
  initialisiert
* Sowohl `v{frag}` als auch `v{valueBegin}` sind optional
* Die Größe des Eintrags hängt von der Anzahl der Bytes ab, die kopiert
  werden

```
d{allocate entry on heap}
	t{int} v{valueLength} = 0;
	k{if} (v{valueBegin}) {
		f{ASSERT}(v{valueBegin} <= v{valueEnd});
		v{valueLength} =
			v{valueEnd} - v{valueBegin};
	}
	t{int} v{entrySize} = v{valueLength} +
		k{sizeof}(t{struct FragEntry});
	v{result} = v{malloc}(v{entrySize});
	f{ASSERT}(v{result});
x{allocate entry on heap}
```
* Die Größe der Struktur wird um die Anzahl der zu kopierenden Bytes
  vergrößert
* Wenn nicht genug Speicher vorhanden ist, dann wird das Programm beendet

```
d{copy entry values}
	k{if} (v{valueBegin}) {
		f{memcpy}(
			v{result}->v{value}, v{valueBegin},
			v{valueLength}
		);
	}
	v{result}->v{valueEnd} =
		v{result}->v{value} + v{valueLength};
	v{result}->v{frag} = v{frag};
x{copy entry values}
```
* Die Bytes werden nur kopiert, wenn welche übergeben wurden

```
a{define frag}
t{struct FragEntry *}
f{allocEmptyFragEntry}() {
	k{return} f{allocFragEntry}(
		k{NULL}, k{NULL}, k{NULL}
	);
}
x{define frag}
```
* Für Tests ist es praktisch, leere Einträge anzulegen

# Fragment-Einträge freigeben

```
a{define frag}
k{void} f{freeFragEntry}(
	t{struct FragEntry *}v{e}
) {
	k{while} (v{e}) {
		t{struct FragEntry *}v{l} =
			v{e}->v{link};
		f{free}(v{e});
		v{e} = v{l};
	}
}
x{define frag}
```
* Wenn ein Eintrag freigegeben wird, so werden auch alle verlinkten
  Einträge freigegeben
* Referenzierte Fragmente werden nicht mit freigegeben

```
d{forward declarations}
t{struct FragEntry};
k{void} f{freeFragEntry}(
	t{struct FragEntry *}v{entry}
);
x{forward declarations}
```

```
d{free frag entries}
	f{freeFragEntry}(v{frag}->v{firstEntry});
x{free frag entries}
```
* Wenn ein Fragment freigegeben wird, so werden auch die anhängenden
  Einträge freigegeben
* Damit die Funktion im `f{freeFrag}` sichtbar ist, wird sie in der
  Include-Sektion definiert

# Auf Attribute zugreifen

```
a{define frag}
	k{int} f{getFragEntryValueSize}(
		t{struct FragEntry *}v{e}
	) {
		k{if} (! v{e}) {
			k{return} n{0};
		}
		k{return} v{e}->v{valueEnd} -
			v{e}->v{value};
	}
x{define frag}
```
* Liefert die Anzahl der enthaltenen Bytes

# Unit Tests

```
a{frag unit tests}
	{
		t{struct FragEntry *}v{entry} =
			f{allocEmptyFragEntry}();

		f{ASSERT}(v{entry});
		f{ASSERT}(! v{entry}->v{link});
		f{ASSERT}(! v{entry}->v{frag});

		f{freeFragEntry}(v{entry});
	}
x{frag unit tests}
```
* Ein leerer Eintrag hat keinen Nachfolger
* Und kein Fragment

```
a{frag unit tests}
	{
		t{struct FragEntry *}v{entry} =
			f{allocEmptyFragEntry}();

		f{ASSERT}(v{entry});
		f{ASSERT}(
			f{getFragEntryValueSize}(
				v{entry}) == n{0);}

		f{freeFragEntry}(v{entry});
	}
x{frag unit tests}
```
* Ein leerer Eintrag hat keine Bytes

```
a{define frag}
	t{struct FragEntry *}
	f{allocTestFragEntry}(
		t{const char *}v{v}
	) {
		t{const char *}v{e} = v{v} +
			f{strlen}(v{v});

		k{return} f{allocFragEntry}(
			k{NULL}, v{v}, v{e}
		);
	}
x{define frag}
```
* Der Test-Konstruktor bekommt eine Null-terminierte Zeichenkette als
  Argument

```
a{frag unit tests}
	{
		t{struct FragEntry *}v{entry} =
			f{allocTestFragEntry}(s{"abc"});

		f{ASSERT}(v{entry});
		f{ASSERT}(
			f{getFragEntryValueSize}(
				v{entry}) == n{3);}

		f{freeFragEntry}(v{entry});
	}
x{frag unit tests}
```
* Ein Eintrag hat die korrekte Anzahl an Bytes

```
a{frag unit tests}
	{
		t{struct FragEntry *}v{entry} =
			f{allocTestFragEntry}(s{"abc"});

		f{ASSERT}(v{entry});
		f{ASSERT}(
			f{memcmp}(v{entry}->v{value},
				s{"abc"}, n{3}) == n{0);}

		f{freeFragEntry}(v{entry});
	}
x{frag unit tests}
```
* Die Bytes eines Eintrags stimmen überein

# Einträge zu Fragmenten hinzufügen

```
a{define frag}
	t{void} f{addEntryToFrag}(
		t{struct Frag *}v{frag},
		t{struct FragEntry *}v{entry}
	) {
		e{assert add entry};
		k{if} (v{frag}->v{firstEntry}) {
			e{append entry};
		} k{else} {
			e{set first entry};
		}
	}
x{define frag}
```
* Ein Eintrag wird entweder an das Ende der Liste der Einträge angehängt
* Oder als neuer Kopf der Liste gesetzt

```
d{assert add entry}
	f{ASSERT}(v{frag});
	f{ASSERT}(v{entry});
	f{ASSERT}(! v{entry}->v{link});
x{assert add entry}
```
* Fragment darf nicht `k{NULL}` sein
* Eintrag darf nicht `k{NULL}` sein
* Eintrag darf noch nicht in einer anderen Liste hängen

```
d{append entry}
	v{frag}->v{lastEntry}->v{link} = v{entry};
	v{frag}->v{lastEntry} = v{entry};
x{append entry}
```
* Da es schon Einträge gibt, muss es bereits einen letzten geben
* Dessen neuer Nachfolger ist der neue Eintrag
* Der neue Eintrag wird zum neuen letzten Eintrag
* Der Nachfolger von `v{entry}` ist bereits `k{NULL}`

```
d{set first entry}
	v{frag}->v{firstEntry} = v{entry};
	v{frag}->v{lastEntry} = v{entry};
x{set first entry}
```
* Der erste Eintrag wird auch als letzter Eintrag gesetzt
* Der Nachfolger von `v{entry}` ist bereits `k{NULL}`

```
a{frag unit tests}
	{
		t{struct Frag *}v{f} =
			f{allocTestFrag}(s{""});
		t{struct FragEntry *}v{e} =
			f{allocEmptyFragEntry}();
		f{addEntryToFrag}(v{f}, v{e});
		f{ASSERT}(
			v{f}->v{firstEntry} == v{e}
		);
		f{freeFrag}(v{f});
	}
x{frag unit tests}
```
* Der erste Eintrag im Fragment wird gesetzt

```
a{frag unit tests}
	{
		t{struct Frag *}v{f} =
			f{allocTestFrag}(s{""});
		t{struct FragEntry *}v{e} =
			f{allocEmptyFragEntry}();
		f{addEntryToFrag}(v{f}, v{e});
		f{ASSERT}(
			v{f}->v{lastEntry} == v{e}
		);
		f{freeFrag}(v{f});
	}
x{frag unit tests}
```
* Der letzte Eintrag im Fragment wird gesetzt

```
a{frag unit tests}
	{
		t{struct Frag *}v{frag} = k{NULL};
		t{struct FragEntry *}v{first};
		t{struct FragEntry *}v{second};
		E{add two entries};
		e{check first of 2};
		f{freeFrag}(v{frag});
	}
x{frag unit tests}
```
* Zwei Einträge werden an ein Fragment angehängt
* Der erste Eintrag muss gesetzt bleiben

```
d{add two entries}
	v{frag} = f{allocTestFrag}(s{""});
	v{first} = f{allocEmptyFragEntry}();
	v{second} = f{allocEmptyFragEntry}();

	f{addEntryToFrag}(v{frag}, v{first});
	f{addEntryToFrag}(v{frag}, v{second});
x{add two entries}

d{check first of 2}
	f{ASSERT}(v{frag}->v{firstEntry} == v{first});
x{check first of 2}
```
* Zwei Einträge werden an ein Fragment angehängt
* Der erste Eintrag muss gesetzt bleiben

```
a{frag unit tests}
	{
		t{struct Frag *}v{frag} = k{NULL};
		t{struct FragEntry *}v{first};
		t{struct FragEntry *}v{second};
		E{add two entries};
		f{ASSERT}(
			v{frag}->v{lastEntry} == v{second}
		);
		f{freeFrag}(v{frag});
	}
x{frag unit tests}
```
* Zwei Einträge werden an ein Fragment angehängt
* Der letzte Eintrag muss gesetzt werden

```
a{define frag}
	t{void} f{addBytesToFrag}(
		t{struct Frag *}v{frag},
		t{const char *}v{value},
		t{const char *}v{valueEnd},
		t{struct Input *}v{input},
		t{int} v{line}
	) {
		t{struct FragEntry *}v{entry} =
			f{allocFragEntry}(
				k{NULL}, v{value}, v{valueEnd}
			);
		e{populate additional entry fields};
		f{addEntryToFrag}(v{frag}, v{entry});
	}
x{define frag}
```
* Für die Daten wird ein neuer Eintrag angelegt
* Dieser wird an das Fragment angehängt

```
a{define frag}
	e{define cycle check}
	t{void} f{addFragToFrag}(
		t{struct Frag *}v{frag},
		t{struct Frag *}v{child}
	) {
		f{ASSERT}(v{frag});
		f{ASSERT}(v{child});
		e{avoid frag cycles};
		e{reuse last entry};
		e{add frag entry};
	}
x{define frag}
```
* Bevor ein Fragment hinzugefügt werden kann, muss sichergestellt werden,
  dass kein Zykel entsteht
* Ein Zykel liegt vor, wenn `v{frag}` gleich `v{child}` ist
* Oder bereits direkt oder indirekt zu `v{child}` hinzugefügt wurde
* Falls der letzte Eintrag noch kein Fragment hat, wird dieser Eintrag
  verwendet

```
d{reuse last entry}
	k{if} (v{frag}->v{firstEntry} &&
		! v{frag}->v{lastEntry}->v{frag}
	) {
		v{frag}->v{lastEntry}->v{frag} = v{child};
		k{return};
	}
x{reuse last entry}
```
* Wenn das Fragment-Attribut im letzten Eintrag noch nicht benutzt wird,
  kann dieses verwendet werden

```
d{add frag entry}
	t{struct FragEntry *}v{entry} =
		f{allocFragEntry}(
			v{child}, k{NULL}, k{NULL}
		);
	f{addEntryToFrag}(v{frag}, v{entry});
x{add frag entry}
```
* Sonst muss ein neuer Eintrag in dem Fragment angelegt werden

# Fragmente serialisieren

```
a{define frag}
	e{serialize test defines};
	t{void} f{serializeFrag}(
		t{struct Frag *}v{frag},
		t{FILE *}v{out},
		t{bool} v{writeLineMacros}
	) {
		f{ASSERT}(v{frag});
		f{ASSERT}(v{out});
		e{iterate entries};
	}
x{define frag}
```
* Jeder Eintrag wird nacheinander bearbeitet
* Fragmente in Einträgen werden rekursiv ausgegeben

```
d{iterate entries}
	t{struct FragEntry *}v{entry} =
		v{frag}->v{firstEntry};
	k{for} (; v{entry}; v{entry} = v{entry}->v{link}) {
		e{serialize bytes};
		k{if} (v{entry}->v{frag}) {
			f{serializeFrag}(
				v{entry}->v{frag}, v{out},
				v{writeLineMacros}
			);
		}
	}
x{iterate entries}
```
* Für jeden Eintrag werden zuerst die Bytes ausgegeben
* Dann wird rekursiv das Fragment ausgegeben, falls vorhanden

```
d{serialize test defines}
	t{char *}v{fragTestBufferCur} = k{NULL};
	t{const char *}v{fragTestBufferEnd} = k{NULL};
x{serialize test defines}
```

```
d{serialize bytes}
	k{if} (f{getFragEntryValueSize}(v{entry})) {
		t{const char *}v{cur} = v{entry}->v{value};
		t{const char *}v{end} = v{entry}->v{valueEnd};
		t{int} v{len} = v{end} - v{cur};
		k{if} (! v{fragTestBufferCur}) {
			f{ASSERT}(f{fwrite}(v{cur}, 1, v{len}, v{out}) == v{len});
		} k{else} {
			f{ASSERT}(v{fragTestBufferCur} + v{len} < v{fragTestBufferEnd});
			f{memcpy}(v{fragTestBufferCur}, v{cur}, v{len});
			v{fragTestBufferCur} += v{len};
			*v{fragTestBufferCur} = s{'\0'};
		}
	}
x{serialize bytes}
```
* Wenn es Bytes in dem Eintrag gibt, wird der `v{consumer}` mit ihnen
  aufgerufen

```
a{define frag}
	t{void} f{testFrag}(t{struct Frag *}
		v{frag}, t{const char *}v{expected}
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
	f{serializeFrag}(v{frag}, (void *) buffer, k{false});
	f{ASSERT}(f{strcmp}(
		v{expected}, v{buffer}
	) == 0);
	fragTestBufferCur = NULL;
	fragTestBufferEnd = NULL;
x{serialize test frag}
```
* Serialisiert das Fragment
* Der Buffer muss die erwarteten Werte enthalten

```
a{define frag}
	t{void} f{addStringToFrag}(
		t{struct Frag *}v{frag},
		t{const char *}v{str}
	) {
		t{int} v{size} = f{strlen}(v{str});
		f{addBytesToFrag}(
			v{frag}, v{str}, v{str} + v{size},
			k{NULL}, n{0}
		);
	}
x{define frag}
```
* Zu Testzwecken kann eine Null-terminierte Zeichenkette hinzugefügt
  werden

```
a{frag unit tests}
	{
		t{struct Frag *}v{frag} =
			f{allocTestFrag}(s{""});
		f{addStringToFrag}(v{frag}, s{"abc"});
		f{addStringToFrag}(v{frag}, s{"def"});
		f{testFrag}(v{frag}, s{"abcdef"});
		f{freeFrag}(v{frag});
	}
x{frag unit tests}
```
* Prüft, ob zwei Strings richtig serialisiert werden

```
a{frag unit tests} {
	t{struct Frag *}v{a} =
		f{allocTestFrag}(s{""});
	t{struct Frag *}v{b} =
		f{allocTestFrag}(s{""});
	f{addStringToFrag}(v{a}, s{"abc"});
	f{addFragToFrag}(v{b}, v{a});
	f{addStringToFrag}(v{b}, s{"def"});
	f{addFragToFrag}(v{b}, v{a});
	f{testFrag}(v{b}, s{"abcdefabc"});
	f{freeFrag}(v{a}); f{freeFrag}(v{b});
} x{frag unit tests}
```
* Prüft, ob Fragmente expandiert werden

# Zykel im Fragment-Graph finden

```
a{includes}
	#include <stdbool.h>
x{includes}
```
* Boolesche Datenwerte werden benötigt

```
d{define cycle check}
	t{bool} f{isFragInFrag}(
		t{struct Frag *}v{needle},
		t{struct Frag *}v{haystack}
	) {
		f{ASSERT}(v{needle});
		f{ASSERT}(v{haystack});
		e{check cycle frag};
		e{check cycle entries};
		k{return} k{false};
	}
x{define cycle check}
```
* Wenn das Fragment das gesuchte ist, dann wurde ein Zykel gefunden
* Danach wird über alle Einträge gesucht
* Wenn das Fragment dort nicht gefunden wurde, dann ist es nicht
  enthalten

```
d{avoid frag cycles}
	f{ASSERT}(! f{isFragInFrag}(
		v{frag}, v{child}
	));
x{avoid frag cycles}
```
* Ein Fragment darf nur hinzugefügt werden, wenn es den Container nicht
  bereits enthält

```
d{check cycle frag}
	k{if} (v{needle} == v{haystack}) {
		k{return} k{true};
	}
x{check cycle frag}
```
* Wenn der Container selbst das gesuchte Fragment ist, liefert die
  Funktion `k{true}` zurück

```
d{check cycle entries}
	t{struct FragEntry *}v{entry} =
		v{haystack}->v{firstEntry};
	k{for} (; v{entry}; v{entry} = v{entry}->v{link}) {
		k{if} (! v{entry}->v{frag}) { k{continue}; }
		k{if} (f{isFragInFrag}(
			v{needle}, v{entry}->v{frag}
		)) {
			k{return} k{true};
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

	t{struct FragMap} {
		t{struct Frag *}v{frags}t{[}
			t{FRAG_SLOTS}
		t{]};
	};
x{define frag}
```
* Eine Kollektion von Fragmenten ist ein Array von Fragment-Ketten
* Alle Felder müssen mit `k{NULL}` initialisiert werden

```
a{define frag}
	t{void} f{clearFragMap}(
		t{struct FragMap *}v{map}
	) {
		t{struct Frag **}v{cur} = v{map}->v{frags};
		t{struct Frag **}v{end} =
			v{cur} + v{FRAG_SLOTS};
		k{for} (; v{cur} < v{end}; ++v{cur}) {
			f{freeFrag}(*v{cur}); *v{cur} = k{NULL};
		}
	}
x{define frag}
```
* Um den Speicher freizugeben, wird jeder Slot gelöscht
* und auf <code class="keyword">NULL</code> gesetzt um wieder verwendet

```
a{define frag}
	t{int} f{calcHash}(t{const char *}v{begin},
		t{const char *}v{end}) {
		f{ASSERT}(v{begin});
		t{unsigned} v{hash} = n{0xf1e2d3c4};
		k{while} (*v{begin} && v{begin} != v{end}) {
			v{hash} ^= *v{begin}++;
			v{hash} = (v{hash} << 3) |
				(v{hash} >> 29);
		}
		k{return} v{hash} % v{FRAG_SLOTS};
	}
x{define frag}
```
* Der Hash wird über den Namen des Fragments erstellt
* Der Name kann entweder durch ein Null-Byte abgeschlossen werden
* Oder ein Ende wird direkt angegeben

```
a{define frag}
	t{struct Frag *}f{allocFragInMap}(
		t{struct FragMap *}v{map},
		t{const char *}v{begin},
		t{const char *}v{end}
	) {
		f{ASSERT}(v{map});
		t{struct Frag *}v{frag} =
			f{allocFrag}(v{begin}, v{end});
		e{insert in slot};
		k{return} v{frag};
	}
x{define frag}
```
* Ein neues Fragment wird erstellt
* Und in der Hash-Map abgelegt

```
d{insert in slot}
	t{int} v{hash} = f{calcHash}(v{begin}, v{end});
	v{frag}->v{link} = v{map}->v{frags}[v{hash}];
	v{map}->v{frags}[v{hash}] = v{frag};
x{insert in slot}
```
* Fragment wird im Slot eingefügt
* Neue Fragmente überlagern alte Fragmente mit gleichem Namen

```
a{define frag}
	t{struct Frag *}f{findFragInMap}(
		t{struct FragMap *}v{map},
		t{const char *}v{begin},
		t{const char *}v{end}
	) {
		f{ASSERT}(v{map});
		t{struct Frag *}v{frag} = f{NULL};
		e{find frag in slot};
		k{return} v{frag};
	}
x{define frag}
```
* Liefert das erste Fragment mit dem übergebenen Namen

```
d{find frag in slot}
	t{int} v{hash} = f{calcHash}(v{begin}, v{end});
	v{frag} = v{map}->v{frags}[v{hash}];
	k{for} (; v{frag}; v{frag} = v{frag}->v{link}) {
		t{const char *}v{a} = v{begin};
		t{const char *}v{b} = v{frag}->v{name};
		k{while} (v{a} != v{end}) {
			k{if} (*v{a}++ != *v{b}++) { k{break}; }
		}
		k{if} (v{a} == v{end} && ! *v{b}) {
			k{return} v{frag}; }
	}
x{find frag in slot}
```
* Im passenden Hash-Slot werden die Namen der Fragmente verglichen

```
a{define frag}
	t{struct Frag *}f{getFragInMap}(
		t{struct FragMap *}v{map},
		t{const char *}v{begin},
		t{const char *}v{end}
	) {
		t{struct Frag *}v{frag} = NULL;
		e{get frag find};
		e{get frag alloc};
		k{return} v{frag};
	}
x{define frag}
```
* Liefert Fragment mit angegebenen Namen oder legt ein neues Fragment mit
  diesem Namen an

```
d{get frag find}
	v{frag} = f{findFragInMap}(
		v{map}, v{begin}, v{end}
	);
	k{if} (v{frag}) {
		k{return} v{frag};
	}
x{get frag find}
```
* Wenn das Fragment in der Kollektion vorhanden ist, wird dieses
  verwendet

```
d{get frag alloc}
	v{frag} = f{allocFragInMap}(
		v{map}, v{begin}, v{end}
	);
x{get frag alloc}
```
* Sonst wird ein neues Fragment angelegt

# Position im Original merken

```
d{additional entry attributes}
	struct Input *input;
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
	v{entry}->v{input} = v{input};
	v{entry}->v{line} = v{line};
x{populate additional entry fields}
```
