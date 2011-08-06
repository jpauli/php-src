/*
 * "streamable kanji code filter and converter"
 * Copyright (c) 1998-2002 HappySize, Inc. All rights reserved.
 *
 * LICENSE NOTICES
 *
 * This file is part of "streamable kanji code filter and converter",
 * which is distributed under the terms of GNU Lesser General Public 
 * License (version 2) as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with "streamable kanji code filter and converter";
 * if not, write to the Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA  02111-1307  USA
 *
 * The author of this file:
 *
 */
/*
 * The source code included in this files was separated from mbfilter.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 * 
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mbfilter.h"
#include "mbfilter_utf8.h"

static int mbfl_filt_ident_utf8(int c, mbfl_identify_filter *filter);

static const unsigned char mblen_table_utf8[] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1
};

static const char *mbfl_encoding_utf8_aliases[] = {"utf8", NULL};

const mbfl_encoding mbfl_encoding_utf8 = {
	mbfl_no_encoding_utf8,
	"UTF-8",
	"UTF-8",
	(const char *(*)[])&mbfl_encoding_utf8_aliases,
	mblen_table_utf8,
	MBFL_ENCTYPE_MBCS
};

const struct mbfl_identify_vtbl vtbl_identify_utf8 = {
	mbfl_no_encoding_utf8,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_utf8
};

const struct mbfl_convert_vtbl vtbl_utf8_wchar = {
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_utf8_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_utf8 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf8,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_utf8,
	mbfl_filt_conv_common_flush
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * UTF-8 => wchar
 */
int mbfl_filt_conv_utf8_wchar(int c, mbfl_convert_filter *filter)
{
	int s, c1, w = 0, flag = 0;

	if (c < 0x80) {
		if (c >= 0) {
			CK((*filter->output_function)(c, filter->data));
		}
		filter->status = 0;
	} else if (c < 0xc0) {
		int status = filter->status & 0xff;
		switch (status) {
		case 0x10: /* 2byte code 2nd char: 0x80-0xbf */
		case 0x21: /* 3byte code 3rd char: 0x80-0xbf */
		case 0x32: /* 4byte code 4th char: 0x80-0xbf */
			filter->status = 0;
			s = filter->cache | (c & 0x3f);
			filter->cache = 0;
			if ((status == 0x10 && s >= 0x80) ||
			    (status == 0x21 && s >= 0x800 && (s < 0xd800 || s > 0xdfff)) ||
			    (status == 0x32 && s >= 0x10000 && s < 0x200000)) {
				CK((*filter->output_function)(s, filter->data));
			} else {
				w = s & MBFL_WCSGROUP_MASK;
				flag = 1;
			}
			break;
		case 0x20: /* 3byte code 2nd char: 0:0xa0-0xbf,D:0x80-9F,1-C,E-F:0x80-0x9f */
			s = filter->cache | ((c & 0x3f) << 6);
			c1 = (s >> 12) & 0xf;
			if ((c1 == 0x0 && c >= 0xa0) || 
				(c1 == 0xd && c < 0xa0) || 
				(c1 > 0x0 && c1 != 0xd)) {
				filter->cache = s;
				filter->status++;
			} else {
				w = s & MBFL_WCSGROUP_MASK;
				flag = 1;
			}
			break;
		case 0x31: /* 4byte code 3rd char: 0x80-0xbf */
			filter->cache |= ((c & 0x3f) << 6);
			filter->status++;
			break;
		case 0x30: /* 4byte code 2nd char: 0:0x90-0xbf,1-3:0x80-0xbf,4:0x80-0x8f */
			s = filter->cache | ((c & 0x3f) << 12);
			c1 = (s >> 18) & 0x7;
			if ((c1 == 0x0 && c >= 0x90) ||
				(c1 > 0x0 && c1 < 0x4) ||
				(c1 == 0x4 && c < 0x90)) {
				filter->cache = s;
				filter->status++;
			} else {
				w = s & MBFL_WCSGROUP_MASK;
				flag = 1;
			}
			break;
		default:
			w = c & MBFL_WCSGROUP_MASK;
			flag = 1;
			break;
		}
	} else if (c < 0xc2) { /* invalid: 0xc0,0xc1 */
		w = c & MBFL_WCSGROUP_MASK;
		flag = 1;
	} else if (c < 0xe0) { /* 2byte code first char: 0xc2-0xdf */
		if (filter->status == 0x0) {
			filter->status = 0x10;
			filter->cache = (c & 0x1f) << 6;
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			flag = 1;
		}
	} else if (c < 0xf0) { /* 3byte code first char: 0xe0-0xef */
		if (filter->status == 0x0) {
			filter->status = 0x20;
			filter->cache = (c & 0xf) << 12;
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			flag = 1;
		}
	} else if (c < 0xf5) { /* 4byte code first char: 0xf0-0xf4 */
		if (filter->status == 0x0) {
			filter->status = 0x30;
			filter->cache = (c & 0x7) << 18;
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			flag = 1;
		}
	} else {
		w = c & MBFL_WCSGROUP_MASK;
		flag = 1;
	}

	if (flag) {
		w |= MBFL_WCSGROUP_THROUGH;
		CK((*filter->output_function)(w, filter->data));
		filter->status = 0;
		filter->cache = 0;
	}

	return c;
}

