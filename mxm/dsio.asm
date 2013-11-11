; =============================================================================
;  MXMPlay/Linux-Win32                                 DirectSound output code
; -----------------------------------------------------------------------------
;  This source is done and (C) 1999 by Domin8R and The Artist Formerly Known
;  As Doctor Roole of ELITEGROUP.
; -----------------------------------------------------------------------------
;  MXMPlay       (C) 1995,1996 Niklas Beisert / Pascal
;  MXMPlay/Win32 (C) 1999 Domin8R and The Artist Formerly Known As Doctor Roole
;  MXMPlay/Linux (C) 2001,2002 Luks
; -----------------------------------------------------------------------------
;  $Id: dsio.asm,v 1.3 2002/03/25 18:22:32 luks Exp $
; =============================================================================

SECTION .bss

struc WAVEFORMATEX
  .wFormatTag      resw 1       ; format type
  .nChannels       resw 1       ; number of channels (i.e. mono, stereo...)
  .nSamplesPerSec  resd 1       ; sample rate
  .nAvgBytesPerSec resd 1       ; for buffer estimation
  .nBlockAlign     resw 1       ; block size of data
  .wBitsPerSample  resw 1       ; number of bits per sample of mono data
  .cbSize          resw 1       ; the count in bytes of the size of extra bytes
  .size
endstruc


struc dsdata
  .sbuf      resd 1             ; ptr to the secondary IDirectSoundBuffer
  .pbuf      resd 1             ; ptr to the primary   IDirectSoundBuffer
  .dssd      resd 1             ; ptr to the IDirectSound instance
  .exitreq   resd 1             ; exit request flag for sound thread
  .thndl     resd 1             ; handle of sound thread
  .buf1      resd 1             ; 1st locked buffer address for streaming
  .len1      resd 1             ; length of 1st buffer
  .buf2      resd 1             ; 2nd locked buffer address for streaming
  .len2      resd 1             ; length of 2nd buffer
  .towrite   resd 1             ; bytes to write
  .curpos    resd 1             ; current read position on buffer
  .lastpos   resd 1             ; last write position in buffer
  .dummy     resd 1             ; dummy dword for thread ID
  .tempwfx   resb WAVEFORMATEX.size
  .mxmheap   resb 10000h        ; resource memory for MXMPlay
  .mixbuffer resd 20000h        ; 32bit stereo mixing buffer

  .size
endstruc

extern _xmpInit
extern _xmpRender
extern _CreateThread@24
extern _WaitForSingleObject@8
extern _Sleep@4
extern _DirectSoundCreate@12
extern _SetThreadPriority@8

global _dsInit@8
global _dsClose@0

globdat:     resb dsdata.size

SECTION .code


clamp:       ; esi: source, edi: dest, ecx: # of samples
    pushad
    shr ecx, 1
    jz  .clampend
    mov ebx, 32500      ; yes, 32500. Just trust us.
    mov edx, ebx
    neg edx
.ClampLoop:
    lodsd
    cmp eax, ebx
    jng .NoClampHigh
    mov eax, ebx
.NoClampHigh:
    cmp eax, edx
    jnl .NextSample
    mov eax, edx
.NextSample:
    stosw
    loop .ClampLoop
    popad
.clampend
    ret




_dsInit@8:

    pushad
    mov      ebx, globdat



    ; clear global data
    mov      ecx, dsdata.size
    mov      edi, ebx
    xor      eax, eax
    rep      stosb

    ; init MXM player
    lea      edi, [ebx + dsdata.mxmheap]
    mov      esi, [esp + 40]              ; first parm (mxmheap)
    call     _xmpInit

    ; create IDirectSound instance
    push     eax
    lea      ecx, [ebx + dsdata.dssd]
    push     ecx
    push     eax
    call     _DirectSoundCreate@12

    or       eax, eax
    jnz      .initfailgate

    ; set DSound cooperative level
    mov      al, 3       ; exclusive mode
    push     eax
    push     dword [esp + 40]            ; second parm (hwnd)
    mov      esi, [ebx + dsdata.dssd]
    push     esi
    mov      edi, [esi]  ; edx = vtbl
    call     [edi + 18h] ; IDirectSound::SetCooperativeLevel

    or       eax, eax
    jnz      .initfailgate

    ; obtain primary buffer
    push     eax
    lea      ebp, [ebx + dsdata.pbuf]
    push     ebp
    push     dword primdesc
    push     esi
    call     [edi + 0ch] ; IDirectSound::CreateSoundBuffer

    or       eax, eax
