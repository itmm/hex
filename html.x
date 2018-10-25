# HTML-Präsentation generieren
* Diese Datei beschreibt Code, um aus den `hx`-Quelldateien
  HTML-Präsentationen zu erzeugen

```
a{global elements}
	t{struct SourceElement} {
		t{struct SourceElement *}p{link};
		t{unsigned} p{magic};
		t{char} v{path}[];
	};
x{global elements}
```
* Alle Dateien, die noch herausgeschrieben werden müssen, werden in einer
  Liste vorgehalten

```
a{global elements}
	t{struct SourceElement} *createSourceElement(
		t{const char *}v{path}
	) {
		f{ASSERT}(v{path});
		t{int} v{len} = f{strlen}(v{path}) + n{1};
		t{int} v{size} =
			v{len} + f{sizeof}(t{struct SourceElement});
		t{struct SourceElement *}v{se} =
			f{malloc}(v{size});
		f{ASSERT}(v{se});
		v{se}->p{link} = k{NULL};
		f{memcpy}(v{se}->v{path}, v{path}, v{len});
		v{se}->p{magic} = m{source element};
		k{return} v{se};
	}
x{global elements}
```
* Die Größe der Struktur wird dynamisch berechnet
* So dass der Name mit abschließenden Null-Byte reinpasst

```
d{write HTML file}
	t{struct SourceElement *}v{cur};
	k{if} (v{argc} > n{1}) {
		v{cur} = f{createSourceElement}(v{argv}[n{1}]);
	} else {
		v{cur} = f{createSourceElement}(s{"index.x"});
	}
	t{struct SourceElement *}v{end} = v{cur};
	k{while} (v{cur}) {
		e{write cur HTML file};
		t{struct SourceElement *}v{next} =
			v{cur}->p{link};
		f{free}(v{cur});
		v{cur} = v{next};
	}
x{write HTML file}
```
* Beim Lesen der Datei können weitere Dateien in die Liste eingefügt
  werden

```
a{global elements}
	k{bool} f{hasSuffix}(
		t{const char *}v{str},
		t{const char *}v{suff}
	) {
		f{ASSERT}(v{str}); f{ASSERT}(v{suff});
		t{int} v{sl} = f{strlen}(v{str});
		t{int} v{su} = f{strlen}(v{suff});
		k{return} v{sl} >= v{su} && n{0} == f{memcmp}(
			v{str} + v{sl} - v{su}, v{suff}, v{su}
		);
	}
x{global elements}
```
* Um zu prüfen, ob Dateinamen die richtige Endung haben, gibt es eine
  extra Funktion

```
d{write cur HTML file}
	k{if} (f{hasSuffix}(v{cur}->v{path}, s{".x"})) {
		t{int} v{len} = f{strlen}(v{cur}->v{path}) + n{4};
		t{char *}v{outPath} = f{malloc}(v{len});
		f{ASSERT}(v{outPath});
		f{memcpy}(v{outPath}, v{cur}->v{path}, v{len} - n{6});
		f{strcpy}(v{outPath} + v{len} - n{6}, s{".html"});
		t{FILE *}v{out} = f{fopen}(v{outPath}, s{"w"});
		f{ASSERT}(v{out});
		E{write cur HTML file to out};
		f{fclose}(v{out});
		f{free}(v{outPath});
	}
x{write cur HTML file}
```
* Nur `.x`-Dateien werden in HTML konvertiert
* Die HTML hat den gleichen Pfad mit der Endung `.html`

```
a{write cur HTML file}
	k{if} (f{hasSuffix}(v{cur}->v{path}, s{".hx"})) {
		t{int} v{len} = f{strlen}(v{cur}->v{path}) + n{3};
		t{char *}v{outPath} = f{malloc}(v{len});
		f{ASSERT}(v{outPath});
		f{memcpy}(v{outPath}, v{cur}->v{path}, v{len} - n{6});
		f{strcpy}(v{outPath} + v{len} - n{6}, s{".html"});
		t{FILE *}v{out} = f{fopen}(v{outPath}, s{"w"});
		f{ASSERT}(v{out});
		E{write cur HTML file to out};
		f{fclose}(v{out});
		f{free}(v{outPath});
	}
x{write cur HTML file}
```

