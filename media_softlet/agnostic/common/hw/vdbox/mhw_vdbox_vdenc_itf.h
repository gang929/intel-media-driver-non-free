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
//! \file     mhw_vdbox_vdenc_itf.h
//! \brief    MHW VDBOX VDENC interface common base
//! \details
//!

#ifndef __MHW_VDBOX_VDENC_ITF_H__
#define __MHW_VDBOX_VDENC_ITF_H__

#include "mhw_itf.h"
#include "mhw_vdbox_vdenc_cmdpar.h"

namespace mhw
{
namespace vdbox
{
namespace vdenc
{
class Itf
{
public:
    class ParSetting
    {
    public:
        virtual ~ParSetting() = default;

        _MHW_SETPAR_DEF(VDENC_CONTROL_STATE);
        _MHW_SETPAR_DEF(VDENC_PIPE_MODE_SELECT);
        _MHW_SETPAR_DEF(VDENC_SRC_SURFACE_STATE);
        _MHW_SETPAR_DEF(VDENC_REF_SURFACE_STATE);
        _MHW_SETPAR_DEF(VDENC_DS_REF_SURFACE_STATE);
        _MHW_SETPAR_DEF(VDENC_PIPE_BUF_ADDR_STATE);
        _MHW_SETPAR_DEF(VDENC_WEIGHTSOFFSETS_STATE);
        _MHW_SETPAR_DEF(VDENC_HEVC_VP9_TILE_SLICE_STATE);
        _MHW_SETPAR_DEF(VDENC_WALKER_STATE);
        _MHW_SETPAR_DEF(VD_PIPELINE_FLUSH);
        _MHW_SETPAR_DEF(VDENC_CMD1);
        _MHW_SETPAR_DEF(VDENC_CMD2);
        _MHW_SETPAR_DEF(VDENC_CMD3);
        _MHW_SETPAR_DEF(VDENC_CMD4);
        _MHW_SETPAR_DEF(VDENC_CMD5);
    };

    virtual ~Itf() = default;

    virtual MOS_STATUS EnableVdencRowstoreCacheIfSupported(uint32_t address) = 0;

    virtual MOS_STATUS EnableVdencRowIpdlstoreCacheIfSupported(uint32_t address) = 0;

    virtual MOS_STATUS SetCacheabilitySettings(MHW_MEMORY_OBJECT_CONTROL_PARAMS settings[MOS_CODEC_RESOURCE_USAGE_END_CODEC]) = 0;

    _MHW_CMD_ALL_DEF_FOR_ITF(VDENC_CONTROL_STATE);

    _MHW_CMD_ALL_DEF_FOR_ITF(VDENC_PIPE_MODE_SELECT);

    _MHW_CMD_ALL_DEF_FOR_ITF(VDENC_SRC_SURFACE_STATE);

    _MHW_CMD_ALL_DEF_FOR_ITF(VDENC_REF_SURFACE_STATE);

    _MHW_CMD_ALL_DEF_FOR_ITF(VDENC_DS_REF_SURFACE_STATE);

    _MHW_CMD_ALL_DEF_FOR_ITF(VDENC_PIPE_BUF_ADDR_STATE);

    _MHW_CMD_ALL_DEF_FOR_ITF(VDENC_WEIGHTSOFFSETS_STATE);

    _MHW_CMD_ALL_DEF_FOR_ITF(VDENC_HEVC_VP9_TILE_SLICE_STATE);

    _MHW_CMD_ALL_DEF_FOR_ITF(VDENC_WALKER_STATE);

    _MHW_CMD_ALL_DEF_FOR_ITF(VD_PIPELINE_FLUSH);

    _MHW_CMD_ALL_DEF_FOR_ITF(VDENC_CMD1);

    _MHW_CMD_ALL_DEF_FOR_ITF(VDENC_CMD2);

    _MHW_CMD_ALL_DEF_FOR_ITF(VDENC_CMD3);

    _MHW_CMD_ALL_DEF_FOR_ITF(VDENC_CMD4);

    _MHW_CMD_ALL_DEF_FOR_ITF(VDENC_CMD5);
};
}  // namespace vdenc
}  // namespace vdbox
}  // namespace mhw

#endif  // __MHW_VDBOX_VDENC_ITF_H__
