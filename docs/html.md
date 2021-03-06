# Generate HTML slides
* code to generate HTML slides from the input files

```c++
@Add(global elements)
	@Put(needed by write_html)
	void write_html() {
		for (auto &cur : inputs) {
			@put(write cur HTML file)
		}
	}
@End(global elements)
```
* generate HTML slides

```c++
@Def(write HTML file)
	if (html_files) {
		write_html();
	}
@end(write HTML file)
```
* generate HTML slides, if they should be generated

```c++
@def(write cur HTML file)
	const std::string &name { cur.first };
	auto ext { name.rfind('.') };
	if (ext == std::string::npos) {
		ext = name.size();
	}
	std::string outPath { name.substr(0, ext) + ".html" };
	std::ofstream out { outPath.c_str() };
	@put(write cur HTML file to out)
	out.close();
@end(write cur HTML file)
```
* the name of the HTML slides results from replacing the extension of the
  input file by `.html`

```c++
@def(write cur HTML file to out)
	@put(write from in to out)
@end(write cur HTML file to out)
```
* write HTML to the just opened `std::ofstream` `out`

```c++
@Def(needed by write_html)
	enum class HtmlState {
		nothing,
		inSlide,
		afterSlide,
		afterSlides
		@put(html state enums)
	};
@end(needed by write_html)
```
* the `HtmlState` keeps track which part of a block is currently
  written
* each input file must start with a header so that the HTML header can
  be written

```c++
@Add(needed by write_html)
	struct HtmlStatus {
		@put(html state elements)
	};
@end(needed by write_html)
```
* the current status needs more information besides the state

```c++
@def(html state elements)
	HtmlState state = HtmlState::nothing;
@end(html state elements)
```
* the state is part of the status

```c++
@Add(includes)
	#include <string>
@end(includes)
```
* needs `std::string`

```c++
@def(write from in to out)
	HtmlStatus status;
	int slide_nr { 0 };
	for (const auto &b : cur.second.blocks) {
		@put(process block)
	}
@end(write from in to out)
```
* process each block

```c++
@def(process block)
	if (b.state == RS::header) {
		@put(process header)
	}
@end(process block)
```
* write header

```c++
@add(process block)
	if (b.state == RS::code) {
		@put(open code page)
		for (const auto &code : b.value) {
			@put(process code)
		}
		@put(close code page)
		if (! b.notes.empty()) {
			for (const auto &note : b.notes) {
				@mul(process note)
			}
		} else {
			out << "<ul></ul>\n";
		}
		@mul(close specials)
		@mul(close slide)
	}
@end(process block)
```
* write code and notes

```c++
@add(process block)
	if (b.state == RS::para) {
		for (const auto &para : b.value) {
			@put(process para)
			@mul(close specials)
		}
		@mul(close specials)
	}
@end(process block)
```
* write paragraphs

```c++
@def(close slide)
	out << "</div>\n";
	status.state = HtmlState::afterSlide;
@end(close slide)
```
* writes a `</div>` that closes a slide
* and adjusts the state

## Headings
* writes heading entry

```c++
@def(process header)
	@put(close previous HTML page)
	@mul(write header tag)
	out << "<div class=\"slides\">\n";
	out << "<div class=\"page\">\n";
	out << "<div class=\"slide slide-header\">";
	@mul(slide nr)
	out << "<div class=\"headers\">\n";
	@mul(write header tag)
	if (b.value.size() > 1) {
		out << "<ul>\n";
		for (unsigned i = 1; i < b.value.size(); ++i) {
			bool hidden { b.value[i][0] == '-' };
			out << "<li";
			if (hidden) {
				out << " class=\"hidden\"";
			}
			out << "><h" << (b.level + 1) << '>';
			const auto &n = b.value[i];
			auto bg { n.begin() };
			if (hidden) { ++bg; }
			process_content(out, bg, n.end());
			out << "</h" << (b.level + 1) << "></li>\n";
		}
		out << "</ul>\n";
	}
	out << "</div></div>\n";
	status.state = HtmlState::inSlide;
@end(process header)
```
* headers start a new slide group
* before the slide group a HTML heading is written
* the slide group starts with a slide that contains the heading

```c++
@add(process header)
	for (const auto &note : b.notes) {
		@mul(process note)
	}
	@mul(close specials)
	@mul(close slide)
@end(process header)
```
* add notes for the header
* and close the slide