```
d{write cur HTML file to out} 
	t{FILE *}v{in} = f{fopen}(v{cur}->v{path}, s{"r"});
	f{ASSERT}(v{in});
	e{write HTML file from in to out};
	f{fclose}(v{in});
x{write HTML html file to out}
```

```
a{global elements}
	t{enum HtmlState} {
		v{hs_NOTHING_WRITTEN},
		v{hs_IN_SLIDE},
			v{hs_AFTER_SLIDE}
		e{html state enums}
	};
x{global elements}
```

```
a{global elements}
	t{struct HtmlStatus} {
		e{html state elements}
	};
x{global elements}
```

```
d{html state elements}
	t{enum HtmlState} v{state};
x{html state elements}
```

```
d{write HTML file from in to out} {
	t{struct HtmlStatus} v{status} = {
		.v{state} = v{hs_NOTHING_WRITTEN}
		e{init html status}
	};
	char last = '\n';
	k{for} (;;) {
		t{int} v{ch} = f{fgetc}(v{in});
		e{process ch for HTML};
		k{if} (v{ch} == k{EOF}) { k{break}; }
		v{last} = v{ch};
	}
} x{write HTML file from in to out}
```

```
d{html state enums}
	, v{hs_IN_HEADER}
x{html state enums}
```

```
a{global elements}
	t{bool} isOutOfHtmlSpecial(
		struct HtmlStatus *s
	) {
		e{check html special state};
		k{return} k{true};
	}
x{global elements}
```

```
d{check html special state}
	k{if} (v{s}->v{state} == v{hs_IN_HEADER}) {
		k{return} k{false};
	}
x{check html special state}
```

```
a{html state elements}
	t{int} v{headerLevel};
		t{char} v{headerName}[100];
		t{char *} v{headerNameEnd};
		t{enum HtmlState} v{headerState};
x{html state elements}
```

```
d{init html status}
	, .v{headerLevel} = 0
	, .v{headerNameEnd} = k{NULL}
x{init html status}
```

```
d{process ch for HTML} 
	k{if} (v{ch} == s{'#'} && v{last} == s{'\n'}) {
		k{if} (f{isOutOfHtmlSpecial}(&v{status}) ||
			v{status}.v{state} == v{hs_IN_HEADER}
		) {
			++v{status}.v{headerLevel};
			if (status.state != hs_IN_HEADER) {
				status.headerState = status.state;
			}
			v{status}.v{state} = v{hs_IN_HEADER};
			k{continue};
		}
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	k{if} (v{status}.v{state} == v{hs_IN_HEADER}) {
		k{if} (v{ch} == s{'\n'}) {
			e{process header in HTML};
			e{reset header state};
			v{last} = v{ch};
			k{continue};
		}
	}
x{process ch for HTML}
```

```
d{reset header state} 
	v{status}.v{state} = v{hs_IN_SLIDE};
	v{status}.v{headerLevel} = n{0};
	v{status}.v{headerNameEnd} = k{NULL};
	status.headerState = hs_IN_SLIDE;
x{reset header state}
```

```
a{process ch for HTML} 
	k{if} (v{status}.v{state} == v{hs_IN_HEADER}) {
		k{if} (v{status}.v{headerNameEnd}) {
			f{ASSERT}(
				v{status}.v{headerNameEnd} <
					v{status}.v{headerName} + f{sizeof}(
						v{status}.v{headerName}
					) - n{1}
			);
			*v{status}.v{headerNameEnd}++ = v{ch};
			v{last} = v{ch};
			k{continue};
		}
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	k{if} (v{status}.v{state} == v{hs_IN_HEADER}) {
		k{if} (! v{status}.v{headerNameEnd} &&
			v{ch} > s{' '}
		) {
			v{status}.v{headerNameEnd} =
				v{status}.v{headerName};
			*v{status}.v{headerNameEnd}++ = v{ch};
			v{last} = v{ch};
			k{continue};
		}
	}
x{process ch for HTML}
```

```
d{process header in HTML} 
	f{ASSERT}(v{status}.v{headerNameEnd});
	e{close previous HTML page};
	E{write header tag};
	f{fprintf}(v{out}, s{"<div class=\"slides\">\n"});
	f{fprintf}(v{out}, s{"<div><div>\n"});
	E{write header tag};
	f{fprintf}(v{out}, s{"</div>\n"});
x{process header in HTML}
```

