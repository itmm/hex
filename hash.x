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
	static inline unsigned initHash() {
		return 0x3d9a73b5;
	}
x{define hash}
```
* Um deterministisch zu sein, werden Hashes mit einer festen Zahl
  initialisiert
* Diese Zahl wurde zufällig gewählt

```
a{define hash}
	unsigned addRangeToHash(
		unsigned hash,
		const char *begin,
		const char *end
	) {
		ASSERT(begin);
		for (; begin < end; ++begin) {
			E{hash next ch};
		}
		return hash;
	}
x{define hash}
```

```
a{define hash}
	unsigned addTerminatedToHash(
		unsigned hash,
		const char *begin
	) {
		ASSERT(begin);
		for (; *begin; ++begin) {
			E{hash next ch};
		}
		return hash;
	}
x{define hash}
```

```
d{hash next ch}
	hash ^= *begin;
	hash = (hash << 3) | (hash >> 29);
x{hash next ch}
```

```
a{define hash}
	int calcHash(
		const char *begin,
		const char *end
	) {
		unsigned hash = initHash();
		hash = addRangeToHash(
			hash, begin, end
		);
		return hash & 0x7ffffff;
	}
x{define hash}
```
* Der Hash wird über den Namen des Fragments erstellt
* Der Name kann entweder durch ein Null-Byte abgeschlossen werden
* Oder ein Ende wird direkt angegeben
* Das Resultat ist eine positive 32-Bit Zahl
