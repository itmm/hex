# HTML-Präsentation generieren
* Diese Datei beschreibt Code, um aus den `x`-Quelldateien
  HTML-Präsentationen zu erzeugen

```
@Def(write HTML file)
	for (auto &cur : inputs) {
		@put(write cur HTML file);
	}
@end(write HTML file)
```
* Alle bisher prozessierten Dateien werden erneut durchgegangen
* Nach der Abarbeitung einer Datei wird deren Speicher freigegeben

```
@def(write cur HTML file)
	const std::string &name { cur->name };
	std::string outPath {
		name.substr(0, name.size() - 2) +
		".html"
	};
	std::ofstream out { outPath.c_str() };
	@put(write cur HTML file to out);
	out.close();
@end(write cur HTML file)
```
* Die HTML hat den gleichen Pfad mit der Endung `.html` anstatt `.x`

```
@def(write cur HTML file to out)
	std::ifstream in {
		cur->name.c_str()
	};
	@put(write from in to out);
	in.close();
@end(write cur HTML file to out)
```
* Zuerst wird die Eingabe-Datei zum Lesen geöffnet

```
@Add(global elements)
	enum class HtmlState {
		nothing,
		inSlide,
		afterSlide,
		afterSlides
		@put(html state enums)
	};
@end(global elements)
```
* In einem Zustands-Automaten wird abgelegt, in welchem Modus sich die
  gerade generierte HTML-Datei befindet
* Vor dem ersten Element muss der HTML-Header geschrieben werden
* Ansonsten wird unterschieden, ob eine Folie bereits geöffnet wurde
* Oder ob gerade keine Folie offen ist

```
@Add(global elements)
	struct HtmlStatus {
		@put(html state elements)
	};
@end(global elements)
```
* Der aktuelle Status wird in `HtmlStatus` abgelegt

```
@def(html state elements)
	HtmlStatus();
	HtmlState state;
@end(html state elements)
```
* Status hat einen Konstruktor
* Der aktuelle Zustand wird im Status abgelegt

```
@Add(global elements)
	inline HtmlStatus::HtmlStatus():
		state {
			HtmlState::nothing
		}
	{ }
@end(global elements)
```
* Der initiale Zustand signalisiert, dass noch nichts geschrieben wurde

```
@Add(includes)
	#include <string>
@end(includes)
```
* Die Anwendung verwendet `std::string` überall

```
@def(write from in to out)
	HtmlStatus status;
	std::string ident;
	std::string line;
	while (std::getline(in, line)) {
		@put(process line);
	}
@end(write from in to out)
```
* Beim Schreiben einer Datei wird zuerst der Status initialisiert
* Die Eingabe wird Zeile für Zeile abgearbeitet
* Bis das Datei-Ende erreicht ist

```
@Add(global elements)
	bool in_code(
		HtmlStatus *s
	) {
		@put(check html special state);
		return false;
	}
@end(global elements)
```
* Die Funktion `@f(in_code)` liefert `true`, wenn sich
  der Zustands-Automat gerade in einem Fragment befindet

```
@def(process line)
	if (in_code(&status)) {
		@put(process code);
		continue;
	}
@end(process line)
```
* Während Code bearbeitet wird, gelten viele andere Regeln nicht mehr
* Daher wird dieser Block als erstes abgehandelt

```
@add(process line)
	if (line == "") {
		@put(close specials);
		switch (status.state) {
			case HtmlState::afterSlide:
			case HtmlState::afterSlides:
			case HtmlState::nothing:
				break;
			default:
				@put(close slide);
		}
		continue;
	}
@end(process line)
```
* Eine leere Seite schließt eine offene Folie ab

```
@def(close slide)
	out << "</div>\n";
	status.state = HtmlState::afterSlide;
@end(close slide)
```
* Die Funktion schreibt das schließende `</div>`
* Und setzt den Zustand passend

# Überschriften
* Überschriften beginnen mit Rauten `#`

```
@add(process line)
	if (line[0] == '#') {
		@put(process header);
		status.state = HtmlState::inSlide;
		continue;
	}
@end(process line)
```
* Wenn am Anfang einer Zeile eine Raute gelesen wird, dann beginnt eine
  Überschrift

```
@def(process header)
	int level = 1;
	while (
		level < (int) line.size() &&
			line[level] == '#'
	) {
		++level;
	}
@end(process header)
```
* Solange weitere Rauten folgen, wird der Level erhöht

