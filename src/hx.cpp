
#line 38 "index.md"

	
#line 169 "index.md"

	
#line 36 "read.md"

	#include <string>

#line 177 "read.md"

	#include <fstream>

#line 96 "blocks.md"

	#include <vector>

#line 9 "log.md"

	#include <iostream>
	#include <exception>

#line 258 "frag.md"

	#include <vector>

#line 783 "frag.md"

	#include <sstream>

#line 5 "input.md"

	#include <iostream>
	#include <vector>
	#include <filesystem>

#line 536 "index.md"

	#include <algorithm>

#line 993 "index.md"

	#include <functional>
	#include <sstream>

#line 84 "html.md"

	#include <string>

#line 730 "html.md"

	#include <cctype>

#line 776 "html.md"

	#include <set>

#line 111 "line.md"

	#include <limits>

#line 5 "ncurses.md"

	#if defined HAVE_CONFIG_H
		#include "config.h"
	#endif

#line 14 "ncurses.md"

	#if defined HAVE_NCURSESW_CURSES_H
		#include <ncursesw/curses.h>
	#elif defined HAVE_NCURSESW_H
		#include <ncursesw.h>
	#elif defined HAVE_NCURSES_CURSES_H
		#include <ncurses/curses.h>
	#elif defined HAVE_NCURSES_H
		#include <ncurses.h>
	#elif defined HAVE_CURSES_H
		#include <curses.h>
	#endif

#line 31 "ncurses.md"

	#include <locale.h>

#line 170 "index.md"
;

#line 181 "index.md"

	class Frag;
	class Frag_Ref;

	Frag *find_frag(const std::string &path, const std::string &key, bool local, std::string *got_path = nullptr);
	Frag *find_frag(const std::string &path, const Frag_Ref &ref, std::string *got_path = nullptr);

	Frag &get_frag(const std::string &path, const std::string &key, bool local);
	Frag &get_frag(const std::string &path, const Frag_Ref &ref);
	const std::string &path_for_global_frag(const std::string &key);

	#include <map>
	using Frag_Map = std::map<std::string, Frag>;

	Frag_Map &frag_map(const std::string &in);
	Frag_Map &frag_map();

	void split_frag(Frag *meta, std::map<std::string, std::string> &&values);
	void clear_frags();
	void eval_metas();

#line 5 "read.md"

	
#line 6 "read.md"
;

#line 269 "index.md"

	std::string stylesheet {
		"slides/slides.css"
	};

#line 1068 "index.md"

	
#line 1155 "index.md"

	std::string file_name(const Frag &f) {
		return f.name.substr(6);
	}

#line 1164 "index.md"

	bool file_changed(const Frag &f, std::string cur_path) {
		std::ifstream in(
			file_name(f).c_str()
		);
		if (! check_frag(f, in, cur_path)) {
			return true;
		}
		if (in.get() != EOF) {
			return true;
		}
		return false;
	}

#line 1069 "index.md"

	void files_write() {
		
#line 1087 "index.md"

	for (auto &i : frag_map()) {
		const Frag *frag {
			&i.second
		};
		std::string cur_path { path_for_global_frag(i.first) };
		
#line 1118 "index.md"
 {
	if (frag->isFile()) {
		
#line 1185 "index.md"

	if (file_changed(*frag, cur_path)) {
		std::ofstream out(
			file_name(*frag).c_str()
		);
		serializeFrag(*frag, out, cur_path);
	}

#line 1120 "index.md"
;
	}
} 
#line 1128 "index.md"
 {
	int sum {
		frag->expands()
			+ frag->multiples()
	};
	if (sum <= 0) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not called\n";
	}
} 
#line 1143 "index.md"

	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not populated\n";
	}

#line 1093 "index.md"
;
	}

#line 1101 "index.md"

	for (auto &j : inputs) {
		std::string cur_path { j.first };
		for (auto &i : frag_map(cur_path)) {
			const Frag *frag {
				&i.second
			};
			
#line 1118 "index.md"
 {
	if (frag->isFile()) {
		
#line 1185 "index.md"

	if (file_changed(*frag, cur_path)) {
		std::ofstream out(
			file_name(*frag).c_str()
		);
		serializeFrag(*frag, out, cur_path);
	}

#line 1120 "index.md"
;
	}
} 
#line 1128 "index.md"
 {
	int sum {
		frag->expands()
			+ frag->multiples()
	};
	if (sum <= 0) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not called\n";
	}
} 
#line 1143 "index.md"

	if (! isPopulatedFrag(frag)) {
		std::cerr << "frag [" <<
			frag->name <<
			"] not populated\n";
	}

#line 1108 "index.md"
;
		}
	}

#line 1071 "index.md"
;
	}

#line 1197 "index.md"

	
#line 1246 "index.md"

	bool no_cmds = false;

#line 1198 "index.md"
;
	void files_process() {
		
#line 1217 "index.md"

	for (auto &i : frag_map()) {
		const Frag *frag {
			&i.second
		};
		const std::string cur_path = path_for_global_frag(i.first);
		
#line 1253 "index.md"
 {
	const std::string cmd { frag->cmd() };
	if (cmd.size()) {
		
#line 1263 "index.md"

	std::ostringstream out {};
	serializeFrag(*frag, out, cur_path);
	std::string o { out.str() };
	if (no_cmds) {
		std::cout << o;
	} else {
		
#line 1279 "index.md"

	std::FILE *f {
		popen(cmd.c_str(), "w")
	};
	if (f) {
		std::fwrite(
			o.c_str(), o.size(), 1, f
		);
		pclose(f);
	}

#line 1270 "index.md"
;
	}

#line 1256 "index.md"
;
	}
} 
#line 1223 "index.md"
;
	}

#line 1230 "index.md"

	for (auto &j : inputs) {
		for (auto &i : frag_map(j.first)) {
			const Frag *frag {
				&i.second
			};
			const std::string cur_path = j.first;
			
#line 1253 "index.md"
 {
	const std::string cmd { frag->cmd() };
	if (cmd.size()) {
		
#line 1263 "index.md"

	std::ostringstream out {};
	serializeFrag(*frag, out, cur_path);
	std::string o { out.str() };
	if (no_cmds) {
		std::cout << o;
	} else {
		
#line 1279 "index.md"

	std::FILE *f {
		popen(cmd.c_str(), "w")
	};
	if (f) {
		std::fwrite(
			o.c_str(), o.size(), 1, f
		);
		pclose(f);
	}

#line 1270 "index.md"
;
	}

#line 1256 "index.md"
;
	}
} 
#line 1237 "index.md"
;
		}
	}

#line 1200 "index.md"
;
	}

