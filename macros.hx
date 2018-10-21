# Makros

```
a{global elements}
	e{define macro};
x{global elements}
```
* Makros sind global sichtbare Strukturen

```
a{define macro}
	t{struct MacroEntry};

	t{struct Macro} {
		t{struct Macro *}v{link};
		t{struct MacroEntry *}v{firstEntry};
		t{struct MacroEntry *}v{lastEntry};
		t{char }v{name}t{[]};
	};
x{define macro}
```
* Die Makros werden in einfach verketteten Listen gesammelt
* Genauso werden die Einträge eines Makros in einer einfach verketteten
  Liste organisiert
* Um schnell Einträge einfügen zu können, gibt es auch einen Verweis auf
  das letzte Element
* Je nach Namen werden für ein Makro unterschiedlich viele Bytes im Heap
  angefordert

```
a{includes}
	#include <stdlib.h>
x{includes}
```
* <code class="include-name">stdlib.h</code> wird für die Definition von
  <code class="keyword">NULL</code> benötigt

# Neues Makro anlegen

```
a{define macro}
	t{struct Macro *}f{allocMacro}(
		t{const char *}v{nameBegin},
		t{const char *}v{nameEnd}
	) {
		t{struct Macro *}v{result} = k{NULL};
		e{allocate macro on heap};
		v{result}->v{link} = k{NULL};
		v{result}->v{firstEntry} = k{NULL};
		e{copy macro name};
		k{return} v{result};
	}
x{define macro}
```
* Die Zeiger werden mit <code class="keyword">NULL</code> initializiert
* <code class="var">lastEntry</code> wird erst initialisiert, wenn
  <code class="var">firstEntry</code> gesetzt wird
* Der Name wird über zwei Zeiger übergeben, muss also nicht mit einem
  Null-Byte terminiert sein

```
a{define logging}
	#define ASSERT(v{COND}) \
		k{if} (! (v{COND})) { \
			f{fprintf}(v{stderr}, \
				s{"%s:%d FAILED\n"}, \
				__FILE__, __LINE__); \
			f{exit}(v{EXIT_FAILURE}); \
		}
x{define logging}
```
* Wenn Bedingung falsch ist, wird Fehlermeldung ausgegeben
* Und das Programm beendet
* Datei und Zeile des Tests wird ausgegeben

```
a{allocate macro on heap}
	f{ASSERT}(v{nameBegin});
	f{ASSERT}(v{nameBegin} <= v{nameEnd});
	t{int} v{nameLength} =
		v{nameEnd} - v{nameBegin};
	t{int} v{macroSize} = k{sizeof}(t{struct Macro})
		+ v{nameLength} + 1;
	v{result} = v{malloc}(v{macroSize});
	f{ASSERT}(v{result});
x{allocate macro on heap}
```
* Die Zeiger werden mit <code class="keyword">NULL</code> initializiert
* <code class="var">lastEntry</code> wird erst initialisiert, wenn
  <code class="var">firstEntry</code> gesetzt wird
* Der Name wird über zwei Zeiger übergeben, muss also nicht mit einem
  Null-Byte terminiert sein

```
a{includes}
	#include <string.h>
x{includes}

a{copy macro name}
	f{memcpy}(
		v{result}->v{name}, v{nameBegin},
		v{nameLength}
	);
	v{result}->v{name}[v{nameLength}] = s{'\0'};
x{copy macro name}
```
* Der Name wird direkt in die das Makro kopiert
* Der Name wird mit einem Null-Byte abgeschlossen

# Makros freigeben

```
a{define macro}
	k{void} f{freeMacro}(
		t{struct Macro *}v{macro}
	) {
		k{while} (v{macro}) {
			t{struct Macro *}v{link} =
				v{macro}->v{link};
			e{free macros entries};
			f{free}(v{macro});
			v{macro} = v{link};
		}
	}
x{define macro}
```
* Mit einem Makro werden auch alle verketteten Makros freigegeben
* Die Freigabe der einzelnen Einträge wird später definiert
		
# Unit Tests

