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
	class Hash {
		private:
			unsigned _hash;
		public:
			Hash(): _hash(0x3d9a73b5) {}
			unsigned hash() const { return _hash & 0x7fffffff; }
			unsigned add(const std::string &s);
	};
x{define hash}
```
* Um deterministisch zu sein, werden Hashes mit einer festen Zahl
  initialisiert
* Diese Zahl wurde zufällig gewählt

```
a{define hash}
	unsigned Hash::add(const std::string &s) {
		for (auto i = s.begin(); i != s.end(); ++i) {
			e{hash next ch};
		}
		return hash();
	}
x{define hash}
```

```
d{hash next ch}
	_hash ^= *i;
	_hash = (_hash << 3) | (_hash >> 29);
x{hash next ch}
```

```
a{define hash}
	int calcHash(
		const char *begin,
		const char *end
	) {
		Hash h;
		return h.add(std::string(begin, end));
	}
x{define hash}
```
* Der Hash wird über den Namen des Fragments erstellt
* Der Name kann entweder durch ein Null-Byte abgeschlossen werden
* Oder ein Ende wird direkt angegeben
* Das Resultat ist eine positive 32-Bit Zahl
