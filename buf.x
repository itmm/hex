# Buffer
* Beschreibt ein Interface um mit Byte-Buffern umgehen zu können
* Die Länge dieser Buffer kann dynamisch wachsen

```
A{global elements}
	e{define buffer};
x{global elements}
```
* Buffer sind global sichtbare Strukturen

```
D{perform unit-tests}
	e{buffer unit tests};
x{perform unit-tests}
```
* Buffer haben ihre eigenen Unit Tests

# Struktur

```
d{define buffer}
	#define INIT_BUFFER_SIZE 16
	t{struct Buffer} {
		t{char} initial[INIT_BUFFER_SIZE];
		t{char *}buffer;
		t{char *}current;
		t{const char *}end;
	};
x{define buffer}
```
* Ein Buffer kann bis zu `INIT_BUFFER_SIZE` Bytes direkt aufnehmen
* Bei weiteren Bytes wird der Speicher im Heap angelegt
* `initial` kann die ersten Bytes aufnehmen, ohne Speicher auf dem
  Heap anzufordern
* `buffer` zeigt auf den Anfang des zu nutzenden Byte-Blocks
* `current` zeigt auf das erste freie Zeichen im Byte-Block
* `current` kann auch `n{NULL}` sein, um anzuzeigen, dass der Buffer
  momentan nicht aktiv ist
* `end` zeigt auf das erste nicht mehr zum Buffer gehörende Byte

```
a{define buffer}
	t{static inline void} bufferInvariant(
		t{const struct Buffer *}b
	) {
		ASSERT(b);
		if (b->current) {
			ASSERT(b->buffer <= b->current);
			ASSERT(b->current <= b->end);
		}
	}
x{define buffer}
```
* Die Zeiger im Buffer müssen immer geordnet sein

```
d{buffer unit tests} {
	t{struct Buffer} b = {};
	bufferInvariant(&b);
} x{buffer unit tests}
```
* Diese Bedingung ist auch erfüllt, wenn alle Bytes im Buffer `0` sind
* Dies ist eine mögliche Initialisierung des Buffers

# Aktiver & Inaktiver Buffer
* Ein Buffer kann aktiv oder inaktiv sein
* Ein aktiver Buffer kann auch leer sein
* Als Unterscheidung dient, ob `current` `n{NULL}` ist
* Ein in aktiver Buffer wird durch das Hinzufügen eines Zeichens aktiv
* Oder durch Aufruf von `activateBuffer`
* Ein Buffer wird durch Zurücksetzen oder Löschen inaktiv

```
a{define buffer}
	t{static inline bool} isActiveBuffer(
		t{const struct Buffer *}b
	) {
		bufferInvariant(b);
		return b->current;
	}
x{define buffer}
```
* Ein Buffer ist aktiv, wenn der Zeiger `current` gesetzt ist

```
a{buffer unit tests} {
	t{struct Buffer} b = {};
	ASSERT(! isActiveBuffer(&b));
} x{buffer unit tests}
```
* Ein leerer Buffer ist nicht aktiv

# Zeichen hinzufügen

```
a{define buffer}
	t{void} activateBuffer(t{struct Buffer} *b) {
		bufferInvariant(b);
		if (! b->buffer) {
			b->buffer = b->initial;
			b->end = b->initial +
				sizeof(b->initial);
		}
		if (! b->current) {
			b->current = b->buffer;
		}
	}
x{define buffer}
```
* Wenn der interne Buffer nicht gesetzt ist, wird er auf den enthaltenen
  Bereich gesetzt
* Das stellt sicher, dass ein initialisierter Buffer keinen Speicher im
  Heap belegt
* Mit `eraseBuffer` kann ggf. vorhandener Heap-Speicher freigegeben
  werden
* Und der Buffer trotzdem weiter benutzt werden

```
a{buffer unit tests} {
	t{struct Buffer} b = {};
	activateBuffer(&b);
	ASSERT(isActiveBuffer(&b));
} x{buffer unit tests}
```
* Ein leerer Buffer kann aktiviert werden

