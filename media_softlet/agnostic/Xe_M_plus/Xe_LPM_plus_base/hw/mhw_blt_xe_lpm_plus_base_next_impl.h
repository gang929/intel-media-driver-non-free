/*
* Copyright (c) 2022, Intel Corporation
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
//! \file     mhw_blt_xe_lpm_plus_base_next_impl.h
//! \brief    MHW blt interface common base for Xe_LPM_PLUS
//! \details
//!

#ifndef __MHW_BLT_XE_LPM_PLUS_BASE_NEXT_IMPL_H__
#define __MHW_BLT_XE_LPM_PLUS_BASE_NEXT_IMPL_H__

#include "mhw_blt_impl.h"
#include "mhw_blt_hwcmd_xe_lpm_plus_next.h"
#include "mhw_blt_itf.h"
#include "mhw_impl.h"
#include "mos_solo_generic.h"

namespace mhw
{
namespace blt
{
namespace xe_lpm_plus_next
{
class Impl : public blt::Impl<mhw::blt::xe_lpm_plus_next::Cmd>
{
public:
    Impl(PMOS_INTERFACE osItf) : base_t(osItf)
    {
        MHW_FUNCTION_ENTER;
    }

    uint32_t GetFastTilingMode(BLT_TILE_TYPE TileType) override
    {
        MHW_FUNCTION_ENTER;
        switch (TileType)
        {
        case BLT_NOT_TILED:
            return Cmd::XY_FAST_COPY_BLT_CMD::DESTINATION_TILING_METHOD_LINEAR_TILINGDISABLED;
        case BLT_TILED_Y:
        case BLT_TILED_4:
            return Cmd::XY_FAST_COPY_BLT_CMD::DESTINATION_TILING_METHOD_TILE_Y;
        case BLT_TILED_64:
            return Cmd::XY_FAST_COPY_BLT_CMD::DESTINATION_TILING_METHOD_64KBTILING;
        default:
            MHW_ASSERTMESSAGE("BLT: Can't support GMM TileType %d.", TileType);
        }
        return Cmd::XY_FAST_COPY_BLT_CMD::DESTINATION_TILING_METHOD_LINEAR_TILINGDISABLED;
    }

protected:
    using base_t = blt::Impl<mhw::blt::xe_lpm_plus_next::Cmd>;
MEDIA_CLASS_DEFINE_END(mhw__blt__xe_lpm_plus_next__Impl)
};  // Impl
}  // xe_lpm_plus_next
}  // namespace blt
}  // namespace mhw

#endif  // __MHW_BLT_IMPL_H__
