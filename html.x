# HTML-Präsentation generieren
* Diese Datei beschreibt Code, um aus den `x`-Quelldateien
  HTML-Präsentationen zu erzeugen

```
D{write HTML file}
	t{struct Input *}cur = used;
	while (cur) {
		e{write cur HTML file};
		t{struct Input *}next = cur->link;
		free(cur);
		cur = next;
	}
x{write HTML file}
```
* Alle bisher prozessierten Dateien werden erneut durchgegangen
* Nach der Abarbeitung einer Datei wird deren Speicher freigegeben

```
d{write cur HTML file}
	t{int} len = strlen(cur->name) + n{4};
	t{char *}outPath = malloc(len);
	ASSERT(outPath);
	memcpy(outPath, cur->name, len - n{6});
	strcpy(outPath + len - n{6}, s{".html"});
	t{FILE *}out = fopen(outPath, s{"w"});
	ASSERT(out);
	e{write cur HTML file to out};
	fclose(out);
	free(outPath);
x{write cur HTML file}
```
* Die HTML hat den gleichen Pfad mit der Endung `.html` anstatt `.x`

```
d{write cur HTML file to out} 
	t{FILE *}in = fopen(cur->name, s{"r"});
	ASSERT(in);
	e{write HTML file from in to out};
	fclose(in);
x{write cur HTML file to out}
```
* Zuerst wird die Eingabe-Datei zum Lesen geöffnet

```
A{global elements}
	t{enum HtmlState} {
		hs_NOTHING_WRITTEN,
		hs_IN_SLIDE,
		hs_AFTER_SLIDE
		e{html state enums}
	};
x{global elements}
```
* In einem Zustands-Automaten wird abgelegt, in welchem Modus sich die
  gerade generierte HTML-Datei befindet
* Vor dem ersten Element muss der HTML-Header geschrieben werden
* Ansonsten wird unterschieden, ob eine Folie bereits geöffnet wurde
* Oder ob gerade keine Folie offen ist

```
A{global elements}
	t{struct HtmlStatus} {
		e{html state elements}
	};
x{global elements}
```
* Der aktuelle Status wird in `t{struct HtmlStatus}` abgelegt

```
d{html state elements}
	t{enum HtmlState} state;
x{html state elements}
```
* Der aktuelle Zustand wird im Status abgelegt

```
d{write HTML file from in to out} {
	t{struct HtmlStatus} status = {
		.state = hs_NOTHING_WRITTEN
		e{init html status}
	};
	t{bool} newline = n{true};
	t{struct Buffer} ident = {};
	for (;;) {
		t{int} ch = fgetc(in);
		e{process ch for HTML};
		if (ch == n{EOF}) { break; }
		E{move ch to last};
	}
} x{write HTML file from in to out}
```
* Beim Schreiben einer Datei wird zuerst der Status initialisiert
* Wir befinden uns am Anfang einer Zeile
* Jedes Zeichen wird prozessiert
* Bis das Datei-Ende erreicht ist

```
d{move ch to last}
	newline = ch == s{'\n'};
	char xx = ch;
	writeEscaped(out, &xx, &xx + 1);
x{move ch to last}
```
* Beim Kopieren des Zeichens wird `n{EOF}` durch `s{'\0'}` ersetzt

# Überschriften

```
d{html state enums}
	, hs_IN_HEADER
x{html state enums}
```
* Es gibt einen eigenen Zustand, wenn eine Überschrift gelesen wird

```
A{global elements}
	t{bool} isOutOfHtmlSpecial(
		t{struct HtmlStatus *}s
	) {
		e{check html special state};
		return n{true};
	}
x{global elements}
```
* Die Funktion `{isOutOfHtmlSpecial}` liefert `n{true}`, wenn sich
  der Zustands-Automat gerade nicht in einem Sonder-Modus (Überschrift,
  Code, Notizen) befindet

```
d{check html special state}
	if (s->state == hs_IN_HEADER) {
		return n{false};
	}
x{check html special state}
```
* Die Überschrift ist ein Sonder-Modus

```
a{html state elements}
	t{int} headerLevel;
	t{struct Buffer} headerName;
	t{enum HtmlState} headerState;
x{html state elements}
```
* Für Überschriften wird abgelegt, auf welchem Level sie vorkommen
* Und der Titel der Überschrift wird gespeichert
* Und der Zustand in dem sich der Parser beim Auftreten der Überschrift
  befunden hat

