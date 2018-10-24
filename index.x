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
* Dieses lehnt an Markdown an und hat die Datei-Endung `.hx`
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
* Wenn nicht definierte Makros zu nichts expandieren
* So kann das Verständnis für ein Programm schrittweise erarbeitet werden

# Definition des Ablaufs
* Zuerst wird das Haupt-Programm in ganz groben Pinselstrichen skizziert
* Nach und nach werden die einzelnen Elemente mit Leben gefüllt

```
d{file: hx.c}
	e{global elements}
	t{int} f{main}(
		t{int} v{argc}, t{const char **}v{argv}
	) {
		e{main body}
	}
x{file: hx.c}
```
* Das Hauptprogramm besteht aus der `f{main}`-Funktion
* Zusätzlich wird ein Fragment definiert, in welchem globale Elemente
  definiert werden können

```
d{main body}
	e{perform unit-tests};
	e{process arguments};
	e{read source file};
	e{serialize fragments};
	e{compile program};
	e{write HTML file};
x{main body}
```
* Bei jedem Start werden alle Unit-Tests ausgeführt (um eine umfangreiche
  Testabdeckung zu sichern)
* Parameter von der Kommandozeile werden ausgewertet
* Dann wird ein Parse-Graph aus Fragmenten aufgebaut
* Und das daraus resultierende Programm generiert und übersetzt
* Zum Schluss wird die HTML-Präsentation der Seiten in einem zweiten
  Durchgang herausgeschrieben

## Was macht `@expand`?
* `@expand`-Blöcke beschreiben Makro-Aufrufe
* Der Wert des Makros mit dem Namen in Klammern wird anstelle des Aufrufs
  im endgültigen Programm gesetzt
* Diese Makros bilden ein zentrales Element von `hx`
* Sie können mit `@add`-`@end`-Sequenzen definiert oder erweitert werden

```
d{global elements}
	e{includes};
	e{define logging};
	e{forward declarations};
x{global elements}
```
* System-Dateien werden vor der Definition von Strukturen und Funktionen
  eingebunden
* Auch müssen Macros für das Logging vor den Funktionen definiert werden,
  die sie verwenden


# Minimale Vorbereitung für das Parsen
* In diesem Abschnitt wird die Grundlage gelegt, um Dateien lesen zu
  können
* Während des Lesens kann die aktuelle Datei unterbrochen und zuerst
  eine weitere Datei gelesen werden

```
d{includes}
	#include <stdio.h>
	#include <stdlib.h>
x{includes}
```
* Standard File-Funktionen werden vom Programm benötigt
* Standard Bibliothek wird für dynamische Speicherverwaltung benötigt

```
a{global elements}
	t{struct Input} {
		t{struct Input *}v{link};
		t{FILE *}v{file};
	};

	t{struct Input *}v{input} = k{NULL};
x{global elements}
```
* Es gibt immer eine aktuelle Datei, die gerade gelesen wird
* Mitten während des Lesens können andere Dateien eingelesen (inkludiert)
  werden
* Daher gibt es einen Stapel offener Dateien
* Aus der letzten wird gelesen

```
a{global elements}
	t{void} f{pushFile} (t{FILE *}v{file}) {
		t{struct Input *}v{i} =
			f{malloc}(f{sizeof}(t{struct Input}));

		e{check memory for input};
		v{i}->v{link} = v{input};
		v{i}->v{file} = v{file};
		v{input} = v{i};
	}
x{global elements}
```
* Eine offene Datei (z.B. die Standard-Eingabe) kann direkt als
  Eingabe-Strom verwendet werden

```
a{global elements}
	t{void} f{pushPath}(t{const char *}v{path}) {
		t{FILE *}v{f} = f{fopen}(v{path}, s{"r"});
		e{check file for path};
		f{pushFile}(v{f});
	}
x{global elements}
```
* Eine Datei, die über einen Pfad beschrieben ist, kann auch als
  Eingabe-Strom verwendet werden
* Sie muss nur vorher geöffnet werden

```
d{process arguments}
	k{if}(v{argc} > n{1}) {
		f{pushPath}(v{argv}[n{1}]);
	} k{else} {
		f{pushPath}(s{"index.x"});
	}
x{process arguments}
```
* Der Source-Code für die Slideware muss aus einer Datei geladen werden
* Der Name kann über die Kommandozeile gesetzt werden
* Ansonsten wird `index.x` als Vorgabe verwendet

```
a{global elements}
	t{int} f{nextCh}() {
		t{int} v{ch} = k{EOF};
		k{while} (v{input}) {
			v{ch} = f{fgetc}(v{input}->v{file});
			k{if} (v{ch} != k{EOF}) { k{break}; }
			t{struct Input *}v{n} = v{input}->v{link};
			f{fclose}(v{input}->v{file});
			f{free}(v{input});
			v{input} = v{n};
		}
		k{return} v{ch};
	}
x{global elements}
```

