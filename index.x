# HTML Extractor
* Dieses Dokument ist eine Präsentation, welche die Entwicklung des
  **HTML Extractors** (`hx`) beschreibt
* Es enthält zusätzlich den gesamten Source-Code von `hx`
* Es handelt sich um den Versuch eines neuen Programmier-Konzeptes:
  das **Slideware-Programming** (SWP).
* Viel Spass.

## Funktionsweise von `hx`
* `hx` generiert Source-Code und HTML-Präsentationen aus einem
  Basis-Format
* Dieses lehnt an Markdown an und hat die Datei-Endung `.x`
* Die Präsentationen bauen Schritt für Schritt das Programm auf
* `hx` kann ebenfalls navigierbare Verweise in die Präsentation einbauen

## Slideware Programming (SWP)
* Aus dem Source-Code, der vollständing in einer HTML-Präsentation
  enthalten ist, kann ein ausführbares Programm generiert werden
* Dabei wird das Programm schrittweise aufgebaut

## SWP ≠ Literate Programming
* SWP beschreibt nicht nur ein fertiges Programm
* Sondern wie das Programm aufgebaut wird
* Zu jedem Zeitpunkt muss das bisher beschriebene Programm ausführbar
  sein
* Nicht definierte Fragmente werden zu nichts expandieren
* So kann das Verständnis für ein Programm schrittweise erarbeitet
  werden

# Definition des Ablaufs
* Zuerst wird das Haupt-Programm in ganz groben Pinselstrichen skizziert
* Nach und nach werden die einzelnen Elemente mit Leben gefüllt

```
D{file: hx.c}
	g{global elements}
	t{int} main(
		t{int} argc, t{const char **}argv
	) {
		e{main body}
	}
x{file: hx.c}
```
* Das Hauptprogramm besteht aus der `main`-Funktion
* Zusätzlich wird ein Fragment definiert, in welchem globale Elemente
  definiert werden können

```
d{main body}
	g{perform unit-tests};
	e{process arguments};
	e{read source file};
	e{serialize fragments};
	g{write HTML file};
x{main body}
```
* Bei jedem Start werden alle Unit-Tests ausgeführt (um eine
  umfangreiche Testabdeckung zu sichern)
* Parameter von der Kommandozeile werden ausgewertet
* Dann wird ein Parse-Graph aus Fragmenten aufgebaut
* Und das daraus resultierende Programm generiert und übersetzt
* Zum Schluss wird die HTML-Präsentation der Seiten in einem zweiten
  Durchgang herausgeschrieben

## Was macht `@expand`?
* `@expand`-Blöcke beschreiben Fragment-Aufrufe
* Der Wert des Fragments mit dem Namen in Klammern wird anstelle des
  Aufrufs im endgültigen Programm gesetzt
* Diese Fragmente bilden ein zentrales Element von `hx`
* Sie können mit `@def`-`@end`-Sequenzen definiert werden
* Oder mit `@add`-`@end` erweitert werden
* Ein `@expand` darf nur einmal aufgelöst werden
* Für mehrfache Auflösungen muss `@mulitple` verwendet werden

```
D{global elements}
	g{includes};
	g{define logging};
	g{forward declarations};
x{global elements}
```
* System-Dateien werden vor der Definition von Strukturen und Funktionen
  eingebunden
* Auch müssen Macros für das Logging vor den Funktionen definiert
  werden, die sie verwenden


# Minimale Vorbereitung für das Parsen
* In diesem Abschnitt wird die Grundlage gelegt, um Dateien lesen zu
  können
* Während des Lesens kann die aktuelle Datei unterbrochen und zuerst
  eine weitere Datei gelesen werden

```
D{includes}
	#include <stdio.h>
	#include <stdlib.h>
x{includes}
```
* Standard File-Funktionen werden vom Programm benötigt
* Standard Bibliothek wird für dynamische Speicherverwaltung benötigt

# Buffer
* Einfache Implementierung in C eines Byte-Vektors
* Der dynamisch wachsen kann

