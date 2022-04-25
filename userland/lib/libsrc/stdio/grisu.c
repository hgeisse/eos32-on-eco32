#include <stdio.h>
#include <math.h>
#include <assert.h>
/* 
    This file belongs to the EOS32-on-ECO32 project.
    It contains an implementation of the Grisu2b algorithm
    described in Florian Loitsch's paper 'Printing Floating-Point
    Numbers Quickly and Accurately with Integers'.
    The named paper contains some listings written in C,
    which we took for our implementation.
    These parts and functions are marked with a hint 
    'by Florian Loitsch'. The paper can be found here:

    https://drive.google.com/file/d/0BwvYOx00EwKmcTZUM0Q4VW1DTHc/view?resourcekey=0-gW-moqRGyWpqFsyhiBI_-w
    
    Further, there are parts of the code which we took
    from a MIT-licensed implementation by the GitHub user
    miloyip and his/her dtoa-benchmark project.
    As you expected, these parts are marked with the hint
    'by miloyip'. The sources can be found here:

    https://github.com/miloyip/dtoa-benchmark

    There are some modifications with this Grisu version.
    The original Grisu algorithm works with double precision (64 bit)
    floats. Our ECO32 provides just 32 bit arithmetics. So,
    this algorithm was modified for 32 bit doubles.
    IT WILL BREAK WITH 64 BIT DOUBLES.

    Thank you for reading!
    
    ~swlt16
*/

/* these types are usually defined in stdint.h ... */
typedef unsigned int uint32_t;
typedef int int32_t;

/* bit masks */
#define DIY_SIGNIFICAND_SIZE    32
#define DP_SIGNIFICAND_SIZE     23
#define DP_EXPONENT_BIAS        (0x7F + DP_SIGNIFICAND_SIZE)
#define DP_MIN_EXPONENT         (-DP_EXPONENT_BIAS)
#define DP_EXPONENT_MASK        0x7F800000
#define DP_SIGNIFICAND_MASK     0x007FFFFF
#define DP_HIDDEN_BIT           (1 << (23))

/* diy fp structure */
typedef struct fp {
    uint32_t    f;
    int32_t     e;
} fp_t;

/* prototypes */
static fp_t fp_subtract(fp_t, fp_t);
static fp_t fp_multiply(fp_t, fp_t);
static fp_t fp_normalize(fp_t);
static fp_t cached_power(int);
static int __xceil(double);
static int k_comp(int, int, int);
static uint32_t double_to_uint32(double);
static fp_t double_to_fp(double);
static void normalized_boundaries(double, fp_t*, fp_t*);
static void digit_gen(fp_t, fp_t, char*, int*, int*);
static void grisu2(double, char*, int*, int*);

static fp_t fp_subtract(fp_t x, fp_t y) {
    /* by Florian Loitsch (see file header) */
    fp_t r;
    assert((x.e == y.e) && (x.f >= y.f));
    r.f = x.f - y.f;
    r.e = x.e;
    return r;
}

static fp_t fp_multiply(fp_t x, fp_t y) {
    /* by Florian Loitsch (see file header) */
    uint32_t a, b, c, d, ac, bc, ad, bd, tmp;
    uint32_t M16 = 0xffff;
    fp_t r;
    a = x.f >> 16;
    b = x.f & M16;
    c = y.f >> 16;
    d = y.f & M16;
    ac = a*c;
    bc = b*c;
    ad = a*d;
    bd = b*d;
    tmp = (bd >> 16) + (ad & M16) + (bc & M16);
    tmp += 1U << 15; /* rounding */
    r.f = ac + (ad >> 16) + (bc >> 16) + (tmp >> 16);
    r.e = x.e + y.e + 32; /* ??? */
    return r;
}

static fp_t fp_normalize(fp_t in) {
    fp_t res = in;
    /* Normalize now */
    /* the original number could have been a denormal. */
    /* shift left until the leading number of .f is 1 */
    while ( ! (res.f & DP_HIDDEN_BIT)) {
        res.f <<= 1;
        res.e--;
    }
    /* do the final shifts in one go. Don't forget the hidden bit (the '-1') */
    res.f <<= (DIY_SIGNIFICAND_SIZE - DP_SIGNIFICAND_SIZE - 1);
    res.e = res.e - (DIY_SIGNIFICAND_SIZE - DP_SIGNIFICAND_SIZE - 1);
    return res;
}

