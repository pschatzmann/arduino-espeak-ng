/*
 * Copyright (C) 2017 Reece H. Dunn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */

#include "config-espk.h"

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>

#include <espeak-ng/espeak_ng.h>
#include <espeak-ng/encoding.h>

#include <espeak-ng/encoding.h>   // for espeak_ng_TEXT_DECODER, ESPEAKNG_EN...
#include "espeak-ng/speak_lib.h"  // for espeakCHARS_16BIT, espeakCHARS_8BIT
#include "mnemonics.h"               // for LookupMnem, MNEM_TAB
#include "translate.h"            // for LEADING_2_BITS, UTF8_TAIL_BITS

// http://www.iana.org/assignments/character-sets/character-sets.xhtml
static const MNEM_TAB mnem_encoding[] = {
	{ "ANSI_X3.4-1968",   ESPEAKNG_ENCODING_US_ASCII },
	{ "ANSI_X3.4-1986",   ESPEAKNG_ENCODING_US_ASCII },
	{ "ASMO-708",         ESPEAKNG_ENCODING_ISO_8859_6 },
	{ "ECMA-114",         ESPEAKNG_ENCODING_ISO_8859_6 },
	{ "ECMA-118",         ESPEAKNG_ENCODING_ISO_8859_7 },
	{ "ELOT_928",         ESPEAKNG_ENCODING_ISO_8859_7 },
	{ "IBM367",           ESPEAKNG_ENCODING_US_ASCII },
	{ "IBM819",           ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "ISCII",            ESPEAKNG_ENCODING_ISCII },
	{ "ISO_646.irv:1991", ESPEAKNG_ENCODING_US_ASCII },
	{ "ISO_8859-1",       ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "ISO_8859-1:1987",  ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "ISO_8859-2",       ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "ISO_8859-2:1987",  ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "ISO_8859-3",       ESPEAKNG_ENCODING_ISO_8859_3 },
	{ "ISO_8859-3:1988",  ESPEAKNG_ENCODING_ISO_8859_3 },
	{ "ISO_8859-4",       ESPEAKNG_ENCODING_ISO_8859_4 },
	{ "ISO_8859-4:1988",  ESPEAKNG_ENCODING_ISO_8859_4 },
	{ "ISO_8859-5",       ESPEAKNG_ENCODING_ISO_8859_5 },
	{ "ISO_8859-5:1988",  ESPEAKNG_ENCODING_ISO_8859_5 },
	{ "ISO_8859-6",       ESPEAKNG_ENCODING_ISO_8859_6 },
	{ "ISO_8859-6:1987",  ESPEAKNG_ENCODING_ISO_8859_6 },
	{ "ISO_8859-7",       ESPEAKNG_ENCODING_ISO_8859_7 },
	{ "ISO_8859-7:1987",  ESPEAKNG_ENCODING_ISO_8859_7 },
	{ "ISO_8859-8",       ESPEAKNG_ENCODING_ISO_8859_8 },
	{ "ISO_8859-8:1988",  ESPEAKNG_ENCODING_ISO_8859_8 },
	{ "ISO_8859-9",       ESPEAKNG_ENCODING_ISO_8859_9 },
	{ "ISO_8859-9:1989",  ESPEAKNG_ENCODING_ISO_8859_9 },
	{ "ISO_8859-10",      ESPEAKNG_ENCODING_ISO_8859_10 },
	{ "ISO_8859-10:1992", ESPEAKNG_ENCODING_ISO_8859_10 },
	{ "ISO_8859-14",      ESPEAKNG_ENCODING_ISO_8859_14 },
	{ "ISO_8859-14:1998", ESPEAKNG_ENCODING_ISO_8859_14 },
	{ "ISO_8859-15",      ESPEAKNG_ENCODING_ISO_8859_15 },
	{ "ISO_8859-16",      ESPEAKNG_ENCODING_ISO_8859_16 },
	{ "ISO_8859-16:2001", ESPEAKNG_ENCODING_ISO_8859_16 },
	{ "ISO646-US",        ESPEAKNG_ENCODING_US_ASCII },
	{ "ISO-10646-UCS-2",  ESPEAKNG_ENCODING_ISO_10646_UCS_2 },
	{ "ISO-8859-1",       ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "ISO-8859-2",       ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "ISO-8859-3",       ESPEAKNG_ENCODING_ISO_8859_3 },
	{ "ISO-8859-4",       ESPEAKNG_ENCODING_ISO_8859_4 },
	{ "ISO-8859-5",       ESPEAKNG_ENCODING_ISO_8859_5 },
	{ "ISO-8859-6",       ESPEAKNG_ENCODING_ISO_8859_6 },
	{ "ISO-8859-7",       ESPEAKNG_ENCODING_ISO_8859_7 },
	{ "ISO-8859-8",       ESPEAKNG_ENCODING_ISO_8859_8 },
	{ "ISO-8859-9",       ESPEAKNG_ENCODING_ISO_8859_9 },
	{ "ISO-8859-10",      ESPEAKNG_ENCODING_ISO_8859_10 },
	{ "ISO-8859-11",      ESPEAKNG_ENCODING_ISO_8859_11 },
	{ "ISO-8859-13",      ESPEAKNG_ENCODING_ISO_8859_13 },
	{ "ISO-8859-14",      ESPEAKNG_ENCODING_ISO_8859_14 },
	{ "ISO-8859-15",      ESPEAKNG_ENCODING_ISO_8859_15 },
	{ "ISO-8859-16",      ESPEAKNG_ENCODING_ISO_8859_16 },
	{ "KOI8-R",           ESPEAKNG_ENCODING_KOI8_R },
	{ "Latin-9",          ESPEAKNG_ENCODING_ISO_8859_15 },
	{ "TIS-620",          ESPEAKNG_ENCODING_ISO_8859_11 },
	{ "US-ASCII",         ESPEAKNG_ENCODING_US_ASCII },
	{ "UTF-8",            ESPEAKNG_ENCODING_UTF_8 },
	{ "cp367",            ESPEAKNG_ENCODING_US_ASCII },
	{ "cp819",            ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "csASCII",          ESPEAKNG_ENCODING_US_ASCII },
	{ "csISO885913",      ESPEAKNG_ENCODING_ISO_8859_13 },
	{ "csISO885914",      ESPEAKNG_ENCODING_ISO_8859_14 },
	{ "csISO885915",      ESPEAKNG_ENCODING_ISO_8859_15 },
	{ "csISO885916",      ESPEAKNG_ENCODING_ISO_8859_16 },
	{ "csISOLatin1",      ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "csISOLatin2",      ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "csISOLatin3",      ESPEAKNG_ENCODING_ISO_8859_3 },
	{ "csISOLatin4",      ESPEAKNG_ENCODING_ISO_8859_4 },
	{ "csISOLatin5",      ESPEAKNG_ENCODING_ISO_8859_9 },
	{ "csISOLatin6",      ESPEAKNG_ENCODING_ISO_8859_10 },
	{ "csISOLatinArabic", ESPEAKNG_ENCODING_ISO_8859_6 },
	{ "csISOLatinCyrillic",ESPEAKNG_ENCODING_ISO_8859_5 },
	{ "csISOLatinGreek",  ESPEAKNG_ENCODING_ISO_8859_7 },
	{ "csISOLatinHebrew", ESPEAKNG_ENCODING_ISO_8859_8 },
	{ "csKOI8R",          ESPEAKNG_ENCODING_KOI8_R },
	{ "csTIS620",         ESPEAKNG_ENCODING_ISO_8859_11 },
	{ "csUTF8",           ESPEAKNG_ENCODING_UTF_8 },
	{ "csUnicode",        ESPEAKNG_ENCODING_ISO_10646_UCS_2 },
	{ "arabic",           ESPEAKNG_ENCODING_ISO_8859_6 },
	{ "cyrillic",         ESPEAKNG_ENCODING_ISO_8859_5 },
	{ "greek",            ESPEAKNG_ENCODING_ISO_8859_7 },
	{ "greek8",           ESPEAKNG_ENCODING_ISO_8859_7 },
	{ "hebrew",           ESPEAKNG_ENCODING_ISO_8859_8 },
	{ "iso-celtic",       ESPEAKNG_ENCODING_ISO_8859_14 },
	{ "iso-ir-6",         ESPEAKNG_ENCODING_US_ASCII },
	{ "iso-ir-100",       ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "iso-ir-101",       ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "iso-ir-109",       ESPEAKNG_ENCODING_ISO_8859_3 },
	{ "iso-ir-110",       ESPEAKNG_ENCODING_ISO_8859_4 },
	{ "iso-ir-126",       ESPEAKNG_ENCODING_ISO_8859_7 },
	{ "iso-ir-127",       ESPEAKNG_ENCODING_ISO_8859_6 },
	{ "iso-ir-138",       ESPEAKNG_ENCODING_ISO_8859_8 },
	{ "iso-ir-144",       ESPEAKNG_ENCODING_ISO_8859_5 },
	{ "iso-ir-148",       ESPEAKNG_ENCODING_ISO_8859_9 },
	{ "iso-ir-157",       ESPEAKNG_ENCODING_ISO_8859_10 },
	{ "iso-ir-199",       ESPEAKNG_ENCODING_ISO_8859_14 },
	{ "iso-ir-226",       ESPEAKNG_ENCODING_ISO_8859_16 },
	{ "latin1",           ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "latin2",           ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "latin3",           ESPEAKNG_ENCODING_ISO_8859_3 },
	{ "latin4",           ESPEAKNG_ENCODING_ISO_8859_4 },
	{ "latin5",           ESPEAKNG_ENCODING_ISO_8859_9 },
	{ "latin6",           ESPEAKNG_ENCODING_ISO_8859_10 },
	{ "latin8",           ESPEAKNG_ENCODING_ISO_8859_14 },
	{ "latin10",          ESPEAKNG_ENCODING_ISO_8859_16 },
	{ "l1",               ESPEAKNG_ENCODING_ISO_8859_1 },
	{ "l2",               ESPEAKNG_ENCODING_ISO_8859_2 },
	{ "l3",               ESPEAKNG_ENCODING_ISO_8859_3 },
	{ "l4",               ESPEAKNG_ENCODING_ISO_8859_4 },
	{ "l5",               ESPEAKNG_ENCODING_ISO_8859_9 },
	{ "l6",               ESPEAKNG_ENCODING_ISO_8859_10 },
	{ "l8",               ESPEAKNG_ENCODING_ISO_8859_14 },
	{ "l10",              ESPEAKNG_ENCODING_ISO_8859_16 },
	{ "us",               ESPEAKNG_ENCODING_US_ASCII },
	{ NULL,               ESPEAKNG_ENCODING_UNKNOWN }
};

