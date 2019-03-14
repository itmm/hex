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
@Def(file: hx.cpp)
	@put(global elements)
	int main(
		int argc,
		const char **argv
	) {
		@put(main body)
	}
@end(file: hx.cpp)
```
* Hex ist in C++ geschrieben
* Das Hauptprogramm besteht aus der `main`-Funktion
* Zusätzlich wird ein Fragment definiert, in welchem globale Elemente
  definiert werden können
* Übergeben wird der Funktion die Anzahl der Argumente in `argc`
* Und der Aufrufname mit Argumenten als Array in `argv`

```
@def(main body)
	@put(perform unit-tests);
	@put(process arguments);
	@put(read source file);
	@put(serialize fragments);
	@put(write HTML file);
@end(main body)
```
* Bei jedem Start werden alle Unit-Tests ausgeführt (um eine
  umfangreiche Testabdeckung zu sichern)
* Parameter von der Kommandozeile werden ausgewertet
* Dann wird ein Parse-Graph aus Fragmenten aufgebaut
* Und das daraus resultierende Programm generiert und übersetzt
* Zum Schluss wird die HTML-Präsentation der Seiten in einem zweiten
  Durchgang herausgeschrieben

## Was macht `@put`?
* `@put`-Blöcke beschreiben Fragment-Aufrufe
* Der Wert des Fragments mit dem Namen in Klammern wird anstelle des
  Aufrufs im endgültigen Programm gesetzt
* Diese Fragmente bilden ein zentrales Element von `hx`
* Sie können mit `@def`-`@end`-Sequenzen definiert werden
* Oder mit `@add`-`@end` erweitert werden
* Ein `@put` darf nur einmal aufgelöst werden
* Für mehrfache Auflösungen muss `@mulitple` verwendet werden

```
@def(global elements)
	@put(includes);
	@put(define logging);
@end(global elements)
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
@inc(log.x)
```
* Bindet Logging ein

# Fragmente
* Fragmenten können während des Parsens erweitert, ersetzt und
  angewendet werden
* Ein Haupt-Vorteil von `hx` gegenüber anderen Makro-Präprozessoren ist
  die Möglichkeit, Fragmente vor der Definition zu verwenden
* Und Fragment an mehreren Stellen zu erweitern

```
@inc(frag.x)
```
* Fragment-Behandlung wird in einer eigenen Datei definiert

# Input
* Eine Klasse für Eingabe-Dateien enthält neben der offenen Datei noch
  die aktuelle Zeilennummer oder verweise auf offene inkludierte Dateien
* Die Definition der Klasse mit allen Methoden ist in einer eigenen
  Datei ausgelagert

```
@inc(input.x)
```
* Bindet Datei-Klasse ein

# Kommandozeile
* Die Kommandozeile wird Element für Element abgearbeitet

```
@add(global elements)
	std::string stylesheet {
		"slides/slides.css"
	};
@end(global elements)
```
* Für die HTML-Ausgabe wird eine Stylesheet-Datei benötigt
* Über die Kommandozeile kann eine alternative Datei angegeben werden

```
@add(global elements)
	int blockLimit = -1;
@end(global elements)
```
* Die Anzahl der Blocks, die ausgegeben werden sollen, kann mit
  diesem Parameter limitiert werden

```
@add(global elements)
	Inputs inputs;
@end(global elements)
```
* `inputs` enthält neben der gerade offenen Datei auch alle Dateien, die
  noch prozessiert werden müssen
* Und alle bereits gelesenen Dateien

```
@def(process arguments)
	bool someFile { false };
	for (int i { 1 }; i < argc; ++i) {
		@put(process argument);
		ASSERT_MSG(false,
			"unknown argument [" <<
			argv[i] << ']'
		);
	}
@end(process arguments)
```
* Die Argumente werden einzeln durchgegangen
* Wenn sie nicht verwendet werden, bricht das Programm ab

```
@def(process argument) {
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
} @end(process argument)
```
* Der Pfad zur Stylesheet-Datei kann über die Kommandozeile gesetzt
  werden

```
@add(process argument) {
	static const std::string prefix {
		"--limit="
	};
	std::string arg { argv[i] };
	if (arg.substr(
		0, prefix.length()
	) == prefix) {
		@put(extract block limit);
	}
} @end(process argument)
```
* Der Benutzer kann die Anzahl der Folien beschränken, aus denen der
  Code generiert wird
* So können Teilabschnitte validiert werden

```
@def(extract block limit)
	std::istringstream iss {
		arg.substr(prefix.length())
	};
	iss >> blockLimit;
	continue;
