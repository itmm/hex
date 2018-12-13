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
			e{functions};
	};
x{define hash}
```
* Der Hash wird Zeichen für Zeichen berechnet und im Attribut `_hash`
  abgelegt

```
d{functions}
	Hash(): _hash(0x3d9a73b5) {}
x{functions}
```
* Um deterministisch zu sein, werden Hashes mit einer festen Zahl
  initialisiert
* Diese Zahl wurde zufällig gewählt

```
a{functions}
	int hash() const {
		return static_cast<int>(
			_hash & 0x7fffffff
		);
	}
x{functions}
```
* Nach außen wird das oberste Bit maskiert
* So kann es als `int` verwendet werden
* Wird aber nie negativ

```
a{functions}
	unsigned add(const std::string &s);
x{functions}
```
* Die `f{add}`-Methode aktualisiert den Hash mit den Zeichen von
  `s`
* Diese Methode ist nicht `inline` implementiert

# Hash berechnen
* Beschreibt die `f{add}`-Methode

```
a{define hash}
	unsigned Hash::add(
		const std::string &s
	) {
		for (auto &ch : s) {
			e{hash next ch};
		}
		return hash();
	}
x{define hash}
```
* Die `f{add}`-Methode iteriert über jedes Zeichen
* Und fügt es dem Hash hinzu
* Das maskierte Ergebnis wird zurück geliefert

```
d{hash next ch}
	_hash ^= ch;
x{hash next ch}
```
* Zuerst wird das aktuelle Zeichen per XOR mit dem aktuellen Hash
  verknüpft

```
a{hash next ch}
	_hash = (_hash << 3) | (_hash >> 29);
x{hash next ch}
```
* Danach wird der Hash um 3 Bit rotiert
* Und das war es schon

```
a{define hash}
	int calcHash(
		const char *begin,
		const char *end
	) {
		Hash h;
		return h.add(
			std::string(begin, end)
		);
	}
x{define hash}
```
* Berechnet den Hash über einen festen Zeichenbereich
* Sobald `frag.x` nicht mehr selber hasht, kann diese Funktion raus