#pragma GCC visibility push(default)

espeak_ng_ENCODING
espeak_ng_EncodingFromName(const char *encoding)
{
	return LookupMnem(mnem_encoding, encoding);
}

#pragma GCC visibility pop

struct espeak_ng_TEXT_DECODER_
{
	const uint8_t *current;
	const uint8_t *end;

	uint32_t (*get)(espeak_ng_TEXT_DECODER *decoder);
	const uint16_t *codepage;
};

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-1.TXT
static const uint16_t ISO_8859_1[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, // a0
	0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af, // a8
	0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7, // b0
	0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf, // b8
	0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, // c0
	0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, // c8
	0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7, // d0
	0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df, // d8
	0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, // e0
	0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, // e8
	0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7, // f0
	0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff, // f8
};

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-2.TXT
static const uint16_t ISO_8859_2[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x0104, 0x02d8, 0x0141, 0x00a4, 0x013d, 0x015a, 0x00a7, // a0
	0x00a8, 0x0160, 0x015e, 0x0164, 0x0179, 0x00ad, 0x017d, 0x017b, // a8
	0x00b0, 0x0105, 0x02db, 0x0142, 0x00b4, 0x013e, 0x015b, 0x02c7, // b0
	0x00b8, 0x0161, 0x015f, 0x0165, 0x017a, 0x02dd, 0x017e, 0x017c, // b8
	0x0154, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0139, 0x0106, 0x00c7, // c0
	0x010c, 0x00c9, 0x0118, 0x00cb, 0x011a, 0x00cd, 0x00ce, 0x010e, // c8
	0x0110, 0x0143, 0x0147, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x00d7, // d0
	0x0158, 0x016e, 0x00da, 0x0170, 0x00dc, 0x00dd, 0x0162, 0x00df, // d8
	0x0155, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x013a, 0x0107, 0x00e7, // e0
	0x010d, 0x00e9, 0x0119, 0x00eb, 0x011b, 0x00ed, 0x00ee, 0x010f, // e8
	0x0111, 0x0144, 0x0148, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x00f7, // f0
	0x0159, 0x016f, 0x00fa, 0x0171, 0x00fc, 0x00fd, 0x0163, 0x02d9, // f8
};

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-3.TXT
static const uint16_t ISO_8859_3[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x0126, 0x02d8, 0x00a3, 0x00a4, 0xfffd, 0x0124, 0x00a7, // a0
	0x00a8, 0x0130, 0x015e, 0x011e, 0x0134, 0x00ad, 0xfffd, 0x017b, // a8
	0x00b0, 0x0127, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x0125, 0x00b7, // b0
	0x00b8, 0x0131, 0x015f, 0x011f, 0x0135, 0x00bd, 0xfffd, 0x017c, // b8
	0x00c0, 0x00c1, 0x00c2, 0xfffd, 0x00c4, 0x010a, 0x0108, 0x00c7, // c0
	0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, // c8
	0xfffd, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x0120, 0x00d6, 0x00d7, // d0
	0x011c, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x016c, 0x015c, 0x00df, // d8
	0x00e0, 0x00e1, 0x00e2, 0xfffd, 0x00e4, 0x010b, 0x0109, 0x00e7, // e0
	0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, // e8
	0xfffd, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x0121, 0x00f6, 0x00f7, // f0
	0x011d, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x016d, 0x015d, 0x02d9, // f8
};

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-4.TXT
static const uint16_t ISO_8859_4[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x0104, 0x0138, 0x0156, 0x00a4, 0x0128, 0x013b, 0x00a7, // a0
	0x00a8, 0x0160, 0x0112, 0x0122, 0x0166, 0x00ad, 0x017d, 0x00af, // a8
	0x00b0, 0x0105, 0x02db, 0x0157, 0x00b4, 0x0129, 0x013c, 0x02c7, // b0
	0x00b8, 0x0161, 0x0113, 0x0123, 0x0167, 0x014a, 0x017e, 0x014b, // b8
	0x0100, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x012e, // c0
	0x010c, 0x00c9, 0x0118, 0x00cb, 0x0116, 0x00cd, 0x00ce, 0x012a, // c8
	0x0110, 0x0145, 0x014c, 0x0136, 0x00d4, 0x00d5, 0x00d6, 0x00d7, // d0
	0x00d8, 0x0172, 0x00da, 0x00db, 0x00dc, 0x0168, 0x016a, 0x00df, // d8
	0x0101, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x012f, // e0
	0x010d, 0x00e9, 0x0119, 0x00eb, 0x0117, 0x00ed, 0x00ee, 0x012b, // e8
	0x0111, 0x0146, 0x014d, 0x0137, 0x00f4, 0x00f5, 0x00f6, 0x00f7, // f0
	0x00f8, 0x0173, 0x00fa, 0x00fb, 0x00fc, 0x0169, 0x016b, 0x02d9, // f8
};

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-5.TXT
static const uint16_t ISO_8859_5[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407, // a0
	0x0408, 0x0409, 0x040a, 0x040b, 0x040c, 0x00ad, 0x040e, 0x040f, // a8
	0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, // b0
	0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e, 0x041f, // b8
	0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, // c0
	0x0428, 0x0429, 0x042a, 0x042b, 0x042c, 0x042d, 0x042e, 0x042f, // c8
	0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437, // d0
	0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, 0x043f, // d8
	0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, // e0
	0x0448, 0x0449, 0x044a, 0x044b, 0x044c, 0x044d, 0x044e, 0x044f, // e8
	0x2116, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457, // f0
	0x0458, 0x0459, 0x045a, 0x045b, 0x045c, 0x00a7, 0x045e, 0x045f, // f8
};

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-6.TXT
static const uint16_t ISO_8859_6[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0xfffd, 0xfffd, 0xfffd, 0x00a4, 0xfffd, 0xfffd, 0xfffd, // a0
	0xfffd, 0xfffd, 0xfffd, 0xfffd, 0x060c, 0x00ad, 0xfffd, 0xfffd, // a8
	0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, // b0
	0xfffd, 0xfffd, 0xfffd, 0x061b, 0xfffd, 0xfffd, 0xfffd, 0x061f, // b8
	0xfffd, 0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627, // c0
	0x0628, 0x0629, 0x062a, 0x062b, 0x062c, 0x062d, 0x062e, 0x062f, // c8
	0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635, 0x0636, 0x0637, // d0
	0x0638, 0x0639, 0x063a, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, // d8
	0x0640, 0x0641, 0x0642, 0x0643, 0x0644, 0x0645, 0x0646, 0x0647, // e0
	0x0648, 0x0649, 0x064a, 0x064b, 0x064c, 0x064d, 0x064e, 0x064f, // e8
	0x0650, 0x0651, 0x0652, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, // f0
	0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, // f8
};

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-7.TXT
static const uint16_t ISO_8859_7[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x2018, 0x2019, 0x00a3, 0x20ac, 0x20af, 0x00a6, 0x00a7, // a0
	0x00a8, 0x00a9, 0x037a, 0x00ab, 0x00ac, 0x00ad, 0xfffd, 0x2015, // a8
	0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x0384, 0x0385, 0x0386, 0x00b7, // b0
	0x0388, 0x0389, 0x038a, 0x00bb, 0x038c, 0x00bd, 0x038e, 0x038f, // b8
	0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397, // c0
	0x0398, 0x0399, 0x039a, 0x039b, 0x039c, 0x039d, 0x039e, 0x039f, // c8
	0x03a0, 0x03a1, 0xfffd, 0x03a3, 0x03a4, 0x03a5, 0x03a6, 0x03a7, // d0
	0x03a8, 0x03a9, 0x03aa, 0x03ab, 0x03ac, 0x03ad, 0x03ae, 0x03af, // d8
	0x03b0, 0x03b1, 0x03b2, 0x03b3, 0x03b4, 0x03b5, 0x03b6, 0x03b7, // e0
	0x03b8, 0x03b9, 0x03ba, 0x03bb, 0x03bc, 0x03bd, 0x03be, 0x03bf, // e8
	0x03c0, 0x03c1, 0x03c2, 0x03c3, 0x03c4, 0x03c5, 0x03c6, 0x03c7, // f0
	0x03c8, 0x03c9, 0x03ca, 0x03cb, 0x03cc, 0x03cd, 0x03ce, 0xfffd, // f8
};

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-8.TXT
static const uint16_t ISO_8859_8[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0xfffd, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, // a0
	0x00a8, 0x00a9, 0x00d7, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af, // a8
	0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7, // b0
	0x00b8, 0x00b9, 0x00f7, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0xfffd, // b8
	0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, // c0
	0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, // c8
	0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, // d0
	0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0x2017, // d8
	0x05d0, 0x05d1, 0x05d2, 0x05d3, 0x05d4, 0x05d5, 0x05d6, 0x05d7, // e0
	0x05d8, 0x05d9, 0x05da, 0x05db, 0x05dc, 0x05dd, 0x05de, 0x05df, // e8
	0x05e0, 0x05e1, 0x05e2, 0x05e3, 0x05e4, 0x05e5, 0x05e6, 0x05e7, // f0
	0x05e8, 0x05e9, 0x05ea, 0xfffd, 0xfffd, 0x200e, 0x200f, 0xfffd, // f8
};

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-9.TXT
static const uint16_t ISO_8859_9[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, // a0
	0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af, // a8
	0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7, // b0
	0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf, // b8
	0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, // c0
	0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, // c8
	0x011e, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7, // d0
	0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x0130, 0x015e, 0x00df, // d8
	0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, // e0
	0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, // e8
	0x011f, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7, // f0
	0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x0131, 0x015f, 0x00ff, // f8
};