/* by swlt16 (computed with Python3) */
static const uint32_t pow10_f[] = {0xbf29dcac, 0xeef453d7, 0x9558b466, 0xbaaee180, 0xe95a99e0, 0x91d8a02c, 0xb64ec837, 0xe3e27a44, 0x8e6d8c6b, 0xb208ef85, 0xde8b2b67, 0x8b16fb20, 0xaddcb9e8, 0xd953e862, 0x87d4713d, 0xa9c98d8d, 0xd43bf0f0, 0x84a57696, 0xa5ced43b, 0xcf42894a, 0x818995ce, 0xa1ebfb42, 0xca66fa13, 0xfd00b897, 0x9e20735f, 0xc5a89036, 0xf712b444, 0x9a6bb0aa, 0xc1069cd5, 0xf148440a, 0x96cd2a86, 0xbc807528, 0xeba09272, 0x93445b87, 0xb8157269, 0xe61acf03, 0x8fd0c162, 0xb3c4f1bb, 0xe0b62e29, 0x8c71dcda, 0xaf8e5410, 0xdb71e914, 0x892731ad, 0xab70fe18, 0xd64d3d9e, 0x85f04683, 0xa76c5823, 0xd1476e2c, 0x82cca4dc, 0xa37fce12, 0xcc5fc197, 0xff77b1fd, 0x9faacf3e, 0xc795830d, 0xf97ae3d1, 0x9becce63, 0xc2e801fb, 0xf3a2027a, 0x9845418c, 0xbe5691ef, 0xedec366b, 0x94b3a203, 0xb9e08a84, 0xe858ad25, 0x91376c37, 0xb5854745, 0xe2e69916, 0x8dd01fae, 0xb1442799, 0xdd95317f, 0x8a7d3eef, 0xad1c8eab, 0xd863b256, 0x873e4f76, 0xa90de353, 0xd3515c28, 0x8412d999, 0xa5178fff, 0xce5d73ff, 0x80fa6880, 0xa139029f, 0xc9874347, 0xfbe91419, 0x9d71ac90, 0xc4ce17b4, 0xf6019da0, 0x99c10284, 0xc0314325, 0xf03d93ef, 0x96267c75, 0xbbb01b93, 0xea9c2277, 0x92a1958a, 0xb749faed, 0xe51c79a8, 0x8f31cc09, 0xb2fe3f0c, 0xdfbdcece, 0x8bd6a141, 0xaecc4991, 0xda7f5bf6, 0x888f9979, 0xaab37fd8, 0xd5605fce, 0x855c3be1, 0xa6b34ad9, 0xd0601d8f, 0x823c1279, 0xa2cb1718, 0xcb7ddcde, 0xfe5d5415, 0x9efa548d, 0xc6b8e9b0, 0xf867241d, 0x9b407692, 0xc2109436, 0xf294b944, 0x979cf3ca, 0xbd8430bd, 0xece53cec, 0x940f4614, 0xb9131799, 0xe757dd7f, 0x9096ea6f, 0xb4bca50b, 0xe1ebce4e, 0x8d3360f1, 0xb080392d, 0xdca04778, 0x89e42cab, 0xac5d37d6, 0xd77485cb, 0x86a8d39f, 0xa8530887, 0xd267caa8, 0x8380dea9, 0xa4611654, 0xcd795be8, 0x806bd971, 0xa086cfce, 0xc8a883c1, 0xfad2a4b1, 0x9cc3a6ef, 0xc3f490aa, 0xf4f1b4d5, 0x99171105, 0xbf5cd546, 0xef340a98, 0x9580869f, 0xbae0a847, 0xe998d259, 0x91ff8377, 0xb67f6455, 0xe41f3d6a, 0x8e938663, 0xb23867fb, 0xdec681fa, 0x8b3c113c, 0xae0b158b, 0xd98ddaee, 0x87f8a8d5, 0xa9f6d30a, 0xd47487cd, 0x84c8d4e0, 0xa5fb0a18, 0xcf79cc9e, 0x81ac1fe3, 0xa21727db, 0xca9cf1d2, 0xfd442e47, 0x9e4a9cec, 0xc5dd4427, 0xf7549531, 0x9a94dd3f, 0xc13a148e, 0xf18899b2, 0x96f5600f, 0xbcb2b813, 0xebdf6618, 0x936b9fcf, 0xb84687c2, 0xe65829b3, 0x8ff71a10, 0xb3f4e094, 0xe0f218b9, 0x8c974f74, 0xafbd2350, 0xdbac6c24, 0x894bc397, 0xab9eb47d, 0xd686619c, 0x8613fd01, 0xa798fc42, 0xd17f3b52, 0x82ef8513, 0xa3ab6658, 0xcc963fee, 0xffbbcfea, 0x9fd561f2, 0xc7caba6e, 0xf9bd690a, 0x9c1661a6, 0xc31bfa10, 0xf3e2f894, 0x986ddb5c, 0xbe895234, 0xee2ba6c0, 0x94db4838, 0xba121a46, 0xe896a0d8, 0x915e2487, 0xb5b5ada9, 0xe3231913, 0x8df5efac, 0xb1736b97, 0xddd0467c, 0x8aa22c0e, 0xad4ab711, 0xd89d64d5, 0x87625f05, 0xa93af6c7, 0xd389b478, 0x843610cb, 0xa54394fe, 0xce947a3e, 0x811ccc67, 0xa163ff80, 0xc9bcff60, 0xfc2c3f38, 0x9d9ba783, 0xc5029164, 0xf64335bd, 0x99ea0196, 0xc06481fc, 0xf07da27b, 0x964e858d, 0xbbe226f0, 0xeadab0ac, 0x92c8ae6b, 0xb77ada06, 0xe5599088, 0x8f57fa55, 0xb32df8ea, 0xdff97724, 0x8bfbea77, 0xaefae514, 0xdab99e5a, 0x88b402f8, 0xaae103b6, 0xd59944a3, 0x857fcae6, 0xa6dfbda0, 0xd097ad08, 0x825ecc25, 0xa2f67f2e, 0xcbb41ef9, 0xfea126b8, 0x9f24b833, 0xc6ede640, 0xf8a95fd0, 0x9b69dbe2, 0xc24452da, 0xf2d56791, 0x97c560ba, 0xbdb6b8e9, 0xed246723, 0x9436c076, 0xb9447094, 0xe7958cb8, 0x90bd77f3, 0xb4ecd5f0, 0xe2280b6c, 0x8d590724, 0xb0af48ec, 0xdcdb1b28, 0x8a08f0f9, 0xac8b2d37, 0xd7adf885, 0x86ccbb53, 0xa87fea28, 0xd29fe4b2, 0x83a3eeef, 0xa48ceaab, 0xcdb02555, 0x808e1755, 0xa0b19d2b, 0xc8de0475, 0xfb158593, 0x9ced737c, 0xc428d05b, 0xf5330471, 0x993fe2c7, 0xbf8fdb79, 0xef73d257, 0x95a86376, 0xbb127c54, 0xe9d71b69, 0x92267121, 0xb6b00d6a, 0xe45c10c4, 0x8eb98a7b, 0xb267ed19, 0xdf01e860, 0x8b61313c, 0xae397d8b, 0xd9c7dced, 0x881cea14, 0xaa242499, 0xd4ad2dc0, 0x84ec3c98, 0xa6274bbe, 0xcfb11ead, 0x81ceb32c, 0xa2425ff7, 0xcad2f7f5, 0xfd87b5f3, 0x9e74d1b8, 0xc6120625, 0xf79687af, 0x9abe14cd, 0xc16d9a01, 0xf1c90081, 0x971da050, 0xbce50865, 0xec1e4a7e, 0x9392ee8f, 0xb877aa32, 0xe69594bf, 0x901d7cf7, 0xb424dc35, 0xe12e1342, 0x8cbccc09, 0xafebff0c, 0xdbe6fecf, 0x89705f41, 0xabcc7712, 0xd6bf94d6, 0x8637bd06, 0xa7c5ac47, 0xd1b71759, 0x83126e98, 0xa3d70a3d, 0xcccccccd, 0x80000000, 0xa0000000, 0xc8000000, 0xfa000000, 0x9c400000, 0xc3500000, 0xf4240000, 0x98968000, 0xbebc2000, 0xee6b2800, 0x9502f900, 0xba43b740, 0xe8d4a510, 0x9184e72a, 0xb5e620f5, 0xe35fa932, 0x8e1bc9bf, 0xb1a2bc2f, 0xde0b6b3a, 0x8ac72305, 0xad78ebc6, 0xd8d726b7, 0x87867832, 0xa968163f, 0xd3c21bcf, 0x84595161, 0xa56fa5ba, 0xcecb8f28, 0x813f3979, 0xa18f07d7, 0xc9f2c9cd, 0xfc6f7c40, 0x9dc5ada8, 0xc5371912, 0xf684df57, 0x9a130b96, 0xc097ce7c, 0xf0bdc21b, 0x96769951, 0xbc143fa5, 0xeb194f8e, 0x92efd1b9, 0xb7abc627, 0xe596b7b1, 0x8f7e32ce, 0xb35dbf82, 0xe0352f63, 0x8c213d9e, 0xaf298d05, 0xdaf3f046, 0x88d8762c, 0xab0e93b7, 0xd5d238a5, 0x85a36367, 0xa70c3c41, 0xd0cf4b51, 0x82818f13, 0xa321f2d7, 0xcbea6f8d, 0xfee50b70, 0x9f4f2726, 0xc722f0f0, 0xf8ebad2c, 0x9b934c3b, 0xc2781f4a, 0xf316271c, 0x97edd872, 0xbde94e8e, 0xed63a232, 0x945e455f, 0xb975d6b7, 0xe7d34c65, 0x90e40fbf, 0xb51d13af, 0xe264589a, 0x8d7eb760, 0xb0de6539, 0xdd15fe87, 0x8a2dbf14, 0xacb92ed9, 0xd7e77a90, 0x86f0ac9a, 0xa8acd7c0, 0xd2d80db0, 0x83c7088e, 0xa4b8cab2, 0xcde6fd5e, 0x80b05e5b, 0xa0dc75f1, 0xc913936e, 0xfb587849, 0x9d174b2e, 0xc45d1df9, 0xf5746578, 0x9968bf6b, 0xbfc2ef45, 0xefb3ab17, 0x95d04aee, 0xbb445daa, 0xea157514, 0x924d692d, 0xb6e0c378, 0xe498f456, 0x8edf98b6, 0xb2977ee3, 0xdf3d5e9c, 0x8b865b21, 0xae67f1ea, 0xda01ee64, 0x884134ff, 0xaa51823e, 0xd4e5e2ce, 0x850fadc1, 0xa6539931, 0xcfe87f7d, 0x81f14fae, 0xa26da39a, 0xcb090c80, 0xfdcb4fa0, 0x9e9f11c4, 0xc646d635, 0xf7d88bc2, 0x9ae75759, 0xc1a12d30, 0xf209787c, 0x9745eb4d, 0xbd176621, 0xec5d3fa9, 0x93ba47ca, 0xb8a8d9bc, 0xe6d3102b, 0x9043ea1b, 0xb454e4a1, 0xe16a1dca, 0x8ce2529e, 0xb01ae746, 0xdc21a117, 0x899504ae, 0xabfa45da, 0xd6f8d751, 0x865b8692, 0xa7f26837, 0xd1ef0245, 0x8335616b, 0xa402b9c6, 0xcd036837, 0x80222122, 0xa02aa96b, 0xc83553c6, 0xfa42a8b7, 0x9c69a973, 0xc38413cf, 0xf46518c3, 0x98bf2f7a, 0xbeeefb58, 0xeeaaba2e, 0x952ab45d, 0xba756174, 0xe912b9d1, 0x91abb423, 0xb616a12b, 0xe39c4976, 0x8e41adea, 0xb1d21964, 0xde469fbe, 0x8aec23d7, 0xada72ccc, 0xd910f7ff, 0x87aa9aff, 0xa99541bf, 0xd3fa922f, 0x847c9b5d, 0xa59bc235, 0xcf02b2c2, 0x8161afb9, 0xa1ba1ba8, 0xca28a292, 0xfcb2cb36, 0x9defbf02, 0xc56baec2, 0xf6c69a73, 0x9a3c2088, 0xc0cb28aa, 0xf0fdf2d4, 0x969eb7c4, 0xbc4665b6, 0xeb57ff23, 0x9316ff76, 0xb7dcbf53, 0xe5d3ef28, 0x8fa47579, 0xb38d92d7, 0xe070f78d, 0x8c469ab8, 0xaf584166, 0xdb2e51c0, 0x88fcf318, 0xab3c2fde, 0xd60b3bd5, 0x85c70565, 0xa738c6bf, 0xd106f86e, 0x82a45b45, 0xa34d7216, 0xcc20ce9c, 0xff290243, 0x9f79a16a, 0xc75809c4, 0xf92e0c35, 0x9bbcc7a1, 0xc2abf98a, 0xf356f7ec, 0x98165af3, 0xbe1bf1b0, 0xeda2ee1c, 0x9485d4d2, 0xb9a74a06, 0xe8111c88, 0x910ab1d5, 0xb54d5e4a, 0xe2a0b5dd, 0x8da471aa, 0xb10d8e14, 0xdd50f199, 0x8a529700, 0xace73cc0, 0xd8210bf0, 0x8714a776, 0xa8d9d153, 0xd31045a8, 0x83ea2b89, 0xa4e4b66b, 0xce1de406, 0x80d2ae84, 0xa1075a25, 0xc94930ae, 0xfb9b7cda, 0x9d412e08, 0xc491798a, 0xf5b5d7ed, 0x9991a6f4, 0xbff610b1, 0xeff394dd, 0x95f83d0a, 0xbb764c4d, 0xea53df60, 0x92746b9c, 0xb7118683, 0xe4d5e824, 0x8f05b116, 0xb2c71d5c, 0xdf78e4b3, 0x8bab8ef0, 0xae9672ac, 0xda3c0f57, 0x88658996, 0xaa7eebfc, 0xd51ea6fb, 0x8533285d, 0xa67ff274, 0xd01fef11, 0x8213f56a, 0xa298f2c5, 0xcb3f2f76, 0xfe0efb54, 0x9ec95d14, 0xc67bb459, 0xf81aa170, 0x9b10a4e6, 0xc1d4ce1f, 0xf24a01a7, 0x976e4109, 0xbd49d14b, 0xec9c459d, 0x93e1ab82, 0xb8da1663, 0xe7109bfc, 0x906a617d, 0xb484f9dd, 0xe1a63854, 0x8d07e334, 0xb049dc01, 0xdc5c5302, 0x89b9b3e1, 0xac2820d9, 0xd7322910, 0x867f59aa, 0xa81f3014, 0xd226fc19, 0x83585d90, 0xa42e74f4, 0xcd3a1231, 0x80444b5e, 0xa0555e36, 0xc86ab5c4, 0xfa856335, 0x9c935e01, 0xc3b83581, 0xf4a642e1, 0x98e7e9cd, 0xbf21e440, 0xeeea5d50, 0x95527a52, 0xbaa718e7, 0xe950df20, 0x91d28b74, 0xb6472e51, 0xe3d8f9e5, 0x8e679c2f, 0xb201833b, 0xde81e40a, 0x8b112e86, 0xadd57a28, 0xd94ad8b2, 0x87cec76f, 0xa9c2794b, 0xd433179e, 0x849feec3, 0xa5c7ea73, 0xcf39e510, 0x81842f2a, 0xa1e53af4, 0xca5e89b2, 0xfcf62c1e, 0x9e19db93, 0xc5a05277, 0xf7086715, 0x9a65406d, 0xc0fe9089, 0xf13e34ab, 0x96c6e0eb, 0xbc789925, 0xeb96bf6f, 0x933e37a5, 0xb80dc58f, 0xe61136f2, 0x8fcac257, 0xb3bd72ed, 0xe0accfa8, 0x8c6c01c9, 0xaf87023c, 0xdb68c2cb, 0x892179bf, 0xab69d82e};
static const int pow10_e[] = {-1171, -1168, -1164, -1161, -1158, -1154, -1151, -1148, -1144, -1141, -1138, -1134, -1131, -1128, -1124, -1121, -1118, -1114, -1111, -1108, -1104, -1101, -1098, -1095, -1091, -1088, -1085, -1081, -1078, -1075, -1071, -1068, -1065, -1061, -1058, -1055, -1051, -1048, -1045, -1041, -1038, -1035, -1031, -1028, -1025, -1021, -1018, -1015, -1011, -1008, -1005, -1002, -998, -995, -992, -988, -985, -982, -978, -975, -972, -968, -965, -962, -958, -955, -952, -948, -945, -942, -938, -935, -932, -928, -925, -922, -918, -915, -912, -908, -905, -902, -899, -895, -892, -889, -885, -882, -879, -875, -872, -869, -865, -862, -859, -855, -852, -849, -845, -842, -839, -835, -832, -829, -825, -822, -819, -815, -812, -809, -806, -802, -799, -796, -792, -789, -786, -782, -779, -776, -772, -769, -766, -762, -759, -756, -752, -749, -746, -742, -739, -736, -732, -729, -726, -722, -719, -716, -712, -709, -706, -703, -699, -696, -693, -689, -686, -683, -679, -676, -673, -669, -666, -663, -659, -656, -653, -649, -646, -643, -639, -636, -633, -629, -626, -623, -619, -616, -613, -610, -606, -603, -600, -596, -593, -590, -586, -583, -580, -576, -573, -570, -566, -563, -560, -556, -553, -550, -546, -543, -540, -536, -533, -530, -526, -523, -520, -517, -513, -510, -507, -503, -500, -497, -493, -490, -487, -483, -480, -477, -473, -470, -467, -463, -460, -457, -453, -450, -447, -443, -440, -437, -433, -430, -427, -423, -420, -417, -414, -410, -407, -404, -400, -397, -394, -390, -387, -384, -380, -377, -374, -370, -367, -364, -360, -357, -354, -350, -347, -344, -340, -337, -334, -330, -327, -324, -321, -317, -314, -311, -307, -304, -301, -297, -294, -291, -287, -284, -281, -277, -274, -271, -267, -264, -261, -257, -254, -251, -247, -244, -241, -237, -234, -231, -227, -224, -221, -218, -214, -211, -208, -204, -201, -198, -194, -191, -188, -184, -181, -178, -174, -171, -168, -164, -161, -158, -154, -151, -148, -144, -141, -138, -134, -131, -128, -125, -121, -118, -115, -111, -108, -105, -101, -98, -95, -91, -88, -85, -81, -78, -75, -71, -68, -65, -61, -58, -55, -51, -48, -45, -41, -38, -35, -31, -28, -25, -22, -18, -15, -12, -8, -5, -2, 2, 5, 8, 12, 15, 18, 22, 25, 28, 32, 35, 38, 42, 45, 48, 52, 55, 58, 62, 65, 68, 71, 75, 78, 81, 85, 88, 91, 95, 98, 101, 105, 108, 111, 115, 118, 121, 125, 128, 131, 135, 138, 141, 145, 148, 151, 155, 158, 161, 164, 168, 171, 174, 178, 181, 184, 188, 191, 194, 198, 201, 204, 208, 211, 214, 218, 221, 224, 228, 231, 234, 238, 241, 244, 248, 251, 254, 258, 261, 264, 267, 271, 274, 277, 281, 284, 287, 291, 294, 297, 301, 304, 307, 311, 314, 317, 321, 324, 327, 331, 334, 337, 341, 344, 347, 351, 354, 357, 360, 364, 367, 370, 374, 377, 380, 384, 387, 390, 394, 397, 400, 404, 407, 410, 414, 417, 420, 424, 427, 430, 434, 437, 440, 444, 447, 450, 454, 457, 460, 463, 467, 470, 473, 477, 480, 483, 487, 490, 493, 497, 500, 503, 507, 510, 513, 517, 520, 523, 527, 530, 533, 537, 540, 543, 547, 550, 553, 556, 560, 563, 566, 570, 573, 576, 580, 583, 586, 590, 593, 596, 600, 603, 606, 610, 613, 616, 620, 623, 626, 630, 633, 636, 640, 643, 646, 649, 653, 656, 659, 663, 666, 669, 673, 676, 679, 683, 686, 689, 693, 696, 699, 703, 706, 709, 713, 716, 719, 723, 726, 729, 733, 736, 739, 743, 746, 749, 752, 756, 759, 762, 766, 769, 772, 776, 779, 782, 786, 789, 792, 796, 799, 802, 806, 809, 812, 816, 819, 822, 826, 829, 832, 836, 839, 842, 845, 849, 852, 855, 859, 862, 865, 869, 872, 875, 879, 882, 885, 889, 892, 895, 899, 902, 905, 909, 912, 915, 919, 922, 925, 929, 932, 935, 939, 942, 945, 948, 952, 955, 958, 962, 965, 968, 972, 975, 978, 982, 985, 988, 992, 995, 998, 1002, 1005, 1008, 1012, 1015, 1018, 1022, 1025, 1028, 1032, 1035, 1038, 1041, 1045, 1048, 1051, 1055, 1058, 1061, 1065, 1068, 1071, 1075, 1078, 1081, 1085, 1088, 1091, 1095, 1098, 1101, 1105, 1108};


