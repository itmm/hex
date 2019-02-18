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
D{file: hx.cpp}
	e{global elements}
	int main(
		int argc,
		const char **argv
	) {
		e{main body}
	}
x{file: hx.cpp}
```
* Hex ist in C++ geschrieben
* Das Hauptprogramm besteht aus der `main`-Funktion
* Zusätzlich wird ein Fragment definiert, in welchem globale Elemente
  definiert werden können
* Übergeben wird der Funktion die Anzahl der Argumente in `argc`
* Und der Aufrufname mit Argumenten als Array in `argv`

```
d{main body}
	e{perform unit-tests};
	e{process arguments};
	e{read source file};
	e{serialize fragments};
	e{write HTML file};
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
d{global elements}
	e{includes};
	e{define logging};
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

# Logging
* Mehrere Programme verwenden die gleichen Log-Makros
* Daher enthält eine eigene Datei diese Makros

```
i{log.x}
```
* Bindet Logging ein

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

# Input
* Eine Klasse für Eingabe-Dateien enthält neben der offenen Datei noch
  die aktuelle Zeilennummer oder verweise auf offene inkludierte Dateien
* Die Definition der Klasse mit allen Methoden ist in einer eigenen
  Datei ausgelagert

```
i{input.x}
```
* Bindet Datei-Klasse ein

# Kommandozeile
* Die Kommandozeile wird Element für Element abgearbeitet

```
a{global elements}
	std::string stylesheet {
		"slides/slides.css"
	};
x{global elements}
```
* Für die HTML-Ausgabe wird eine Stylesheet-Datei benötigt
* Über die Kommandozeile kann eine alternative Datei angegeben werden

```
a{global elements}
	int blockLimit = -1;
x{global elements}
```
* Die Anzahl der Blocks, die ausgegeben werden sollen, kann mit
  diesem Parameter limitiert werden

```
a{global elements}
	Inputs inputs;
x{global elements}
```
* `inputs` enthält neben der gerade offenen Datei auch alle Dateien, die
  noch prozessiert werden müssen
* Und alle bereits gelesenen Dateien

```
d{process arguments}
	bool someFile { false };
	for (int i { 1 }; i < argc; ++i) {
		e{process argument};
		ASSERT_MSG(false,
			"unknown argument [" <<
			argv[i] << ']'
		);
	}
x{process arguments}
```
* Die Argumente werden einzeln durchgegangen
* Wenn sie nicht verwendet werden, bricht das Programm ab

```
d{process argument} {
	static const std::string prefix {
		"--css="
	};
	std::string arg { argv[i] };
	if (arg.substr(
		0, prefix.length()
	) == prefix) {
		stylesheet =
			arg.substr(prefix.length());
		continue;
	}
} x{process argument}
```
* Der Pfad zur Stylesheet-Datei kann über die Kommandozeile gesetzt
  werden

```
a{process argument} {
	static const std::string prefix {
		"--limit="
	};
	std::string arg { argv[i] };
	if (arg.substr(
		0, prefix.length()
	) == prefix) {
		e{extract block limit};
	}
} x{process argument}
```
* Der Benutzer kann die Anzahl der Folien beschränken, aus denen der
  Code generiert wird
* So können Teilabschnitte validiert werden

```
d{extract block limit}
	std::istringstream iss {
		arg.substr(prefix.length())
	};
	iss >> blockLimit;
	continue;
x{extract block limit}
```
* Das Programm legt das Argument in einen Input-Stream
* Und liest aus diesem eine Zahl

```
a{process argument}
	if (! someFile) {
		inputs.push(argv[1]);
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
		inputs.push("index.x");
	}
x{process arguments}
```
* Wenn kein Pfad angegeben wurde, wird `index.x` als Vorgabe verwendet

# Eingabe-Dateien lesen
* In diesem Abschnitt werden die Eingabe-Dateien gelesen, um die
  Fragmente aufzubauen und alle notwendigen Beziehungen zwischen den
  einzelnen Folien zu finden

```
d{read source file} {
	e{additional read vars};
	std::string line;
	while (inputs.getLine(line)) {
		e{process line};
	}
} x{read source file}
```
* `hx` liest die Eingabe-Dateien zeilenweise
* Inkludierungen werden transparent in `inputs` behandelt