@end(extract block limit)
```
* Das Programm legt das Argument in einen Input-Stream
* Und liest aus diesem eine Zahl

```
@add(process argument)
	if (! someFile) {
		inputs.push(argv[1]);
		someFile = true;
		continue;
	}
@end(process argument)
```
* Ansonsten wird das Argument als Pfad der `.x`-Datei interpretiert
* Aus dieser werden HTML-Slides und Source-Code generiert
* Es kann nur eine Datei angegeben werden

```
@add(process arguments)
	if (! someFile) {
		inputs.push("index.x");
	}
@end(process arguments)
```
* Wenn kein Pfad angegeben wurde, wird `index.x` als Vorgabe verwendet

# Eingabe-Dateien lesen
* In diesem Abschnitt werden die Eingabe-Dateien gelesen, um die
  Fragmente aufzubauen und alle notwendigen Beziehungen zwischen den
  einzelnen Folien zu finden

```
@def(read source file) {
	@put(additional read vars);
	std::string line;
	while (inputs.getLine(line)) {
		@put(process line);
	}
} @end(read source file)
```
* `hx` liest die Eingabe-Dateien zeilenweise
* Inkludierungen werden transparent in `inputs` behandelt

```
@add(global elements)
	using SI =
		std::string::const_iterator;
@end(global elements)
```
* Die Anwendung verwendet den String-Iterator an vielen Stellen
* Daher definiert sie eine Abkürzung, damit die Folien nicht überlaufen

```
@add(global elements)
	void process_chars(
		Frag *frag, SI i, SI e
	) {
		@put(process chars);
	}
@end(global elements)
```
* Fügt ein Range an den Inhalt von `frag` an

```
@add(global elements)
	void process_char(Frag *frag, char ch) {
		@put(process char);
	}
@end(global elements)
```
* Fügt ein Zeichen an den Inhalt von `frag` an

```
@def(process line)
	auto end = line.cend();
	for (
		auto i = line.cbegin();
		i != end; ++i
	) {
		@put(process special lines);
		process_chars(frag, i, i + 1);
	}
	process_char(frag, '\n');
@end(process line)
```
* Neben dem aktuellen Zeichen wird auch das letzte Zeichen aufgehoben
* Dabei kann `hx` auch mit einer leeren Eingabe-Datei umgehen (wenn
  schon das erste Zeichen ein `EOF` ist)


```
@def(additional read vars)
	Frag *frag { nullptr };
@end(additional read vars)
```
* Wir unterscheiden, ob wir in einem Code-Block sind, oder außerhalb
* In einem Code sind wir sogar in einem Fragment, dessen Inhalt gerade
  gelesen wird
* Am Anfang sind wir außerhalb eines Code-Blocks
* In einem Code-Block ist `frag` nicht `nullptr`

```
@def(process chars)
	if (frag) {
		std::string str {i, e};
		frag->add(
			str, inputs.cur()->name,
			inputs.cur()->line()
		);
	}
@end(process chars)
```
* Fügt mehrere Zeichen an das aktuelle Fragment an

```
@def(process char)
	if (frag) {
		frag->add(
			ch, inputs.cur()->name,
			inputs.cur()->line()
		);
	}
@end(process char)
```
* Fügt einzelnes Zeichen an das aktuelle Fragment an

```
@add(includes)
	#include <algorithm>
@end(includes)
```
* Wird für find benötigt

```
@def(process special lines)
	if (*i == '@') {
		auto nb = i + 1;
		auto ne = nb;
		while (ne != end && *ne != '(') {
			if (! isalpha(*ne)) {
				ne = end;
				break;
			}
			++ne;
		}
		if (ne != end && ne != nb) {
			std::string name {nb, ne};
			@put(macro argument);
		}
	}
@end(process special lines)
```
* Makros können mit dem Ampersand eingeleitet werden
* Danach kommt der Name des Makros

```
@def(macro argument)
	auto ab = ne + 1;
	auto ae = ab;
	while (ae != end && *ae != ')') {
		if (*ae == '@') {
			if (++ae == end) { break; }
		}
		++ae;
	}
	if (ae != end) {
		std::string arg {ab, ae};
		@put(macro found);
		continue;
	}
@end(macro argument)
```
* Argument des Makros wird von Klammern umschlossen

```
@def(macro found)
	i = ae;
	do {
		@put(do macro);
		@put(default expansion);
	} while (false);