```
d{init html status}
	, .headerLevel = n{0}
	, .headerName = {}
x{init html status}
```
* Level und Name werden leer initialisiert

```
d{process ch for HTML} 
	if (ch == s{'#'} && newline) {
		if (isOutOfHtmlSpecial(&status) ||
			status.state == hs_IN_HEADER
		) {
			++status.headerLevel;
			if (status.state != hs_IN_HEADER) {
				status.headerState =
					status.state;
			}
			status.state = hs_IN_HEADER;
			continue;
		}
	}
x{process ch for HTML}
```
* Wenn am Anfang einer Zeile eine Raute gelesen wird, dann beginnt eine
  Überschrift
* Solange weitere Rauten folgen, wird der Level erhöht
* Bei der ersten Raute muss der alte Zustand gesichert werden

```
a{process ch for HTML} 
	if (status.state == hs_IN_HEADER) {
		if (ch == s{'\n'}) {
			e{process header in HTML};
			e{reset header state};
			E{move ch to last};
			continue;
		}
	}
x{process ch for HTML}
```
* Wenn die Zeile mit der Überschrift beendet wurde, wird die
  Überschrift ausgegeben

```
d{reset header state} 
	status.state = hs_IN_SLIDE;
	status.headerLevel = n{0};
	resetBuffer(&status.headerName);
	status.headerState = hs_IN_SLIDE;
x{reset header state}
```
* Beim Zurücksetzen des Zustands wird sichergestellt, das der Level und
  Name der Überschrift zurückgesetzt sind

```
a{process ch for HTML} 
	if (status.state == hs_IN_HEADER) {
		if (
			isActiveBuffer(&status.headerName)
		) {
			addToBuffer(
				&status.headerName, ch
			);
			E{move ch to last};
			continue;
		}
	}
x{process ch for HTML}
```
* Innerhalb der Überschrift wird das aktuelle Zeichen zur Überschrift
  hinzugefügt

```
a{process ch for HTML} 
	if (status.state == hs_IN_HEADER) {
		if (ch > s{' '} && ! isActiveBuffer(
			&status.headerName
		)) {
			addToBuffer(
				&status.headerName, ch
			);
			E{move ch to last};
			continue;
		}
	}
x{process ch for HTML}
```
* Leerzeichen zwischen den Rauten und der Überschrift werden ignoriert

```
d{process header in HTML} 
	ASSERT(isActiveBuffer(&status.headerName));
	e{close previous HTML page};
	E{write header tag};
	fprintf(out, s{"<div class=\"slides\">\n"});
	fprintf(out, s{"<div><div>\n"});
	E{write header tag};
	fprintf(out, s{"</div>\n"});
x{process header in HTML}
```
* Falls schon eine Seite offen ist, dann wird diese geschlossen
* Dann wird eine HTML-Überschrift ausgegeben
* Und eine Folie mit einer HTML-Überschrift wird erzeugt

```
A{global elements} 
	t{void} writeEscaped(
		t{FILE *}out, t{const char *}str,
		t{const char *}end
	) {
		ASSERT(out); ASSERT(str);
		for (; *str && str != end; ++str) {
			switch (*str) {
				e{escape special}
				default:
					fputc(*str, out);
			}
		}
	}
x{global elements}
```
* Zeichen, die einen HTML-Befehl einleiten können, werden durch passende
  Entitäten ersetzt

```
d{escape special}
	case s{'<'}:
		fprintf(out, s{"&lt;"});
		break;
	case s{'>'}:
		fprintf(out, s{"&gt;"});
		break;
	case s{'&'}:
		fprintf(out, s{"&amp;"});
		break;
x{escape special}
```
* Die Zeichen `<`, `>` und `&` müssen ersetzt werden

```
d{write header tag} 
	fprintf(out, s{"<h%d>"}, status.headerLevel);
	writeEscaped(
		out,
		status.headerName.buffer,
		status.headerName.current
	);
	fprintf(out, s{"</h%d>\n"}, status.headerLevel);
x{write header tag}
```
* Die HTML-Überschrift enthält den eingelesenen Level

```
d{close previous HTML page} 
	switch (status.headerState) {
		case hs_NOTHING_WRITTEN: {
			e{write HTML header};
			break;
		}
		case hs_IN_SLIDE: {
			fprintf(out, s{"</div>\n"});
			fprintf(out, s{"</div>\n"});
			break;
		}
		default: {
			fprintf(out, s{"</div>\n"});
		}
	}
x{close previous HTML page}
```
* Je nach Zustand müssen unterschiedlich viele HTML-Tags geschlossen
  werden

