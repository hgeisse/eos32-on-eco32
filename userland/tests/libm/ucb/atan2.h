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
// atan2s(+-0,+anything but nan) is +-0
T6(RN,0x00000000, 0x00000000, 0x00000000, 0x00000000, 0)
T6(RN,0x00000000, 0x00000001, 0x00000000, 0x00000000, 0)
T6(RN,0x00000000, 0x00100000, 0x00000000, 0x00000000, 0)
T6(RN,0x00000000, 0x7f7fffff, 0x00000000, 0x00000000, 0)
T6(RN,0x00000000, 0x7f800000, 0x00000000, 0x00000000, 0)
T6(RN,0x80000000, 0x00000000, 0x80000000, 0x00000000, 0)
T6(RN,0x80000000, 0x00000001, 0x80000000, 0x00000000, 0)
T6(RN,0x80000000, 0x00100000, 0x80000000, 0x00000000, 0)
T6(RN,0x80000000, 0x7f7fffff, 0x80000000, 0x00000000, 0)
T6(RN,0x80000000, 0x7f800000, 0x80000000, 0x00000000, 0)
// atan2s(+-0, -anything but nan) is +-pi
T6(RN,0x00000000, 0xff800000, 0x40490fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x00000000, 0xff7fffff, 0x40490fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x00000000, 0x80100000, 0x40490fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x00000000, 0x80000001, 0x40490fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x00000000, 0x80000000, 0x40490fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x80000000, 0xff800000, 0xc0490fdb, 0xbebbbd2e, INEXACT)
T6(RN,0x80000000, 0xff7fffff, 0xc0490fdb, 0xbebbbd2e, INEXACT)
T6(RN,0x80000000, 0x80100000, 0xc0490fdb, 0xbebbbd2e, INEXACT)
T6(RN,0x80000000, 0x80000001, 0xc0490fdb, 0xbebbbd2e, INEXACT)
T6(RN,0x80000000, 0x80000000, 0xc0490fdb, 0xbebbbd2e, INEXACT)
//  atan2s(+-anything but 0 and nan, 0) is +- pi/2
T6(RN,0x7f800000, 0x00000000, 0x3fc90fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x7f800000, 0x80000000, 0x3fc90fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x7f7fffff, 0x00000000, 0x3fc90fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x7f7fffff, 0x80000000, 0x3fc90fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x00800000, 0x00000000, 0x3fc90fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x00800000, 0x80000000, 0x3fc90fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x00000001, 0x00000000, 0x3fc90fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x00000001, 0x80000000, 0x3fc90fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0xff800000, 0x00000000, 0xbfc90fdb, 0xbebbbd2e, INEXACT)
T6(RN,0xff800000, 0x80000000, 0xbfc90fdb, 0xbebbbd2e, INEXACT)
T6(RN,0xff7fffff, 0x00000000, 0xbfc90fdb, 0xbebbbd2e, INEXACT)
T6(RN,0xff7fffff, 0x80000000, 0xbfc90fdb, 0xbebbbd2e, INEXACT)
T6(RN,0x80800000, 0x00000000, 0xbfc90fdb, 0xbebbbd2e, INEXACT)
T6(RN,0x80800000, 0x80000000, 0xbfc90fdb, 0xbebbbd2e, INEXACT)
T6(RN,0x80000001, 0x00000000, 0xbfc90fdb, 0xbebbbd2e, INEXACT)
T6(RN,0x80000001, 0x80000000, 0xbfc90fdb, 0xbebbbd2e, INEXACT)
// atan2s(big,small) :=: +-pi/2
T6(RN,0x7f7fffff, 0x00800000, 0x3fc90fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x7f7fffff, 0x80800000, 0x3fc90fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0xff7fffff, 0x00800000, 0xbfc90fdb, 0xbebbbd2e, INEXACT)
T6(RN,0xff7fffff, 0x80800000, 0xbfc90fdb, 0xbebbbd2e, INEXACT)
T6(RZ,0x7f7fffff, 0x00800000, 0x3fc90fda, 0xbf222169, INEXACT)
T6(RZ,0x7f7fffff, 0x80800000, 0x3fc90fda, 0xbf222169, INEXACT)
T6(RZ,0xff7fffff, 0x00800000, 0xbfc90fda, 0x3f222169, INEXACT)
T6(RZ,0xff7fffff, 0x80800000, 0xbfc90fda, 0x3f222169, INEXACT)
T6(RU,0x7f7fffff, 0x00800000, 0x3fc90fdb, 0x3ebbbd2e, INEXACT)
T6(RU,0x7f7fffff, 0x80800000, 0x3fc90fdb, 0x3ebbbd2e, INEXACT)
T6(RU,0xff7fffff, 0x00800000, 0xbfc90fda, 0x3f222169, INEXACT)
T6(RU,0xff7fffff, 0x80800000, 0xbfc90fda, 0x3f222169, INEXACT)
T6(RD,0x7f7fffff, 0x00800000, 0x3fc90fda, 0xbf222169, INEXACT)
T6(RD,0x7f7fffff, 0x80800000, 0x3fc90fda, 0xbf222169, INEXACT)
T6(RD,0xff7fffff, 0x00800000, 0xbfc90fdb, 0xbebbbd2e, INEXACT)
T6(RD,0xff7fffff, 0x80800000, 0xbfc90fdb, 0xbebbbd2e, INEXACT)
// atan2s(small,big) = small/big (big>0)
T6(RN,0x00800000, 0x7f7fffff, 0x00000000, 0x8b000001, INEXACT|UNDERFLOW)
T6(RN,0x80800000, 0x7f7fffff, 0x80000000, 0x0b000001, INEXACT|UNDERFLOW)
T6(RN,0x00800000, 0xff7fffff, 0x40490fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x80800000, 0xff7fffff, 0xc0490fdb, 0xbebbbd2e, INEXACT)
T6(RZ,0x00800000, 0x7f7fffff, 0x00000000, 0x8b000001, INEXACT|UNDERFLOW)
T6(RZ,0x80800000, 0x7f7fffff, 0x80000000, 0x0b000001, INEXACT|UNDERFLOW)
T6(RZ,0x00800000, 0xff7fffff, 0x40490fda, 0xbf222169, INEXACT)
T6(RZ,0x80800000, 0xff7fffff, 0xc0490fda, 0x3f222169, INEXACT)
T6(RU,0x00800000, 0x7f7fffff, 0x00000001, 0x3f800000, INEXACT|UNDERFLOW)
T6(RU,0x80800000, 0x7f7fffff, 0x80000000, 0x0b000001, INEXACT|UNDERFLOW)
T6(RU,0x00800000, 0xff7fffff, 0x40490fdb, 0x3ebbbd2e, INEXACT)
T6(RU,0x80800000, 0xff7fffff, 0xc0490fda, 0x3f222169, INEXACT)
T6(RD,0x00800000, 0x7f7fffff, 0x00000000, 0x8b000001, INEXACT|UNDERFLOW)
T6(RD,0x80800000, 0x7f7fffff, 0x80000001, 0xbf800000, INEXACT|UNDERFLOW)
T6(RD,0x00800000, 0xff7fffff, 0x40490fda, 0xbf222169, INEXACT)
T6(RD,0x80800000, 0xff7fffff, 0xc0490fdb, 0xbebbbd2e, INEXACT)
// atan2s(+-x,+x) = +-pi/4 for normal x
T6(RN,0x00800000, 0x00800000, 0x3f490fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x80800000, 0x00800000, 0xbf490fdb, 0xbebbbd2e, INEXACT)
T6(RN,0x7f7fffff, 0x7f7fffff, 0x3f490fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0xff7fffff, 0x7f7fffff, 0xbf490fdb, 0xbebbbd2e, INEXACT)
// atan2s(+-x, -x) = +-3pi/4 for normal x
T6(RN,0x00800000, 0x80800000, 0x4016cbe4, 0x3cccde2e, INEXACT)
T6(RN,0x80800000, 0x80800000, 0xc016cbe4, 0xbcccde2e, INEXACT)
T6(RN,0x3f800000, 0xbf800000, 0x4016cbe4, 0x3cccde2e, INEXACT)
T6(RN,0xbf800000, 0xbf800000, 0xc016cbe4, 0xbcccde2e, INEXACT)
T6(RN,0x7f7fffff, 0xff7fffff, 0x4016cbe4, 0x3cccde2e, INEXACT)
T6(RN,0xff7fffff, 0xff7fffff, 0xc016cbe4, 0xbcccde2e, INEXACT)
// random arguments between -2.0 and 2.0
T6(RN,0xbf099426, 0x3fb65470, 0xbeb8b690, 0x3e480888, INEXACT)
T6(RN,0x3fe16530, 0xbfaaf890, 0x400e1037, 0x3e8d6021, INEXACT)
T6(RN,0xbf0ab3cf, 0xbda0f099, 0xbfdb7fa2, 0xbd97657a, INEXACT)
T6(RN,0x3f940d87, 0x3fdae71a, 0x3f183c4e, 0x3e0bf30d, INEXACT)
T6(RN,0xbeac1a41, 0xbff54112, 0xc03df284, 0x3e726304, INEXACT)
T6(RN,0xbf5570c0, 0x3dbd4ed1, 0xbfbaedbe, 0x3ca41dbe, INEXACT)
T6(RN,0xbfc22800, 0x3fb65191, 0xbf511bd9, 0xbe719310, INEXACT)
T6(RN,0xbfff04df, 0xbfffb63f, 0xc016e21e, 0x3d9a4609, INEXACT)
T6(RN,0xbf692641, 0x3f94db41, 0xbf2a1451, 0x3e72cff2, INEXACT)
T6(RN,0x3f12f53f, 0x3f7ccacc, 0x3f06ce4b, 0xbc1f7c76, INEXACT)
// atan2s involve nan
T6(RN,0x00000000, 0x7fc00000, 0x7fc00000, 0x00000000, 0)
T6(RN,0x3f800000, 0x7fc00000, 0x7fc00000, 0x00000000, 0)
T6(RN,0x7fc00000, 0x00000001, 0x7fc00000, 0x00000000, 0)
T6(RN,0x7fc00000, 0xff7fffff, 0x7fc00000, 0x00000000, 0)
T6(RN,0x7fc00000, 0x7fc00000, 0x7fc00000, 0x00000000, 0)
T6(RN,0x7fc00000, 0x7fc00000, 0x7fc00000, 0x00000000, 0)
T6(RZ,0x00000000, 0x7fc00000, 0x7fc00000, 0x00000000, 0)
T6(RZ,0x3f800000, 0x7fc00000, 0x7fc00000, 0x00000000, 0)
T6(RZ,0x7fc00000, 0x00000001, 0x7fc00000, 0x00000000, 0)
T6(RZ,0x7fc00000, 0xff7fffff, 0x7fc00000, 0x00000000, 0)
T6(RZ,0x7fc00000, 0x7fc00000, 0x7fc00000, 0x00000000, 0)
T6(RZ,0x7fc00000, 0x7fc00000, 0x7fc00000, 0x00000000, 0)
T6(RU,0x00000000, 0x7fc00000, 0x7fc00000, 0x00000000, 0)
T6(RU,0x3f800000, 0x7fc00000, 0x7fc00000, 0x00000000, 0)
T6(RU,0x7fc00000, 0x00000001, 0x7fc00000, 0x00000000, 0)
T6(RU,0x7fc00000, 0xff7fffff, 0x7fc00000, 0x00000000, 0)
T6(RU,0x7fc00000, 0x7fc00000, 0x7fc00000, 0x00000000, 0)
T6(RU,0x7fc00000, 0x7fc00000, 0x7fc00000, 0x00000000, 0)
T6(RD,0x00000000, 0x7fc00000, 0x7fc00000, 0x00000000, 0)
T6(RD,0x3f800000, 0x7fc00000, 0x7fc00000, 0x00000000, 0)
T6(RD,0x7fc00000, 0x00000001, 0x7fc00000, 0x00000000, 0)
T6(RD,0x7fc00000, 0xff7fffff, 0x7fc00000, 0x00000000, 0)
T6(RD,0x7fc00000, 0x7fc00000, 0x7fc00000, 0x00000000, 0)
T6(RD,0x7fc00000, 0x7fc00000, 0x7fc00000, 0x00000000, 0)
//  atan2s(+-(anything but inf and nan), +inf) is +-0
T6(RN,0x00000001, 0x7f800000, 0x00000000, 0x00000000, 0)
T6(RN,0x7f7fffff, 0x7f800000, 0x00000000, 0x00000000, 0)
T6(RN,0x80000001, 0x7f800000, 0x80000000, 0x00000000, 0)
T6(RN,0xff7fffff, 0x7f800000, 0x80000000, 0x00000000, 0)
//  atan2s(+-(anything but inf and nan), -inf) is +-pi
T6(RN,0x00000001, 0xff800000, 0x40490fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x7f7fffff, 0xff800000, 0x40490fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x80000001, 0xff800000, 0xc0490fdb, 0xbebbbd2e, INEXACT)
T6(RN,0xff7fffff, 0xff800000, 0xc0490fdb, 0xbebbbd2e, INEXACT)
//  atan2s(+-inf,+inf ) is +-pi/4
T6(RN,0x7f800000, 0x7f800000, 0x3f490fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0xff800000, 0x7f800000, 0xbf490fdb, 0xbebbbd2e, INEXACT)
//  atan2s(+-inf, -inf ) is +-3pi/4
T6(RN,0x7f800000, 0xff800000, 0x4016cbe4, 0x3cccde2e, INEXACT)
T6(RN,0xff800000, 0xff800000, 0xc016cbe4, 0xbcccde2e, INEXACT)
//  atan2s(+-inf, (anything but, 0,nan, and inf)) is +-pi/2
T6(RN,0x7f800000, 0x00000001, 0x3fc90fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x7f800000, 0x80000001, 0x3fc90fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x7f800000, 0x7f7fffff, 0x3fc90fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0x7f800000, 0xff7fffff, 0x3fc90fdb, 0x3ebbbd2e, INEXACT)
T6(RN,0xff800000, 0x00000001, 0xbfc90fdb, 0xbebbbd2e, INEXACT)
T6(RN,0xff800000, 0x80000001, 0xbfc90fdb, 0xbebbbd2e, INEXACT)
T6(RN,0xff800000, 0x7f7fffff, 0xbfc90fdb, 0xbebbbd2e, INEXACT)
T6(RN,0xff800000, 0xff7fffff, 0xbfc90fdb, 0xbebbbd2e, INEXACT)
