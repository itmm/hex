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
D{file: hx.cc}
	g{global elements}
	int main(
		int argc,
		const char **argv
	) {
		e{main body}
	}
x{file: hx.cc}
```
* Hex ist in C++ geschrieben
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
	#include <memory>
	#include <vector>
x{includes}
```
* Standard File-Funktionen werden vom Programm benötigt
* Standard Bibliothek wird für dynamische Speicherverwaltung benötigt

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
	struct Input {
		FILE *file;
		std::string name;
		e{additional input elements};
		e{input methods};
	};
x{global elements}
```
* Die `Input`-Klasse enthält den Dateinamen der Eingabe-Dateien
* Zusätzlich kann sie eine offene Datei enthalten
* Alle Eingabe-Dateien, die während einer Inklude-Kaskade eingebunden
  werden bleiben offen, damit an der richtigen Stelle weiter gearbeitet
  werden kann
* Weiter Attribute und Methoden können später definiert werden

```
d{input methods}
	Input(
		FILE *file,
		const std::string &name
	): file(file), name(name)
		e{direct input constr}
	{
	}
x{input methods}
```
* Im Konstruktor werden Datei und Name gesetzt
* Später eingeführte Attribute können auch dann initialisiert werden

```
A{global elements}
	std::unique_ptr<Input> input;
	std::vector<std::unique_ptr<Input>> pending;
	std::vector<std::unique_ptr<Input>> used;
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
	FragMap root;
	FragMap *frags = &root;
x{global elements}
```
* Kollektion mit allen Fragmenten wird für folgende Schritte sichtbar
  angelegt

```
A{global elements}
	void pushPath(const std::string &path) {
		FILE *f = fopen(path.c_str(), "r");
		e{check file for path};
		std::unique_ptr<Input> i = std::make_unique<Input>(f, path);
		e{init additional input fields};
		if (input) { pending.push_back(std::move(input)); }
		input = std::move(i);
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
		f, "can't open [%s]", path.c_str()
	);
x{check file for path}
```
* Wenn die Datei nicht geöffnet werden kann, bricht das Programm ab

# Lokale Fragmente

```
d{additional input elements}
	FragMap frags;
x{additional input elements}
```
* Jede Source-Datei hat eine eigene Fragment-Map mit lokalen
  Definitionen

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
	const char *stylesheet =
		"slides/slides.css";
x{global elements}
```
* Für die HTML-Ausgabe wird eine Stylesheet-Datei benötigt
* Über die Kommandozeile kann eine alternative Datei angegeben werden

```
d{process arguments}
	bool someFile = false;
	for (int i = 1; i < argc; ++i) {
		e{process argument};
		ASSERT(
			false,
			"unknown argument [%s]",
			argv[i]
		);
	}
x{process arguments}
```
* Die Argumente werden einzeln durchgegangen
* Wenn sie nicht verwendet werden, bricht das Programm ab

```
d{process argument} {
	const char prefix[] = "--css=";
	int len = sizeof(prefix) - 1;
	if (memcmp(argv[i], prefix, len) == 0) {
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
		pushPath(argv[1]);
		someFile = true;
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
		pushPath("index.x");
	}
x{process arguments}
```
* Wenn kein Pfad angegeben wurde, wird `index.x` als Vorgabe verwendet

# Nächstes Zeichen
* Die Funktion `nextCh` liest das nächste Zeichen aus der aktuellen
  Datei
* Wenn das Dateiende erreicht ist, wird die nächste Datei aus dem
  Stapel der offenen Dateien geholt
* Erst wenn die letzte Datei fertig gelesen wurde, wird ein `EOF`
  zurück geliefert

```
A{global elements}
	int nextCh() {
		int ch = EOF;
		while (input) {
			ch = fgetc(input->file);
			e{preprocess char};
			if (ch != EOF) { break; }
			e{get next input file};
		}
		return ch;
	}
x{global elements}
```
* Wenn kein `EOF` gelesen wurde, dann wird das Zeichen zurück
  geliefert
* Ansonsten wird aus der nächsten Datei ein Zeichen gelesen

```
d{get next input file}
	fclose(input->file);
	used.push_back(std::move(input));
	if (! pending.empty()) {
		input = std::move(pending.back());
		pending.pop_back();
	}
	FragMap *nxt = frags->link;
	frags->link = nullptr;
	frags = nxt;
x{get next input file}
```
* Die aktuelle Datei wird geschlossen und in die Liste der bereits
  verarbeiteten Dateien eingereiht
* Dann wird der Vorgänger zur aktuellen Datei erklärt
* Der Vorgänger wird aus dem globalen Namensraum wieder entfernt

# Eingabe-Dateien lesen
* In diesem Abschnitt werden die Eingabe-Dateien gelesen, um die
  Fragmente aufzubauen und alle notwendigen Beziehungen zwischen den
  einzelnen Folien zu finden