```
d{write HTML header} 
	fprintf(out, s{"<!doctype html>\n"});
	fprintf(out, s{"<html lang=\"de\"l>\n"});
	fprintf(out, s{"<head>\n"});
	e{write HTML header entries};
	fprintf(out, s{"</head>\n"});
	fprintf(out, s{"<body>\n"});
x{write HTML header}
```
* Dies wird am Anfang der HTML-Datei ausgegeben
* Dafür muss jede `.x`-Datei mit einer Überschrift beginnen
* Diese Überschrift ist dann auch der Titel der HTML-Datei

```
d{write HTML header entries} 
	fprintf(
		out, s{"<meta charset=\"utf-8\">\n"}
	);
	fprintf(out, s{"<title>"});
	writeEscaped(
		out,
		status.headerName.buffer,
		status.headerName.current
	);
	fprintf(out, s{"</title>"});
	fprintf(
		out, s{"<link rel=\"stylesheet\" "}
		s{"type=\"text/css\" href=\"%s\">"},
		stylesheet
	);
x{write HTML header entries}
```
* Im Header wird das Zeichenformat auf UTF-8 gesetzt
* Und der Titel ausgegeben
* Und die Stylesheet-Datei eingebunden

# Code formatieren
* Code kann auf Seiten oder in den Notizen ausgegeben werden

```
a{html state enums}
	, hs_IN_CODE
x{html state enums}
```
* Es gibt einen eigenen Zustand, wenn Code ausgegeben wird

```
a{check html special state}
	if (s->state == hs_IN_CODE) {
		return n{false};
	}
x{check html special state}
```
* Die Code-Ausgabe ist ein besonderer Zustand

```
a{html state elements}
	t{int} codeOpening;
x{html state elements}
```
* Der Code-Block wird mit drei Backticks betreten und verlassen
* Die Anzahl der bisher gelesenen Backticks wird in `codeOpening`
  festgehalten

```
a{html state elements}
	t{int} codeIndent;
x{html state elements}
```
* Tabs am Anfang der Zeile werden durch passenden HTML-Elemente ersetzt
* Um die Einrückung nachzubilden
* In `codeIndent` wird der Einrück-Level gespeichert

```
a{html state elements}
	t{char} codeSpecial;
	t{char} codeName[n{100}];
	t{char *}codeNameEnd;
x{html state elements}
```
* Wenn ein Befehl gelesen wurde, dann enthält `codeSpecial` den
  Code des Befehls
* In `codeName` wird bis zu `codeEnd` das Argument abgelegt

```
a{init html status}
	, .codeOpening = n{0}
	, .codeIndent = n{0}
	, .codeSpecial = s{'\0'}
	, .codeNameEnd = n{NULL}
x{init html status}
```
* Zur Initialisierung werden die Parameter auf `n{0}` gesetzt
* Das Code-Argument `codeName` wird nicht zurückgesetzt

```
a{process ch for HTML} 
	if (newline && ch == s{'`'}) {
		if (isOutOfHtmlSpecial(&status) ||
			status.state == hs_IN_CODE
		) {
			++status.codeOpening;
			continue;
		}
	}
x{process ch for HTML}
```
* Wenn am Anfang der Zeile Backticks kommen, dann werden sie gezählt

```
a{process ch for HTML} 
	if (
		ch == s{'\n'} && status.codeOpening == n{3}
	) {
		status.codeOpening = n{0};
		if (isOutOfHtmlSpecial(&status)) {
			e{open code page};
			continue;
		} else if (
			status.state == hs_IN_CODE
		) {
			e{close code page};
			continue;
		}
	}
	status.codeOpening = n{0};
x{process ch for HTML}
```
* Wenn die Anzahl der Backticks genau `n{3}` war, dann wird der
  Code-Modus betreten oder verlassen

```
d{open code page}
	if (status.state == hs_IN_SLIDE) {
		fprintf(out, s{"</div>\n"});
	}
	fprintf(out, s{"<div><div>\n"});
	fprintf(out, s{"<code>\n"});
	status.state = hs_IN_CODE;
	E{move ch to last};
