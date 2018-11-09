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
* Nicht definierte Makros werden zu nichts expandieren
* So kann das Verständnis für ein Programm schrittweise erarbeitet
  werden

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
* `@expand`-Blöcke beschreiben Makro-Aufrufe
* Der Wert des Makros mit dem Namen in Klammern wird anstelle des
  Aufrufs im endgültigen Programm gesetzt
* Diese Makros bilden ein zentrales Element von `hx`
* Sie können mit `@def`-`@end`-Sequenzen definiert werden
* Oder mit `@add`-`@end` erweitert werden
* Ein `@expand` darf nur einmal aufgelöst werden
* Für mehrfache Auflösungen muss `@mulitple` verwendet werden

```
d{global elements}
	e{includes};
	e{define logging};
	e{forward declarations};
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
		e{additional input elements};
		t{char} v{name}t{[]};
	};

	t{struct Input *}v{input} = k{NULL};
	t{struct Input *}v{used} = k{NULL};
x{global elements}
```
* Es gibt immer eine aktuelle Datei, die gerade gelesen wird
* Mitten während des Lesens können andere Dateien eingelesen
  (inkludiert) werden
* Daher gibt es einen Stapel offener Dateien
* Aus der letzten wird aktuell gelesen
* Eine Liste aller gelesenen Dateien wird in `v{used}` verwaltet
* Damit wird verhindert, dass eine Datei mehrfach gelesen wird
* Auch signalisiert es der HTML-Ausgabe, welche Dateien generiert
  werden müssen

```
a{global elements}
	t{void} f{pushPath}(t{const char *}v{path}) {
		t{FILE *}v{f} = f{fopen}(v{path}, s{"r"});
		e{check file for path};
		t{int} v{len} = f{strlen}(v{path}) + n{1};
		t{struct Input *}v{i} = f{malloc}(
			f{sizeof}(t{struct Input}) + v{len}
		);
		e{check memory for input};
		v{i}->v{link} = v{input};
		v{i}->v{file} = v{f};
		f{memcpy}(v{i}->v{name}, v{path}, v{len});
		e{init additional input elements};
		v{input} = v{i};
	}
x{global elements}
```
* Dateien werden über ihren Pfad identifiziert
* Dieser wird als Name gespeichert
* Die Struktur wird dynamisch so groß gewählt, dass der Name hinein
  passt

```
d{check file for path}
	f{ASSERT}(
		v{f}, s{"can't open [%s]"}, v{path}
	);
x{check file for path}
```
* Wenn die Datei nicht geöffnet werden kann, bricht das Programm ab

```
d{check memory for input}
	f{ASSERT}(
		v{i},
		s{"no memory for input"}
	);
x{check memory for input}
```
* Wenn kein Speicher für die `t{struct Input}` vorhanden ist bricht das
  Programm ab

# Kommandozeile
* Die Kommandozeile wird Element für Element abgearbeitet

```
a{global elements}
	t{const char *}v{stylesheet} =
		s{"slides/slides.css"};
x{global elements}
```
* Für die HTML-Ausgabe wird eine Stylesheet-Datei benötigt
* Über die Kommandozeile kann eine alternative Datei angegeben werden

```
d{process arguments}
	t{bool} v{someFile} = k{false};
	k{for} (t{int} v{i} = n{1}; v{i} < v{argc}; ++v{i}) {
		e{process argument};
		f{ASSERT}(
			k{false},
			s{"unknown argument [%s]"},
			v{argv}[v{i}]
		);
	}
x{process arguments}
```
* Die Argumente werden einzeln durchgegangen
* Wenn sie nicht verwendet werden, bricht das Programm ab

```
d{process argument} {
	t{const char} v{prefix}t{[]} = s{"--css="};
	t{int} v{len} = f{sizeof}(v{prefix}) - n{1};
	k{if} (f{memcmp}(v{argv}[v{i}], v{prefix}, v{len}) == n{0}) {
		v{stylesheet} = v{argv}[v{i}] + v{len};
		k{continue};
	}
} x{process argument}
```
* Der Pfad zur Stylesheet-Datei kann über die Kommandozeile gesetzt
  werden

