# HTML-Präsentation generieren
* Diese Datei beschreibt Code, um aus den `x`-Quelldateien
  HTML-Präsentationen zu erzeugen

```
D{write HTML file}
	for (auto &cur : inputs) {
		e{write cur HTML file};
	}
x{write HTML file}
```
* Alle bisher prozessierten Dateien werden erneut durchgegangen
* Nach der Abarbeitung einer Datei wird deren Speicher freigegeben

```
d{write cur HTML file}
	const std::string &name { cur->name };
	std::string outPath {
		name.substr(0, name.size() - 2) +
		".html"
	};
	std::ofstream out { outPath.c_str() };
	e{write cur HTML file to out};
	out.close();
x{write cur HTML file}
```
* Die HTML hat den gleichen Pfad mit der Endung `.html` anstatt `.x`

```
d{write cur HTML file to out} 
	std::ifstream in {
		cur->name.c_str()
	};
	e{write from in to out};
	in.close();
x{write cur HTML file to out}
```
* Zuerst wird die Eingabe-Datei zum Lesen geöffnet

```
A{global elements}
	enum class HtmlState {
		nothing,
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
	HtmlState state;
x{html state elements}
```
* Status hat einen Konstruktor
* Der aktuelle Zustand wird im Status abgelegt

```
A{global elements}
	inline HtmlStatus::HtmlStatus():
		state {
			HtmlState::nothing
		}
	{ }
x{global elements}
```
* Der initiale Zustand signalisiert, dass noch nichts geschrieben wurde

```
A{includes}
	#include <string>
x{includes}
```
* Die Anwendung verwendet `std::string` überall

```
d{write from in to out}
	HtmlStatus status;
	std::string ident;
	std::string line;
	while (std::getline(in, line)) {
		e{process line};
	}
x{write from in to out}
```
* Beim Schreiben einer Datei wird zuerst der Status initialisiert
* Die Eingabe wird Zeile für Zeile abgearbeitet
* Bis das Datei-Ende erreicht ist

```
A{global elements}
	bool in_code(
		HtmlStatus *s
	) {
		e{check html special state};
		return false;
	}
x{global elements}
```
* Die Funktion `f{in_code}` liefert `true`, wenn sich
  der Zustands-Automat gerade in einem Fragment befindet

```
d{process line} 
	if (in_code(&status)) {
		e{process code};
		continue;
	}
x{process line}
```
* Während Code bearbeitet wird, gelten viele andere Regeln nicht mehr
* Daher wird dieser Block als erstes abgehandelt

```
a{process line}
	if (line == "") {
		e{close specials};
		switch (status.state) {
			case HtmlState::afterSlide:
			case HtmlState::nothing:
				break;
			default:
				e{close slide};
		}
		continue;
	}
x{process line}
```
* Eine leere Seite schließt eine offene Folie ab

```
d{close slide}
	out << "</div>\n";
	status.state = HtmlState::afterSlide;
x{close slide}
```
* Die Funktion schreibt das schließende `</div>`
* Und setzt den Zustand passend

# Überschriften
* Überschriften beginnen mit Rauten `#`

```
a{process line}
	if (line[0] == '#') {
		e{process header};
		status.state = HtmlState::inSlide;
		continue;
	}
x{process line}
```
* Wenn am Anfang einer Zeile eine Raute gelesen wird, dann beginnt eine
  Überschrift

```
d{process header}
	int level = 1;
	while (
		level < (int) line.size() &&
			line[level] == '#'
	) {
		++level;
	}
x{process header}
```
* Solange weitere Rauten folgen, wird der Level erhöht

```
a{process header}
	auto e = line.end();
	auto b = line.begin() + level;
	while (b != e && *b <= ' ') {
		++b;
	}
x{process header}
```
* Nach den Rauten dürfen beliebig viele Leerzeichen vorkommen

```
a{process header} 
	ASSERT(b != e);
	std::string name {b, e};
	e{close previous HTML page};
	E{write header tag};
	out << "<div class=\"slides\">\n";
	out << "<div><div>\n";
	E{write header tag};
	out << "</div>\n";
x{process header}
```
* Falls schon eine Seite offen ist, dann wird diese geschlossen
* Dann wird eine HTML-Überschrift ausgegeben
* Und eine Folie mit einer HTML-Überschrift wird erzeugt

