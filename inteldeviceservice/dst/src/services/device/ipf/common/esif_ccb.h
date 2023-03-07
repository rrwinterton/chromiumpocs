/*******************************************************************************
** This file is provided under a dual BSD/GPLv2 license.  When using or
** redistributing this file, you may do so under either license.
**
** GPL LICENSE SUMMARY
**
** Copyright (c) 2013-2022 Intel Corporation All Rights Reserved
**
** This program is free software; you can redistribute it and/or modify it under
** the terms of version 2 of the GNU General Public License as published by the
** Free Software Foundation.
**
** This program is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
** details.
**
** You should have received a copy of the GNU General Public License along with
** this program; if not, write to the Free Software  Foundation, Inc.,
** 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
** The full GNU General Public License is included in this distribution in the
** file called LICENSE.GPL.
**
** BSD LICENSE
**
** Copyright (c) 2013-2022 Intel Corporation All Rights Reserved
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this
**   list of conditions and the following disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice,
**   this list of conditions and the following disclaimer in the documentation
**   and/or other materials provided with the distribution.
** * Neither the name of Intel Corporation nor the names of its contributors may
**   be used to endorse or promote products derived from this software without
**   specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
**
*******************************************************************************/

#pragma once




/* OS Agnostic */
#include <stdio.h>
#include <stdlib.h>


/* Windows OS */
#pragma strict_gs_check(on)


#include <windows.h>




/* Add Linux Base Types for Windows */
typedef unsigned char u8;	/* A BYTE  */
typedef unsigned short u16;	/* A WORD  */
typedef unsigned int u32;	/* A DWORD */
typedef unsigned long long u64;	/* A QWORD */

typedef	char *esif_string;		/* NULL-teriminated ANSI string */
typedef HANDLE esif_os_handle_t;/* opaque OS Handle (not a pointer) */
typedef u64 esif_handle_t;	/* opaque ESIF 64-bit handle (may NOT be a pointer) */
typedef u64 esif_context_t;	/* opaque ESIF 64-bit context (may be a pointer) */

#define ESIF_ATTR_OS		"Windows"				/* OS Is Windows */
#define ESIF_INLINE			__inline				/* Inline Function Directive */
#define ESIF_FUNC			__FUNCTION__			/* Current Function Name */
#define ESIF_CALLCONV		__cdecl					/* SDK Calling Convention */
#define ESIF_PATH_SEP		"\\"					/* Path Separator String */
#define ESIF_EXPORT			__declspec(dllexport)	/* Used for Exported Symbols */
#define ESIF_INVALID_HANDLE	((esif_handle_t)(-1))	/* Invalid ESIF Handle */



/* Sleep Interface */
#define esif_ccb_sleep(sec)		Sleep(sec * 1000)
#define esif_ccb_sleep_msec(msec)	Sleep(msec)

/* Deduce Platform based on predefined compiler flags */
#ifdef _WIN64
#define ESIF_ATTR_64BIT
#endif

/* Deduce Debug Build for Windows. Non-Windows can define this in Makefile */
#ifdef _DEBUG
#define ESIF_ATTR_DEBUG
#endif

/*
 * OS Agnostic
 */
#if defined(ESIF_ATTR_USER) && !defined(ESIF_ATTR_NO_TYPES)
typedef u8   		UInt8;	/* A CCB BYTE  */
typedef char 		Int8;
typedef u16  		UInt16;	/* A CCB WORD  */
typedef short 		Int16;
typedef u32 		UInt32;	/* A CCB DWORD */
typedef int     	Int32;
typedef u64 		UInt64;	/* A CCB QWORD */
typedef long long 	Int64;
#ifdef __cplusplus
typedef bool  		Bool;	/* C++ BOOLEAN */
#else
typedef u8		Bool;	/* C BOOLEAN */
#endif
#endif


/*
 * Macros required for esif_rc and esif_sdk headers
 */

/* ID Lookup Failed To Find String */
#define ESIF_NOT_AVAILABLE (esif_string) "NA"

/* Used for Value-To-String case statements */
#define ESIF_CASE_ENUM(e)	case e: return (esif_string) #e
#define ESIF_CASE(e, val)	case e: return (esif_string) val

/* Platform Architecture Type */
#ifdef ESIF_ATTR_64BIT
#define ESIF_PLATFORM_TYPE "x64"
#else
#define ESIF_PLATFORM_TYPE "x86"
#endif

/* Build Type */
#ifdef ESIF_ATTR_DEBUG
#define ESIF_BUILD_TYPE	"Debug"
#else
#define ESIF_BUILD_TYPE "Release"
#endif