```
a{global elements}
	using SI =
		std::string::const_iterator;
x{global elements}
```
* Die Anwendung verwendet den String-Iterator an vielen Stellen
* Daher definiert sie eine Abkürzung, damit die Folien nicht überlaufen

```
a{global elements}
	void process_chars(
		Frag *frag, SI i, SI e
	) {
		e{process chars};
	}
x{global elements}
```
* Fügt ein Range an den Inhalt von `frag` an

```
a{global elements}
	void process_char(Frag *frag, char ch) {
		e{process char};
	}
x{global elements}
```
* Fügt ein Zeichen an den Inhalt von `frag` an

```
d{process line}
	auto end = line.cend();
	for (
		auto i = line.cbegin();
		i != end; ++i
	) {
		e{process special lines};
		process_chars(frag, i, i + 1);
	}
	process_char(frag, '\n');
x{process line}
```
* Neben dem aktuellen Zeichen wird auch das letzte Zeichen aufgehoben
* Dabei kann `hx` auch mit einer leeren Eingabe-Datei umgehen (wenn
  schon das erste Zeichen ein `EOF` ist)

```
a{global elements}
	bool is_macro_start(
		const Frag *frag, SI begin, SI end
	) {
		e{is macro start};
		return false;
	}
x{global elements}
```
* Diese Funktion gibt an, ob an der Stelle `i` ein Makro beginnt

```
a{global elements}
	SI find_macro_end(SI i, SI e) {
		e{find macro end};
		return i;
	}
x{global elements}
```
* Liefert das Ende des Makros

```
d{process special lines}
	if (is_macro_start(frag, i, end)) {
		auto j = find_macro_end(i, end);
		if (j != end) {
			do {
				e{process macro};
			} while (false); 
			i += (j - i);
			continue;
		}
	}
x{process special lines}
```
* Wenn in einer Zeile ein Makro gefunden wird, dann wird dieses
  verarbeitet
* In `s{process macro}` kann bei Erfolg mit `break` der Block verlassen
  werden


```
d{is macro start}
	auto n = begin + 1;
	if (n >= end) { return false; }
	if (*n != '{') { return false; }
x{is macro start}
```
* Hinter dem Bezeichner muss eine öffnende Mengen-Klammer stehen

```
d{additional read vars}
	Frag *frag { nullptr };
x{additional read vars}
```
* Wir unterscheiden, ob wir in einem Code-Block sind, oder außerhalb
* In einem Code sind wir sogar in einem Fragment, dessen Inhalt gerade
  gelesen wird
* Am Anfang sind wir außerhalb eines Code-Blocks
* In einem Code-Block ist `frag` nicht `nullptr`

```
d{process chars}
	if (frag) {
		std::string str {i, e};
		frag->add(
			str, inputs.cur()->name,
			inputs.cur()->line()
		);
	}
x{process chars}
```
* Fügt mehrere Zeichen an das aktuelle Fragment an

```
d{process char}
	if (frag) {
		frag->add(
			ch, inputs.cur()->name,
			inputs.cur()->line()
		);
	}
x{process char}
```
* Fügt einzelnes Zeichen an das aktuelle Fragment an

```
a{includes}
	#include <algorithm>
x{includes}
```
* `find_macro_end` benötigt `std::find`

```
d{find macro end}
	return std::find(i, e, '}');
x{find macro end}
```
* Eine schließende Mengen-Klammer markiert das Ende des Makros

```
d{process macro}
	char openCh {*i};
	i += 2;
	std::string name {i, j};
x{process macro}
```
* `openCh` enthält Makro-Indikator
* Die Zeichen zwischen den Mengen-Klammern sind der Name des Makros

```
a{is macro start}
	if (! frag) {
		e{may start block};
	}
x{is macro start}
```
* Prüft ob ein Makro außerhalb eines Fragments gültig ist

```
d{may start block}
	static const std::string valids {
		"aAdDirR"
	};
	bool found {
		valids.find(*begin) !=
			std::string::npos
	};
	if (found && blockLimit != 0) {
		e{start block};
	}
x{may start block}
```
* Außerhalb eines Fragments können nur Makros verwendet werden, um ein
  Fragment zu erweitern