```c++
@Add(needed by write_html) 
	void writeOneEscaped(std::ostream &out, char ch) {
		switch (ch) {
			@put(escape special)
			default:
				out << ch;
		}
	}
@end(needed by write_html)
```
* writes a character in the HTML output
* special characters are escaped

```c++
@Add(needed by write_html)
	void writeEscaped(std::ostream &out, const std::string &str) {
		for (char ch : str) {
			writeOneEscaped(out, ch);
		}
	}
@End(needed by write_html)
```
* writes multiple characters in the HTML outputs
* by calling the single character version

```c++
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
* the characters `<`, `>`, and `&` are replaced by their entities

```c++
@Add(needed by write_html)
	@put(process code helper)
	void process_code(std::ostream &out, SI begin, SI end) {
		@put(do code)
	}
@end(needed by write_html)
```
* writes a line of source code
* this is used in code blocks
* and in normal text that contains embedded code (e.g. in the notes or
  headings)

```c++
@Add(needed by write_html)
	void process_content(std::ostream &out, SI begin, SI end) {
		@put(process content line)
	}
@end(needed by write_html)
```
* writes normal Markdown content to HTML
* embedded code is pretty printed

```c++
@def(write header tag) {
	out << "<h" << b.level << '>';
	const auto &n = b.value[0];
	process_content(out, n.begin(), n.end());
	out << "</h" << b.level << ">\n";
} @end(write header tag)
```
* writes a HTML header tag of the right level

```c++
@def(close previous HTML page)
	switch (status.state) {
		case HtmlState::nothing:
			@put(write HTML header)
			break;
		case HtmlState::inSlide:
			out << "</div>\n";
			// fallthrough
		default:
			out << "</div>\n";
	}
@end(close previous HTML page)
```
* closes the current slide
* and the current slide group
* at the beginning of the document the HTML header is written

```c++
@def(write HTML header)
	out << "<!doctype html>\n";
	out << "<html lang=\"en\">\n";
	out << "<head>\n";
	@put(write HTML header entries)
	out << "</head>\n";
	out << "<body>\n";
@end(write HTML header)
```
* write HTML header and opens the `<body>` tag

```c++
@def(write HTML header entries)
	out << "<meta charset=\"utf-8\">\n";
	out << "<title>";
	writeEscaped(out, b.value[0]);
	out << "</title>\n";
	out << "<link rel=\"stylesheet\" type=\"text/css\" href=\"" <<
		stylesheet << "\">";
@end(write HTML header entries)
```
* the document is encoded in `UTF-8`
* the first header of the input file is also the `<title>` of the
  HTML document
* also include a stylesheet

## Format Code
* Code is pretty printed

```c++
@def(html state enums)
	, inCode
@end(html state enums)
```
* special state to format code

```c++
@def(open code page)
	if (status.state == HtmlState::afterSlides) {
		out << "<div class=\"slides\">\n";
	}
@end(open code page)
```
* close a previous slide, if it is open

```c++
@add(open code page)
	if (status.state == HtmlState::inSlide) {
		out << "</div>\n";
	}
	out << "<div class=\"page\"><div class=\"slide\">";
	@mul(slide nr)
	out << "\n<code>\n";
	status.state = HtmlState::inCode;
@end(open code page)
```
* a code slide contains one big `<code>` tag
* and sets the current state

```c++
@def(close code page)
	out << "</code></div>\n";
	status.state = HtmlState::afterSlide;
@end(close code page)
```
* closes `<code>` tag when the slide is closed

```c++
@add(write from in to out)
	if (status.state == HtmlState::inCode) {
		std::cerr << "unterminated code block\n";
	}
@end(write from in to out)
```
* it is an error, if the code block was not closed at the end of a file

```c++
@add(write from in to out)
	if (status.state != HtmlState::nothing) {
		out << "</body>\n</html>\n";
	}
@end(write from in to out)
```
* closes the `<body>` tag at the end of the input file
* if some output was written

```c++
@def(process code)
	process_code(out, code.begin(), code.end());
	out << "<br/>\n";
@end(process code)
```
* processes code in a code slide
* write the code and adds a new line

```c++
@def(do code)
	int indent = 0;
	while (begin != end && *begin == '\t') {
		++indent; ++begin;
	}
	if (indent) {
		out << "<span class=\"in" << indent << "\"></span>";
	}