```
a{perform unit tests}
	e{macro unit tests};
x{perform unit tests}

a{define macro}
	t{struct Macro *}f{allocTestMacro}(
		t{const char *}v{name}
	) {
		k{return} f{allocMacro}(
			v{name}, v{name} + f{strlen}(v{name})
		);
	}
x{define macro}
```
* Für Unit-Tests gibt es einen einfacheren Konstruktor
* Die Länge des Namens wird anhand des Null-Bytes berechnet

```
a{define macro}
	k{void} f{testMacroName}(
		t{const char *}v{name}
	) {
		t{struct Macro *}v{macro} =
			f{allocTestMacro}(v{name});
		f{ASSERT}(v{macro});
		f{ASSERT}(
			f{strcmp}(v{macro}->v{name}, v{name}) == 0
		);
		f{freeMacro}(v{macro});
	}
x{define macro}
```
* <code class="fn">testMacroName</code> prüft, ob der Name korrekt in ein
  Makro kopiert wurde

```
a{macro unit tests}
	f{testMacroName}(s{"abc"});
	f{testMacroName}(s{""});
	f{testMacroName}(s{"A c"});
	{
		t{struct Macro *}v{macro} =
			f{allocTestMacro}(s{"ab"});
		f{ASSERT}(v{macro});
		f{ASSERT}(! v{macro}->v{link});
		f{ASSERT}(! v{macro}->v{firstEntry});
		f{freeMacro}(v{macro});
	}
x{macro unit tests}
```
* Zum einen wird getestet, ob die Namen korrekt kopiert werden
* Zum anderen wird sichergestellt, dass die Verweise
  <code class="keyword">NULL</code> sind

# Makro Einträge

```
a{define macro}
	t{struct MacroEntry} {
		t{struct MacroEntry *}v{link};
		t{struct Macro *}v{macro};
		t{const char *}v{valueEnd};
		t{char }v{value}t{[]};
	};
x{define macro}
```
* Ein Eintrag kann entweder auf ein anderes Makro verweisen (wenn dieses
  an der aktuellen Stelle expandiert werden soll)
* Oder er enthält Bytes, die beim Expandieren direkt expandiert werden
* Die Länge des Byte-Arrays wird über einen Zeiger angezeigt (damit auch
  Null-Bytes verwendet werden können)
* Wenn ein Eintrag sowohl Daten als auch ein Makro enthält, so wird
  zuerst der Text ausgegeben
		
# Makro Eintrag anlegen

```
a{define macro}
	t{struct MacroEntry *}f{allocMacroEntry}(
		t{struct Macro *}v{macro},
		t{const char *}v{valueBegin},
		t{const char *}v{valueEnd}
	) {
		t{struct MacroEntry *}v{result} = k{NULL};
		e{allocate entry on heap};
		v{result}->v{link} = k{NULL};
		e{copy entry values};
		k{return} v{result};
	}
x{define macro}
```
* Wie beim Macro werden die <code class="var">link</code> Verweise auf
  <code class="keyword">NULL</code> initialisiert
* Sowohl <code class="var">macro</code> als auch
  <code class="var">valueBegin</code> sind optional
* Die Größe des Eintrags hängt von der Größe der Bytes ab, die kopiert
  werden

```
a{allocate entry on heap}
	t{int} v{valueLength} = 0;
	k{if} (v{valueBegin}) {
		f{ASSERT}(v{valueBegin} <= v{valueEnd});
		v{valueLength} =
			v{valueEnd} - v{valueBegin};
	}
	t{int} v{entrySize} = v{valueLength} +
		k{sizeof}(t{struct MacroEntry});
	v{result} = v{malloc}(v{entrySize});
	f{ASSERT}(v{result});
x{allocate entry on heap}
```
* Die Größe der Struktur wird um die Anzahl der zu kopierenden Bytes
  vergrößert
* Wenn nicht genug Speicher vorhanden ist, dann wird das Programm beendet

```
a{copy entry values}
	k{if} (v{valueBegin}) {
		f{memcpy}(
			v{result}->v{value}, v{valueBegin},
			v{valueLength}
		);
	}
	v{result}->v{valueEnd} =
		v{result}->v{value} + v{valueLength};
	v{result}->v{macro} = v{macro};
x{copy entry values}
```
* Die Bytes werden nur kopiert, wenn welche übergeben wurden

