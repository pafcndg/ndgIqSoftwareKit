/*-------------------------------------------------------------------------
 * INTEL CONFIDENTIAL
 *
 * Copyright 2015 Intel Corporation All Rights Reserved.
 *
 * This source code and all documentation related to the source code
 * ("Material") contains trade secrets and proprietary and confidential
 * information of Intel and its suppliers and licensors. The Material is
 * deemed highly confidential, and is protected by worldwide copyright and
 * trade secret laws and treaty provisions. No part of the Material may be
 * used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you by
 * disclosure or delivery of the Materials, either expressly, by
 * implication, inducement, estoppel or otherwise. Any license under such
 * intellectual property rights must be express and approved by Intel in
 * writing.
 *-------------------------------------------------------------------------
 */

/*
 * Author(s) : Binuraj Ravindran (binuraj.ravindran@intel.com)
 * Group : NBG/NTG
*/


/*
 * This file contains macros that will enable the usage of ARC DSP and IDE
 *   instructions so that they can be called as C-subroutines
 */

#define intrinsic_2OP(NAME, MOP, SOP)                                           \
    ".extInstruction " NAME "," #MOP "," #SOP ",SUFFIX_NONE, SYNTAX_2OP\n\t"
#define intrinsic_3OP(NAME, MOP, SOP)                                           \
    ".extInstruction " NAME "," #MOP "," #SOP ",SUFFIX_NONE, SYNTAX_3OP\n\t"


#define __ide_dsp_fp_div(src1, src2) ({float __dst;                             \
    __asm__ __volatile__ ( intrinsic_3OP("dsp_fp_div", 0x07, 0x2a)              \
             "dsp_fp_div %0, %1, %2\n\t"                                        \
             : "=r" (__dst)                                                     \
             : "r" (src1), "r" (src2));                                         \
             __dst;})

#define __ide_dsp_fp_flt2i(src) ({int __dst;                                    \
    __asm__ __volatile__ ( intrinsic_2OP("dsp_fp_flt2i", 0x07, 0x2b)            \
             "dsp_fp_flt2i %0, %1\n\t"                                          \
             : "=r" (__dst)                                                     \
             : "r" (src));                                                      \
             __dst;})

#define __ide_dsp_fp_i2flt(src) ({float __dst;                                  \
      __asm__ __volatile__ ( intrinsic_2OP("dsp_fp_i2flt", 0x07, 0x2c)          \
             "dsp_fp_i2flt %0, %1\n\t"                                          \
             : "=r" (__dst)                                                     \
             : "r" (src));                                                      \
             __dst;})

#define __ide_dsp_fp_sqrt(src) ({float __dst;                                   \
    __asm__ __volatile__ ( intrinsic_2OP("dsp_fp_sqrt", 0x07, 0x2d)             \
             "dsp_fp_sqrt %0, %1\n\t"                                           \
             : "=r" (__dst)                                                     \
             : "r" (src));                                                      \
             __dst;})

#define __ide_dsp_fp_cmp_eq(src1, src2) ({int __dst;                            \
    __asm__ __volatile__ ( intrinsic_3OP("dsp_fp_cmp", 0x07, 0x2b)              \
             "dsp_fp_cmp %0, %1, %2\n\t"                                        \
             "cmp    %0,1\n\t"                                                  \
             "mov    %0,0\n\t"                                                  \
             "mov.eq %0,1\n\t"                                                  \
             : "=r" (__dst)                                                     \
             : "r" (src1), "r" (src2));                                         \
             __dst;})

#define __ide_dsp_fp_cmp_lt(src1, src2) ({int __dst;                            \
    __asm__ __volatile__ ( intrinsic_3OP("dsp_fp_cmp", 0x07, 0x2b)              \
             "dsp_fp_cmp %0, %1, %2\n\t"                                        \
             "cmp    %0,2\n\t"                                                  \
             "mov    %0,0\n\t"                                                  \
             "mov.eq %0,1\n\t"                                                  \
             : "=r" (__dst)                                                     \
             : "r" (src1), "r" (src2));                                         \
             __dst;})

#define __ide_dsp_fp_cmp_gt(src1, src2) ({int __dst;                            \
    __asm__ __volatile__ ( intrinsic_3OP("dsp_fp_cmp", 0x07, 0x2b)              \
             "dsp_fp_cmp %0, %1, %2\n\t"                                        \
             "cmp    %0,4\n\t"                                                  \
             "mov    %0,0\n\t"                                                  \
             "mov.eq %0,1\n\t"                                                  \
             : "=r" (__dst)                                                     \
             : "r" (src1), "r" (src2));                                         \
             __dst;})



#define fdiv __ide_dsp_fp_div
#define f2i __ide_dsp_fp_flt2i
#define i2f __ide_dsp_fp_i2flt
#define fsqrt __ide_dsp_fp_sqrt
#define fcmp_eq __ide_dsp_fp_cmp_eq //return: 1 equal, otherwise 0
#define fcmp_lt __ide_dsp_fp_cmp_lt //return: 1 less than, otherwise 0
#define fcmp_gt __ide_dsp_fp_cmp_gt //return: 1 greater than, otherwise 0