static fp_t cached_power(int k) {
    fp_t r;
    int index = 343 + k;
    r.f = pow10_f[index];
    r.e = pow10_e[index];
    return r;
}

static int __xceil(double num) {
    /* by swlt16 */
    /* TODO: REMOVE */
    int inum = (int)num;
    if (num == (double)inum) {
        return inum;
    }
    return inum + 1;
}

static int k_comp(int e, int alpha, int gamma) {
    /* by Florian Loitsch (see file header) */
    /* TODO: use libm's ceil */
    return (int) __xceil((alpha - e + 63) * 0.30102999); /* 1/lg(10) */
}

static uint32_t double_to_uint32(double d) {
    /* by swlt16 */ 
    void* caster; /* i know what im doing! */
    caster = &d;
    return *((uint32_t*)caster);
}

static fp_t double_to_fp(double d) {
    /* by swlt16 */
    uint32_t d32 = double_to_uint32(d);
    int biased_e = (d32 & DP_EXPONENT_MASK) >> DP_SIGNIFICAND_SIZE; /* get raw exponent */
    uint32_t significand = (d32 & DP_SIGNIFICAND_MASK); /* get raw mantissa */
    fp_t res;
    if (biased_e != 0) { /* if exponent unequal zero */ 
        res.f = significand + DP_HIDDEN_BIT;
        res.e = biased_e - DP_EXPONENT_BIAS; /* 2^(k−1) − 1 */
    } else {
        res.f = significand;
        res.e = DP_MIN_EXPONENT + 1;
    }
    return res;
}