```
a{global elements} 
	t{void} f{writeEscaped}(
		t{FILE *}v{out}, t{const char *}v{str}, const char *end
	) {
		f{ASSERT}(v{out}); f{ASSERT}(v{str});
		k{for} (; *v{str} && str != end; ++v{str}) k{switch} (*v{str}) {
			k{case} s{'<'}:
				f{fprintf}(v{out}, s{"&lt;"}); k{break};
			k{case} s{'>'}:
				f{fprintf}(v{out}, s{"&gt;"}); k{break};
			k{case} s{'&'}:
				f{fprintf}(v{out}, s{"&"}); k{break};
			k{default:}
				f{fputc}(*v{str}, v{out});
		}
	}
x{global elements}
```

```
d{write header tag} 
	f{fprintf}(v{out}, s{"<h%d>"}, v{status}.v{headerLevel});
	f{writeEscaped}(v{out}, v{status}.v{headerName}, v{status}.v{headerNameEnd});
	f{fprintf}(v{out}, s{"</h%d>\n"}, v{status}.v{headerLevel});
x{write header tag}
```

```
d{close previous HTML page} 
	switch (status.headerState) {
		case hs_NOTHING_WRITTEN: {
			e{write HTML header};
			break;
		}
		case hs_IN_SLIDE: {
			f{fprintf}(v{out}, s{"</div>\n"});
			f{fprintf}(v{out}, s{"</div>\n"});
			break;
		}
		default: {
			f{fprintf}(v{out}, s{"</div>\n"});
		}
	}
x{close previous HTML page}
```

```
d{write HTML header} 
	f{fprintf}(v{out}, s{"<!doctype html>\n"});
	f{fprintf}(v{out}, s{"<html lang=\"de\"l>\n"});
	f{fprintf}(v{out}, s{"<head>\n"});
	e{write HTML header entries};
	f{fprintf}(v{out}, s{"</head>\n"});
	f{fprintf}(v{out}, s{"<body>\n"});
x{write HTML header}
```

```
d{write HTML header entries} 
	f{fprintf}(
		v{out}, s{"<meta charset=\"utf-8\">\n"}
	);
	f{fprintf}(v{out}, s{"<title>"});
	f{writeEscaped}(v{out}, v{status}.v{headerName}, v{status}.v{headerNameEnd});
	f{fprintf}(v{out}, s{"</title>"});
	f{fprintf}(
		v{out}, s{"<link rel=\"stylesheet\" "}
		s{"type=\"text/css\" "}
		s{"href=\"slides/slides.css\">"}
	);
x{write HTML header entries}
```

```
a{html state enums}
	, v{hs_IN_CODE}
x{html state enums}
```

```
a{check html special state}
	k{if} (v{s}->v{state} == v{hs_IN_CODE}) {
		k{return} k{false};
	}
x{check html special state}
```

```
a{html state elements}
	t{int} v{codeOpening};
	t{int} v{codeIndent};
	t{char} v{codeSpecial};
	t{char} v{codeName}[100];
	t{char *}v{codeNameEnd};
x{html state elements}
```

```
a{init html status}
	, .v{codeOpening} = 0
	, .v{codeIndent} = 0
	, .v{codeSpecial} = s{'\0'}
	, .v{codeNameEnd} = k{NULL}
x{init html status}
```

```
a{process ch for HTML} 
	k{if} (v{last} == s{'\n'} && v{ch} == s{'`'}) {
		k{if} (f{isOutOfHtmlSpecial}(&v{status}) ||
			v{status}.v{state} == v{hs_IN_CODE}
		) {
			++v{status}.v{codeOpening};
			k{continue};
		}
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	k{if} (v{ch} == s{'\n'} && v{status}.v{codeOpening} == n{3}) {
		v{status}.v{codeOpening} = n{0};
		k{if} (f{isOutOfHtmlSpecial}(&v{status})) {
			k{if} (v{status}.v{state} == v{hs_IN_SLIDE}) {
				fprintf(out, "</div>\n");
			}
			fprintf(out, "<div><div>\n");
			fprintf(out, "<code>\n");
			v{status}.v{state} = v{hs_IN_CODE};
			v{last} = v{ch};
			k{continue};
		} k{else} k{if} (v{status}.v{state} == v{hs_IN_CODE}) {
			fprintf(out, "</code>\n");
			fprintf(out, "</div>\n");
			v{status}.v{state} = v{hs_IN_SLIDE};
			v{status}.v{codeIndent} = 0;
			v{status}.v{codeSpecial} = s{'\0'};
			v{last} = v{ch};
			k{continue};
		}
	}
	v{status}.v{codeOpening} = n{0};