// Reference: http://www.iana.org/go/rfc1345
// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-10.TXT
static const uint16_t ISO_8859_10[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x0104, 0x0112, 0x0122, 0x012a, 0x0128, 0x0136, 0x00a7, // a0
	0x013b, 0x0110, 0x0160, 0x0166, 0x017d, 0x00ad, 0x016a, 0x014a, // a8
	0x00b0, 0x0105, 0x0113, 0x0123, 0x012b, 0x0129, 0x0137, 0x00b7, // b0
	0x013c, 0x0111, 0x0161, 0x0167, 0x017e, 0x2015, 0x016b, 0x014b, // b8
	0x0100, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x012e, // c0
	0x010c, 0x00c9, 0x0118, 0x00cb, 0x0116, 0x00cd, 0x00ce, 0x00cf, // c8
	0x00d0, 0x0145, 0x014c, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x0168, // d0
	0x00d8, 0x0172, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df, // d8
	0x0101, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x012f, // e0
	0x010d, 0x00e9, 0x0119, 0x00eb, 0x0117, 0x00ed, 0x00ee, 0x00ef, // e8
	0x00f0, 0x0146, 0x014d, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x0169, // f0
	0x00f8, 0x0173, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x0138, // f8
};

// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-11.TXT
static const uint16_t ISO_8859_11[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x0e01, 0x0e02, 0x0e03, 0x0e04, 0x0e05, 0x0e06, 0x0e07, // a0
	0x0e08, 0x0e09, 0x0e0a, 0x0e0b, 0x0e0c, 0x0e0d, 0x0e0e, 0x0e0f, // a8
	0x0e10, 0x0e11, 0x0e12, 0x0e13, 0x0e14, 0x0e15, 0x0e16, 0x0e17, // b0
	0x0e18, 0x0e19, 0x0e1a, 0x0e1b, 0x0e1c, 0x0e1d, 0x0e1e, 0x0e1f, // b8
	0x0e20, 0x0e21, 0x0e22, 0x0e23, 0x0e24, 0x0e25, 0x0e26, 0x0e27, // c0
	0x0e28, 0x0e29, 0x0e2a, 0x0e2b, 0x0e2c, 0x0e2d, 0x0e2e, 0x0e2f, // c8
	0x0e30, 0x0e31, 0x0e32, 0x0e33, 0x0e34, 0x0e35, 0x0e36, 0x0e37, // d0
	0x0e38, 0x0e39, 0x0e3a, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0x0e3f, // d8
	0x0e40, 0x0e41, 0x0e42, 0x0e43, 0x0e44, 0x0e45, 0x0e46, 0x0e47, // e0
	0x0e48, 0x0e49, 0x0e4a, 0x0e4b, 0x0e4c, 0x0e4d, 0x0e4e, 0x0e4f, // e8
	0x0e50, 0x0e51, 0x0e52, 0x0e53, 0x0e54, 0x0e55, 0x0e56, 0x0e57, // f0
	0x0e58, 0x0e59, 0x0e5a, 0x0e5b, 0xfffd, 0xfffd, 0xfffd, 0xfffd, // f8
};

// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-13.TXT
static const uint16_t ISO_8859_13[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x201d, 0x00a2, 0x00a3, 0x00a4, 0x201e, 0x00a6, 0x00a7, // a0
	0x00d8, 0x00a9, 0x0156, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00c6, // a8
	0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x201c, 0x00b5, 0x00b6, 0x00b7, // b0
	0x00f8, 0x00b9, 0x0157, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00e6, // b8
	0x0104, 0x012e, 0x0100, 0x0106, 0x00c4, 0x00c5, 0x0118, 0x0112, // c0
	0x010c, 0x00c9, 0x0179, 0x0116, 0x0122, 0x0136, 0x012a, 0x013b, // c8
	0x0160, 0x0143, 0x0145, 0x00d3, 0x014c, 0x00d5, 0x00d6, 0x00d7, // d0
	0x0172, 0x0141, 0x015a, 0x016a, 0x00dc, 0x017b, 0x017d, 0x00df, // d8
	0x0105, 0x012f, 0x0101, 0x0107, 0x00e4, 0x00e5, 0x0119, 0x0113, // e0
	0x010d, 0x00e9, 0x017a, 0x0117, 0x0123, 0x0137, 0x012b, 0x013c, // e8
	0x0161, 0x0144, 0x0146, 0x00f3, 0x014d, 0x00f5, 0x00f6, 0x00f7, // f0
	0x0173, 0x0142, 0x015b, 0x016b, 0x00fc, 0x017c, 0x017e, 0x2019, // f8
};

// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-14.TXT
static const uint16_t ISO_8859_14[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x1e02, 0x1e03, 0x00a3, 0x010a, 0x010b, 0x1e0a, 0x00a7, // a0
	0x1e80, 0x00a9, 0x1e82, 0x1e0b, 0x1ef2, 0x00ad, 0x00ae, 0x0178, // a8
	0x1e1e, 0x1e1f, 0x0120, 0x0121, 0x1e40, 0x1e41, 0x00b6, 0x1e56, // b0
	0x1e81, 0x1e57, 0x1e83, 0x1e60, 0x1ef3, 0x1e84, 0x1e85, 0x1e61, // b8
	0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, // c0
	0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, // c8
	0x0174, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x1e6a, // d0
	0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x0176, 0x00df, // d8
	0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, // e0
	0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, // e8
	0x0175, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x1e6b, // f0
	0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x0177, 0x00ff, // f8
};

// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-15.TXT
static const uint16_t ISO_8859_15[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x20ac, 0x00a5, 0x0160, 0x00a7, // a0
	0x0161, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af, // a8
	0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x017d, 0x00b5, 0x00b6, 0x00b7, // b0
	0x017e, 0x00b9, 0x00ba, 0x00bb, 0x0152, 0x0153, 0x0178, 0x00bf, // b8
	0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, // c0
	0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, // c8
	0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7, // d0
	0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df, // d8
	0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, // e0
	0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, // e8
	0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7, // f0
	0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff, // f8
};

