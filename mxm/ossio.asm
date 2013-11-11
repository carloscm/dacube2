; =============================================================================
;  MXMPlay/Linux-Win32                                Open Sound System output
; -----------------------------------------------------------------------------
;  This source is done by Luks. It support also playback at freq 22050Hz and
;  8-bit output.
; -----------------------------------------------------------------------------
;  MXMPlay       (C) 1995,1996 Niklas Beisert / Pascal
;  MXMPlay/Win32 (C) 1999 Domin8R and The Artist Formerly Known As Doctor Roole
;  MXMPlay/Linux (C) 2001,2002 Luks
; -----------------------------------------------------------------------------
;  $Id: ossio.asm,v 1.2 2002/03/18 15:33:35 luks Exp $
; =============================================================================

section .bss

%define USESYSCALLS 1

%include "mxm/mxmallfx.inc"

%if (USE22050ASDEFAULT == 1) && (USECOMPATIBLEMODE == 0)
%error "USE22050ASDEFAULT is not supported without USECOMPATIBLEMODE"
%endif

%if (USE8BITASDEFAULT == 1) && (USECOMPATIBLEMODE == 0)
%error "USE8BITASDEFAULT is not supported without USECOMPATIBLEMODE"
%endif

%macro aglobal 1
global %1
%1:
%endmacro

%define NumFrags        8
%define FragSizeValue   10

%define FragmentValue   (((NumFrags) << 16) | (FragSizeValue))
%define FragmentSize    (1 << FragSizeValue)

extern pthread_create
extern pthread_join
extern pthread_exit

%define O_WRONLY 01

%define SNDCTL_DSP_SAMPLESIZE   -1073459195
%define SNDCTL_DSP_STEREO       -1073459197
%define SNDCTL_DSP_SPEED        -1073459198
%define SNDCTL_DSP_SETFRAGMENT  -1073459190

extern xmpInit
extern xmpRender

section .bss

%define INIT_DSP_FILE   1
%define INIT_THREAD     2

struc ossdata
        .exitreq        resd 1                  ; exit request flag for thread
        .iniflag        resd 1                  ; what is initilized?
        .thread         resd 1                  ; thread handle
        .dspfile        resd 1                  ; dsp file handle
%if USECOMPATIBLEMODE
        .clamp          resd 1                  ; clamp proc
        .bsizebyte      resd 1                  ; buffer size in bytes
        .mixflags       resd 1                  ; mix flags for mxm player
%endif
        .mxmdata        resd 1                  ; pointer to module
        .mxmheap        resb 0x10000            ; memory for player
        .mixbuf         resd FragmentSize << 3  ; mix buffer (32bit, stereo)
        .sndbuf         resd FragmentSize << 2  ; final sound buffer (16bit, stereo)
        .size
endstruc

tmp     resd 1
globdat resb ossdata.size

section .data

filename db '/dev/dsp', 0
msg db "0",0

section .text