x{process ch for HTML}
```

```
a{process ch for HTML}
	k{if} (status.state == hs_IN_CODE) {
		e{process ch in HTML code};
		if (last) {
			writeEscaped(out, &last, &last + 1);
		}
		last = ch;
		continue;
	}
x{process ch for HTML}
```

```
d{process ch in HTML code} 
	k{if} (v{ch} == s{'\n'}) {
		if (last) {
			writeEscaped(out, &last, &last + 1);
		}
		fprintf(out, "<br/>\n");
			last = ch;
		continue;
	}
x{process ch in HTML code}
```

```
a{process ch in HTML code} 
	k{if} (v{last} == s{'\n'} && v{ch} == s{'\t'}) {
		++v{status}.v{codeIndent};
		k{continue};
	}
	if (status.codeIndent) {
		fprintf(
			out, "<span class=\"in%d\"></span>", status.codeIndent
		);
		status.codeIndent = 0;
	}
x{process ch in HTML code}
```

```
a{process ch in HTML code}
	E{escape HTML code tag};
x{process ch in HTML code}
```


```
d{escape HTML code tag}
	if (ch == '{') {
		switch (last) {
			e{escape html macro}
			default: break;
		}
		if (status.codeSpecial) {
			last = 0;
			continue;
		}
	}
x{escape HTML code tag}
```

```
a{escape HTML code tag}
	if (ch == '}' && status.codeSpecial) {
		if (last) {
			writeEscaped(out, &last, &last + 1);
			last = 0;
		}
		switch (status.codeSpecial) {
			case 'i': {
				e{handle html include};
				break;
			}
			case 'a': case 'e': case 'E': case 'x':
			case 'r': case 'd': case 'p': case 'm': {
				fprintf(out, ")</span>");
			}
		}
		fprintf(out, "</span>");
		status.codeSpecial = 0;
		continue;
	}
x{escape HTML code tag}
```

```
d{handle html include}
	ASSERT(status.codeNameEnd <
		status.codeName + sizeof(status.codeName)
	);
	*status.codeNameEnd = '\0';
	while (status.codeNameEnd >= status.codeName && *status.codeNameEnd
		!= '.') {
		--status.codeNameEnd;
	}
	ASSERT(status.codeNameEnd >= status.codeName, "no period");
	*status.codeNameEnd = '\0';
	fprintf(out, "<a href=\"%s.html\">", status.codeName);
	*status.codeNameEnd = '.';
	fprintf(out, "%s</a>)</span>", status.codeName);
	end->p{link} = createSourceElement(status.codeName);
	end = end->p{link};
	status.codeNameEnd = NULL;

x{handle html include}
```

```
a{process ch in HTML code}
	if (ch != EOF && status.codeNameEnd) {
		ASSERT(status.codeNameEnd <
			status.codeName + sizeof(status.codeName)
		);
		*status.codeNameEnd++ = ch;
		continue;
	}