@end(macro found)
```
* Besondere Makros werden zuerst ausgewertet
* Wenn diese Auswertung nicht greift, wird die Standard-Expandierung
  verwendet


```
@add(global elements)
	void expand_macro_arg(
		Frag *f, const std::string &arg
	) {
		auto b = arg.begin();
		auto e = arg.end();
		@put(expand loop);
	}
@end(global elements)
```

```
@def(expand loop)
	while (b != e) {
		auto x = std::find(b, e, '@');
		if (x != e) {
			@put(expand inner);
			b = x + 1;
			if (b != e) {
				f->add(
					*b,
					inputs.cur()->name,
					inputs.cur()->line()
				);
				++b;
			}
		} else {
			@put(expand rest);
			b = e;
		}
	}
@end(expand loop)
```

```
@def(expand inner)
	f->add(
		std::string { b, x },
		inputs.cur()->name,
		inputs.cur()->line()
	);
@end(expand inner)
```

```
@def(expand rest)
	f->add(
		std::string { b, e },
		inputs.cur()->name,
		inputs.cur()->line()
	);
	b = e;
@end(expand rest)
```

```
@def(default expansion)
	if (frag) {
		expand_macro_arg(frag, arg);
	}
@end(default expansion)
```
* Wenn das Makro nicht behandelt wurde, dann muss es sich um eine
  Formatierung handeln, die nur in Fragmenten vorkommen darf
* Es wird einfach nur das Argument ausgegeben

```
@def(do macro)
	if (name == "def") {
		ASSERT_MSG(! frag, "def(" << arg << ") in frag");
		FragMap *fm {
			&inputs.cur()->frags
		};
		@mul(check for double def);
		if (! frag) {
			frag = &(*fm)[arg];
		}
		break;
	}
@end(do macro)
```
* Erzeugt ein neues Fragment
* Das Fragment darf nicht mehrfach definiert werden

```
@def(check for double def)
	frag = fm->find(arg);
	if (isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			arg <<
			"] already defined\n";
	}
@end(check for double def)
```
* Wenn das Fragment bereits existiert, wird es vielleicht nur verwendet
* Es muss geprüft werden, ob es schon Inhalt hat
* Das wäre dann eine Fehlermeldung wert
* Bricht aber die Abarbeitung nicht ab

```
@add(do macro)
	if (name == "end" || name == "End") {
		ASSERT_MSG(frag,
			'@' << name << "(" << arg <<
			") not in frag"
		);
		@put(frag names must match);
		frag = nullptr;
		break;
	}
@end(do macro)
```
* Bei einem schließenden Befehl wird das aktuelle Fragment unterbrochen

```
@def(frag names must match)
	ASSERT_MSG(frag->name == arg,
		"closing [" << arg <<
		"] != [" << frag->name << ']'
	);
@end(frag names must match)
```
* Wenn der öffnende und schließende Name nicht passt, wird die
  Abarbeitung abgebrochen

```
@add(do macro)
	if (name == "add") {
		ASSERT_MSG(! frag, "add in frag");
		FragMap *fm {
			&inputs.cur()->frags
		};
		FragMap *ins { fm };
		frag = fm->find(arg);
		@mul(check for add w/o def);
		break;
	}
@end(do macro)
```
* Bei einem öffnenden Befehl wird das passende Fragment gesucht
* Weitere Bytes können zu diesem Fragment hinzugefügt werden

```
@def(check for add w/o def)
	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			arg <<
			"] not defined\n";
		frag = &fm->get(
			arg, *ins
		);
	}
@end(check for add w/o def)
```
* Das Fragment muss bereits vorhanden und nicht leer sein

```
@add(do macro)
	if (name == "put") {
		ASSERT_MSG(frag,
			"expand not in frag"
		);
		Frag &sub = inputs.cur()->frags[
			arg
		];
		@mul(check frag ex. count);
		sub.addExpand();
		frag->add(&sub);
		break;
	}
@end(do macro)
```
* Bei einem `@put` wird das Fragment gesucht und eingebunden
* Ggf. wird das Fragment dabei auch erzeugt, um später befüllt zu werden

```
@def(check frag ex. count)
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
@end(check frag ex. count)
```
* Wenn das Fragment bereits expandiert wurde, dann wird eine Meldung
  ausgegeben
* Wenn das Fragment bereits im Mehrfach-Modus ausgegeben wurde, wird
  ebenfalls eine Meldung ausgegeben

```
@add(do macro)
	if (name == "inc") {
		ASSERT_MSG(! frag,
			"include in frag"
		);
		if (! inputs.has(arg)) {
			inputs.push(arg);
		}
		break;
	}