```
a{define macro}
	t{struct MacroEntry *}
	f{allocEmptyMacroEntry}() {
		k{return} f{allocMacroEntry}(
			k{NULL}, k{NULL}, k{NULL}
		);
	}
x{define macro}
```
* Für Tests ist es praktisch, leere Einträge anzulegen

# Makro Einträge freigeben

```
a{define macro}
	k{void} f{freeMacroEntry}(
		t{struct MacroEntry *}v{entry}
	) {
		k{while} (v{entry}) {
			t{struct MacroEntry *}v{link} =
				v{entry}->v{link};
			f{free}(v{entry});
			v{entry} = v{link};
		}
	}
x{define macro}
```
* Wenn ein Eintrag freigegeben wird, so werden auch alle verlinkten
  Einträge freigegeben
* Referenzierte Makros werden nicht mit freigegeben

```
a{forward declarations}
	t{struct MacroEntry};
	k{void} f{freeMacroEntry}(
		t{struct MacroEntry *}v{entry}
	);
x{forward declarations}

a{free macros entries}
	f{freeMacroEntry}(v{macro}->v{firstEntry});
x{free macros entries}
```
* Wenn ein Makro freigegeben wird, so werden auch die anhängenden
  Einträge freigegeben
* Damit die Funktion im <code class="fn">freeMacro</code> sichtbar ist,
  wird sie in der Include-Sektion definiert

# Auf Attribute zugreifen

```
a{define macro}
	k{int} f{getMacroEntryValueSize}(
		t{struct MacroEntry *}v{entry}
	) {
		k{if} (! v{entry}) {
			k{return} n{0};
		}
		k{return} v{entry}->v{valueEnd} -
			v{entry}->v{value};
	}
x{define macro}
```
* Liefert die Anzahl der enthaltenen Bytes

# Unit Tests

```
a{macro unit tests}
	{
		t{struct MacroEntry *}v{entry} =
			f{allocEmptyMacroEntry}();

		f{ASSERT}(v{entry});
		f{ASSERT}(! v{entry}->v{link});
		f{ASSERT}(! v{entry}->v{macro});

		f{freeMacroEntry}(v{entry});
	}
x{macro unit tests}
```
* Ein leerer Eintrag hat keinen Nachfolger
* Und kein Makro

```
a{macro unit tests}
	{
		t{struct MacroEntry *}v{entry} =
			f{allocEmptyMacroEntry}();

		f{ASSERT}(v{entry});
		f{ASSERT}(
			f{getMacroEntryValueSize}(
				v{entry}) == n{0);}

		f{freeMacroEntry}(v{entry});
	}
x{macro unit tests}
```
* Ein leerer Eintrag hat keine Bytes

```
a{define macro}
	t{struct MacroEntry *}
	f{allocTestMacroEntry}(
		t{const char *}v{value}
	) {
		t{const char *}v{end} = v{value} +
			f{strlen}(v{value});

		k{return} f{allocMacroEntry}(
			k{NULL}, v{value}, v{end}
		);
	}
x{define macro}
```
* Der Test-Konstruktor bekommt eine Null-terminierte Zeichenkette als
  Argument

```
a{macro unit tests}
	{
		t{struct MacroEntry *}v{entry} =
			f{allocTestMacroEntry}(s{"abc"});

		f{ASSERT}(v{entry});
		f{ASSERT}(
			f{getMacroEntryValueSize}(
				v{entry}) == n{3);}

		f{freeMacroEntry}(v{entry});
	}
x{macro unit tests}
```
* Ein Eintrag hat die korrekte Anzahl an Bytes

```
a{macro unit tests}
	{
		t{struct MacroEntry *}v{entry} =
			f{allocTestMacroEntry}(s{"abc"});

		f{ASSERT}(v{entry});
		f{ASSERT}(
			f{memcmp}(v{entry}->v{value},
				s{"abc"}, n{3}) == n{0);}

		f{freeMacroEntry}(v{entry});
	}
x{macro unit tests}
```
* Die Bytes eines Eintrags stimmen überein

# Einträge zu Makros hinzufügen