```
d{read source file} {
	e{additional read vars};
	int last = nextCh();
	int ch = last != EOF ? nextCh() : EOF;
	while (ch != EOF) {
		e{process current char};
		last = ch; ch = nextCh();
	}
} x{read source file}
```
* Neben dem aktuellen Zeichen wird auch das letzte Zeichen aufgehoben
* Dabei kann `hx` auch mit einer leeren Eingabe-Datei umgehen (wenn
  schon das erste Zeichen ein `EOF` ist)

```
d{process current char}
	switch (ch) {
		case '{':
			e{process open brace};
			break;
		case '}': {
			bool processed = false;
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
	Frag *frag = nullptr;
	std::string buffer;
	int bufferLine = 0;
x{additional read vars}
```
* Wir unterscheiden, ob wir in einem Code-Block sind, oder außerhalb
* In einem Code sind wir sogar in einem Fragment, dessen Inhalt gerade
  gelesen wird
* Am Anfang sind wir außerhalb eines Code-Blocks
* In einem Code-Block ist `frag` nicht `nullptr`

```
a{additional read vars}
	char openCh = '\0';
x{additional read vars}
```
* Das Zeichenvor einer öffnenden Mengenklammer wird in `openCh`
  zwischengespeichert
* Es beschreibt, welcher Befehl ausgeführt werden soll

```
a{additional read vars}
	std::string name;
	bool useName = false;
	int nameLine = 0;
x{additional read vars}
```
* Wenn `name` aktiv ist, dann wird ein Name in Buffer gelesen

```
d{process close brace} {
	if (useName || ! name.empty()) {
		e{process frag name};
		name.clear(); useName = false;
		last = ch;
		ch = nextCh();
	}
} x{process close brace}
```
* Bei einer schließenden Mengenklammer wird der Befehls-Name ausgewertet
* Danach wird der Namenszeiger zurückgesetzt

```
d{process other char} {
	if (useName || ! name.empty()) {
		name.push_back(ch);
		break;
	}
} x{process other char}
```
* Wenn ein Name geparst wird, dann der Namensbuffer entsprechend
  erweitert

```
a{process other char} {
	if (frag) {
		if (buffer.empty()) {
			bufferLine = input->line;
		}
		buffer.push_back(last);
	}
} x{process other char}
```
* Wenn es ein aktuelles Fragment gibt, dann müssen sonstige Zeichen dort
  angefügt werden

```
d{process open brace} {
	if (! frag) {
		static const char valids[] = "aAdDirR";
		if (strchr(valids, last)) {
			openCh = last;
			useName = true;
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
	if (openCh == 'd') {
		ASSERT(! frag, "def in frag");
		FragMap *fm = &input->frags;
		E{check for double def};
		if (! frag) {
			frag = &(*fm)[name];
		}
		processed = true;
	}
x{process frag name}
```
* Erzeugt ein neues Fragment
* Das Fragment darf nicht mehrfach definiert werden

```
a{process frag name}
	if (openCh == 'D') {
		ASSERT(! frag, "def in frag");
		FragMap *fm = frags;
		E{check for double def};
		if (! frag) {
			frag = &root[name];
		}
		processed = true;
	}
x{process frag name}
```
* Erzeugt ein neues Fragment im globalen Namensraum
* Das Fragment darf nicht mehrfach definiert werden

```
d{check for double def}
	frag = fm->find(name);
	if (isPopulatedFrag(frag)) {
		printf(
			"frag [%s] already defined\n",
			name.c_str()
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
	if (openCh == 'a') {
		ASSERT(! frag, "add in frag");
		FragMap *fm = &input->frags;
		FragMap *ins = fm;
		frag = fm->find(name);
		E{check for add without def};
		processed = true;
	}
x{process frag name}
```
* Bei einem öffnenden Befehl wird das passende Fragment gesucht
* Weitere Bytes können zu diesem Fragment hinzugefügt werden

```
a{process frag name}
	if (openCh == 'A') {
		ASSERT(! frag, "add in frag");
		FragMap *fm = frags;
		FragMap *ins = &root;
		frag = fm->find(name);
		E{check for add without def};
		processed = true;
	}
x{process frag name}
```
* Erweitert ein global definiertes Fragment

```
d{check for add without def}
	if (! isPopulatedFrag(frag)) {
		printf(
			"frag [%s] not defined\n",
			name.c_str()
		);
		frag = &fm->get(name, *ins);
	}
x{check for add without def}
```
* Das Fragment muss bereits vorhanden und nicht leer sein

```
a{process frag name}
	if (openCh == 'r') {
		ASSERT(! frag, "replace in frag");
		frag = &(input->frags[name]);
		ASSERT(
			frag, "frag %s not defined",
			name.c_str()
		);
		frag->clear();
		processed = true;
	}
x{process frag name}
```
* Bei einem `@replace` wird der Inhalt eines Fragments zurückgesetzt
* Das Fragment muss bereits vorhanden sein

