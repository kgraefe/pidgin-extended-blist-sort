/* Copyright (C) 2008-2017 Konrad Gräfe <konradgraefe@aol.com>
 *
 * This software may be modified and distributed under the terms
 * of the GPL2.0 license. See the COPYING file for details.
 */

#ifndef INTERNAL_H
#define INTERNAL_H

#include <glib.h>

#if GLIB_CHECK_VERSION(2,4,0)
#ifndef __APPLE_CC__
#define __APPLE_CC__ 0
#endif

#include <glib/gi18n-lib.h>
#else
#include <locale.h>
#include <libintl.h>
#define _(String) dgettext (GETTEXT_PACKAGE, String)
#define Q_(String) g_strip_context ((String), dgettext (GETTEXT_PACKAGE, String))
#ifdef gettext_noop
#define N_(String) gettext_noop (String)
#else
#define N_(String) (String)
#endif
#endif

/* This works around the lack of G_GNUC_NULL_TERMINATED in old glib and the
 * lack of the NULL sentinel in GCC older than 4.0.0 and non-GCC compilers */
#ifndef G_GNUC_NULL_TERMINATED
#  if     __GNUC__ >= 4
#    define G_GNUC_NULL_TERMINATED __attribute__((__sentinel__))
#  else
#    define G_GNUC_NULL_TERMINATED
#  endif
#endif

#if GLIB_CHECK_VERSION(2,6,0)
# include <glib/gstdio.h>
#endif /* GLIB_CHECK_VERSION(2,6,0) */

#ifdef _WIN32
# include <win32dep.h>
#endif /* _WIN32 */

#if !GLIB_CHECK_VERSION(2,6,0)
# define g_freopen freopen
# define g_fopen fopen
# define g_rmdir rmdir
# define g_remove remove
# define g_unlink unlink
# define g_lstat lstat
# define g_stat stat
# define g_mkdir mkdir
# define g_rename rename
# define g_open open
#endif /* !GLIB_CHECK_VERSION(2,6,0) */

#endif