# Buffer
* Einfache Implementierung in C eines Byte-Vektors
* Der dynamisch wachsen kann

```
i{buffer.x}
```
* Buffer werden in einer eigenen Datei definiert

# Consumer
* Schnittstelle um Bytes zu konsumieren

```
i{consumer.x}
```
* Consumer werden in einer eigenen Datei definiert

# Makros
* Makros können während des Parsens erweitert, ersetzt und angewendet
  werden
* Ein Haupt-Vorteil von `hx` gegenüber anderen Makro-Präprozessoren ist
  die Möglichkeit, Makros vor der Definition zu verwenden
* Und Makros an mehreren Stellen zu erweitern

```
i{macros.x}
```
* Makros werden in einer eigenen Datei definiert

# Eingabe-Dateien lesen
* In diesem Abschnitt werden die Eingabe-Dateien gelesen, um die Makros
  aufzubauen und alle notwendigen Beziehungen zwischen den einzelnen
  Folien zu finden

```
d{read source file}
	e{global source vars};
	{
		e{additional read vars};
		t{int} v{last} = f{nextCh}();
		t{int} v{ch} = f{nextCh}();
		k{while} (v{ch} != v{EOF}) {
			e{process current char};
			v{last} = v{ch}; v{ch} = f{nextCh}();
		}
	}
x{read source file}
```
* Neben dem aktuellen Zeichen wird auch das letzte Zeichen aufgehoben

```
d{process current char}
	k{switch} (v{ch}) {
		k{case} '{':
			e{process open brace};
			k{break};
		k{case} '}': {
			t{bool} v{processed} = k{false};
			e{process close brace};
			k{break};
		}
		k{default}:
			e{process other char};
	}
x{process current char}
```
* Beim Parsen kommt es nur auf das Öffnen und Schließen von
  Mengenklammern an

```
d{additional read vars}
	t{struct Macro *} v{macro} = k{NULL};
	t{struct Buffer} v{buffer} = {};
x{additional read vars}
```
* Wir unterscheiden, ob wir in einem Code-Block sind, oder außerhalb
* In einem Code sind wir sogar in einem Makro, dessen Inhalt gelesen wird
* Am Anfang sind wir außerhalb eines Code-Blocks

```
a{additional read vars}
	t{char} v{openCh} = s{'\0'};
x{additional read vars}
```
* Der Befehl vor einer öffnenden Mengenklammer wird in dieser Variable
  zwischengespeichert

```
a{additional read vars}
	t{char} v{name}t{[128]};
	t{char *}v{nameCur} = k{NULL};	
	t{const char *}v{nameEnd} = v{name} +
		k{sizeof}(v{name});
x{additional read vars}
```
* Wenn `v{nameCur}` gesetzt ist, dann wird ein Name in Buffer gelesen

```
d{process close brace} {
	k{if} (v{nameCur}) {
		*v{nameCur} = s{'\0'};
		e{process macro name};
		v{nameCur} = k{NULL};
		v{last} = v{ch};
		v{ch} = f{nextCh}();
	}
} x{process close brace}
```
* Bei einer schließenden Mengenklammer wird der Makro-Name ausgewertet
* Danach wird der Namenszeiger zurückgesetzt

```
d{process other char} {
	k{if} (v{nameCur}) {
		f{ASSERT}(v{nameCur} < v{nameEnd}, s{"name too long"});
		*v{nameCur}++ = v{ch};
		k{break};
	}
} x{process other char}
```
* Wenn ein Name geparst wird, dann der Namensbuffer entsprechend
  erweitert

```
a{process other char} {
	k{if} (v{macro}) {
		f{addToBuffer}(&v{buffer}, v{last});
	}
} x{process other char}
```
* Wenn es ein aktuelles Makro gibt, dann müssen sonstige Zeichen dort
  angefügt werden

```
d{process open brace} {
	k{if} (! v{macro}) {
		k{static} t{const char} v{valids}[] = s{"adir"};
		k{if} (f{strchr}(v{valids}, v{last})) {
			v{openCh} = v{last};
			v{nameCur} = v{name};
			k{break};
		}
	}
} x{process open brace}
```
* Wenn außerhalb eines Makros die Folge `a`, `{` gelesen wird, dann
  beginnt ein `@add`-Fragment
* Falls ein `i`, `{` gelesen wird, dann muss eine andere Datei mit
  `@include` eingebunden werden
* Es folgt der Name des Makros oder der Pfad der Datei bis zum nächsten
  `}`