```
i{buf.x}
```
* Buffer werden in einer eigenen Datei definiert

# Fragmente
* Fragmenten können während des Parsens erweitert, ersetzt und
  angewendet werden
* Ein Haupt-Vorteil von `hx` gegenüber anderen Makro-Präprozessoren ist
  die Möglichkeit, Fragmente vor der Definition zu verwenden
* Und Fragment an mehreren Stellen zu erweitern

```
i{frag.x}
```
* Fragment-Behandlung wird in einer eigenen Datei definiert

```
A{global elements}
	t{struct Input} {
		t{struct Input *}link;
		t{FILE *}file;
		e{additional input elements};
		t{char} name[];
	};

	t{struct Input *}input = n{NULL};
	t{struct Input *}used = n{NULL};
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
	t{struct FragMap} root = {};
	t{struct FragMap *}frags = &root;
x{global elements}
```
* Kollektion mit allen Fragmenten wird für folgende Schritte sichtbar
  angelegt

```
A{global elements}
	t{void} pushPath(t{const char *}path) {
		t{FILE *}f = fopen(path, s{"r"});
		e{check file for path};
		t{int} len = strlen(path) + n{1};
		t{struct Input *}i = malloc(
			sizeof(t{struct Input}) + len
		);
		e{check memory for input};
		i->link = input;
		i->file = f;
		memcpy(i->name, path, len);
		e{init additional input fields};
		input = i;
	}
x{global elements}
```
* Dateien werden über ihren Pfad identifiziert
* Dieser wird als Name gespeichert
* Die Struktur wird dynamisch so groß gewählt, dass der Name hinein
  passt

```
d{check file for path}
	ASSERT(
		f, s{"can't open [%s]"}, path
	);
x{check file for path}
```
* Wenn die Datei nicht geöffnet werden kann, bricht das Programm ab

```
d{check memory for input}
	ASSERT(
		i,
		s{"no memory for input"}
	);
x{check memory for input}
```
* Wenn kein Speicher für die `t{struct Input}` vorhanden ist bricht das
  Programm ab

```
d{init additional input fields}
	if (input) {
		input->frags.link = frags;
		frags = &input->frags;
	}
x{init additional input fields}
```
* Wenn es bereits eine offene Input-Datei gibt, dann wird deren lokale
  Fragmente in den globalen Namensraum aufgenommen

# Kommandozeile
* Die Kommandozeile wird Element für Element abgearbeitet

```
A{global elements}
	t{const char *}stylesheet =
		s{"slides/slides.css"};
x{global elements}
```
* Für die HTML-Ausgabe wird eine Stylesheet-Datei benötigt
* Über die Kommandozeile kann eine alternative Datei angegeben werden

```
d{process arguments}
	t{bool} someFile = n{false};
	for (t{int} i = n{1}; i < argc; ++i) {
		e{process argument};
		ASSERT(
			n{false},
			s{"unknown argument [%s]"},
			argv[i]
		);
	}
x{process arguments}
```
* Die Argumente werden einzeln durchgegangen
* Wenn sie nicht verwendet werden, bricht das Programm ab

```
d{process argument} {
	t{const char} prefix[] = s{"--css="};
	t{int} len = sizeof(prefix) - n{1};
	if (memcmp(argv[i], prefix, len) == n{0}) {
		stylesheet = argv[i] + len;
		continue;
	}
} x{process argument}
```
* Der Pfad zur Stylesheet-Datei kann über die Kommandozeile gesetzt
  werden

```
a{process argument}
	if (! someFile) {
		pushPath(argv[n{1}]);
		someFile = n{true};
		continue;
	}
x{process argument}
```
* Ansonsten wird das Argument als Pfad der `.x`-Datei interpretiert
* Aus dieser werden HTML-Slides und Source-Code generiert
* Es kann nur eine Datei angegeben werden

```
a{process arguments}
	if (! someFile) {
		pushPath(s{"index.x"});
	}
x{process arguments}
```
* Wenn kein Pfad angegeben wurde, wird `index.x` als Vorgabe verwendet