```
a{define macro}
	t{void} f{addEntryToMacro}(
		t{struct Macro *}v{macro},
		t{struct MacroEntry *}v{entry}
	) {
		e{assert add entry};
		k{if} (v{macro}->v{firstEntry}) {
			e{append entry};
		} k{else} {
			e{set first entry};
		}
	}
x{define macro}
```
* Ein Eintrag wird entweder an das Ende der Liste der Einträge angehängt
* Oder als neuer Kopf der Liste gesetzt

```
a{assert add entry}
	f{ASSERT}(v{macro});
	f{ASSERT}(v{entry});
	f{ASSERT}(! v{entry}->v{link});
x{assert add entry}
```
* Makro darf nicht <code class="keyword">NULL</code> sein
* Eintrag darf nicht <code class="keyword">NULL</code> sein
* Eintrag darf noch nicht in einer anderen Liste hängen

```
a{append entry}
	v{macro}->v{lastEntry}->v{link} = v{entry};
	v{macro}->v{lastEntry} = v{entry};
x{append entry}
```
* Da es schon Einträge gibt, muss es bereits einen letzten geben
* Dessen neuer Nachfolger ist der neue Eintrag
* Der neue Eintrag wird zum neuen letzten Eintrag
* Der Nachfolger von <code class="var">entry</code> ist bereits
  <code class="keyword">NULL</code>

```
a{set first entry}
	v{macro}->v{firstEntry} = v{entry};
	v{macro}->v{lastEntry} = v{entry};
x{set first entry}
```
* Der erste Eintrag wird auch als letzter Eintrag gesetzt
* Der Nachfolger von <code class="var">entry</code> ist bereits
* <code class="keyword">NULL</code>

```
a{macro unit tests}
	{
		t{struct Macro *}v{macro} =
			f{allocTestMacro}(s{""});
		t{struct MacroEntry *}v{entry} =
			f{allocEmptyMacroEntry}();
		f{addEntryToMacro}(v{macro}, v{entry});
		f{ASSERT}(
			v{macro}->v{firstEntry} == v{entry}
		);
		f{freeMacro}(v{macro});
	}
x{macro unit tests}
```
* Der erste Eintrag im Makro wird gesetzt

```
a{macro unit tests}
	{
		t{struct Macro *}v{macro} =
			f{allocTestMacro}(s{""});
		t{struct MacroEntry *}v{entry} =
			f{allocEmptyMacroEntry}();
		f{addEntryToMacro}(v{macro}, v{entry});
		f{ASSERT}(
			v{macro}->v{lastEntry} == v{entry}
		);
		f{freeMacro}(v{macro});
	}
x{macro unit tests}
```
* Der letzte Eintrag im Makro wird gesetzt

```
a{macro unit tests}
	{
		t{struct Macro *}v{macro} = k{NULL};
		t{struct MacroEntry *}v{first};
		t{struct MacroEntry *}v{second};
		e{add two entries};
		e{check first of 2};
		f{freeMacro}(v{macro});
	}
x{macro unit tests}
```
* Zwei Einträge werden an ein Makro angehängt
* Der erste Eintrag muss gesetzt bleiben

```
a{add two entries}
	v{macro} = f{allocTestMacro}(s{""});
	v{first} = f{allocEmptyMacroEntry}();
	v{second} = f{allocEmptyMacroEntry}();

	f{addEntryToMacro}(v{macro}, v{first});
	f{addEntryToMacro}(v{macro}, v{second});
x{add two entries}

a{check first of 2}
	f{ASSERT}(v{macro}->v{firstEntry} == v{first});
x{check first of 2}
```
* Zwei Einträge werden an ein Makro angehängt
* Der erste Eintrag muss gesetzt bleiben

```
a{macro unit tests}
	{
		t{struct Macro *}v{macro} = k{NULL};
		t{struct MacroEntry *}v{first};
		t{struct MacroEntry *}v{second};
		e{add two entries};
		f{ASSERT}(
			v{macro}->v{lastEntry} == v{second}
		);
		f{freeMacro}(v{macro});
	}
x{macro unit tests}
```
* Zwei Einträge werden an ein Makro angehängt
* Der letzte Eintrag muss gesetzt werden