x{open code page}
```
* Beim Betreten wird eine Seite mit einem `<code>`-Tag geöffnet
* Und der Zustand passend gesetzt

```
d{close code page}
	fprintf(out, s{"</code>\n"});
	fprintf(out, s{"</div>\n"});
	status.state = hs_IN_SLIDE;
	status.codeIndent = n{0};
	status.codeSpecial = s{'\0'};
	E{move ch to last};
x{close code page}
```
* Beim Verlassen wird die Folie (aber nicht die Seite) geschlossen

```
a{process ch for HTML}
	if (status.state == hs_IN_CODE) {
		if (ch == n{EOF}) {
			fprintf(
				stderr,
				s{"unterminated code block\n"}
			);
			break;
		}
	}
x{process ch for HTML}
```
* Wenn wir beim Beenden des Parsens noch im Code-Modus sind, dann
  stimmt etwas nicht
* Eine Fehlermeldung wird ausgegeben

```
A{includes}
	#include <ctype.h>
x{includes}
```

```
a{process ch for HTML}
	if (status.state == hs_IN_CODE) {
		if (! status.codeNameEnd && isalnum(ch)) {
			addToBuffer(&ident, ch);
			continue;
		}
	}
x{process ch for HTML}
```

```
A{global elements}
	int compareBuffer(const char *s, const struct Buffer *b) {
		const char *x = b->buffer;
		while (*s && x != b->current) {
			if (*s == *x) {
				++s; ++x;
			} else if (*s < *x) {
				return -1;
			} else {
				return 1;
			}
		}
		if (*s) { return 1; }
		if (x != b->current) { return -1; }
		return 0;
	}
x{global elements}
```

```
A{global elements}
	void escapeIdent(
		FILE *out,
		const char *cls,
		const struct Buffer *b
	) {
		fprintf(
			out,
			"<span class=\"%s\">%.*s</span>",
			cls,
			(int) (b->current - b->buffer),
			b->buffer
		);
	}
x{global elements}
```

```
A{global elements}
	bool contains(
		const char **begin,
		const char **end,
		const struct Buffer *b
	) {
		for (; begin != end; ++begin) {
			if (compareBuffer(*begin, b) == 0) {
				return true;
			}
		}
		return false;
	}
x{global elements}
```

```
A{global elements}
	bool isKeyword(const struct Buffer *b) {
		static const char *begin[] = {
			s{"break"},
			s{"case"},
			s{"continue"},
			s{"default"},
			s{"else"},
			s{"for"},
			s{"if"},
			s{"return"},
			s{"static"},
			s{"switch"},
			s{"while"}
		};
		static const char **end = (void *) begin + sizeof(begin);
		return contains(begin, end, b);
	}
x{global elements}
```

```
d{flush pending}
	if (isActiveBuffer(&ident)) {
		if (ch == '(') {
			escapeIdent(out, "fn", &ident);
		} else if (isKeyword(&ident)) {
			escapeIdent(out, "keyword", &ident);
		} else {
			escapeIdent(out, "var", &ident);
		}
		resetBuffer(&ident);
	}
x{flush pending}
```

```
a{process ch for HTML}
	if (status.state == hs_IN_CODE) {
		e{process ch in HTML code};
		E{flush pending};
		E{move ch to last};
		continue;
	}
x{process ch for HTML}
```
* Zeichen wird im Code-Modus verarbeitet

```
d{process ch in HTML code} 
	if (ch == s{'\n'}) {
		E{flush pending};
		fprintf(out, s{"<br/>\n"});
		E{move ch to last};
		continue;
	}
x{process ch in HTML code}
```
* Bei Zeilen-Umbrüchen wird ein `<br/>`-Tag gesendet

```
a{process ch in HTML code} 
	if (newline && ch == s{'\t'}) {
		++status.codeIndent;
		continue;
	}
x{process ch in HTML code}
```
* Am Anfang der Zeile werden Tabulatoren gezählt

```
a{process ch in HTML code} 
	if (status.codeIndent) {
		fprintf(
			out,
			s{"<span class=\"in%d\"></span>"},
			status.codeIndent
		);
		status.codeIndent = 0;
	}
x{process ch in HTML code}
```
* Wenn es Tabulatoren gab, dann werden sie als Span-Element ausgegeben

```
a{process ch in HTML code}
	E{escape HTML code tag};
x{process ch in HTML code}
```
* Ansonsten werden Befehle in HTML-Tags expandiert

```
d{escape HTML code tag}
	if (ch == '{' && ident.current - ident.buffer == 1) {
		char lc = *ident.buffer;
		switch (lc) {
			e{escape html frag}
			default: break;
		}
		if (status.codeSpecial) {
			resetBuffer(&ident);
			newline = n{false};
			continue;
		}
	}