@end(do code)
```
* counts the tab stops at the beginning of the line
* an `<span>` for the indentation is written

```c++
@add(do code)
	for (; begin != end; ++begin) {
		@put(process code ch)
		writeOneEscaped(out, *begin);
	}
@end(do code)
```
* process each character
* if their is no special treatment, copy it to HTML

```c++
@def(process code ch)
	if (*begin == '`' || *begin == '\'' || *begin == '"') {
		@put(process string)
		continue;
	}
@end(process code ch)
```
* treat the next characters as a string

```c++
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
* search for the end of the string
* watch out for escape characters on the way
* if no end is found, then treat the first character as an ordinary
  character

```c++
@def(process code helper)
	void span_str(std::ostream &out, const char *cls, const std::string &s) {
		out << "<span class=\"" << cls << "\">";
		writeEscaped(out, s);
		out << "</span>";
	}
@end(process code helper)
```
* writes a `<span>` with the given class around a string

```c++
@add(process string)
	std::string name { begin, w + 1 };
	span_str(out, "str", name);
	begin = w;
@end(process string)
```
* write formatted string

```c++
@add(process code ch)
	if (*begin == '@') {
		auto nb = begin + 1;
		auto ne = nb;
		@put(process cmd)
	}
@end(process code ch)
```
* process a command

```c++
@def(process cmd)
	while (ne != end && *ne != '(') {
		if (! isalpha(*ne)) {
			ne = end;
			break;
		}
		++ne;
	}
@end(process cmd)
```
* check that the command name only consists of letters
* and ends with an open parenthesis

```c++
@add(process cmd)
	if (ne != end) {
		std::string name {nb, ne};
		auto ab = ne + 1;
		auto ae = ab;
		@put(macro loop)
	}
@end(process cmd)
```
* if a command is found, parse its argument

```c++
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
		@put(got macro)
		continue;
	}
@end(macro loop)
```
* if a command argument is parsed, the command is parsed

```c++
@def(got macro)
	do {
		@put(special macro)
		@put(macro default)
	} while (false);
	begin = ae;
@end(got macro)
```
* special commands can change the formatting and are processed first
* if the command is not special, it will be copied to HTML with some
  warning

```c++
@def(macro default)
	writeOneEscaped(out, '@');
	writeEscaped(out, name);
	writeOneEscaped(out, '(');
	writeEscaped(out, arg);
	writeOneEscaped(out, ')');
@end(macro default)
```
* write error message
* and copy command to HTML

```c++
@def(special macro)
	static Set macros = {
		"def", "end", "add", "put", "mul", "Def",
		"Add", "Mul", "rep", "Rep", "Put", "End"
	};
@end(special macro)
```
* known commands

```c++
@add(special macro)
	if (macros.find(name) != macros.end()) {
		writeMacroHeader(out, name);
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}
@end(special macro)
```
* fragment commands are copied with pretty printing to HTML

```c++
@add(special macro)
	if (name == "inc") {
		@put(write include)
		break;
	}
@end(special macro)
```
* include commands are treated special

```c++
@def(write include)
	auto ext = arg.find_last_of('.');
	if (ext == std::string::npos) {
		ext = arg.size();
	}
	writeMacroHeader(out, name);
	out << "<a href=\"" << arg.substr(0, ext) << ".html\">";
	out << arg << "</a></span>)</span>";
@end(write include)
```
* includes also contain a link to the referenced file

```c++
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
* format the argument as string

```c++
@add(special macro)
	if (name == "f" || name == "fn") {
		writeMacroClass(out, "fn");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}
@end(special macro)
```
* format the argument as a function

```c++
@add(special macro)
	if (name == "v" || name == "var") {
		writeMacroClass(out, "var");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}
@end(special macro)
```
* format the argument as a variable

```c++
@add(special macro)
	if (name == "k" || name == "key") {
		writeMacroClass(out, "keyword");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}
@end(special macro)
```
* format the argument as a keyword

```c++
@add(special macro)
	if (name == "n" || name == "num") {
		writeMacroClass(out, "num");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}
@end(special macro)
```
* format the argument as a numeric value

```c++
@add(special macro)
	if (name == "t" || name == "typ") {
		writeMacroClass(out, "type");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}
@end(special macro)
```
* format the argument as a type

```c++
@add(special macro)
	if (name == "b" || name == "br") {
		writeMacroClass(out, "virt");
		out << "</span>";
		break;
	}