```
a{define macro}
	t{void} f{addBytesToMacro}(
		t{struct Macro *}v{macro},
		t{const char *}v{value},
		t{const char *}v{valueEnd}
	) {
		t{struct MacroEntry *}v{entry} =
			f{allocMacroEntry}(
				k{NULL}, v{value}, v{valueEnd}
			);
		f{addEntryToMacro}(v{macro}, v{entry});
	}
x{define macro}
```
* Für die Daten wird ein neuer Eintrag angelegt
* Dieser wird an das Makro angehängt

```
a{define macro}
	e{define cycle check}
	t{void} f{addMacroToMacro}(
		t{struct Macro *}v{macro},
		t{struct Macro *}v{child}
	) {
		f{ASSERT}(v{macro});
		f{ASSERT}(v{child});
		e{avoid macro cycles};
		e{reuse last entry};
		e{add macro entry};
	}
x{define macro}
```
* Bevor ein Makro hinzugefügt werden kann, muss sichergestellt werden,
  dass kein Zykel entsteht
* Ein Zykel liegt vor, wenn <code class="var">macro</code> gleich
  <code class="var">child</code> ist
* Oder bereits direkt oder indirekt zu <code class="var">child</code>
  hinzugefügt wurde
* Falls der letzte Eintrag noch kein Makro hat, wird dieser Eintrag
  verwendet

```
a{reuse last entry}
	k{if} (v{macro}->v{firstEntry} &&amp;
		! v{macro}->v{lastEntry}->v{macro}
	) {
		v{macro}->v{lastEntry}->v{macro} = v{child};
		k{return};
	}
x{reuse last entry}
```
* Wenn das Makro-Attribut im letzten Eintrag noch nicht benutzt wird,
  kann dieses verwendet werden

```
a{add macro entry}
	t{struct MacroEntry *}v{entry} =
		f{allocMacroEntry}(
			v{child}, k{NULL}, k{NULL}
		);
	f{addEntryToMacro}(v{macro}, v{entry});
x{add macro entry}
```
* Sonst muss ein neuer Eintrag mit dem Makro angelegt werden

# Makros serialisieren

```
a{define macro}
	t{void} f{serializeMacro}(
		t{struct Macro *}v{macro,}
		t{struct Consumer *}v{consumer}
	) {
		f{ASSERT}(v{macro});
		f{ASSERT}(v{consumer});
		e{iterate entries};
	}
x{define macro}
```
* Jeder Eintrag wird nacheinander bearbeitet
* Makros in Einträgen werden rekursiv ausgegeben

```
a{iterate entries}
	t{struct MacroEntry *}v{entry} =
		v{macro}->v{firstEntry};
	k{for} (; v{entry}; v{entry} = v{entry}->v{link}) {
		e{serialize bytes};
		k{if} (v{entry}->v{macro}) {
			f{serializeMacro}(
				v{entry}->v{macro}, v{consumer}
			);
		}
	}
x{iterate entries}
```
* Für jeden Eintrag werden zuerst die Bytes ausgegeben
* Dann wird rekursiv das Makro ausgegeben, falls vorhanden

```
a{serialize bytes}
	k{if} (f{getMacroEntryValueSize}(v{entry})) {
		t{const char *}v{cur} = v{entry}->v{value};
		t{const char *}v{end} = v{entry}->v{valueEnd};
		k{for} (; v{cur} < v{end}; ++v{cur}) {
			f{putToConsumer}(v{consumer}, *v{cur});
		}
	}
x{serialize bytes}
```
* Wenn es Bytes in dem Eintrag gibt, wird der
  <code class="var">consumer</code> mit ihnen aufgerufen

```
a{define macro}
	t{void} f{testMacro}(t{struct Macro *}
		v{macro}, t{const char *}v{expected}
	) {
		t{struct BufferConsumer} v{bc};
		f{setupBufferConsumer}(&v{bc});
		e{serialize test macro};
		f{eraseBuffer}(&v{bc}.v{buffer});
	}
x{define macro}
```
* Diese Hilfsfunktion prüft ob die Serialisierung eines Makros der
  Erwartung entspricht