```
A{global elements} 
	void writeOneEscaped(
		std::ostream &out, char ch
	) {
		switch (ch) {
			e{escape special}
			default:
				out << ch;
		}
	}
x{global elements}
```
* Schreibt ein Zeichen
* Zeichen mit besonderer Bedeutung in HTML ersetzt die Funktion durch
  Entitäten

```
A{global elements}
	void writeEscaped(
		std::ostream &out,
		const std::string &str
	) {
		for (char ch : str) {
			writeOneEscaped(out, ch);
		}
	}
x{global elements}
```
* Schreibt mehrere Zeichen
* Zeichen mit besonderer Bedeutung in HTML ersetzt die Funktion durch
  Entitäten

```
d{escape special}
	case '<':
		out << "&lt;";
		break;
	case '>':
		out << "&gt;";
		break;
	case '&':
		out << "&amp;";
		break;
x{escape special}
```
* Die Zeichen `<`, `>` und `&` werden ersetzt

```
d{write header tag} 
	out << "<h" << level << '>';
	writeEscaped(out, name);
	out << "</h" << level << ">\n";
x{write header tag}
```
* Die HTML-Überschrift enthält den eingelesenen Level

```
d{close previous HTML page} 
	switch (status.state) {
		case HtmlState::nothing: {
			e{write HTML header};
			break;
		}
		case HtmlState::inSlide: {
			out << "</div>\n";
			// fallthrough
		}
		default: {
			out << "</div>\n";
		}
	}
x{close previous HTML page}
```
* Je nach Zustand müssen unterschiedlich viele HTML-Tags geschlossen
  werden

```
d{write HTML header} 
	out << "<!doctype html>\n";
	out << "<html lang=\"de\">\n";
	out << "<head>\n";
	e{write HTML header entries};
	out << "</head>\n";
	out << "<body>\n";
x{write HTML header}
```
* Dies wird am Anfang der HTML-Datei ausgegeben
* Dafür muss jede `.x`-Datei mit einer Überschrift beginnen
* Diese Überschrift ist dann auch der Titel der HTML-Datei

```
d{write HTML header entries} 
	out << "<meta charset=\"utf-8\">\n";
	out << "<title>";
	writeEscaped(out, name);
	out << "</title>\n";
	out << "<link rel=\"stylesheet\" "
		"type=\"text/css\" href=\""
		<< stylesheet << "\">";
x{write HTML header entries}
```
* Im Header wird das Zeichenformat auf UTF-8 gesetzt
* Und der Titel ausgegeben
* Und die Stylesheet-Datei eingebunden

# Code formatieren
* Code kann auf Seiten oder in den Notizen ausgegeben werden

```
d{html state enums}
	, inCode
x{html state enums}
```
* Es gibt einen eigenen Zustand, wenn Code ausgegeben wird

```
d{check html special state}
	if (s->state == HtmlState::inCode) {
		return true;
	}
x{check html special state}
```
* Die Code-Ausgabe ist ein besonderer Zustand


```
a{process line} 
	if (line == "```") {
		e{open code page};
		continue;
	}
x{process line}
```
* Eine Zeile mit drei Backticks wechselt in den Code Modus

```
d{open code page}
	if (
		status.state == HtmlState::inSlide
	) {
		out << "</div>\n";
	}
	out << "<div><div>\n";
	out << "<code>\n";
	status.state = HtmlState::inCode;
x{open code page}
```
* Beim Betreten wird eine Seite mit einem `<code>`-Tag geöffnet
* Und der Zustand passend gesetzt

```
d{process code}
	if (line == "```") {
		e{close code page};
		continue;
	}
x{process code}
```
* Eine Zeile mit drei Backticks wechselt aus dem Code Modus zurück

```
d{close code page}
	out << "</code></div>\n";
	status.state = HtmlState::inSlide;
x{close code page}
```
* Beim Verlassen wird die Folie (aber nicht die Seite) geschlossen

```
a{write from in to out}
	if (
		status.state == HtmlState::inCode
	) {
		std::cerr <<
			"unterminated code block\n";
	}
x{write from in to out}
```
* Wenn wir beim Beenden des Parsens noch im Code-Modus sind, dann
  stimmt etwas nicht
* Eine Fehlermeldung wird ausgegeben

```
a{write from in to out}
	if (
		status.state != HtmlState::nothing
	) {
		out << "</body>\n</html>\n";
	}