static void normalized_boundaries(double d, fp_t* out_m_minus, fp_t* out_m_plus) {
    /* by miloyip */
    fp_t v = double_to_fp(d);
    fp_t pl, mi;
    char significand_is_zero = v.f == DP_HIDDEN_BIT; /* bool */
    pl.f  = (v.f << 1) + 1; pl.e  = v.e - 1;
    pl = fp_normalize(pl);
    if (significand_is_zero) {
        mi.f = (v.f << 2) - 1;
        mi.e = v.e - 2;
    } else {
        mi.f = (v.f << 1) - 1;
        mi.e = v.e - 1;
    }
    mi.f <<= mi.e - pl.e;
    mi.e = pl.e;
    *out_m_plus = pl;
    *out_m_minus = mi;
}

static void digit_gen(fp_t Mp, fp_t delta, char* buffer, int* len, int* K) {
    /* by Florian Loitsch (see file header) */
    uint32_t div;
    uint32_t p1, p2;
    int d, kappa;
    fp_t one;
    one.f = ((uint32_t) 1) << -Mp.e;
    one.e = Mp.e;
    p1 = Mp.f >> -one.e;
    p2 = Mp.f & (one.f - 1);
    *len = 0;
    kappa = 10;
    div = 1000000000;

    while(kappa > 0) {
        d = p1 / div;
        if(d || *len) buffer[(*len)++] = '0' + d;
        p1 %= div;
        kappa--;
        div /= 10;
        if((((uint32_t)p1) << -one.e) + p2 <= delta.f) {
            *K += kappa;
            return;
        }
    }
    do {
        p2 *= 10;
        d = p2 >> -one.e;
        if(d || *len) buffer[(*len)++] = '0' + d;
        p2 &= one.f - 1;
        kappa--;
        delta.f *= 10;
    } while(p2 > delta.f);
    *K += kappa;
}