@end(do macro)
```
* Wenn eine Datei eingebunden werden soll, dann wird sie geöffnet und
  auf den Stapel der offenen Dateien gelegt
* Wenn die Datei bereits geöffnet wurde, dann wird sie ignoriert

```
@add(do macro)
	if (name == "mul") {
		ASSERT_MSG(frag,
			"mul not in frag"
		);
		Frag &sub { inputs.cur()->frags[
			arg
		] };
		@mul(check for prev expands);
		sub.addMultiple();
		frag->add(&sub);
		break;
	}
@end(do macro)
```
* Mit einem `@mul` Befehl kann ein Fragment an mehreren Stellen
  expandiert werden

```
@def(check for prev expands)
	if (sub.expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub.name << "]\n";
	}
@end(check for prev expands)
```
* Es ist ein Fehler, wenn das Fragment bereits mit `@put` eingebunden
  wurde

```
@add(do macro)
	if (name == "Def") {
		ASSERT_MSG(! frag, "Def in frag");
		FragMap *fm { frags };
		@mul(check for double def);
		if (! frag) {
			frag = &root[arg];
		}
		break;
	}
@end(do macro)
```
* Erzeugt ein neues Fragment im globalen Namensraum
* Das Fragment darf nicht mehrfach definiert werden

```
@add(do macro)
	if (name == "Add") {
		ASSERT_MSG(! frag, "Add in frag");
		FragMap *fm { frags };
		FragMap *ins { &root };
		frag = fm->find(arg);
		@mul(check for add w/o def);
		break;
	}
@end(do macro)
```
* Erweitert ein global definiertes Fragment

```
@add(do macro)
	if (name == "rep") {
		ASSERT_MSG(! frag,
			"rep in frag"
		);
		frag = &(inputs.cur()->frags[
			arg
		]);
		@mul(clear frag);
		break;
	}
@end(do macro)
```
* Bei einem `@rep` wird der Inhalt eines Fragments zurückgesetzt
* Das Fragment muss bereits vorhanden sein

```
@add(do macro)
	if (name == "Rep") {
		ASSERT_MSG(! frag,
			"Rep in frag"
		);
		FragMap *fm { frags };
		frag = fm->find(arg);
		@mul(clear frag);
		break;
	}
@end(do macro)
```
* Bei einem `@rep` wird der Inhalt eines Fragments zurückgesetzt
* Das Fragment muss bereits vorhanden sein

```
@def(clear frag)
	ASSERT_MSG(frag, "frag " <<
		name <<
		" not defined"
	);
	frag->clear();
@end(clear frag)
```
* Löscht das aktuelle Fragment

```
@add(do macro)
	if (name == "rep") {
		ASSERT_MSG(! frag,
			"replace in frag"
		);
		frag = &frags->get(
			arg, root
		);
		@mul(clear frag);
		break;
	}
@end(do macro)
```
* Ersetzt ein global definiertes Fragment

```
@add(do macro)
	if (name == "Put") {
		ASSERT_MSG(frag,
			"Put not in frag"
		);
		Frag &sub = frags->get(
			arg, root
		);
		@mul(check frag ex. count);
		sub.addExpand();
		frag->add(&sub);
		break;
	}
@end(do macro)
```
* Bei einem `@Put` wird das Fragment in den umschließenden
  `FragMap`s gesucht

```
@add(do macro)
	if (name == "Mul") {
		ASSERT_MSG(frag,
			"globmult not in frag"
		);
		Frag &sub { frags->get(
			arg, root
		) };
		@mul(check for prev expands);
		sub.addMultiple();
		frag->add(&sub);
		break;
	}
@end(do macro)
```
* `@Mul` expandiert ein globales Fragment an mehreren Stellen

```
@add(do macro)
	if (name == "priv") {
		ASSERT_MSG(frag,
			"@priv not in frag"
		);
		@put(process private frag 2);
		break;
	}
@end(do macro)
```
* Private Bezeichner werden durch einen Hash erweitert
* Um sie global unique zu machen

```
@add(includes)
	#include <functional>
	#include <sstream>
@end(includes)
```
* Enthält Hash-Funktion

```
@def(process private frag 2)
	std::hash<std::string> h;
	unsigned cur {
		h(inputs.cur()->name +
			':' + arg) &
				0x7fffffff
	};
@end(process private frag 2)
```
* Der Hash wird aus dem aktuellen Dateinamen
* Und dem aktuellen Bezeichner berechnet
* Zum Schluss wird er auf eine positive Zahl maskiert

```
@add(process private frag 2)
	std::ostringstream hashed;
	hashed << "_private_" <<
		cur << '_' <<
		name;
	frag->add(
		hashed.str(),
		inputs.cur()->name,
		inputs.cur()->line()
	);
