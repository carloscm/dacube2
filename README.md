dacube2
=======

Source code for the Windows and (outdated) Linux port of [dacube2](https://www.pouet.net/prod.php?which=9120) by [Fuzzion](http://fuzzion.org/).


Porting guide
=============

The main intro code is in the files

`font.c title.c dacube2.c drawchar.c 3dengine.c effects.c`

They were cleaned up of assembler code and to compile with GCC.

The `oldskool` files are the porting layer, all the graphics and MOD music calls are done here. This layer in turn will call platform specific APIs so the main intro code stays clean of them.

The MOD player is a separate library. Two of them are included in the code. For the final version the MXM player was used since it was compact and allowed to stay under 64KB. Minifmod is also included and it is a better target for porting since it should be clean C code.

Check the Makefiles for the exact files used by each platform.