static void grisu2(double v, char* buffer, int* length, int* K) {
    /* by swlt16 */
    fp_t w, w_minus, w_plus;
    fp_t M, M_minus, M_plus;
    fp_t c_mk, delta;
    int q, alpha, gamma, mk;
    
    w = fp_normalize(double_to_fp(v));
    normalized_boundaries(v, &w_minus, &w_plus); /* step 1+2 */
    q = 32;
    alpha = -59;
    gamma = -32; 
    mk = k_comp(w.e + q, alpha, gamma); /* step 3 */
    c_mk = cached_power(mk); /* step 3 */

    M = fp_multiply(w, c_mk);
    M_minus = fp_multiply(w_minus, c_mk);
    M_plus = fp_multiply(w_plus, c_mk);

    M_minus.f++;
    M_plus.f--;
    delta = fp_subtract(M_plus, M_minus);
    *K = -mk;
    *length = 0;
    digit_gen(M, delta, buffer, length, K);
}

static int printFloat(double fnum, int flags, int width, int prec, FILE *stream) {
    /* by swlt16 */
    int numDigits, K;
    char digits[20];
    int outputLength;
    int i, j;
    int calcLen;
    char isNegative = 0;

    /* STEP 0: some prerequisites */
    /* workaround for negative numbers */
    if(fnum < 0.0) {
        fnum = -fnum; /* grisu needs a positive float */
        isNegative = 1;
    }
    /* default fallback for precision (6) */
    if( ! ((flags & FLAG_PREC) == FLAG_PREC)) {
        prec = 6;
    }  

    /* STEP 1: call grisu */
    grisu2(fnum, digits, &numDigits, &K);

    /* STEP 2: printf-like stuff */
    outputLength = 0;
    /* calculate padding according to width parameter */
    calcLen = (isNegative || ((flags & FLAG_PLUS) == FLAG_PLUS) || ((flags & FLAG_SPACE) == FLAG_SPACE)) /* sign */
            + (numDigits + K) /* int part */
            + 1 /* dot */
            + prec; /* frac part */
    /* print padding if spaces needed AND if no left-alignment is needed */
    if( ! ((flags & FLAG_ZERO) == FLAG_ZERO) && ! ((flags & FLAG_LEFT) == FLAG_LEFT)) {
        for(i = 0; i < (width - calcLen); i++) {
            putc(' ', stream); outputLength++;
        }
    }
    /* print sign */
    if(isNegative || ((flags & FLAG_PLUS) == FLAG_PLUS)) {
        putc(isNegative ? '-' : '+', stream); outputLength++;
    } else {
        if((flags & FLAG_SPACE) == FLAG_SPACE) {
            putc(' ', stream); outputLength++;
        }
    }
    /* print padding if zeroes needed */
    if((flags & FLAG_ZERO) == FLAG_ZERO && ! ((flags & FLAG_LEFT) == FLAG_LEFT)) {
        for(i = 0; i < (width - calcLen); i++) {
            putc('0', stream); outputLength++;
        }
    }
    /* print integer part */
    for(i = 0; i < (numDigits + K); i++) {
        putc(digits[i], stream); outputLength++;
    }
    /* print dot */
    if(prec || ((flags & FLAG_ALT) == FLAG_ALT)) {
        putc('.', stream); outputLength++;
        /* print fraction part */
        j = 0;
        for(i = (numDigits + K); i < numDigits; i++) {
            putc(digits[i], stream); outputLength++;
            j++;
            if(j == prec) break;
        }
        for(;j < prec; j++) {
            putc('0', stream); outputLength++;
        }
    }
    /* print padding if left-alignment is needed */
    if((flags & FLAG_LEFT) == FLAG_LEFT) {
        for(i = 0; i < (width - calcLen); i++) {
            putc(' ', stream); outputLength++;
        }
    }
    return outputLength;
}