```
a{process frag name}
	if (openCh == 'R') {
		ASSERT(! frag, "replace in frag");
		frag = &frags->get(name, root);
		ASSERT(
			frag, "frag %s not defined",
			name.c_str()
		);
		frag->clear();
		processed = true;
	}
x{process frag name}
```
* Ersetzt ein global definiertes Fragment

```
a{process frag name}
	if (openCh == 'x') {
		ASSERT(frag, "end not in frag");
		e{frag names must match};
		E{flush frag buffer};
		frag = nullptr;
		processed = true;
	}
x{process frag name}
```
* Bei einem schließenden Befehl wird das aktuelle Fragment unterbrochen

```
d{frag names must match}
	ASSERT(
		frag->name == name,
		"closing [%s] != [%s]",
		name.c_str(), frag->name.c_str()
	);
x{frag names must match}
```
* Wenn der öffnende und schließende Name nicht passt, wird die
  Abarbeitung abgebrochen

```
A{global elements}
	bool alreadyUsed(const std::string &name) {
		if (input && input->name == name) {
			return true;
		}
		for (auto &j : pending) {
			if (j->name == name) {
				return true;
			}
		}
		for (auto &j : used) {
			if (j->name == name) {
				return true;
			}
		}
		return false;
	}
x{global elements}
```
* Prüft ob eine Datei bereits verwendet wurde
* Sowohl die offenen als auch die bereits prozessierten Dateien werden
  durchgegangen
* Dadurch wird bei Einbettungen verhindert, dass eine Datei mehrfach
  verarbeitet wird

```
a{process frag name}
	if (openCh == 'i') {
		ASSERT(! frag, "include in frag");
		if (! alreadyUsed(name.c_str())) {
			pushPath(name);
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
	if (openCh == 'e') {
		ASSERT(frag, "expand not in frag");
		E{flush frag buffer};
		Frag &sub = input->frags[name];
		E{check frag expand count};
		++sub.expands;
		frag->add(&sub);
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
	if (openCh == 'g') {
		ASSERT(frag, "expand not in frag");
		E{flush frag buffer};
		Frag &sub = frags->get(name, root);
		E{check frag expand count};
		++sub.expands;
		frag->add(&sub);
		processed = true;
	}
x{process frag name}
```

```
d{check frag expand count}
	if (sub.expands) {
		printf(
			"multiple expands of [%s]\n",
			sub.name.c_str()
		);
	}
	if (sub.multiples) {
		printf(
			"expand after mult of [%s]\n",
			sub.name.c_str()
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
	if (openCh == 'E') {
		ASSERT(frag, "multiple not in frag");
		E{flush frag buffer};
		Frag &sub = input->frags[name];
		E{check for prev expands};
		++sub.multiples;
		frag->add(&sub);
		processed = true;
	}
x{process frag name}
```
* Mit einem `@multiple` Befehl kann ein Fragment an mehreren Stellen
  expandiert werden

```
a{process frag name}
	if (openCh == 'G') {
		ASSERT(frag, "multiple not in frag");
		E{flush frag buffer};
		Frag &sub = frags->get(name, root);
		E{check for prev expands};
		++sub.multiples;
		frag->add(&sub);
		processed = true;
	}
x{process frag name}
```

```
d{check for prev expands}
	if (sub.expands) {
		printf(
			"multiple after expand "
				"of [%s]\n",
			sub.name.c_str()
		);
	}
x{check for prev expands}
```
* Es ist ein Fehler, wenn das Fragment bereits normal `@expand`iert
  wurde

```
a{process frag name}
	if (openCh == 'p') {
		ASSERT(frag, "private not in frag");
		e{process private frag};
		processed = true;
	}
x{process frag name}
```
* Private Bezeichner werden durch einen Hash erweitert
* Um sie global unique zu machen

```
d{process private frag}
	Hash h;
	h.add(input->name);
	unsigned cur = h.add(name);
x{process private frag}
```
* Der Hash wird aus dem aktuellen Dateinamen
* Und dem aktuellen Bezeichner berechnet
* Zum Schluss wird er auf eine positive Zahl maskiert

```
a{process private frag}
	static char hash[12];
	char *end = hash + sizeof(hash);
	char *head = end;
	*--head = '_';
	for (;;) {
		ASSERT(head > hash);
		*--head = (cur % 10) + '0';
		cur /= 10;
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
	static char prefix[] = "_private_";
	frag->add(
		prefix, input->name, nameLine
	);
	frag->add(
		std::string(head, end),
		input->name, nameLine
	);
	frag->add(
		name,
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
	if (openCh == 'm') {
		ASSERT(frag, "magic not in frag");
		e{process magic frag};
		processed = true;
	}
x{process frag name}
```
* Der `@magic`-Befehl erzeugt einen Hash-Wert
* Der sich aus dem Dateinamen und dem Argument des Befehls
  zusammen setzt