```
a{process argument}
	k{if} (! v{someFile}) {
		f{pushPath}(v{argv}[n{1}]);
		v{someFile} = k{true};
		k{continue};
	}
x{process argument}
```
* Ansonsten wird das Argument als Pfad der `.x`-Datei interpretiert
* Aus dieser werden HTML-Slides und Source-Code generiert
* Es kann nur eine Datei angegeben werden

```
a{process arguments}
	k{if} (! v{someFile}) {
		f{pushPath}(s{"index.x"});
	}
x{process arguments}
```
* Wenn kein Pfad angegeben wurde, wird `index.x` als Vorgabe verwendet

# Nächstes Zeichen
* Die Funktion `f{nextCh}` liest das nächste Zeichen aus der aktuellen
  Datei
* Wenn das Dateiende erreicht ist, wird die nächste Datei aus dem
  Stapel der offenen Dateien geholt
* Erst wenn die letzte Datei fertig gelesen wurde, wird ein `k{EOF}`
  zurück geliefert

```
a{global elements}
	t{int} f{nextCh}() {
		t{int} v{ch} = k{EOF};
		k{while} (v{input}) {
			v{ch} = f{fgetc}(v{input}->v{file});
			e{preprocess char};
			k{if} (v{ch} != k{EOF}) { k{break}; }
			e{get next input file};
		}
		k{return} v{ch};
	}
x{global elements}
```
* Wenn kein `k{EOF}` gelesen wurde, dann wird das Zeichen zurück
  geliefert
* Ansonsten wird aus der nächsten Datei ein Zeichen gelesen

```
d{get next input file}
	t{struct Input *}v{n} = v{input}->v{link};
	f{fclose}(v{input}->v{file});
	v{input}->v{link} = v{used};
	v{used} = v{input};
	v{input} = v{n};
x{get next input file}
```
* Die aktuelle Datei wird geschlossen und in die Liste der bereits
  verarbeiteten Dateien eingereiht
* Dann wird der Vorgänger zur aktuellen Datei erklärt

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
		t{int} v{ch} = v{last} != k{EOF} ? f{nextCh}() : k{EOF};
		k{while} (v{ch} != v{EOF}) {
			e{process current char};
			v{last} = v{ch}; v{ch} = f{nextCh}();
		}
	}
x{read source file}

```
* Neben dem aktuellen Zeichen wird auch das letzte Zeichen aufgehoben
* Dabei kann `hx` auch mit einer leeren Eingabe-Datei umgehen (wenn
  schon das erste Zeichen ein `k{EOF}` ist)

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
* Diese bestimmen den Anfang und das Ende von Makro-Sequenzen
* Welche die Bearbeitung der sonstigen Zeichen steuern

```
d{additional read vars}
	t{struct Frag *}v{macro} = k{NULL};
	t{struct Buffer} v{buffer} = {};
	t{int} v{bufferLine} = n{0};
x{additional read vars}
```
* Wir unterscheiden, ob wir in einem Code-Block sind, oder außerhalb
* In einem Code sind wir sogar in einem Makro, dessen Inhalt gerade
  gelesen wird
* Am Anfang sind wir außerhalb eines Code-Blocks
* In einem Code-Block ist `v{macro}` nicht `k{NULL}`

```
a{additional read vars}
	t{char} v{openCh} = s{'\0'};
x{additional read vars}
```
* Das Zeichenvor einer öffnenden Mengenklammer wird in `v{openCh}`
  zwischengespeichert
* Es beschreibt, welcher Befehl ausgeführt werden soll

```
a{additional read vars}
	t{struct Buffer} v{name} = {};
	t{int} v{nameLine} = n{0};