```
@add(process header)
	auto e = line.end();
	auto b = line.begin() + level;
	while (b != e && *b <= ' ') {
		++b;
	}
@end(process header)
```
* Nach den Rauten dürfen beliebig viele Leerzeichen vorkommen

```
@add(process header)
	ASSERT(b != e);
	std::string name {b, e};
	@put(close previous HTML page);
	@mul(write header tag);
	out << "<div class=\"slides\">\n";
	out << "<div><div>\n";
	@mul(write header tag);
	out << "</div>\n";
@end(process header)
```
* Falls schon eine Seite offen ist, dann wird diese geschlossen
* Dann wird eine HTML-Überschrift ausgegeben
* Und eine Folie mit einer HTML-Überschrift wird erzeugt

```
@Add(global elements) 
	void writeOneEscaped(
		std::ostream &out, char ch
	) {
		switch (ch) {
			@put(escape special)
			default:
				out << ch;
		}
	}
@end(global elements)
```
* Schreibt ein Zeichen
* Zeichen mit besonderer Bedeutung in HTML ersetzt die Funktion durch
  Entitäten

```
@Add(global elements)
	void writeEscaped(
		std::ostream &out,
		const std::string &str
	) {
		for (char ch : str) {
			writeOneEscaped(out, ch);
		}
	}
@end(global elements)
```
* Schreibt mehrere Zeichen
* Zeichen mit besonderer Bedeutung in HTML ersetzt die Funktion durch
  Entitäten

```
@def(escape special)
	case '<':
		out << "&lt;";
		break;
	case '>':
		out << "&gt;";
		break;
	case '&':
		out << "&amp;";
		break;
@end(escape special)
```
* Die Zeichen `<`, `>` und `&` werden ersetzt

```
@Add(global elements)
	@put(process code helper);
	void process_code(
		std::ostream &out,
		SI begin, SI end
	) {
		@put(do code);
	}
@end(global elements)
```
* Verarbeitet ein Code-Fragment
* Dies Funktion wird im Code Modus verwendet
* Und wenn in Notizen Code-Fragmente eingebettet sind

```
@Add(global elements)
	void process_content(
		std::ostream &out,
		SI begin, SI end
	) {
		@put(process content line);
	}
@end(global elements)
```
* Diese Funktion formatiert beliebigen Markdown-Text als HTML
* Code-Schnippsel werden ebenfalls formatiert

```
@def(write header tag)
	out << "<h" << level << '>';
	process_content(out, name.begin(), name.end());
	out << "</h" << level << ">\n";
@end(write header tag)
```
* Die HTML-Überschrift enthält den eingelesenen Level

```
@def(close previous HTML page)
	switch (status.state) {
		case HtmlState::nothing: {
			@put(write HTML header);
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
@end(close previous HTML page)
```
* Je nach Zustand müssen unterschiedlich viele HTML-Tags geschlossen
  werden

```
@def(write HTML header)
	out << "<!doctype html>\n";
	out << "<html lang=\"de\">\n";
	out << "<head>\n";
	@put(write HTML header entries);
	out << "</head>\n";
	out << "<body>\n";
@end(write HTML header)
```
* Dies wird am Anfang der HTML-Datei ausgegeben
* Dafür muss jede `.x`-Datei mit einer Überschrift beginnen
* Diese Überschrift ist dann auch der Titel der HTML-Datei

```
@def(write HTML header entries)
	out << "<meta charset=\"utf-8\">\n";
	out << "<title>";
	writeEscaped(out, name);
	out << "</title>\n";
	out << "<link rel=\"stylesheet\" "
		"type=\"text/css\" href=\""
		<< stylesheet << "\">";
@end(write HTML header entries)
```
* Im Header wird das Zeichenformat auf UTF-8 gesetzt
* Und der Titel ausgegeben
* Und die Stylesheet-Datei eingebunden

# Code formatieren
* Code kann auf Seiten oder in den Notizen ausgegeben werden

```
@def(html state enums)
	, inCode
@end(html state enums)
```
* Es gibt einen eigenen Zustand, wenn Code ausgegeben wird

```
@def(check html special state)
	if (s->state == HtmlState::inCode) {
		return true;
	}
@end(check html special state)
```
* Die Code-Ausgabe ist ein besonderer Zustand