static int printScientific(double fnum, int flags, int width, int prec, FILE *stream) {
    /* by swlt16 */
    int numDigits, K;
    char digits[20];
    int outputLength;
    int i, j;
    int calcLen;
    char isNegative = 0;

    /* STEP 0: some prerequisites */
    /* workaround for negative numbers */
    if(fnum < 0.0) {
        fnum = -fnum; /* grisu needs a positive float */
        isNegative = 1;
    }
    /* default fallback for precision (6) */
    if( ! ((flags & FLAG_PREC) == FLAG_PREC)) {
        prec = 6;
    }  

    /* STEP 1: call grisu */
    grisu2(fnum, digits, &numDigits, &K);
    /* printf("<<DBG len=%d, K=%d>>", numDigits, K); */

    /* STEP 2: printf-like stuff */
    outputLength = 0;
    /* calculate padding according to width parameter */
    calcLen = (isNegative || ((flags & FLAG_PLUS) == FLAG_PLUS) || ((flags & FLAG_SPACE) == FLAG_SPACE)) /* sign */
            + 1 /* int part */
            + 1 /* dot */
            + prec /* frac part */
            + 1 /* e */
            + 3; /* exponent */ 
    /* print padding if spaces needed AND if no left-alignment is needed */
    if( ! ((flags & FLAG_ZERO) == FLAG_ZERO) && ! ((flags & FLAG_LEFT) == FLAG_LEFT)) {
        for(i = 0; i < (width - calcLen); i++) {
            putc(' ', stream); outputLength++;
        }
    }
    /* print sign */
    if(isNegative || ((flags & FLAG_PLUS) == FLAG_PLUS)) {
        putc(isNegative ? '-' : '+', stream); outputLength++;
    } else {
        if((flags & FLAG_SPACE) == FLAG_SPACE) {
            putc(' ', stream); outputLength++;
        }
    }
    /* print padding if zeroes needed */
    if((flags & FLAG_ZERO) == FLAG_ZERO && ! ((flags & FLAG_LEFT) == FLAG_LEFT)) {
        for(i = 0; i < (width - calcLen); i++) {
            putc('0', stream); outputLength++;
        }
    }
    /* print integer part */
    /* print normalized first digit */
    putc(digits[0], stream); outputLength++;
    /* print dot */
    if(prec || ((flags & FLAG_ALT) == FLAG_ALT)) {
        putc('.', stream); outputLength++;
        /* print fraction part */
        j = 0;
        for(i = 1; i < numDigits; i++) {
            putc(digits[i], stream); outputLength++;
            j++;
            if(j == prec) break;
        }
        for(;j < prec; j++) {
            putc('0', stream); outputLength++;
        }
    }
    /* print e or E */
    putc((flags & FLAG_CAPS) == FLAG_CAPS ? 'E' : 'e', stream); outputLength++;
    /* print exponent */
    outputLength += fprintf(stream, "%+.2d", (numDigits + K - 1)); /* only do it once... */
    /* print padding if left-alignment is needed */
    if((flags & FLAG_LEFT) == FLAG_LEFT) {
        for(i = 0; i < (width - calcLen); i++) {
            putc(' ', stream); outputLength++;
        }
    }
    return outputLength;
}

