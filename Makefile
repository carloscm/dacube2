CXX=g++
CC=gcc
CLINK=gcc
WINDRES=
CDEPS=gcc -MM
DEPEX=*.c

CXXFLAGS=-g3 `gtk-config --cflags` -D__COMPILE_MINIFMOD__ -D__LINUX__ -Wall -W -Os -pipe
CFLAGS=-g3  `gtk-config --cflags` -D__COMPILE_MINIFMOD__ -D__LINUX__ -Wall -W -Os  -pipe
LDFLAGS=-g3 -D__COMPILE_MINIFMOD__ -D__LINUX__ -Wall -W -Os -pipe
LIBS=-lm -lpthread  `gtk-config --libs`
NASM=nasm
NASMFLAGS=-f elf

SOURCE:=font.c title.c dacube2.c drawchar.c 3dengine.c effects.c
SOURCE+=oldskool-linux.c oldskool-mxm.c
SOURCE+=linux/x11.c linux/x11common.c linux/config.c linux/vidmode.c linux/xf86vmodemini.c
SOURCE+=mxm/mxmplay.asm mxm/ossio.asm mxm/module.asm

#SOURCE+=minifmod/fmusic.c minifmod/fsound.c minifmod/mixer_clipcopy.c
#SOURCE+=minifmod/mixer_fpu_ramp.c minifmod/mixer_fpu_ramp_asm.asm minifmod/music_formatxm.c
#SOURCE+=minifmod/system_file.c

EXE=dacube2
BUILD=linux-i586
OEX=o

include Makefile.common

