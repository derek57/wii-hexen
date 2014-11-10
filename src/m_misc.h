// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
// DESCRIPTION:
//      Miscellaneous.
//    
//-----------------------------------------------------------------------------


#ifndef __M_MISC__
#define __M_MISC__

#include <stdio.h>
#include <stdarg.h>

#include "doomtype.h"

char *M_StringDuplicate(const char *orig);
boolean M_WriteFile(char *name, void *source, int length);
int M_ReadFile(char *name, byte **buffer);
void M_MakeDirectory(char *dir);
char *M_TempFile(char *s);
boolean M_FileExists(char *file);
long M_FileLength(FILE *handle);
boolean M_StrToInt(const char *str, int *result);
void M_ExtractFileBase(char *path, char *dest);
void M_ForceUppercase(char *text);
char *M_StrCaseStr(char *haystack, char *needle);
boolean M_StringCopy(char *dest, const char *src, size_t dest_size);
boolean M_StringConcat(char *dest, const char *src, size_t dest_size);
char *M_StringReplace(const char *haystack, const char *needle,
                      const char *replacement);
char *M_StringJoin(const char *s, ...);
boolean M_StringStartsWith(const char *s, const char *prefix);
boolean M_StringEndsWith(const char *s, const char *suffix);
int M_vsnprintf(char *buf, size_t buf_len, const char *s, va_list args);
int M_snprintf(char *buf, size_t buf_len, const char *s, ...);
char *M_OEMToUTF8(const char *ansi);

#endif