# Nächstes Zeichen
* Die Funktion `nextCh` liest das nächste Zeichen aus der aktuellen
  Datei
* Wenn das Dateiende erreicht ist, wird die nächste Datei aus dem
  Stapel der offenen Dateien geholt
* Erst wenn die letzte Datei fertig gelesen wurde, wird ein `n{EOF}`
  zurück geliefert

```
A{global elements}
	t{int} nextCh() {
		t{int} ch = n{EOF};
		while (input) {
			ch = fgetc(input->file);
			e{preprocess char};
			if (ch != n{EOF}) { break; }
			e{get next input file};
		}
		return ch;
	}
x{global elements}
```
* Wenn kein `n{EOF}` gelesen wurde, dann wird das Zeichen zurück
  geliefert
* Ansonsten wird aus der nächsten Datei ein Zeichen gelesen

```

d{get next input file}
	t{struct Input *}n = input->link;
	fclose(input->file);
	input->link = used;
	used = input;
	input = n;
	t{struct FragMap *}nxt = frags->link;
	frags->link = n{NULL};
	frags = nxt;
x{get next input file}
```
* Die aktuelle Datei wird geschlossen und in die Liste der bereits
  verarbeiteten Dateien eingereiht
* Dann wird der Vorgänger zur aktuellen Datei erklärt
* Der Vorgänger wird aus dem globalen Namensraum wieder entfernt

# Lokale Fragmente

```
d{additional input elements}
	t{struct FragMap} frags;
x{additional input elements}
```
* Jede Source-Datei hat eine eigene Fragment-Map mit lokalen
  Definitionen

```
a{init additional input fields}
	memset(
		&i->frags, n{0},
		sizeof(i->frags)
	);
x{init additional input fields};
```
* Eine Map kann initialisiert werden, indem alle Bytes auf `n{0}`
  gesetzt werden

# Eingabe-Dateien lesen
* In diesem Abschnitt werden die Eingabe-Dateien gelesen, um die
  Fragmente aufzubauen und alle notwendigen Beziehungen zwischen den
  einzelnen Folien zu finden

```
d{read source file}
	e{global source vars};
	{
		e{additional read vars};
		t{int} last = nextCh();
		t{int} ch = last != n{EOF} ? nextCh() : n{EOF};
		while (ch != n{EOF}) {
			e{process current char};
			last = ch; ch = nextCh();
		}
	}
x{read source file}

```
* Neben dem aktuellen Zeichen wird auch das letzte Zeichen aufgehoben
* Dabei kann `hx` auch mit einer leeren Eingabe-Datei umgehen (wenn
  schon das erste Zeichen ein `n{EOF}` ist)

```
d{process current char}
	switch (ch) {
		case '{':
			e{process open brace};
			break;
		case '}': {
			t{bool} processed = n{false};
			e{process close brace};
			break;
		}
		default:
			e{process other char};
	}
x{process current char}
```
* Beim Parsen kommt es nur auf das Öffnen und Schließen von
  Mengenklammern an
* Diese bestimmen den Anfang und das Ende von Befehls-Sequenzen
* Welche die Bearbeitung der sonstigen Zeichen steuern

```
d{additional read vars}
	t{struct Frag *}frag = n{NULL};
	t{struct Buffer} buffer = {};
	t{int} bufferLine = n{0};
x{additional read vars}
```
* Wir unterscheiden, ob wir in einem Code-Block sind, oder außerhalb
* In einem Code sind wir sogar in einem Fragment, dessen Inhalt gerade
  gelesen wird
* Am Anfang sind wir außerhalb eines Code-Blocks
* In einem Code-Block ist `frag` nicht `n{NULL}`

```
a{additional read vars}
	t{char} openCh = s{'\0'};
x{additional read vars}
```
* Das Zeichenvor einer öffnenden Mengenklammer wird in `openCh`
  zwischengespeichert
* Es beschreibt, welcher Befehl ausgeführt werden soll