```
a{serialize test macro}
	f{serializeMacro}(v{macro}, &v{bc}.v{consumer});
	f{putToConsumer}(&v{bc}.v{consumer}, k{EOF});
	f{ASSERT}(f{strcmp}(
		v{expected}, v{bc}.v{buffer}.v{buffer}
	) == 0);
x{serialize test macro}
```
* Serialisiert das Makro
* Der Buffer muss die erwarteten Werte enthalten

```
a{define macro}
	t{void} f{addStringToMacro}(
		t{struct Macro *}v{macro},
		t{const char *}v{str}
	) {
		t{int} v{size} = f{strlen}(v{str});
		f{addBytesToMacro}(
			v{macro}, v{str}, v{str} + v{size}
		);
	}
x{define macro}
```
* Zu Testzwecken kann eine Null-terminierte Zeichenkette hinzugefügt
  werden

```
a{macro unit tests}
	{
		t{struct Macro *}v{macro} =
			f{allocTestMacro}(s{""});
		f{addStringToMacro}(v{macro}, s{"abc"});
		f{addStringToMacro}(v{macro}, s{"def"});
		f{testMacro}(v{macro}, s{"abcdef"});
		f{freeMacro}(v{macro});
	}
x{macro unit tests}
```
* Prüft, ob zwei Strings richtig serialisiert werden

```
a{macro unit tests} {
	t{struct Macro *}v{a} =
		f{allocTestMacro}(s{""});
	t{struct Macro *}v{b} =
		f{allocTestMacro}(s{""});
	f{addStringToMacro}(v{a}, s{"abc"});
	f{addMacroToMacro}(v{b}, v{a});
	f{addStringToMacro}(v{b}, s{"def"});
	f{addMacroToMacro}(v{b}, v{a});
	f{testMacro}(v{b}, s{"abcdefabc"});
	f{freeMacro}(v{a}); f{freeMacro}(v{b});
} x{macro unit tests}
```
* Prüft, ob Makros expandiert werden

# Zykel im Makro-Graph finden

```
a{includes}
	#include <stdbool.h>
x{includes}
```
* Boolesche Datenwerte werden benötigt

```
a{define cycle check}
	t{bool} f{isMacroInMacro}(
		t{struct Macro *}v{needle},
		t{struct Macro *}v{haystack}
	) {
		f{ASSERT}(v{needle});
		f{ASSERT}(v{haystack});
		e{check cycle macro};
		e{check cycle entries};
		k{return} k{false};
	}
x{define cycle check}
```
* Wenn das Makro das gesuchte ist, dann wurde ein Zykel gefunden
* Danach wird über alle Einträge gesucht
* Wenn das Makro dort nicht gefunden wurde, dann ist es nicht enthalten

```
a{avoid macro cycles}
	f{ASSERT}(! f{isMacroInMacro}(
		v{macro}, v{child}
	));
x{avoid macro cycles}
```
* Ein Makro darf nur hinzugefügt werden, wenn es den Container nicht
  bereits enthält

```
a{check cycle macro}
	k{if} (v{needle} == v{haystack}) {
		k{return} k{true};
	}
x{check cycle macro}
```
* Wenn der Container selbst das gesuchte Makro ist, liefert die Funktion
  <code class="keyword">true</code> zurück

```
a{check cycle entries}
	t{struct MacroEntry *}v{entry} =
		v{haystack}->v{firstEntry};
	k{for} (; v{entry}; v{entry} = v{entry}->v{link}) {
		k{if} (! v{entry}->v{macro}) { k{continue}; }
		k{if} (f{isMacroInMacro}(
			v{needle}, v{entry}->v{macro}
		)) {
			k{return} k{true};
		}
	}
x{check cycle entries}
```
* Alle Makros in den Einträgen werden rekursiv untersucht
* Damit wird der ganze Graph durchsucht

# Makro-Kollektion

```
a{define macro}
	#define MACRO_SLOTS 128

	t{struct MacroMap} {
		t{struct Macro *}v{macros}t{[}
			t{MACRO_SLOTS}
		t{]};
	};
x{define macro}
```
* Eine Kollektion von Makros ist ein Array von Makro-Ketten
* Alle Felder müssen mit <code class="keyword">NULL</code> initialisiert
  werden