; ============================================================================
;  clamp(esi: source, edi: dest, ecx: # of samples)
; ============================================================================

clamp16:
        push    ebx
        or      ecx,ecx
        jz      .End
        mov     ebx,32767
        mov     edx,ebx
        neg     edx
.Loop:  lodsd
        cmp     eax,ebx
        jng     .NoClampHigh
        mov     eax,ebx
.NoClampHigh:
        cmp     eax,edx
        jnl     .NextSample
        mov     eax,edx
.NextSample:
        stosw
        loop    .Loop
        pop     ebx
.End:   ret

%if USECOMPATIBLEMODE
clamp8:
        push    ebx
        or      ecx,ecx
        jz      .End
        mov     ebx,32767
        mov     edx,ebx
        neg     edx
.Loop:  lodsd
        cmp     eax,ebx
        jng     .NoClampHigh
        mov     eax,ebx
.NoClampHigh:
        cmp     eax,edx
        jnl     .NextSample
        mov     eax,edx
.NextSample:
        sar     eax,8
        xor     al,0x80
        stosb
        loop    .Loop
        pop     ebx
.End:   ret
%endif

; ============================================================================
;  int ossInit(void * mxmfile)
; ----------------------------------------------------------------------------
;  initialize player, initialize oss, create thread
; ============================================================================

aglobal ossInit

        push    ebp

        mov     ebp,globdat
        mov     eax,[esp+8]
        mov     dword [ebp+ossdata.mxmdata],eax

        pushad

        mov     dword [ebp+ossdata.iniflag],0

        ; open '/dev/dsp' file
        mov     eax,5
        mov     ebx,filename
        mov     ecx,O_WRONLY
        xor     edx,edx
        int     0x80
        cmp     eax,-1
        je      near .error             ; error -> '/dev/dsp' not found
        mov     [ebp+ossdata.dspfile],eax
        or      dword [ebp+ossdata.iniflag],INIT_DSP_FILE

        ; set wave format

%if USE8BITASDEFAULT

        mov     dword [tmp],8
        mov     eax,54
        mov     ebx,[ebp+ossdata.dspfile]
        mov     ecx,SNDCTL_DSP_SAMPLESIZE
        mov     edx,tmp
        int     0x80
        cmp     dword [tmp],8
        jne     .try16bit
        mov     dword [ebp+ossdata.clamp],clamp8
        mov     dword [ebp+ossdata.bsizebyte],FragmentSize << 1
        jmp     short .try16bitdone

.try16bit:
        mov     dword [tmp],16
        mov     eax,54
        mov     ebx,[ebp+ossdata.dspfile]
        mov     ecx,SNDCTL_DSP_SAMPLESIZE
        mov     edx,tmp
        int     0x80
        cmp     dword [tmp],16
        jne     near .error
        mov     dword [ebp+ossdata.clamp],clamp16
        mov     dword [ebp+ossdata.bsizebyte],FragmentSize << 2
.try16bitdone:

%else ; USE8BITASDEFAULT

        mov     dword [tmp],16
        mov     eax,54
        mov     ebx,[ebp+ossdata.dspfile]
        mov     ecx,SNDCTL_DSP_SAMPLESIZE
        mov     edx,tmp
        int     0x80
        cmp     dword [tmp],16
%if USECOMPATIBLEMODE
        jne     .try8bit
        mov     dword [ebp+ossdata.clamp],clamp16
        mov     dword [ebp+ossdata.bsizebyte],FragmentSize << 2
        jmp     short .try8bitdone
%else
        jne     near .error
%endif

%if USECOMPATIBLEMODE
.try8bit:
        mov     dword [tmp],8
        mov     eax,54
        mov     ebx,[ebp+ossdata.dspfile]
        mov     ecx,SNDCTL_DSP_SAMPLESIZE
        mov     edx,tmp
        int     0x80
        cmp     dword [tmp],8
        jne     near .error
        mov     dword [ebp+ossdata.clamp],clamp8
        mov     dword [ebp+ossdata.bsizebyte],FragmentSize << 1
.try8bitdone:
%endif

%endif ;USE8BITASDEFAULT

        mov     dword [tmp],1           ; stereo -> yes
        mov     eax,54
        mov     ebx,[ebp+ossdata.dspfile]
        mov     ecx,SNDCTL_DSP_STEREO
        mov     edx,tmp
        int     0x80
        cmp     dword [tmp],1
        jne     near .error

%if USE22050ASDEFAULT == 0
        mov     dword [tmp],44100       ; frequency -> 44100
        mov     eax,54
        mov     ebx,[ebp+ossdata.dspfile]
        mov     ecx,SNDCTL_DSP_SPEED
        mov     edx,tmp
        int     0x80
        cmp     dword [tmp],44100
%if USECOMPATIBLEMODE
        jne     .try22050
        mov     dword [ebp+ossdata.mixflags],0
        jmp     short .try22050done
%else
        jne     .error
%endif
%endif

%if USECOMPATIBLEMODE
.try22050:
        mov     dword [tmp],22050
        mov     eax,54
        mov     ebx,[ebp+ossdata.dspfile]
        mov     ecx,SNDCTL_DSP_SPEED
        mov     edx,tmp
        int     0x80
        cmp     dword [tmp],22050
        jne     .error
        mov     byte [ebp+ossdata.mixflags],1
.try22050done:
%endif

        ; set fragment size
        mov     dword [tmp],FragmentValue
        mov     eax,54
        mov     ebx,[ebp+ossdata.dspfile]
        mov     ecx,SNDCTL_DSP_SETFRAGMENT
        mov     edx,tmp
        int     0x80

        ; initialize player
        lea     edi,[ebp+ossdata.mxmheap]
        mov     esi,[ebp+ossdata.mxmdata]
%if USECOMPATIBLEMODE
        mov     al,[ebp+ossdata.mixflags]
%endif
        call    xmpInit
        or      eax,eax
        jnz     near .error                     ; error -> player ???

        ; clear exit request flag
        mov     dword [ebp+ossdata.exitreq],0

        ; create thread
        push    dword 0
        push    dword threadproc
        push    dword 0
        push    dword globdat+ossdata.thread
        call    pthread_create
        add     esp,16
        or      eax,eax
        jnz     .error                  ; error -> can't create thread
        or      dword [ebp+ossdata.iniflag],INIT_THREAD

        popad
        xor     eax,eax
        pop     ebp
        ret

.error: call    ossClose
        popad
	xor	eax,eax
	inc	eax
        pop     ebp
        ret

; ============================================================================
;  void ossClose()
; ----------------------------------------------------------------------------
;  stop thread, close dsp file
; ============================================================================

aglobal ossClose

        pushad

        ; set global data pointer
        mov     ebp,globdat

        ; stop thread
        inc     dword [ebp+ossdata.exitreq]

        ; close dsp file
        test    dword [ebp+ossdata.iniflag],INIT_DSP_FILE
        jz      .FileDone
        mov     eax,6
        mov     ebx,[ebp+ossdata.dspfile]
        int     0x80
.FileDone:

        ; wait for thread
        test    dword [ebp+ossdata.iniflag],INIT_THREAD
        jz      .ThreadDone
.ThreadWait:
        cmp     dword [ebp+ossdata.exitreq],0
        je      .ThreadDone
        jmp     .ThreadWait
.ThreadDone:

        popad
        ret

; ============================================================================
;  threadproc()
; ----------------------------------------------------------------------------
;  sound thread proceudre
; ============================================================================


threadproc:

        mov     ebp,globdat

        ; main loop
.Loop   cmp     dword [ebp+ossdata.exitreq],0
        jne     near .Exit

        lea     edi,[ebp+ossdata.mixbuf]
        mov     eax,FragmentSize
        call    xmpRender

        lea     esi,[ebp+ossdata.mixbuf]
        lea     edi,[ebp+ossdata.sndbuf]
        mov     ecx,FragmentSize << 1
%if USECOMPATIBLEMODE
        call    [ebp+ossdata.clamp]
%else
        call    clamp16
%endif

        mov     eax,4
        mov     ebx,[ebp+ossdata.dspfile]
        lea     ecx,[ebp+ossdata.sndbuf]
%if USECOMPATIBLEMODE
        mov     edx,[ebp+ossdata.bsizebyte]
%else
        mov     edx,FragmentSize << 2
%endif
        int     0x80

        ; next cycle
        jmp     .Loop

        ; exit
.Exit   dec     dword [ebp+ossdata.exitreq]
        push    dword 0                 ; return value = 0
        call    pthread_exit            ; exit thread


; =============================================================================
; $Log: ossio.asm,v $
; Revision 1.2  2002/03/18 15:33:35  luks
; Changed error checking from xmpInit.
;
; Revision 1.1  2002/03/18 15:04:31  luks
; Project added under CVS control.
;
; =============================================================================