```
a{additional read vars}
	t{struct Buffer} name = {};
	t{int} nameLine = n{0};
x{additional read vars}
```
* Wenn `name` aktiv ist, dann wird ein Name in Buffer gelesen

```
d{process close brace} {
	if (isActiveBuffer(&name)) {
		addToBuffer(&name, s{'\0'});
		e{process frag name};
		resetBuffer(&name);
		last = ch;
		ch = nextCh();
	}
} x{process close brace}
```
* Bei einer schließenden Mengenklammer wird der Befehls-Name ausgewertet
* Danach wird der Namenszeiger zurückgesetzt

```
d{process other char} {
	if (isActiveBuffer(&name)) {
		addToBuffer(&name, ch);
		break;
	}
} x{process other char}
```
* Wenn ein Name geparst wird, dann der Namensbuffer entsprechend
  erweitert

```
a{process other char} {
	if (frag) {
		if (! isActiveBuffer(&buffer)) {
			bufferLine = input->line;
		}
		addToBuffer(&buffer, last);
	}
} x{process other char}
```
* Wenn es ein aktuelles Fragment gibt, dann müssen sonstige Zeichen dort
  angefügt werden

```
d{process open brace} {
	if (! frag) {
		static t{const char} validst{[]} = s{"aAdDirR"};
		if (strchr(valids, last)) {
			openCh = last;
			activateBuffer(&name);
			break;
		}
	}
} x{process open brace}
```
* Wenn außerhalb eines Fragments die Folge `a`, `{` gelesen wird, dann
  beginnt ein `@add`-Fragment
* Falls ein `i`, `{` gelesen wird, dann muss eine andere Datei mit
  `@include` eingebunden werden
* Es folgt der Name des Fragments oder der Pfad der Datei bis zum
  nächsten `}`

```
d{process frag name}
	if (openCh == s{'d'}) {
		ASSERT(! frag, s{"def in frag"});
		t{struct FragMap *}fm = &input->frags;
		E{check for double def};
		if (! frag) {
			frag = allocFragInMap(
				fm, name.buffer,
				name.current - n{1}
			);
		}
		processed = n{true};
	}
x{process frag name}
```
* Erzeugt ein neues Fragment
* Das Fragment darf nicht mehrfach definiert werden

```
a{process frag name}
	if (openCh == s{'D'}) {
		ASSERT(! frag, s{"def in frag"});
		t{struct FragMap *}fm = frags;
		E{check for double def};
		if (! frag) {
			frag = allocFragInMap(
				&root, name.buffer,
				name.current - n{1}
			);
		}
		processed = n{true};
	}
x{process frag name}
```
* Erzeugt ein neues Fragment im globalen Namensraum
* Das Fragment darf nicht mehrfach definiert werden

```
d{check for double def}
	frag = findFragInMap(
		fm, name.buffer,
		name.current - n{1}
	);
	if (isPopulatedFrag(frag)) {
		printf(
			s{"frag [%s] already defined\n"},
			name.buffer
		);
	}
x{check for double def}
```
* Wenn das Fragment bereits existiert, wird es vielleicht nur verwendet
* Es muss geprüft werden, ob es schon Inhalt hat
* Das wäre dann eine Fehlermeldung wert
* Bricht aber die Abarbeitung nicht ab

```
a{process frag name}
	if (openCh == s{'a'}) {
		ASSERT(! frag, s{"add in frag"});
		t{struct FragMap *}fm = &input->frags;
		t{struct FragMap *}ins = fm;
		frag = findFragInMap(
			fm, name.buffer,
			name.current - n{1}
		);
		E{check for add without def};
		processed = n{true};
	}
x{process frag name}
```
* Bei einem öffnenden Befehl wird das passende Fragment gesucht
* Weitere Bytes können zu diesem Fragment hinzugefügt werden

