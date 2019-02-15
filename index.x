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

```
i{log.x}
```

# Buffer
* Buffer können als Erweiterung von Strings aktiv gesetzt werden
* Und tracken den enthaltenen Range

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

# Input

```
i{input.x}
```

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

```
d{extract block limit}
	std::istringstream iss {
		arg.substr(prefix.length())
	};
	iss >> blockLimit;
	continue;
x{extract block limit}
```

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
		auto end = line.end();
		for (auto i = line.begin(); i != end; ++i) {
			if (is_macro_start(frag, i, end)) {
				auto j = find_macro_end(i, end);
				if (j != end) {
					process_chars(frag, line.begin(), i);
					process_macro(frag, i, j);
					process_chars(frag, j, end);
					break;
				}
			} else {
				process_chars(frag, i, i + 1);
			}
		}
		process_char(frag, '\n');
	}
} x{read source file}
```
* Neben dem aktuellen Zeichen wird auch das letzte Zeichen aufgehoben
* Dabei kann `hx` auch mit einer leeren Eingabe-Datei umgehen (wenn
  schon das erste Zeichen ein `EOF` ist)


```
a{global elements}
	void process_char(const Frag *frag, char ch) {
		if (frag) {
			Buf buffer;
			buffer.add(
				ch,
				inputs.cur()->name,
				inputs.cur()->line()
			);
		}
	}
x{global elements}
```

```
a{global elements}
	void process_chars(const Frag *frag, std::string::const_iterator i, std::string::const_iterator e) {
		for (; i != e; ++i) {
			process_char(frag, *i);
		}
	}
x{global elements}
```

```
a{global elements}
	bool is_macro_start(const Frag *frag, std::string::const_iterator i, std::string::const_iterator e) {
		auto n = i + 1;
		if (n >= e) { return false; }
		if (*n != '{') { return false; }
		e{process open brace};
		return false;
	}
x{global elements}
```

```
a{global elements}
	std::string::const_iterator find_macro_end(std::string::const_iterator i, std::string::const_iterator e) {
		while (i != e && *i != '}') {
			++i;
		}
		return i;
	}
x{global elements}
```

```
a{global elements}
	void process_macro(Frag *&frag, std::string::const_iterator i, std::string::const_iterator e) {
		char openCh{*i};
		i += 2;
		std::string name {i, e};
		e{process frag name};
	}
x{global elements}
```

```
d{additional read vars}
	Frag *frag { nullptr };
	Buf buffer;
x{additional read vars}
```
* Wir unterscheiden, ob wir in einem Code-Block sind, oder außerhalb
* In einem Code sind wir sogar in einem Fragment, dessen Inhalt gerade
  gelesen wird
* Am Anfang sind wir außerhalb eines Code-Blocks
* In einem Code-Block ist `frag` nicht `nullptr`

```
a{additional read vars}
	Buf name;
x{additional read vars}
```
* Wenn `name` aktiv ist, dann wird ein Name in Buffer gelesen

```
d{process open brace}
	if (! frag) {
		e{may start block};
	}
x{process open brace}
```

```
d{may start block}
	static const std::string valids {
		"aAdDirR"
	};
	bool found {
		valids.find(*i) !=
			std::string::npos
	};
	if (found && blockLimit > 0) {
		e{start block};
	}
