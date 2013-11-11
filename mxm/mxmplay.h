//=============================================================================
// MXMPlay/Linux-Win32                                       c/c++ header file
//-----------------------------------------------------------------------------
// MXMPlay       (C) 1995,1996 Niklas Beisert / Pascal
// MXMPlay/Win32 (C) 1999 Domin8R and The Artist Formerly Known As Doctor Roole
// MXMPlay/Linux (C) 2001 Luks
//-----------------------------------------------------------------------------
// $Id: mxmplay.h,v 1.3 2002/03/25 18:23:25 luks Exp $
//=============================================================================

#ifndef __MXMPLAY_H__
#define __MXMPLAY_H__

#ifdef __cplusplus
extern "C"
 {
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// ============================================================================
//  int dsInit(HWND hwnd, void * module)
// ----------------------------------------------------------------------------
//  initialize player, load module, intialize ds, create thread, ...
// ----------------------------------------------------------------------------
//  input : window handle
//          mxm module to play
//  output: 0 -> ok
//          ? -> error (don't call other functions if ossInit fail)
// ============================================================================
int __stdcall dsInit(HWND hwnd, void * module);

// ============================================================================
//  void dsClose()
// ----------------------------------------------------------------------------
//  shuts down DirectSound and MXMPlay
// ----------------------------------------------------------------------------
//  input : --
//  output: --
// ============================================================================
void __stdcall dsClose();

#else

// ============================================================================
//  int ossInit(void * module)
// ----------------------------------------------------------------------------
//  initialize player, load module, intialize oss, create thread, ...
// ----------------------------------------------------------------------------
//  input : mxm module to play
//  output: 0 -> ok
//          ? -> error (don't call other functions if ossInit fail)
// ============================================================================
int ossInit(void * module);

// ============================================================================
//  void ossClose()
// ----------------------------------------------------------------------------
//  close oss and stop thread.
// ----------------------------------------------------------------------------
//  input : --
//  output: --
// ============================================================================
void ossClose();

#endif /* WIN32 */

// ============================================================================
//  void xmpPlay(int order)
// ----------------------------------------------------------------------------
//  play module from specified order
// ----------------------------------------------------------------------------
//  input : start order (default is 0)
//  output: --
// ============================================================================
void xmpPlay(int order);

// ============================================================================
//  void xmpStop()
// ----------------------------------------------------------------------------
//  stop playing module
// ----------------------------------------------------------------------------
//  input : --
//  output: --
// ============================================================================
void xmpStop();

// ============================================================================
//  void xmpSetVolume(int volume)
// ----------------------------------------------------------------------------
//  set global volume
// ----------------------------------------------------------------------------
//  input : volume (0..64)
//  output: --
// ============================================================================
void xmpSetVolume(int volume);

// ============================================================================
//  inc xmpGetSync()
// ----------------------------------------------------------------------------
//  get latest sync value
// ----------------------------------------------------------------------------
//  input : --
//  output: sync value
// ============================================================================
int xmpGetSync();

// ============================================================================
//  int xmpGetPosition()
// ----------------------------------------------------------------------------
//  get actual row and order
// ----------------------------------------------------------------------------
//  input : --
//  output: 0x????OORR (OO = order, RR = row)
// ============================================================================
int xmpGetPos();

// ============================================================================
//  void xmpSetSyncCallback(SyncCallback callback);
// ----------------------------------------------------------------------------
//  set callback for sync-effect
// ----------------------------------------------------------------------------
//  input : pointer to callback
//  output: --
// ============================================================================

typedef void (*SyncCallback)(unsigned char syncval);

void xmpSetSyncCallback(SyncCallback callback);

#ifdef __cplusplus
 };
#endif

#endif /* !__MXMPLAY_H__ */

//=============================================================================
// $Log: mxmplay.h,v $
// Revision 1.3  2002/03/25 18:23:25  luks
// Support for sync callback.
//
// Revision 1.2  2002/03/18 15:06:35  luks
// Removed mistake ds_close() -> dsClose()
//
// Revision 1.1  2002/03/18 15:04:31  luks
// Project added under CVS control.
//
//=============================================================================