x{write from in to out}
```
* Am Ende der Datei schließt die Funktion
  die HTML-Tags

```
A{global elements}
	e{process code helper};
	void process_code(
		std::ostream &out,
		SI begin, SI end
	) {
		e{do code};
	}
x{global elements}
```
* Verarbeitet ein Code-Fragment
* Dies Funktion wird im Code Modus verwendet
* Und wenn in Notizen Code-Fragmente eingebettet sind

```
a{process code}
	process_code(
		out, line.begin(), line.end()
	);
	out << "<br/>\n";
x{process code}
```
* Im Code Modus wird die Funktion aufgerufen
* Und danach ein Zeilenumbruch ausgegeben

```
d{do code}
	int indent = 0;
	while (
		begin != end && *begin == '\t'
	) {
		++indent; ++begin;
	}
	if (indent) {
		out << "<span class=\"in"
			<< indent
			<< "\"></span>";
	}
x{do code}
```
* Die Funktion zählt Tabulatoren am Anfang der Zeile
* Und gibt ein Tag für die passende Einrückung aus

```
a{do code}
	for (; begin != end; ++begin) {
		e{process code ch};
		writeOneEscaped(out, *begin);
	}
x{do code}
```
* Danach wird die Zeile Zeichen für Zeichen verarbeitet
* Wenn keine besondere Verarbeitung geschieht, gibt die Funktion das
  Zeichen direkt aus

```
d{process code ch}
	if (
		*begin == '`' ||
		*begin == '\'' ||
		*begin == '"'
	) {
		e{process string};
		continue;
	}
x{process code ch}
```
* Wenn ein Zeichen eine Zeichenkette einleitet, wird diese geparst

```
d{process string}
	auto w = begin + 1;
	while (w != end && *w != *begin) {
		if (*w == '\\') {
			++w;
			if (w == end) { break; }
		}
		++w;
	}
	if (w == end) {
		writeOneEscaped(out, *begin);
		continue;
	}
x{process string}
```
* Die Zeichenkette endet mit dem gleichen Zeichen, mit dem sie angefangen
  hat
* Wenn dieses nicht durch einen Backslash escaped ist

```
d{process code helper}
	void span_str(
		std::ostream &out,
		const char *cls,
		const std::string &s
	) {
		out << "<span class=\"" <<
			cls << "\">";
		writeEscaped(out, s);
		out << "</span>";
	}
x{process code helper}
```
* Diese Hilfsfunktion umgibt einen Bezeichner mit einem `<span>`-Tag
  einer bestimmten Klasse

```
a{process string}
	std::string name {begin, w + 1};
	span_str(out, "str", name);
	begin = w;
x{process string}
```
* Die Funktion gibt die Zeichenkette als `<span>` mit der Klasse `s{str}`
  aus

```
A{includes}
	#include <cctype>
x{includes}
```
* Die Funktion verwendet `std::isalnum`

```
a{process code ch}
	auto w = begin;
	e{find identifier end};
	if (w != begin) {
		e{process identifier};
		continue;
	}
x{process code ch}
```
* An der aktuellen Position wird ein Identifier gesucht
* Wenn dieser gefunden wird, dann wird er abgearbeitet

```
d{find identifier end}
	while (w != end && (
		std::isalnum(*w) ||
			*w == '_' || *w == '$'
	)) {
		++w;
	}
x{find identifier end};
```
* Zeichen und Ziffern können einen Identifier bilden
* Zusätzlich noch der Unterstrich und das Dollar-Zeichen

```
d{process identifier}
	std::string ident {begin, w};
	begin = w - 1;
	if (w != end && *w == '{') {
		e{do macro};
	} else {
		process_ident(
			out, ident,
			w != end ? *w : ' '
		);
	}
x{process identifier}
```
* Wenn dem Identifier eine öffnende Mengen-Klammer folgt, dann wird
  ein Makro-Aufruf geparst
* `begin` wird am Anfang gesetzt, da die Makro-Verarbeitung dieses ggf.
  korrigiert
* Sonst wird der Identifier normal verarbeitet
* Bei der normalen Verarbeitung ist wichtig, welches Zeichen dem
  Identifier folgt

```
A{includes}
	#include <set>
x{includes}
```
* Besondere Bezeichner werden in `std::set` abgelegt

```
a{process code helper}
	using Set = std::set<std::string>;

	bool isKeyword(const std::string &s) {
		static Set reserved {
			e{keywords}
		};
		return
			reserved.find(s) !=
				reserved.end();
	}
