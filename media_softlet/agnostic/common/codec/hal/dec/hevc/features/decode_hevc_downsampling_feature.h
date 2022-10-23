/*
* Copyright (c) 2020, Intel Corporation
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
//! \file     decode_hevc_downsampling_feature.h
//! \brief    Defines the interface for Hevc decode downsampling feature
//! \details  The Hevc decode downsampling feature interface is maintaining the down sampling context.
//!
#ifndef __DECODE_HEVC_DOWNSAMPLING_FEATURE_H__
#define __DECODE_HEVC_DOWNSAMPLING_FEATURE_H__

#include "decode_downsampling_feature.h"

#ifdef _DECODE_PROCESSING_SUPPORTED

namespace decode
{
class HevcDownSamplingFeature: public DecodeDownSamplingFeature
{
public:
    HevcDownSamplingFeature(MediaFeatureManager *featureManager, DecodeAllocator *allocator, PMOS_INTERFACE osInterface);
    virtual ~HevcDownSamplingFeature();

protected:
    virtual MOS_STATUS GetRefFrameList(std::vector<uint32_t> &refFrameList) override;
    virtual MOS_STATUS GetDecodeTargetSize(SurfaceWidthT &width, SurfaceHeightT &height) override;
    virtual MOS_STATUS GetDecodeTargetFormat(MOS_FORMAT &format) override;
    virtual MOS_STATUS UpdateDecodeTarget(MOS_SURFACE &surface) override;

MEDIA_CLASS_DEFINE_END(decode__HevcDownSamplingFeature)
};
}//decode

#endif // !_DECODE_PROCESSING_SUPPORTED

#endif // !__DECODE_HEVC_DOWNSAMPLING_FEATURE_H__