@end(process private frag 2)
```
* Zuerst werden eventuell zwischengespeicherte Zeichen ausgegeben
* Dann kommt der neue Bezeichner
* Dieser besteht aus einem konstanten Präfix
* Dem Hash-Wert
* Und dem alten Bezeichner

```
@add(do macro)
	if (name == "magic") {
		ASSERT_MSG(frag,
			"@magic not in frag"
		);
		@put(process magic frag 2);
		break;
	}
@end(do macro)
```
* Der `@magic`-Befehl erzeugt einen Hash-Wert
* Der sich aus dem Dateinamen und dem Argument des Befehls
  zusammen setzt

```
@def(process magic frag 2)
	std::hash<std::string> h;
	unsigned cur {
		h(inputs.cur()->name +
			':' + arg) &
				0x7fffffff
	};
@end(process magic frag 2)
```
* Berechnet Hash-Wert

```
@add(process magic frag 2)
	std::ostringstream value;
	value << cur;
	frag->add(
		value.str(),
		inputs.cur()->name,
		inputs.cur()->line()
	);
@end(process magic frag 2)
```
* Gibt den Hash-Wert aus
* Vorher wird noch eventuell gespeicherte Zeichen ausgegeben

# Fragmente serialisieren
* Fragmente, die Dateien spezifizieren werden in diese Dateien
  rausgeschrieben

```
@def(serialize fragments)
	for (auto &i : root) {
		const Frag *frag { &i.second };
		@mul(serialize frag);
	}
@end(serialize fragments)
```
* Fragmente, die mit `file:` beginnen, werden in die entsprechenden
  Dateien rausgeschrieben
* Zusätzlich wird geprüft, ob Fragmente zu selten oder zu oft expandiert
  wurden

```
@add(serialize fragments)
	for (auto &j : inputs) {
		for (auto &i : j->frags) {
			const Frag *frag {
				&i.second
			};
			@mul(serialize frag);
		}
	}
@end(serialize fragments)
```
* Auch alle lokalen Fragmente bearbeiten

```
@def(serialize frag) {
	if (frag->isFile()) {
		@put(write in file);
	}
} @end(serialize frag)
```
* Wenn der Name eines Fragments mit `file: ` beginnt, dann wird es in
  die passende Datei geschrieben
* Zusätzlich zählt das als eine Expansion

```
@add(serialize frag) {
	int sum {
		frag->expands()
			+ frag->multiples()
	};
	if (sum <= 0) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not called\n";
	}
} @end(serialize frag)
```
* Ein Fragment wurde nicht aufgerufen
* Dies wird mit einer Meldung protokolliert

```
@add(serialize frag)
	if (frag->multiples() == 1) {
		std::cerr <<
			"multiple frag [" <<
			frag->name <<
			"] only used once\n";
	}
@end(serialize frag)
```
* Ein Fragment das zur mehrfachen Verwendung deklariert wurde, wird nur
  einmal verwendet
* Dies wird mit einer Meldung protokolliert

```
@add(serialize frag)
	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not populated\n";
	}
@end(serialize frag)
```
* Für jedes Fragment, das nicht befüllt wurde wird eine Meldung
  ausgegeben

```
@def(write in file)
	std::ofstream out(
		frag->name.substr(6).c_str()
	);
	serializeFrag(*frag, out, false);
	out.close();
@end(write in file)
```
* Das Fragment wird in die entsprechende Datei geschrieben

```
@add(serialize fragments)
	for (auto &i : root) {
		const Frag *frag { &i.second };
		@mul(serialize cmd);
	}
@end(serialize fragments)
```

```
@add(serialize fragments)
	for (auto &j : inputs) {
		for (auto &i : j->frags) {
			const Frag *frag {
				&i.second
			};
			@mul(serialize cmd);
		}
	}
@end(serialize fragments)
```

```
@def(serialize cmd) {
	const std::string cmd { frag->cmd() };
	if (cmd.size()) {
		@put(write cmd in file);
	}
} @end(serialize cmd)
```

```
@def(write cmd in file)
	std::ostringstream out {};
	serializeFrag(*frag, out, false);
	std::string o { out.str() };
	std::FILE *f {
		popen(cmd.c_str(), "w")
	};
	if (f) {
		std::fwrite(o.c_str(), o.size(), 1, f);
		pclose(f);
	}
@end(write cmd in file)
```

```
@inc(html.x)
```
* Die Generierung liegt in einer eigenen Datei