.initfailgate:
    jnz      .gate2


    ; obtain secondary buffer
    push     eax
    lea      edx, [ebx + dsdata.sbuf]
    push     edx
    push     dword streamdesc
    push     esi
    call     [edi + 0ch] ; IDirectSound::CreateSoundBuffer

    or       eax, eax
.gate2
    jnz      near .InitFailed


    ; set primary buffer format

    lea      edi, [ebx + dsdata.tempwfx]
    push     edi

    lea      esi, [wfxprimary]
    lea      ecx, [eax + WAVEFORMATEX.size]
    rep      movsb

    mov      esi, [ebp]
    push     esi
    mov      edi, [esi]  ; edx = vtbl
    call     [edi + 38h] ; IDirectSoundBuffer::SetFormat


    ; lock, clear and unlock secondary buffer
    xor      esi,esi
    push     dword 2                   ; DSBLOCK_ENTIREBUFFER
    lea      edx, [ebx + dsdata.len2]
    push     edx
    lea      edx, [ebx + dsdata.buf2]
    push     edx
    lea      edx, [ebx + dsdata.len1]
    push     edx
    lea      edx, [ebx + dsdata.buf1]
    push     edx
    push     esi
    push     esi
    mov      ebp, [ebx + dsdata.sbuf]
    mov      esi, [ebp]    ; vtbl
    push     ebp
    call     [esi + 2ch]   ; IDirectSoundBuffer::Lock
    or       eax, eax
    jnz      .InitFailed
    mov      ecx, [ebx + dsdata.len1]
    mov      edi, [ebx + dsdata.buf1]
    rep      stosb
    mov      ecx, [ebx + dsdata.len2]
    mov      edi, [ebx + dsdata.buf2]
    rep      stosb
    push     dword [ebx + dsdata.len2]
    push     dword [ebx + dsdata.buf2]
    push     dword [ebx + dsdata.len1]
    push     dword [ebx + dsdata.buf1]
    push     ebp
    call     [esi + 4ch]    ; IDirectSoundBuffer::Unlock
    or       eax, eax
    jnz      .InitFailed


    ; activate the streaming buffer
    xor      eax,eax
    inc      al
    push     eax         ; for later
    push     eax         ; DSPLAY_LOOPING
    dec      al
    push     eax
    push     eax
    push     ebp
    call     [esi + 30h] ; IDirectSoundBuffer::Play

    or       eax, eax
    jnz      .InitFailed

    ; start sound thread
    lea      edx, [ebx + dsdata.dummy]
    push     edx
    push     eax
    push     eax
    push     dword threadfunc
    push     eax
    push     eax
    call     _CreateThread@24

    mov      [ebx + dsdata.thndl], eax

    inc      dword [esp]
    push     eax
    call     _SetThreadPriority@8

    ; ok, everything's done
    popad
    clc
    jmp     short .initends


.InitFailed   ; oh no, we've encountered an error!

    call     _dsClose@0
    popad
    stc
.initends
    sbb     eax,eax
    ret     8h



_dsClose@0:

    pushad
    mov     ebx, globdat

    ; set exit request..
    inc     dword [ebx + dsdata.exitreq]
    mov     eax, [ebx + dsdata.thndl]
    or      eax, eax
    jz      .NoThread

    ; give the thread a chance to finish

    push    dword 1000
    push    eax
    call    _WaitForSingleObject@8

.NoThread:

    ; remove allocated instances
    mov     edi, .ReleaseComObj
    xchg    esi, ebx
    lodsd
    call    edi
    lodsd
    call    edi
    lodsd

    db  0b1h   ; 31337 untergrund trick!

.ReleaseComObj:
    pushad
    or      eax, eax
    jz      .endRelease
    mov     edx, [eax]
    push    eax
    call    [edx+08h]  ; IUnknown::Release
.endRelease
    popad
    ret


threadfunc:
    pushad
    mov     ebx, globdat

.looping:
    ; check for exit request
    cmp     byte [ebx + dsdata.exitreq], 0
    je      .loopok

    popad
    xor     eax,eax
    ret     4h