#line 5 "html.md"

	
#line 52 "html.md"

	enum class HtmlState {
		nothing,
		inSlide,
		afterSlide,
		afterSlides
		
#line 317 "html.md"

	, inCode

#line 961 "html.md"

	, inNotes

#line 1110 "html.md"

	, inPara

#line 58 "html.md"

	};

#line 68 "html.md"

	struct HtmlStatus {
		
#line 77 "html.md"

	HtmlState state = HtmlState::nothing;

#line 70 "html.md"

	};

#line 181 "html.md"
 
	void writeOneEscaped(
		std::ostream &out, char ch
	) {
		switch (ch) {
			
#line 212 "html.md"

	case '<':
		out << "&lt;";
		break;
	case '>':
		out << "&gt;";
		break;
	case '&':
		out << "&amp;";
		break;

#line 186 "html.md"

			default:
				out << ch;
		}
	}

#line 197 "html.md"

	void writeEscaped(
		std::ostream &out,
		const std::string &str
	) {
		for (char ch : str) {
			writeOneEscaped(out, ch);
		}
	}

#line 227 "html.md"

	
#line 458 "html.md"

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

#line 783 "html.md"

	using Set = std::set<std::string>;

#line 790 "html.md"

	bool isKeyword(const std::string &s) {
		static Set reserved {
			
#line 807 "html.md"

	"break", "case", "catch", "continue",
	"default", "delete", "else", "for",
	"if", "in", "new", "return", "static",
	"switch", "try", "typeof", "while",
	"class", "public", "private",
	"template", "typename", "using",
	"function", "throw", "namespace",
	"once", "constexpr", "volatile"

#line 793 "html.md"

		};
		return
			reserved.find(s) !=
				reserved.end() ||
					(s.size() &&
						s[0] == '#'
					);
	}

#line 821 "html.md"

	bool isType(const std::string &s) {
		
#line 831 "html.md"

	static Set reserved {
		
#line 845 "html.md"

	"FILE", "auto", "bool", "char",
	"const", "enum", "extern", "int",
	"let", "long", "signed", "struct",
	"union", "unsigned", "void", "double",
	"string", "std", "ifstream",
	"istream", "ofstream", "ostream",
	"vector", "map", "list"

#line 833 "html.md"

	};
	if (reserved.find(s) !=
		reserved.end()
	) {
		return true;
	}

#line 858 "html.md"

	if (s.size() >= 2) {
		if (isupper(s[0]) &&
			islower(s[1])
		) {
			return true;
		}
	}

#line 823 "html.md"
;
		return false;
	}

#line 872 "html.md"

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

#line 891 "html.md"

	void process_ident(
		std::ostream &out,
		const std::string ident,
		char w
	) {
		
#line 904 "html.md"

	if (isKeyword(ident)) {
		span_str(out, "keyword", ident);
	} else if (w == '(') {
		span_str(out, "fn", ident);
	
#line 920 "html.md"

	} else if (isType(ident)) {
		span_str(out, "type", ident);
	} else if (isNum(ident)) {
		span_str(out, "num", ident);

#line 909 "html.md"

	} else {
		span_str(out, "var", ident);
	}

#line 897 "html.md"
;
	}

#line 931 "html.md"

	void writeMacroClass(
		std::ostream &out,
		const char *name
	) {
		out << "<span class=\"" <<
			name << "\">";
	}

#line 944 "html.md"

	void writeMacroHeader(
		std::ostream &out,
		const std::string &name
	) {
		writeMacroClass(out, "macro");
		out << '@' << name <<
			"(<span class=\"name\">";
	}

