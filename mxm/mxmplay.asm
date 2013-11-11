; =============================================================================
;  MXMPlay/Linux-Win32                                          player & mixer
; -----------------------------------------------------------------------------
;  This source is 99% same as in MXMPlay/Win32. I (luks) only add 22050 mixing
;  frequency and optimize bpm checking.
; -----------------------------------------------------------------------------
;  MXMPlay       (C) 1995,1996 Niklas Beisert / Pascal
;  MXMPlay/Win32 (C) 1999 Domin8R and The Artist Formerly Known As Doctor Roole
;  MXMPlay/Linux (C) 2001,2002 Luks
; -----------------------------------------------------------------------------
;  $Id: mxmplay.asm,v 1.2 2002/03/25 18:21:07 luks Exp $
; =============================================================================

SECTION .bss

%include "./mxm/mxmallfx.inc"

%define USEVOLCOL      (USEVVOL || USEVVOLSLIDE || USEVFVOLSLIDE || USEVVIBRATE || USEVVIBRATO || USEVPAN || USEVPANSLIDE || USEVPORTANOTE)
%define USEAMIGAFREQ   ((USEFREQTAB == 1) == 0)
%define USELINEARFREQ  ((USEFREQTAB == 0) == 0)
%define USEBOTHFREQ    (USEAMIGAFREQ && USELINEARFREQ)

mixfreq     equ 44100
channelsize equ 256
headersize  equ 2750h

%ifdef WIN32
%macro global 1
%define %1 _%1
[global %1]
%endmacro
%endif

struc mxmheader
  .hdMXMSig             resd 1
  .hdNOrders            resd 1
  .hdOrdLoopStart       resd 1
  .hdNChannels          resd 1
  .hdNPatterns          resd 1
  .hdNInstruments       resd 1
  .hdIniTempo           resb 1
  .hdIniBPM             resb 1
  .hdOptions            resw 1
  .hdSampStart          resd 1
  .hdSampMem8           resd 1
  .hdSampMem16          resd 1
  .hdPitchMin           resd 1
  .hdPitchMax           resd 1
  .hdPanPos             resb 32
  .hdOrderTable         resb 256
  .hdInstrTable         resd 128
  .hdPatternTable       resd 256
  .hdSampleTable        resd 2048
endstruc


struc instrument
  .insNSamples          resd 1
  .insSamples           resb 96
  .insVolFade           resw 1
  .insVibType           resb 1
  .insVibSweep          resb 1
  .insVibDepth          resb 1
  .insVibRate           resb 1
  .insVNum              resb 1
  .insVSustain          resb 1
  .insVLoopS            resb 1
  .insVLoopE            resb 1
  .insVEnv              resw 24
  .insPNum              resb 1
  .insPSustain          resb 1
  .insPLoopS            resb 1
  .insPLoopE            resb 1
  .insPEnv              resw 24
  .reserved             resb 46
endstruc

struc sample
  .smpLoopStart         resd 1
  .smpEnd               resd 1
  .smpMixMode           resb 1
  .smpDefVol            resb 1
  .smpDefPan            resb 1
  .smpNormNote          resw 1
  .smpIndex             resw 1
  .reserved             resb 1
endstruc


struc channel
  .chMixInited          resb 1
  .chMixStartPos        resd 1
  .chMixEndPos          resd 1
  .chMixLoopPos         resd 1
  .chMixMode            resb 1
  .chMixStopIt          resb 1
  .chMixChangeSamp      resb 1
  .chMixNextPos         resd 1
  .chMixFrq             resw 1
  .chMixVol             resw 1
  .chMixPan             resb 1
  .chMixPosW            resd 1
  .chMixPosF            resd 1
  .chVol                resb 1
  .chFinalVol           resb 1
  .chPan                resb 1
  .chFinalPan           resb 1
  .chPitch              resd 1
  .chFinalPitch         resd 1
  .chCurIns             resb 1
  .chEnvIns             resd 1
  .chCurNormNote        resw 1
  .chSustain            resb 1
  .chFadeVol            resw 1
  .chAVibPos            resb 1
  .chAVibSwpPos         resb 1
  .chVolEnvPos          resd 1
  .chVolEnvSegPos       resw 1
  .chPanEnvPos          resd 1
  .chPanEnvSegPos       resw 1
  .chDefVol             resb 1
  .chDefPan             resb 1
  .chCommand            resb 1
  .chVCommand           resb 1
  .chPortaToPitch       resd 1
  .chPortaToVal         resd 1
  .chVolSlideVal        resb 1
  .chGVolSlideVal       resb 1
  .chVVolPanSlideVal    resb 1
  .chPanSlideVal        resb 1
  .chFineVolSlideUVal   resb 1
  .chFineVolSlideDVal   resb 1
  .chPortaUVal          resd 1
  .chPortaDVal          resd 1
  .chFinePortaUVal      resb 1
  .chFinePortaDVal      resb 1
  .chXFinePortaUVal     resb 1
  .chXFinePortaDVal     resb 1
  .chVibRate            resb 1
  .chVibPos             resb 1
  .chVibType            resb 1
  .chVibDep             resb 1
  .chTremRate           resb 1
  .chTremPos            resb 1
  .chTremType           resb 1
  .chTremDep            resb 1
  .chPatLoopCount       resb 1
  .chPatLoopStart       resb 1
  .chArpPos             resb 1
  .chArpNotes           resb 3
  .chActionTick         resb 1
  .chMRetrigPos         resb 1
  .chMRetrigLen         resb 1
  .chMRetrigAct         resb 1
  .chDelayNote          resb 1
  .chOffset             resb 1
  .chGlissando          resb 1
  .chTremorPos          resb 1
  .chTremorLen          resb 1
  .chTremorOff          resb 1
endstruc


struc GDS
  .globalvol            resb 1
  .uservol              resb 1
  .syncval              resb 1
  .curtick              resb 1
  .curtempo             resb 1
  .tick0                resb 1
  .currow               resd 1
  .patptr               resd 1
  .patlen               resd 1
  .curord               resd 1
  .jumptoord            resd 1
  .jumptorow            resd 1
  .patdelay             resb 1
  .procnot              resb 1
  .procins              resb 1
  .procvol              resb 1
  .proccmd              resb 1
  .procdat              resb 1
  .notedelayed          resb 1
  .stimerlen            resd 1
  .portatmp             resb 1
  .keyofftmp            resb 1
  .head                 resb headersize
  .chandata             resb channelsize*32
  .vibtabs              resb 1024
  .samplestogib         resd 1
  .samplesleft          resd 1
  .freqf                resd 1
  .freqw                resd 1
  .smptomix             resd 1
  .destbuf              resd 1
  .smpremain            resd 1
  .voll                 resd 1
  .volr                 resd 1
  .synccb		resd 1
  .isplaying            resb 1
%if USECOMPATIBLEMODE
  .mixflags             resb 1
%endif
endstruc

GDP resd 1

SECTION .code

%if USECOPYRIGHT
  db "... MXMPlay is still (C) Niklas Beisert somewhen in the 90s."
%endif