```
d{process magic frag}
	Hash h;
	h.add(input->name);
	unsigned cur = h.add(name);
x{process magic frag}
```
* Berechnet Hash-Wert

```
a{process magic frag}
	static char magic[12];
	char *end = magic + sizeof(magic);
	char *head = end;
	for (;;) {
		ASSERT(head > magic);
		*--head = (cur % 10) + '0';
		cur /= 10;
		if (! cur) { break; }
	}
	E{flush frag buffer};
	frag->add(
		std::string(head, end),
		input->name, nameLine
	);
x{process magic frag}
```
* Gibt den Hash-Wert aus
* Vorher wird noch eventuell gespeicherte Zeichen ausgegeben

```
d{flush frag buffer}
	if (! buffer.empty()) {
		frag->add(
			buffer,
			input->name, bufferLine
		);
		buffer.clear();
	}
x{flush frag buffer}
```
* Das Fragment fügt alle Bytes im Buffer an ein Fragment an
* Danach wird der Buffer zurück gesetzt

```
a{process open brace} {
	if (frag) {
		bool valid = false;
		e{check valid names};
		if (valid) {
			openCh = last;
			nameLine = input->line;
			useName = true;
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
	static const char valids[] =
		"fvsntkxeEgGpmb";
	if (strchr(valids, last)) {
		valid = true;
	}
x{check valid names}
```
* Gültige Kommando-Zeichen sind in einem String abgelegt
* Wenn das Zeichen im String vorkommt, dann ist es gültig

```
a{process frag name}
	if (! processed) {
		ASSERT(
			frag, "unknown frag %s",
			name.c_str()
		);
		for (auto &c : name) {
			if (buffer.empty()) {
				bufferLine = input->line;
			}
			buffer.push_back(c);
		}
		processed = true;
	}
x{process frag name}
```
* Wenn kein bekannter Befehl erkannt wurde, dann ist die
  befehlsähnliche Eingabe Teil des Programms
* Und wird daher in den entsprechenden Buffer kopiert

```
a{process open brace}
	if (frag) {
		if (buffer.empty()) {
			bufferLine = input->line;
		}
		buffer.push_back(last);
	}
x{process open brace}
```
* Wenn wir uns in einem Fragment befinden und bis hier gekommen sind,
  dann wird das Zeichen vor der öffnenden Klammer zum Fragment hinzu
  gefügt

```
a{process close brace}
	if (frag && ! processed) {
		if (buffer.empty()) {
			bufferLine = input->line;
		}
		buffer.push_back(last);
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
	for (auto &i : root.map) {
		Frag *frag = &i.second;
		E{serialize frag};
	}
} x{serialize fragments}
```
* Fragmente, die mit `file:` beginnen, werden in die entsprechenden
  Dateien rausgeschrieben
* Zusätzlich wird geprüft, ob Fragmente zu selten oder zu oft expandiert
  wurden

```
a{serialize fragments} {
	for (auto &j : used)
	{
		for (auto &i : j->frags.map) {
			Frag *frag = &i.second;
			E{serialize frag};
		}
	}
} x{serialize fragments}
```
* Auch alle lokalen Fragmente bearbeiten

```
d{serialize frag}
	if (! memcmp(
		"file: ", frag->name.data(), 6
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
	int sum =
		frag->expands + frag->multiples;
	if (sum <= 0) {
		printf(
			"frag [%s] not called\n",
			frag->name.c_str()
		);
	}
} x{serialize frag}
```
* Ein Fragment wurde nicht aufgerufen
* Dies wird mit einer Meldung quittiert

```
a{serialize frag}
	if (frag->multiples == 1) {
		printf(
			"multiple frag [%s] only "
				"used once\n",
			frag->name.c_str()
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
			"frag [%s] not populated\n",
			frag->name.c_str()
		);
	}
x{serialize frag}
```
* Für jedes Fragment, das nicht befüllt wurde wird eine Meldung
  ausgegeben

```
d{write in file}
	FILE *f =
		fopen(frag->name.substr(6).c_str(), "w");
	ASSERT(
		f, "can't open %s",
		frag->name.substr(6).c_str()
	);
	serializeFrag(*frag, f, false);
	fclose(f);
x{write in file}
```
* Das Fragment wird in die entsprechende Datei geschrieben

# Zeilennummern

```
a{additional input elements}
	int line;
x{additional input elements}
```
* Pro Datei wird die aktuelle Zeile festgehalten

```
d{direct input constr}
	, line(1)
x{direct input constr}
```
* Zeile wird mit `1` initialisiert

```
d{preprocess char}
	if (ch == '\n') {
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