```
@add(process line)
	if (line == "```") {
		@put(open code page);
		continue;
	}
@end(process line)
```
* Eine Zeile mit drei Backticks wechselt in den Code Modus

```
@def(open code page)
	if (
		status.state == HtmlState::afterSlides
	) {
		out << "<div class=\"slides\">\n";
	}
	if (
		status.state == HtmlState::inSlide
	) {
		out << "</div>\n";
	}
	out << "<div><div>\n";
	out << "<code>\n";
	status.state = HtmlState::inCode;
@end(open code page)
```
* Beim Betreten wird eine Seite mit einem `<code>`-Tag geöffnet
* Und der Zustand passend gesetzt

```
@def(process code)
	if (line == "```") {
		@put(close code page);
		continue;
	}
@end(process code)
```
* Eine Zeile mit drei Backticks wechselt aus dem Code Modus zurück

```
@def(close code page)
	out << "</code></div>\n";
	status.state = HtmlState::inSlide;
@end(close code page)
```
* Beim Verlassen wird die Folie (aber nicht die Seite) geschlossen

```
@add(write from in to out)
	if (
		status.state == HtmlState::inCode
	) {
		std::cerr <<
			"unterminated code block\n";
	}
@end(write from in to out)
```
* Wenn wir beim Beenden des Parsens noch im Code-Modus sind, dann
  stimmt etwas nicht
* Eine Fehlermeldung wird ausgegeben

```
@add(write from in to out)
	if (
		status.state != HtmlState::nothing
	) {
		out << "</body>\n</html>\n";
	}
@end(write from in to out)
```
* Am Ende der Datei schließt die Funktion
  die HTML-Tags

```
@add(process code)
	process_code(
		out, line.begin(), line.end()
	);
	out << "<br/>\n";
@end(process code)
```
* Im Code Modus wird die Funktion aufgerufen
* Und danach ein Zeilenumbruch ausgegeben

```
@def(do code)
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
@end(do code)
```
* Die Funktion zählt Tabulatoren am Anfang der Zeile
* Und gibt ein Tag für die passende Einrückung aus

```
@add(do code)
	for (; begin != end; ++begin) {
		@put(process code ch);
		writeOneEscaped(out, *begin);
	}
@end(do code)
```
* Danach wird die Zeile Zeichen für Zeichen verarbeitet
* Wenn keine besondere Verarbeitung geschieht, gibt die Funktion das
  Zeichen direkt aus

```
@def(process code ch)
	if (
		*begin == '`' ||
		*begin == '\'' ||
		*begin == '"'
	) {
		@put(process string);
		continue;
	}
@end(process code ch)
```
* Wenn ein Zeichen eine Zeichenkette einleitet, wird diese geparst

```
@def(process string)
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
@end(process string)
```
* Die Zeichenkette endet mit dem gleichen Zeichen, mit dem sie angefangen
  hat
* Wenn dieses nicht durch einen Backslash escaped ist

```
@def(process code helper)
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
@end(process code helper)
```
* Diese Hilfsfunktion umgibt einen Bezeichner mit einem `<span>`-Tag
  einer bestimmten Klasse

```
@add(process string)
	std::string name {begin, w + 1};
	span_str(out, "str", name);
	begin = w;
@end(process string)
```
* Die Funktion gibt die Zeichenkette als `<span>` mit der Klasse `@s(str)`
  aus

```
@add(process code ch)
	if (*begin == '@') {
		auto nb = begin + 1;
		auto ne = nb;
		while (ne != end && *ne != '(') {
			if (! isalpha(*ne)) {
				ne = end;
				break;
			}
			++ne;
		}
		if (ne != end) {
			std::string name {nb, ne};
			auto ab = ne + 1;
			auto ae = ab;
			@put(macro loop);
		}
	}
@end(process code ch)
```

```
@def(macro loop)
	while (ae != end && *ae != ')') {
		if (*ae == '@') {
			++ae;
			if (ae == end) { break; }
		}
		++ae;
	}
	if (ae != end) {
		std::string arg {ab, ae};
		@put(got macro);
		continue;
	}
@end(macro loop)
```

```
@def(got macro)
	do {
		@put(special macro);
		@put(macro default);
	} while (false);
	begin = ae;
