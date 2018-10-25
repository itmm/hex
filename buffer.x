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

# Zeichen hinzufügen

```
a{define buffer}
	t{void} f{addToBuffer}(
		t{struct Buffer *}v{buffer}, t{char} ch
	) {
		f{ASSERT}(v{buffer});
		e{may initialize buffer}
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
d{may initialize buffer}
	k{if} (! v{buffer}->v{buffer}) {
		v{buffer}->v{buffer} =
			v{buffer}->v{initial};
		v{buffer}->v{current} =
			v{buffer}->v{buffer};
		v{buffer}->v{end} =
			v{buffer}->v{initial} +
				INIT_BUFFER_SIZE;
	}
x{may initialize buffer}
```
* Wenn der interne Buffer nicht gesetzt ist, wird er auf den enthaltenen
  Bereich gesetzt
* Das stellt sicher, dass ein initialisierter Buffer keinen Speicher im
  Heap belegt
* Mit `f{eraseBuffer}` kann ggf. vorhandener Heap-Speicher freigegeben
  werden
* Und der Buffer trotzdem weiter benutzt werden

```
d{assure buffer size}
	k{if} (
		v{buffer}->v{current} >= v{buffer}->v{end}
	) {
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
	k{if} (
		v{buffer}->v{buffer} == v{buffer}->v{initial}
	) {
		v{newBuffer} = f{malloc}(v{newSize});
		e{copy initial buffer};
	} k{else} {
		v{newBuffer} = f{realloc}(
			v{buffer}->v{buffer}, v{newSize});
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
		f{ASSERT}(v{buffer});
		v{buffer}->v{current} = v{buffer}->v{buffer};
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
		f{ASSERT}(v{buffer});
		e{erase heap buffer};
		v{buffer}->v{current} = v{buffer}->v{buffer};
	}
x{define buffer}
```
* Wenn ein Buffer auf dem Heap angelegt wurde, wird dieser freigegeben
* Zusätzlich wird der aktuelle Zeiger auf den Anfang zurückgesetzt

```
d{erase heap buffer}
	k{if} (v{buffer}->v{buffer} &&
		v{buffer}->v{buffer} != v{buffer}->v{initial}
	) {
		f{free}(v{buffer}->v{buffer});
		v{buffer}->v{buffer} = v{buffer}->v{initial};
	}
x{erase heap buffer}
```
* Wenn wir nicht den internen Buffer verwenden, wird der Buffer
  freigegeben
* Und der Buffer-Zeiger wird gelöscht

# Unit Tests

```
d{buffer unit tests} {
	t{struct Buffer} v{buffer} = {};
	f{addToBuffer}(&v{buffer}, s{'x'});
	f{ASSERT}(*v{buffer}.v{buffer} == s{'x'});
	f{ASSERT}(v{buffer}.v{buffer} + 1 ==
		v{buffer}.v{current});
	f{ASSERT}(v{buffer}.v{buffer} ==
		v{buffer}.v{initial});
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
		f{ASSERT}(v{buffer});
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
	t{struct Buffer} v{buffer} = {};
	f{addCharsToBuffer}(&v{buffer}, s{'x'},
		1000);
	f{ASSERT}(*v{buffer}.v{buffer} == s{'x'});
	f{ASSERT}(v{buffer}.v{buffer} + 1000 ==
		v{buffer}.v{current});
	f{ASSERT}(v{buffer}.v{buffer} !=
		v{buffer}.v{initial});
	f{eraseBuffer}(&v{buffer});
	f{ASSERT}(v{buffer}.v{buffer} ==
		v{buffer}.v{initial});
} x{buffer unit tests}
```
* In diesem Test werden 1.000 Zeichen eingefügt
* Dafür reicht der initiale Buffer nicht aus
* Aber der zusätzliche Speicher wird nach dem Löschen freigegeben
