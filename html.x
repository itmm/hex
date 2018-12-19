# HTML-Präsentation generieren
* Diese Datei beschreibt Code, um aus den `x`-Quelldateien
  HTML-Präsentationen zu erzeugen

```
D{write HTML file}
	for (auto &cur : used) {
		e{write cur HTML file};
	}
	used.clear();
x{write HTML file}
```
* Alle bisher prozessierten Dateien werden erneut durchgegangen
* Nach der Abarbeitung einer Datei wird deren Speicher freigegeben

```
d{write cur HTML file}
	const std::string &name = cur->name;
	std::string outPath =
		name.substr(0, name.size() - 2) +
		".html";
	FILE *out = fopen(outPath.c_str(), "w");
	ASSERT(out);
	e{write cur HTML file to out};
	fclose(out);
x{write cur HTML file}
```
* Die HTML hat den gleichen Pfad mit der Endung `.html` anstatt `.x`

```
d{write cur HTML file to out} 
	FILE *in = fopen(cur->name.c_str(), "r");
	ASSERT(in);
	e{write HTML file from in to out};
	fclose(in);
x{write cur HTML file to out}
```
* Zuerst wird die Eingabe-Datei zum Lesen geöffnet

```
A{global elements}
	enum HtmlState {
		nothingWritten,
		inSlide,
		afterSlide
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
	struct HtmlStatus {
		e{html state elements}
	};
x{global elements}
```
* Der aktuelle Status wird in `HtmlStatus` abgelegt

```
d{html state elements}
	HtmlStatus();
	enum HtmlState state;
x{html state elements}
```
* Der aktuelle Zustand wird im Status abgelegt

```
A{global elements}
	inline HtmlStatus::HtmlStatus():
		state(HtmlState::nothingWritten)
		e{init html status}
	{ }
x{global elements}
```

```
A{includes}
	#include <set>
	#include <string>
x{includes}
```

```
d{write HTML file from in to out} {
	HtmlStatus status;
	bool newline = true;
	std::string ident;
	for (;;) {
		int ch = fgetc(in);
		e{process ch for HTML};
		if (ch == EOF) { break; }
		E{move ch to last};
	}
} x{write HTML file from in to out}
```
* Beim Schreiben einer Datei wird zuerst der Status initialisiert
* Wir befinden uns am Anfang einer Zeile
* Jedes Zeichen wird prozessiert
* Bis das Datei-Ende erreicht ist

# Überschriften

```
d{html state enums}
	, inHeader
x{html state enums}
```
* Es gibt einen eigenen Zustand, wenn eine Überschrift gelesen wird

```
d{move ch to last}
	newline = ch == '\n';
	if (status.state != HtmlState::inHeader) {
		char xx = ch;
		writeEscaped(out, &xx, &xx + 1);
	}
x{move ch to last}
```
* Beim Kopieren des Zeichens wird `EOF` durch `'\0'` ersetzt

```
A{global elements}
	bool isOutOfHtmlSpecial(
		HtmlStatus *s
	) {
		e{check html special state};
		return true;
	}
x{global elements}
```
* Die Funktion `{isOutOfHtmlSpecial}` liefert `true`, wenn sich
  der Zustands-Automat gerade nicht in einem Sonder-Modus (Überschrift,
  Code, Notizen) befindet

```
d{check html special state}
	if (s->state == HtmlState::inHeader) {
		return false;
	}
x{check html special state}
```
* Die Überschrift ist ein Sonder-Modus

```
a{html state elements}
	int headerLevel;
	std::string headerName;
	enum HtmlState headerState;
x{html state elements}
```
* Für Überschriften wird abgelegt, auf welchem Level sie vorkommen
* Und der Titel der Überschrift wird gespeichert
* Und der Zustand in dem sich der Parser beim Auftreten der Überschrift
  befunden hat

```
d{init html status}
	, headerLevel(0)
	, headerName()
x{init html status}
```
* Level und Name werden leer initialisiert

```
d{process ch for HTML} 
	if (ch == '#' && newline) {
		if (isOutOfHtmlSpecial(&status) ||
			status.state == HtmlState::inHeader
		) {
			++status.headerLevel;
			if (status.state != HtmlState::inHeader) {
				status.headerState =
					status.state;
			}
			status.state = HtmlState::inHeader;
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
	if (status.state == HtmlState::inHeader) {
		if (ch == '\n') {
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
	status.state = HtmlState::inSlide;
	status.headerLevel = 0;
	status.headerName.clear();
	status.headerState = HtmlState::inSlide;
x{reset header state}
```
* Beim Zurücksetzen des Zustands wird sichergestellt, das der Level und
  Name der Überschrift zurückgesetzt sind