// Reference: http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-16.TXT
static const uint16_t ISO_8859_16[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x0104, 0x0105, 0x0141, 0x20ac, 0x201e, 0x0160, 0x00a7, // a0
	0x0161, 0x00a9, 0x0218, 0x00ab, 0x0179, 0x00ad, 0x017a, 0x017b, // a8
	0x00b0, 0x00b1, 0x010c, 0x0142, 0x017d, 0x201d, 0x00b6, 0x00b7, // b0
	0x017e, 0x010d, 0x0219, 0x00bb, 0x0152, 0x0153, 0x0178, 0x017c, // b8
	0x00c0, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0106, 0x00c6, 0x00c7, // c0
	0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, // c8
	0x0110, 0x0143, 0x00d2, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x015a, // d0
	0x0170, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x0118, 0x021a, 0x00df, // d8
	0x00e0, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x0107, 0x00e6, 0x00e7, // e0
	0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, // e8
	0x0111, 0x0144, 0x00f2, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x015b, // f0
	0x0171, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x0119, 0x021b, 0x00ff, // f8
};

// Reference: http://www.iana.org/go/rfc1489
// Reference: http://www.unicode.org/Public/MAPPINGS/VENDORS/MISC/KOI8-R.TXT
static const uint16_t KOI8_R[0x80] = {
	0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, // 80
	0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, // 88
	0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, // 90
	0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f, // 98
	0x00a0, 0x0104, 0x0105, 0x0141, 0x20ac, 0x201e, 0x0160, 0x00a7, // a0
	0x0161, 0x00a9, 0x0218, 0x00ab, 0x0179, 0x00ad, 0x017a, 0x017b, // a8
	0x00b0, 0x00b1, 0x010c, 0x0142, 0x017d, 0x201d, 0x00b6, 0x00b7, // b0
	0x017e, 0x010d, 0x0219, 0x00bb, 0x0152, 0x0153, 0x0178, 0x017c, // b8
	0x00c0, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0106, 0x00c6, 0x00c7, // c0
	0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, // c8
	0x0110, 0x0143, 0x00d2, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x015a, // d0
	0x0170, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x0118, 0x021a, 0x00df, // d8
	0x00e0, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x0107, 0x00e6, 0x00e7, // e0
	0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, // e8
	0x0111, 0x0144, 0x00f2, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x015b, // f0
	0x0171, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x0119, 0x021b, 0x00ff, // f8
};

// Reference: http://varamozhi.sourceforge.net/iscii91.pdf
static const uint16_t ISCII[0x80] = {
	0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, // 80
	0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, // 88
	0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, // 90
	0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, // 98
	0xfffd, 0x0901, 0x0902, 0x0903, 0x0905, 0x0906, 0x0907, 0x0908, // a0
	0x0909, 0x090a, 0x090b, 0x090e, 0x090f, 0x0910, 0x090d, 0x0912, // a8
	0x0913, 0x0914, 0x0911, 0x0915, 0x0916, 0x0917, 0x0918, 0x0919, // b0
	0x091a, 0x091b, 0x091c, 0x091d, 0x091e, 0x091f, 0x0920, 0x0921, // b8
	0x0922, 0x0923, 0x0924, 0x0925, 0x0926, 0x0927, 0x0928, 0x0929, // c0
	0x092a, 0x092b, 0x092c, 0x092d, 0x092e, 0x092f, 0x095f, 0x0930, // c8
	0x0931, 0x0932, 0x0933, 0x0934, 0x0935, 0x0936, 0x0937, 0x0938, // d0
	0x0939, 0x0020, 0x093e, 0x093f, 0x0940, 0x0941, 0x0942, 0x0943, // d8
	0x0946, 0x0947, 0x0948, 0x0945, 0x094a, 0x094b, 0x094c, 0x0949, // e0
	0x094d, 0x093c, 0x0964, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, // e8
	0x0020, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, // f0
	0x0037, 0x0038, 0x0039, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, // f8
};

