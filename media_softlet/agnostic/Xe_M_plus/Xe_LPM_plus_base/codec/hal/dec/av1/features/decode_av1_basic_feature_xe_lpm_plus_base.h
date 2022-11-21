/*
* Copyright (c) 2020-2021, Intel Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/
//!
//! \file     decode_av1_basic_feature_xe_lpm_plus_base.h
//! \brief    Defines the common interface for decode av1 basic feature
//!
#ifndef __DECODE_AV1_BASIC_FEATURE_XE_LPM_PLUS_BASE_H__
#define __DECODE_AV1_BASIC_FEATURE_XE_LPM_PLUS_BASE_H__

#include "decode_av1_basic_feature.h"
#include "codec_hw_xe_lpm_plus_base.h"

namespace decode
{
    class Av1BasicFeatureXe_Lpm_Plus_Base: public Av1BasicFeature
    {
    public:
        //!
        //! \brief  Av1BasicFeature constructor
        //!
        Av1BasicFeatureXe_Lpm_Plus_Base(DecodeAllocator *allocator, void *hwInterface, PMOS_INTERFACE osInterface);

        //!
        //! \brief  Av1BasicFeature deconstructor
        //!
        virtual ~Av1BasicFeatureXe_Lpm_Plus_Base() {}

    MEDIA_CLASS_DEFINE_END(decode__Av1BasicFeatureXe_Lpm_Plus_Base)
    };

}  // namespace decode

#endif  // !__DECODE_AV1_BASIC_FEATURE_XE_LPM_PLUS_BASE_H__
