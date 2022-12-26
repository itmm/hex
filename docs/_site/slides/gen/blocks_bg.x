# Zufällige Rechtecke
* Generiert eine Hintergrund-Grafik mit zufälligen Rechtecken

```
@def(file: blocks_bg.c)
	@put(includes)
	@put(globals)
	@put(functions)
	int main() {
		@put(main)
	}
@end(file: blocks_bg.c)
```
* Das C-Programm besteht aus mehreren Blöcken

```
@def(includes)
	#include <stdio.h>
	#include <stdlib.h>
	#include <time.h>
	#include <math.h>
@end(includes)
```
* `@s(<time.h>)` wird für Zufalls-Initialisierung benötigt
* `@s(<math.h>)` wird für die Abstandsberechnung benötigt (Wurzel)

```
@def(globals)
	static const int cols = 10;
	static const int rows = 8;
@end(globals)
```
* Anzahl der Rechtecke

```
@add(globals)
	static const int width = 10;
	static const int gap = 2;
	static const int step = 12;
@end(globals)
```
* Kantenlänge, Abstand und Offset

```
@def(functions)
	static inline void toHex(
		char *dst, int v
	) {
		static const char digits[] =
			"0123456789abcdef";
		dst[0] = digits[v >> 4];
		dst[1] = digits[v & 0xf];
	}
@end(functions)
```
* Generiert zweistellige Hex-Zahl aus einem Byte

```
@add(functions)
	static inline void buildColor(
		char *dst,
		int r, int g, int b
	) {
		toHex(dst + 1, r);
		toHex(dst + 3, g);
		toHex(dst + 5, b);
	}
@end(functions)
```
* Generiert Farbe in hexadezimaler Schreibweise
* Komponenten sind rot, grün und blau

```
@def(main)
	srand(time(NULL));
	printf(
		"<svg version=\"1.1\" "
		"baseProfile=\"full\" "
		"width=\"%d\" height=\"%d\" "
		"xmlns=\"http://www.w3.org/"
			"2000/svg\">\n",
		cols * step - gap,
		rows * step - gap
	);
	@put(generate cells);
	puts("</svg>");
@end(main)
```
* Schreibt SVG-Rahmen
* Und generiert einzelne Zellen

```
@def(generate cells)
	for (int j = 0; j < rows; ++j) {
		int dy = j;
		int dyq = dy * dy;
		double maxDist = sqrt(
			cols * cols + rows * rows
		);
		for (int i = 0; i < cols; ++i) {
			@put(cells in row);
		}
	}
@end(generate cells)
```
* Die Zellen werden zeilenweise generiert

```
@def(cells in row)
	int dx = (cols - 1 - i);
	int dxq = dx * dx;
	double dist =
		sqrt(dxq + dyq) / maxDist;
	@put(generate cell);
@end(cells in row)
```
* Für jede Zelle wird relativer Abstand zur rechten oberen Ecke bestimmt
* Und die Zelle generiert

```
@def(generate cell)
	double r = rand() * 1.0 / RAND_MAX;
	if (r < dist) {
		char fill[8] = "#123456";
		@put(set fill color);
		printf(
			"\t<rect x=\"%d\" y=\"%d\" "
			"width=\"%d\" height=\"%d\" "
			"fill=\"%s\"></rect>\n",
			i * step, j * step, width,
			width, fill
		);
	}
@end(generate cell)
```
* Es wird per Zufall entschieden, ob das Rechteck gezeichnet werden soll
* In der rechten oberen Ecke werden weniger Rechtecke gezeichnet

```
@def(set fill color)
	int d = rand() % 0x08;
	if (rand() % 10 == 0) {
		buildColor(
			fill, 0xee, 0xee, 0xff - d
		);
	} else {
		buildColor(
			fill, 0xee, 0xff - d, 0xee
		);
	}
@end(set fill color)
```
* Das Rechteck wird entweder grünlich oder bläulich gezeichnet
* Der Farb-Code wird um eine zufällige Komponente verschoben