```
a{process ch for HTML} 
	if (status.state == HtmlState::inHeader) {
		if (! status.headerName.empty()) {
			status.headerName.push_back(ch);
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
	if (status.state == HtmlState::inHeader) {
		if (ch > ' ' && status.headerName.empty()) {
			status.headerName.push_back(ch);
			E{move ch to last};
			continue;
		}
	}
x{process ch for HTML}
```
* Leerzeichen zwischen den Rauten und der Überschrift werden ignoriert

```
d{process header in HTML} 
	ASSERT(! status.headerName.empty());
	e{close previous HTML page};
	E{write header tag};
	fprintf(out, "<div class=\"slides\">\n");
	fprintf(out, "<div><div>\n");
	E{write header tag};
	fprintf(out, "</div>\n");
x{process header in HTML}
```
* Falls schon eine Seite offen ist, dann wird diese geschlossen
* Dann wird eine HTML-Überschrift ausgegeben
* Und eine Folie mit einer HTML-Überschrift wird erzeugt

```
A{global elements} 
	template <typename T>
	void writeEscaped(
		FILE *out, T str, T end
	) {
		ASSERT(out);
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
	case '<':
		fprintf(out, "&lt;");
		break;
	case '>':
		fprintf(out, "&gt;");
		break;
	case '&':
		fprintf(out, "&amp;");
		break;
x{escape special}
```
* Die Zeichen `<`, `>` und `&` müssen ersetzt werden

```
d{write header tag} 
	fprintf(out, "<h%d>", status.headerLevel);
	writeEscaped(
		out,
		status.headerName.begin(),
		status.headerName.end()
	);
	fprintf(out, "</h%d>\n", status.headerLevel);
x{write header tag}
```
* Die HTML-Überschrift enthält den eingelesenen Level

```
d{close previous HTML page} 
	switch (status.headerState) {
		case HtmlState::nothingWritten: {
			e{write HTML header};
			break;
		}
		case HtmlState::inSlide: {
			fprintf(out, "</div>\n");
			fprintf(out, "</div>\n");
			break;
		}
		default: {
			fprintf(out, "</div>\n");
		}
	}
x{close previous HTML page}
```
* Je nach Zustand müssen unterschiedlich viele HTML-Tags geschlossen
  werden

```
d{write HTML header} 
	fprintf(out, "<!doctype html>\n");
	fprintf(out, "<html lang=\"de\"l>\n");
	fprintf(out, "<head>\n");
	e{write HTML header entries};
	fprintf(out, "</head>\n");
	fprintf(out, "<body>\n");
x{write HTML header}
```
* Dies wird am Anfang der HTML-Datei ausgegeben
* Dafür muss jede `.x`-Datei mit einer Überschrift beginnen
* Diese Überschrift ist dann auch der Titel der HTML-Datei

```
d{write HTML header entries} 
	fprintf(
		out, "<meta charset=\"utf-8\">\n"
	);
	fprintf(out, "<title>");
	writeEscaped(
		out,
		status.headerName.begin(),
		status.headerName.end()
	);
	fprintf(out, "</title>");
	fprintf(
		out, "<link rel=\"stylesheet\" "
		"type=\"text/css\" href=\"%s\">",
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
	, inCode
x{html state enums}
```
* Es gibt einen eigenen Zustand, wenn Code ausgegeben wird

```
a{check html special state}
	if (s->state == HtmlState::inCode) {
		return false;
	}
x{check html special state}
```
* Die Code-Ausgabe ist ein besonderer Zustand

```
a{html state elements}
	int codeOpening;
x{html state elements}
```
* Der Code-Block wird mit drei Backticks betreten und verlassen
* Die Anzahl der bisher gelesenen Backticks wird in `codeOpening`
  festgehalten

```
a{html state elements}
	int codeIndent;
x{html state elements}
```
* Tabs am Anfang der Zeile werden durch passenden HTML-Elemente ersetzt
* Um die Einrückung nachzubilden
* In `codeIndent` wird der Einrück-Level gespeichert

```
a{html state elements}
	char codeSpecial;
	char codeName[100];
	char *codeNameEnd;
x{html state elements}
```
* Wenn ein Befehl gelesen wurde, dann enthält `codeSpecial` den
  Code des Befehls