@end(got macro)
```

```
@def(macro default)
	std::cerr << "unknown macro @" <<
		name << '(' << arg << ")\n";
	writeOneEscaped(out, '@');
	writeEscaped(out, name);
	writeOneEscaped(out, '(');
	writeEscaped(out, arg);
	writeOneEscaped(out, ')');
@end(macro default)
```

```
@def(special macro)
	static Set macros = {
		"def", "end", "add", "put", "mul",
		"Def", "Add", "Mul", "rep", "Rep",
		"Put"
	};
	if (
		macros.find(name) != macros.end()
	) {
		writeMacroHeader(out, name);
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}
@end(special macro)
```

```
@add(special macro)
	if (name == "inc") {
		@put(write include);
		break;
	}
@end(special macro)
```

```
@def(write include)
	auto ext = arg.find_last_of('.');
	ASSERT_MSG(
		ext != std::string::npos,
		"no period"
	);
	writeMacroHeader(out, name);
	out << "<a href=\"" <<
		arg.substr(0, ext) <<
		".html\">";
	out << arg <<
		"</a></span>)</span>";
@end(write include)
```
* Die Datei-Extension wird beim generierten Link durch `@s(.html)`
  ersetzt

```
@add(special macro)
	if (name == "s" || name == "str") {
		writeMacroClass(out, "str");
		for (auto x = arg.begin();; ++x) {
			if (*x == '@') {
				++x;
			}
			if (x == arg.end()) { break; }
			writeOneEscaped(out, *x);
		}
		out << "</span>";
		break;
	}
@end(special macro)
```

```
@add(special macro)
	if (name == "f" || name == "fn") {
		writeMacroClass(out, "fn");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}
@end(special macro)
```
* Der Bezeichner `@s(f)` steht für eine Funktion-Formatierung

```
@add(special macro)
	if (name == "v" || name == "var") {
		writeMacroClass(out, "var");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}
@end(special macro)
```
* Der Bezeichner `@s(v)` steht für eine Variablen-Formatierung

```
@add(special macro)
	if (name == "k" || name == "key") {
		writeMacroClass(out, "keyword");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}
@end(special macro)
```
* Der Bezeichner `@s(k)` steht für eine Schlüsselwort-Formatierung

```
@add(special macro)
	if (name == "n" || name == "num") {
		writeMacroClass(out, "num");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}
@end(special macro)
```
* Der Bezeichner `@s(n)` steht für eine Wert-Formatierung

```
@add(special macro)
	if (name == "t" || name == "typ") {
		writeMacroClass(out, "type");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}
@end(special macro)
```
* Der Bezeichner `@s(t)` steht für eine Typ-Formatierung

```
@add(special macro)
	if (name == "b" || name == "br") {
		writeMacroClass(out, "virt");
		out << "</span><br/>";
		break;
	}
@end(special macro)
```
* Der Bezeichner `@s(b)` wird als Zeilenumbruch dargestellt

```
@add(special macro)
	if (name == "priv") {
		writeMacroClass(out, "var");
		out << "@priv(<span>";
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}
@end(special macro)
```
* `@priv`-Makro ist ein Bezeichner

```
@add(special macro)
	if (name == "magic") {
		writeMacroClass(out, "num");
		out << "@magic(<span>";
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}
@end(special macro)
```
* `@magic`-Makro ist ein Wert

```
@Add(includes)
	#include <cctype>
@end(includes)
```
* Die Funktion verwendet `std::isalnum`

```
@add(process code ch)
	auto w = begin;
	@put(find identifier end);
	if (w != begin) {
		@put(process identifier);
		continue;
	}
@end(process code ch)
```
* An der aktuellen Position wird ein Identifier gesucht
* Wenn dieser gefunden wird, dann wird er abgearbeitet

```
@def(find identifier end)
	while (w != end && (
		std::isalnum(*w) ||
			*w == '_' || *w == '$' ||
			*w == '#'
	)) {
		++w;
	}
@end(find identifier end)
```
* Zeichen und Ziffern können einen Identifier bilden
* Zusätzlich noch der Unterstrich und das Dollar-Zeichen

```
@def(process identifier)
	std::string ident {begin, w};
	begin = w - 1;
	process_ident(
		out, ident,
		w != end ? *w : ' '
	);
@end(process identifier)
```
* `begin` wird am Anfang gesetzt, da die Makro-Verarbeitung dieses ggf.
  korrigiert
* Sonst wird der Identifier normal verarbeitet
* Bei der normalen Verarbeitung ist wichtig, welches Zeichen dem
  Identifier folgt

```
@Add(includes)
	#include <set>