```
d{global source vars}
	t{struct MacroMap} v{macros} = {};
x{global source vars}
```
* Kollektion mit allen Makros wird für folgende Schritte sichtbar
  angelegt

```
d{process macro name}
	k{if} (v{openCh} == s{'d'}) {
		f{ASSERT}(! v{macro}, "def in macro");
		v{macro} = f{findMacroInMap}(
			&v{macros}, v{name}, v{nameCur}
		);
		if (isPopulatedMacro(macro)) {
			printf("macro [%.*s] already defined\n", (int) (nameCur - name), name);
		}
		if (! macro) {
			v{macro} = f{allocMacroInMap}(
				&v{macros}, v{name}, v{nameCur}
			);
		}
		v{processed} = k{true};
	}
x{process macro name}
```
* Erzeugt eine neues Makro
* Das Makro darf nicht bereits vorhanden sein

```
a{process macro name}
	k{if} (v{openCh} == s{'a'}) {
		f{ASSERT}(! v{macro}, "add in macro");
		v{macro} = f{findMacroInMap}(
			&v{macros}, v{name}, v{nameCur}
		);
		if (! isPopulatedMacro(macro)) {
			printf("macro [%.*s] not defined\n", (int) (nameCur - name), name);
			macro = getMacroInMap(&macros, name, nameCur);
		}
		v{processed} = k{true};
	}
x{process macro name}
```
* Bei einem öffnenden Makro wird das passende Makro gesucht
* Weitere Bytes können zu diesem Makro hinzugefügt werden
* Das Makro muss bereits vorhanden sein

```
a{process macro name}
	k{if} (v{openCh} == s{'r'}) {
		f{ASSERT}(! v{macro}, "replace in macro");
		v{macro} = f{getMacroInMap}(
			&v{macros}, v{name}, v{nameCur}
		);
		f{ASSERT}(v{macro}, "macro %.*s not defined", (int) (nameCur - name), name);
		f{freeMacrosEntries}(v{macro});
		v{processed} = k{true};
	}
x{process macro name}
```
* Bei einem `@replace` wird der Inhalt eines Makros zurückgesetzt
* Das Makro muss bereits vorhanden sein

```
a{process macro name}
	k{if} (v{openCh} == s{'x'}) {
		f{ASSERT}(v{macro}, "end not in macro");
		e{macro names must match};
		E{flush macro buffer};
		v{macro} = k{NULL};
		v{processed} = k{true};
	}
x{process macro name}
```
* Bei einem schließenden Makro wird das aktuelle Makro unterbrochen

```
a{process macro name}
	k{if} (v{openCh} == s{'i'}) {
		f{ASSERT}(! v{macro}, "include in macro");
		f{pushPath}(v{name});
		v{processed} = k{true};
	}
x{process macro name}
```
* Wenn eine Datei eingebunden werden soll, dann wird sie geöffnet und auf
  den Stapel der offenen Dateien gelegt

```
a{process macro name}
	k{if} (v{openCh} == s{'e'}) {
		f{ASSERT}(v{macro}, "expand not in macro");
		E{flush macro buffer};
		t{struct Macro *}v{sub} =
			f{getMacroInMap}(
				&v{macros}, v{name}, v{nameCur});
		if (sub->expands) {
			printf("multiple expands of [%s]\n", sub->name);
		}
		if (sub->multiples) {
			printf("expand after mult of [%s]\n", sub->name);
		}
		++sub->expands;
		f{addMacroToMacro}(
			v{macro}, v{sub});
		v{processed} = k{true};
	}
x{process macro name}
```

```
a{process macro name}
	k{if} (v{openCh} == s{'E'}) {
		f{ASSERT}(v{macro}, "multiple not in macro");
		E{flush macro buffer};
		t{struct Macro *}v{sub} =
			f{getMacroInMap}(
				&v{macros}, v{name}, v{nameCur});
		if (sub->expands) {
			printf("multiple after expand of [%s]\n", sub->name);
		}
		++sub->multiples;
		f{addMacroToMacro}(
			v{macro}, v{sub});
		v{processed} = k{true};
	}
x{process macro name}
```

```
a{process macro name}
	k{if} (v{openCh} == s{'p'}) {
		f{ASSERT}(v{macro}, "private not in macro");
		e{process private macro};
		v{processed} = k{true};
	}
x{process macro name}
```
* Die Implementierung wird später durch eine sinnvollere ersetzt
* Daher ist der zugehörige Code in einem eigenen Fragment gekapselt
* Dieses kann später ersetzt werden
* Nachdem diese Funktionalität implementiert wurde

