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
	HtmlState state;
x{html state elements}
```
* Der aktuelle Zustand wird im Status abgelegt

```
A{global elements}
	inline HtmlStatus::HtmlStatus():
		state {
			HtmlState::nothingWritten
		}
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
d{write from in to out}
	HtmlStatus status;
	std::string ident;
	std::string line;
	while (std::getline(in, line)) {
		e{process ch for HTML};
	}
x{write from in to out}
```
* Beim Schreiben einer Datei wird zuerst der Status initialisiert
* Wir befinden uns am Anfang einer Zeile
* Jedes Zeichen wird prozessiert
* Bis das Datei-Ende erreicht ist

# Überschriften


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
* Die Funktion `{isOutOfHtmlSpecial}` liefert `true`, wenn sich
  der Zustands-Automat gerade nicht in einem Sonder-Modus (Überschrift,
  Code, Notizen) befindet

```
d{process ch for HTML} 
	if (in_code(&status)) {
		e{process code};
		continue;
	}
x{process ch for HTML}
```

```
a{process ch for HTML}
	if (line == "") {
		e{close specials};
		if (status.state != HtmlState::afterSlide && status.state != HtmlState::nothingWritten) {
			out << "</div>\n";
			status.state = HtmlState::afterSlide;
		}
		continue;
	}
x{process ch for HTML}
```

```
a{process ch for HTML}
	if (line[0] == '#') {
		e{process header};
		status.state = HtmlState::inSlide;
		continue;
	}
x{process ch for HTML}
```
* Wenn am Anfang einer Zeile eine Raute gelesen wird, dann beginnt eine
  Überschrift
* Solange weitere Rauten folgen, wird der Level erhöht
* Bei der ersten Raute muss der alte Zustand gesichert werden

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

```
a{process header}
	auto e = line.end();
	auto b = line.begin() + level;
	while (b != e && *b <= ' ') {
		++b;
	}
x{process header}
```

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
* Zeichen, die einen HTML-Befehl einleiten können, werden durch passende
  Entitäten ersetzt

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
* Die Zeichen `<`, `>` und `&` müssen ersetzt werden

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
		case HtmlState::nothingWritten: {
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
a{process ch for HTML} 
	if (line == "```") {
		e{open code page};
		continue;
	}
x{process ch for HTML}
```
* Wenn am Anfang der Zeile Backticks kommen, dann werden sie gezählt

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

```
d{close code page}
	out << "</code></div>\n";
	status.state = HtmlState::inSlide;
x{close code page}
```
* Beim Verlassen wird die Folie (aber nicht die Seite) geschlossen

```
A{global elements}
	e{process code helper};
	void process_code(std::ostream &out, SI begin, SI end) {
		e{do code};
	}
x{global elements}
```

```
a{process code}
	process_code(out, line.begin(), line.end());
	out << "<br/>\n";
x{process code}
```

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

```
a{do code}
	for (; begin != end; ++begin) {
		e{process code ch};
	}
x{do code}
```

```
d{process code ch}
	if (*begin == '`' || *begin == '\'' || *begin == '"') {
		e{process string};
		continue;
	}