.loopok:
    ; sleep 25 msecs
    xor     ebp,ebp
    lea     edx,[ebp+25]
    push    edx
    call    _Sleep@4

    mov     [ebx + dsdata.buf1], ebp
    mov     [ebx + dsdata.buf2], ebp

.lockLoop:
    ; fetch current buffer position
    xor     eax, eax
    push    eax

    lea     eax, [ebx + dsdata.curpos]
    push    eax

    mov     ebp, [ebx + dsdata.sbuf]
    push    ebp
    mov     edx, [ebp]      ; vtbl
    call    [edx + 10h]     ; IDirectSoundBuffer::GetCurrentPosition
    cmp     eax, 88780096h  ; DSERR_BUFFERLOST
    je      .tryRestore

    ; find out how many bytes to write
    mov     eax, [ebx + dsdata.curpos]
    and     eax, ~1fh
    mov     ecx, eax
    sub     eax, [ebx + dsdata.lastpos]
    jns     .ja
    add     eax, 10000h
.ja:
    mov     edi, eax

    ; try to lock the buffer
    xor     esi,esi
    push    esi
    lea     edx, [ebx + dsdata.len2]
    push    edx
    lea     edx, [ebx + dsdata.buf2]
    push    edx
    lea     edx, [ebx + dsdata.len1]
    push    edx
    lea     edx, [ebx + dsdata.buf1]
    push    edx

    push    eax
    push    dword [ebx + dsdata.lastpos]
    mov     [ebx + dsdata.lastpos],ecx

    mov     edx, [ebp]    ; vtbl
    push    ebp
    call    [edx + 2ch]   ; IDirectSoundBuffer::Lock
    cmp     eax, 88780096h; DSERR_BUFFERLOST
    jne     .lockOK
.tryRestore:
    mov     edx, [ebp]    ; vtbl
    push    ebp
    call    [edx + 50h]   ; IDirectSoundBuffer::Restore
    jmp     short .lockLoop

.lockOK:
    mov     eax, edi
    shr     eax, 2
    lea     edi, [ebx + dsdata.mixbuffer]
    call    _xmpRender

    mov     esi, edi

    push    ebp
    mov     ebp, clamp

    ; check buffer one
    mov     edi, [ebx + dsdata.buf1]
    or      edi, edi
    jz      .nobuf1
    mov     ecx, [ebx + dsdata.len1]
    call    ebp
    shl     ecx, 1
    add     esi, ecx

.nobuf1:
    ; check buffer two
    mov     edi, [ebx + dsdata.buf2]
    or      edi, edi
    jz      .nobuf2
    mov     ecx, [ebx + dsdata.len2]
    call    ebp

.nobuf2:
    pop     ebp
    push    dword [ebx + dsdata.len2]
    push    dword [ebx + dsdata.buf2]
    push    dword [ebx + dsdata.len1]
    push    dword [ebx + dsdata.buf1]
    push    ebp
    mov     esi, [ebp]     ; vtbl
    call    [esi + 4ch]    ; IDirectSoundBuffer::Unlock

    jmp     .looping


wfxprimary:
    DB  01H, 00H, 02H, 00H, 44H, 0acH, 00H, 00H
    DB  10H, 0b1H, 02H, 00H, 04H, 00H, 10H, 00H
    DB  00H, 00H, 00H, 00H

streamdesc:
    DB  14H, 00H, 00H, 00H, 00H, 80H, 01H, 00H
    DB  00H, 00H, 01H, 00H, 00H, 00H, 00H, 00H
    DD  wfxprimary

primdesc:
    DB  14H, 00H, 00H, 00H, 01H, 00H, 00H, 00H
    DB  00H, 00H, 00H, 00H, 00H, 00H, 00H, 00H
    DB  00H, 00H, 00H, 00H

; =============================================================================
; $Log: dsio.asm,v $
; Revision 1.3  2002/03/25 18:22:32  luks
; Swaped return values from dsInit: 0 = ok, 1 = error
;
; Revision 1.2  2002/03/18 15:33:36  luks
; Changed error checking from xmpInit.
;
; Revision 1.1  2002/03/18 15:04:31  luks
; Project added under CVS control.
;
; =============================================================================
