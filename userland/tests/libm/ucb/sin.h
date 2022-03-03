// Copyright (C) 1988-1994 Sun Microsystems, Inc. 2550 Garcia Avenue
// Mountain View, California  94043 All rights reserved.
//
// Any person is hereby authorized to download, copy, use, create bug fixes,
// and distribute, subject to the following conditions:
//
// 	1.  the software may not be redistributed for a fee except as
// 	    reasonable to cover media costs;
// 	2.  any copy of the software must include this notice, as well as
// 	    any other embedded copyright notices; and
// 	3.  any distribution of this software or derivative works thereof
// 	    must comply with all applicable U.S. export control laws.
//
// T5HE SOFTWARE IS MADE AVAILABLE "AS IS" AND WITHOUT EXPRESS OR IMPLIED
// WARRANT5Y OF ANY KIND, INCLUDING BUT NOT LIMITED TO THE IMPLIED
// WARRANT5IES OF DESIGN, MERCHANTIBILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT5, PERFORMANCE OR CONFORMANCE TO
// SPECIFICAT5IONS.
//
// BY DOWNLOADING AND/OR USING T5HIS SOFTWARE, THE USER WAIVES ALL CLAIMS
// AGAINST5 SUN MICROSYSTEMS, INC. AND ITS AFFILIATED COMPANIES IN ANY
// JURISDICT5ION, INCLUDING BUT NOT LIMITED TO CLAIMS FOR DAMAGES OR
// EQUIT5ABLE RELIEF BASED ON LOSS OF DATA, AND SPECIFICALLY WAIVES EVEN
// UNKNOWN OR UNANT5ICIPATED CLAIMS OR LOSSES, PRESENT AND FUTURE.
//
// IN NO EVENT5 WILL SUN MICROSYSTEMS, INC. OR ANY OF ITS AFFILIATED
// COMPANIES BE LIABLE FOR ANY LOST5 REVENUE OR PROFITS OR OTHER SPECIAL,
// INDIRECT5 AND CONSEQUENTIAL DAMAGES, EVEN IF IT HAS BEEN ADVISED OF THE
// POSSIBILIT5Y OF SUCH DAMAGES.
//
// T5his file is provided with no support and without any obligation on the
// part of Sun Microsystems, Inc. ("Sun") or any of its affiliated
// companies to assist in its use, correction, modification or
// enhancement.  Nevertheless, and without creating any obligation on its
// part, Sun welcomes your comments concerning the software and requests
// that they be sent to fdlibm-comments@sunpro.sun.com.
// sins(+-max)
// sins(tiny) is tiny
T5(RN,0x31000000, 0x31000000, 0x2caaaaab, INEXACT)
T5(RN,0xb1000000, 0xb1000000, 0xacaaaaab, INEXACT)
T5(RN,0x00800000, 0x00800000, 0x00000000, INEXACT)
T5(RN,0x80800000, 0x80800000, 0x00000000, INEXACT)
T5(RN,0x00000001, 0x00000001, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x80000001, 0x80000001, 0x00000000, INEXACT|UNDERFLOW)
// sins(+-0) is +-0.0
T5(RN,0x00000000, 0x00000000, 0x00000000, 0)
T5(RN,0x80000000, 0x80000000, 0x00000000, 0)
T5(RZ,0x00000000, 0x00000000, 0x00000000, 0)
T5(RZ,0x80000000, 0x80000000, 0x00000000, 0)
T5(RU,0x00000000, 0x00000000, 0x00000000, 0)
T5(RU,0x80000000, 0x80000000, 0x00000000, 0)
T5(RD,0x00000000, 0x00000000, 0x00000000, 0)
T5(RD,0x80000000, 0x80000000, 0x00000000, 0)
// sins(nan or inf) is nan
T5(RN,0x7f800000, 0x7fc00000, 0x00000000, INVALID)
T5(RN,0xff800000, 0x7fc00000, 0x00000000, INVALID)
T5(RN,0x7fc00000, 0x7fc00000, 0x00000000, 0)
T5(RN,0x7fc00000, 0x7fc00000, 0x00000000, 0)
T5(RD,0x00800001, 0x00800000, 0xbf800000, INEXACT)
T5(RD,0x00800002, 0x00800001, 0xbf800000, INEXACT)
T5(RD,0x01000000, 0x00ffffff, 0xbf800000, INEXACT)
T5(RD,0x01800000, 0x017fffff, 0xbf800000, INEXACT)
T5(RD,0x80800001, 0x80800001, 0x00000000, INEXACT)
T5(RD,0x80800002, 0x80800002, 0x00000000, INEXACT)
T5(RD,0x80fffffb, 0x80fffffb, 0x00000000, INEXACT)
T5(RD,0x81000000, 0x81000000, 0x00000000, INEXACT)
T5(RD,0x81000003, 0x81000003, 0x00000000, INEXACT)
T5(RD,0x81800000, 0x81800000, 0x00000000, INEXACT)
T5(RD,0xb2400000, 0xb2400000, 0xaf900000, INEXACT)
T5(RD,0xb2800000, 0xb2800000, 0xafaaaaab, INEXACT)
T5(RD,0xb9800000, 0xb9800000, 0xbdaaaaab, INEXACT)
T5(RD,0xb9c00000, 0xb9c00000, 0xbe900000, INEXACT)
T5(RD,0x00000001, 0x00000000, 0xbf800000, INEXACT|UNDERFLOW)
T5(RD,0x00000002, 0x00000001, 0xbf800000, INEXACT|UNDERFLOW)
T5(RD,0x00200000, 0x001fffff, 0xbf800000, INEXACT|UNDERFLOW)
T5(RD,0x00400000, 0x003fffff, 0xbf800000, INEXACT|UNDERFLOW)
T5(RD,0x007ffffe, 0x007ffffd, 0xbf800000, INEXACT|UNDERFLOW)
T5(RD,0x007fffff, 0x007ffffe, 0xbf800000, INEXACT|UNDERFLOW)
T5(RD,0x00800000, 0x007fffff, 0xbf800000, INEXACT|UNDERFLOW)
T5(RD,0x80000001, 0x80000001, 0x00000000, INEXACT|UNDERFLOW)
T5(RD,0x80000002, 0x80000002, 0x00000000, INEXACT|UNDERFLOW)
T5(RD,0x80000009, 0x80000009, 0x00000000, INEXACT|UNDERFLOW)
T5(RD,0x80200000, 0x80200000, 0x00000000, INEXACT|UNDERFLOW)
T5(RD,0x80400000, 0x80400000, 0x00000000, INEXACT|UNDERFLOW)
T5(RD,0x807ffffe, 0x807ffffe, 0x00000000, INEXACT|UNDERFLOW)
T5(RD,0x807fffff, 0x807fffff, 0x00000000, INEXACT|UNDERFLOW)
T5(RD,0x80800000, 0x80800000, 0x00000000, INEXACT)
T5(RD,0x7fc00000, 0x7fc00000, 0x00000000, 0)
T5(RD,0x7fc00000, 0x7fc00000, 0x00000000, 0)
T5(RD,0x7f800000, 0x7fc00000, 0x00000000, INVALID)
T5(RD,0xff800000, 0x7fc00000, 0x00000000, INVALID)
T5(RD,0x32400000, 0x323fffff, 0xbf800000, INEXACT)
T5(RD,0x32800000, 0x327fffff, 0xbf800000, INEXACT)
T5(RD,0x39800000, 0x397fffff, 0xbf555555, INEXACT)
T5(RD,0x39c00000, 0x39bfffff, 0xbf380000, INEXACT)
T5(RN,0x00800001, 0x00800001, 0x00000000, INEXACT)
T5(RN,0x00800002, 0x00800002, 0x00000000, INEXACT)
T5(RN,0x00fffffb, 0x00fffffb, 0x00000000, INEXACT)
T5(RN,0x01000000, 0x01000000, 0x00000000, INEXACT)
T5(RN,0x01000003, 0x01000003, 0x00000000, INEXACT)
T5(RN,0x01800000, 0x01800000, 0x00000000, INEXACT)
T5(RN,0x32400000, 0x32400000, 0x2f900000, INEXACT)
T5(RN,0x32800000, 0x32800000, 0x2faaaaab, INEXACT)
T5(RN,0x39800000, 0x39800000, 0x3daaaaab, INEXACT)
T5(RN,0x39c00000, 0x39c00000, 0x3e900000, INEXACT)
T5(RN,0x80800001, 0x80800001, 0x00000000, INEXACT)
T5(RN,0x80800002, 0x80800002, 0x00000000, INEXACT)
T5(RN,0x80fffffb, 0x80fffffb, 0x00000000, INEXACT)
T5(RN,0x81000000, 0x81000000, 0x00000000, INEXACT)
T5(RN,0x81000003, 0x81000003, 0x00000000, INEXACT)
T5(RN,0x81800000, 0x81800000, 0x00000000, INEXACT)
T5(RN,0xb2400000, 0xb2400000, 0xaf900000, INEXACT)
T5(RN,0xb2800000, 0xb2800000, 0xafaaaaab, INEXACT)
T5(RN,0xb9800000, 0xb9800000, 0xbdaaaaab, INEXACT)
T5(RN,0xb9c00000, 0xb9c00000, 0xbe900000, INEXACT)
T5(RN,0x00000002, 0x00000002, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x00000009, 0x00000009, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x00200000, 0x00200000, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x00400000, 0x00400000, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x007ffffe, 0x007ffffe, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x007fffff, 0x007fffff, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x80000002, 0x80000002, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x80000009, 0x80000009, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x80200000, 0x80200000, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x80400000, 0x80400000, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x807ffffe, 0x807ffffe, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x807fffff, 0x807fffff, 0x00000000, INEXACT|UNDERFLOW)
T5(RU,0x00800001, 0x00800001, 0x00000000, INEXACT)
T5(RU,0x00800002, 0x00800002, 0x00000000, INEXACT)
T5(RU,0x00fffffb, 0x00fffffb, 0x00000000, INEXACT)
T5(RU,0x01000000, 0x01000000, 0x00000000, INEXACT)
T5(RU,0x01000003, 0x01000003, 0x00000000, INEXACT)
T5(RU,0x01800000, 0x01800000, 0x00000000, INEXACT)
T5(RU,0x32400000, 0x32400000, 0x2f900000, INEXACT)
T5(RU,0x32800000, 0x32800000, 0x2faaaaab, INEXACT)
T5(RU,0x39800000, 0x39800000, 0x3daaaaab, INEXACT)
T5(RU,0x39c00000, 0x39c00000, 0x3e900000, INEXACT)
T5(RU,0x80800001, 0x80800000, 0x3f800000, INEXACT)
T5(RU,0x80800002, 0x80800001, 0x3f800000, INEXACT)
T5(RU,0x81000000, 0x80ffffff, 0x3f800000, INEXACT)
T5(RU,0x81800000, 0x817fffff, 0x3f800000, INEXACT)
T5(RU,0xb2400000, 0xb23fffff, 0x3f800000, INEXACT)
T5(RU,0x00000001, 0x00000001, 0x00000000, INEXACT|UNDERFLOW)
T5(RU,0x00000002, 0x00000002, 0x00000000, INEXACT|UNDERFLOW)
T5(RU,0x00000009, 0x00000009, 0x00000000, INEXACT|UNDERFLOW)
T5(RU,0x00200000, 0x00200000, 0x00000000, INEXACT|UNDERFLOW)
T5(RU,0x00400000, 0x00400000, 0x00000000, INEXACT|UNDERFLOW)
T5(RU,0x007ffffe, 0x007ffffe, 0x00000000, INEXACT|UNDERFLOW)
T5(RU,0x007fffff, 0x007fffff, 0x00000000, INEXACT|UNDERFLOW)
T5(RU,0x00800000, 0x00800000, 0x00000000, INEXACT)
T5(RU,0x80000001, 0x80000000, 0x3f800000, INEXACT|UNDERFLOW)
T5(RU,0x80000002, 0x80000001, 0x3f800000, INEXACT|UNDERFLOW)
T5(RU,0x80200000, 0x801fffff, 0x3f800000, INEXACT|UNDERFLOW)
T5(RU,0x80400000, 0x803fffff, 0x3f800000, INEXACT|UNDERFLOW)
T5(RU,0x807ffffe, 0x807ffffd, 0x3f800000, INEXACT|UNDERFLOW)
T5(RU,0x807fffff, 0x807ffffe, 0x3f800000, INEXACT|UNDERFLOW)
T5(RU,0x80800000, 0x807fffff, 0x3f800000, INEXACT|UNDERFLOW)
T5(RU,0x7fc00000, 0x7fc00000, 0x00000000, 0)
T5(RU,0x7fc00000, 0x7fc00000, 0x00000000, 0)
T5(RU,0x7f800000, 0x7fc00000, 0x00000000, INVALID)
T5(RU,0xff800000, 0x7fc00000, 0x00000000, INVALID)
T5(RU,0xb2800000, 0xb27fffff, 0x3f800000, INEXACT)
T5(RU,0xb9800000, 0xb97fffff, 0x3f555555, INEXACT)
T5(RU,0xb9c00000, 0xb9bfffff, 0x3f380000, INEXACT)
T5(RZ,0x00800001, 0x00800000, 0xbf800000, INEXACT)
T5(RZ,0x00800002, 0x00800001, 0xbf800000, INEXACT)
T5(RZ,0x01000000, 0x00ffffff, 0xbf800000, INEXACT)
T5(RZ,0x01800000, 0x017fffff, 0xbf800000, INEXACT)
T5(RZ,0x80800001, 0x80800000, 0x3f800000, INEXACT)
T5(RZ,0x80800002, 0x80800001, 0x3f800000, INEXACT)
T5(RZ,0x81000000, 0x80ffffff, 0x3f800000, INEXACT)
T5(RZ,0x81800000, 0x817fffff, 0x3f800000, INEXACT)
T5(RZ,0xb2400000, 0xb23fffff, 0x3f800000, INEXACT)
T5(RZ,0x00000001, 0x00000000, 0xbf800000, INEXACT|UNDERFLOW)
T5(RZ,0x00000002, 0x00000001, 0xbf800000, INEXACT|UNDERFLOW)
T5(RZ,0x00200000, 0x001fffff, 0xbf800000, INEXACT|UNDERFLOW)
T5(RZ,0x00400000, 0x003fffff, 0xbf800000, INEXACT|UNDERFLOW)
T5(RZ,0x007ffffe, 0x007ffffd, 0xbf800000, INEXACT|UNDERFLOW)
T5(RZ,0x007fffff, 0x007ffffe, 0xbf800000, INEXACT|UNDERFLOW)
T5(RZ,0x00800000, 0x007fffff, 0xbf800000, INEXACT|UNDERFLOW)
T5(RZ,0x80000001, 0x80000000, 0x3f800000, INEXACT|UNDERFLOW)
T5(RZ,0x80000002, 0x80000001, 0x3f800000, INEXACT|UNDERFLOW)
T5(RZ,0x80200000, 0x801fffff, 0x3f800000, INEXACT|UNDERFLOW)
T5(RZ,0x80400000, 0x803fffff, 0x3f800000, INEXACT|UNDERFLOW)
T5(RZ,0x807ffffe, 0x807ffffd, 0x3f800000, INEXACT|UNDERFLOW)
T5(RZ,0x807fffff, 0x807ffffe, 0x3f800000, INEXACT|UNDERFLOW)
T5(RZ,0x80800000, 0x807fffff, 0x3f800000, INEXACT|UNDERFLOW)
T5(RZ,0x7fc00000, 0x7fc00000, 0x00000000, 0)
T5(RZ,0x7fc00000, 0x7fc00000, 0x00000000, 0)
T5(RZ,0x7f800000, 0x7fc00000, 0x00000000, INVALID)
T5(RZ,0xff800000, 0x7fc00000, 0x00000000, INVALID)
T5(RZ,0x32400000, 0x323fffff, 0xbf800000, INEXACT)
T5(RZ,0x32800000, 0x327fffff, 0xbf800000, INEXACT)
T5(RZ,0x39800000, 0x397fffff, 0xbf555555, INEXACT)
T5(RZ,0x39c00000, 0x39bfffff, 0xbf380000, INEXACT)
T5(RZ,0xb2800000, 0xb27fffff, 0x3f800000, INEXACT)
T5(RZ,0xb9800000, 0xb97fffff, 0x3f555555, INEXACT)
T5(RZ,0xb9c00000, 0xb9bfffff, 0x3f380000, INEXACT)