```
d{process private macro}
	t{static char} v{prefix}[] = "_private_";
	E{flush macro buffer};
	f{addBytesToMacro}(
		v{macro}, v{prefix}, v{prefix} + f{sizeof}(v{prefix}) - 1
	);
	f{addBytesToMacro}(
		v{macro}, v{name}, v{nameCur}
	);
x{process private macro}
```
* Erstmal nur ein konstanter String, um private Bezeichner zu
  verstecken
* Später soll ein individueller Hash integriert werden
* Der vom Dateinamen abhängt

```
a{process macro name}
	k{if} (v{openCh} == s{'m'}) {
		f{ASSERT}(v{macro}, "magic not in macro");
		e{process magic macro};
		v{processed} = k{true};
	}
x{process macro name}
```
* Auch die Implementierung des magci-Makros soll später ersetzt werden
* Daher wird es in einem eigenen Fragment gekapselt

```
d{process magic macro}
	t{static char} v{magic}[] = "2478325";
	E{flush macro buffer};
	f{addBytesToMacro}(
		v{macro}, v{magic}, v{magic} + f{sizeof}(v{magic}) - 1
	);
x{process magic macro}
```
* Die provisorische Version liefert nur eine konstante Zahl
* Aber auch hier sollte eigentlich ein Hash geliefert werden
* Der vom Dateinamen und vom Argument abhängt

```
d{flush macro buffer}
	k{if} (
		v{buffer}.v{buffer} != v{buffer}.v{current}
	) {
		f{addBytesToMacro}(
			v{macro}, v{buffer}.v{buffer},
			v{buffer}.v{current}
		);
		f{resetBuffer}(&v{buffer});
	}
x{flush macro buffer}
```

```
a{process open brace} {
	k{if} (v{macro}) {
		t{bool} v{valid} = k{false};
		e{check valid names};
		k{if} (v{valid}) {
			v{openCh} = v{last};
			v{nameCur} = v{name};
			k{break};
		}
	}
} x{process open brace}
```

```
d{check valid names}
	k{static} t{const char} v{valids}[] =
		s{"123456fvsntkxeEpm"};
	k{if} (f{strchr}(v{valids}, v{last})) {
		v{valid} = k{true};
	}
x{check valid names}
```

```
a{process macro name}
	k{if} (! v{processed}) {
		f{ASSERT}(v{macro}, "unknown macro %.*s", (int) (nameCur - name), name);
		t{const char *}v{c} = v{name};
		k{for} (; v{c} != v{nameCur}; ++v{c}) {
			f{addToBuffer}(&v{buffer}, *v{c});
		}
		v{processed} = k{true};
	}
x{process macro name}
```

```
a{process open brace}
	k{if} (v{macro}) {
		f{addToBuffer}(&v{buffer}, v{last});
	}
x{process open brace}

a{process close brace}
	k{if} (v{macro} && ! v{processed}) {
		f{addToBuffer}(&v{buffer}, v{last});
	}
x{process close brace}
```

* Entities werden in einer eigenen Datei expandiert

# Fragmente serialisieren
* Fragmente, die Dateien spezifizieren werden in diese Dateien
  rausgeschrieben

```
d{serialize fragments} {
	t{struct Macro **}v{cur} = v{macros}.v{macros};
	t{struct Macro **}v{end} =
		v{cur} + v{MACRO_SLOTS};
	k{for} (; v{cur} < v{end}; ++v{cur}) {
		t{struct Macro *}v{macro} = *v{cur};
		k{for} (; v{macro}; v{macro} = v{macro}->v{link}) {
			e{serialize macro};
		}
	}
} x{serialize fragments}
```
* Fragmente, die mit `file:` beginnen, werden in die entsprechenden
  Dateien rausgeschrieben

```
d{serialize macro}
	k{if} (! f{memcmp}(
		s{"file: "}, macro->name, n{6}
	)) {
		++v{macro}->v{expands};
		e{write in file};
	}
x{serialize macro}
```

```
a{serialize macro}
	k{if} (macro->expands + macro->multiples <= 0) {
		printf("macro [%s] not used\n", macro->name);
	}
x{serialize macro}
```

```
d{write in file}
	t{FILE *}v{f} = f{fopen}(v{macro}->v{name} + n{6}, "w");
	f{ASSERT}(v{f}, "can't open %s", v{macro}->v{name} + n{6});
	t{struct FileConsumer} v{fc};
	f{setupFileConsumer}(&v{fc}, v{f});
	f{serializeMacro}(
		v{macro}, &v{fc}.v{consumer}
	);
	f{fclose}(v{f});
x{write in file}
```
* Das Fragment wird in die entsprechende Datei geschrieben
* Dabei werden HTML-Entitäten aufgelöst


# HTML generieren
* Aus `hx`-Dateien wird ein HTML-Foliensatz generiert

```
i{html.x}
```
* Die Generierung liegt in einer eigenen Datei

