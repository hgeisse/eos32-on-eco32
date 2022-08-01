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
// THE SOFTWARE IS MADE AVAILABLE "AS IS" AND WITHOUT EXPRESS OR IMPLIED
// WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO THE IMPLIED
// WARRANTIES OF DESIGN, MERCHANTIBILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT, PERFORMANCE OR CONFORMANCE TO
// SPECIFICATIONS.
//
// BY DOWNLOADING AND/OR USING THIS SOFTWARE, THE USER WAIVES ALL CLAIMS
// AGAINST SUN MICROSYSTEMS, INC. AND ITS AFFILIATED COMPANIES IN ANY
// JURISDICTION, INCLUDING BUT NOT LIMITED TO CLAIMS FOR DAMAGES OR
// EQUITABLE RELIEF BASED ON LOSS OF DATA, AND SPECIFICALLY WAIVES EVEN
// UNKNOWN OR UNANTICIPATED CLAIMS OR LOSSES, PRESENT AND FUTURE.
//
// IN NO EVENT WILL SUN MICROSYSTEMS, INC. OR ANY OF ITS AFFILIATED
// COMPANIES BE LIABLE FOR ANY LOST REVENUE OR PROFITS OR OTHER SPECIAL,
// INDIRECT AND CONSEQUENTIAL DAMAGES, EVEN IF IT HAS BEEN ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGES.
//
// This file is provided with no support and without any obligation on the
// part of Sun Microsystems, Inc. ("Sun") or any of its affiliated
// companies to assist in its use, correction, modification or
// enhancement.  Nevertheless, and without creating any obligation on its
// part, Sun welcomes your comments concerning the software and requests
// that they be sent to fdlibm-comments@sunpro.sun.com.
// sinh(log(2*max)chopped) is finite
T5(RN,0x42b2d4fc, 0x7f7fffec, 0xbdfa39ca, INEXACT)
T5(RN,0xc2b2d4fc, 0xff7fffec, 0x3dfa39ca, INEXACT)
T5(RZ,0x42b2d4fc, 0x7f7fffec, 0xbdfa39ca, INEXACT)
T5(RZ,0xc2b2d4fc, 0xff7fffec, 0x3dfa39ca, INEXACT)
T5(RU,0x42b2d4fc, 0x7f7fffed, 0x3f60b8c7, INEXACT)
T5(RU,0xc2b2d4fc, 0xff7fffec, 0x3dfa39ca, INEXACT)
T5(RD,0x42b2d4fc, 0x7f7fffec, 0xbdfa39ca, INEXACT)
T5(RD,0xc2b2d4fc, 0xff7fffed, 0xbf60b8c7, INEXACT)
// sinh(tiny) :=: tiny
T5(RN,0x31000000, 0x31000000, 0xacaaaaab, INEXACT)
T5(RN,0xb1000000, 0xb1000000, 0x2caaaaab, INEXACT)
T5(RN,0x00800000, 0x00800000, 0x00000000, INEXACT)
T5(RN,0x80800000, 0x80800000, 0x00000000, INEXACT)
// sinh(+-0) = +-0
T5(RN,0x00000000, 0x00000000, 0x00000000, 0)
T5(RN,0x80000000, 0x80000000, 0x00000000, 0)
T5(RZ,0x00000000, 0x00000000, 0x00000000, 0)
T5(RZ,0x80000000, 0x80000000, 0x00000000, 0)
T5(RU,0x00000000, 0x00000000, 0x00000000, 0)
T5(RU,0x80000000, 0x80000000, 0x00000000, 0)
T5(RD,0x00000000, 0x00000000, 0x00000000, 0)
T5(RD,0x80000000, 0x80000000, 0x00000000, 0)
// random arguments between -30 30
T5(RN,0xc100fae6, 0xc4c61436, 0xbd4df754, INEXACT)
T5(RN,0x41aaef28, 0x4e62df7c, 0xbef8c33b, INEXACT)
T5(RN,0x41d34ed8, 0x5209d175, 0x3ea86458, INEXACT)
T5(RN,0xc1a04908, 0xcd6fbe4d, 0xbeeef7db, INEXACT)
T5(RN,0xc1020897, 0xc4d38f58, 0xbeb78286, INEXACT)
T5(RN,0xbf96e191, 0xbfbc54d1, 0x3d816148, INEXACT)
T5(RN,0x418accaf, 0x4b82c03a, 0xbeac6300, INEXACT)
T5(RN,0x41cd38a2, 0x5180cc07, 0x3ef48065, INEXACT)
T5(RN,0xc0a15899, 0xc29ac86b, 0x3e91110e, INEXACT)
T5(RN,0xc1e5ed02, 0xd3b09225, 0xbec79756, INEXACT)
// sinh(nan) is nan
T5(RN,0x7fc00000, 0x7fc00000, 0x00000000, 0)
T5(RN,0x7fc00000, 0x7fc00000, 0x00000000, 0)
// sinh(+-inf) is inf
T5(RN,0x7f800000, 0x7f800000, 0x00000000, 0)
T5(RN,0xff800000, 0xff800000, 0x00000000, 0)
T5(RZ,0x7f800000, 0x7f800000, 0x00000000, 0)
T5(RZ,0xff800000, 0xff800000, 0x00000000, 0)
T5(RU,0x7f800000, 0x7f800000, 0x00000000, 0)
T5(RU,0xff800000, 0xff800000, 0x00000000, 0)
T5(RD,0x7f800000, 0x7f800000, 0x00000000, 0)
T5(RD,0xff800000, 0xff800000, 0x00000000, 0)
// sinh(+-subnormal)
T5(RN,0x00000001, 0x00000001, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x80000001, 0x80000001, 0x00000000, INEXACT|UNDERFLOW)
// sinh overflow threshold
T5(RN,0x42b2d4fd, 0x7f800000, 0x00000000, INEXACT|OVERFLOW)
T5(RN,0xc2b2d4fd, 0xff800000, 0x00000000, INEXACT|OVERFLOW)
T5(RZ,0x42b2d4fd, 0x7f7fffff, 0xbf800000, INEXACT|OVERFLOW)
T5(RZ,0xc2b2d4fd, 0xff7fffff, 0x3f800000, INEXACT|OVERFLOW)
T5(RU,0x42b2d4fd, 0x7f800000, 0x00000000, INEXACT|OVERFLOW)
T5(RU,0xc2b2d4fd, 0xff7fffff, 0x3f800000, INEXACT|OVERFLOW)
T5(RD,0x42b2d4fd, 0x7f7fffff, 0xbf800000, INEXACT|OVERFLOW)
T5(RD,0xc2b2d4fd, 0xff800000, 0x00000000, INEXACT|OVERFLOW)
T5(RD,0x00800000, 0x00800000, 0x00000000, INEXACT)
T5(RD,0x00800001, 0x00800001, 0x00000000, INEXACT)
T5(RD,0x00800002, 0x00800002, 0x00000000, INEXACT)
T5(RD,0x01000000, 0x01000000, 0x00000000, INEXACT)
T5(RD,0x01800000, 0x01800000, 0x00000000, INEXACT)
T5(RD,0x32800000, 0x32800000, 0xafaaaaab, INEXACT)
T5(RD,0x32c00000, 0x32c00000, 0xb0900000, INEXACT)
T5(RD,0x33000000, 0x33000000, 0xb0aaaaab, INEXACT)
T5(RD,0x33200000, 0x33200000, 0xb126aaab, INEXACT)
T5(RD,0x39c00000, 0x39c00000, 0xbe900000, INEXACT)
T5(RD,0x80800000, 0x80800001, 0xbf800000, INEXACT)
T5(RD,0x80800001, 0x80800002, 0xbf800000, INEXACT)
T5(RD,0x80800002, 0x80800003, 0xbf800000, INEXACT)
T5(RD,0x81000000, 0x81000001, 0xbf800000, INEXACT)
T5(RD,0x81800000, 0x81800001, 0xbf800000, INEXACT)
T5(RD,0x00000001, 0x00000001, 0x00000000, INEXACT|UNDERFLOW)
T5(RD,0x00000002, 0x00000002, 0x00000000, INEXACT|UNDERFLOW)
T5(RD,0x00200000, 0x00200000, 0x00000000, INEXACT|UNDERFLOW)
T5(RD,0x00400000, 0x00400000, 0x00000000, INEXACT|UNDERFLOW)
T5(RD,0x007ffffe, 0x007ffffe, 0x00000000, INEXACT|UNDERFLOW)
T5(RD,0x007fffff, 0x007fffff, 0x00000000, INEXACT|UNDERFLOW)
T5(RD,0x80000001, 0x80000002, 0xbf800000, INEXACT|UNDERFLOW)
T5(RD,0x80000002, 0x80000003, 0xbf800000, INEXACT|UNDERFLOW)
T5(RD,0x80200000, 0x80200001, 0xbf800000, INEXACT|UNDERFLOW)
T5(RD,0x80400000, 0x80400001, 0xbf800000, INEXACT|UNDERFLOW)
T5(RD,0x807ffffe, 0x807fffff, 0xbf800000, INEXACT|UNDERFLOW)
T5(RD,0x807fffff, 0x80800000, 0xbf800000, INEXACT|UNDERFLOW)
T5(RD,0x42b30000, 0x7f7fffff, 0xbf800000, INEXACT|OVERFLOW)
T5(RD,0x4431a000, 0x7f7fffff, 0xbf800000, INEXACT|OVERFLOW)
T5(RD,0x7e800000, 0x7f7fffff, 0xbf800000, INEXACT|OVERFLOW)
T5(RD,0x7f000000, 0x7f7fffff, 0xbf800000, INEXACT|OVERFLOW)
T5(RD,0x7f7ffffe, 0x7f7fffff, 0xbf800000, INEXACT|OVERFLOW)
T5(RD,0x7f7fffff, 0x7f7fffff, 0xbf800000, INEXACT|OVERFLOW)
T5(RD,0xc2b30000, 0xff800000, 0x00000000, INEXACT|OVERFLOW)
T5(RD,0xc431a000, 0xff800000, 0x00000000, INEXACT|OVERFLOW)
T5(RD,0xfe800000, 0xff800000, 0x00000000, INEXACT|OVERFLOW)
T5(RD,0xff000000, 0xff800000, 0x00000000, INEXACT|OVERFLOW)
T5(RD,0xff7ffffe, 0xff800000, 0x00000000, INEXACT|OVERFLOW)
T5(RD,0xff7fffff, 0xff800000, 0x00000000, INEXACT|OVERFLOW)
T5(RD,0x7fc00000, 0x7fc00000, 0x00000000, 0)
T5(RD,0x7fc00000, 0x7fc00000, 0x00000000, 0)
T5(RD,0xb2800000, 0xb2800001, 0xbf800000, INEXACT)
T5(RD,0xb2c00000, 0xb2c00001, 0xbf800000, INEXACT)
T5(RD,0xb3000000, 0xb3000001, 0xbf800000, INEXACT)
T5(RD,0xb3200000, 0xb3200001, 0xbf800000, INEXACT)
T5(RD,0xb9c00000, 0xb9c00001, 0xbf380000, INEXACT)
T5(RN,0x00800001, 0x00800001, 0x00000000, INEXACT)
T5(RN,0x00800002, 0x00800002, 0x00000000, INEXACT)
T5(RN,0x01000000, 0x01000000, 0x00000000, INEXACT)
T5(RN,0x01800000, 0x01800000, 0x00000000, INEXACT)
T5(RN,0x32800000, 0x32800000, 0xafaaaaab, INEXACT)
T5(RN,0x32c00000, 0x32c00000, 0xb0900000, INEXACT)
T5(RN,0x33000000, 0x33000000, 0xb0aaaaab, INEXACT)
T5(RN,0x33200000, 0x33200000, 0xb126aaab, INEXACT)
T5(RN,0x39c00000, 0x39c00000, 0xbe900000, INEXACT)
T5(RN,0x80800001, 0x80800001, 0x00000000, INEXACT)
T5(RN,0x80800002, 0x80800002, 0x00000000, INEXACT)
T5(RN,0x81000000, 0x81000000, 0x00000000, INEXACT)
T5(RN,0x81800000, 0x81800000, 0x00000000, INEXACT)
T5(RN,0xb2800000, 0xb2800000, 0x2faaaaab, INEXACT)
T5(RN,0xb2c00000, 0xb2c00000, 0x30900000, INEXACT)
T5(RN,0xb3000000, 0xb3000000, 0x30aaaaab, INEXACT)
T5(RN,0xb3200000, 0xb3200000, 0x3126aaab, INEXACT)
T5(RN,0xb9c00000, 0xb9c00000, 0x3e900000, INEXACT)
T5(RN,0x00000002, 0x00000002, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x00200000, 0x00200000, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x00400000, 0x00400000, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x007ffffe, 0x007ffffe, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x007fffff, 0x007fffff, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x80000002, 0x80000002, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x80200000, 0x80200000, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x80400000, 0x80400000, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x807ffffe, 0x807ffffe, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x807fffff, 0x807fffff, 0x00000000, INEXACT|UNDERFLOW)
T5(RN,0x42b30000, 0x7f800000, 0x00000000, INEXACT|OVERFLOW)
T5(RN,0x4431a000, 0x7f800000, 0x00000000, INEXACT|OVERFLOW)
T5(RN,0x7e800000, 0x7f800000, 0x00000000, INEXACT|OVERFLOW)
T5(RN,0x7f000000, 0x7f800000, 0x00000000, INEXACT|OVERFLOW)
T5(RN,0x7f7ffffe, 0x7f800000, 0x00000000, INEXACT|OVERFLOW)
T5(RN,0x7f7fffff, 0x7f800000, 0x00000000, INEXACT|OVERFLOW)
T5(RN,0xc2b30000, 0xff800000, 0x00000000, INEXACT|OVERFLOW)
T5(RN,0xc431a000, 0xff800000, 0x00000000, INEXACT|OVERFLOW)
T5(RN,0xfe800000, 0xff800000, 0x00000000, INEXACT|OVERFLOW)
T5(RN,0xff000000, 0xff800000, 0x00000000, INEXACT|OVERFLOW)
T5(RN,0xff7ffffe, 0xff800000, 0x00000000, INEXACT|OVERFLOW)
T5(RN,0xff7fffff, 0xff800000, 0x00000000, INEXACT|OVERFLOW)
T5(RU,0x32c00000, 0x32c00001, 0x3f800000, INEXACT)
T5(RU,0x39c00000, 0x39c00001, 0x3f380000, INEXACT)
T5(RU,0x80800000, 0x80800000, 0x00000000, INEXACT)
T5(RU,0x80800001, 0x80800001, 0x00000000, INEXACT)
T5(RU,0x80800002, 0x80800002, 0x00000000, INEXACT)
T5(RU,0x81000000, 0x81000000, 0x00000000, INEXACT)
T5(RU,0x81800000, 0x81800000, 0x00000000, INEXACT)
T5(RU,0x80000001, 0x80000001, 0x00000000, INEXACT|UNDERFLOW)
T5(RU,0x80000002, 0x80000002, 0x00000000, INEXACT|UNDERFLOW)
T5(RU,0x80200000, 0x80200000, 0x00000000, INEXACT|UNDERFLOW)
T5(RU,0x80400000, 0x80400000, 0x00000000, INEXACT|UNDERFLOW)
T5(RU,0x807ffffe, 0x807ffffe, 0x00000000, INEXACT|UNDERFLOW)
T5(RU,0x807fffff, 0x807fffff, 0x00000000, INEXACT|UNDERFLOW)
T5(RU,0x42b30000, 0x7f800000, 0x00000000, INEXACT|OVERFLOW)
T5(RU,0x4431a000, 0x7f800000, 0x00000000, INEXACT|OVERFLOW)
T5(RU,0x7e800000, 0x7f800000, 0x00000000, INEXACT|OVERFLOW)
T5(RU,0x7f000000, 0x7f800000, 0x00000000, INEXACT|OVERFLOW)
T5(RU,0x7f7ffffe, 0x7f800000, 0x00000000, INEXACT|OVERFLOW)
T5(RU,0x7f7fffff, 0x7f800000, 0x00000000, INEXACT|OVERFLOW)
T5(RU,0xc2b30000, 0xff7fffff, 0x3f800000, INEXACT|OVERFLOW)
T5(RU,0xc431a000, 0xff7fffff, 0x3f800000, INEXACT|OVERFLOW)
T5(RU,0xfe800000, 0xff7fffff, 0x3f800000, INEXACT|OVERFLOW)
T5(RU,0xff000000, 0xff7fffff, 0x3f800000, INEXACT|OVERFLOW)
T5(RU,0xff7ffffe, 0xff7fffff, 0x3f800000, INEXACT|OVERFLOW)
T5(RU,0xff7fffff, 0xff7fffff, 0x3f800000, INEXACT|OVERFLOW)
T5(RU,0x7fc00000, 0x7fc00000, 0x00000000, 0)
T5(RU,0x7fc00000, 0x7fc00000, 0x00000000, 0)
T5(RU,0x00800000, 0x00800001, 0x3f800000, INEXACT)
T5(RU,0x00800001, 0x00800002, 0x3f800000, INEXACT)
T5(RU,0x00800002, 0x00800003, 0x3f800000, INEXACT)
T5(RU,0x01000000, 0x01000001, 0x3f800000, INEXACT)
T5(RU,0x01800000, 0x01800001, 0x3f800000, INEXACT)
T5(RU,0x32800000, 0x32800001, 0x3f800000, INEXACT)
T5(RU,0x33000000, 0x33000001, 0x3f800000, INEXACT)
T5(RU,0x33200000, 0x33200001, 0x3f800000, INEXACT)
T5(RU,0xb2800000, 0xb2800000, 0x2faaaaab, INEXACT)
T5(RU,0xb2c00000, 0xb2c00000, 0x30900000, INEXACT)
T5(RU,0xb3000000, 0xb3000000, 0x30aaaaab, INEXACT)
T5(RU,0xb3200000, 0xb3200000, 0x3126aaab, INEXACT)
T5(RU,0xb9c00000, 0xb9c00000, 0x3e900000, INEXACT)
T5(RU,0x00000001, 0x00000002, 0x3f800000, INEXACT|UNDERFLOW)
T5(RU,0x00000002, 0x00000003, 0x3f800000, INEXACT|UNDERFLOW)
T5(RU,0x00200000, 0x00200001, 0x3f800000, INEXACT|UNDERFLOW)
T5(RU,0x00400000, 0x00400001, 0x3f800000, INEXACT|UNDERFLOW)
T5(RU,0x007ffffe, 0x007fffff, 0x3f800000, INEXACT|UNDERFLOW)
T5(RU,0x007fffff, 0x00800000, 0x3f800000, INEXACT|UNDERFLOW)
T5(RZ,0x00800000, 0x00800000, 0x00000000, INEXACT)
T5(RZ,0x00800001, 0x00800001, 0x00000000, INEXACT)
T5(RZ,0x00800002, 0x00800002, 0x00000000, INEXACT)
T5(RZ,0x01000000, 0x01000000, 0x00000000, INEXACT)
T5(RZ,0x01800000, 0x01800000, 0x00000000, INEXACT)
T5(RZ,0x32800000, 0x32800000, 0xafaaaaab, INEXACT)
T5(RZ,0x32c00000, 0x32c00000, 0xb0900000, INEXACT)
T5(RZ,0x33000000, 0x33000000, 0xb0aaaaab, INEXACT)
T5(RZ,0x33200000, 0x33200000, 0xb126aaab, INEXACT)
T5(RZ,0x39c00000, 0x39c00000, 0xbe900000, INEXACT)
T5(RZ,0x80800000, 0x80800000, 0x00000000, INEXACT)
T5(RZ,0x80800001, 0x80800001, 0x00000000, INEXACT)
T5(RZ,0x80800002, 0x80800002, 0x00000000, INEXACT)
T5(RZ,0x81000000, 0x81000000, 0x00000000, INEXACT)
T5(RZ,0x81800000, 0x81800000, 0x00000000, INEXACT)
T5(RZ,0x00000001, 0x00000001, 0x00000000, INEXACT|UNDERFLOW)
T5(RZ,0x00000002, 0x00000002, 0x00000000, INEXACT|UNDERFLOW)
T5(RZ,0x00200000, 0x00200000, 0x00000000, INEXACT|UNDERFLOW)
T5(RZ,0x00400000, 0x00400000, 0x00000000, INEXACT|UNDERFLOW)
T5(RZ,0x007ffffe, 0x007ffffe, 0x00000000, INEXACT|UNDERFLOW)
T5(RZ,0x007fffff, 0x007fffff, 0x00000000, INEXACT|UNDERFLOW)
T5(RZ,0x80000001, 0x80000001, 0x00000000, INEXACT|UNDERFLOW)
T5(RZ,0x80000002, 0x80000002, 0x00000000, INEXACT|UNDERFLOW)
T5(RZ,0x80200000, 0x80200000, 0x00000000, INEXACT|UNDERFLOW)
T5(RZ,0x80400000, 0x80400000, 0x00000000, INEXACT|UNDERFLOW)
T5(RZ,0x807ffffe, 0x807ffffe, 0x00000000, INEXACT|UNDERFLOW)
T5(RZ,0x807fffff, 0x807fffff, 0x00000000, INEXACT|UNDERFLOW)
T5(RZ,0x42b30000, 0x7f7fffff, 0xbf800000, INEXACT|OVERFLOW)
T5(RZ,0x4431a000, 0x7f7fffff, 0xbf800000, INEXACT|OVERFLOW)
T5(RZ,0x7e800000, 0x7f7fffff, 0xbf800000, INEXACT|OVERFLOW)
T5(RZ,0x7f000000, 0x7f7fffff, 0xbf800000, INEXACT|OVERFLOW)
T5(RZ,0x7f7ffffe, 0x7f7fffff, 0xbf800000, INEXACT|OVERFLOW)
T5(RZ,0x7f7fffff, 0x7f7fffff, 0xbf800000, INEXACT|OVERFLOW)
T5(RZ,0xc2b30000, 0xff7fffff, 0x3f800000, INEXACT|OVERFLOW)
T5(RZ,0xc431a000, 0xff7fffff, 0x3f800000, INEXACT|OVERFLOW)
T5(RZ,0xfe800000, 0xff7fffff, 0x3f800000, INEXACT|OVERFLOW)
T5(RZ,0xff000000, 0xff7fffff, 0x3f800000, INEXACT|OVERFLOW)
T5(RZ,0xff7ffffe, 0xff7fffff, 0x3f800000, INEXACT|OVERFLOW)
T5(RZ,0xff7fffff, 0xff7fffff, 0x3f800000, INEXACT|OVERFLOW)
T5(RZ,0x7fc00000, 0x7fc00000, 0x00000000, 0)
T5(RZ,0x7fc00000, 0x7fc00000, 0x00000000, 0)
T5(RZ,0xb2800000, 0xb2800000, 0x2faaaaab, INEXACT)
T5(RZ,0xb2c00000, 0xb2c00000, 0x30900000, INEXACT)
T5(RZ,0xb3000000, 0xb3000000, 0x30aaaaab, INEXACT)
T5(RZ,0xb3200000, 0xb3200000, 0x3126aaab, INEXACT)
T5(RZ,0xb9c00000, 0xb9c00000, 0x3e900000, INEXACT)