* Oder um eine weitere `hx`-Datei einzubinden
* Das Makro wird nur verarbeitet, wenn die Anzahl der zu verarbeitenden
  Folien noch nicht erreicht wurde

```
d{start block}
	--blockLimit;
	return true;
x{start block}
```
* Reduziert die Anzahl der noch zu verarbeitenden Folien
* Und liefert `true` zurück

```
a{process macro}
	if (openCh == 'd') {
		ASSERT_MSG(! frag, "def in frag");
		FragMap *fm {
			&inputs.cur()->frags
		};
		E{check for double def};
		if (! frag) {
			frag = &(*fm)[name];
		}
		break;
	}
x{process macro}
```
* Erzeugt ein neues Fragment
* Das Fragment darf nicht mehrfach definiert werden

```
a{process macro}
	if (openCh == 'D') {
		ASSERT_MSG(! frag, "def in frag");
		FragMap *fm { frags };
		E{check for double def};
		if (! frag) {
			frag = &root[name];
		}
		break;
	}
x{process macro}
```
* Erzeugt ein neues Fragment im globalen Namensraum
* Das Fragment darf nicht mehrfach definiert werden

```
d{check for double def}
	frag = fm->find(name);
	if (isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			name <<
			"] already defined\n";
	}
x{check for double def}
```
* Wenn das Fragment bereits existiert, wird es vielleicht nur verwendet
* Es muss geprüft werden, ob es schon Inhalt hat
* Das wäre dann eine Fehlermeldung wert
* Bricht aber die Abarbeitung nicht ab

```
a{process macro}
	if (openCh == 'a') {
		ASSERT_MSG(! frag, "add in frag");
		FragMap *fm {
			&inputs.cur()->frags
		};
		FragMap *ins { fm };
		frag = fm->find(name);
		E{check for add w/o def};
		break;
	}
x{process macro}
```
* Bei einem öffnenden Befehl wird das passende Fragment gesucht
* Weitere Bytes können zu diesem Fragment hinzugefügt werden

```
a{process macro}
	if (openCh == 'A') {
		ASSERT_MSG(! frag, "add in frag");
		FragMap *fm { frags };
		FragMap *ins { &root };
		frag = fm->find(name);
		E{check for add w/o def};
		break;
	}
x{process macro}
```
* Erweitert ein global definiertes Fragment

```
d{check for add w/o def}
	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			name <<
			"] not defined\n";
		frag = &fm->get(
			name, *ins
		);
	}
x{check for add w/o def}
```
* Das Fragment muss bereits vorhanden und nicht leer sein

```
a{process macro}
	if (openCh == 'r') {
		ASSERT_MSG(! frag,
			"replace in frag"
		);
		frag = &(inputs.cur()->frags[
			name
		]);
		E{clear frag};
		break;
	}
x{process macro}
```
* Bei einem `@replace` wird der Inhalt eines Fragments zurückgesetzt
* Das Fragment muss bereits vorhanden sein

```
d{clear frag}
	ASSERT_MSG(frag, "frag " <<
		name <<
		" not defined"
	);
	frag->clear();
x{clear frag}
```
* Löscht das aktuelle Fragment

```
a{process macro}
	if (openCh == 'R') {
		ASSERT_MSG(! frag,
			"replace in frag"
		);
		frag = &frags->get(
			name, root
		);
		E{clear frag};
		break;
	}
x{process macro}
```
* Ersetzt ein global definiertes Fragment

```
a{process macro}
	if (openCh == 'x') {
		ASSERT_MSG(frag,
			"end not in frag"
		);
		e{frag names must match};
		frag = nullptr;
		break;
	}
x{process macro}
```
* Bei einem schließenden Befehl wird das aktuelle Fragment unterbrochen

```
d{frag names must match}
	ASSERT_MSG(frag->name == name,
		"closing [" << name <<
		"] != [" << frag->name << ']'
	);
x{frag names must match}
```
* Wenn der öffnende und schließende Name nicht passt, wird die
  Abarbeitung abgebrochen

```
a{process macro}
	if (openCh == 'i') {
		ASSERT_MSG(! frag,
			"include in frag"
		);
		if (! inputs.has(name)) {
			inputs.push(name);
		}
		break;
	}
x{process macro}
```
* Wenn eine Datei eingebunden werden soll, dann wird sie geöffnet und
  auf den Stapel der offenen Dateien gelegt