; =============================================================================
;  mixerproc(ecx: # of samples, edi: 32bit destbuffer)
; =============================================================================

mixerproc:

        ; ecx: # of samples
        ; edi: 32bit destbuffer (ist nachher aktualisiert)
        ; * edi: nach dem 32bit destbuffer

        mov     [ebp + GDS.smptomix], ecx
        mov     [ebp + GDS.destbuf],  edi

        add     ecx, ecx
        xor     eax, eax
        rep     stosd

        mov     ecx, 32

        lea     esi, [ebp+GDS.chandata]         ; instr ptr.

.ChanLoop:

        push    ecx
        push    esi

        cmp     byte [esi + channel.chMixInited], 0
        je      near .NextChan

        cmp     dword [esi + channel.chMixNextPos], -1
        je      .NoNextPos

        mov     ebx,  [esi + channel.chMixNextPos]     ; nextpos handling
        mov     dword [esi + channel.chMixNextPos], -1
        mov     dword [esi + channel.chMixPosW], ebx
        mov         dword [esi + channel.chMixPosF], 0

.NoNextPos:

        mov     ecx, [ebp + GDS.smptomix]
        mov     [ebp + GDS.smpremain], ecx
        mov     edi,[ebp + GDS.destbuf]

.FragLoop:

        mov     ecx, [ebp + GDS.smpremain]
        or      ecx, ecx
        jz      near .NextChan

        ; das SOLLTE klappen
        movzx   eax, word [esi + channel.chMixFrq]
        xor     ebx,ebx
        shld    ebx,eax,22
        shl     eax,22

        test    byte [esi + channel.chMixMode],32
        jz      .isfwd1

        not     eax
        add     eax,1
        not     ebx
        adc     ebx,0

.isfwd1:
        mov     dword [ebp + GDS.freqw], ebx
        mov     dword [ebp + GDS.freqf], eax

        xor     eax, eax
        sub     eax, [esi + channel.chMixPosF]
        mov     edx, [esi + channel.chMixEndPos]
        sbb     edx, [esi + channel.chMixPosW]

        ; edx:eax -> 32:32bit Bis End
        ; ebx     ->  6:10bit Freq

        ; PrÑmisse: 1/1 == 1
        ;

        shrd    eax, edx, 22
        sar     edx, 22

        movzx   ebx, word [esi + channel.chMixFrq]
        test    byte [esi + channel.chMixMode],32
        jz      .isfwd2
        neg     ebx
.isfwd2:
        add     eax, ebx
        sub     eax, 1
; ccm was here
	push	ebx
	cmp	ebx, 0
	jne	.ccmsux
	mov	ebx, 1
.ccmsux
        idiv    ebx
	pop	ebx

        mov     ecx, eax
        cmp     ecx, [ebp + GDS.smpremain]
        jbe     .passt
        mov     ecx, [ebp + GDS.smpremain]
.passt:
        sub     [ebp + GDS.smpremain],ecx

        or      ecx,ecx
        je      near .mixend
        js      near .mixend

        ; hier: ecx = wie viel wir mixen mÅssen
        ; d.h. theoretisch kînnten wir anfangen
        ; problem: ich habe angst.

        test    byte [esi + channel.chMixMode], 4
        jz      near .Mix8

.mix16:
        movzx   eax,word [esi + channel.chMixVol]
        movzx   ebx,byte [esi + channel.chMixPan]
        imul    eax,ebx
        mov     [ebp + GDS.volr],eax
        movzx   eax, word [esi + channel.chMixVol]
        xor     ebx,0ffh
        imul    eax,ebx
        mov     [ebp + GDS.voll],eax

        mov     ebx,[esi + channel.chMixStartPos]
        mov     eax,[esi + channel.chMixPosW]

.m16loop:
        push    ecx

        mov     ecx,[esi + channel.chMixPosF]
        shr     ecx,16

        movsx   edx,word [ebx+2*eax]
        movsx   eax,word [ebx+2*eax+2]
        sub     eax,edx
        imul    eax,ecx
        sar     eax,16
        add     edx,eax
        mov     eax,edx

        imul    eax,[ebp + GDS.voll]
        sar     eax,16
        add     [dword edi], eax

        imul    edx,[ebp + GDS.volr]
        sar     edx,16
        add     [dword edi+4], edx

        mov     ecx,[esi + channel.chMixPosF]
        add     ecx,[ebp + GDS.freqf]
        mov     [esi + channel.chMixPosF],ecx

        mov     eax,[esi + channel.chMixPosW]
        adc     eax,[ebp + GDS.freqw]
        mov     [esi + channel.chMixPosW],eax

        pop     ecx
        add     edi,8
        dec     ecx
        jnz     .m16loop
        jmp     .mixend


.Mix8:
        movzx   eax,word [esi + channel.chMixVol]
        movzx   ebx,byte [esi + channel.chMixPan]
        imul    eax,ebx
        shl     eax,8
        mov     [ebp + GDS.volr],eax
        movzx   eax,word [esi + channel.chMixVol]
        xor     ebx,0ffh
        imul    eax,ebx
        shl     eax,8
        mov     [ebp + GDS.voll],eax

        mov     ebx,[esi + channel.chMixStartPos]
        mov     eax,[esi + channel.chMixPosW]

.m8loop:
        push    ecx

        mov     ecx,[esi + channel.chMixPosF]
        mov     eax,[esi + channel.chMixPosW]
        shr     ecx,16

        movsx   edx,byte [ebx+eax]
        movsx   eax,byte [ebx+eax+1]
        sub     eax,edx
        imul    eax,ecx
        sar     eax,16
        add     edx,eax
        mov     eax,edx

        imul    edx,[ebp + GDS.voll]
        sar     edx,16
        add     [edi], edx

        imul    eax,[ebp + GDS.volr]
        sar     eax,16
        add     [dword edi+4], eax

        mov     ecx,[esi + channel.chMixPosF]
        add     ecx,[ebp + GDS.freqf]
        mov     [esi + channel.chMixPosF],ecx

        mov     eax,[esi + channel.chMixPosW]
        adc     eax,[ebp + GDS.freqw]
        mov     [esi + channel.chMixPosW],eax

        pop     ecx
        add     edi,8
        dec     ecx
        jnz     .m8loop

.mixend:

        mov     eax, [esi + channel.chMixPosW]

        test    byte [esi + channel.chMixMode], 32
        jz      .isfwd3

        cmp     eax, [esi + channel.chMixEndPos]
        jl      .irgendwas
        jmp     .FragLoop

.isfwd3:

        cmp     eax, [esi + channel.chMixEndPos]
        jl      near .FragLoop

.irgendwas:

        ; (hier entweder sampleende oder loop)
        ; (nachteil: beides mu· man coden)

        test    byte [esi + channel.chMixMode], 24
        jz      .samplezuende

        test    byte [esi + channel.chMixMode], 16
        jz      .fwdloop

        ; Bidi-Loops stinken

        ; plan A: spiegeln
        mov     edx, [esi + channel.chMixEndPos]
        shl     edx, 1
        xor     eax, eax
        sub     eax, [esi + channel.chMixPosF]
        mov     [esi + channel.chMixPosF], eax
        sbb     edx, [esi + channel.chMixPosW]
        mov     [esi + channel.chMixPosW], edx

        ; plan B: andersrum
        xor     byte [esi + channel.chMixMode], 32

        ; plan C: BÑumchen Wechsel Dich
        mov     eax,[esi + channel.chMixEndPos]
        xchg    eax,[esi + channel.chMixLoopPos]
        mov     [esi + channel.chMixEndPos],eax
        jmp     .FragLoop

.fwdloop:
        mov     ebx, [esi + channel.chMixEndPos]
        sub     ebx, [esi + channel.chMixLoopPos]
        sub     eax, ebx
        mov     [esi + channel.chMixPosW], eax
        jmp     .FragLoop

.samplezuende:
        mov     byte [esi + channel.chMixInited],0

.NextChan:

        pop     esi
        pop     ecx
        add     esi, channelsize

        dec     ecx
        jnz     near .ChanLoop

        mov     ecx,[ebp + GDS.smptomix]
        mov     edi,[ebp + GDS.destbuf]
        lea     edi,[edi+8*ecx]

        ret

; =============================================================================
; xmpRender(edi: dest buffer, eax: # of samples)
; -----------------------------------------------------------------------------
; render sample stream into 32-bit stereo buffer
; =============================================================================

global xmpRender
xmpRender:
global _xmpRender
_xmpRender:

        pushad
        call    loadebp
        cld

        mov     [ebp + GDS.samplestogib], eax

        cmp     byte [ebp+GDS.isplaying], 0
        jne     .gibloop

        mov     ecx,eax
        xor     eax,eax
        shl     ecx,1
        rep     stosd
        jmp     .ende

.gibloop:
        ;int 3
        mov     eax, [ebp + GDS.samplestogib]
        or      eax, eax
        jz      .ende

        mov     ebx, [ebp + GDS.samplesleft]
        cmp     ebx, 256
        jae     .nonewtick

        pushad
        call    PlayTick
        mov     eax,[ebp+GDS.stimerlen]
        add     [ebp+GDS.samplesleft],eax
        popad

.nonewtick:
        mov     ecx, [ebp + GDS.samplesleft]
        shr     ecx, 8
        cmp     ecx, [ebp + GDS.samplestogib]
        jbe     .nagut
        mov     ecx, [ebp + GDS.samplestogib]
.nagut:

        mov     eax,ecx
        shl     eax,8
        sub     [ebp + GDS.samplesleft],eax
        sub     [ebp + GDS.samplestogib],ecx
        call    mixerproc
        jmp     .gibloop
.ende:
        popad
        ret


; *****************************************************************************
;  XM player
; *****************************************************************************

loadebp:
        call    .loadebp_getadr
.loadebp_getadr:
        pop     ebp
        mov     ebp, [GDP]
        ret

inittables:
%if (USEVIBRATO || USEVVIBRATO || USEAUTOVIBRATO)
        call    .inittables_getadr
.inittables_getadr:
        pop     esi
        add     esi,sintab - .inittables_getadr
        lea     edi,[ebp + GDS.vibtabs]
        mov     ecx,16
        rep     movsd
        mov     al,64
        stosb
        mov     cl,63
.inittables_sintabloop1:
        dec     esi
        mov     al,[esi]
        stosb
        dec     cl
        jnz     .inittables_sintabloop1

        lea esi,[ebp + GDS.vibtabs]
        mov     cl,128
.inittables_sintabloop2:
        lodsb
        neg     al
        stosb
        dec     cl
        jnz .inittables_sintabloop2

%if (USEVIBTYPE || USETREMTYPE || USEAUTOVIBRATOTYPE)
.inittables_dwntabloop:

        mov     al,cl
        sar     al,1
        neg     al
        stosb
        dec     cl
        jnz     .inittables_dwntabloop

.inittables_rectabloop:

        mov     al,cl
        and     al,80h
        sub     al,40h
        stosb
        dec cl
        jnz     .inittables_rectabloop

.inittables_uptabloop:

        mov     al,cl
        sar     al,1
        stosb
        dec     cl
        jnz     .inittables_uptabloop
%endif
%endif
        ret

; ============================================================================
;  xmpInit(esi: mxmdata, edi: mxmheap, al: mixflags)
; ----------------------------------------------------------------------------
;  initialize player and load module
; ============================================================================

global xmpInit
xmpInit:
global _xmpInit
_xmpInit:

        pushad
        mov     [GDP],edi
        xchg    ebp,edi
        mov     ebx,4000h
.xmpInit_inilp:
        dec     ebx
        mov     byte [ebp+ebx],0
        jnz     .xmpInit_inilp

%if USECOMPATIBLEMODE
        mov     [ebp+GDS.mixflags],al
%endif

        mov     byte [ebp + GDS.uservol],40h
        cmp     dword [esi + mxmheader.hdMXMSig],004D584Dh
        jne     near .xmpInit_fail

        lea     edi,[ebp + GDS.head + mxmheader.hdMXMSig]
        mov     ecx,headersize/4
        rep     movsd

        ; relocation loop:
        sub     esi,headersize
        sub     edi,2600h
        mov     ecx,2600h/4
.xmpInit_relloop:
        add     [edi],esi
        lea     edi, [edi+4]
        loop    .xmpInit_relloop

%if USEDELTASAMP
        test    byte [esi + mxmheader.hdOptions],4
        jz      .xmpInit_nodelta
        and     word [esi + mxmheader.hdOptions], ~4
        mov     ecx, [esi + mxmheader.hdSampMem8]
        mov     ebx, [esi + mxmheader.hdSampStart]
        xor     eax, eax
        jecxz   .xmpInit_8bitfini

.xmpInit_8bitdelta:
        add     al,[ebx+esi]
        mov     [ebx+esi],al
        inc     ebx
        loop    .xmpInit_8bitdelta
.xmpInit_8bitfini:

%if USE16BIT
        mov     ecx,[esi + mxmheader.hdSampMem16]
        xor     eax,eax
        jecxz   .xmpInit_16bitfini

.xmpInit_16bitdelta:
        add     ax, [ebx+esi]
        mov     [ebx+esi],ax
        lea     ebx, [ebx+2]
        loop    .xmpInit_16bitdelta
.xmpInit_16bitfini
%endif

.xmpInit_nodelta:
%endif

        mov     ecx,[esi + mxmheader.hdSampMem16]
        shl     ecx,1
        add     ecx,[esi + mxmheader.hdSampMem8]

        mov     eax,[esi + mxmheader.hdSampStart]
        add     esi,eax
        call    inittables
        mov     byte [ebp + GDS.isplaying], 0
        popad
        xor     eax,eax
        ret

.xmpInit_fail:
        popad
        xor     eax,eax
        inc     eax
        ret



getfreq6848:
        push    edx
        push    ebx
        push    ecx
        push    esi
        push    edi
        add     eax,8000h
        mov     edi,15
        mov     edx,eax
        mov     ebx,eax
        mov     ecx,eax
        shr     eax,12
        shr     edx,8
        shr     ebx,4
        and     eax,edi
        and     ebx,edi
        and     ecx, edi
        and     edx,edi
        call    .getfreq6848_getadr
.getfreq6848_getadr:
        pop     esi
        add     esi,logfreqtab-.getfreq6848_getadr
        mov     eax,[esi + 6*16+eax*4]
        movzx   edx,word [esi + 4*16+edx*2]
        movzx   ebx,word [esi + 2*16+ebx*2]
        movzx   ecx,word [esi + 0*16+ecx*2]
        mul     edx
        shrd    eax,edx,15
        mul     ebx
        shrd    eax,edx,15
        mul     ecx
        shrd    eax,edx,15
        pop     edi
        pop     esi
        pop     ecx
        pop     ebx
        pop     edx
        ret




PlayNote:
        mov     byte [ebp + GDS.portatmp],  0
        mov     byte [ebp + GDS.keyofftmp], 0
        cmp     byte [ebp + GDS.proccmd],   3
        jne     .PlayNote_noportac
        mov     byte [ebp + GDS.portatmp],  1
.PlayNote_noportac:

        cmp     byte [ebp + GDS.proccmd],   5
        jne     .PlayNote_noportacv
        mov     byte [ebp + GDS.portatmp],  1
.PlayNote_noportacv:

        cmp     byte [ebp + GDS.procvol],   0f0h
        jb      .PlayNote_noportav
        mov     byte [ebp + GDS.portatmp],  1
.PlayNote_noportav:

        cmp     byte [ebp + GDS.proccmd],   20
        jne     .PlayNote_nokeycmd
        cmp     byte [ebp + GDS.procdat],   0
        je      .PlayNote_dokeycmd
.PlayNote_nokeycmd:

        cmp     byte [ebp + GDS.procnot],   97
        jne     .PlayNote_nokeyoff
        mov     byte [ebp + GDS.procnot],   0

.PlayNote_dokeycmd:
        mov     byte [ebp + GDS.keyofftmp], 1
        mov     byte [edi + channel.chSustain],0
        ; if no instrument and no volenv. kill note

.PlayNote_nokeyoff:
        movzx   eax, byte [ebp + GDS.procins]
        cmp     al,0
        je      .PlayNote_noins1
        cmp     eax,[ebp + GDS.head + mxmheader.hdNInstruments]
        ja      .PlayNote_noins1
        mov     [edi + channel.chCurIns],al

.PlayNote_noins1:
        cmp     byte [edi + channel.chCurIns],0
        je      near .PlayNote_done

        movzx   eax, byte [ebp + GDS.procnot]
        cmp     al,0
        je      near .PlayNote_nonote
        cmp     byte [ebp + GDS.procins],0

        je      .PlayNote_nohit
        mov     byte [edi + channel.chSustain],1

.PlayNote_nohit:

        mov     byte [edi + channel.chDelayNote],al
        cmp     byte [ebp + GDS.proccmd],49
        jne     .PlayNote_nodelay
        cmp     byte [ebp + GDS.procdat],0
        jne     near .PlayNote_done
.PlayNote_nodelay:

        dec     al
        cmp     byte [ebp + GDS.portatmp],1
        je      near .PlayNote_portanote
        mov     byte [edi + channel.chMixStopIt],1

        movzx   edx, byte [edi + channel.chCurIns]

        mov     edx,[ebp + GDS.head + mxmheader.hdInstrTable +4*edx-4]
        movzx   ebx, byte [edx + instrument.insSamples + eax]
        cmp     ebx,[edx + instrument.insNSamples]
        jae     near .PlayNote_done
        shl     ebx,4
        lea     ebx,[ebx+edx+256]

        mov     byte [edi + channel.chMixInited],1
        mov     byte [edi + channel.chMixChangeSamp],1

        push    eax
        movzx   eax,word [ebx + sample.smpIndex]
        mov     eax,[ebp + GDS.head + mxmheader.hdSampleTable + 4*eax]
        mov     [edi + channel.chMixStartPos],eax
        mov     eax,[ebx + sample.smpLoopStart]
        mov     [edi + channel.chMixLoopPos],eax
        mov     eax,[ebx + sample.smpEnd]
        mov     [edi + channel.chMixEndPos],eax
        mov     al,[ebx + sample.smpMixMode]
        mov     [edi + channel.chMixMode],al
        pop     eax

        cmp     byte [ebp + GDS.procins],0
        je      .PlayNote_noins2
        mov     [edi + channel.chEnvIns],edx
        mov     dl,[ebx + sample.smpDefVol]
        mov     [edi + channel.chDefVol],dl
        mov     dl,[ebx + sample.smpDefPan]
        mov     [edi + channel.chDefPan],dl
.PlayNote_noins2:

        mov     dx,[ebx + sample.smpNormNote]
        mov     [edi + channel.chCurNormNote],dx

    ; process finetune here (proccmd==41)
    ; overwrite top 4 bits of instrument finetune value
    ; cannot do this correctly, since conversion reduced information... :(
    ; this command sucks anyway!!!

        shl     eax,8
        add     ax,dx
        neg     ax
        add     ah,48
        movsx   eax,ax
%if USEBOTHFREQ
        test    byte [ebp + GDS.head + mxmheader.hdOptions],1
        jnz     .PlayNote_noamiga1
%endif
%if USEAMIGAFREQ
        neg    eax
        call   getfreq6848
.PlayNote_noamiga1:
%endif
        mov     [edi + channel.chPitch],eax
        mov     [edi + channel.chFinalPitch],eax
        mov     [edi + channel.chPortaToPitch],eax

        xor     eax,eax
%if USEOFFSET
        cmp     byte [ebp + GDS.proccmd],9
        jne     .PlayNote_nooffset
        mov     al,[ebp + GDS.procdat]
        cmp     al,0
        je      .PlayNote_reuseoffset
        mov     [edi + channel.chOffset],al
.PlayNote_reuseoffset:
        movzx   eax,byte [edi + channel.chOffset]

        shl     eax,8
.PlayNote_nooffset:
%endif

        mov     [edi + channel.chMixNextPos],eax
        mov     byte [edi + channel.chVibPos],0
        mov     byte [edi + channel.chTremPos],0
        mov     byte [edi + channel.chArpPos],0
        mov     byte [edi + channel.chMRetrigPos],0
        mov     byte [edi + channel.chTremorPos],0
        jmp     .PlayNote_nonote

.PlayNote_portanote:
        shl     eax,8
        add     ax,[edi + channel.chCurNormNote]

        neg     ax
        add     ah,48
        movsx   eax,ax
%if USEBOTHFREQ
        test    byte [ebp + GDS.head + mxmheader.hdOptions],1
        jnz     .PlayNote_noamiga2
%endif
%if USEAMIGAFREQ
        neg     eax
        call    getfreq6848
.PlayNote_noamiga2:
%endif
        mov     [edi + channel.chPortaToPitch],eax
.PlayNote_nonote:

        cmp     byte [edi + channel.chSustain],0
        je      .PlayNote_killbug
        cmp     byte [ebp + GDS.procins],0
        je      .PlayNote_killbug
        cmp     byte [ebp + GDS.notedelayed],1
        je      .PlayNote_noinsvolpan
        mov     al,[edi + channel.chDefVol]

        mov     [edi + channel.chVol],al
        mov     [edi + channel.chFinalVol],al
        test    byte [ebp + GDS.head + mxmheader.hdOptions],2
        jnz     .PlayNote_noinsvolpan
        mov     al,[edi + channel.chDefPan]

        mov     [edi + channel.chPan],al
        mov     [edi + channel.chFinalPan],al
.PlayNote_noinsvolpan:
        xor     eax,eax
        mov     word [edi + channel.chFadeVol],8000h
        mov     [edi + channel.chAVibPos],al
        mov     [edi + channel.chAVibSwpPos],al
        mov     [edi + channel.chVolEnvPos],eax
        mov     [edi + channel.chVolEnvSegPos],ax
        mov     [edi + channel.chPanEnvPos],eax
        mov     [edi + channel.chPanEnvSegPos],ax

.PlayNote_killbug:
        cmp     byte [ebp + GDS.keyofftmp],0
        je      .PlayNote_done
        cmp     byte [ebp + GDS.procins],0
        jne     .PlayNote_done
        mov     ebx,[edi + channel.chEnvIns]

        cmp     byte [ebx + instrument.insVNum],0
        jne     .PlayNote_done
        mov     word [edi + channel.chFadeVol],0

.PlayNote_done:
        ret

; ===========================================================================
;  xmpSetVolume(int volume)
; ---------------------------------------------------------------------------
;  set global volume (stack call interface)
; ===========================================================================

global xmpSetVolume
xmpSetVolume:
        push    ebp
        mov     ebp,esp
        push    eax
        mov     eax,[ebp + 8]
        call    @@xmpSetVolume
        pop     eax
        pop     ebp
        ret

; ===========================================================================
;  @@xmpSetVolume(al: volume)
; ---------------------------------------------------------------------------
;  set global volume
; ===========================================================================

@@xmpSetVolume:
        push    ebp
        call    loadebp
        mov     [ebp + GDS.uservol],al
        pop     ebp
        ret

; ============================================================================
;  int xmpGetSync()
; ----------------------------------------------------------------------------
;  get current sync value
; ============================================================================

global xmpGetSync
xmpGetSync:
        push    ebp
        call    loadebp
        movzx   eax,byte [ebp + GDS.syncval]
        pop     ebp
        ret

; ============================================================================
;  void xmpSetSyncCallback(SyncCallback proc)
; ----------------------------------------------------------------------------
;  set callback for sync effect 
; ============================================================================

global xmpSetSyncCallback
xmpSetSyncCallback:
        push    ebp
        call    loadebp
        mov     eax,[esp + 8]
        mov     [ebp + GDS.synccb],eax
        pop     ebp
        ret

; ============================================================================
;  int xmpGetPos()
; ----------------------------------------------------------------------------
;  get current row and order
; ============================================================================

global xmpGetPos
global _xmpGetPos
xmpGetPos:
_xmpGetPos:
        push    ebp
        call    loadebp
        xor     eax,eax
        mov     al,byte [ebp + GDS.currow]
        mov     ah,byte [ebp + GDS.curord]
        pop     ebp
        ret

freqrange:
        cmp     eax,[ebp + GDS.head+mxmheader.hdPitchMin]
        jg      .lowlimok
        mov     eax,[ebp + GDS.head+mxmheader.hdPitchMin]
.lowlimok:
        cmp     eax,[ebp + GDS.head+mxmheader.hdPitchMax]
        jl      .highlimok
        mov     eax,[ebp + GDS.head+mxmheader.hdPitchMax]
.highlimok:
        ret

; ****************************************************************************
;  Effects
; ****************************************************************************

procnothing:
        ret

%if USEJUMP
procjump:
        movzx   eax, byte [ebp + GDS.procdat]
        mov     [ebp + GDS.jumptoord],eax
        mov     dword [ebp + GDS.jumptorow],0
        ret
%else
procjump equ procnothing
%endif


%if USEBREAK
procbreak:
        cmp     dword [ebp + GDS.jumptoord],-1
        jne     .onlyrow
        mov     eax, [ebp + GDS.curord]
        inc     eax
        mov     [ebp + GDS.jumptoord],eax
.onlyrow:
        movzx   eax,byte [ebp + GDS.procdat]
        mov     ebx,eax
        shr     al,4
        imul    eax,10
        and     bl,0fh
        add     eax,ebx
        mov     [ebp + GDS.jumptorow],eax
        ret
%else
procbreak equ procnothing
%endif


%if USEPATLOOP
procpatloop:
        mov     al,[ebp + GDS.procdat]
        cmp     al,0
        je      .set
        inc     byte [edi + channel.chPatLoopCount]

        cmp     [edi + channel.chPatLoopCount],al
        ja      .nextrow
        movzx   eax, byte [edi + channel.chPatLoopStart]

        mov     [ebp + GDS.jumptorow],eax
        mov     eax,[ebp + GDS.curord]
        mov     [ebp + GDS.jumptoord],eax
        jmp     .done
.nextrow:
        mov     byte [edi + channel.chPatLoopCount],0
        mov     al,byte [ebp + GDS.currow]
        inc     al
        mov     [edi + channel.chPatLoopStart],al
        jmp     .done
.set:
        mov     al,byte [ebp + GDS.currow]
        mov     [edi + channel.chPatLoopStart],al
.done:
        ret
%else
procpatloop equ procnothing
%endif


%if USEPATDELAY
procpatdelay:
        mov     al,[ebp + GDS.procdat]
        mov     [ebp + GDS.patdelay],al
        ret
%else
procpatdelay equ procnothing
%endif


;%if USESPEED
proctempo:
        movzx   ebx,byte[ebp + GDS.procdat]
        cmp     bl,20h
        jb      .proctempo_speed
.tempo:
%if USECOMPATIBLEMODE
        mov     eax,44100
        test    byte [ebp+GDS.mixflags],1       ; use frequency 22050
        jz      .nodownsample
        shr     eax,1
.nodownsample
        xor     edx,edx
        imul    eax,5*256
%else
        mov     eax,44100*5*256
        xor     edx,edx
%endif
        shl     ebx,1
        idiv    ebx
        mov     [ebp+GDS.stimerlen],eax
        ret
.proctempo_speed:
        cmp     bl,0
        je      .proctempo_ignore
        mov     [ebp + GDS.curtempo],bl
.proctempo_ignore:
        ret
;%else
;proctempo equ procnothing
;%endif


%if USEVOL
procnvol:
    	mov     al,[ebp + GDS.procdat]
    	cmp     al,40h
    	jbe     .procnvol_vok
    	mov     al,40h
.procnvol_vok:
    	mov     [edi + channel.chVol],al
    	mov     [edi + channel.chFinalVol],al
    	ret
%else
procnvol equ procnothing
%endif


%if USEGVOL
procgvol:
    mov     al,[ebp + GDS.procdat]
    cmp     al,40h
    jbe     .procgvol_vok
    mov     al,40h
.procgvol_vok:
    mov     [ebp + GDS.globalvol],al
    ret
%else
procgvol equ procnothing
%endif


%if USEPAN
procpan:
    	mov     al, [ebp + GDS.procdat]
    	mov     [edi + channel.chPan], al
    	mov     [edi + channel.chFinalPan], al
    	ret
%else
procpan equ procnothing
%endif


%if USESPAN
procspan:
    	mov     al,[ebp + GDS.procdat]
    	shl     al,4
    	or      al,[ebp + GDS.procdat]
    	mov     [edi + channel.chPan],al
    	mov     [edi + channel.chFinalPan],al
    	ret
%else
procspan equ procnothing
%endif


%if USEVPAN
procvpan:
    	mov     al,[ebp + GDS.procvol]
    	shl     al,4
    	or      al,[ebp + GDS.procvol]
    	mov     [edi + channel.chPan],al
    	mov     [edi + channel.chFinalPan],al
    	ret
%else
procvpan equ procnothing
%endif


%if USEARPEGGIO
procarpeggio:
    movzx   eax,byte [ebp + GDS.procdat]
    cmp     al,0
    jne     .procarpeggio_doit
    mov     byte [edi + channel.chCommand],0ffh
.procarpeggio_doit:
    shl     eax,4
    shr     al,4
    mov     byte [edi + channel.chArpNotes + 0],0
    mov     [edi + channel.chArpNotes + 1],ah
    mov     [edi + channel.chArpNotes + 2],al
    ret

doarpeggio:
    movzx   eax, byte [edi + channel.chArpPos]
    mov     al,[edi + channel.chArpNotes + eax]

%if USEBOTHFREQ
    test    byte [ebp + GDS.head + mxmheader.hdOptions],1
    jz      .doarpeggio_amiga
%endif
%if USELINEARFREQ
    shl     eax,8
    neg     eax
    add     eax,[edi + channel.chFinalPitch]

    call    freqrange
    mov     [edi + channel.chFinalPitch],eax
%endif
%if USEBOTHFREQ
    jmp     .doarpeggio_noamiga
%endif
%if USEAMIGAFREQ
.doarpeggio_amiga:
    call    .doarpeggio_getadr
  .doarpeggio_getadr:
    pop     edx
    mov     ax,[edx+eax*2+16*4+(logfreqtab-.doarpeggio_getadr)]
    mul     dword [edi + channel.chFinalPitch]

    shrd    eax,edx,15
    call    freqrange
    mov     [edi + channel.chFinalPitch],eax
%endif

.doarpeggio_noamiga:
    inc     byte [edi + channel.chArpPos]

    cmp     byte [edi + channel.chArpPos],3
    jne     .doarpeggio_done
    mov     byte [edi + channel.chArpPos],0
.doarpeggio_done:
    ret

%else
procarpeggio equ procnothing
doarpeggio equ procnothing
%endif



%if (USETREMTYPE && USETREMOLO)
proctremtype:
    	mov     al,[ebp + GDS.procdat]
    	and     al,3
    	mov     [edi + channel.chTremType],al
    	ret
%else
proctremtype equ procnothing
%endif



%if USETREMOLO
proctremolo:
    mov     al,[ebp + GDS.procdat]
    and     al,0Fh
    jz      .proctremolo_reusel
    shl     al,2
    mov     [edi + channel.chTremDep],al
.proctremolo_reusel:
    mov     al,[ebp + GDS.procdat]
    and     al,0F0h
    jz      .proctremolo_reuseh
    shr     al,2
    mov     [edi + channel.chTremRate],al
.proctremolo_reuseh:
    ret

dotremolo:
    movzx   eax,byte [edi + channel.chTremPos]

    movsx   eax,byte [ebp + GDS.vibtabs + eax]
    imul    byte [edi + channel.chTremDep]

    sar     eax,6
    add     al,[edi + channel.chFinalVol]

    jns     .dotremolo_lok
    xor     al,al
.dotremolo_lok:
    cmp     al,40h
    jbe     .dotremolo_tok
    mov     al,40h
.dotremolo_tok:
    mov     [edi + channel.chFinalVol],al

    cmp     byte [ebp + GDS.tick0],0
    jne     .dotremolo_done
    mov     al,[edi + channel.chTremRate]

    add     [edi + channel.chTremPos],al
.dotremolo_done:
    ret
%else
proctremolo equ procnothing
dotremolo equ procnothing
%endif


%if USEFPORTA
procfportau:
    mov     al,[ebp + GDS.procdat]
    cmp     al,0
    je      .procfportau_reuse
    mov     [edi + channel.chFinePortaUVal],al
.procfportau_reuse:
    movzx   eax,byte [edi + channel.chFinePortaUVal]

    shl     eax,4
    neg     eax
    add     eax,[edi + channel.chPitch]

    call    freqrange
    mov     [edi + channel.chPitch],eax
    mov     [edi + channel.chFinalPitch],eax
    ret

procfportad:
    mov     al,[ebp + GDS.procdat]
    cmp     al,0
    je      .procfportad_reuse
    mov     [edi + channel.chFinePortaDVal],al
.procfportad_reuse:
    movzx   eax,byte [edi + channel.chFinePortaDVal]

    shl     eax,4
    add     eax,[edi + channel.chPitch]

    call    freqrange
    mov     [edi + channel.chPitch],eax
    mov     [edi + channel.chFinalPitch],eax
    ret
%else
procfportau equ procnothing
procfportad equ procnothing
%endif


%if USEXFPORTA
procxfporta:
    movzx   eax,byte [ebp + GDS.procdat]
    shl     eax,4
    shr     al,4
    cmp     ah,2
    je      .procxfporta_down
    cmp     ah,1
    jne     .procxfporta_done

    cmp     al,0
    je      .procxfporta_reuseu
    mov     [edi + channel.chXFinePortaUVal],al
.procxfporta_reuseu:
    movzx   eax,byte [edi + channel.chXFinePortaUVal]

    shl     eax,2
    neg     eax
    add     eax,[edi + channel.chPitch]

    call    freqrange
    mov     [edi + channel.chPitch],eax
    mov     [edi + channel.chFinalPitch],eax
    jmp     .procxfporta_done

.procxfporta_down:
    cmp     al,0
    je      .procxfporta_reused
    mov     [edi + channel.chXFinePortaDVal],al
.procxfporta_reused:
    movzx   eax,byte [edi + channel.chXFinePortaDVal]

    shl     eax,2
    add     eax,[edi + channel.chPitch]

    call    freqrange
    mov     [edi + channel.chPitch],eax
    mov     [edi + channel.chFinalPitch],eax

.procxfporta_done:
    ret
%else
procxfporta equ procnothing
%endif


%if USEFVOLSLIDE
procfvolup:
    mov     al,[ebp + GDS.procdat]
    cmp     al,0
    je      .procfvolup_reuse
    mov     [edi + channel.chFineVolSlideUVal],al
.procfvolup_reuse:
    mov     al,[edi + channel.chVol]

    add     al,[edi + channel.chFineVolSlideUVal]

    cmp     al,40h
    jbe     .procfvolup_vok
    mov     al,40h
.procfvolup_vok:
    mov     [edi + channel.chVol],al
    mov     [edi + channel.chFinalVol],al
    ret

procfvoldn:
    mov     al,[ebp + GDS.procdat]
    cmp     al,0
    je      .procfvoldn_reuse
    mov     [edi + channel.chFineVolSlideDVal],al
.procfvoldn_reuse:
    mov     al,[edi + channel.chVol]
    sub     al,[edi + channel.chFineVolSlideDVal]
    jnc     .procfvoldn_vok
    mov     al,0
.procfvoldn_vok:
    mov     [edi + channel.chVol],al
    mov     [edi + channel.chFinalVol],al
    ret
%else
procfvolup equ procnothing
procfvoldn equ procnothing
%endif



%if USEVVOL
procvvol:
procvvol4:
    mov     byte [ebp + GDS.procvol],10h
procvvol3:
    add     byte [ebp + GDS.procvol],10h
procvvol2:
    add     byte [ebp + GDS.procvol],10h
procvvol1:
    add     byte [ebp + GDS.procvol],10h
procvvol0:
    mov     al,[ebp + GDS.procvol]
    mov     [edi + channel.chVol],al
    mov     [edi + channel.chFinalVol],al
    ret
%else
procvvol0 equ procnothing
procvvol1 equ procnothing
procvvol2 equ procnothing
procvvol3 equ procnothing
procvvol4 equ procnothing
%endif



%if (USEVPANSLIDE || USEVVOLSLIDE)
procvvpsl:
    mov     al,[ebp + GDS.procvol]
    mov     [edi + channel.chVVolPanSlideVal],al
    ret
%else
procvvpsl equ procnothing
%endif



%if USEVVOLSLIDE
dovvolsld:
    mov     al,[edi + channel.chVol]

    cmp     byte [ebp + GDS.tick0],0
    jne     .dovvolsld_done
    sub     al,[edi + channel.chVVolPanSlideVal]

    jnc     .dovvolsld_done
    mov     al,0
.dovvolsld_done:
    mov     [edi + channel.chVol],al
    mov     [edi + channel.chFinalVol],al
    ret

dovvolslu:
    mov     al,[edi + channel.chVol]

    cmp     byte [ebp + GDS.tick0],0
    jne     .dovvolslu_done
    add     al,[edi + channel.chVVolPanSlideVal]

    cmp     al,40h
    jbe     .dovvolslu_done
    mov     al,40h
.dovvolslu_done:
    mov     [edi + channel.chVol],al
    mov     [edi + channel.chFinalVol],al
    ret
%else
dovvolsld equ procnothing
dovvolslu equ procnothing
%endif


%if USEVPANSLIDE
dovpansll:
    mov     al,[edi + channel.chPan]
    cmp     byte [ebp + GDS.tick0],0
    jne     .dovpansll_done
    sub     al,[edi + channel.chVVolPanSlideVal]
    jnc     .dovpansll_done
    mov     al,0
.dovpansll_done:
    mov     [edi + channel.chPan],al
    mov     [edi + channel.chFinalPan],al
    ret

dovpanslr:
    mov     al,[edi + channel.chVol]

    cmp     byte [ebp + GDS.tick0],0
    jne     .dovpanslr_done
    add     al,[edi + channel.chVVolPanSlideVal]

    jnc     .dovpanslr_done
    mov     al,0ffh
.dovpanslr_done:
    mov     [edi + channel.chPan],al
    mov     [edi + channel.chFinalPan],al
    ret
%else
dovpansll equ procnothing
dovpanslr equ procnothing
%endif


%if USEVFVOLSLIDE
procvfvolup:
    mov     al,[edi + channel.chVol]

    add     al,[ebp + GDS.procvol]
    cmp     al,40h
    jbe     .procvfvolup_vok
    mov     al,40h
.procvfvolup_vok:
    mov     [edi + channel.chVol],al
    mov     [edi + channel.chFinalVol],al
    ret

procvfvoldn:
    mov     al,[edi + channel.chVol]

    sub     al,[ebp + GDS.procvol]
    jnc     .procvfvoldn_vok
    mov     al,0
.procvfvoldn_vok:
    mov     [edi + channel.chVol],al
    mov     [edi + channel.chFinalVol],al
    ret
%else
procvfvolup equ procnothing
procvfvoldn equ procnothing
%endif


%if USESYNC
procsync:
    ; set sync value
    mov     al,[ebp + GDS.procdat]
    mov     [ebp + GDS.syncval],al
    ; call user's callback 
    mov	    ebx, [ebp + GDS.synccb]
    or	    ebx,ebx
    jz      .exit
    pushad
    push    eax    
    call    ebx    
    add	    esp, 4
    popad    
.exit:  
    ret
%else
procsync equ procnothing
%endif


%if USETREMOR
proctremor:
    movzx   eax, byte [ebp + GDS.procdat]
    cmp     al,0
    je      .proctremor_reuse
    shl     eax,4
    shr     al,4
    inc     al
    inc     ah
    add     al,ah
    mov     [edi + channel.chTremorLen],al
    mov     [edi + channel.chTremorOff],ah
    mov     byte [edi + channel.chTremorPos],0
.proctremor_reuse:
    ret

dotremor:
    mov     al,[edi + channel.chTremorPos]
    cmp     al,[edi + channel.chTremorOff]
    jb      .dotremor_on
    mov     byte [edi + channel.chFinalVol],0
.dotremor_on:
    cmp     byte [ebp + GDS.tick0],0
    jne     .dotremor_done
    mov     al,[edi + channel.chTremorPos]
    inc     al
    cmp     al,[edi + channel.chTremorLen]
    jb      .dotremor_noloop
    xor     al,al
.dotremor_noloop:
    mov     [edi + channel.chTremorPos],al
.dotremor_done:
    ret
%else
proctremor equ procnothing
dotremor equ procnothing
%endif



%if USEENVPOS
procenvpos:
    cmp     dword [edi + channel.chEnvIns],0
    je      .procenvpos_noenvins
    mov     ebx,[edi + channel.chEnvIns]
    xor     eax,eax
    movzx   edx, byte [ebp + GDS.procdat]
    jmp     .procenvpos_venvloops
.procenvpos_venvloop:
    sub     dx,[ebx+instrument.insVEnv + 4*eax]
    jb      .procenvpos_venvok
    inc     eax
.procenvpos_venvloops:
    cmp     al,[ebx + instrument.insVNum]
    jne     .procenvpos_venvloop
    xor     edx,edx
    sub     dx,[ebx+instrument.insVEnv+4*eax]
.procenvpos_venvok:
    add     dx,[ebx + instrument.insVEnv +4*eax]
    mov     [edi + channel.chVolEnvPos],eax
    mov     [edi + channel.chVolEnvSegPos],dx
    xor     eax,eax
    movzx   edx,byte [ebp + GDS.procdat]
    jmp     .procenvpos_penvloops
.procenvpos_penvloop:
    sub     dx,[ebx+instrument.insPEnv + 4*eax]
    jb      .procenvpos_penvok
    inc     eax
.procenvpos_penvloops:
    cmp     al,[ebx + instrument.insPNum]
    jne     .procenvpos_penvloop
    xor     edx,edx
    sub     dx,[ebx+instrument.insPEnv + 4*eax]
.procenvpos_penvok:
    add     dx,[ebx+instrument.insPEnv + 4*eax]
    mov     [edi + channel.chPanEnvPos],eax
    mov     [edi + channel.chPanEnvSegPos],dx
.procenvpos_noenvins:
    ret
%else
procenvpos equ procnothing
%endif




%if USEPORTA
procportau:
    movzx   eax,byte [ebp + GDS.procdat]
    cmp     al,0
    je      .procportau_reuse
    shl     eax,4
    mov     [edi + channel.chPortaUVal],eax
.procportau_reuse:
    ret

procportad:
    movzx   eax,byte [ebp + GDS.procdat]
    cmp     al,0
    je      .procportad_reuse
    shl     eax,4
    mov     [edi + channel.chPortaDVal],eax
.procportad_reuse:
    ret

doportau:
    cmp     byte [ebp + GDS.tick0],0
    jne     .doportau_done
    mov     eax,[edi + channel.chPitch]
    sub     eax,[edi + channel.chPortaUVal]
    call    freqrange
    mov     [edi + channel.chPitch],eax
    mov     [edi + channel.chFinalPitch],eax
.doportau_done:
    ret

doportad:
    cmp     byte [ebp + GDS.tick0],0
    jne     .doportad_done
    mov     eax,[edi + channel.chPitch]
    add     eax,[edi + channel.chPortaDVal]
    call    freqrange
    mov     [edi + channel.chPitch],eax
    mov     [edi + channel.chFinalPitch],eax
.doportad_done:
    ret
%else
procportau equ procnothing
procportad equ procnothing
doportau equ procnothing
doportad equ procnothing
%endif


%if USEPORTANOTE
procportanote:
    movzx   eax,byte [ebp + GDS.procdat]
    cmp     al,0
    je      .procportanote_reuse
    shl     eax,4
    mov     [edi + channel.chPortaToVal],eax
.procportanote_reuse:
    ret
%else
procportanote equ procnothing
%endif


%if USEVPORTANOTE
procvportanote:
    movzx   eax,byte [ebp + GDS.procvol]
    cmp     al,0
    je      .procvportanote_reuse
    shl     eax,8
    mov     [edi + channel.chPortaToVal],eax
.procvportanote_reuse:
    ret
%else
procvportanote equ procnothing
%endif


%if (USEGLISSANDO && (USEPORTANOTE || USEVPORTANOTE))
procgliss:
    mov     al,[ebp + GDS.procdat]
    mov     [edi + channel.chGlissando],al
    ret
%else
procgliss equ procnothing
%endif


%if (USEPORTANOTE || USEVPORTANOTE)
doportanote:
    mov     eax,[edi + channel.chPitch]

    cmp     byte [ebp + GDS.tick0],0
    jne     .doportanote_set
    cmp     eax,[edi + channel.chPortaToPitch]

    je      .doportanote_set
    jg      .doportanote_down
    add     eax,[edi + channel.chPortaToVal]
    cmp     eax,[edi + channel.chPortaToPitch]
    jle     .doportanote_set
    mov     eax,[edi + channel.chPortaToPitch]

    jmp     .doportanote_set
.doportanote_down:
    sub     eax,[edi + channel.chPortaToVal]
    cmp     eax,[edi + channel.chPortaToPitch]
    jge     .doportanote_set
    mov     eax,[edi + channel.chPortaToPitch]

.doportanote_set:
    mov     [edi + channel.chPitch],eax

%if USEGLISSANDO
    cmp     byte [edi + channel.chGlissando],0
    je      .doportanote_setfinpitch
%if USEBOTHFREQ
    test    byte [ebp + GDS.head + mxmheader.hdOptions],1
    jz      .doportanote_amiga
%endif



%if USELINEARFREQ
    movzx   ebx,word [edi + channel.chCurNormNote]

    add     eax,ebx
    add     eax,80h
    xor     al,al
    sub     eax,ebx
%endif
%if USEBOTHFREQ
    jmp     .doportanote_setfinpitch
%endif
%if USEAMIGAFREQ
.doportanote_amiga:
    mov     edx,eax ;// search for closest note
    mov     ebx,eax ;// how should i do it??
    push    ecx
    mov     ecx,-1
    mov     eax,-48*256
.doportanote_aloop:
    push    eax
    add     ax,[edi + channel.chCurNormNote]

    movsx   eax,ax
    call    getfreq6848
    sub     eax,edx
    jae     .doportanote_apos
    neg     eax
.doportanote_apos:
    cmp     eax,ecx
    jae     .doportanote_aold
    mov     ecx,eax
    mov     eax,[esp]
    add     ax,[edi + channel.chCurNormNote]

    movsx   eax,ax
    call    getfreq6848
    mov     ebx,eax
.doportanote_aold:
    pop     eax
    inc     ah
    cmp     ah,48
    jne     .doportanote_aloop
    pop     ecx
    mov     eax,ebx
%endif
%endif

.doportanote_setfinpitch:
    mov     [edi + channel.chFinalPitch],eax
.doportanote_done:
    ret
%else
doportanote equ procnothing
%endif


%if (USEVIBTYPE && (USEVIBRATO || USEVVIBRATO))
procvibtype:
    mov     al,[ebp + GDS.procdat]
    and     al,3
    mov     [edi + channel.chVibType], al
    ret
%else
procvibtype equ procnothing
%endif


%if USEVIBRATO
procvibrato:
    mov     al,[ebp + GDS.procdat]
    and     al,0Fh
    jz      .procvibrato_reusel
    shl     al,2
    mov     [edi + channel.chVibDep],al
.procvibrato_reusel:
    mov     al,[ebp + GDS.procdat]
    and     al,0F0h
    jz      .procvibrato_reuseh
    shr     al,2
    mov     [edi + channel.chVibRate],al
.procvibrato_reuseh:
    ret
%else
procvibrato equ procnothing
%endif


%if USEVVIBRATE
procvvibrat:
    mov     al,[ebp + GDS.procvol]
    shl     al,2
    je      .procvvibrat_reuse
    mov     [edi + channel.chVibRate],al
.procvvibrat_reuse:
    ret
%else
procvvibrat equ procnothing
%endif


%if USEVVIBRATO
procvvib:
    mov al,[ebp + GDS.procvol]
    shl al,2
    je .procvvib_reuse
    mov [edi + channel.chVibDep],al
.procvvib_reuse:
    ret
%else
    procvvib equ procnothing
%endif


%if (USEVIBRATO || USEVVIBRATO)
dovibrato:
    movzx   eax,byte [edi + channel.chVibPos]

    movsx   eax,byte [ebp + GDS.vibtabs + eax]
    imul    byte [edi + channel.chVibDep]

    sar     eax,3
    add     eax,[edi + channel.chFinalPitch]

    call    freqrange
    mov     [edi + channel.chFinalPitch],eax

    cmp     byte [ebp + GDS.tick0],0
    jne     .dovibrato_done
    mov     al,[edi + channel.chVibRate]
    shr     al, 1

    add     [edi + channel.chVibPos],al
.dovibrato_done:
    ret
%else
    dovibrato equ procnothing
%endif


%if (USEVOLSLIDE || USEVIBRATOVOL || USEPORTAVOL)
procvolsl:
    mov     al,[ebp + GDS.procdat]
    cmp     al,0
    je      .procvolsl_reuse
    mov     [edi + channel.chVolSlideVal],al
.procvolsl_reuse:
    ret

dovolsl:
    mov     bl,[edi + channel.chVolSlideVal]
    mov     al,[edi + channel.chVol]
    cmp     byte [ebp + GDS.tick0],0
    jne     .dovolsl_done
    test    bl,0f0h
    jnz     .dovolsl_up
    sub     al,bl
    jnc     .dovolsl_done
    mov     al,0
    jmp     .dovolsl_done
.dovolsl_up:
    shr     bl,4
    add     al,bl
    cmp     al,40h
    jbe     .dovolsl_done
    mov     al,40h
.dovolsl_done:
    mov     [edi + channel.chVol],al
    mov     [edi + channel.chFinalVol],al
    ret
%else
procvolsl equ procnothing
dovolsl equ procnothing
%endif



%if (USEVIBRATOVOL && USEVIBRATO)
dovibvol:
    call    dovibrato
    jmp     dovolsl
%else
dovibvol equ procnothing
%endif


%if (USEPORTAVOL && USEPORTANOTE)
doportavol:
    call    doportanote
    jmp     dovolsl
%else
doportavol equ procnothing
%endif


%if USEGVOLSLIDE
procgvolsl:
    mov     al,[ebp + GDS.procdat]
    cmp     al,0
    je      .procgvolsl_reuse
    mov     [edi + channel.chGVolSlideVal],al
.procgvolsl_reuse:
    ret

dogvolsl:
    mov     bl,[edi + channel.chGVolSlideVal]
    mov     al,[ebp + GDS.globalvol]
    cmp     byte [ebp + GDS.tick0],0
    jne     .dogvolsl_done
    test    bl,0f0h
    jnz     .dogvolsl_up
    sub     al,bl
    jnc     .dogvolsl_done
    mov     al,0
    jmp     .dogvolsl_done
.dogvolsl_up:
    shr     bl,4
    add     al,bl
    cmp     al,40h
    jbe     .dogvolsl_done
    mov     al,40h
.dogvolsl_done:
    mov     [ebp + GDS.globalvol],al
    ret
%else
procgvolsl equ procnothing
dogvolsl equ procnothing
%endif


%if USEPANSLIDE
procpansl:
    mov     al,[ebp + GDS.procdat]
    cmp     al,0
    je      .procpansl_reuse
    mov     [edi + channel.chPanSlideVal],al
.procpansl_reuse:
    ret

dopansl:
    mov     bl,[edi + channel.chPanSlideVal]
    mov     al,[edi + channel.chPan]
    cmp     byte [ebp + GDS.tick0],0
    jne     .dopansl_done
    test    bl,0f0h
    jnz     .dopansl_left
    add     al,bl
    jnc     .dopansl_done
    mov     al,0ffh
    jmp     .dopansl_done
.dopansl_left:
    shr     bl,4
    sub     al,bl
    jnc     .dopansl_done
    mov     al,0
.dopansl_done:
    mov     [edi + channel.chPan],al
    mov     [edi + channel.chFinalPan],al
    ret
%else
procpansl equ procnothing
dopansl equ procnothing
%endif


%if (USEDELAY || USEKEYOFFCMD || USENOTECUT)
proctick:
    mov     al,[ebp + GDS.procdat]
    mov     [edi + channel.chActionTick],al
    ret
%else
proctick equ procnothing
%endif


%if USEDELAY
dodelay:
    cmp     byte [ebp + GDS.tick0],0
    jne     .dodelay_done
    mov     al,[ebp + GDS.curtick]
    cmp     al,[edi + channel.chActionTick]
    jne     .dodelay_done

    mov     byte [ebp + GDS.notedelayed],1
    mov     al,[edi + channel.chDelayNote]

    mov     [ebp + GDS.procnot],al
    mov     al,[edi + channel.chCurIns]

    mov     [ebp + GDS.procins],al
    mov     byte [ebp + GDS.proccmd],0ffh
    mov     byte [ebp + GDS.procvol],0
    call    PlayNote
.dodelay_done:
    ret
%else
dodelay equ procnothing
%endif


%if USEKEYOFFCMD
dokeyoff:
    cmp     byte [ebp + GDS.tick0],0
    jne     .dokeyoff_done
    mov     al,[ebp + GDS.curtick]
    cmp     al,[edi + channel.chActionTick]
    jne     .dokeyoff_done
    mov     byte [edi + channel.chSustain],0
    mov     eax,[edi + channel.chEnvIns]
    cmp     eax,0
    je      .dokeyoff_done
    cmp     byte [eax + instrument.insVNum],0
    jne     .dokeyoff_done
    mov     word [edi + channel.chFadeVol],0
.dokeyoff_done:
    ret
%else
dokeyoff equ procnothing
%endif


%if USENOTECUT
donotecut:
    cmp     byte [ebp + GDS.tick0],0
    jne     .donotecut_done
    mov     al,[ebp + GDS.curtick]
    cmp     al,[edi + channel.chActionTick]

    jne     .donotecut_done
    mov     byte [edi + channel.chVol],0
    mov     byte [edi + channel.chFinalVol],0
.donotecut_done:
    ret
%else
donotecut equ procnothing
%endif


%if USERETRIG
doretrig:
    cmp     byte [edi + channel.chActionTick],0
    je      .doretrig_done
    movzx   eax,byte [ebp + GDS.curtick]
    xor     edx, edx
    div     byte [edi + channel.chActionTick]

    cmp     ah,0
    jne     .doretrig_done
    mov     dword [edi + channel.chMixNextPos],0
.doretrig_done:
    ret
%else
    doretrig equ procnothing
%endif


%if USEMRETRIG
procmretrig:
    movzx   eax,byte [ebp + GDS.procdat]
    cmp     al,0
    je      .procmretrig_reuse
    shl     eax,4
    shr     al,4
    mov     [edi + channel.chMRetrigLen],al
    mov     [edi + channel.chMRetrigAct],ah
    mov     byte [edi + channel.chMRetrigPos],0
.procmretrig_reuse:
    ret

domretrig:
    mov     al,[edi + channel.chMRetrigPos]
    inc     byte [edi + channel.chMRetrigPos]
    cmp     al,[edi + channel.chMRetrigLen]
    jne     .domretrig_done
    mov     byte [edi + channel.chMRetrigPos],0
    mov     dword [edi + channel.chMixNextPos],0
    mov     al,[edi + channel.chVol]
    mov     bl,[edi + channel.chMRetrigAct]

    mov     ah,128
    xchg    bl,cl
    rol     ah,cl
    xchg    bl,cl
    test    bl,7
    jz      .domretrig_done
    test    bl,8
    jnz     .domretrig_up
    cmp     bl,5
    ja      .domretrig_nosub
    sub     al,ah
.domretrig_nosub:
    cmp     bl,6
    jne     .domretrig_not6
    mov     ah,al
    shr     al,2
    add     al,ah
    shr     al,1
.domretrig_not6:
    cmp     bl,7
    jne     .domretrig_setvol
    shr     al,1
    jmp     .domretrig_setvol

.domretrig_up:
    cmp     bl,13
    ja      .domretrig_noadd
    add     al,ah
.domretrig_noadd:
    cmp     bl,14
    jne     .domretrig_not14
    mov     ah,al
    shr     al,1
    add     al,ah
.domretrig_not14:
    cmp     bl,15
    jne     .domretrig_setvol
    shl     al,1
    jns     .domretrig_setvol
    dec     al

.domretrig_setvol:
    cmp     al,0
    jge     .domretrig_lok
    mov     al,0
.domretrig_lok:
    cmp     al,40h
    jbe     .domretrig_tok
    mov     al,40h
.domretrig_tok:
    mov     [edi + channel.chVol],al
    mov     [edi + channel.chFinalVol],al
.domretrig_done:
    ret
%else
procmretrig equ procnothing
domretrig equ procnothing
%endif

; ****************************************************************************
;  effects end
; ****************************************************************************

callproccmdtab:
    call    .callproccmdtab_getadr
.callproccmdtab_getadr:
    pop     ebx
    mov     eax,[ebx+4*eax+(proccmdtab-.callproccmdtab_getadr)]
    lea     eax,[eax+ebx+(procnothing-.callproccmdtab_getadr)]
    jmp     eax


PlayTick:
    mov     byte [ebp + GDS.tick0],0
    lea     edi,[ebp + GDS.chandata]
    xor     ecx,ecx
.PlayTick_resetvalloop:
    mov     al,[edi + channel.chVol]

    mov     [edi + channel.chFinalVol],al
    mov     al,[edi + channel.chPan]

    mov     [edi + channel.chFinalPan],al
    mov     eax,[edi + channel.chPitch]

    mov     [edi + channel.chFinalPitch],eax
    add     edi,channelsize
    inc     ecx
    cmp     ecx,[ebp + GDS.head + mxmheader.hdNChannels]
    jne     .PlayTick_resetvalloop

    inc     byte [ebp + GDS.curtick]
    mov     al,[ebp + GDS.curtick]
    cmp     al,[ebp + GDS.curtempo]
    jne     near .PlayTick_notnextrow

    mov     byte [ebp + GDS.curtick],0
    cmp     byte [ebp + GDS.patdelay],0
    jz      .PlayTick_nextrow
    dec     byte [ebp + GDS.patdelay]
    jmp     .PlayTick_notnextrow
.PlayTick_nextrow:
    mov     byte [ebp + GDS.tick0],1

    inc     dword [ebp + GDS.currow]
    cmp     dword [ebp + GDS.jumptoord],-1
    jne     .PlayTick_dojump
    mov     eax,[ebp + GDS.currow]
    cmp     eax,[ebp + GDS.patlen]
    jb      near .PlayTick_donotjump
    mov     eax,[ebp + GDS.curord]
    inc     eax
    mov     [ebp + GDS.jumptoord],eax
    mov     dword [ebp + GDS.jumptorow],0
.PlayTick_dojump:

    mov     eax,[ebp + GDS.jumptoord]
    cmp     [ebp + GDS.curord],eax
    je      .PlayTick_noresetploop
    lea     edi,[ebp + GDS.chandata]
    xor     ecx,ecx
.PlayTick_resetplloop:
    mov     byte [edi + channel.chPatLoopCount],0
    mov     byte [edi + channel.chPatLoopStart],0
    add     edi,channelsize
    inc     ecx
    cmp     ecx,[ebp + GDS.head + mxmheader.hdNChannels]
    jne     .PlayTick_resetplloop
.PlayTick_noresetploop:

    mov     eax,[ebp + GDS.jumptoord]
    cmp     eax,[ebp + GDS.head + mxmheader.hdNOrders]
    jb      .PlayTick_dontloop
    mov     eax,[ebp + GDS.head + mxmheader.hdOrdLoopStart]
.PlayTick_dontloop:
    mov     [ebp + GDS.curord],eax
    mov     eax,[ebp + GDS.jumptorow]
    mov     [ebp + GDS.currow],eax
    mov     dword [ebp + GDS.jumptoord],-1
    mov     eax,[ebp + GDS.curord]
    movzx   eax,byte [ebp + GDS.head + mxmheader.hdOrderTable + eax]
    mov     esi,[ebp + GDS.head + mxmheader.hdPatternTable + 4*eax]
    lodsd
    mov     [ebp + GDS.patlen],eax
    cmp     dword [ebp + GDS.jumptorow],0
    je      .PlayTick_rowfound
.PlayTick_rowfind:
.PlayTick_chanskip:
    lodsb
    cmp     al,0
    je      .PlayTick_rowend

    test    al,20h
    jz      .PlayTick_not20
    add     esi,2
.PlayTick_not20:
    test    al,40h
    jz      .PlayTick_not40
    inc     esi
.PlayTick_not40:
    test    al,80h
    jz      .PlayTick_chanskip
    add     esi,2
    jmp     .PlayTick_chanskip
.PlayTick_rowend:
    dec     dword [ebp + GDS.jumptorow]
    jnz     .PlayTick_rowfind
.PlayTick_rowfound:
    mov     [ebp + GDS.patptr],esi
.PlayTick_donotjump:

    mov     esi,[ebp + GDS.patptr]
    lea     edi,[ebp + GDS.chandata]
    xor     ecx,ecx
.PlayTick_processrow:
    mov     byte  [ebp + GDS.procnot],0
    mov     dword [ebp + GDS.procins],0
    mov     byte [edi + channel.chCommand],0ffh

    mov     al,[esi]
    cmp     al,0
    je      .PlayTick_procnextchan

    and     al,1fh
    cmp     al,cl
    jne     .PlayTick_procnextchan

    lodsb
    mov     ah,al
    test    ah,20h
    jz      .PlayTick_nonot
    lodsb
    mov     [ebp + GDS.procnot],al
    lodsb
    mov     [ebp + GDS.procins],al
.PlayTick_nonot:
    test    ah,40h
    jz      .PlayTick_novol
    lodsb
    mov     [ebp + GDS.procvol],al
    .PlayTick_novol:
    test    ah,80h
    jz      .PlayTick_nocmd
    lodsb
    mov     [ebp + GDS.proccmd],al
    lodsb
    mov     [ebp + GDS.procdat],al
.PlayTick_nocmd:
.PlayTick_procnote:
    mov     byte [ebp + GDS.notedelayed],0
    call    PlayNote

%if USEVOLCOL
    movzx   eax,byte [ebp + GDS.procvol]
    and     byte [ebp + GDS.procvol],0fh
    shr     eax,4
    mov     [edi + channel.chVCommand],al
    add     eax,(procvoltab-proccmdtab)/4
    call    callproccmdtab
%endif
    movzx   eax,byte [ebp + GDS.proccmd]
    cmp     al,52
    jae     .PlayTick_procnextchan
    mov     [edi + channel.chCommand],al
    call    callproccmdtab
.PlayTick_procnextchan:
    add     edi,channelsize
    inc     ecx
    cmp     ecx,[ebp + GDS.head + mxmheader.hdNChannels]
    jne     near .PlayTick_processrow
    inc     esi
    mov     [ebp + GDS.patptr],esi
.PlayTick_notnextrow:
    lea     edi,[ebp + GDS.chandata]
    xor     ecx,ecx
.PlayTick_dotickloop:
%if USEVOLCOL
;//process volume column
    movzx   eax,byte [edi + channel.chVCommand]
    add     eax,(dovoltab-proccmdtab)/4
    call    callproccmdtab
%endif

;//process command
    movzx   eax,byte [edi + channel.chCommand]
    cmp     al,52
    jae     .PlayTick_donocmd
    add     eax,(docmdtab-proccmdtab)/4
    call    callproccmdtab
.PlayTick_donocmd:
    mov     ebx,[edi + channel.chEnvIns]
    cmp     ebx,0
    je      near .PlayTick_noenvins

;//process fadeout
    movzx   eax,byte [edi + channel.chFinalVol]

    mul     byte [ebp + GDS.uservol]
    shr     eax,6
    mul     byte [ebp + GDS.globalvol]
    mul     word [edi + channel.chFadeVol]

    shr     edx,4
    mov     [edi + channel.chFinalVol],dl

    cmp     byte [edi + channel.chSustain],0
    jne     .PlayTick_sustain
    mov     ax,[ebx + instrument.insVolFade]
    sub     [edi + channel.chFadeVol],ax
    jnb     .PlayTick_sustain
    mov     word [edi + channel.chFadeVol],0
.PlayTick_sustain:

%if USEVOLENV
;//process volume envelope
    mov     eax,[edi + channel.chVolEnvPos]
    cmp     word [edi + channel.chVolEnvSegPos],0
    je      .PlayTick_vnoloop
    cmp     al,[ebx + instrument.insVLoopE]
    jne     .PlayTick_vnoloop
    mov     al,[ebx + instrument.insVLoopS]
    mov     [edi + channel.chVolEnvPos],eax
.PlayTick_vnoloop:
    lea     esi,[ebx + instrument.insVEnv + 4*eax]

    cmp     al,[ebx + instrument.insVNum]
    je      .PlayTick_venvlast

    mov     ax,[esi + 4+2]
    mov     dx,[esi + 0+2]
    sub     eax,edx
    imul    word [edi + channel.chVolEnvSegPos]

    idiv    word   [esi + 0]
    add     al,byte [esi + 2]
    mul     byte [edi + channel.chFinalVol]

    shr     eax,6
    mov     [edi + channel.chFinalVol],al

    mov     ax,[edi + channel.chVolEnvSegPos]

    cmp     ax,0
    jne     .PlayTick_vnosustain
    cmp     byte [edi + channel.chSustain],0
    je      .PlayTick_vnosustain
    mov     edx,[edi + channel.chVolEnvPos]

    cmp     dl,[ebx + instrument.insVSustain]
    je      .PlayTick_venvnostep
.PlayTick_vnosustain:
    inc     eax
    cmp     ax,[esi + 0]
    jb      .PlayTick_venvnostep
    xor     eax,eax
    inc     dword [edi + channel.chVolEnvPos]
.PlayTick_venvnostep:
    mov     [edi + channel.chVolEnvSegPos],ax
    jmp     .PlayTick_venvend
.PlayTick_venvlast:
    mov     al,byte [esi + 2]
    mul     byte [edi + channel.chFinalVol]
    shr     eax,6
    mov     [edi + channel.chFinalVol],al
.PlayTick_venvend:
%endif

%if USEPANENV
;//process panning envelope
    mov     eax,[edi + channel.chPanEnvPos]
    cmp     word [edi + channel.chPanEnvSegPos],0
    je      .PlayTick_pnoloop
    cmp     al,[ebx + instrument.insPLoopE]
    jne     .PlayTick_pnoloop
    mov     al,[ebx + instrument.insPLoopS]
    mov     [edi + channel.chPanEnvPos],eax
.PlayTick_pnoloop:
    lea     esi,[ebx + instrument.insPEnv + 4*eax]
    cmp     al,[ebx + instrument.insPNum]
    je      .PlayTick_penvlast
    mov     ax,[esi + 4+2]
    mov     dx,[esi + 0+2]
    sub     eax,edx
    imul    word [edi + channel.chPanEnvSegPos]
    idiv    byte [esi + 0]
    add     al,byte [esi + 2]
    sub     al,32
    movsx   edx,byte [edi + channel.chFinalPan]

    xor     dl,dh
    imul    dl
    shr     eax,5
    add     [edi + channel.chFinalPan],al

    mov     ax,[edi + channel.chPanEnvSegPos]

    cmp     ax,0
    jne     .PlayTick_pnosustain
    cmp     byte [edi + channel.chSustain],0
    je      .PlayTick_pnosustain
    mov     edx,[edi + channel.chPanEnvPos]

    cmp     dl,[ebx + instrument.insPSustain]
    je      .PlayTick_penvnostep
.PlayTick_pnosustain:
    inc     eax
    cmp     ax,[esi + 0]
    jb      .PlayTick_penvnostep
    xor     eax,eax
    inc     dword [edi + channel.chPanEnvPos]

.PlayTick_penvnostep:
    mov     [edi + channel.chPanEnvSegPos],ax
    jmp     .PlayTick_penvend

.PlayTick_penvlast:
    mov     al,byte [esi + 2]
    sub     al,32
    movsx   edx,byte [edi + channel.chFinalPan]

    xor     dl,dh
    imul    dl
    shr     eax,5
    add     [edi + channel.chFinalPan],al
.PlayTick_penvend:
%endif

%if USEAUTOVIBRATO
        ;//process auto vibrato
        movzx   eax,byte [edi + channel.chAVibPos]

        mov     ah,[ebx + instrument.insVibType]
        mov     al,[ebp + GDS.vibtabs + eax]
        imul    byte [ebx + instrument.insVibDepth]
        shr     eax,4

        mov     dl,[edi + channel.chAVibSwpPos]

        cmp     dl,[ebx + instrument.insVibSweep]
        jae     .PlayTick_nosweep
        imul    dl
        idiv    byte [ebx + instrument.insVibSweep]
        inc     byte [edi + channel.chAVibSwpPos]
.PlayTick_nosweep:
        neg     eax
        movsx   eax,al
        add     eax,[edi + channel.chFinalPitch]

        call    freqrange
        mov     [edi + channel.chFinalPitch],eax

        mov     al,[ebx + instrument.insVibRate]
        add     [edi + channel.chAVibPos],al
%endif

.PlayTick_noenvins:

        ;//conv vals for Mix
        movzx   eax,byte [edi + channel.chFinalVol]
        mov     [edi + channel.chMixVol],ax
        mov     al,[edi + channel.chFinalPan]
        mov     [edi + channel.chMixPan],al
        mov     eax,[edi + channel.chFinalPitch]
%if USEBOTHFREQ
        test    byte [ebp+GDS.head+mxmheader.hdOptions],1
        jz      .PlayTick_amiga
%endif
%if USELINEARFREQ
        call    getfreq6848
        imul    eax,14
        mov     ebx,494
        xor     edx,edx
        div     ebx
%endif
%if USEBOTHFREQ
        jmp     .PlayTick_noamiga
%endif
%if USEAMIGAFREQ
.PlayTick_amiga:
        cmp     eax,100
        jb      .PlayTick_noamiga
        mov     ebx,eax
        mov     eax,94929*14
        xor     edx,edx
        div     ebx
.PlayTick_noamiga:
%endif
%if USECOMPATIBLEMODE
        test    byte [ebp+GDS.mixflags],1
        jz      .nodownsample
        shl     eax,1           ; use 22050
.nodownsample:
%endif
        and     al,~1
        mov     [edi+channel.chMixFrq],ax

        add     edi,channelsize
        inc     ecx
        cmp     ecx,[ebp+GDS.head+mxmheader.hdNChannels]
        jne     near .PlayTick_dotickloop
        ret

; ============================================================================
;  xmpPlay(int order)
; ----------------------------------------------------------------------------
;  play loaded module from selected order position (stack call interface)
; ============================================================================

global xmpPlay
global _xmpPlay
xmpPlay:
_xmpPlay:
        push    ebp
        mov     eax,[esp+8]
        call    @@xmpPlay
        pop     ebp
        ret

; ============================================================================
;  xmpPlay(eax: order)
; ----------------------------------------------------------------------------
;  play loaded module from selected order position
; ============================================================================

@@xmpPlay:
        pushad
        call    loadebp

        cmp     byte [ebp + GDS.isplaying],0
        jne     .exit

        mov     [ebp + GDS.jumptoord],eax
        mov     [ebp + GDS.curord],eax
        xor     eax,eax
        mov     [ebp + GDS.currow],eax

        lea     edi,[ebp + GDS.chandata]
        mov     ecx,channelsize*8
        xor     eax,eax
        rep     stosd

        lea     edi,[ebp + GDS.chandata]
        xor     ecx,ecx
.panloop:
        mov     al,[ebp + GDS.head + mxmheader.hdPanPos + ecx]
        mov     [edi + channel.chPan],al
        add     edi,channelsize
        inc     ecx
        cmp     cl,32
        jne     .panloop

        xor     eax,eax
        mov     byte [ebp + GDS.globalvol],40h
        mov     [ebp + GDS.jumptorow],eax
        mov     [ebp + GDS.syncval],al

        mov     al,[ebp + GDS.head + mxmheader.hdIniTempo]
        mov     [ebp + GDS.curtempo],al
        dec     al
        mov     [ebp + GDS.curtick],al

        movzx   ebx,byte [ebp + GDS.head + mxmheader.hdIniBPM]
        call    proctempo.tempo
;        mov     [ebp + GDS.stimerlen],ebx

        inc     byte [ebp + GDS.isplaying]
.exit:  popad
        ret

; ============================================================================
;  xmpStop()
; ----------------------------------------------------------------------------
;  stop playing module
; ============================================================================

global xmpStop
global _xmpStop
xmpStop:
_xmpStop:
        push    ebp
        call    loadebp
        cmp     byte [ebp + GDS.isplaying],0
        je      .exit
        dec     byte [ebp + GDS.isplaying]
.exit:  pop     ebp
        ret


sintab db 0,2,3,5,6,8,9,11,12,14,16,17,19,20,22,23,24,26,27,29,30,32,33
       db 34,36,37,38,39,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56
       db 56,57,58,59,59,60,60,61,61,62,62,62,63,63,63,64,64,64,64,64

logfreqtab dw 32768,32761,32753,32746,32738,32731,32724,32716,32709,32702,32694,32687,32679,32672,32665,32657
           dw 32768,32650,32532,32415,32298,32182,32066,31950,31835,31720,31606,31492,31379,31266,31153,31041
           dw 32768,30929,29193,27554,26008,24548,23170,21870,20643,19484,18390,17358,16384,15464,14596,13777
           dd 11131415,4417505,1753088,695713,276094,109568,43482,17256,6848,2718,1078,428,170,67,27,11


proccmdtab:
  dd procarpeggio-procnothing
  dd procportau-procnothing
  dd procportad-procnothing
  dd procportanote-procnothing
  dd procvibrato-procnothing
  dd procvolsl-procnothing
  dd procvolsl-procnothing
  dd proctremolo-procnothing
  dd procpan-procnothing
  dd 0
  dd procvolsl-procnothing
  dd procjump-procnothing
  dd procnvol-procnothing
  dd procbreak-procnothing
  dd 0
  dd proctempo-procnothing
  dd procgvol-procnothing
  dd procgvolsl-procnothing
  dd 0
  dd 0
  dd proctick-procnothing
  dd procenvpos-procnothing
  dd 0
  dd 0
  dd 0
  dd procpansl-procnothing
  dd 0
  dd procmretrig-procnothing
  dd procsync-procnothing
  dd proctremor-procnothing
  dd 0
  dd 0
  dd procsync-procnothing
  dd procxfporta-procnothing
  dd 0
  dd 0

  dd 0
  dd procfportau-procnothing
  dd procfportad-procnothing
  dd procgliss-procnothing
  dd procvibtype-procnothing
  dd 0
  dd procpatloop-procnothing
  dd proctremtype-procnothing
  dd procspan-procnothing
  dd proctick-procnothing
  dd procfvolup-procnothing
  dd procfvoldn-procnothing
  dd proctick-procnothing
  dd proctick-procnothing
  dd procpatdelay-procnothing
  dd procsync-procnothing

docmdtab:
  dd doarpeggio-procnothing
  dd doportau-procnothing
  dd doportad-procnothing
  dd doportanote-procnothing
  dd dovibrato-procnothing
  dd doportavol-procnothing
  dd dovibvol-procnothing
  dd dotremolo-procnothing
  dd 0
  dd 0
  dd dovolsl-procnothing
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd dogvolsl-procnothing
  dd 0
  dd 0
  dd dokeyoff-procnothing
  dd 0
  dd 0
  dd 0
  dd 0
  dd dopansl-procnothing
  dd 0
  dd domretrig-procnothing
  dd 0
  dd dotremor-procnothing
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0

  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd doretrig-procnothing
  dd 0
  dd 0
  dd donotecut-procnothing
  dd dodelay-procnothing
  dd 0
  dd 0

%if USEVOLCOL
procvoltab:
  dd 0
  dd procvvol0-procnothing
  dd procvvol1-procnothing
  dd procvvol2-procnothing
  dd procvvol3-procnothing
  dd procvvol4-procnothing
  dd procvvpsl-procnothing
  dd procvvpsl-procnothing
  dd procvfvoldn-procnothing
  dd procvfvolup-procnothing
  dd procvvibrat-procnothing
  dd procvvib-procnothing
  dd procvpan-procnothing
  dd procvvpsl-procnothing
  dd procvvpsl-procnothing
  dd procvportanote-procnothing

dovoltab:
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd dovvolsld-procnothing
  dd dovvolslu-procnothing
  dd 0
  dd 0
  dd 0
  dd dovibrato-procnothing
  dd 0
  dd dovpansll-procnothing
  dd dovpanslr-procnothing
  dd doportanote-procnothing
%endif

end

; =============================================================================
; $Log: mxmplay.asm,v $
; Revision 1.2  2002/03/25 18:21:07  luks
; Added support for sync callbacks.
;
; Revision 1.1  2002/03/18 15:04:31  luks
; Project added under CVS control.
;
; =============================================================================