```
a{define macro}
	t{void} f{clearMacroMap}(
		t{struct MacroMap *}v{map}
	) {
		t{struct Macro **}v{cur} = v{map}->v{macros};
		t{struct Macro **}v{end} =
			v{cur} + v{MACRO_SLOTS};
		k{for} (; v{cur} < v{end}; ++v{cur}) {
			f{freeMacro}(*v{cur}); *v{cur} = k{NULL};
		}
	}
x{define macro}
```
* Um den Speicher freizugeben, wird jeder Slot gelöscht
* und auf <code class="keyword">NULL</code> gesetzt um wieder verwendet
  zu werden

```
a{define macro}
	t{int} f{calcHash}(t{const char *}v{begin},
		t{const char *}v{end}) {
		f{ASSERT}(v{begin});
		t{unsigned} v{hash} = n{0xf1e2d3c4};
		k{while} (*v{begin} &&amp; v{begin} != v{end}) {
			v{hash} ^= *v{begin}++;
			v{hash} = (v{hash} << 3) |
				(v{hash} >> 29);
		}
		k{return} v{hash} % v{MACRO_SLOTS};
	}
x{define macro}
```
* Der Hash wird über den Namen des Makros erstellt
* Der Name kann entweder durch ein Null-Byte abgeschlossen werden
* Oder ein Ende wird direkt angegeben

```
a{define macro}
	t{struct Macro *}f{allocMacroInMap}(
		t{struct MacroMap *}v{map},
		t{const char *}v{begin},
		t{const char *}v{end}
	) {
		f{ASSERT}(v{map});
		t{struct Macro *}v{macro} =
			f{allocMacro}(v{begin}, v{end});
		e{insert in slot};
		k{return} v{macro};
	}
x{define macro}
```
* Ein neues Makro wird erstellt
* Und in der Hash-Map abgelegt

```
a{insert in slot}
	t{int} v{hash} = f{calcHash}(v{begin}, v{end});
	v{macro}->v{link} = v{map}->v{macros}[v{hash}];
	v{map}->v{macros}[v{hash}] = v{macro};
x{insert in slot}
```
* Makros werden im Slot eingefügt
* Neue Makros überlagern alte Slots mit gleichem Namen

```
a{define macro}
	t{struct Macro *}f{findMacroInMap}(
		t{struct MacroMap *}v{map},
		t{const char *}v{begin},
		t{const char *}v{end}
	) {
		f{ASSERT}(v{map});
		t{struct Macro *}v{macro} = f{NULL};
		e{find macro in slot};
		k{return} v{macro};
	}
x{define macro}
```
* Liefert das erste Makro mit dem übergebenen Namen

```
a{find macro in slot}
	t{int} v{hash} = f{calcHash}(v{begin}, v{end});
	v{macro} = v{map}->v{macros}[v{hash}];
	k{for} (; v{macro}; v{macro} = v{macro}->v{link}) {
		t{const char *}v{a} = v{begin};
		t{const char *}v{b} = v{macro}->v{name};
		k{while} (v{a} != v{end}) {
			k{if} (*v{a}++ != *v{b}++) { k{break}; }
		}
		k{if} (v{a} == v{end} &&amp; ! *v{b}) {
			k{return} v{macro}; }
	}
x{find macro in slot}
```
* Im passenden Hash-Slot werden die Namen der Makros verglichen

```
a{define macro}
	t{struct Macro *}f{getMacroInMap}(
		t{struct MacroMap *}v{map},
		t{const char *}v{begin},
		t{const char *}v{end}
	) {
		t{struct Macro *}v{macro} = NULL;
		e{get macro find};
		e{get macro alloc};
		k{return} v{macro};
	}
x{define macro}
```
* Liefert Makro mit angegebenen Namen oder legt ein neues Makro mit
  diesem Namen an

```
a{get macro find}
	v{macro} = f{findMacroInMap}(
		v{map}, v{begin}, v{end}
	);
	k{if} (v{macro}) {
		k{return} v{macro};
	}
x{get macro find}
```
* Wenn das Makro in der Kollektion vorhanden ist, wird dieses verwendet

```
a{get macro alloc}
	v{macro} = f{allocMacroInMap}(
		v{map}, v{begin}, v{end}
	);
x{get macro alloc}
```
* Sonst wird ein neues Makro angelegt
