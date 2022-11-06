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
//! \file     media_hevc_feature_defs.h
//! \brief    Defines for hevc coding features
//!

#ifndef __MEDIA_HEVC_FEATURE_DEFS_H__
#define __MEDIA_HEVC_FEATURE_DEFS_H__
#include "media_feature_manager.h"

struct HevcFeatureIDs : public FeatureIDs
{
    enum featureIDs
    {
        // HEVC features
        hevcCqpFeature = CONSTRUCTFEATUREID(FEATURE_COMPONENT_ENCODE, FEATURE_SUBCOMPONENT_HEVC, 1),
        hevcBrcFeature,
        hevcVdencRoiFeature,
        hevcVdencWpFeature,
        hevcVdencDssFeature,
        hevcVdencSccFeature,
        vdencLplaAnalysisFeature,
        hevcVdencLplaEncFeature,
        hevcFullEncFeature,
#if _MEDIA_RESERVED
#define HEVC_FEATURE_IDS_EXT
#include "media_hevc_feature_defs_ext.h"
#undef HEVC_FEATURE_IDS_EXT
#endif
    };
};

#endif  // !__MEDIA_HEVC_FEATURE_DEFS_H__