x{escape HTML code tag}
```
* Bei einer öffnenden Mengenklammer wird geprüft, ob ein bekannter
  Befehls-Code im vorherigen Zeichen liegt
* In diesem Fall wird die weiter Abarbeitung abgebrochen
* Statt dessen wurden passende Tags generiert und die nächsten Zeichen
  sind das Argument des Befehls
* Diese werden direkt kopiert

```
a{escape HTML code tag}
	if (ch == '}' && status.codeSpecial) {
		E{flush pending};
		resetBuffer(&ident);
		newline = n{false};
		if (status.codeSpecial != 'i') {
			writeEscaped(out, status.codeName, status.codeNameEnd);
		}
		switch (status.codeSpecial) {
			e{handle special codes}
		}
		fprintf(out, s{"</span>"});
		status.codeSpecial = n{0};
		status.codeNameEnd = n{NULL};
		continue;
	}
x{escape HTML code tag}
```
* Nach dem Lesen eines Arguments muss das HTML-Tag wieder geschlossen
  werden
* Bei besonderen Befehlen müssen mehrere Tags geschlossen werden

```
d{handle special codes}
	case s{'i'}: {
		e{handle html include};
		break;
	}
	case s{'a'}: case s{'e'}: case s{'E'}: case s{'x'}:
	case s{'g'}: case s{'G'}: case s{'A'}: case s{'D'}:
	case s{'R'}:
	case s{'r'}: case s{'d'}: case s{'p'}: case s{'m'}: {
		fprintf(out, s{"</span>)"});
	}
x{handle special codes}
```
* `@include`-Befehle haben eine Sonderbehandlung, da sie einen Link
  generieren
* Ansonsten werden alle Befehle, die nicht der Formatierung gelten mit
  zwei schließenden Tags abgeschlossen

```
d{handle html include}
	ASSERT(
		status.codeNameEnd <
		status.codeName +
			sizeof(status.codeName)
	);
	*status.codeNameEnd = s{'\0'};
	while (
		status.codeNameEnd >= status.codeName
		&& *status.codeNameEnd != s{'.'}
	) {
		--status.codeNameEnd;
	}
x{handle html include}
```
* Bei der Behandlung von Includes wurde der Dateiname in `codeName`
  abgelegt
* Hier wird von hinten alles bis zum ersten Punkt abgeschnitten
* Also wird die Dateiextension aus dem Namen entfernt

```
a{handle html include}
	ASSERT(
		status.codeNameEnd >= status.codeName,
		s{"no period"}
	);
	*status.codeNameEnd = s{'\0'};
	fprintf(out,
		s{"<a href=\"%s.html\">"},
		status.codeName
	);
	*status.codeNameEnd = s{'.'};
	fprintf(out,
		s{"%s</a>)</span>"}, status.codeName
	);
	status.codeNameEnd = n{NULL};
x{handle html include}
```
* Statt der ursprünglichen `.x`-Datei verweist der Link auf eine
  HTML-Datei
* Mit dem gleichen Basis-Namen

```
a{process ch in HTML code}
	if (ch != n{EOF} && status.codeNameEnd) {
		ASSERT(
			status.codeNameEnd <
				status.codeName +
					sizeof(status.codeName)
		);
		*status.codeNameEnd++ = ch;
		continue;
	}