@end(special macro)
```
* create a line break in the HTML output
* the generated source code does not contain this line break

```c++
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
* `@priv` commands are formatted specially in the HTML output

```c++
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
* `@magic` commands are formatted specially in the HTML output

```c++
@Add(includes)
	#include <cctype>
@end(includes)
```
* needs `std::isalnum`

```c++
@add(process code ch)
	auto w = begin;
	@put(find identifier end)
	if (w != begin) {
		@put(process identifier)
		continue;
	}
@end(process code ch)
```
* checks if at the current position is an identifier

```c++
@def(find identifier end)
	while (w != end && (
		std::isalnum(*w) || *w == '_' || *w == '$' || *w == '#'
	)) {
		++w;
	}
@end(find identifier end)
```
* alphanumeric characters can form an identifier
* also `_`, `$` and `#` are valid parts of an identifier

```c++
@def(process identifier)
	std::string ident {begin, w};
	begin = w - 1;
	process_ident(out, ident, w != end ? *w : ' ');
@end(process identifier)
```
* the new input is one before the end of the identifier
* because the loop will increment it

```c++
@Add(includes)
	#include <set>
@end(includes)
```
* needs `std::set`

```c++
@add(process code helper)
	using Set = std::set<std::string>;
@end(process code helper)
```
* shorthand for the used `std::set`

```c++
@add(process code helper)
	bool isKeyword(const std::string &s) {
		static Set reserved { @put(keywords) };
		return reserved.find(s) != reserved.end() ||
			(s.size() && s[0] == '#');
	}
@end(process code helper)
```
* checks if identifier is a known keyword

```c++
@def(keywords)
	"break", "case", "catch", "continue", "default", "delete",
	"do", "else", "for", "if", "in", "inline", "new", "return",
	"static", "switch", "try", "typeof", "while", "class", "public",
	"private", "template", "typename", "using", "function", "throw",
	"namespace", "once", "constexpr", "volatile", "override"
@end(keywords)
```
* known keywords

```c++
@add(process code helper)
	bool isType(const std::string &s) {
		@put(is type)
		return false;
	}
@end(process code helper)
```
* checks if identifier is a type

```c++
@def(is type)
	static Set reserved { @put(types) };
	if (reserved.find(s) != reserved.end()) {
		return true;
	}
@end(is type)
```
* an identifier is a type, if it is one of the known types

```c++
@def(types)
	"FILE", "auto", "bool", "char", "const", "enum",
	"extern", "int", "let", "long", "signed", "struct",
	"union", "unsigned", "void", "double", "string",
	"std", "ifstream", "istream", "ofstream", "ostream",
	"vector", "map", "list", "float"
@end(types)
```
* known types

```c++
@add(is type)
	if (s.size() >= 2) {
		if (isupper(s[0]) && islower(s[1])) {
			return true;
		}
	}
@end(is type)
```
* an identifier is a type if it contains both an upper case and lower
  case letter

```c++
@add(process code helper)
	bool isNum(const std::string &s) {
		static Set reserved {
			"EOF", "NULL", "nullptr", "false", "null", "true",
			"undefined"
		};
		if (std::isdigit(s[0])) { return true; }
		return reserved.find(s) != reserved.end();
	}
@end(process code helper)
```
* an identifier is a numeric value if it starts with a digit
* or is one of the known numeric values

```c++
@add(process code helper)
	void process_ident(std::ostream &out, const std::string ident, char w) {
		@put(process ident)
	}
@end(process code helper)
```
* process identifiers

```c++
@def(process ident)
	if (isKeyword(ident)) {
		span_str(out, "keyword", ident);
	} else if (w == '(') {
		span_str(out, "fn", ident);
	@put(special ident classes)
	} else {
		span_str(out, "var", ident);
	}
@end(process ident)
```
* keywords are directly recognized
* an identifier is a function, if it is followed by `(`
* the default type for an identifier is a variable

```c++
@def(special ident classes)
	} else if (isType(ident)) {
		span_str(out, "type", ident);
	} else if (isNum(ident)) {
		span_str(out, "num", ident);
@end(special ident classes)
```
* types and numeric values are formatted, if the identifier is neither
  keyword nor function

```c++
@add(process code helper)
	void writeMacroClass(std::ostream &out, const char *name) {
		out << "<span class=\"" << name << "\">";
	}
@end(process code helper)
```
* opens `<span>` tag with a given class for a command

