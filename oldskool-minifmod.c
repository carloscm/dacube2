
#include <stdio.h>
#include <stdlib.h>

#include "oldskool.h"

#ifdef __COMPILE_MINIFMOD__
#undef __COMPILE_MINIFMOD__
#endif
#include "minifmod/minifmod.h"

char moduloak_data[] = {
#include "data/modxm.i"
};
#define MODULOAK_SIZE 53525

static int mod_pos = 0;

static unsigned int memopen(char *name) {
	mod_pos = 0;
	return 0xcafebabe;
}

static void memclose(unsigned int handle) {
	mod_pos = 0;
}

static int memread(void *buffer, int size, unsigned int handle) {
	if (mod_pos + size >= MODULOAK_SIZE)
		size = MODULOAK_SIZE - mod_pos;
        memcpy(buffer, (char *)moduloak_data+mod_pos, size);
	mod_pos += size;
	return size ;
}

static void memseek(unsigned int handle, int pos, signed char mode) {
	if (mode == SEEK_SET)
		mod_pos = pos;
	else if (mode == SEEK_CUR)
		mod_pos += pos;
	else if (mode == SEEK_END)
		mod_pos = MODULOAK_SIZE + pos;
	
	if (mod_pos > MODULOAK_SIZE)
		mod_pos = MODULOAK_SIZE;
}

static int memtell(unsigned int handle) {
	return mod_pos;
}

static FMUSIC_MODULE* moduloak = NULL;

int os_mod_start() {
	FSOUND_File_SetCallbacks(memopen, memclose, memread, memseek, memtell);
	if (!FSOUND_Init(44100, 0)) {
		os_panic("Error initializing MiniFMOD");
		return -1;
	}
	moduloak = FMUSIC_LoadSong(NULL, NULL);
	if (moduloak == NULL) {
		os_panic("Error loading song");
		return -1;
	}
        FMUSIC_PlaySong(moduloak);
	return 0;
}

void os_mod_end() {
#ifdef WIN32
	FMUSIC_StopSong(moduloak);
#endif
	FSOUND_Close();
}

int os_mod_get_order() {
	return FMUSIC_GetOrder(moduloak);
}

int os_mod_get_row() {
	return FMUSIC_GetRow(moduloak) + 1;
}