static uint32_t
string_decoder_getc_us_ascii(espeak_ng_TEXT_DECODER *decoder)
{
	uint8_t c = *decoder->current++;
	return (c >= 0x80) ? 0xFFFD : c;
}

static uint32_t
string_decoder_getc_codepage(espeak_ng_TEXT_DECODER *decoder)
{
	uint8_t c = *decoder->current++;
	return (c >= 0x80) ? decoder->codepage[c - 0x80] : c;
}

static uint32_t
string_decoder_getc_utf_8(espeak_ng_TEXT_DECODER *decoder)
{
	uint8_t  c = *decoder->current++;
	uint32_t ret;
	switch (c & 0xF0)
	{
	// 1-byte UTF-8 sequence
	case 0x00: case 0x10: case 0x20: case 0x30:
	case 0x40: case 0x50: case 0x60: case 0x70:
		return c;
	// UTF-8 tail byte -- invalid in isolation
	case 0x80: case 0x90: case 0xA0: case 0xB0:
		return 0xFFFD;
	// 2-byte UTF-8 sequence
	case 0xC0: case 0xD0:
		if (decoder->current + 1 >= decoder->end) goto eof;
		ret = c & 0x1F;
		if (((c = *decoder->current++) & LEADING_2_BITS) != UTF8_TAIL_BITS) goto error;
		ret = (ret << 6) + (c & 0x3F);
		return ret;
	// 3-byte UTF-8 sequence
	case 0xE0:
		if (decoder->current + 2 >= decoder->end) goto eof;
		ret = c & 0x0F;
		if (((c = *decoder->current++) & LEADING_2_BITS) != UTF8_TAIL_BITS) goto error;
		ret = (ret << 6) + (c & 0x3F);
		if (((c = *decoder->current++) & LEADING_2_BITS) != UTF8_TAIL_BITS) goto error;
		ret = (ret << 6) + (c & 0x3F);
		return ret;
	// 4-byte UTF-8 sequence
	case 0xF0:
		if (decoder->current + 3 >= decoder->end) goto eof;
		ret = c & 0x0F;
		if (((c = *decoder->current++) & LEADING_2_BITS) != UTF8_TAIL_BITS) goto error;
		ret = (ret << 6) + (c & 0x3F);
		if (((c = *decoder->current++) & LEADING_2_BITS) != UTF8_TAIL_BITS) goto error;
		ret = (ret << 6) + (c & 0x3F);
		if (((c = *decoder->current++) & LEADING_2_BITS) != UTF8_TAIL_BITS) goto error;
		ret = (ret << 6) + (c & 0x3F);
		return (ret <= 0x10FFFF) ? ret : 0xFFFD;
	}
error:
	--decoder->current;
	return 0xFFFD;
eof:
	decoder->current = decoder->end;
	return 0xFFFD;
}

static uint32_t
string_decoder_getc_iso_10646_ucs_2(espeak_ng_TEXT_DECODER *decoder)
{
	if (decoder->current + 1 >= decoder->end) {
		decoder->current = decoder->end;
		return 0xFFFD;
	}

	uint8_t c1 = *decoder->current++;
	uint8_t c2 = *decoder->current++;
	return c1 + (c2 << 8);
}

static uint32_t
string_decoder_getc_wchar(espeak_ng_TEXT_DECODER *decoder)
{
	wchar_t c = *(const wchar_t *)decoder->current;
	decoder->current += sizeof(wchar_t);
	return c;
}

static uint32_t
string_decoder_getc_auto(espeak_ng_TEXT_DECODER *decoder)
{
	const uint8_t *ptr = decoder->current;
	uint32_t c = string_decoder_getc_utf_8(decoder);
	if (c == 0xFFFD) {
		decoder->get = string_decoder_getc_codepage;
		decoder->current = ptr;
		c = decoder->get(decoder);
	}
	return c;
}

static uint32_t
null_decoder_getc(espeak_ng_TEXT_DECODER *decoder)
{
	(void)decoder; // unused parameter
	return 0;
}

typedef struct
{
	uint32_t (*get)(espeak_ng_TEXT_DECODER *decoder);
	const uint16_t *codepage;
} encoding_t;

static const encoding_t string_decoders[] = {
	{ NULL, NULL },
	{ string_decoder_getc_us_ascii, NULL },
	{ string_decoder_getc_codepage, ISO_8859_1 },
	{ string_decoder_getc_codepage, ISO_8859_2 },
	{ string_decoder_getc_codepage, ISO_8859_3 },
	{ string_decoder_getc_codepage, ISO_8859_4 },
	{ string_decoder_getc_codepage, ISO_8859_5 },
	{ string_decoder_getc_codepage, ISO_8859_6 },
	{ string_decoder_getc_codepage, ISO_8859_7 },
	{ string_decoder_getc_codepage, ISO_8859_8 },
	{ string_decoder_getc_codepage, ISO_8859_9 },
	{ string_decoder_getc_codepage, ISO_8859_10 },
	{ string_decoder_getc_codepage, ISO_8859_11 },
	// ISO-8859-12 is not a valid encoding.
	{ string_decoder_getc_codepage, ISO_8859_13 },
	{ string_decoder_getc_codepage, ISO_8859_14 },
	{ string_decoder_getc_codepage, ISO_8859_15 },
	{ string_decoder_getc_codepage, ISO_8859_16 },
	{ string_decoder_getc_codepage, KOI8_R },
	{ string_decoder_getc_codepage, ISCII },
	{ string_decoder_getc_utf_8, NULL },
	{ string_decoder_getc_iso_10646_ucs_2, NULL },
};

