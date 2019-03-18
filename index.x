# HTML Extractor
* This presentation is the program **HTML Extractor** (`hx`)
* It contains all source code that is needed to build the executable
* But the code is spread over multiple slides
* To form an extreme form of Literate Programming
* that I call **Slideware-Programming** (SWP)
* Have fun!

## What is `hx`?
* `hx` is a program that parses a `x`-document and extracts source  code
  or an executable program out of it
* Think of it as a very powerful macro processor that combines, extends
  and orders small fragments of code
* But also it generates a HTML documentation like the one you are
  currently reading

## What is a `x`-document?
* `x`-documents are text documents with a markdown-like syntax
* It contains sections at different levels, paragraphs and code snippets
* The sections and code snippets are automatically formatted as slides
  of a very big slide show
* These slides can be decorated with notes

## SWP ≠ Literate Programming
* SWP should not document a finished program
* but it should document the process of creating a program instead
* After every slide you can generate the code from all the slides that
  you have read
* and without peeking in the future a runnable program must result
* So it does not contain features that are described the later slides

## A very top-down view of `hx`
* In the first code slide contains the highest view of the `hx` program
* While not very interesting, it contains a lot of commands that show
  how `hx` works

```
@Def(file: hx.cpp)
	@put(global elements)
	int main(
		int argc,
		const char **argv
	) {
		@put(main body)
	}
@End(file: hx.cpp)
```
* `hx` is written in C++ and consists of one source file `@s(hx.cpp)`
* Global elements like types, macros and functions are defined before
  the central `@f(main)` function is defined
* These elements and the body of the `@f(main)` function will be defined
  and refined in later slides

### Commands
* The first code slide has three `hx` commands: `@k(@Def)`, `@k(@put)`,
  and `@k(@End)`
* Each command starts with an ampersand (`@`), some letters and an
  argument block
* The argument block consists of an opening parenthesis, the argument
  value, and a closing parenthesis
* For now assume, that no closing parenthesis or ampersand is part of
  the argument value

### Fragments
* `hx` processes code fragments
* A fragment starts with an opening command like `@k(@Def)`
* and ends with a matching `@k(@End)` or `@k(@end)` closing command
* The **name** of the fragment is the argument value of the opening
  command
* and must be the same as the argument value of the closing command

### Defined Fragments
* Three fragments are defined on the first code page
* They have the names `@s(file: hx.cpp)`, `@s(global elements)`, and
  `@s(main body)`
* Not all `hx` commands use their argument value as fragment name
* but the three presented so far all do

### Including fragments
* A fragment can be included into another fragment with the `@k(@put)`
  command
* Of course you must not include a fragment that directly or indirectly
  includes the current fragment
* The inclusion will happen after all slides are processed
* So it is possible to include a fragment that is not defined yet

### Naming files
* Fragments whose name start with `@s(file: )` are special
* `hx` writes them into files
* the file name is the rest of the argument value after the `@s(file: )`
  prefix
* The first code slide creates one fragment that is written into the
  file named `@s(hx.cpp)`

### Generating the first source code
* If we stop here and run `hx`, the following code will be generated

```
	int main(
		int argc,
		const char **argv
	) {
	}
```
* The unknown fragments are noted in the output of `hx`
* But the file compiles without errors
* So it does not do much

### Steps in `@f(main)`
* The next code slides identify multiple steps to perform in the
  `@f(main)` function

```
@def(main body)
	#if ! NDEBUG
		@put(perform unit-tests);
	#endif
@end(main body)
```
* The unit-tests are performed at every start of the program
* unless a release version is build
* The use of `@k(@def)` instead of `@k(@Def)` is no mistake
* `@k(@Def)` defines a global fragment that is visible in all `x`-files
* `@k(@def)` defines a local fragment that is only visible in the
  current `x`-file and in all included `x`-files

```
@add(main body)
	@put(process arguments);
@end(main body)
```
* After the unit-tests `@f(main)` parses the arguments passed to it from
  the command line
* `@k(@add)` extends an existing local fragment
* The ability of macros to grow with time is borrowed from Literate
  Programming
* But is is far more important in SWP, as a slide provides only limited
  space

```
@add(main body)
	@put(read source files);
@end(main body)
```
* Next the source files are read into a fragment tree
* Here a lot of interesting things are happening but right now it will
  expand to nothing as nothing is described yet

```
@add(main body)
	@put(serialize fragments);
@end(main body)
```
* In the next step `hx` writes the content of every file fragment in its
  designated file

```
@add(main body)
	@put(write HTML file);
@end(main body)
```
* And lastly the HTML documentation is generated

```
@def(global elements)
	@put(includes);
@end(global elements)
```
* `hx` defines a global fragment for included files

## Next steps
* The following slides refer to documents that document the next steps
  of `hx`
* Follow them in order so that you do not miss important concepts

```
@inc(read.x)
```
* Defines the mechanisms of reading files line by line
* The `@k(@inc)` command includes a different `x`-file at the current
  position