@end(includes)
```
* Besondere Bezeichner werden in `std::set` abgelegt

```
@add(process code helper)
	using Set = std::set<std::string>;

	bool isKeyword(const std::string &s) {
		static Set reserved {
			@put(keywords)
		};
		return
			reserved.find(s) !=
				reserved.end() ||
					(s.size() && s[0] == '#');
	}
@end(process code helper)
```
* Prüft ob ein Bezeichner ein reserviertes Schlüsselwort ist

```
@def(keywords)
	"break", "case", "catch", "continue",
	"default", "delete", "else", "for",
	"if", "in", "new", "return", "static",
	"switch", "try", "typeof", "while",
	"class", "public", "private",
	"template", "typename", "using"
@end(keywords)
```
* Reservierte Schlüsselwörter

```
@add(process code helper)
	bool isType(const std::string &s) {
		@put(is type);
		return false;
	}
@end(process code helper)
```
* Prüft, ob ein Bezeichner ein Typ ist

```
@def(is type)
	static Set reserved {
		@put(types)
	};
	if (reserved.find(s) !=
		reserved.end()
	) {
		return true;
	}
@end(is type)
```
* Wenn der Bezeichner ein reservierter Typ ist, liefert die Funktion
  `true`

```
@def(types)
	"FILE", "auto", "bool", "char",
	"const", "enum", "extern", "int",
	"let", "long", "signed", "struct",
	"union", "unsigned", "void", "double",
	"string", "std"
@end(types)
```
* Reservierte Typen

```
@add(is type)
	if (s.size() >= 2) {
		if (isupper(s[0]) &&
			islower(s[1])
		) {
			return true;
		}
	}
@end(is type)
```
* Ein Bezeichner ist zusätzlich ein Typ, wenn das sei erstes Zeichen
  groß geschrieben ist
* und das zweite Zeichen klein geschrieben ist

```
@add(process code helper)
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
@end(process code helper)
```
* Ein Bezeichner ist eine Zahl, wenn er mit einer Ziffer beginnt
* Oder wenn er einen reservierten Namen hat

```
@add(process code helper)
	void process_ident(
		std::ostream &out,
		const std::string ident,
		char w
	) {
		if (w == '(') {
			span_str(out, "fn", ident);
		@put(special ident classes)
		} else {
			span_str(out, "var", ident);
		}
	}