#pragma GCC visibility push(default)

espeak_ng_TEXT_DECODER *
create_text_decoder(void)
{
	espeak_ng_TEXT_DECODER *decoder = malloc(sizeof(espeak_ng_TEXT_DECODER));
	if (!decoder) return NULL;

	decoder->current = NULL;
	decoder->end = NULL;
	decoder->get = NULL;
	decoder->codepage = NULL;
	return decoder;
}

void
destroy_text_decoder(espeak_ng_TEXT_DECODER *decoder)
{
	if (decoder) free(decoder);
}

espeak_ng_STATUS
text_decoder_decode_string(espeak_ng_TEXT_DECODER *decoder,
                           const char *string,
                           int length,
                           espeak_ng_ENCODING encoding)
{
	if (encoding > ESPEAKNG_ENCODING_ISO_10646_UCS_2)
		return ENS_UNKNOWN_TEXT_ENCODING;

	const encoding_t *enc = string_decoders + encoding;
	if (enc->get == NULL)
		return ENS_UNKNOWN_TEXT_ENCODING;

	if (length < 0) length = string ? strlen(string) + 1 : 0;

	decoder->get = string ? enc->get : null_decoder_getc;
	decoder->codepage = enc->codepage;
	decoder->current = (const uint8_t *)string;
	decoder->end = (const uint8_t *)(string ? string + length : string);
	return ENS_OK;
}

espeak_ng_STATUS
text_decoder_decode_string_auto(espeak_ng_TEXT_DECODER *decoder,
                                const char *string,
                                int length,
                                espeak_ng_ENCODING encoding)
{
	if (encoding > ESPEAKNG_ENCODING_ISO_10646_UCS_2)
		return ENS_UNKNOWN_TEXT_ENCODING;

	const encoding_t *enc = string_decoders + encoding;
	if (enc->get == NULL)
		return ENS_UNKNOWN_TEXT_ENCODING;

	if (length < 0) length = string ? strlen(string) + 1 : 0;

	decoder->get = string ? string_decoder_getc_auto : null_decoder_getc;
	decoder->codepage = enc->codepage;
	decoder->current = (const uint8_t *)string;
	decoder->end = (const uint8_t *)(string ? string + length : string);
	return ENS_OK;
}

espeak_ng_STATUS
text_decoder_decode_wstring(espeak_ng_TEXT_DECODER *decoder,
                            const wchar_t *string,
                            int length)
{
	if (length < 0) length = string ? wcslen(string) + 1 : 0;

	decoder->get = string ? string_decoder_getc_wchar : null_decoder_getc;
	decoder->codepage = NULL;
	decoder->current = (const uint8_t *)string;
	decoder->end = (const uint8_t *)(string ? string + length : string);
	return ENS_OK;
}

espeak_ng_STATUS
text_decoder_decode_string_multibyte(espeak_ng_TEXT_DECODER *decoder,
                                     const void *input,
                                     espeak_ng_ENCODING encoding,
                                     int flags)
{
	switch (flags & 7)
	{
	case espeakCHARS_WCHAR:
		return text_decoder_decode_wstring(decoder, (const wchar_t *)input, -1);
	case espeakCHARS_AUTO:
		return text_decoder_decode_string_auto(decoder, (const char *)input, -1, encoding);
	case espeakCHARS_UTF8:
		return text_decoder_decode_string(decoder, (const char *)input, -1, ESPEAKNG_ENCODING_UTF_8);
	case espeakCHARS_8BIT:
		return text_decoder_decode_string(decoder, (const char *)input, -1, encoding);
	case espeakCHARS_16BIT:
		return text_decoder_decode_string(decoder, (const char *)input, -1, ESPEAKNG_ENCODING_ISO_10646_UCS_2);
	default:
		return ENS_UNKNOWN_TEXT_ENCODING;
	}
}

int
text_decoder_eof(espeak_ng_TEXT_DECODER *decoder)
{
	return decoder->current == decoder->end;
}

uint32_t
text_decoder_getc(espeak_ng_TEXT_DECODER *decoder)
{
	return decoder->get(decoder);
}

uint32_t
text_decoder_peekc(espeak_ng_TEXT_DECODER *decoder)
{
	if (decoder->current == decoder->end) return 0;

	const uint8_t *current = decoder->current;
	uint32_t c = decoder->get(decoder);
	decoder->current = current;
	return c;
}

const void *
text_decoder_get_buffer(espeak_ng_TEXT_DECODER *decoder)
{
	if (text_decoder_eof(decoder))
		return NULL;
	return decoder->current;
}

#pragma GCC visibility pop