* In `codeName` wird bis zu `codeEnd` das Argument abgelegt

```
a{init html status}
	, codeOpening(0)
	, codeIndent(0)
	, codeSpecial('\0')
	, codeNameEnd(nullptr)
x{init html status}
```
* Zur Initialisierung werden die Parameter auf `0` gesetzt
* Das Code-Argument `codeName` wird nicht zurückgesetzt

```
a{process ch for HTML} 
	if (newline && ch == '`') {
		if (isOutOfHtmlSpecial(&status) ||
			status.state == HtmlState::inCode
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
		ch == '\n' && status.codeOpening == 3
	) {
		status.codeOpening = 0;
		if (isOutOfHtmlSpecial(&status)) {
			e{open code page};
			continue;
		} else if (
			status.state == HtmlState::inCode
		) {
			e{close code page};
			continue;
		}
	}
x{process ch for HTML}
```
* Wenn die Anzahl der Backticks genau `3` war, dann wird der
  Code-Modus betreten oder verlassen

```
a{process ch for HTML}
	if (status.codeOpening == 1) {
		if (! status.codeSpecial && status.state == HtmlState::inCode) {
			status.codeSpecial = '`';
			status.codeNameEnd = status.codeName;
			E{flush pending};
			if (status.codeIndent) {
				fprintf(
					out,
					"<span class=\"in%d\"></span>",
					status.codeIndent
				);
				status.codeIndent = 0;
			}
			fprintf(out, "<span class=\"str\">`");
		} else if (
			status.codeSpecial == '`' && status.codeNameEnd[-1] != '\x5c'
		) {
			E{flush pending};
			writeEscaped(out, status.codeName, status.codeNameEnd);
			fprintf(out, "`</span>");
			status.codeSpecial = 0;
			status.codeNameEnd = nullptr;
		}
	}
x{process ch for HTML}
```

```
a{process ch for HTML}
	status.codeOpening = 0;
x{process ch for HTML}
```

```
d{open code page}
	if (status.state == HtmlState::inSlide) {
		fprintf(out, "</div>\n");
	}
	fprintf(out, "<div><div>\n");
	fprintf(out, "<code>\n");
	status.state = HtmlState::inCode;
	E{move ch to last};
x{open code page}
```
* Beim Betreten wird eine Seite mit einem `<code>`-Tag geöffnet
* Und der Zustand passend gesetzt

```
d{close code page}
	fprintf(out, "</code>\n");
	fprintf(out, "</div>\n");
	status.state = HtmlState::inSlide;
	status.codeIndent = 0;
	status.codeSpecial = '\0';
	E{move ch to last};