#line 228 "html.md"
;
	void process_code(
		std::ostream &out,
		SI begin, SI end
	) {
		
#line 394 "html.md"

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

#line 412 "html.md"

	for (; begin != end; ++begin) {
		
#line 423 "html.md"

	if (
		*begin == '`' ||
		*begin == '\'' ||
		*begin == '"'
	) {
		
#line 437 "html.md"

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

#line 474 "html.md"

	std::string name {begin, w + 1};
	span_str(out, "str", name);
	begin = w;

#line 429 "html.md"
;
		continue;
	}

#line 483 "html.md"

	if (*begin == '@') {
		auto nb = begin + 1;
		auto ne = nb;
		
#line 494 "html.md"

	while (ne != end && *ne != '(') {
		if (! isalpha(*ne)) {
			ne = end;
			break;
		}
		++ne;
	}

#line 508 "html.md"

	if (ne != end) {
		std::string name {nb, ne};
		auto ab = ne + 1;
		auto ae = ab;
		
#line 520 "html.md"

	while (ae != end && *ae != ')') {
		if (*ae == '@') {
			++ae;
			if (ae == end) { break; }
		}
		++ae;
	}
	if (ae != end) {
		std::string arg {ab, ae};
		
#line 538 "html.md"

	do {
		
#line 563 "html.md"

	static Set macros = {
		"def", "end", "add", "put", "mul",
		"Def", "Add", "Mul", "rep", "Rep",
		"Put", "End"
	};

#line 574 "html.md"

	if (
		macros.find(name) != macros.end()
	) {
		writeMacroHeader(out, name);
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}

#line 588 "html.md"

	if (name == "inc") {
		
#line 598 "html.md"

	auto ext = arg.find_last_of('.');
	if (ext == std::string::npos) {
		ext = arg.size();
	}
	writeMacroHeader(out, name);
	out << "<a href=\"" <<
		arg.substr(0, ext) <<
		".html\">";
	out << arg <<
		"</a></span>)</span>";

#line 590 "html.md"
;
		break;
	}

#line 614 "html.md"

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

#line 632 "html.md"

	if (name == "f" || name == "fn") {
		writeMacroClass(out, "fn");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 644 "html.md"

	if (name == "v" || name == "var") {
		writeMacroClass(out, "var");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 656 "html.md"

	if (name == "k" || name == "key") {
		writeMacroClass(out, "keyword");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 668 "html.md"

	if (name == "n" || name == "num") {
		writeMacroClass(out, "num");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 680 "html.md"

	if (name == "t" || name == "typ") {
		writeMacroClass(out, "type");
		writeEscaped(out, arg);
		out << "</span>";
		break;
	}

#line 692 "html.md"

	if (name == "b" || name == "br") {
		writeMacroClass(out, "virt");
		out << "</span><br/>";
		break;
	}

#line 704 "html.md"

	if (name == "priv") {
		writeMacroClass(out, "var");
		out << "@priv(<span>";
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}

#line 717 "html.md"

	if (name == "magic") {
		writeMacroClass(out, "num");
		out << "@magic(<span>";
		writeEscaped(out, arg);
		out << "</span>)</span>";
		break;
	}

#line 540 "html.md"
;
		
#line 551 "html.md"

	writeOneEscaped(out, '@');
	writeEscaped(out, name);
	writeOneEscaped(out, '(');
	writeEscaped(out, arg);
	writeOneEscaped(out, ')');

#line 541 "html.md"
;
	} while (false);
	begin = ae;

#line 530 "html.md"
;
		continue;
	}

#line 513 "html.md"
;
	}

#line 487 "html.md"
;
	}

#line 737 "html.md"

	auto w = begin;
	
#line 749 "html.md"

	while (w != end && (
		std::isalnum(*w) ||
			*w == '_' || *w == '$' ||
			*w == '#'
	)) {
		++w;
	}

#line 739 "html.md"
;
	if (w != begin) {
		
#line 763 "html.md"

	std::string ident {begin, w};
	begin = w - 1;
	process_ident(
		out, ident,
		w != end ? *w : ' '
	);

#line 741 "html.md"
;
		continue;
	}

#line 414 "html.md"
;
		writeOneEscaped(out, *begin);
	}

#line 233 "html.md"
;
	}

#line 243 "html.md"

	void process_content(
		std::ostream &out,
		SI begin, SI end
	) {
		
#line 1018 "html.md"

	for(; begin != end; ++begin) {
		
#line 1030 "html.md"

	if (*begin == '`') {
		
#line 1039 "html.md"

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

#line 1032 "html.md"
;
	}

#line 1056 "html.md"

	if (
		*begin == '*' &&
		(begin + 1) != end &&
		*(begin + 1) == '*'
	) {
		
#line 1069 "html.md"

	auto w = begin + 2;
	while (
		w != end && (w + 1) != end &&
		(*w != '*' || *(w + 1) != '*')
	) {
		++w;
	}

#line 1082 "html.md"

	if (
		w != end && (w + 1 ) != end &&
		*w == '*' && *(w + 1) == '*'
	) {
		
#line 1095 "html.md"

	out << "<b>";
	writeEscaped(
		out, std::string {begin + 2, w}
	);
	out << "</b>";
	begin = w + 1;

#line 1087 "html.md"
;
		continue;
	}

#line 1062 "html.md"
;
	}

#line 1020 "html.md"
;
		writeOneEscaped(out, *begin);
	}

#line 248 "html.md"
;
	}

#line 6 "html.md"

	void write_html() {
		for (auto &cur : inputs) {
			
#line 9 "html.md"
;
		}
	}

#line 5 "view.md"

	bool interactive = false;

#line 12 "view.md"

	std::map<std::string, Input>::iterator curInput;
	std::vector<Block>::iterator curBlock;

#line 20 "view.md"

	bool write_files = true;

#line 27 "view.md"

	bool process_files = true;

#line 35 "view.md"

	bool html_files = true;

#line 77 "view.md"

	void draw_block() {
		
#line 79 "view.md"
;
	}

#line 96 "view.md"

	void draw_position() {
		
#line 98 "view.md"
;
	}

#line 105 "view.md"

	void trim(std::string &s) {
		while (
			! s.empty() &&
				(s[0] & 0xff) <= ' '
		) {
			s.erase(0, 1);
		}
	}

#line 5 "line.md"

	class Line {
		public:
			
#line 8 "line.md"
;
		private:
			
#line 10 "line.md"
;
	};

#line 130 "line.md"

	
#line 5 "range.md"

	class Range {
		public:
			
#line 8 "range.md"
;
		private:
			Line _prev;
			Line _last;
	};

#line 131 "line.md"
;
	
#line 139 "range.md"

	Range range;

#line 132 "line.md"
;

#line 146 "line.md"

	int get_number(std::string &s) {
		int res = 0;
		while (
			! s.empty() && isdigit(s[0])
		) {
			res = res * 10 + s[0] - '0';
			s.erase(0, 1);
		}
		return res;
	}

#line 162 "line.md"

	Line get_line(std::string &s) {
		Line line {};
		do {
			
#line 166 "line.md"
;
		} while (false);
		trim(s);
		return line;
	}

#line 5 "edit.md"

	void insert_before(
		const std::string &prefix,
		std::vector<std::string> &c
	) {
		
#line 10 "edit.md"
;
	}

#line 5 "write.md"

	
#line 6 "write.md"

	void write_input() {
		for (const auto &cur : inputs) {
			
#line 9 "write.md"
;
		}
	}

#line 302 "write.md"

	bool is_prefix(
		const std::string &s,
		const std::string &p
	) {
		return s.size() >= p.size() &&
			std::equal(
				p.begin(), p.end(),
				s.begin()
			);
	}

#line 8 "add.md"

	void add_block(Read_State state) {
		
#line 10 "add.md"
;
	}

#line 38 "ncurses.md"

	#if HAVE_CURSES
		
#line 47 "ncurses.md"

	
#line 54 "ncurses.md"

	bool with_ncurses = false;

#line 99 "ncurses.md"

	class Ncurses_Handler {
		public:
			Ncurses_Handler() {
				
#line 114 "ncurses.md"

	setlocale(LC_CTYPE, "");
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();

#line 103 "ncurses.md"
;
			}
			~Ncurses_Handler() {
				
#line 125 "ncurses.md"

	endwin();

#line 106 "ncurses.md"
;
			}
	};

#line 132 "ncurses.md"

	struct End_Of_Curses {};

#line 139 "ncurses.md"

	
#line 185 "ncurses.md"

	void draw_number(int l) {
		int r = l / 10;
		if (r) { draw_number(r); }
		addch((l % 10) + '0');
	}

#line 196 "ncurses.md"

	void draw_line(int l) {
		if (l <= 9) {
			addch(' ');
		}
		draw_number(l);
		addstr(": ");
	}

#line 140 "ncurses.md"

	void draw_page() {
		clear();
		move(0, 0);
		
#line 210 "ncurses.md"

	if (curBlock->state == RS::header) {
		
#line 219 "ncurses.md"

	int i = 0;
	for (
		const auto &l : curBlock->value
	) {
		
#line 231 "ncurses.md"

	draw_line(++i);
	for (
		int j = 0; j < curBlock->level;
		++j
	) {
		addch('#');
	}
	addch(' ');
	addstr(l.c_str());
	addstr("\n\n");

#line 224 "ncurses.md"
;
	}

#line 212 "ncurses.md"
;
	}

#line 247 "ncurses.md"

	if (curBlock->state == RS::code) {
		
#line 256 "ncurses.md"

	addstr("    ```\n");
	int i = 0;
	for (
		const auto &l : curBlock->value
	) {
		draw_line(++i);
		addstr(l.c_str());
		addch('\n');
	}
	addstr("    ```\n\n");

#line 249 "ncurses.md"
;
	}

#line 272 "ncurses.md"

	if (curBlock->state == RS::para) {
		
#line 281 "ncurses.md"

	int i = 0;
	for (
		const auto &l : curBlock->value
	) {
		draw_line(++i);
		addstr(l.c_str());
		addstr("\n\n");
	}

#line 274 "ncurses.md"
;
	}

#line 295 "ncurses.md"

	int j = 0;
	for (
		const auto &l : curBlock->notes
	) {
		draw_line(++j);
		addstr("* ");
		addstr(l.c_str());
		addch('\n');
	}
	addch('\n');

#line 311 "ncurses.md"

	int idx = 1;
	for (const auto &xx : inputs) {
		if (xx.first == curInput->first) { break; }
		++idx;
	}
	draw_number(idx);
	if (idx ==
		static_cast<int>(inputs.size())
	) {
		addstr(" = $");
	}
	addch(' ');
	addstr(curInput->first.c_str());
	addch(':');

#line 332 "ncurses.md"

	auto &bs { curInput->second.blocks };
	idx = (curBlock - bs.begin()) + 1;
	draw_number(idx);
	if (
		idx == static_cast<int>(bs.size())
	) {
		addstr(" = $");
	}

#line 144 "ncurses.md"
;
		refresh();
	}

#line 48 "ncurses.md"


#line 40 "ncurses.md"

	#endif

#line 1353 "index.md"

	using Inputs_Frag_Map = std::map<std::string, Frag_Map>;

	class Frag_State {
		public:
			std::unique_ptr<Frag_State> parent;
			Inputs_Frag_Map state;
			Frag_State(std::unique_ptr<Frag_State> &&p): parent { std::move(p) } { }
			Frag *meta = nullptr;
			std::string meta_path;
			std::map<std::string, std::string> meta_values;
	};

	std::unique_ptr<Frag_State> _all_frags = std::move(std::make_unique<Frag_State>(nullptr));
	Frag_State *_cur_state = nullptr;

	Frag_State &cur_state() {
		return _cur_state ? *_cur_state : *_all_frags;
	}

	Frag *find_frag(Frag_State &state, const std::string &in, const std::string &key) {
		auto got { state.state[in].find(key) };
		if (got != state.state[in].end()) {
			return &got->second;
		}
		if (state.parent) {
			Frag *pg = find_frag(*state.parent, in, key);
			if (pg) {
				pg->super = &state.state[in].insert({ key, { key, pg } }).first->second;
				return pg->super;
			}
		}
		return nullptr;
	}

	Frag *find_frag(const std::string &in, const std::string &key) {
		return find_frag(cur_state(), in, key);
	}

	Frag *find_frag_in_files(const std::string &path, const std::string &key, std::string *got_path) {
		std::string p { path };
		for (;;) {
			Frag *f { find_frag(p, key) };
			if (f) {
				if (got_path) { *got_path = p; }
				return f;
			}
			const Input &i { inputs[p] };
			if (i.prev.empty()) { return nullptr; }
			p = i.prev;
		}
	}

	Frag *find_frag(const std::string &path, const std::string &key, bool local, std::string *got_path) {
		if (local) {
			if (got_path) { *got_path = path; }
			return find_frag(path, key);
		} else {
			Frag *f { nullptr };
			Input &i { inputs[path] };
			if (! i.prev.empty()) {
				f = find_frag_in_files(i.prev, key, got_path);
			}
			if (! f) {
				f = find_frag(std::string { }, key);
				if (f) {
					if (got_path) { *got_path = path_for_global_frag(key); }
				}
			}
			return f;
		}
	}

	Frag *find_frag(const std::string &path, const Frag_Ref &ref, std::string *got_path) {
		return find_frag(path, ref.name, ref.local, got_path);
	}

	Frag &add_frag(Frag_State &state, const std::string &in, const std::string &key) {
		Frag *prev { nullptr };
		if (state.parent) {
			prev = &add_frag(*state.parent, in, key);
		}
		Frag &res { state.state[in].insert({ key, { key, prev } }).first->second };
		if (prev) { prev->super = &res; }
		return res;
	}

	std::map<std::string, std::string> _global_frags;

	Frag &add_frag(const std::string &in, const std::string &key) {
		return add_frag(cur_state(), in, key);
	}

	Frag &get_frag(const std::string &path, const std::string &key, bool local) {
		std::cerr << "get [" << key << "], " << ( local ? "local": "global") << ", " << path << "\n";
		Frag *f { find_frag(path, key, local) };
		if (f) { return *f; }
		if (! local) {
			_global_frags[key] = path;
		}
		return add_frag(local ? path : std::string { }, key);
	}

	Frag &get_frag(const std::string &path, const Frag_Ref &ref) {
		return get_frag(path, ref.name, ref.local);
	}

	Frag_Map &frag_map(Frag_State &state, const std::string &in) {
		Frag_Map &cur { state.state[in] };
		if (state.parent) {
			Frag_Map &prev { frag_map(*state.parent, in) };
			for (auto &f: prev) {
				if (cur.find(f.first) == cur.end()) {
					f.second.super = &cur.insert({ f.first, { f.first, &f.second } }).first->second;
				}
			}
		}
		return cur;
	}
	Frag_Map &frag_map(const std::string &in) {
		return frag_map(cur_state(), in);
	}

	Frag_Map &frag_map() {
		return frag_map(std::string { });
	}

	void split_frag(Frag *meta, std::map<std::string, std::string> &&values) {
		Frag_State &current = *_all_frags;
		current.meta = meta;
		current.meta_path = inputs.open_head();
		current.meta_values = std::move(values);
		std::unique_ptr<Frag_State> n { std::move(std::make_unique<Frag_State>(std::move(_all_frags))) };
		_all_frags = std::move(n);
		_cur_state = nullptr;
	}

	void clear_frags() { 
		_all_frags = std::move(std::make_unique<Frag_State>(nullptr)); _cur_state = nullptr;
		_global_frags.clear();
	}

	void eval_meta(Frag_State &fs) {
		if (fs.parent) {
			eval_meta(*fs.parent);
		}
		if (fs.meta) {
			
#line 1515 "index.md"

	std::ostringstream out;
	serializeFrag(*fs.meta, out, fs.meta_path);
	std::istringstream in { out.str() };
	std::string line;
	Frag *frag = nullptr;
	std::string cur_path = fs.meta_path;
	int cur_line { 1 };
	auto &cmd_values = fs.meta_values;
	_cur_state = &fs;
	while (std::getline(in, line)) {
		auto end = line.cend();
		for (
			auto i = line.cbegin();
			i != end; ++i
		) {
			
#line 398 "index.md"

	if (*i == '@') {
		auto nb = i + 1;
		auto ne = nb;
		
#line 416 "index.md"

	while (ne != end && *ne != '(') {
		if (! isalpha(*ne)) {
			ne = end;
			break;
		}
		++ne;
	}

#line 402 "index.md"
;
		if (ne != end && ne != nb) {
			std::string name { nb, ne };
			
#line 430 "index.md"

	auto ab = ne + 1; auto ae = ab;
	while (ae != end && *ae != ')') {
		if (*ae == '@') {
			
#line 450 "index.md"

	if (++ae == end) { break; }
	if (isalpha(*ae)) {
		
#line 459 "index.md"

	auto ac { ae };
	while (isalpha(*ac)) {
		++ac; if (ac == end) { break; }
	}
	if (ac != end && *ac == '(') {
		int cnt = 1; ++ac;
		while (ac != end && cnt != 0) {
			if (*ac == '(') { ++cnt; }
			if (*ac == ')') { --cnt; }
			++ac;
		}
		if (cnt == 0) {
			ae = ac - 1;
		}
	}

#line 453 "index.md"
;
	}

#line 434 "index.md"
;
		}
		++ae;
	}
	if (ae != end) {
		std::string arg {ab, ae};
		
#line 479 "index.md"

	i = ae;
	bool outside = ! frag;
	do {
		if (outside && ! blockLimit) {
			break;
		}
		
#line 605 "index.md"

	if (name == "def") {
		ASSERT_NOT_FRAG();
		frag = &get_frag(cur_path, arg, true);
		CHECK_NOT_DEFINED();
		break;
	}

#line 630 "index.md"
 {
	auto i { cmd_values.find(name) };
	if (i != cmd_values.end()) {
		frag->add(i->second, cur_path, cur_line);
		break;
	}
} 
#line 640 "index.md"

	if (name == "end" || name == "End") {
		ASSERT_FRAG();
		if (frag->is_meta()) {
			std::string pattern;
			std::map<std::string, std::string> values;
			parse_args(arg, pattern, values);
			if (frag->name == pattern) {
				frag = nullptr;
			} else {
				auto f { cur_path };
				auto l { cur_line };
				frag->add('@', f, l);
				frag->add(name, f, l);
				frag->add('(', f, l);
				frag->add(arg, f, l);
				frag->add(')', f, l);
			}
		} else {
			
#line 669 "index.md"

	ASSERT_MSG(frag->name == arg,
		"closing [" << arg <<
		"] != [" << frag->name << ']'
	);

#line 659 "index.md"
;
			frag = nullptr;
		}
		break;
	}

#line 691 "index.md"

	if (name == "add") {
		if (frag && frag->is_meta()) {
			auto f { cur_path };
			auto l { cur_line };
			frag->add('@', f, l);
			frag->add(name, f, l);
			frag->add('(', f, l);
			frag->add(arg, f, l);
			frag->add(')', f, l);
		} else {
			ASSERT_NOT_FRAG();
			frag = &get_frag(cur_path, arg, true);
			CHECK_DEFINED();
		}
		break;
	}

#line 745 "index.md"

	if (name == "put") {
		if (! frag && arg.find('@') != std::string::npos) {
			std::string pattern;
			std::map<std::string, std::string> values;
			parse_args(arg, pattern, values);
			Frag *sub = &get_frag(cur_path, pattern, true);
			sub->addMultiple();
			split_frag(sub, std::move(values));
		} else {
			ASSERT_MSG(frag, "@put" << "(" <<
				arg << ") not in frag"
			);
			Frag *sub = &get_frag(cur_path, arg, true);
			ASSERT(sub);
			
#line 773 "index.md"

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

#line 760 "index.md"
;
			sub->addExpand();
			frag->add(cur_path, sub, true);
		}
		break;
	}

#line 790 "index.md"

	if (name == "inc") {
		ASSERT_MSG(! frag,
			"include in frag [" <<
				frag->name << ']'
		);
		if (! inputs.has(arg)) {
			inputs.push(arg);
		}
		break;
	}

#line 808 "index.md"

	if (name == "mul") {
		ASSERT_MSG(frag,
			"@mul not in frag"
		);
		Frag *sub = &get_frag(cur_path, arg, true);
		if (sub) {
			
#line 826 "index.md"

	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}

#line 815 "index.md"
;
			sub->addMultiple();
			frag->add(cur_path, sub, true);
		}
		break;
	}

#line 839 "index.md"

	if (name == "Def") {
		
#line 849 "index.md"

	ASSERT_MSG(! frag,
		"@Def in frag [" <<
		frag->name << ']'
	);
	frag = &get_frag(cur_path, arg, false);
	if (isPopulatedFrag(frag)) {
		std::cerr << "Frag [" <<
			arg << "] already defined\n";
	}

#line 841 "index.md"
;
		break;
	}

#line 864 "index.md"

	if (name == "Add") {
		
#line 874 "index.md"

	ASSERT_MSG(! frag, "@Add in frag [" <<
		frag->name << ']'
	);
	frag = &get_frag(cur_path, arg, false);
	if (! isPopulatedFrag(frag)) {
		std::cerr << "Frag [" << arg <<
			"] not defined\n";
	}

#line 866 "index.md"
;
		break;
	}

#line 890 "index.md"

	if (name == "rep") {
		ASSERT_MSG(! frag,
			"@rep in frag [" <<
				frag->name << ']'
		);
		frag = &get_frag(cur_path, arg, true);
		
#line 920 "index.md"

	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();

#line 897 "index.md"
;
		break;
	}

#line 905 "index.md"

	if (name == "Rep") {
		ASSERT_MSG(! frag,
			"@Rep in frag [" <<
				frag->name << ']'
		);
		frag = &get_frag(cur_path, arg, false);
		
#line 920 "index.md"

	ASSERT_MSG(frag, "frag [" <<
		name <<
		"] not defined"
	);
	frag->clear();

#line 912 "index.md"
;
		break;
	}

#line 932 "index.md"

	if (name == "Put") {
		
#line 942 "index.md"

	ASSERT_MSG(frag, "@Put not in frag");
	Frag *sub = &get_frag(cur_path, arg, false);
	if (sub) {
		
#line 773 "index.md"

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

#line 946 "index.md"
;
		sub->addExpand();
		frag->add(cur_path, sub, false);
	}

#line 934 "index.md"
;
		break;
	}

#line 955 "index.md"

	if (name == "Mul") {
		
#line 965 "index.md"

	ASSERT_MSG(frag, "@Mul not in frag");
	Frag *sub = &get_frag(cur_path, arg, false);
	if (sub) {
		
#line 826 "index.md"

	if (sub->expands()) {
		std::cerr <<
			"multiple after " <<
			"expand of [" <<
			sub->name << "]\n";
	}

#line 969 "index.md"
;
		sub->addMultiple();
		frag->add(cur_path, sub, false);
	}

#line 957 "index.md"
;
		break;
	}

#line 978 "index.md"

	if (name == "priv") {
		ASSERT_MSG(frag,
			"@priv not in frag"
		);
		
#line 1001 "index.md"

	std::hash<std::string> h;
	auto cur {
		h(cur_path + ':' + arg) &
			0x7fffffff
	};

#line 1013 "index.md"

	std::ostringstream hashed;
	hashed << "_private_" <<
		cur << '_' <<
		arg;
	frag->add(
		hashed.str(), cur_path, cur_line
	);

#line 983 "index.md"
;
		break;
	}

#line 1028 "index.md"

	if (name == "magic") {
		ASSERT_MSG(frag,
			"@magic not in frag"
		);
		
#line 1041 "index.md"

	std::hash<std::string> h;
	auto cur {
		h(cur_path + ':' + arg) &
			0x7fffffff
	};

#line 1052 "index.md"

	std::ostringstream value;
	value << cur;
	frag->add(
		value.str(), cur_path, cur_line
	);

#line 1033 "index.md"
;
		break;
	}

#line 486 "index.md"
;
		
#line 516 "index.md"

	if (frag) {
		if (frag->is_meta()) {
			auto f { cur_path };
			auto l { cur_line };
			frag->add('@', f, l);
			frag->add(name, f, l);
			frag->add('(', f, l);
			frag->add(arg, f, l);
			frag->add(')', f, l);
		} else {
			expand_cmd_arg(frag, arg, cur_path, cur_line);
		}
	}

#line 487 "index.md"
;
	} while (false);
	if (blockLimit && outside && frag) {
		--blockLimit;
	}

#line 440 "index.md"
;
		continue;
	}

#line 405 "index.md"
;
		}
	}

#line 1531 "index.md"
;
			process_char(frag, *i, cur_path, cur_line);
		}
		process_char(frag, '\n', cur_path, cur_line);
	}
	_cur_state = nullptr;

#line 1500 "index.md"
;
		}
	}

	void eval_metas() {
		eval_meta(*_all_frags);
	}

	const std::string &path_for_global_frag(const std::string &key) {
		return _global_frags[key];
	}

#line 39 "index.md"

	int main(
		int argc,
		const char **argv
	) {
		
#line 115 "index.md"

	#if ! NDEBUG
		
#line 397 "frag.md"

	
#line 398 "frag.md"
;

#line 241 "line.md"

	
#line 242 "line.md"
;

#line 117 "index.md"
;
	#endif

#line 132 "index.md"

	
#line 249 "index.md"

	for (int i { 1 }; i < argc; ++i) {
		std::string arg { argv[i] };
		
#line 280 "index.md"
 {
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
} 
#line 303 "index.md"
 {
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
} 
#line 1294 "index.md"
 {
	static const std::string prefix {
		"--no-cmds"
	};
	if (arg == prefix) {
		no_cmds = true;
		continue;
	}
} 
#line 42 "view.md"

	if (
		arg == "-i" ||
		arg == "--interactive"
	) {
		interactive = true;
		write_files = false;
		process_files = false;
		html_files = false;
		continue;
	}

#line 61 "ncurses.md"

	#if HAVE_CURSES
		if (
			arg == "-c" ||
			arg == "--curses"
		) {
			
#line 67 "ncurses.md"
;
			continue;
		}
	#endif

#line 252 "index.md"
;
		
#line 321 "index.md"

	inputs.add(arg);
	continue;

#line 253 "index.md"
;
		ASSERT_MSG(false,
			"unknown argument [" <<
			argv[i] << ']'
		);
	}

#line 133 "index.md"
;

#line 145 "index.md"

	
#line 101 "read.md"

	read_sources();

#line 146 "index.md"
;

#line 154 "index.md"

	
#line 1078 "index.md"

	if (write_files) {
		files_write();
	}

#line 1208 "index.md"

	if (process_files) {
		files_process();
	}

#line 155 "index.md"
;

#line 162 "index.md"

	
#line 17 "html.md"

	if (html_files) {
		write_html();
	}

#line 163 "index.md"
;

#line 58 "view.md"

	if (interactive) {
		
#line 67 "view.md"

	curInput = inputs.begin();
	std::cerr << "curInput == " << curInput->first << "; is end == " << (curInput == inputs.end()) << "\n";
	curBlock = curInput->second.blocks.begin();
	std::cerr << "curBlock == " << &*curBlock << "; is end == " << (curBlock == curInput->second.blocks.end()) << "\n";

#line 86 "view.md"

	draw_block();
	for (;;) {
		
#line 119 "view.md"

	std::string cmd;
	draw_position();
	std::cout << "> ";
	std::getline(std::cin, cmd);
	trim(cmd);
	if (cmd.empty()) { continue; }
	
#line 65 "range.md"

	range = Range {};
	range << get_line(cmd);
	if (! cmd.empty() && cmd[0] == ',') {
		cmd.erase(0, 1);
		range << get_line(cmd);
	}

#line 126 "view.md"
;

#line 134 "view.md"

	if (cmd == "q" || cmd == "quit") {
		break;
	}

#line 143 "view.md"

	if (cmd == "n" || cmd == "next") {
		
#line 154 "view.md"

	int next = (curBlock -
		curInput->second.blocks.begin()) + 1;
	while (next >= static_cast<int>(
		curInput->second.blocks.size()
	)) {
		--next;
	}
	
#line 78 "range.md"

	if (range) {
		next = range.last()((curBlock -
			curInput->second.blocks.begin()) + 1,
			curInput->second.blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 162 "view.md"
;
	curBlock =
		curInput->second.blocks.begin() + next;

#line 145 "view.md"
;
		draw_block();
		continue;
	}

#line 170 "view.md"

	if (cmd == "p" || cmd == "prev") {
		
#line 181 "view.md"

	int next =curBlock -
		curInput->second.blocks.begin();
	if (next > 0) {
		--next;
	}
	
#line 78 "range.md"

	if (range) {
		next = range.last()((curBlock -
			curInput->second.blocks.begin()) + 1,
			curInput->second.blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 187 "view.md"
;
	curBlock =
		curInput->second.blocks.begin() + next;

#line 172 "view.md"
;
		draw_block();
		continue;
	}

#line 308 "view.md"

	if (cmd == "f" || cmd == "forward") {
		
#line 319 "view.md"

	int next = 1;
	for (const auto &xx : inputs) {
		if (xx.first == curInput->first) { break; }
		++next;
	}
	while (next >= static_cast<int>(
		inputs.size()
	)) {
		--next;
	}
	
#line 91 "range.md"

	if (range) {
		int idx = 1;
		for (const auto &xx : inputs) {
			if (xx.first == curInput->first) { break; }
			++idx;
		}
		next = range.last()(
			idx,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 330 "view.md"
;
	curInput = inputs.begin();
	for (; next > 0 && curInput != inputs.end(); --next) {
		++curInput;
	}
	curBlock = curInput->second.blocks.begin();

#line 310 "view.md"
;
		draw_block();
		continue;
	}

#line 341 "view.md"

	if (cmd == "b" || cmd == "backward") {
		
#line 352 "view.md"

	int next = 0;
	for (const auto &xx : inputs) {
		if (xx.first == curInput->first) { break; }
		++next;
	}
	if (next) {
		--next;
	}
	
#line 91 "range.md"

	if (range) {
		int idx = 1;
		for (const auto &xx : inputs) {
			if (xx.first == curInput->first) { break; }
			++idx;
		}
		next = range.last()(
			idx,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 361 "view.md"
;
	curInput = inputs.begin();
	for (; next > 0 && curInput != inputs.end(); --next) {
		++curInput;
	}
	curBlock = curInput->second.blocks.begin();

#line 343 "view.md"
;
		draw_block();
		continue;
	}

#line 18 "edit.md"

	if (cmd == "l" || cmd == "list") {
		insert_before(
			"l", curBlock->notes
		);
		continue;
	}

#line 30 "edit.md"

	if (cmd == "a" || cmd == "add") {
		std::string prefix;
		
#line 33 "edit.md"
;
		insert_before(
			prefix,
			curBlock->value
		);
		continue;
	}

#line 94 "edit.md"

	if (cmd == ">>") {
		if (
			curBlock->state == RS::header
		) {
			++curBlock->level;
			draw_block();
			continue;
		}
	}

#line 109 "edit.md"

	if (cmd == "<<") {
		if (curBlock->level > 1) {
			--curBlock->level;
			draw_block();
			continue;
		}
	}

#line 17 "write.md"

	if (cmd == "W" || cmd == "Write") {
		write_input();
		continue;
	}

#line 213 "write.md"

	if (cmd == "H" || cmd == "Html") {
		write_input();
		write_html();
		continue;
	}

#line 225 "write.md"

	if (cmd == "F" || cmd == "Files") {
		write_input();
		write_html();
		Inputs old { std::move(inputs) };
		
#line 230 "write.md"
;
		curInput = inputs.begin();
		curBlock =
			curInput->second.blocks.begin();
		continue;
	}

#line 256 "write.md"

	if (cmd == "P" || cmd == "Process") {
		write_input();
		write_html();
		Inputs old { std::move(inputs) };
		
#line 261 "write.md"
;
		curInput = inputs.begin();
		curBlock =
			curInput->second.blocks.begin();
		continue;
	}

#line 290 "write.md"

	if (cmd == "M" || cmd == "Make") {
		write_input();
		system("make");
		continue;
	}

#line 318 "write.md"
 {
	static const std::string p { "M " };
	if (is_prefix(cmd, p)) {
		write_input();
		system(("make " +
			cmd.substr(p.size())).c_str()
		);
		continue;
	}
} 
#line 333 "write.md"
 {
	static const std::string p {
		"Make "
	};
	if (is_prefix(cmd, p)) {
		write_input();
		system(("make " + 
			cmd.substr(p.size())
		).c_str());
		continue;
	}
} 
#line 350 "write.md"

	if (cmd == "G" || cmd == "Git") {
		write_input();
		system("git status");
		continue;
	}

#line 362 "write.md"
 {
	static const std::string p { "G " };
	if (is_prefix(cmd, p)) {
		write_input();
		system(("git " +
			cmd.substr(p.size())
		).c_str());
		continue;
	}
} 
#line 377 "write.md"
 {
	static const std::string p { "Git " };
	if (is_prefix(cmd, p)) {
		write_input();
		system(("git " +
			cmd.substr(p.size())
		).c_str());
		continue;
	}
} 
#line 19 "add.md"

	if (cmd == "h" || cmd == "header") {
		add_block(RS::header);
		continue;
	}

#line 29 "add.md"

	if (cmd == "c" || cmd == "code") {
		add_block(RS::code);
		continue;
	}

#line 39 "add.md"

	if (cmd == "o" || cmd == "other") {
		add_block(RS::para);
		continue;
	}

#line 101 "add.md"

	if (cmd == "d" || cmd == "dup") {
		if (curInput != inputs.end()) {
			if (curBlock !=
				curInput->second.blocks.end()
			) {
				
#line 107 "add.md"
;
			}
		} else {
			std::cerr << "! no file\n";
		}
		continue;
	}

#line 89 "view.md"
;
	}

#line 60 "view.md"
;
	}

#line 88 "ncurses.md"

	#if HAVE_CURSES
		if (with_ncurses) {
			
#line 152 "ncurses.md"

	Ncurses_Handler handler;
	curInput = inputs.begin();
	curBlock = curInput->second.blocks.begin();
	draw_page();

#line 163 "ncurses.md"

	int ch;
	try {
		for (;;) {
			switch (ch = getch()) {
				
#line 178 "ncurses.md"

	case 'q': throw End_Of_Curses {};

#line 347 "ncurses.md"

	case 'n': {
		
#line 358 "ncurses.md"

	int next = (curBlock -
		curInput->second.blocks.begin()) + 1;
	while (next >= static_cast<int>(
		curInput->second.blocks.size()
	)) {
		--next;
	}
	
#line 78 "range.md"

	if (range) {
		next = range.last()((curBlock -
			curInput->second.blocks.begin()) + 1,
			curInput->second.blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 366 "ncurses.md"
;
	curBlock = curInput->second.blocks.begin() +
		next;

#line 349 "ncurses.md"
;
		draw_page();
		break;
	}

#line 374 "ncurses.md"

	case 'p' : {
		
#line 385 "ncurses.md"

	int next = curBlock -
		curInput->second.blocks.begin();
	if (next > 0) {
		--next;
	}
	
#line 78 "range.md"

	if (range) {
		next = range.last()((curBlock -
			curInput->second.blocks.begin()) + 1,
			curInput->second.blocks.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 391 "ncurses.md"
;
	curBlock =
		curInput->second.blocks.begin() + next;

#line 376 "ncurses.md"
;
		draw_page();
		break;
	}

#line 399 "ncurses.md"

	case 'f': {
		
#line 410 "ncurses.md"

	int next = 1;
	for (const auto &xx : inputs) {
		if (xx.first == curInput->first) { break; }
		++next;
	}
	while (next >= static_cast<int>(
		inputs.size()
	)) {
		--next;
	}
	
#line 91 "range.md"

	if (range) {
		int idx = 1;
		for (const auto &xx : inputs) {
			if (xx.first == curInput->first) { break; }
			++idx;
		}
		next = range.last()(
			idx,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 421 "ncurses.md"
;
	curInput = inputs.begin();
	for (; next > 0 && curInput != inputs.end(); --next) {
		++curInput;
	}
	curBlock = curInput->second.blocks.begin();

#line 401 "ncurses.md"
;
		draw_page();
		continue;
	}

#line 432 "ncurses.md"

	case 'b': {
		
#line 443 "ncurses.md"

	int next = 0;
	for (const auto &xx : inputs) {
		if (xx.first == curInput->first) { break; }
		++next;
	}
	if (next) {
		--next;
	}
	
#line 91 "range.md"

	if (range) {
		int idx = 1;
		for (const auto &xx : inputs) {
			if (xx.first == curInput->first) { break; }
			++idx;
		}
		next = range.last()(
			idx,
			inputs.size()
		) - 1;
		if (next < 0) { next = 0; }
	}

#line 452 "ncurses.md"
;
	curInput = inputs.begin();
	for (; next > 0 && curInput != inputs.end(); --next) {
		++curInput;
	}
	curBlock = curInput->second.blocks.begin();

#line 434 "ncurses.md"
;
		draw_page();
		continue;
	}

#line 168 "ncurses.md"

			}
			draw_page();
		}
	} catch (const End_Of_Curses &) {}

#line 91 "ncurses.md"
;
		}
	#endif

#line 44 "index.md"

	}