/*
 * wchar => UTF-8
 */
int mbfl_filt_conv_wchar_utf8(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < 0x110000) {
		if (c < 0x80) {
			CK((*filter->output_function)(c, filter->data));
		} else if (c < 0x800) {
			CK((*filter->output_function)(((c >> 6) & 0x1f) | 0xc0, filter->data));
			CK((*filter->output_function)((c & 0x3f) | 0x80, filter->data));
		} else if (c < 0x10000) {
			CK((*filter->output_function)(((c >> 12) & 0x0f) | 0xe0, filter->data));
			CK((*filter->output_function)(((c >> 6) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)((c & 0x3f) | 0x80, filter->data));
		} else {
			CK((*filter->output_function)(((c >> 18) & 0x07) | 0xf0, filter->data));
			CK((*filter->output_function)(((c >> 12) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)(((c >> 6) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)((c & 0x3f) | 0x80, filter->data));
		}
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

static int mbfl_filt_ident_utf8(int c, mbfl_identify_filter *filter)
{
	int c1;

	c1 = (filter->status >> 8) & 0xff;
	filter->status &= 0xff;

	if (c < 0x80) {
		if (c < 0) { 
			filter->flag = 1;	/* bad */
		} else if (filter->status) {
			filter->flag = 1;	/* bad */
		}
		filter->status = 0;
	} else if (c < 0xc0) {
		switch (filter->status) {
		case 0x20: /* 3 byte code 2nd char */
			if ((c1 == 0x0 && c >= 0xa0) || 
				(c1 == 0xd && c < 0xa0) || 
				(c1 > 0x0 && c1 != 0xd)) {
				filter->status++;
			} else {
				filter->flag = 1;	/* bad */
				filter->status = 0;				
			}
			break;
		case 0x30: /* 4 byte code 2nd char */
			if ((c1 == 0x0 && c >= 0x90) || 
				(c1 > 0x0 && c1 < 0x4) || 
				(c1 == 0x4 && c < 0x90)) {
				filter->status++;
			} else {
				filter->flag = 1;	/* bad */
				filter->status = 0;				
			}
			break;
		case 0x31: /* 4 byte code 3rd char */
			filter->status++;
			break;
		case 0x10: /* 2 byte code 2nd char */
		case 0x21: /* 3 byte code 3rd char */
		case 0x32: /* 4 byte code 4th char */
			filter->status = 0;
			break;
		default:
			filter->flag = 1;	/* bad */
			filter->status = 0;
			break;
		}
	} else if (c < 0xc2) { /* 0xc0,0xc1 */
		filter->flag = 1;	/* bad */
		filter->status = 0;
	} else {
		if (filter->status) {
			filter->flag = 1;	/* bad */
		}
		filter->status = 0;
		if (c < 0xe0) {				/* 2 byte code first char */
			filter->status = 0x10;
		} else if (c < 0xf0) {		/* 3 byte code 1st char */
			filter->status = 0x20;
			filter->status |= (c & 0xf) << 8;
		} else if (c < 0xf5) {		/* 4 byte code 1st char */
			filter->status = 0x30;
			filter->status |= (c & 0x7) << 8;
		} else {
			filter->flag = 1;	/* bad */
		}
	}

	return c;
}
