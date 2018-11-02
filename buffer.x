# Buffer
* Beschreibt ein Interface um mit Byte-Buffern umgehen zu können
* Die Länge dieser Buffer kann dynamisch wachsen

```
a{global elements}
	e{define buffer};
x{global elements}
```
* Buffer sind global sichtbare Strukturen

```
d{perform unit-tests}
	e{buffer unit tests};
x{perform unit-tests}
```
* Buffer haben ihre eigenen Unit Tests

# Struktur

```
d{define buffer}
	#define INIT_BUFFER_SIZE 16
	t{struct Buffer} {
		t{char} v{initial}t{[INIT_BUFFER_SIZE]};
		t{char *}v{buffer};
		t{char *}v{current};
		t{const char *}v{end};
	};
x{define buffer}
```
* Ein Buffer kann bis zu `INIT_BUFFER_SIZE` Bytes direkt aufnehmen
* Bei weiteren Bytes wird der Speicher im Heap angelegt
* `v{initial}` kann die ersten Bytes aufnehmen, ohne Speicher auf dem
  Heap anzufordern
* `v{buffer}` zeigt auf den Anfang des zu nutzenden Byte-Blocks
* `v{current}` zeigt auf das erste freie Zeichen im Byte-Block
* `v{current}` kann auch `k{NULL}` sein, um anzuzeigen, dass der Buffer
  momentan nicht aktiv ist
* `v{end}` zeigt auf das erste nicht mehr zum Buffer gehörende Byte

```
a{define buffer}
	t{static inline void} f{bufferInvariant}(
		t{const struct Buffer *}b
	) {
		f{ASSERT}(v{b});
		k{if} (v{b}->v{current}) {
			f{ASSERT}(v{b}->v{buffer} <= v{b}->v{current});
			f{ASSERT}(v{b}->v{current} <= v{b}->v{end});
		}
	}
x{define buffer}
```
* Die Zeiger im Buffer müssen immer geordnet sein

```
d{buffer unit tests} {
	t{struct Buffer} b = {};
	f{bufferInvariant}(&b);
} x{buffer unit tests}
```
* Diese Bedingung ist auch erfüllt, wenn alle Bytes im Buffer `0` sind
* Dies ist eine mögliche Initialisierung des Buffers

# Aktiver & Inaktiver Buffer
* Ein Buffer kann aktiv oder inaktiv sein
* Ein aktiver Buffer kann auch leer sein
* Als Unterscheidung dient, ob `v{current}` `k{NULL}` ist
* Ein in aktiver Buffer wird durch das Hinzufügen eines Zeichens aktiv
* Oder durch Aufruf von `f{activateBuffer}`
* Ein Buffer wird durch Zurücksetzen oder Löschen inaktiv

```
a{define buffer}
	t{static inline bool} f{isActiveBuffer}(
		t{const struct Buffer *}b
	) {
		bufferInvariant(b);
		return b->current;
	}
x{define buffer}
```
* Ein Buffer ist aktiv, wenn der Zeiger `v{current}` gesetzt ist

```
a{buffer unit tests} {
	t{struct Buffer} b = {};
	f{ASSERT}(! f{isActiveBuffer}(&b));
} x{buffer unit tests}
```
* Ein leerer Buffer ist nicht aktiv

# Zeichen hinzufügen

```
a{define buffer}
	t{void} f{activateBuffer}(t{struct Buffer} *b) {
		f{bufferInvariant}(v{b});
		k{if} (! v{b}->v{buffer}) {
			v{b}->v{buffer} = v{b}->v{initial};
			v{b}->v{end} = v{b}->v{initial} +
				f{sizeof}(v{b}->v{initial});
		}
		k{if} (! v{b}->v{current}) {
			v{b}->v{current} = v{b}->v{buffer};
		}
	}
x{define buffer}
```
* Wenn der interne Buffer nicht gesetzt ist, wird er auf den enthaltenen
  Bereich gesetzt
* Das stellt sicher, dass ein initialisierter Buffer keinen Speicher im
  Heap belegt
* Mit `f{eraseBuffer}` kann ggf. vorhandener Heap-Speicher freigegeben
  werden
* Und der Buffer trotzdem weiter benutzt werden

```
a{buffer unit tests} {
	t{struct Buffer} b = {};
	f{activateBuffer}(&b);
	f{ASSERT}(f{isActiveBuffer}(&b));
} x{buffer unit tests}
```
* Ein leerer Buffer kann aktiviert werden

```
a{define buffer}
	t{void} f{addToBuffer}(
		t{struct Buffer *}v{buffer}, t{char} ch
	) {
		f{bufferInvariant}(v{buffer});
		f{activateBuffer}(v{buffer});
		e{assure buffer size}
		*v{buffer}->v{current}++ = v{ch};
	}
x{define buffer}
```
* Wenn die Struktur leer ist, muss sie auf den interenen Buffer
  initialisiert werden
* Das ermöglicht die Initialisierung als Literal
* Zusätzlich muss sicher gestellt werden, dass ein weiteres Byte angefügt
  werden kann
* Dann wird das Byte angefügt