```
a{process frag name}
	if (openCh == s{'A'}) {
		ASSERT(! frag, s{"add in frag"});
		t{struct FragMap *}fm = frags;
		t{struct FragMap *}ins = &root;
		frag = findFragInMap(
			fm, name.buffer,
			name.current - n{1}
		);
		E{check for add without def};
		processed = n{true};
	}
x{process frag name}
```
* Erweitert ein global definiertes Fragment

```
d{check for add without def}
	if (! isPopulatedFrag(frag)) {
		printf(
			s{"frag [%s] not defined\n"},
			name.buffer
		);
		frag = getFragInMap(
			fm, name.buffer,
			name.current - n{1},
			ins
		);
	}
x{check for add without def}
```
* Das Fragment muss bereits vorhanden und nicht leer sein

```
a{process frag name}
	if (openCh == s{'r'}) {
		ASSERT(! frag, s{"replace in frag"});
		frag = getFragInMap(
			&input->frags, name.buffer,
			name.current - n{1}, &input->frags
		);
		ASSERT(
			frag, s{"frag %s not defined"},
			name.buffer
		);
		freeFragEntries(frag);
		processed = n{true};
	}
x{process frag name}
```
* Bei einem `@replace` wird der Inhalt eines Fragments zurückgesetzt
* Das Fragment muss bereits vorhanden sein

```
a{process frag name}
	if (openCh == s{'R'}) {
		ASSERT(! frag, s{"replace in frag"});
		frag = getFragInMap(
			frags, name.buffer,
			name.current - n{1}, &root
		);
		ASSERT(
			frag, s{"frag %s not defined"},
			name.buffer
		);
		freeFragEntries(frag);
		processed = n{true};
	}
x{process frag name}
```
* Ersetzt ein global definiertes Fragment

```
a{process frag name}
	if (openCh == s{'x'}) {
		ASSERT(frag, s{"end not in frag"});
		e{frag names must match};
		E{flush frag buffer};
		frag = n{NULL};
		processed = n{true};
	}
x{process frag name}
```
* Bei einem schließenden Befehl wird das aktuelle Fragment unterbrochen

```
d{frag names must match}
	ASSERT(
		! strcmp(frag->name, name.buffer),
		s{"closing [%s] != [%s]"},
		name.buffer, frag->name
	);
x{frag names must match}
```
* Wenn der öffnende und schließende Name nicht passt, wird die
  Abarbeitung abgebrochen

```
d{global source vars}
	t{bool} alreadyUsed(t{const char *}name) {
		t{struct Input *}i = input;
		for (; i; i = i->link) {
			if (strcmp(i->name, name) == n{0}) {
				return n{true};
			}
		}
		for (i = used; i; i = i->link) {
			if (strcmp(i->name, name) == n{0}) {
				return n{true};
			}
		}
		return n{false};
	}
x{global source vars}
```
* Prüft ob eine Datei bereits verwendet wurde
* Sowohl die offenen als auch die bereits prozessierten Dateien werden
  durchgegangen
* Dadurch wird bei Einbettungen verhindert, dass eine Datei mehrfach
  verarbeitet wird

```
a{process frag name}
	if (openCh == s{'i'}) {
		ASSERT(! frag, s{"include in frag"});
		if (! alreadyUsed(name.buffer)) {
			pushPath(name.buffer);
		}
		processed = true;
	}
x{process frag name}
```
* Wenn eine Datei eingebunden werden soll, dann wird sie geöffnet und
  auf den Stapel der offenen Dateien gelegt
* Wenn die Datei bereits geöffnet wurde, dann wird sie ignoriert

```
a{process frag name}
	if (openCh == s{'e'}) {
		ASSERT(frag, s{"expand not in frag"});
		E{flush frag buffer};
		t{struct Frag *}sub = getFragInMap(
			&input->frags, name.buffer,
			name.current - n{1}, &input->frags
		);
		E{check frag expand count};
		++sub->expands;
		addFragToFrag(frag, sub);
		processed = true;
	}
x{process frag name}
```
* Bei einem `@expand` wird das Fragment gesucht und eingebunden
* Ggf. wird das Fragment dabei auch erzeugt, um später befüllt zu werden
* Das Attribut `expands` zählt, wie häufig das Fragment expandiert
  wurde

