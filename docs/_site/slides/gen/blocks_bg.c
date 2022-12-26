
	
	#include <stdio.h>
	#include <stdlib.h>
	#include <time.h>
	#include <math.h>

	
	static const int cols = 10;
	static const int rows = 8;

	static const int width = 10;
	static const int gap = 2;
	static const int step = 12;

	
	static inline void toHex(
		char *dst, int v
	) {
		static const char digits[] =
			"0123456789abcdef";
		dst[0] = digits[v >> 4];
		dst[1] = digits[v & 0xf];
	}

	static inline void buildColor(
		char *dst,
		int r, int g, int b
	) {
		toHex(dst + 1, r);
		toHex(dst + 3, g);
		toHex(dst + 5, b);
	}

	int main() {
		
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
	
	for (int j = 0; j < rows; ++j) {
		int dy = j;
		int dyq = dy * dy;
		double maxDist = sqrt(
			cols * cols + rows * rows
		);
		for (int i = 0; i < cols; ++i) {
			
	int dx = (cols - 1 - i);
	int dxq = dx * dx;
	double dist =
		sqrt(dxq + dyq) / maxDist;
	
	double r = rand() * 1.0 / RAND_MAX;
	if (r < dist) {
		char fill[8] = "#123456";
		
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
;
		printf(
			"\t<rect x=\"%d\" y=\"%d\" "
			"width=\"%d\" height=\"%d\" "
			"fill=\"%s\"></rect>\n",
			i * step, j * step, width,
			width, fill
		);
	}
;
;
		}
	}
;
	puts("</svg>");

	}
