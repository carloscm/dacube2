; =============================================================================
;  MXMPlay/Linux-Win32                                    module for c example
; -----------------------------------------------------------------------------
;  MXMPlay       (C) 1995,1996 Niklas Beisert / Pascal
;  MXMPlay/Win32 (C) 1999 Domin8R and The Artist Formerly Known As Doctor Roole
;  MXMPlay/Linux (C) 2001,2002 Luks
; -----------------------------------------------------------------------------
;  $Id: test-mxm.asm,v 1.1 2002/03/18 15:04:32 luks Exp $
; =============================================================================

section .data

global _module
_module:
global module
module:
incbin "./data/dacube2.mxm"


; =============================================================================
; $Log: test-mxm.asm,v $
; Revision 1.1  2002/03/18 15:04:32  luks
; Project added under CVS control.
;
; =============================================================================