```
a{process frag name}
	if (openCh == s{'g'}) {
		ASSERT(frag, s{"expand not in frag"});
		E{flush frag buffer};
		t{struct Frag *}sub = getFragInMap(
			frags, name.buffer,
			name.current - n{1}, &root
		);
		E{check frag expand count};
		++sub->expands;
		addFragToFrag(frag, sub);
		processed = n{true};
	}
x{process frag name}
```

```
d{check frag expand count}
	if (sub->expands) {
		printf(
			s{"multiple expands of [%s]\n"},
			sub->name
		);
	}
	if (sub->multiples) {
		printf(
			s{"expand after mult of [%s]\n"},
			sub->name
		);
	}
x{check frag expand count}
```
* Wenn das Fragment bereits expandiert wurde, dann wird eine Meldung
  ausgegeben
* Wenn das Fragment bereits im Mehrfach-Modus ausgegeben wurde, wird
  ebenfalls eine Meldung ausgegeben

```
a{process frag name}
	if (openCh == s{'E'}) {
		ASSERT(frag, s{"multiple not in frag"});
		E{flush frag buffer};
		t{struct Frag *}sub =
			getFragInMap(
				&input->frags, name.buffer,
				name.current - n{1}, &input->frags
			);
		E{check for prev expands};
		++sub->multiples;
		addFragToFrag(
			frag, sub);
		processed = n{true};
	}
x{process frag name}
```
* Mit einem `@multiple` Befehl kann ein Fragment an mehreren Stellen
  expandiert werden

```
a{process frag name}
	if (openCh == s{'G'}) {
		ASSERT(frag, s{"multiple not in frag"});
		E{flush frag buffer};
		t{struct Frag *}sub =
			getFragInMap(
				frags, name.buffer,
				name.current - n{1}, &root
			);
		E{check for prev expands};
		++sub->multiples;
		addFragToFrag(
			frag, sub);
		processed = n{true};
	}
x{process frag name}
```

```
d{check for prev expands}
	if (sub->expands) {
		printf(
			s{"multiple after expand "}
				s{"of [%s]\n"},
			sub->name
		);
	}
x{check for prev expands}
```
* Es ist ein Fehler, wenn das Fragment bereits normal `@expand`iert
  wurde

```
a{process frag name}
	if (openCh == s{'p'}) {
		ASSERT(frag, s{"private not in frag"});
		e{process private frag};
		processed = n{true};
	}
x{process frag name}
```
* Private Bezeichner werden durch einen Hash erweitert
* Um sie global unique zu machen

```
d{process private frag}
	t{unsigned} cur = initHash();
	cur = addTerminatedToHash(
		cur, input->name
	);
	cur = addRangeToHash(
		cur, name.buffer, name.current
	);
	cur &= n{0x7fffffff};
x{process private frag}
```
* Der Hash wird aus dem aktuellen Dateinamen
* Und dem aktuellen Bezeichner berechnet
* Zum Schluss wird er auf eine positive Zahl maskiert

```
a{process private frag}
	t{static char} hash[12];
	t{char *}end = hash + sizeof(hash);
	t{char *}head = end;
	*--head = '_';
	for (;;) {
		ASSERT(head > hash);
		*--head = (cur % n{10}) + s{'0'};
		cur /= n{10};
		if (! cur) { break; }
	}
x{process private frag}
```
* Das Textfeld mit dem Hash-Wert wird von hinten aus gefüllt
* Das erleichtert das extrahieren der einzelnen Dezimal-Stellen
* Zusätzlich wird noch ein Unterstrich an den Hash angehängt