x{may start block}
```
* Wenn außerhalb eines Fragments die Folge `a`, `{` gelesen wird, dann
  beginnt ein `@add`-Fragment
* Falls ein `i`, `{` gelesen wird, dann muss eine andere Datei mit
  `@include` eingebunden werden
* Es folgt der Name des Fragments oder der Pfad der Datei bis zum
  nächsten `}`

```
d{start block}
	--blockLimit;
	return true;
x{start block}
```

```
d{process frag name}
	if (openCh == 'd') {
		ASSERT_MSG(! frag, "def in frag");
		FragMap *fm {
			&inputs.cur()->frags
		};
		E{check for double def};
		if (! frag) {
			frag = &(*fm)[name];
		}
		return;
	}
x{process frag name}
```
* Erzeugt ein neues Fragment
* Das Fragment darf nicht mehrfach definiert werden

```
a{process frag name}
	if (openCh == 'D') {
		ASSERT_MSG(! frag, "def in frag");
		FragMap *fm { frags };
		E{check for double def};
		if (! frag) {
			frag = &root[name];
		}
		return;
	}
x{process frag name}
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
a{process frag name}
	if (openCh == 'a') {
		ASSERT_MSG(! frag, "add in frag");
		FragMap *fm {
			&inputs.cur()->frags
		};
		FragMap *ins { fm };
		frag = fm->find(name);
		E{check for add w/o def};
		return;
	}
x{process frag name}
```
* Bei einem öffnenden Befehl wird das passende Fragment gesucht
* Weitere Bytes können zu diesem Fragment hinzugefügt werden

```
a{process frag name}
	if (openCh == 'A') {
		ASSERT_MSG(! frag, "add in frag");
		FragMap *fm { frags };
		FragMap *ins { &root };
		frag = fm->find(name);
		E{check for add w/o def};
		return;
	}
x{process frag name}
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
a{process frag name}
	if (openCh == 'r') {
		ASSERT_MSG(! frag,
			"replace in frag"
		);
		frag = &(inputs.cur()->frags[
			name
		]);
		ASSERT_MSG(frag, "frag " <<
			name << " not defined"
		);
		frag->clear();
		return;
	}
x{process frag name}
```
* Bei einem `@replace` wird der Inhalt eines Fragments zurückgesetzt
* Das Fragment muss bereits vorhanden sein

```
a{process frag name}
	if (openCh == 'R') {
		ASSERT_MSG(! frag,
			"replace in frag"
		);
		frag = &frags->get(
			name, root
		);
		ASSERT_MSG(frag, "frag " <<
			name <<
			" not defined"
		);
		frag->clear();
		return;
	}
x{process frag name}
```
* Ersetzt ein global definiertes Fragment

```
a{process frag name}
	if (openCh == 'x') {
		ASSERT_MSG(frag,
			"end not in frag"
		);
		e{frag names must match};
		frag = nullptr;
		return;
	}
x{process frag name}
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
a{process frag name}
	if (openCh == 'i') {
		ASSERT_MSG(! frag,
			"include in frag"
		);
		if (! inputs.has(name)) {
			inputs.push(name);
		}
		return;
	}
x{process frag name}
```
* Wenn eine Datei eingebunden werden soll, dann wird sie geöffnet und
  auf den Stapel der offenen Dateien gelegt
* Wenn die Datei bereits geöffnet wurde, dann wird sie ignoriert

```
a{process frag name}
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
		return;
	}
x{process frag name}
```
* Bei einem `@expand` wird das Fragment gesucht und eingebunden
* Ggf. wird das Fragment dabei auch erzeugt, um später befüllt zu werden

```
a{process frag name}
	if (openCh == 'g') {
		ASSERT_MSG(frag,
			"expand not in frag"
		);
		Frag &sub = frags->get(
			name, root
		);
		E{check frag ex. count};
		sub.addExpand();
		frag->add(&sub);
		return;
	}
x{process frag name}
```

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
a{process frag name}
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
		return;
	}
x{process frag name}
```
* Mit einem `@multiple` Befehl kann ein Fragment an mehreren Stellen
  expandiert werden

```
a{process frag name}
	if (openCh == 'G') {
		ASSERT_MSG(frag,
			"multiple not in frag"
		);
		Frag &sub { frags->get(
			name, root
		) };
		E{check for prev expands};
		sub.addMultiple();
		frag->add(&sub);
		return;
	}
x{process frag name}
```

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
a{process frag name}
	if (openCh == 'p') {
		ASSERT_MSG(frag,
			"private not in frag"
		);
		e{process private frag};
		return;
	}
x{process frag name}
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
a{process frag name}
	if (openCh == 'm') {
		ASSERT_MSG(frag,
			"magic not in frag"
		);
		e{process magic frag};
		return;
	}
x{process frag name}
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
a{process open brace} {
	if (frag) {
		e{check valid names};
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
	static const std::string valids { 
		"fvsntkxeEgGpmb"
	};
	bool found {
		valids.find(
			static_cast<char>(*i)
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
a{process frag name}
	ASSERT_MSG(frag,
		"unknown frag " << name
	);
	process_chars(frag, name.begin(), name.end());
	return;
x{process frag name}
```
* Wenn kein bekannter Befehl erkannt wurde, dann ist die
  befehlsähnliche Eingabe Teil des Programms
* Und wird daher in den entsprechenden Buffer kopiert

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
* Dies wird mit einer Meldung quittiert

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
* Dies wird mit einer Meldung quittiert

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