```
d{assure buffer size}
	k{if} (v{buffer}->v{current} >= v{buffer}->v{end}) {
		t{int} v{size} = v{buffer}->v{current} -
			v{buffer}->v{buffer};
		t{int} v{newSize} = n{2} * v{size};
		e{reallocate buffer};
	}
x{assure buffer size}
```
* Wenn der Buffer ausgeschöpft ist, dann wird seine Größe verdoppelt

```
d{reallocate buffer}
	t{char *}v{newBuffer};
	k{if} (v{buffer}->v{buffer} == v{buffer}->v{initial}) {
		v{newBuffer} = f{malloc}(v{newSize});
		e{copy initial buffer};
	} k{else} {
		v{newBuffer} = f{realloc}(
			v{buffer}->v{buffer}, v{newSize}
		);
	}
	e{adjust buffer pointers};
x{reallocate buffer}
```
* Wenn der initiale Buffer ausgeschöpft ist, muss neuer Speicher angelegt
  und manuell befüllt werden
* Andernfalls kann der Speicherblock im Heap vergrößert werden

```
d{copy initial buffer}
	f{ASSERT}(v{newBuffer});
	f{memcpy}(
		v{newBuffer}, v{buffer}->v{buffer}, v{size}
	);
x{copy initial buffer}
```
* Vom initialen Buffer werden die Werte direkt kopiert

```
d{adjust buffer pointers}
	f{ASSERT}(v{newBuffer});
	v{buffer}->v{buffer} = v{newBuffer};
	v{buffer}->v{current} = v{newBuffer} + v{size};
	v{buffer}->v{end} = v{newBuffer} + v{newSize};
x{adjust buffer pointers}
```
* Die Zeiger müssen an den neuen Speicher angepaßt werden

# Buffer zurücksetzen

```
a{define buffer}
	t{void} f{resetBuffer}(
		t{struct Buffer *}v{buffer}
	) {
		f{bufferInvariant}(v{buffer});
		v{buffer}->v{current} = k{NULL};
	}
x{define buffer}
```
* Um einen Buffer wieder zu verwenden, der `v{current}` muss auf
  `v{buffer}` zurück gesetzt werden
* Das funktioniert auch, wenn der initiale Buffer verwendet wird
* Oder die Zeiger noch `k{NULL}` sind
* Es wird kein Speicher freigegeben

```
a{define buffer}
	t{void} f{eraseBuffer}(
		t{struct Buffer *}v{buffer}
	) {
		f{bufferInvariant}(v{buffer});
		e{erase heap buffer};
		v{buffer}->v{buffer} = k{NULL};
		v{buffer}->v{current} = k{NULL};
	}
x{define buffer}
```
* Wenn ein Buffer auf dem Heap angelegt wurde, wird dieser freigegeben
* Die Zeiger werden auf `k{NULL}` gesetzt
* Nur `v{end}` muss nicht aktualisiert werden, da es bei der
  Initialisierung gesetzt wird

```
d{erase heap buffer}
	k{if} (v{buffer}->v{buffer} &&
		v{buffer}->v{buffer} != v{buffer}->v{initial}
	) {
		f{free}(v{buffer}->v{buffer});
	}
x{erase heap buffer}
```
* Wenn wir nicht den internen Buffer verwenden, wird der Buffer
  freigegeben

# Unit Tests

```
a{buffer unit tests} {
	t{struct Buffer} v{b} = {};
	f{addToBuffer}(&v{b}, s{'x'});
	f{ASSERT}(*v{b}.v{buffer} == s{'x'});
	f{ASSERT}(v{b}.v{buffer} + 1 == v{b}.v{current});
	f{ASSERT}(v{b}.v{buffer} == v{b}.v{initial});
} x{buffer unit tests}
```
* Die Struktur wird mit dem Hinzufügen initialisiert
* Danach ist ein Byte belegt
* Und nur der initiale Buffer wird verwendet

```
a{define buffer}
	t{void} f{addCharsToBuffer}(
		t{struct Buffer *}v{buffer},
		t{char} v{ch}, t{int} v{count}
	) {
		f{bufferInvariant}(v{buffer});
		f{ASSERT}(v{count} >= 0);
		k{for} (; v{count}; --v{count}) {
			f{addToBuffer}(v{buffer}, v{ch});
		}
	}
x{define buffer}
```
* Fügt mehrere Kopien eines Zeichens in den Buffer ein

```
a{buffer unit tests} {
	t{struct Buffer} v{b} = {};
	f{addCharsToBuffer}(&v{b}, s{'x'}, 1000);
	f{ASSERT}(f{isActiveBuffer}(&v{b}));
	f{ASSERT}(*v{b}.v{buffer} == s{'x'});
	f{ASSERT}(v{b}.v{buffer} + 1000 == v{b}.v{current});
	f{ASSERT}(v{b}.v{buffer} != v{b}.v{initial});
	f{eraseBuffer}(&v{b});
	f{ASSERT}(! f{isActiveBuffer}(&v{b}));
} x{buffer unit tests}
```
* In diesem Test werden 1.000 Zeichen eingefügt
* Dafür reicht der initiale Buffer nicht aus
* Aber der zusätzliche Speicher wird nach dem Löschen freigegeben