* Wenn die Datei bereits geöffnet wurde, dann wird sie ignoriert

```
a{process macro}
	if (openCh == 'e') {
		ASSERT_MSG(frag,
			"expand not in frag"
		);
		Frag &sub = inputs.cur()->frags[
			name
		];
		E{check frag ex. count};
		sub.addExpand();
		frag->add(&sub);
		break;
	}
x{process macro}
```
* Bei einem `@expand` wird das Fragment gesucht und eingebunden
* Ggf. wird das Fragment dabei auch erzeugt, um später befüllt zu werden

```
a{process macro}
	if (openCh == 'g') {
		ASSERT_MSG(frag,
			"globexpand not in frag"
		);
		Frag &sub = frags->get(
			name, root
		);
		E{check frag ex. count};
		sub.addExpand();
		frag->add(&sub);
		break;
	}
x{process macro}
```
* Bei einem `@globexpand` wird das Fragment in den umschließenden
  `FragMap`s gesucht

```
d{check frag ex. count}
	if (sub.expands()) {
		std::cerr <<
			"multiple expands of [" <<
			sub.name << "]\n";
	}
	if (sub.multiples()) {
		std::cerr <<
			"expand after mult of ["
			<< sub.name << "]\n";
	}
x{check frag ex. count}
```
* Wenn das Fragment bereits expandiert wurde, dann wird eine Meldung
  ausgegeben
* Wenn das Fragment bereits im Mehrfach-Modus ausgegeben wurde, wird
  ebenfalls eine Meldung ausgegeben

```
a{process macro}
	if (openCh == 'E') {
		ASSERT_MSG(frag,
			"multiple not in frag"
		);
		Frag &sub { inputs.cur()->frags[
			name
		] };
		E{check for prev expands};
		sub.addMultiple();
		frag->add(&sub);
		break;
	}
x{process macro}
```
* Mit einem `@multiple` Befehl kann ein Fragment an mehreren Stellen
  expandiert werden

```
a{process macro}
	if (openCh == 'G') {
		ASSERT_MSG(frag,
			"globmult not in frag"
		);
		Frag &sub { frags->get(
			name, root
		) };
		E{check for prev expands};
		sub.addMultiple();
		frag->add(&sub);
		break;
	}
x{process macro}
```
* `@globmult` expandiert ein globales Fragment an mehreren Stellen

```
d{check for prev expands}
	if (sub.expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub.name << "]\n";
	}
x{check for prev expands}
```
* Es ist ein Fehler, wenn das Fragment bereits normal `@expand`iert
  wurde

```
a{process macro}
	if (openCh == 'p') {
		ASSERT_MSG(frag,
			"private not in frag"
		);
		e{process private frag};
		break;
	}
x{process macro}
```
* Private Bezeichner werden durch einen Hash erweitert
* Um sie global unique zu machen

```
a{includes}
	#include <functional>
	#include <sstream>
x{includes}
```
* Enthält Hash-Funktion

```
d{process private frag}
	std::hash<std::string> h;
	unsigned cur {
		h(inputs.cur()->name +
			':' + name) &
				0x7fffffff
	};
x{process private frag}
```
* Der Hash wird aus dem aktuellen Dateinamen
* Und dem aktuellen Bezeichner berechnet
* Zum Schluss wird er auf eine positive Zahl maskiert

```
a{process private frag}
	std::ostringstream hashed;
	hashed << "_private_" <<
		cur << '_' <<
		name;
	frag->add(
		hashed.str(),
		inputs.cur()->name,
		inputs.cur()->line()
	);
x{process private frag}
```
* Zuerst werden eventuell zwischengespeicherte Zeichen ausgegeben
* Dann kommt der neue Bezeichner
* Dieser besteht aus einem konstanten Präfix
* Dem Hash-Wert
* Und dem alten Bezeichner

```
a{process macro}
	if (openCh == 'm') {
		ASSERT_MSG(frag,
			"magic not in frag"
		);
		e{process magic frag};
		break;
	}
x{process macro}
```
* Der `@magic`-Befehl erzeugt einen Hash-Wert
* Der sich aus dem Dateinamen und dem Argument des Befehls
  zusammen setzt

