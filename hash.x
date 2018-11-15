# Schnelle Hash-Funktion
* Nicht super sicher
* Nicht super toll
* Aber schnell

```
A{global elements}
	e{define hash};
x{global elements}
```
* Hash-Berechnung ist eine global sichtbare Funktion

```
d{define hash}
	k{static inline} t{unsigned} f{initHash}() {
		k{return} n{0x3d9a73b5};
	}
x{define hash}
```
* Um deterministisch zu sein, werden Hashes mit einer festen Zahl
  initialisiert
* Diese Zahl wurde zufällig gewählt

```
a{define hash}
	t{unsigned} f{addRangeToHash}(
		t{unsigned} v{hash},
		t{const char *}v{begin},
		t{const char *}v{end}
	) {
		f{ASSERT}(v{begin});
		k{for} (; v{begin} < v{end}; ++v{begin}) {
			E{hash next ch};
		}
		k{return} v{hash};
	}
x{define hash}
```

```
a{define hash}
	t{unsigned} f{addTerminatedToHash}(
		t{unsigned} v{hash},
		t{const char *}v{begin}
	) {
		f{ASSERT}(v{begin});
		k{for} (; *v{begin}; ++v{begin}) {
			E{hash next ch};
		}
		k{return} v{hash};
	}
x{define hash}
```

```
d{hash next ch}
	v{hash} ^= *v{begin};
	v{hash} = (v{hash} << 3) | (v{hash} >> 29);
x{hash next ch}
```

```
a{define hash}
	t{int} f{calcHash}(
		t{const char *}v{begin},
		t{const char *}v{end}
	) {
		t{unsigned} v{hash} = f{initHash}();
		v{hash} = f{addRangeToHash}(
			v{hash}, v{begin}, v{end}
		);
		k{return} v{hash} & n{0x7ffffff};
	}
x{define hash}
```
* Der Hash wird über den Namen des Fragments erstellt
* Der Name kann entweder durch ein Null-Byte abgeschlossen werden
* Oder ein Ende wird direkt angegeben
* Das Resultat ist eine positive 32-Bit Zahl
