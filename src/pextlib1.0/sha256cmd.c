/*
 * sha256cmd.c
 * $Id$
 *
 * Copyright (c) 2009 The MacPorts Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Apple Computer, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <tcl.h>

#include "sha256cmd.h"

#if !defined(HAVE_LIBMD) || defined(__APPLE__)

/* We do not have libmd.
 * let's use our own version of sha256* libraries.
 */
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>

#include "sha2.h"
#include "sha2.c"
#include "md_wrappers.h"
CHECKSUMEnd(SHA256_, SHA256_CTX, SHA256_DIGEST_LENGTH)
CHECKSUMFile(SHA256_, SHA256_CTX)

#elif defined(HAVE_LIBMD)
#include <sys/types.h>
#include <sha256.h>
#define SHA256_DIGEST_LENGTH 32
#else
#error libcrypto or libmd are required
#endif

int SHA256Cmd(ClientData clientData UNUSED, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
	char *file, *action;
	char buf[2*SHA256_DIGEST_LENGTH + 1];
	const char usage_message[] = "Usage: sha256 file";
	const char error_message[] = "Could not open file: ";
	Tcl_Obj *tcl_result;

	if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "action ?file?");
		return TCL_ERROR;
	}

	/*
	 * Only the 'file' action is currently supported
	 */
	action = Tcl_GetString(objv[1]);
	if (strcmp(action, "file") != 0) {
		tcl_result = Tcl_NewStringObj(usage_message, sizeof(usage_message) - 1);
		Tcl_SetObjResult(interp, tcl_result);
		return TCL_ERROR;
	}
	file = Tcl_GetString(objv[2]);

	if (!SHA256_File(file, buf)) {
		tcl_result = Tcl_NewStringObj(error_message, sizeof(error_message) - 1);
		Tcl_AppendObjToObj(tcl_result, Tcl_NewStringObj(file, -1));
		Tcl_SetObjResult(interp, tcl_result);
		return TCL_ERROR;
	}
	tcl_result = Tcl_NewStringObj(buf, sizeof(buf) - 1);
	Tcl_SetObjResult(interp, tcl_result);
	return TCL_OK;
}