```
a{process private frag}
	E{flush frag buffer};
	t{static char} prefix[] = s{"_private_"};
	addBytesToFrag(
		frag, prefix,
		prefix + sizeof(prefix) - n{1},
		input->name, nameLine
	);
	addBytesToFrag(
		frag, head, end,
		input->name, nameLine
	);
	addBytesToFrag(
		frag, name.buffer, name.current - n{1},
		input->name, nameLine
	);
x{process private frag}
```
* Zuerst werden eventuell zwischengespeicherte Zeichen ausgegeben
* Dann kommt der neue Bezeichner
* Dieser besteht aus einem konstanten Präfix
* Dem Hash-Wert
* Und dem alten Bezeichner

```
a{process frag name}
	if (openCh == s{'m'}) {
		ASSERT(frag, "magic not in frag");
		e{process magic frag};
		processed = n{true};
	}
x{process frag name}
```
* Der `@magic`-Befehl erzeugt einen Hash-Wert
* Der sich aus dem Dateinamen und dem Argument des Befehls
  zusammen setzt

```
d{process magic frag}
	t{unsigned} cur = initHash();
	cur = addTerminatedToHash(
		cur, input->name
	);
	cur = addRangeToHash(
		cur, name.buffer, name.current
	);
	cur &= n{0x7fffffff};
x{process magic frag}
```
* Berechnet Hash-Wert

```
a{process magic frag}
	t{static char} magic[12];
	t{char *}end = magic + sizeof(magic);
	t{char *}head = end;
	for (;;) {
		ASSERT(head > magic);
		*--head = (cur % n{10}) + s{'0'};
		cur /= n{10};
		if (! cur) { break; }
	}
	E{flush frag buffer};
	addBytesToFrag(
		frag, head, end,
		input->name, nameLine
	);
x{process magic frag}
```
* Gibt den Hash-Wert aus
* Vorher wird noch eventuell gespeicherte Zeichen ausgegeben

```
d{flush frag buffer}
	if (
		buffer.buffer != buffer.current
	) {
		addBytesToFrag(
			frag, buffer.buffer,
			buffer.current,
			input->name, bufferLine
		);
		resetBuffer(&buffer);
	}
x{flush frag buffer}
```
* Das Fragment fügt alle Bytes im Buffer an ein Fragment an
* Danach wird der Buffer zurück gesetzt

```
a{process open brace} {
	if (frag) {
		t{bool} valid = n{false};
		e{check valid names};
		if (valid) {
			openCh = last;
			nameLine = input->line;
			activateBuffer(&name);
			break;
		}
	}
} x{process open brace}
```
* Prüft, ob ein Befehl innerhalb eines Fragments mit einem gültigen
  Zeichen beginnt
* In diesem Fall wird das Zeichen als Befehls-Opcode gesichert und der
  Buffer aktiviert um alle Zeichen bis zu eine schließenden
  Mengenklammer zu speichern

```
d{check valid names}
	t{static const char} valids[] =
		s{"fvsntkxeEgGpm"};
	if (strchr(valids, last)) {
		valid = n{true};
	}
x{check valid names}
```
* Gültige Kommando-Zeichen sind in einem String abgelegt
* Wenn das Zeichen im String vorkommt, dann ist es gültig

```
a{process frag name}
	if (! processed) {
		ASSERT(
			frag, s{"unknown frag %s"},
			name.buffer
		);
		t{const char *}c = name.buffer;
		for (; c != name.current - n{1}; ++c) {
			if (! isActiveBuffer(&buffer)) {
				bufferLine = input->line;
			}
			addToBuffer(&buffer, *c);
		}
		processed = n{true};
	}
x{process frag name}
```
* Wenn kein bekannter Befehl erkannt wurde, dann ist die
  befehlsähnliche Eingabe Teil des Programms
* Und wird daher in den entsprechenden Buffer kopiert

```
a{process open brace}
	if (frag) {
		if (! isActiveBuffer(&buffer)) {
			bufferLine = input->line;
		}
		addToBuffer(&buffer, last);
	}
x{process open brace}
```
* Wenn wir uns in einem Fragment befinden und bis hier gekommen sind,
  dann wird das Zeichen vor der öffnenden Klammer zum Fragment hinzu
  gefügt