```c++
@add(process code helper)
	void writeMacroHeader(std::ostream &out, const std::string &name) {
		writeMacroClass(out, "macro");
		out << '@' << name << "(<span class=\"name\">";
	}
@end(process code helper)
```
* writes name of command

## Notes
* notes are part of a page, but not part of a slide

```c++
@add(html state enums)
	, inNotes
@end(html state enums)
```
* special state for states

```c++
@def(close specials)
	if (status.state == HtmlState::inNotes) {
		out << "</li></ul>\n";
	}
@end(close specials)
```
* if output was in notes state, the list is closed

```c++
@def(process note)
	auto end = note.end();
	auto begin = note.begin();
@end(process note)
```
* quick access for limits

```c++
@add(process note)
	if (status.state != HtmlState::inNotes) {
		@put(switch into note mode)
	} else {
		out << "</li><li>\n";
	}
	process_content(out, begin, end);
	out << '\n';
@end(process note)
```
* switch into note state, if it is not the current state
* otherwise open a new list entry
* and write note

```c++
@def(switch into note mode)
	if (status.state != HtmlState::inSlide &&
		status.state != HtmlState::afterSlide
	) {
		out << "<div class=\"page\">\n";
	}
	status.state = HtmlState::inNotes;
	out << "<ul><li>\n";
@end(switch into note mode)
```
* create a new page, if no page is open
* open list

```c++
@def(process content line)
	for(; begin != end; ++begin) {
		@put(special content line)
		writeOneEscaped(out, *begin);
	}
@end(process content line)
```
* go through the line character by character
* if no special treatment happens, the character is copied to the
  HTML output

```c++
@def(special content line)
	if (*begin == '`') {
		@put(inline code)
	}
@end(special content line)
```
* handle embedded code

```c++
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
* if a closing tick is found, format the code with `@f(process_code)`

```c++
@add(special content line)
	if (*begin == '*' && (begin + 1) != end && *(begin + 1) == '*') {
		@put(bold block)
	}
@end(special content line)
```
* `**` encloses bold text

```c++
@def(bold block)
	auto w = begin + 2;
	while (w != end && (w + 1) != end && (*w != '*' || *(w + 1) != '*')) {
		++w;
	}
@end(bold block)
```
* search for the closing mark

```c++
@add(bold block)
	if (w != end && (w + 1 ) != end && *w == '*' && *(w + 1) == '*') {
		@put(do bold)
		continue;
	}
@end(bold block)
```
* if end mark is found, format bold

```c++
@def(do bold)
	out << "<b>";
	writeEscaped(out, std::string {begin + 2, w});
	out << "</b>";
	begin = w + 1;
@end(do bold)
```
* format bold

## Paragraphs
* format paragraphs between slide groups

```c++
@add(html state enums)
	, inPara
@end(html state enums)
```
* special state for paragraphs

```c++
@add(close specials)
	if (status.state == HtmlState::inPara) {
		out << "</p>\n";
		status.state = HtmlState::afterSlides;
	}
@end(close specials)
```
* close paragraph if necessary

```c++
@def(process para)
	if (status.state == HtmlState::afterSlide) {
		out << "</div>\n";
	}
@end(process para)
```
* close slide group

```c++
@add(process para)
	if (status.state != HtmlState::inPara) {
		out << "<p>";
		status.state = HtmlState::inPara;
	}
	process_content(out, para.begin(), para.end());
	out << '\n';
@end(process para)
```
* open paragraph
* process content with embedded code

```c++
@add(process block)
	if (b.state == RS::img) {
		@put(open img page)
		for (const auto &img : b.value) {
			@put(process img)
		}
		for (const auto &note : b.notes) {
			@mul(process note)
		}
		@mul(close specials)
		@mul(close slide)
	}
@end(process block)
```
* write code and notes

```c++
@def(open img page)
	if (status.state == HtmlState::afterSlides) {
		out << "<div class=\"slides\">\n";
	}
	status.state = HtmlState::inSlide;
@end(open img page)
```
* close a previous slide, if it is open

```c++
@def(process img)
	out << "<div class=\"page\"><div class=\"slide\">";
	@mul(slide nr)
	out	<< "\n<img src=\"" << img << "\">\n";
	out << "</div>\n";
@end(process img)
```

```c++
@def(slide nr)
	out << "<div class=\"slide-nr\">" << ++slide_nr << "</div>";
@end(slide nr)
```