* The file is read only once, no matter how often it is included
* You can click on the argument value in the HTML documentation to
  navigate to the documentation from this file

```
@inc(blocks.x)
```

# WORKING HERE

```
@inc(log.x)
```
* Defines logging mechanism

# Fragmente
* Fragmenten können während des Parsens erweitert, ersetzt und
  angewendet werden
* Ein Haupt-Vorteil von `hx` gegenüber anderen Makro-Präprozessoren
  ist  die Möglichkeit, Fragmente vor der Definition zu verwenden
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
@Def(needed by read_sources)
	int blockLimit = -1;
@End(needed by read_sources)
```
* Die Anzahl der Blocks, die ausgegeben werden sollen, kann mit  diesem
  Parameter limitiert werden

```
@def(process arguments)
	bool someFile { false };
	for (int i { 1 }; i < argc; ++i) {
		std::string arg { argv[i] };
		@put(process argument);
		@put(process file argument);
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
	if (arg.substr(
		0, prefix.length()
	) == prefix) {
		blockLimit = std::stoi(
			arg.substr(prefix.length())
		);
		continue;
	}
} @end(process argument)
```
* Der Benutzer kann die Anzahl der Folien beschränken, aus denen der
  Code generiert wird
* So können Teilabschnitte validiert werden

```
@def(process file argument)
	if (! someFile) {
		inputs.push(argv[1]);
		someFile = true;
		continue;
	}
@end(process file argument)
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
@Add(needed by read_sources)
	using SI =
		std::string::const_iterator;
@end(needed by read_sources)
```
* Die Anwendung verwendet den String-Iterator an vielen Stellen
* Daher definiert sie eine Abkürzung, damit die Folien nicht
  überlaufen

```
@Add(needed by read_sources)
	void process_chars(
		Frag *frag, SI i, SI e
	) {
		@put(process chars);
	}
@End(needed by read_sources)
```
* Fügt ein Range an den Inhalt von `frag` an

```
@Add(needed by read_sources)
	void process_char(Frag *frag, char ch) {
		@put(process char);
	}
@End(needed by read_sources)
```
* Fügt ein Zeichen an den Inhalt von `frag` an

```
@Add(process line)
	auto end = line.cend();
	for (
		auto i = line.cbegin();
		i != end; ++i
	) {
		@put(process special lines);
		process_chars(frag, i, i + 1);
	}
	process_char(frag, '\n');
@End(process line)
```
* Neben dem aktuellen Zeichen wird auch das letzte Zeichen aufgehoben
* Dabei kann `hx` auch mit einer leeren Eingabe-Datei umgehen (wenn
  schon das erste Zeichen ein `EOF` ist)

```
@Def(additional read vars)
	Frag *frag { nullptr };
@End(additional read vars)
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
			str, inputs.cur().input().path(),
			inputs.cur().line()
		);
	}