x{process code helper}
```
* Prüft ob ein Bezeichner ein reserviertes Schlüsselwort ist

```
d{keywords}
	"break", "case", "catch", "continue",
	"default", "delete", "else", "for",
	"if", "in", "new", "return", "static",
	"switch", "try", "typeof", "while"
x{keywords}
```
* Reservierte Schlüsselwörter

```
a{process code helper}
	bool isType(const std::string &s) {
		e{is type};
		return false;
	}
x{process code helper}
```
* Prüft, ob ein Bezeichner ein Typ ist

```
d{is type}
	static Set reserved {
		e{types}
	};
	if (reserved.find(s) !=
		reserved.end()
	) {
		return true;
	}
x{is type}
```
* Wenn der Bezeichner ein reservierter Typ ist, liefert die Funktion
  `true`

```
d{types}
	"FILE", "auto", "bool", "char",
	"const", "enum", "extern", "int",
	"let", "long", "signed", "struct",
	"union", "unsigned", "void"
x{types}
```
* Reservierte Typen

```
a{is type}
	if (s.size() >= 2) {
		if (isupper(s[0]) &&
			islower(s[1])
		) {
			return true;
		}
	}
x{is type}
```
* Ein Bezeichner ist zusätzlich ein Typ, wenn das sei erstes Zeichen
  groß geschrieben ist
* und das zweite Zeichen klein geschrieben ist

```
a{process code helper}
	bool isNum(const std::string &s) {
		static Set reserved {
			"EOF", "NULL", "nullptr",
			"false", "null", "true",
			"undefined"
		};
		if (std::isdigit(s[0])) {
			return true;
		}
		return reserved.find(s) !=
			reserved.end();
	}
x{process code helper}
```
* Ein Bezeichner ist eine Zahl, wenn er mit einer Ziffer beginnt
* Oder wenn er einen reservierten Namen hat

```
a{process code helper}
	void process_ident(
		std::ostream &out,
		const std::string ident,
		char w
	) {
		if (w == '(') {
			span_str(out, "fn", ident);
		e{special ident classes}
		} else {
			span_str(out, "var", ident);
		}
	}