@end(process code helper)
```
* Ein Bezeichner, dem eine öffnende Klammer folgt, wird immer als
  Funktion dargestellt
* Wenn keine andere Regel greift, wird der Bezeichner als Variable
  formatiert

```
@def(special ident classes)
	} else if (isKeyword(ident)) {
		span_str(out, "keyword", ident);
	} else if (isType(ident)) {
		span_str(out, "type", ident);
	} else if (isNum(ident)) {
		span_str(out, "num", ident);
@end(special ident classes)
```
* Schlüsselwörter, Typen und Zahlen werden durch die Hilfsfunktionen
  identifiziert

```
@add(process code helper)
	void writeMacroClass(
		std::ostream &out,
		const char *name
	) {
		out << "<span class=\"" <<
			name << "\">";
	}
@end(process code helper)
```
* Öffnet ein `<span>`-Tag mit einer bestimmten Klasse

```
@add(process code helper)
	void writeMacroHeader(
		std::ostream &out,
		const std::string &name
	) {
		writeMacroClass(out, "macro");
		out << '@' << name << "(<span class=\"name\">";
	}
@end(process code helper)
```
* Öffnet ein Makro mit einem bestimmetn Namen

# Notizen
* Zu den Folien können Notizen mit `*` markiert werden
* Notizen sind Teil einer Seite, aber nicht Teil einer Folie

```
@add(html state enums)
	, inNotes
@end(html state enums)
```
* Notizen sind ein eigener Zustand
* Da sie mehrzeilig sein können

```
@add(process line)
	if (
		line[0] == '*' ||
		status.state == HtmlState::inNotes
	) {
		@put(process note);
		continue;
	}
@end(process line)
```
* Eine Notiz beginnt entweder mit `*` oder folgt einer solchen Zeile

```
@def(close specials)
	if (
		status.state == HtmlState::inNotes
	) {
		out << "</li></ul>\n";
	}
@end(close specials)
```
* Bei einer Leerzeile werden Notizen beendet

```
@def(process note)
	if (line[0] == '*') {
		@put(process note line);
	} else {
		process_content(
			out, line.begin(), line.end()
		);
	}
@end(process note)
```
* Notizen die mit `*` beginnen, müssen eine neue Notiz einleiten
* Folge-Zeilen werden direkt ausgegeben

```
@def(process note line)
	auto end = line.end();
	auto begin = line.begin() + 1;
	while (
		begin != end && *begin == ' '
	) {
		++begin;
	}
@end(process note line)
```
* Die Funktion überspringt den Stern und folgende Leerzeichen

```
@add(process note line)
	if (
		status.state != HtmlState::inNotes
	) {
		@put(switch into note mode);
	} else {
		out << "</li><li>\n";
	}
	process_content(out, begin, end);
@end(process note line)
```
* Wenn noch keine Notizen geschrieben wurden, wird in den Notizen Modus
  gewechselt
* Andernfalls wird nur eine neue Notiz geöffnet

```
@def(switch into note mode)
	if (
		status.state != HtmlState::inSlide
	) {
		out << "<div>\n";
	}
	status.state = HtmlState::inNotes;
	out << "<ul><li>\n";
@end(switch into note mode)
```
* Notizen können nur auf einer Seite erscheinen
* Wenn noch keine Seite offen ist, dann öffnet die Funktion eine Seite
* Die Funktion schreibt passende HTML-Tags

```
@def(process content line)
	for(; begin != end; ++begin) {
		@put(special content line);
		writeOneEscaped(out, *begin);
	}
	out << '\n';
@end(process content line)
```
* Die Zeile wird Zeichen für Zeichen abgearbeitet
* Wenn keine Sonderbehandlung erfolgt, wird das Zeichen ausgegeben

```
@def(special content line)
	if (*begin == '`') {
		@put(inline code);
	}
@end(special content line)
```
* Ein Backtick signalisiert eingeschobene Code-Schnippsel

```
@def(inline code)
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
@end(inline code)
```
* Eingeschobene Code-Schnippsel werden ebenfalls mit `@f(process_code)`
  formatiert

```
@add(special content line)
	if (
		*begin == '*' &&
		(begin + 1) != end &&
		*(begin + 1) == '*'
	) {
		@put(bold block);
	}
@end(special content line)
```
* Zwei Sterne markieren den Start eines fetten Textes

```
@def(bold block)
	auto w = begin + 2;
	while (
		w != end && (w + 1) != end &&
		(*w != '*' || *(w + 1) != '*')
	) {
		++w;
	}
@end(bold block)
```
* Funktion sucht zwei weitere Sterne

```
@add(bold block)
	if (
		w != end && (w + 1 ) != end &&
		*w == '*' && *(w + 1) == '*'
	) {
		@put(do bold);
		continue;
	}
@end(bold block)
```
* Wenn Ende-Markierung gefunden wurde, wird der Text dazwischen fett
  formatiert

```
@def(do bold)
	out << "<b>";
	writeEscaped(
		out, std::string {begin + 2, w}
	);
	out << "</b>";
	begin = w + 1;
@end(do bold)
```
* Formatiert Text fett

# Absätze
* Zwischen Folien verarbeitet `hex` Absätze mit normalen Text

```
@add(html state enums)
	, inPara
@end(html state enums)
```
* Absätze sind ein eigener Zustand
* Da sie mehrzeilig sein können

```
@add(process line)
	@put(process para);
@end(process line)
```
* Wenn nichts anderes bearbeitet wurde, dann verarbeitet der Renderer
  einen Absatz

```
@add(close specials)
	if (
		status.state == HtmlState::inPara
	) {
		out << "</p>\n";
		status.state =
			HtmlState::afterSlides;
	}
@end(close specials)
```
* Bei einer Leerzeile werden Absätze beendet beendet

```
@def(process para)
	if (status.state == HtmlState::afterSlide) {
		out << "</div>\n";
	}
	if (
		status.state != HtmlState::inPara
	) {
		out << "<p>";
		status.state = HtmlState::inPara;
	}
	process_content(
		out, line.begin(), line.end()
	);
@end(process para)
```
* Am Anfang wird ein Absatz geöffnet
* Danach gibt `hex` die Zeilen aus