x{process ch in HTML code}
```
* Wenn das Argument gespeichert werden soll (d.h. wenn `codeNameEnd`
  nicht `n{NULL}`)
* Wird das aktuelle Zeichen im `codeName` gespeichert

```
d{escape html frag}
	case s{'d'}:
		fprintf(out,
			s{"<span class=\"add\">@def("}
		);
		fprintf(out,
			s{"<span class=\"name\">"}
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'D'}:
		fprintf(out,
			s{"<span class=\"add\">@globdef("}
		);
		fprintf(out,
			s{"<span class=\"name\">"}
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'a'}:
		fprintf(out,
			s{"<span class=\"add\">@add("}
		);
		fprintf(out,
			s{"<span class=\"name\">"}
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'A'}:
		fprintf(out,
			s{"<span class=\"add\">@globadd("}
		);
		fprintf(out,
			s{"<span class=\"name\">"}
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'r'}:
		fprintf(out,
			s{"<span class=\"add\">@replace("}
		);
		fprintf(out,
			s{"<span class=\"name\">"}
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'R'}:
		fprintf(out,
			s{"<span class=\"add\">@globreplace("}
		);
		fprintf(out,
			s{"<span class=\"name\">"}
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'x'}:
		fprintf(out,
			s{"<span class=\"end\">@end("}
		);
		fprintf(out,
			s{"<span class=\"name\">"}
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'e'}:
		fprintf(out,
			s{"<span class=\"expand\">@expand("}
		);
		fprintf(out,
			s{"<span class=\"name\">"}
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'E'}:
		fprintf(out,
			s{"<span class=\"expand\">@multiple("}
		);
		fprintf(out,
			s{"<span class=\"name\">"}
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'g'}:
		fprintf(out,
			s{"<span class=\"expand\">"}
				s{"@globexpand("}
		);
		fprintf(out,
			s{"<span class=\"name\">"}
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'G'}:
		fprintf(out,
			s{"<span class=\"expand\">"}
				s{"@globmult("}
		);
		fprintf(out,
			s{"<span class=\"name\">"}
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'i'}:
		fprintf(out,
			s{"<span class=\"include\">@include("}
		);
		fprintf(out,
			s{"<span class=\"name\">"}
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'t'}:
		fprintf(out, s{"<span class=\"type\">"});
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'v'}:
		fprintf(out, s{"<span class=\"var\">"});
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'f'}:
		fprintf(out, s{"<span class=\"fn\">"});
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'k'}:
		fprintf(out, s{"<span class=\"keyword\">"});
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'s'}:
		fprintf(out, s{"<span class=\"str\">"});
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'n'}:
		fprintf(out, s{"<span class=\"num\">"});
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'p'}:
		fprintf(out,
			s{"<span class=\"type\">"}
				s{"@priv(<span>"}
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case s{'m'}:
		fprintf(
			out,
			s{"<span class=\"num\">"}
				s{"@magic(<span>"}
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```
* `@magic`-Befehle werden als Zahlen formatiert

# Notizen

```
a{html state elements}
	t{bool} noteInCode;
	t{bool} noteInBold;
x{html state elements}
```

```
a{init html status}
	, .noteInCode = n{false}
	, .noteInBold = n{false}
x{init html status}
```

```
a{html state enums}
	, hs_IN_NOTES
x{html state enums}
```

```
a{check html special state}
	if (s->state == hs_IN_NOTES) {
		return n{false};
	}
x{check html special state}
```

```
a{process ch for HTML} 
	if (
		newline &&
		status.state == hs_IN_NOTES
	) {
		if (ch == s{'*'}) {
			fprintf(out, s{"</li><li>\n"});
			resetBuffer(&ident);
			newline = n{false};
			continue;
		} else if (ch != s{' '} && ch != s{'\t'}) {
			fprintf(out, s{"</li></ul></div>\n"});
			status.state = hs_AFTER_SLIDE;
			E{move ch to last};
			continue;
		}
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	if (newline && ch == s{'*'}) {
		if (isOutOfHtmlSpecial(&status)) {
			if (status.state != hs_IN_SLIDE) {
				fprintf(out, s{"<div>\n"});
			}
			status.state = hs_IN_NOTES;
			fprintf(out, s{"<ul><li>\n"});
			resetBuffer(&ident);
			newline = n{false};
			continue;
		}
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	if (
		ch == s{'`'} &&
		status.state == hs_IN_NOTES
	) {
		E{flush pending};
		if (status.noteInCode) {
			fprintf(out, s{"</code>"});
		} else {
			fprintf(out, s{"<code>"});
		}
		status.noteInCode = ! status.noteInCode;
		resetBuffer(&ident);
		newline = n{false};
		continue;
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	if (status.state == hs_IN_NOTES &&
	status.noteInCode) {
		E{escape HTML code tag};
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	if (
		false &&
		ch == s{'*'} && /*last == s{'*'} && */
		status.state == hs_IN_NOTES
	) {
		if (status.noteInBold) {
			fprintf(out, s{"</b>"});
		} else {
			fprintf(out, s{"<b>"});
		}
		status.noteInBold =
			! status.noteInBold;
		newline = n{false};
		continue;
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	if (status.state == hs_IN_NOTES) {
		E{flush pending};
		E{move ch to last};
		continue;
	}
x{process ch for HTML}
```