```
a{define buffer}
	t{void} addToBuffer(
		t{struct Buffer *}buffer, t{char} ch
	) {
		bufferInvariant(buffer);
		activateBuffer(buffer);
		e{assure buffer size}
		*buffer->current++ = ch;
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
	if (buffer->current >= buffer->end) {
		t{int} size = buffer->current -
			buffer->buffer;
		t{int} newSize = n{2} * size;
		e{reallocate buffer};
	}
x{assure buffer size}
```
* Wenn der Buffer ausgeschöpft ist, dann wird seine Größe verdoppelt

```
d{reallocate buffer}
	t{char *}newBuffer;
	if (buffer->buffer == buffer->initial) {
		newBuffer = malloc(newSize);
		e{copy initial buffer};
	} else {
		newBuffer = realloc(
			buffer->buffer, newSize
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
	ASSERT(newBuffer);
	memcpy(
		newBuffer, buffer->buffer, size
	);
x{copy initial buffer}
```
* Vom initialen Buffer werden die Werte direkt kopiert

```
d{adjust buffer pointers}
	ASSERT(newBuffer);
	buffer->buffer = newBuffer;
	buffer->current = newBuffer + size;
	buffer->end = newBuffer + newSize;
x{adjust buffer pointers}
```
* Die Zeiger müssen an den neuen Speicher angepaßt werden

# Buffer zurücksetzen

```
a{define buffer}
	t{void} resetBuffer(
		t{struct Buffer *}buffer
	) {
		bufferInvariant(buffer);
		buffer->current = n{NULL};
	}
x{define buffer}
```
* Um einen Buffer wieder zu verwenden, der `current` muss auf
  `buffer` zurück gesetzt werden
* Das funktioniert auch, wenn der initiale Buffer verwendet wird
* Oder die Zeiger noch `n{NULL}` sind
* Es wird kein Speicher freigegeben

```
a{define buffer}
	t{void} eraseBuffer(
		t{struct Buffer *}buffer
	) {
		bufferInvariant(buffer);
		e{erase heap buffer};
		buffer->buffer = n{NULL};
		buffer->current = n{NULL};
	}
x{define buffer}
```
* Wenn ein Buffer auf dem Heap angelegt wurde, wird dieser freigegeben
* Die Zeiger werden auf `n{NULL}` gesetzt
* Nur `end` muss nicht aktualisiert werden, da es bei der
  Initialisierung gesetzt wird

```
d{erase heap buffer}
	if (buffer->buffer &&
		buffer->buffer != buffer->initial
	) {
		free(buffer->buffer);
	}
x{erase heap buffer}
```
* Wenn wir nicht den internen Buffer verwenden, wird der Buffer
  freigegeben

# Unit Tests

```
a{buffer unit tests} {
	t{struct Buffer} b = {};
	addToBuffer(&b, s{'x'});
	ASSERT(*b.buffer == s{'x'});
	ASSERT(b.buffer + 1 == b.current);
	ASSERT(b.buffer == b.initial);
} x{buffer unit tests}
```
* Die Struktur wird mit dem Hinzufügen initialisiert
* Danach ist ein Byte belegt
* Und nur der initiale Buffer wird verwendet

```
a{define buffer}
	t{void} addCharsToBuffer(
		t{struct Buffer *}buffer,
		t{char} ch, t{int} count
	) {
		bufferInvariant(buffer);
		ASSERT(count >= 0);
		for (; count; --count) {
			addToBuffer(buffer, ch);
		}
	}
x{define buffer}
```
* Fügt mehrere Kopien eines Zeichens in den Buffer ein

```
a{buffer unit tests} {
	t{struct Buffer} b = {};
	addCharsToBuffer(&b, s{'x'}, 1000);
	ASSERT(isActiveBuffer(&b));
	ASSERT(*b.buffer == s{'x'});
	ASSERT(b.buffer + 1000 == b.current);
	ASSERT(b.buffer != b.initial);
	eraseBuffer(&b);
	ASSERT(! isActiveBuffer(&b));
} x{buffer unit tests}
```
* In diesem Test werden 1.000 Zeichen eingefügt
* Dafür reicht der initiale Buffer nicht aus
* Aber der zusätzliche Speicher wird nach dem Löschen freigegeben