x{close code page}
```
* Beim Verlassen wird die Folie (aber nicht die Seite) geschlossen

```
a{process ch for HTML}
	if (status.state == HtmlState::inCode) {
		if (ch == EOF) {
			fprintf(
				stderr,
				"unterminated code block\n"
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
	if (status.state == HtmlState::inCode) {
		if (! status.codeSpecial && (isalnum(ch) || ch == '_')) {
			ident.push_back(ch);
			continue;
		}
	}
x{process ch for HTML}
```

```
A{global elements}
	void escapeIdent(
		FILE *out,
		const char *cls,
		const std::string &s
	) {
		fprintf(
			out,
			"<span class=\"%s\">%.*s</span>",
			cls,
			(int) s.size(),
			s.data()
		);
	}
x{global elements}
```

```
A{global elements}
	bool isKeyword(const std::string &s) {
		static std::set<std::string> reserved {
			"break", "case", "catch", "continue",
			"default", "delete", "else", "for",
			"if", "in", "new", "return", "static",
			"switch", "try", "typeof", "while"
		};
		return reserved.find(s) != reserved.end();
	}
x{global elements}
```

```
A{global elements}
	bool isType(const std::string &s) {
		static std::set<std::string> reserved {
			"FILE",
			"bool", "char", "const", "enum",
			"int", "let", "long", "signed", "struct",
			"union", "unsigned", "void"
		};
		if (reserved.find(s) != reserved.end()) {
			return true;
		}
		if (s.size() >= 2) {
			if (isupper(s[0]) && islower(s[1])) {
				return true;
			}
		}
		return false;
	}
x{global elements}
```

```
A{global elements}
	bool isNum(const std::string &s) {
		static std::set<std::string> reserved {
			"EOF", "NULL", "nullptr",
			"false", "null", "true", "undefined"
		};
		if (isdigit(s[0])) { return true; }
		return reserved.find(s) != reserved.end();
	}
x{global elements}
```

```
d{flush pending}
	if (! ident.empty()) {
		if (ch == '(') {
			escapeIdent(out, "fn", ident);
		} else if (isKeyword(ident)) {
			escapeIdent(out, "keyword", ident);
		} else if (isType(ident)) {
			escapeIdent(out, "type", ident);
		} else if (isNum(ident)) {
			escapeIdent(out, "num", ident);
		} else {
			escapeIdent(out, "var", ident);
		}
		ident.clear();
	}
x{flush pending}
```

```
a{process ch for HTML}
	if (status.state == HtmlState::inCode) {
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
	if (ch == '\n') {
		E{flush pending};
		fprintf(out, "<br/>\n");
		E{move ch to last};
		continue;
	}
x{process ch in HTML code}
```
* Bei Zeilen-Umbrüchen wird ein `<br/>`-Tag gesendet

```
a{process ch in HTML code} 
	if (newline && ch == '\t') {
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
			"<span class=\"in%d\"></span>",
			status.codeIndent
		);
		status.codeIndent = 0;
	}
x{process ch in HTML code}
```
* Wenn es Tabulatoren gab, dann werden sie als Span-Element ausgegeben

```
a{process ch in HTML code}
	if (! status.codeSpecial && (ch == '\'' || ch == '"' || ch == '`')) {
		status.codeSpecial = ch;
		status.codeNameEnd = status.codeName;
		fprintf(out, "<span class=\"str\">%c", ch);
		continue;
	}
x{process ch in HTML code}
```

```
a{process ch in HTML code}
	E{escape HTML code tag};
x{process ch in HTML code}
```
* Ansonsten werden Befehle in HTML-Tags expandiert

```
d{escape HTML code tag}
	if (ch == '{' && ident.size() == 1) {
		char lc = ident.front();
		switch (lc) {
			e{escape html frag}
			default: break;
		}
		if (status.codeSpecial) {
			ident.clear();
			newline = false;
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
	if (
		(
			ch == '}' &&
			status.codeSpecial &&
			status.codeSpecial != '\'' &&
			status.codeSpecial != '"' &&
			status.codeSpecial != '`'
		) 
	||
		(
			(
				status.codeSpecial == '\'' ||
				status.codeSpecial == '"' ||
				status.codeSpecial == '`'
			) &&
			ch == status.codeSpecial &&
			status.codeNameEnd[-1] != '\x5c'
		)
	) {
		E{flush pending};
		ident.clear();
		newline = false;
		if (status.codeSpecial != 'i') {
			writeEscaped(out, status.codeName, status.codeNameEnd);
		}
		switch (status.codeSpecial) {
			e{handle special codes}
		}
		if (status.codeSpecial != 'b') {
			fprintf(out, "</span>");
		}
		status.codeSpecial = 0;
		status.codeNameEnd = nullptr;
		continue;
	}
x{escape HTML code tag}
```
* Nach dem Lesen eines Arguments muss das HTML-Tag wieder geschlossen
  werden
* Bei besonderen Befehlen müssen mehrere Tags geschlossen werden

```
d{handle special codes}
	case 'i': {
		e{handle html include};
		break;
	}
	case 'a': case 'e': case 'E': case 'x':
	case 'g': case 'G': case 'A': case 'D':
	case 'R':
	case 'r': case 'd': case 'p': case 'm': {
		fprintf(out, "</span>)");
		break;
	}
	case '\'': case '"': case '`': {
		fputc(status.codeSpecial, out);
		break;
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
	*status.codeNameEnd = '\0';
	while (
		status.codeNameEnd >= status.codeName
		&& *status.codeNameEnd != '.'
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
		"no period"
	);
	*status.codeNameEnd = '\0';
	fprintf(out,
		"<a href=\"%s.html\">",
		status.codeName
	);
	*status.codeNameEnd = '.';
	fprintf(out,
		"%s</a>)</span>", status.codeName
	);
	status.codeNameEnd = nullptr;
x{handle html include}
```
* Statt der ursprünglichen `.x`-Datei verweist der Link auf eine
  HTML-Datei
* Mit dem gleichen Basis-Namen

```
a{process ch in HTML code}
	if (ch != EOF && status.codeNameEnd) {
		ASSERT(
			status.codeNameEnd <
				status.codeName +
					sizeof(status.codeName),
			" [%c], [%.*s]", status.codeSpecial,
			(int) sizeof(status.codeName), status.codeName
		);
		*status.codeNameEnd++ = ch;
		continue;
	}
x{process ch in HTML code}
```
* Wenn das Argument gespeichert werden soll (d.h. wenn `codeNameEnd`
  nicht `nullptr`)
* Wird das aktuelle Zeichen im `codeName` gespeichert

```
d{escape html frag}
	case 'd':
		fprintf(out,
			"<span class=\"add\">@def("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'D':
		fprintf(out,
			"<span class=\"add\">@globdef("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'a':
		fprintf(out,
			"<span class=\"add\">@add("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'A':
		fprintf(out,
			"<span class=\"add\">@globadd("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'r':
		fprintf(out,
			"<span class=\"add\">@replace("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'R':
		fprintf(out,
			"<span class=\"add\">@globreplace("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'x':
		fprintf(out,
			"<span class=\"end\">@end("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'e':
		fprintf(out,
			"<span class=\"expand\">@expand("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'E':
		fprintf(out,
			"<span class=\"expand\">@multiple("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'g':
		fprintf(out,
			"<span class=\"expand\">"
				"@globexpand("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'G':
		fprintf(out,
			"<span class=\"expand\">"
				"@globmult("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'i':
		fprintf(out,
			"<span class=\"include\">@include("
		);
		fprintf(out,
			"<span class=\"name\">"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 't':
		fprintf(out, "<span class=\"type\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'v':
		fprintf(out, "<span class=\"var\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'f':
		fprintf(out, "<span class=\"fn\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'k':
		fprintf(out, "<span class=\"keyword\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 's':
		fprintf(out, "<span class=\"str\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'n':
		fprintf(out, "<span class=\"num\">");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'p':
		fprintf(out,
			"<span class=\"type\">"
				"@priv(<span>"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```

```
a{escape html frag}
	case 'm':
		fprintf(
			out,
			"<span class=\"num\">"
				"@magic(<span>"
		);
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```
* `@magic`-Befehle werden als Zahlen formatiert

```
a{escape html frag}
	case 'b':
		fprintf(out, "<span class=\"virt\"></span><br/>");
		status.codeSpecial = lc;
		status.codeNameEnd = status.codeName;
		break;
x{escape html frag}
```
* `@magic`-Befehle werden als Zahlen formatiert

# Notizen

```
a{html state elements}
	bool noteInCode;
	bool noteInBold;
x{html state elements}
```

```
a{init html status}
	, noteInCode(false)
	, noteInBold(false)
x{init html status}
```

```
a{html state enums}
	, inNotes
x{html state enums}
```

```
a{check html special state}
	if (s->state == HtmlState::inNotes) {
		return false;
	}
x{check html special state}
```

```
a{process ch for HTML} 
	if (
		newline &&
		status.state == HtmlState::inNotes
	) {
		if (ch == '*') {
			fprintf(out, "</li><li>\n");
			ident.clear();
			newline = false;
			continue;
		} else if (ch != ' ' && ch != '\t') {
			fprintf(out, "</li></ul></div>\n");
			status.state = HtmlState::afterSlide;
			E{move ch to last};
			continue;
		}
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	if (newline && ch == '*') {
		if (isOutOfHtmlSpecial(&status)) {
			if (status.state != HtmlState::inSlide) {
				fprintf(out, "<div>\n");
			}
			status.state = HtmlState::inNotes;
			fprintf(out, "<ul><li>\n");
			ident.clear();
			newline = false;
			continue;
		}
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	if (
		ch == '`' &&
		status.state == HtmlState::inNotes
	) {
		E{flush pending};
		if (status.noteInCode) {
			fprintf(out, "</code>");
		} else {
			fprintf(out, "<code>");
		}
		status.noteInCode = ! status.noteInCode;
		ident.clear();
		newline = false;
		continue;
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	if (status.state == HtmlState::inNotes &&
	status.noteInCode) {
		E{escape HTML code tag};
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	if (
		false &&
		ch == '*' && /*last == '*' && */
		status.state == HtmlState::inNotes
	) {
		if (status.noteInBold) {
			fprintf(out, "</b>");
		} else {
			fprintf(out, "<b>");
		}
		status.noteInBold =
			! status.noteInBold;
		newline = false;
		continue;
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	if (status.state == HtmlState::inNotes) {
		E{flush pending};
		E{move ch to last};
		continue;
	}
x{process ch for HTML}
```
