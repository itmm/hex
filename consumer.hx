# Ausgaben konsumieren
* Diese Datei beschreibt eine generische Funktion um zeichenweise
  Ausgaben zu konsumieren
* Damit sind leichtere Tests möglich, da nicht so viele temporären
  Dateien angelegt werden müssen

```
a{global elements}
	e{expand consumer}
x{global elements}
```
* Diese Datei stellt globale Funktionen bereit um ein abstraktes
  Interface zur Verfügung zu stellen


```
a{expand consumer}
	t{struct Consumer} {
		t{int} (* f{put})(
			t{struct Consumer *}v{consumer}, t{int} v{ch}
		);
	};
x{expand consumer}
```
* Ein Consumer besteht nur aus einer Funktion, die ein Zeichen
  entgegennimmt
* Der Consumer ist trotzdem als <code>struct</code> ausgeführt, damit
  nicht immer ein Paar aus Funktionszeiger und Kontext durchgereicht
  werden muss
* Das Ende wird signalisiert durch das Senden von <code>EOF</code>

```
a{expand consumer}
	t{void} f{putToConsumer}(
		t{struct Consumer *}v{c}, t{int} v{ch}
	) {
		f{ASSERT}(v{c}); f{ASSERT}(v{c}->v{put});
		v{c}->v{put}(v{c}, v{ch});
	}
x{expand consumer}
```
* Diese Funktion vereinfacht den Aufruf des Consumers

# Beispiel: FileConsumer
* Ein einfacher Consumer schreibt die Daten in eine Datei

```
a{expand consumer}
	t{struct FileConsumer} {
		t{struct Consumer} v{consumer};
		t{FILE *} v{file};
	};
x{expand consumer}
```
* Neben der Consumer-Struktur muss die Datei bekannt sein

```
a{expand consumer}
	t{int} f{consumeInFile}(
		t{struct Consumer *}v{c}, t{int} v{ch}
	) {
		t{struct FileConsumer *}v{fc} = (t{void *}) v{c};
		f{ASSERT}(v{fc}); f{ASSERT}(v{fc}->v{file});
		k{if} (v{ch} != k{EOF}) {
			v{ch} = f{fputc}(v{ch}, v{fc}->v{file});
		} k{else} {
			f{fclose}(v{fc}->v{file});
			v{fc}->v{file} = k{NULL};
		}
		k{return} v{ch};
	}
x{expand consumer}
```
* Der Consumer wird auf den richtigen Typ gecastet
* Das setzt vorraus, dass er korrekt initialisiert wurde
* Wenn nicht das Ende erreicht ist, wird das Zeichen in die Datei
  geschrieben
* Andernfalls wird die Datei geschlossen

```
a{expand consumer}
	t{void} f{setupFileConsumer}(
		t{struct FileConsumer *}v{fc}, t{FILE *}v{f}
	) {
		f{ASSERT}(v{fc}); f{ASSERT}(v{f});
		v{fc}->v{file} = v{f};
		v{fc}->v{consumer}.v{put} = f{consumeInFile};
	}
x{expand consumer}
```
* Die Struktur wird mit einer offenen Datei initialisiert

# Beispiel: BufferConsumer
* Dieser Consumer speichert in einen Buffer
* Er ist praktisch zum Unit-Testen

```
a{expand consumer}
	t{struct BufferConsumer} {
		t{struct Consumer} v{consumer};
		t{struct Buffer} v{buffer};
	};
x{expand consumer}
```
* Neben der Consumer-Struktur muss Platz für den Buffer vorhanden sein

```
a{expand consumer}
	t{int} f{consumeInBuffer}(
		t{struct Consumer *}v{c}, t{int} v{ch}
	) {
		t{struct BufferConsumer *}v{bc} = (t{void *}) v{c};
		f{ASSERT}(v{bc});
		t{int} v{cr} = v{ch} != k{EOF} ? v{ch} : s{'\0'};
		f{addToBuffer}(&v{bc}->v{buffer}, v{cr});
		k{return} v{ch};
	}
x{expand consumer}
```
* Der Consumer wird auf den richtigen Typ gecastet
* Das setzt vorraus, dass er korrekt initialisiert wurde
* Wenn nicht das Ende erreicht ist, wird das Zeichen in den Buffer
  geschrieben
* Wenn das Ende erreicht ist, wird ein Nullbyte geschrieben, um den
  String zu terminieren

```
a{expand consumer}
	t{void} f{setupBufferConsumer}(
		t{struct BufferConsumer *}v{bc}
	) {
		f{ASSERT}(v{bc});
		f{memset}(
			&v{bc}->v{buffer}, 0,
			f{sizeof}(v{bc}->v{buffer})
		);
		v{bc}->v{consumer}.v{put} = f{consumeInBuffer};
	}
x{expand consumer}
```
* Die Struktur wird mit einem leeren Buffer initialisiert

# Unit Tests
* Testet den BufferConsumer als proof of concept

```
a{perform unit-tests} {
	e{consumer unit-tests}
} x{perform unit-tests}
```
* Die Unit-Tests werden in einem eigenen Makro gesammelt

```
a{consumer unit-tests} {
	t{struct BufferConsumer} v{bc};
	f{setupBufferConsumer}(&v{bc});
	t{struct Consumer *}v{c} = &v{bc}.v{consumer};
	f{putToConsumer}(v{c}, s{'a'});
	f{putToConsumer}(v{c}, s{'b'});
	f{putToConsumer}(v{c}, k{EOF});
	f{ASSERT}(
		f{strcmp}(s{"ab"}, v{bc}.v{buffer}.v{buffer}) == 0
	);
} x{consumer unit-tests}
```
* Zwei Zeichen werden in den Buffer eingefügt
