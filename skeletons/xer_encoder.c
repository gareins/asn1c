/*-
 * Copyright (c) 2003, 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_internal.h>
#include <constr_TYPE.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

/*
 * The XER encoder of any type. May be invoked by the application.
 */
asn_enc_rval_t
xer_encode(asn1_TYPE_descriptor_t *td, void *sptr,
	enum xer_encoder_flags_e xer_flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	asn_enc_rval_t er, tmper;
	const char *mname;
	size_t mlen;
	int xcan = (xer_flags & XER_F_CANONICAL) ? 1 : 2;

	if(!td || !sptr) {
		er.encoded = -1;
		er.failed_type = td;
		er.structure_ptr = sptr;
		return er;
	}

	mname = td->name;
	mlen = strlen(mname);

	_ASN_CALLBACK3("<", 1, mname, mlen, ">", 1);

	tmper = td->xer_encoder(td, sptr, 1, xer_flags, cb, app_key);
	if(tmper.encoded == -1) return tmper;

	_ASN_CALLBACK3("</", xcan, mname, mlen, ">\n",xcan);

	er.encoded = 2 + (2 * xcan) + (2 * mlen) + tmper.encoded;

	return er;
}

static int _print2fp(const void *buffer, size_t size, void *app_key);

int
xer_fprint(FILE *stream, asn1_TYPE_descriptor_t *td, void *sptr) {
	asn_enc_rval_t er;

	if(!stream) stream = stdout;
	if(!td || !sptr)
		return -1;

	er = xer_encode(td, sptr, XER_F_BASIC, _print2fp, stream);
	if(er.encoded == -1)
		return -1;

	return fflush(stream);
}

static int
_print2fp(const void *buffer, size_t size, void *app_key) {
	FILE *stream = (FILE *)app_key;

	if(fwrite(buffer, 1, size, stream) != size)
		return -1;

	return 0;
}