x{process code ch}
```

```
d{process string}
	auto w = begin + 1;
	while (w != end && *w != *begin) {
		if (*w == '\x5c') {
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

```
a{process string}
	std::string name {begin, w + 1};
	out << "<span class=\"str\">";
	writeEscaped(out, name);
	out << "</span>";
	begin = w;
x{process string}
```

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
A{includes}
	#include <cctype>
x{includes}
```

```
a{process code ch}
	auto w = begin;
	while (w != end && (std::isalnum(*w) || *w == '_' || *w == '$')) {
		++w;
	}

	if (w != begin) {
		std::string ident {begin, w};
		begin = w - 1;
		if (w != end && *w == '{') {
			e{do macro};
		} else {
			process_ident(out, ident, w != end ? *w : ' ');
		}
	}
x{process code ch}
```

```
a{process code ch}
	if (w == begin && w != end) {
		writeOneEscaped(out, *begin);
	}
x{process code ch}
```

```
d{process code helper}
	void escapeIdent(
		std::ostream &out,
		const char *cls,
		const std::string &s
	) {
		out << "<span class=\"" <<
			cls << "\">" << s <<
			"</span>";
	}
x{process code helper}
```

```
a{process code helper}
	bool isKeyword(const std::string &s) {
		static std::set<std::string>
			reserved {
				e{keywords}
			};
		return
			reserved.find(s) !=
				reserved.end();
	}
x{process code helper}
```

```
d{keywords}
	"break", "case", "catch", "continue",
	"default", "delete", "else", "for",
	"if", "in", "new", "return", "static",
	"switch", "try", "typeof", "while"
x{keywords}
```

```
a{process code helper}
	bool isType(const std::string &s) {
		e{is type};
		return false;
	}
x{process code helper}
```

```
d{is type}
	static std::set<std::string>
		reserved {
			e{types}
		};
	if (reserved.find(s) !=
		reserved.end()
	) {
		return true;
	}
x{is type}
```

```
d{types}
	"FILE", "auto", "bool", "char",
	"const", "enum", "extern", "int",
	"let", "long", "signed", "struct",
	"union", "unsigned", "void"
x{types}
```

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

```
a{process code helper}
	bool isNum(const std::string &s) {
		static std::set<std::string> reserved {
			"EOF", "NULL", "nullptr",
			"false", "null", "true", "undefined"
		};
		if (std::isdigit(s[0])) { return true; }
		return reserved.find(s) != reserved.end();
	}
x{process code helper}
```

```
a{process code helper}
	void process_ident(std::ostream &out, const std::string ident, char w) {
		if (w == '(') {
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
	}
x{process code helper}
```

```
d{do macro}
	auto q = w + 1;
	while (q != end && *q != '}') {
		++q;
	}
	if (q == end) {
		writeEscaped(out, ident);
		writeOneEscaped(out, '{');
		begin = w;
	} else {
		std::string name {w + 1, q};
		e{escape html frag};
		begin = q;
	}
x{do macro}
```
* Bei einer öffnenden Mengenklammer wird geprüft, ob ein bekannter
  Befehls-Code im vorherigen Zeichen liegt
* In diesem Fall wird die weiter Abarbeitung abgebrochen
* Statt dessen wurden passende Tags generiert und die nächsten Zeichen
  sind das Argument des Befehls
* Diese werden direkt kopiert

```
d{escape html frag}
	if (ident == "i") {
		auto ext = name.find_last_of('.');
		ASSERT_MSG(ext != std::string::npos,
			"no period"
		);
		writeMacroHeader(out, "include");
		out << "<a href=\"" 
			<< name.substr(0, ext) << ".html\">";
		out << name << "</a></span>)</span>";
		begin = q;
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "d") {
		writeMacroHeader(out, "def");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{escape html frag}
```

```
a{process code helper}
	void writeMacroClass(
		std::ostream &out,
		const char *name
	) {
		out << "<span class=\"" << name << "\">";
	}
x{process code helper}
```

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

```
a{escape html frag}
	else if (ident == "D") {
		writeMacroHeader(out, "globdef");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "a") {
		writeMacroHeader(out, "add");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "A") {
		writeMacroHeader(out, "globadd");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "r") {
		writeMacroHeader(out, "replace");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "R") {
		writeMacroHeader(out, "globreplace");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "x") {
		writeMacroHeader(out, "end");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "e") {
		writeMacroHeader(out, "expand");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "E") {
		writeMacroHeader(out, "multiple");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "g") {
		writeMacroHeader(out, "globexpand");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "G") {
		writeMacroHeader(out, "globmult");
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "t") {
		writeMacroClass(out, "type");
		writeEscaped(out, name);
		out << "</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "v") {
		writeMacroClass(out, "var");
		writeEscaped(out, name);
		out << "</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "f") {
		writeMacroClass(out, "fn");
		writeEscaped(out, name);
		out << "</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "k") {
		writeMacroClass(out, "keyword");
		writeEscaped(out, name);
		out << "</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "s") {
		writeMacroClass(out, "str");
		writeEscaped(out, name);
		out << "</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "n") {
		writeMacroClass(out, "num");
		writeEscaped(out, name);
		out << "</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "p") {
		writeMacroClass(out, "var");
		out << "@priv(<span>";
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{escape html frag}
```

```
a{escape html frag}
	else if (ident == "m") {
		writeMacroClass(out, "var");
		out << "@magic(<span>";
		writeEscaped(out, name);
		out << "</span>)</span>";
	}
x{escape html frag}
```
* `@magic`-Befehle werden als Zahlen formatiert

```
a{escape html frag}
	else if (ident == "b") {
		writeMacroClass(out, "virt");
		out << "</span><br/>";
	}
x{escape html frag}
```
* Zeilenumbrüche

```
a{escape html frag}
	else {
		process_ident(out, ident, '{');
		writeOneEscaped(out, '{');
		q = w + 1;
	}
x{escape html frag}
```

# Notizen

```
a{html state enums}
	, inNotes
x{html state enums}
```

```
a{process ch for HTML} 
	if (
		line[0] == '*' ||
		status.state == HtmlState::inNotes
	) {
		e{process note};
	}
x{process ch for HTML}
```

```
A{global elements}
	void process_content(std::ostream &out, SI begin, SI end) {
		e{process note line};
	}
x{global elements}
```

```
d{close specials}
	if (status.state == HtmlState::inNotes) {
		out << "</li></ul>\n";
	}
x{close specials}
```

```
d{process note}
	if (line[0] == '*') {
		auto end = line.end();
		auto begin = line.begin() + 1;
		while (begin != end && *begin == ' ') {
			++begin;
		}
		if (status.state != HtmlState::inNotes) {
			if (status.state != HtmlState::inSlide) {
				out << "<div>\n";
			}
			status.state = HtmlState::inNotes;
			out << "<ul><li>\n";
		} else {
			out << "</li><li>\n";
		}
		process_content(out, begin, end);
	} else {
		process_content(out, line.begin(), line.end());
	}
x{process note}
```

```
d{process note line}
	for(; begin != end; ++begin) {
		e{special note line};
		writeOneEscaped(out, *begin);
	}
	out << '\n';
x{process note line}
```

```
d{special note line}
	if (*begin == '`') {
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
	}
x{special note line}
```

```
a{special note line}
	if (*begin == '*' && (begin + 1) != end && *(begin + 1) == '*') {
		auto w = begin + 2;
		while (w != end && (w + 1) != end && (*w != '*' || *(w + 1) != '*')) {
			++w;
		}
		if (w != end && (w + 1 ) != end && *w == '*' && *(w + 1) == '*') {
			out << "<b>";
			writeEscaped(out, std::string {begin + 2, w});
			out << "</b>";
			begin = w + 1;
			continue;
		}
	}
x{special note line}
```