```
a{process close brace}
	if (frag && ! processed) {
		if (! isActiveBuffer(&buffer)) {
			bufferLine = input->line;
		}
		addToBuffer(&buffer, last);
	}
x{process close brace}
```
* Wenn schließende Mengenklammern nicht Teil eines Befehls sind, können
  sie Teil des Programms sein
* Und werden daher zum Buffer direkt hinzugefügt

# Fragmente serialisieren
* Fragmente, die Dateien spezifizieren werden in diese Dateien
  rausgeschrieben

```
d{serialize fragments} {
	t{struct Frag **}cur = root.frags;
	t{struct Frag **}end =
		cur + FRAG_SLOTS;
	for (; cur < end; ++cur) {
		t{struct Frag *}frag = *cur;
		for (; frag; frag = frag->link) {
			E{serialize frag};
		}
	}
} x{serialize fragments}
```
* Fragmente, die mit `file:` beginnen, werden in die entsprechenden
  Dateien rausgeschrieben
* Zusätzlich wird geprüft, ob Fragmente zu selten oder zu oft expandiert
  wurden

```
a{serialize fragments} {
	input = used;
	for (; input; input = input->link)
	{
		t{struct Frag **}cur =
			input->frags.frags;
		t{struct Frag **}end =
			cur + FRAG_SLOTS;
		for (; cur < end; ++cur) {
			t{struct Frag *}frag = *cur;
			while (frag) {
				E{serialize frag};
				frag = frag->link;
			}
		}
	}
} x{serialize fragments}
```
* Auch alle lokalen Fragmente bearbeiten

```
d{serialize frag}
	if (! memcmp(
		s{"file: "}, frag->name, n{6}
	)) {
		++frag->expands;
		e{write in file};
	}
x{serialize frag}
```
* Wenn der Name eines Fragments mit `file: ` beginnt, dann wird es in die
  passende Datei geschrieben
* Zusätzlich zählt das als eine Expansion

```
a{serialize frag} {
	t{int} sum =
		frag->expands + frag->multiples;
	if (sum <= n{0}) {
		printf(
			s{"frag [%s] not called\n"},
			frag->name
		);
	}
} x{serialize frag}
```
* Ein Fragment wurde nicht aufgerufen
* Dies wird mit einer Meldung quittiert

```
a{serialize frag}
	if (frag->multiples == n{1}) {
		printf(
			s{"multiple frag [%s] only "}
				s{"used once\n"},
			frag->name
		);
	}
x{serialize frag}
```
* Ein Fragment das zur mehrfachen Verwendung deklariert wurde, wird nur
  einmal verwendet
* Dies wird mit einer Meldung quittiert

```
a{serialize frag}
	if (! isPopulatedFrag(frag)) {
		printf(
			s{"frag [%s] not populated\n"},
			frag->name
		);
	}
x{serialize frag}
```
* Für jedes Fragment, das nicht befüllt wurde wird eine Meldung
  ausgegeben

```
d{write in file}
	t{FILE *}f =
		fopen(frag->name + n{6}, s{"w"});
	ASSERT(
		f, s{"can't open %s"},
		frag->name + n{6}
	);
	serializeFrag(frag, f, n{false});
	fclose(f);
x{write in file}
```
* Das Fragment wird in die entsprechende Datei geschrieben

# Zeilennummern

```
a{additional input elements}
	t{int} line;
x{additional input elements}
```
* Pro Datei wird die aktuelle Zeile festgehalten

```
a{init additional input fields}
	i->line = n{1};
x{init additional input fields}
```
* Beim Öffnen einer neuen `.x`-Datei befinden wir uns in Zeile `n{1}`

```
d{preprocess char}
	if (ch == s{'\n'}) {
		++input->line;
	}
x{preprocess char}
```
* Wenn ein Zeilenumbruch gelesen wird, wird die aktuelle Zeile
  erhöht

# HTML generieren
* Aus `hx`-Dateien wird ein HTML-Foliensatz generiert

```
i{html.x}
```
* Die Generierung liegt in einer eigenen Datei