```
d{process magic frag}
	std::hash<std::string> h;
	unsigned cur {
		h(inputs.cur()->name +
			':' + name) &
				0x7fffffff
	};
x{process magic frag}
```
* Berechnet Hash-Wert

```
a{process magic frag}
	std::ostringstream value;
	value << cur;
	frag->add(
		value.str(),
		inputs.cur()->name,
		inputs.cur()->line()
	);
x{process magic frag}
```
* Gibt den Hash-Wert aus
* Vorher wird noch eventuell gespeicherte Zeichen ausgegeben

```
a{is macro start} {
	if (frag) {
		e{check valid names};
	}
} x{is macro start}
```
* Prüft, ob ein Befehl innerhalb eines Fragments mit einem gültigen
  Zeichen beginnt
* In diesem Fall wird das Zeichen als Befehls-Opcode gesichert und der
  Buffer aktiviert um alle Zeichen bis zu eine schließenden
  Mengenklammer zu speichern

```
d{check valid names}
	static const std::string valids { 
		"fvsntkxeEgGpmb"
	};
	bool found {
		valids.find(
			static_cast<char>(*begin)
		) != std::string::npos
	};
	if (found) {
		return true;
	}
x{check valid names}
```
* Gültige Kommando-Zeichen sind in einem String abgelegt
* Wenn das Zeichen im String vorkommt, dann ist es gültig

```
a{process macro}
	ASSERT_MSG(frag,
		"must be in frag " << openCh <<
		 	'{' << name << '}'
	);
	frag->add(
		name,
		inputs.cur()->name,
		inputs.cur()->line()
	);
x{process macro}
```
* Sonstige Makros dürfen nur in Fragmenten vorkommen
* Sie dienen der Formatierung und können bei der Code-Generierung
  ignoriert werden

# Fragmente serialisieren
* Fragmente, die Dateien spezifizieren werden in diese Dateien
  rausgeschrieben

```
d{serialize fragments}
	for (auto &i : root) {
		const Frag *frag { &i.second };
		E{serialize frag};
	}
x{serialize fragments}
```
* Fragmente, die mit `file:` beginnen, werden in die entsprechenden
  Dateien rausgeschrieben
* Zusätzlich wird geprüft, ob Fragmente zu selten oder zu oft expandiert
  wurden

```
a{serialize fragments}
	for (auto &j : inputs) {
		for (auto &i : j->frags) {
			const Frag *frag {
				&i.second
			};
			E{serialize frag};
		}
	}
x{serialize fragments}
```
* Auch alle lokalen Fragmente bearbeiten

```
d{serialize frag} {
	if (frag->isFile()) {
		e{write in file};
	}
} x{serialize frag}
```
* Wenn der Name eines Fragments mit `file: ` beginnt, dann wird es in
  die passende Datei geschrieben
* Zusätzlich zählt das als eine Expansion

```
a{serialize frag} {
	int sum {
		frag->expands()
			+ frag->multiples()
	};
	if (sum <= 0) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not called\n";
	}
} x{serialize frag}
```
* Ein Fragment wurde nicht aufgerufen
* Dies wird mit einer Meldung protokolliert

```
a{serialize frag}
	if (frag->multiples() == 1) {
		std::cerr <<
			"multiple frag [" <<
			frag->name <<
			"] only used once\n";
	}
x{serialize frag}
```
* Ein Fragment das zur mehrfachen Verwendung deklariert wurde, wird nur
  einmal verwendet
* Dies wird mit einer Meldung protokolliert

```
a{serialize frag}
	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not populated\n";
	}
x{serialize frag}
```
* Für jedes Fragment, das nicht befüllt wurde wird eine Meldung
  ausgegeben

```
d{write in file}
	std::ofstream out(
		frag->name.substr(6).c_str()
	);
	serializeFrag(*frag, out, false);
	out.close();
x{write in file}
```
* Das Fragment wird in die entsprechende Datei geschrieben

# HTML generieren
* Aus `hx`-Dateien wird ein HTML-Foliensatz generiert

```
i{html.x}
```
* Die Generierung liegt in einer eigenen Datei

