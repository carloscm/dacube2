
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "oldskool.h"

#include "effects.h"
#include "drawchar.h"
#include "struct.h"
#include "3dengine.h"

char *FontBuffer;

int oldskool = 0;

static void set_default_palette() {
	int i;
	for (i = 0; i < 251; ++i) {
		os_fb_set_palette(i, i >> 2, i >> 2, i >> 2);
	}
}

void dacube2_exit(int val) {
	os_ticks_end();
	os_mod_end();
	os_fb_end();

	free(FontBuffer);

	exit(val);
}

int main(int argc, char** argv) {
	char *Sbuf;

	inittable();

	oldskool = os_fb_init(argc, argv);
	if (oldskool == -1) {
		dacube2_exit(1);
	}

	Sbuf = os_fb_get_buffer();

	FontBuffer=(char *) malloc(440*15);
	DeCrunchFont(FontBuffer);

	if (os_mod_start(argc, argv) == -1) {
		dacube2_exit(1);
	}
	os_ticks_init(70);
	set_default_palette();

	Cubes6(Sbuf);
	title2(Sbuf);
	set_default_palette();
	Cubes7(Sbuf,1);
	Cubes8(Sbuf);
	Cubes7(Sbuf,2);
	os_fb_clear(0);

	while(os_mod_get_order()<0x0A) {
		//Flip(Sbuf);
	}
	Cubes4(Sbuf);
	WaveCubes(Sbuf);


	set_default_palette();

	Greetings(Sbuf);
	CubesInside(Sbuf);

	set_default_palette();

	Last(Sbuf);

	ReLast(Sbuf);
	title2_2(Sbuf);
	Last2(Sbuf);

	dacube2_exit(0);
	return 0;
}