x{additional read vars}
```
* Wenn `v{name}` aktiv ist, dann wird ein Name in Buffer gelesen

```
d{process close brace} {
	k{if} (isActiveBuffer(&v{name})) {
		f{addToBuffer}(&v{name}, s{'\0'});
		e{process macro name};
		f{resetBuffer}(&v{name});
		v{last} = v{ch};
		v{ch} = f{nextCh}();
	}
} x{process close brace}
```
* Bei einer schließenden Mengenklammer wird der Makro-Name ausgewertet
* Danach wird der Namenszeiger zurückgesetzt

```
d{process other char} {
	k{if} (isActiveBuffer(&v{name})) {
		f{addToBuffer}(&v{name}, v{ch});
		k{break};
	}
} x{process other char}
```
* Wenn ein Name geparst wird, dann der Namensbuffer entsprechend
  erweitert

```
a{process other char} {
	k{if} (v{macro}) {
		if (! isActiveBuffer(&v{buffer)) {
			bufferLine = input->line;
		}
		f{addToBuffer}(&v{buffer}, v{last});
	}
} x{process other char}
```
* Wenn es ein aktuelles Makro gibt, dann müssen sonstige Zeichen dort
  angefügt werden

```
d{process open brace} {
	k{if} (! v{macro}) {
		k{static} t{const char} v{valids}t{[]} = s{"adir"};
		k{if} (f{strchr}(v{valids}, v{last})) {
			v{openCh} = v{last};
			f{activateBuffer}(&v{name});
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
	t{struct FragMap} v{frags} = {};
x{global source vars}
```
* Kollektion mit allen Makros wird für folgende Schritte sichtbar
  angelegt

```
d{process macro name}
	k{if} (v{openCh} == s{'d'}) {
		f{ASSERT}(! v{macro}, s{"def in macro"});
		e{check for double def};
		k{if} (! v{macro}) {
			v{macro} = f{allocFragInMap}(
				&v{frags}, v{name}.v{buffer},
				v{name}.v{current} - n{1}
			);
		}
		v{processed} = k{true};
	}
x{process macro name}
```
* Erzeugt ein neues Makro
* Das Makro darf nicht mehrfach definiert werden

```
d{check for double def}
	v{macro} = f{findFragInMap}(
		&v{frags}, v{name}.v{buffer},
		v{name}.v{current} - n{1}
	);
	k{if} (f{isPopulatedFrag}(v{macro})) {
		f{printf}(
			s{"macro [%s] already defined\n"},
			v{name}.v{buffer}
		);
	}
x{check for double def}
```
* Wenn das Makro bereits existiert, wird es vielleicht nur verwendet
* Es muss geprüft werden, ob es schon Inhalt hat
* Das wäre dann eine Fehlermeldung wert
* Bricht aber die Abarbeitung nicht ab

```
a{process macro name}
	k{if} (v{openCh} == s{'a'}) {
		f{ASSERT}(! v{macro}, s{"add in macro"});
		v{macro} = f{findFragInMap}(
			&v{frags}, v{name}.v{buffer},
			v{name}.v{current} - n{1}
		);
		e{check for add without def};
		v{processed} = k{true};
	}
x{process macro name}
```
* Bei einem öffnenden Makro wird das passende Makro gesucht
* Weitere Bytes können zu diesem Makro hinzugefügt werden

```
d{check for add without def}
	k{if} (! f{isPopulatedFrag}(v{macro})) {
		f{printf}(
			s{"macro [%s] not defined\n"},
			v{name}.v{buffer}
		);
		v{macro} = f{getFragInMap}(
			&v{frags}, v{name}.v{buffer},
			v{name}.v{current} - n{1}
		);
	}
x{check for add without def}
```
* Das Makro muss bereits vorhanden und nicht leer sein

```
a{process macro name}
	k{if} (v{openCh} == s{'r'}) {
		f{ASSERT}(! v{macro}, s{"replace in macro"});
		v{macro} = f{getFragInMap}(
			&v{frags}, v{name}.v{buffer},
			v{name}.v{current} - n{1}
		);
		f{ASSERT}(
			v{macro}, s{"macro %s not defined"},
			v{name}.v{buffer}
		);
		f{freeFragEntries}(v{macro});
		v{processed} = k{true};
	}
x{process macro name}
```
* Bei einem `@replace` wird der Inhalt eines Makros zurückgesetzt
* Das Makro muss bereits vorhanden sein

```
a{process macro name}
	k{if} (v{openCh} == s{'x'}) {
		f{ASSERT}(v{macro}, s{"end not in macro"});
		e{macro names must match};
		E{flush macro buffer};
		v{macro} = k{NULL};
		v{processed} = k{true};
	}
x{process macro name}
```
* Bei einem schließenden Makro wird das aktuelle Makro unterbrochen

```
d{macro names must match}
	f{ASSERT}(
		! f{strcmp}(v{macro}->v{name}, v{name}.v{buffer}),
		s{"closing [%s] != [%s]"},
		v{name}.v{buffer}, v{macro}->v{name}
	);
x{macro names must match}
```
* Wenn der öffnende und schließende Name nicht passt, wird die
  Abarbeitung abgebrochen

```
a{global source vars}
	t{bool} f{alreadyUsed}(t{const char *}v{name}) {
		t{struct Input *}v{i} = v{input};
		k{for} (; v{i}; v{i} = v{i}->v{link}) {
			k{if} (f{strcmp}(v{i}->v{name}, v{name}) == n{0}) {
				k{return} k{true};
			}
		}
		k{for} (v{i} = v{used}; v{i}; v{i} = v{i}->v{link}) {
			k{if} (f{strcmp}(v{i}->v{name}, v{name}) == n{0}) {
				k{return} k{true};
			}
		}
		k{return} k{false};
	}
x{global source vars}
```
* Prüft ob eine Datei bereits verwendet wurde
* Sowohl die offenen als auch die bereits prozessierten Dateien werden
  durchgegangen
* Dadurch wird bei Einbettungen verhindert, dass eine Datei mehrfach
  verarbeitet wird

```
a{process macro name}
	k{if} (v{openCh} == s{'i'}) {
		f{ASSERT}(! v{macro}, s{"include in macro"});
		k{if} (! f{alreadyUsed}(v{name}.v{buffer})) {
			f{pushPath}(v{name}.v{buffer});
		}
		v{processed} = k{true};
	}
x{process macro name}
```
* Wenn eine Datei eingebunden werden soll, dann wird sie geöffnet und
  auf den Stapel der offenen Dateien gelegt
* Wenn die Datei bereits geöffnet wurde, dann wird sie ignoriert

```
a{process macro name}
	k{if} (v{openCh} == s{'e'}) {
		f{ASSERT}(v{macro}, s{"expand not in macro"});
		E{flush macro buffer};
		t{struct Frag *}v{sub} = f{getFragInMap}(
			&v{frags}, v{name}.buffer,
			v{name}.v{current} - n{1}
		);
		e{check macro expand count};
		++sub->expands;
		f{addFragToFrag}(v{macro}, v{sub});
		v{processed} = k{true};
	}
x{process macro name}
```
* Bei einem `@expand` wird das Makro gesucht und eingebunden
* Ggf. wird das Makro dabei auch erzeugt, um später befüllt zu werden
* Das Attribut `v{expands}` zählt, wie häufig das Makro expandiert
  wurde

```
d{check macro expand count}
	k{if} (v{sub}->v{expands}) {
		f{printf}(
			s{"multiple expands of [%s]\n"},
			v{sub}->v{name}
		);
	}
	k{if} (v{sub}->v{multiples}) {
		f{printf}(
			s{"expand after mult of [%s]\n"},
			v{sub}->v{name}
		);
	}
x{check macro expand count}
```
* Wenn das Fragment bereits expandiert wurde, dann wird eine Meldung
  ausgegeben
* Wenn das Fragment bereits im Mehrfach-Modus ausgegeben wurde, wird
  ebenfalls eine Meldung ausgegeben

```
a{process macro name}
	k{if} (v{openCh} == s{'E'}) {
		f{ASSERT}(v{macro}, s{"multiple not in macro"});
		E{flush macro buffer};
		t{struct Frag *}v{sub} =
			f{getFragInMap}(
				&v{frags}, v{name}.v{buffer},
				v{name}.v{current} - n{1}
			);
		e{check for prev expands};
		++sub->multiples;
		f{addFragToFrag}(
			v{macro}, v{sub});
		v{processed} = k{true};
	}
x{process macro name}
```
* Mit einem `@multiple` Befehl kann ein Fragment an mehreren Stellen
  expandiert werden

```
d{check for prev expands}
	k{if} (v{sub}->v{expands}) {
		f{printf}(
			s{"multiple after expand "}
				s{"of [%s]\n"},
			v{sub}->v{name}
		);
	}
x{check for prev expands}
```
* Es ist ein Fehler, wenn das Fragment bereits normal `@expand`iert
  wurde

```
a{process macro name}
	k{if} (v{openCh} == s{'p'}) {
		f{ASSERT}(v{macro}, s{"private not in macro"});
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
	t{static char} v{prefix}t{[]} = s{"_private_"};
	E{flush macro buffer};
	f{addBytesToFrag}(
		v{macro}, v{prefix},
		v{prefix} + f{sizeof}(v{prefix}) - n{1},
		v{input}, v{nameLine}
	);
	f{addBytesToFrag}(
		v{macro}, v{name}.v{buffer}, v{name}.v{current} - n{1},
		k{input}, v{nameLine}
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
* Auch die Implementierung des `@magic`-Befehls soll später ersetzt
  werden
* Daher wird es in einem eigenen Fragment gekapselt

```
d{process magic macro}
	t{static char} v{magic}t{[]} = s{"2478325"};
	E{flush macro buffer};
	f{addBytesToFrag}(
		v{macro}, v{magic}, v{magic} + f{sizeof}(v{magic}) - n{1},
		v{input}, v{nameLine}
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
		f{addBytesToFrag}(
			v{macro}, v{buffer}.v{buffer},
			v{buffer}.v{current},
			v{input}, v{bufferLine}
		);
		f{resetBuffer}(&v{buffer});
	}
x{flush macro buffer}
```
* Das Fragment fügt alle Bytes im Buffer an ein Fragment an
* Danach wird der Buffer zurück gesetzt

```
a{process open brace} {
	k{if} (v{macro}) {
		t{bool} v{valid} = k{false};
		e{check valid names};
		k{if} (v{valid}) {
			v{openCh} = v{last};
			v{nameLine} = v{input}->v{line};
			f{activateBuffer}(&v{name});
			k{break};
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
	t{static const char} v{valids}t{[]} =
		s{"fvsntkxeEpm"};
	k{if} (f{strchr}(v{valids}, v{last})) {
		v{valid} = k{true};
	}
x{check valid names}
```
* Gültige Kommando-Zeichen sind in einem String abgelegt
* Wenn das Zeichen im String vorkommt, dann ist es gültig

```
a{process macro name}
	k{if} (! v{processed}) {
		f{ASSERT}(
			v{macro}, s{"unknown macro %s"},
			v{name}.v{buffer}
		);
		t{const char *}v{c} = v{name}.v{buffer};
		k{for} (; v{c} != v{name.current} - n{1}; ++v{c}) {
			if (! isActiveBuffer(&v{buffer)) {
				bufferLine = input->line;
			}
			f{addToBuffer}(&v{buffer}, *v{c});
		}
		v{processed} = k{true};
	}
x{process macro name}
```
* Wenn kein bekannter Befehl erkannt wurde, dann ist die
  befehlsähnliche Eingabe Teil des Programms
* Und wird daher in den entsprechenden Buffer kopiert

```
a{process open brace}
	k{if} (v{macro}) {
		if (! isActiveBuffer(&v{buffer)) {
			bufferLine = input->line;
		}
		f{addToBuffer}(&v{buffer}, v{last});
	}
x{process open brace}
```
* Wenn wir uns in einem Makro befinden und bis hier gekommen sind, dann
  wird das Zeichen vor der öffnenden Klammer zum Makro hinzu gefügt

```
a{process close brace}
	k{if} (v{macro} && ! v{processed}) {
		if (! isActiveBuffer(&v{buffer)) {
			bufferLine = input->line;
		}
		f{addToBuffer}(&v{buffer}, v{last});
	}
x{process close brace}
```
* Wenn schließende Mengenklammern nicht Teil eines Makros sind, können
  sie Teil des Programms sein
* Und werden daher zum Buffer direkt hinzugefügt

# Fragmente serialisieren
* Fragmente, die Dateien spezifizieren werden in diese Dateien
  rausgeschrieben

```
d{serialize fragments} {
	t{struct Frag **}v{cur} = v{frags}.v{frags};
	t{struct Frag **}v{end} =
		v{cur} + v{FRAG_SLOTS};
	k{for} (; v{cur} < v{end}; ++v{cur}) {
		t{struct Frag *}v{macro} = *v{cur};
		k{for} (; v{macro}; v{macro} = v{macro}->v{link}) {
			e{serialize macro};
		}
	}
} x{serialize fragments}
```
* Fragmente, die mit `file:` beginnen, werden in die entsprechenden
  Dateien rausgeschrieben
* Zusätzlich wird geprüft, ob Fragmente zu selten oder zu oft expandiert
  wurden

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
* Wenn der Name eines Makros mit `file: ` beginnt, dann wird es in die
  passende Datei geschrieben
* Zusätzlich zählt das als eine Expansion

```
a{serialize macro} {
	t{int} v{sum} =
		v{macro}->v{expands} + v{macro}->v{multiples};
	k{if} (v{sum} <= n{0}) {
		f{printf}(
			s{"macro [%s] not called\n"},
			v{macro}->v{name}
		);
	}
} x{serialize macro}
```
* Ein Makro wurde nicht aufgerufen
* Dies wird mit einer Meldung quittiert

```
a{serialize macro}
	k{if} (v{macro}->v{multiples} == n{1}) {
		f{printf}(
			s{"multiple macro [%s] only "}
				s{"used once\n"},
			v{macro}->v{name}
		);
	}
x{serialize macro}
```
* Ein Makro das zur mehrfachen Verwendung deklariert wurde, wird nur
  einmal verwendet
* Dies wird mit einer Meldung quittiert

```
a{serialize macro}
	k{if} (! f{isPopulatedFrag}(v{macro})) {
		f{printf}(
			s{"macro [%s] not populated\n"},
			v{macro}->v{name}
		);
	}
x{serialize macro}
```
* Für jedes Makro, das nicht befüllt wurde wird eine Meldung ausgegeben

```
d{write in file}
	t{FILE *}v{f} =
		f{fopen}(v{macro}->v{name} + n{6}, s{"w"});
	f{ASSERT}(
		v{f}, s{"can't open %s"},
		v{macro}->v{name} + n{6}
	);
	f{serializeFrag}(v{macro}, v{f}, k{false});
	f{fclose}(v{f});
x{write in file}
```
* Das Fragment wird in die entsprechende Datei geschrieben

# Zeilennummern

```
d{additional input elements}
	t{int} v{line};
x{additional input elements}
```
* Pro Datei wird die aktuelle Zeile festgehalten

```
d{init additional input elements}
	v{i}->v{line} = n{1};
x{init additional input elements}
```
* Beim Öffnen einer neuen `.x`-Datei befinden wir uns in Zeile `n{1}`

```
d{preprocess char}
	k{if} (v{ch} == s{'\n'}) {
		++v{input}->v{line};
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