@end(process chars)
```
* Fügt mehrere Zeichen an das aktuelle Fragment an

```
@def(process char)
	if (frag) {
		frag->add(
			ch, inputs.cur().input().path(),
			inputs.cur().line()
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
	bool outside = !frag;
	do {
		if (! frag && ! blockLimit) { break; }
		@put(do macro);
		@put(default expansion);
	} while (false);
	if (blockLimit && outside && frag) {
		--blockLimit;
	}
@end(macro found)
```
* Besondere Makros werden zuerst ausgewertet
* Wenn diese Auswertung nicht greift, wird die Standard-Expandierung
  verwendet

```
@Add(needed by read_sources)
	void expand_macro_arg(
		Frag *f, const std::string &arg
	) {
		auto b = arg.begin();
		auto e = arg.end();
		@put(expand loop);
	}
@End(needed by read_sources)
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
					inputs.cur().input().path(),
					inputs.cur().line()
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
		inputs.cur().input().path(),
		inputs.cur().line()
	);
@end(expand inner)
```

```
@def(expand rest)
	f->add(
		std::string { b, e },
		inputs.cur().input().path(),
		inputs.cur().line()
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
		ASSERT_MSG(! frag, @s("@@def(") << arg << ") in frag [" << frag->name << ']');
		frag = inputs.get_local(arg);
		if (isPopulatedFrag(frag)) {
			std::cerr << "frag [" << arg << "] already defined\n";
		}
		break;
	}
@end(do macro)
```
* Erzeugt ein neues Fragment
* Das Fragment darf nicht mehrfach definiert werden

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
		ASSERT_MSG(! frag, "add in frag " << frag->name);
		frag = inputs.get_local(arg);
		if (! isPopulatedFrag(frag)) {
			std::cerr << "frag [" << arg << "] not defined\n";
		}
		break;
	}
@end(do macro)
```
* Bei einem öffnenden Befehl wird das passende Fragment gesucht
* Weitere Bytes können zu diesem Fragment hinzugefügt werden

```
@add(do macro)
	if (name == "put") {
		ASSERT_MSG(frag,
			"@put not in frag"
		);
		Frag *sub = inputs.get_local(arg);
		if (sub) {
			@mul(check frag ex. count);
			sub->addExpand();
			frag->add(sub);
		}
		break;
	}
@end(do macro)
```
* Bei einem `@put` wird das Fragment gesucht und eingebunden
* Ggf. wird das Fragment dabei auch erzeugt, um später befüllt zu
  werden

```
@def(check frag ex. count)
	if (sub->expands()) {
		std::cerr <<
			"multiple expands of [" <<
			sub->name << "]\n";
	}
	if (sub->multiples()) {
		std::cerr <<
			"expand after mult of ["
			<< sub->name << "]\n";
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
			"include in frag [" << frag->name << ']'
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
			"@mul not in frag"
		);
		Frag *sub = inputs.get_local(arg);
		if (sub) {
			@mul(check for prev expands);
			sub->addMultiple();
			frag->add(sub);
		}
		break;
	}
@end(do macro)
```
* Mit einem `@mul` Befehl kann ein Fragment an mehreren Stellen
  expandiert werden

```
@def(check for prev expands)
	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}
@end(check for prev expands)
```
* Es ist ein Fehler, wenn das Fragment bereits mit `@put` eingebunden
  wurde

```
@add(do macro)
	if (name == "Def") {
		ASSERT_MSG(! frag, "@Def in frag [" << frag->name << ']');
		frag = inputs.get_global(arg);
		if (isPopulatedFrag(frag)) {
			std::cerr << "Frag [" << arg << "] already defined\n";
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
		ASSERT_MSG(! frag, "@Add in frag [" << frag->name << ']');
		frag = inputs.get_global(arg);
		if (! isPopulatedFrag(frag)) {
			std::cerr << "Frag [" << arg << "] not defined\n";
		}
		break;
	}
@end(do macro)
```
* Erweitert ein global definiertes Fragment

```
@add(do macro)
	if (name == "rep") {
		ASSERT_MSG(! frag,
			"@rep in frag [" << frag->name << ']'
		);
		frag = inputs.get_local(arg);
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
			"@Rep in frag [" << frag->name << ']'
		);
		frag = inputs.get_global(arg);
		@mul(clear frag);
		break;
	}
@end(do macro)
```
* Bei einem `@rep` wird der Inhalt eines Fragments zurückgesetzt
* Das Fragment muss bereits vorhanden sein

```
@def(clear frag)
	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();
@end(clear frag)
```
* Löscht das aktuelle Fragment

```
@add(do macro)
	if (name == "Put") {
		ASSERT_MSG(frag,
			"@Put not in frag"
		);
		Frag *sub = inputs.get_global(arg);
		if (sub) {
			@mul(check frag ex. count);
			sub->addExpand();
			frag->add(sub);
		}
		break;
	}
@end(do macro)
```
* Bei einem `@Put` wird das Fragment in den umschließenden  `FragMap`s
  gesucht

```
@add(do macro)
	if (name == "Mul") {
		ASSERT_MSG(frag,
			"@Mul not in frag"
		);
		Frag *sub = inputs.get_global(arg);
		if (sub) {
			@mul(check for prev expands);
			sub->addMultiple();
			frag->add(sub);
		}
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
		@put(process private frag);
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
@def(process private frag)
	std::hash<std::string> h;
	unsigned cur {
		h(inputs.cur().input().path() +
			':' + arg) &
				0x7fffffff
	};
@end(process private frag)
```
* Der Hash wird aus dem aktuellen Dateinamen
* Und dem aktuellen Bezeichner berechnet
* Zum Schluss wird er auf eine positive Zahl maskiert

```
@add(process private frag)
	std::ostringstream hashed;
	hashed << "_private_" <<
		cur << '_' <<
		name;
	frag->add(
		hashed.str(),
		inputs.cur().input().path(),
		inputs.cur().line()
	);
@end(process private frag)
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
		@put(process magic frag);
		break;
	}
@end(do macro)
```
* Der `@magic`-Befehl erzeugt einen Hash-Wert
* Der sich aus dem Dateinamen und dem Argument des Befehls  zusammen
  setzt

```
@def(process magic frag)
	std::hash<std::string> h;
	unsigned cur {
		h(inputs.cur().input().path() +
			':' + arg) &
				0x7fffffff
	};
@end(process magic frag)
```
* Berechnet Hash-Wert

```
@add(process magic frag)
	std::ostringstream value;
	value << cur;
	frag->add(
		value.str(),
		inputs.cur().input().path(),
		inputs.cur().line()
	);
@end(process magic frag)
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
* Zusätzlich wird geprüft, ob Fragmente zu selten oder zu oft
  expandiert  wurden

```
@add(serialize fragments)
	for (auto &j : inputs) {
		for (auto &i : j.frags) {
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
		for (auto &i : j.frags) {
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

```
@inc(view.x)
```
* Interactive display of slides

```
@inc(line.x)
```

```
@inc(edit.x)
```
* Edit slides in place

```
@inc(range.x)
```

```
@inc(write.x)
```

```
@inc(add.x)
```