x{process code helper}
```
* Ein Bezeichner, dem eine öffnende Klammer folgt, wird immer als
  Funktion dargestellt
* Wenn keine andere Regel greift, wird der Bezeichner als Variable
  formatiert

```
d{special ident classes}
	} else if (isKeyword(ident)) {
		span_str(out, "keyword", ident);
	} else if (isType(ident)) {
		span_str(out, "type", ident);
	} else if (isNum(ident)) {
		span_str(out, "num", ident);
x{special ident classes}
```
* Schlüsselwörter, Typen und Zahlen werden durch die Hilfsfunktionen
  identifiziert

```
d{do macro}
	auto q = std::find(w + 1, end, '}');
	if (q == end) {
		writeEscaped(out, ident);
		writeOneEscaped(out, '{');
		begin = w;
	} else {
		std::string name {w + 1, q};
		e{write macro};
		begin = q;
	}
x{do macro}
```
* Wenn keine schließende Mengen-Klammer gefunden wird, dann ist der
  Bezeichner kein Makro
* Sonst wird das Makro ausgegeben

```
d{write macro}
	if (ident == "i") {
		e{write include};
	}
x{write macro}
```
* Der Bezeichner `s{i}` steht für ein `@include`-Makro

```
d{write include}
	auto ext = name.find_last_of('.');
	ASSERT_MSG(
		ext != std::string::npos,
		"no period"
	);
	writeMacroHeader(out, "include");
	out << "<a href=\"" <<
		name.substr(0, ext) <<
		".html\">";
	out << name <<
		"</a></span>)</span>";
x{write include}
```
* Die Datei-Extension wird beim generierten Link durch `s{.html}`
  ersetzt

```
a{process code helper}
	void writeMacroClass(
		std::ostream &out,
		const char *name
	) {
		out << "<span class=\"" <<
			name << "\">";
	}
x{process code helper}
```
* Öffnet ein `<span>`-Tag mit einer bestimmten Klasse

```
a{process code helper}
	void writeMacroHeader(
		std::ostream &out,
		const char *name
	) {
		writeMacroClass(out, "macro");
		out << '@' << name << "(<span class=\"name\">";
	}
x{process code helper}
```
* Öffnet ein Makro mit einem bestimmetn Namen

```
a{write macro}
	else if (ident == "d") {
		writeMacroHeader(out, "def");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{write macro}
```
* Der Bezeichner `s{d}` steht für ein `@def`-Makro

```
a{write macro}
	else if (ident == "D") {
		writeMacroHeader(out, "globdef");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{write macro}
```
* Der Bezeichner `s{D}` steht für ein `@globdef`-Makro

```
a{write macro}
	else if (ident == "a") {
		writeMacroHeader(out, "add");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{write macro}
```
* Der Bezeichner `s{a}` steht für ein `@add`-Makro

```
a{write macro}
	else if (ident == "A") {
		writeMacroHeader(out, "globadd");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{write macro}
```
* Der Bezeichner `s{A}` steht für ein `@globadd`-Makro

```
a{write macro}
	else if (ident == "r") {
		writeMacroHeader(out, "replace");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{write macro}
```
* Der Bezeichner `s{r}` steht für ein `@replace`-Makro

```
a{write macro}
	else if (ident == "R") {
		writeMacroHeader(
			out, "globreplace"
		);
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{write macro}
```
* Der Bezeichner `s{R}` steht für ein `@globreplace`-Makro

```
a{write macro}
	else if (ident == "x") {
		writeMacroHeader(out, "end");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{write macro}
```
* Der Bezeichner `s{x}` steht für ein `@end`-Makro

```
a{write macro}
	else if (ident == "e") {
		writeMacroHeader(out, "expand");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{write macro}
```
* Der Bezeichner `s{e}` steht für ein `@expand`-Makro

```
a{write macro}
	else if (ident == "E") {
		writeMacroHeader(out, "multiple");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{write macro}
```
* Der Bezeichner `s{E}` steht für ein `@multiple`-Makro

```
a{write macro}
	else if (ident == "g") {
		writeMacroHeader(
			out, "globexpand"
		);
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{write macro}
```
* Der Bezeichner `s{g}` steht für ein `@globexpand`-Makro

```
a{write macro}
	else if (ident == "G") {
		writeMacroHeader(out, "globmult");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{write macro}
```
* Der Bezeichner `s{G}` steht für ein `@globmult`-Makro

```
a{write macro}
	else if (ident == "t") {
		writeMacroClass(out, "type");
		writeEscaped(out, name);
		out << "</span>";
	}
x{write macro}
```
* Der Bezeichner `s{t}` steht für eine Typ-Formatierung

```
a{write macro}
	else if (ident == "v") {
		writeMacroClass(out, "var");
		writeEscaped(out, name);
		out << "</span>";
	}
x{write macro}
```
* Der Bezeichner `s{v}` steht für eine Variablen-Formatierung

```
a{write macro}
	else if (ident == "f") {
		writeMacroClass(out, "fn");
		writeEscaped(out, name);
		out << "</span>";
	}
x{write macro}
```
* Der Bezeichner `s{f}` steht für eine Funktion-Formatierung

```
a{write macro}
	else if (ident == "k") {
		writeMacroClass(out, "keyword");
		writeEscaped(out, name);
		out << "</span>";
	}
x{write macro}
```
* Der Bezeichner `s{k}` steht für eine Schlüsselwort-Formatierung

```
a{write macro}
	else if (ident == "s") {
		writeMacroClass(out, "str");
		writeEscaped(out, name);
		out << "</span>";
	}
x{write macro}
```
* Der Bezeichner `s{s}` steht für eine Zeichenketten-Formatierung

```
a{write macro}
	else if (ident == "n") {
		writeMacroClass(out, "num");
		writeEscaped(out, name);
		out << "</span>";
	}
x{write macro}
```
* Der Bezeichner `s{n}` steht für eine Werte-Formatierung

```
a{write macro}
	else if (ident == "p") {
		writeMacroClass(out, "var");
		out << "@priv(<span>";
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{write macro}
```
* Der Bezeichner `s{p}` steht für ein `@priv`-Makro

```
a{write macro}
	else if (ident == "m") {
		writeMacroClass(out, "var");
		out << "@magic(<span>";
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{write macro}
```
* Der Bezeichner `s{m}` steht für ein `@magic`-Makro

```
a{write macro}
	else if (ident == "b") {
		writeMacroClass(out, "virt");
		out << "</span><br/>";
	}
x{write macro}
```
* Der Bezeichner `s{b}` wird als Zeilenumbruch dargestellt

```
a{write macro}
	else {
		process_ident(out, ident, '{');
		writeOneEscaped(out, '{');
		q = w + 1;
	}
x{write macro}
```
* Andere Bezeichner werden nicht als Makro formatiert

# Notizen
* Zu den Folien können Notizen mit `*` markiert werden
* Notizen sind Teil einer Seite, aber nicht Teil einer Folie

```
a{html state enums}
	, inNotes
x{html state enums}
```
* Notizen sind ein eigener Zustand
* Da sie mehrzeilig sein können

```
a{process line} 
	if (
		line[0] == '*' ||
		status.state == HtmlState::inNotes
	) {
		e{process note};
	}
x{process line}
```
* Eine Notiz beginnt entweder mit `*` oder folgt einer solchen Zeile

```
A{global elements}
	void process_content(
		std::ostream &out,
		SI begin, SI end
	) {
		e{process content line};
	}
x{global elements}
```
* Diese Funktion formatiert beliebigen Markdown-Text als HTML
* Code-Schnippsel werden ebenfalls formatiert

```
d{close specials}
	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}
x{close specials}
```
* Bei einer Leerzeile werden Notizen beendet

```
d{process note}
	if (line[0] == '*') {
		e{process note line};
	} else {
		process_content(
			out, line.begin(), line.end()
		);
	}
x{process note}
```
* Notizen die mit `*` beginnen, müssen eine neue Notiz einleiten
* Folge-Zeilen werden direkt ausgegeben

```
d{process note line}
	auto end = line.end();
	auto begin = line.begin() + 1;
	while (
		begin != end && *begin == ' '
	) {
		++begin;
	}
x{process note line}
```
* Die Funktion überspringt den Stern und folgende Leerzeichen

```
a{process note line}
	if (
		status.state != HtmlState::inNotes
	) {
		e{switch into note mode};
	} else {
		out << "</li><li>\n";
	}
	process_content(out, begin, end);
x{process note line}
```
* Wenn noch keine Notizen geschrieben wurden, wird in den Notizen Modus
  gewechselt
* Andernfalls wird nur eine neue Notiz geöffnet

```
d{switch into note mode}
	if (
		status.state != HtmlState::inSlide
	) {
		out << "<div>\n";
	}
	status.state = HtmlState::inNotes;
	out << "<ul><li>\n";
x{switch into note mode}
```
* Notizen können nur auf einer Seite erscheinen
* Wenn noch keine Seite offen ist, dann öffnet die Funktion eine Seite
* Die Funktion schreibt passende HTML-Tags

```
d{process content line}
	for(; begin != end; ++begin) {
		e{special content line};
		writeOneEscaped(out, *begin);
	}
	out << '\n';
x{process content line}
```
* Die Zeile wird Zeichen für Zeichen abgearbeitet
* Wenn keine Sonderbehandlung erfolgt, wird das Zeichen ausgegeben

```
d{special content line}
	if (*begin == '`') {
		e{inline code};
	}
x{special content line}
```
* Ein Backtick signalisiert eingeschobene Code-Schnippsel

```
d{inline code}
	auto w = begin + 1;
	while (w != end && *w != '`') {
		++w;
	}
	if (w != end) {
		out << "<code>";
		process_code(out, begin + 1, w);
		out << "</code>";
		begin = w;
		continue;
	}
x{inline code}
```
* Eingeschobene Code-Schnippsel werden ebenfalls mit `f{process_code}`
  formatiert

```
a{special content line}
	if (
		*begin == '*' &&
		(begin + 1) != end &&
		*(begin + 1) == '*'
	) {
		e{bold block};
	}
x{special content line}
```
* Zwei Sterne markieren den Start eines fetten Textes

```
d{bold block}
	auto w = begin + 2;
	while (
		w != end && (w + 1) != end &&
		(*w != '*' || *(w + 1) != '*')
	) {
		++w;
	}
x{bold block}
```
* Funktion sucht zwei weitere Sterne

```
a{bold block}
	if (
		w != end && (w + 1 ) != end &&
		*w == '*' && *(w + 1) == '*'
	) {
		e{do bold};
		continue;
	}
x{bold block}
```
* Wenn Ende-Markierung gefunden wurde, wird der Text dazwischen fett
  formatiert

```
d{do bold}
	out << "<b>";
	writeEscaped(
		out, std::string {begin + 2, w}
	);
	out << "</b>";
	begin = w + 1;
x{do bold}
```
* Formatiert Text fett