x{process ch in HTML code}
```

```
d{escape html macro}
	case 'd':
		fprintf(out, "<span class=\"add\">@def(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;
	case 'a':
		fprintf(out, "<span class=\"add\">@add(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;
	case 'r':
		fprintf(out, "<span class=\"add\">@replace(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;
x{escape html macro}
```

```
a{escape html macro}
	case 'x':
		fprintf(out, "<span class=\"end\">@end(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;
x{escape html macro}
```

```
a{escape html macro}
	case 'e':
		fprintf(out, "<span class=\"expand\">@expand(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;
	case 'E':
		fprintf(out, "<span class=\"expand\">@multiple(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		break;
x{escape html macro}
```

```
a{escape html macro}
	case 'i':
		fprintf(out, "<span class=\"include\">@include(");
		fprintf(out, "<span class=\"name\">");
		status.codeSpecial = last;
		status.codeNameEnd = status.codeName;
		break;
x{escape html macro}
```

```
a{escape html macro}
	case 't':
		fprintf(out, "<span class=\"type\">");
		status.codeSpecial = last;
		break;
x{escape html macro}
```

```
a{escape html macro}
	case 'v':
		fprintf(out, "<span class=\"var\">");
		status.codeSpecial = last;
		break;
x{escape html macro}
```

```
a{escape html macro}
	case 'f':
		fprintf(out, "<span class=\"fn\">");
		status.codeSpecial = last;
		break;
x{escape html macro}
```

```
a{escape html macro}
	case 'k':
		fprintf(out, "<span class=\"keyword\">");
		status.codeSpecial = last;
		break;
x{escape html macro}
```

```
a{escape html macro}
	case 's':
		fprintf(out, "<span class=\"str\">");
		status.codeSpecial = last;
		break;
x{escape html macro}
```

```
a{escape html macro}
	case 'n':
		fprintf(out, "<span class=\"num\">");
		status.codeSpecial = last;
		break;
x{escape html macro}
```

```
a{escape html macro}
	case 'p':
		fprintf(out, "<span class=\"type\">@priv(<span>");
		status.codeSpecial = last;
		break;
x{escape html macro}
```

```
a{escape html macro}
	case 'm':
		fprintf(out, "<span class=\"num\">@magic(<span>");
		status.codeSpecial = last;
		break;
x{escape html macro}
```

```
a{html state elements}
	t{bool} v{noteInCode};
	t{bool} v{noteInBold};
x{html state elements}
```

```
a{init html status}
	, .v{noteInCode} = k{false}
	, .v{noteInBold} = k{false}
x{init html status}
```

```
a{html state enums}
	, v{hs_IN_NOTES}
x{html state enums}
```

```
a{check html special state}
	k{if} (v{s}->v{state} == v{hs_IN_NOTES}) {
		k{return} k{false};
	}
x{check html special state}
```

```
a{process ch for HTML} 
	k{if} (v{last} == s{'\n'} && v{status}.v{state} == v{hs_IN_NOTES}) {
		k{if} (ch == s{'*'}) {
			f{fprintf}(v{out}, s{"</li><li>\n"});
			v{last} = 0;
			k{continue};
		} k{else} k{if} (ch != s{' '} && ch != s{'\t'}) {
			f{fprintf}(v{out}, s{"</li></ul></div>\n"});
			v{status}.v{state} = hs_AFTER_SLIDE;
			v{last} = v{ch};
			k{continue};
		}
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	k{if} (v{last} == s{'\n'} && v{ch} == s{'*'}) {
		k{if} (f{isOutOfHtmlSpecial}(&v{status})) {
			k{if}(v{status}.v{state} != v{hs_IN_SLIDE}) {
				f{fprintf}(v{out}, s{"<div>\n"});
			}
			v{status}.v{state} = v{hs_IN_NOTES};
			f{fprintf}(v{out}, s{"<ul><li>\n"});
			v{last} = s{'\0'};
			k{continue};
		}
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	k{if} (v{ch} == s{'`'} && v{status}.v{state} == v{hs_IN_NOTES}) {
		if (last) {
			writeEscaped(out, &last, &last + 1);
		}
		k{if} (v{status}.v{noteInCode}) {
			f{fprintf}(v{out}, s{"</code>"});
		} k{else} {
			f{fprintf}(v{out}, s{"<code>"});
		}
		v{status}.v{noteInCode} = ! v{status}.v{noteInCode};
		v{last} = 0;
		v{continue};
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	k{if} (v{status}.v{state} == v{hs_IN_NOTES} &&
	v{status}.v{noteInCode}) {
		E{escape HTML code tag};
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	k{if} (v{ch} == s{'*'} && v{last} == s{'*'} && v{status}.v{state} == v{hs_IN_NOTES}) {
		k{if} (v{status}.v{noteInBold}) {
			f{fprintf}(v{out}, s{"</b>"});
		} k{else} {
			f{fprintf}(v{out}, s{"<b>"});
		}
		v{status}.v{noteInBold} = ! v{status}.v{noteInBold};
		v{last} = 0;
		v{continue};
	}
x{process ch for HTML}
```

```
a{process ch for HTML} 
	k{if} (v{status}.v{state} == v{hs_IN_NOTES}) {
		k{if} (v{last}) {
			writeEscaped(out, &last, &last + 1);
		}
		v{last} = v{ch};
		v{continue};
	}
x{process ch for HTML}
```
