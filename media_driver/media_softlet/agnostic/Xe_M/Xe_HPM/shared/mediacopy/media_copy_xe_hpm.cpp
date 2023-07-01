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
//! \file     media_copy_xe_hpm.cpp
//! \brief    Common interface and structure used in media copy
//! \details  Common interface and structure used in media copy which are platform independent
//!

#include "media_copy_xe_hpm.h"
#include "media_vebox_copy.h"
#include "media_blt_copy_xe_hpm.h"
#include "media_render_copy_xe_hpm.h"
#include "media_skuwa_specific.h"
#include "media_vebox_copy.h"
#include "mos_os.h"
#include "mos_resource_defs.h"
#include "mos_utilities.h"
#include "vp_common.h"
class MhwInterfaces;
MediaCopyState_Xe_Hpm::MediaCopyState_Xe_Hpm() :
    MediaCopyBaseState()
{

}

MOS_STATUS MediaCopyState_Xe_Hpm::Initialize(  PMOS_INTERFACE  osInterface, MhwInterfaces *mhwInterfaces)
{
    MOS_STATUS eStatus = MOS_STATUS_SUCCESS;
    MEDIA_FEATURE_TABLE  *pSkuTable;
    MCPY_CHK_NULL_RETURN(osInterface);
    MCPY_CHK_NULL_RETURN(mhwInterfaces);

    m_osInterface   = osInterface;
    m_mhwInterfacesXeHpm = mhwInterfaces;

    MCPY_CHK_STATUS_RETURN(MediaCopyBaseState::Initialize(osInterface));

    // blt init
    if (nullptr == m_bltState)
    {
        m_bltState = MOS_New(BltState_Xe_Hpm, m_osInterface, m_mhwInterfacesXeHpm);
        MCPY_CHK_NULL_RETURN(m_bltState);
        MCPY_CHK_STATUS_RETURN(m_bltState->Initialize());
    }
    // vebox init
    if (nullptr == m_veboxCopyState)
    {
        m_veboxCopyState = MOS_New(VeboxCopyState, m_osInterface, m_mhwInterfacesXeHpm);
        MCPY_CHK_NULL_RETURN(m_veboxCopyState);
        MCPY_CHK_STATUS_RETURN(m_veboxCopyState->Initialize());
    }
    //  render copy init
    pSkuTable = osInterface->pfnGetSkuTable(osInterface);
    if (MEDIA_IS_SKU(pSkuTable, FtrCCSNode))
    {
        // render copy init
        if (nullptr == m_renderCopy )
        {
            m_renderCopy = MOS_New(RenderCopy_Xe_Hpm, m_osInterface, m_mhwInterfacesXeHpm);
            MCPY_CHK_NULL_RETURN(m_renderCopy);
            MCPY_CHK_STATUS_RETURN(m_renderCopy->Initialize());
        }
    }
    else
    {
        MCPY_NORMALMESSAGE(" Rendercopy don't support due to no CCS Ring");
    }
    return eStatus;
}

MediaCopyState_Xe_Hpm::~MediaCopyState_Xe_Hpm()
{
    MOS_Delete(m_bltState);
    MOS_Delete(m_veboxCopyState);
    MOS_Delete(m_renderCopy);
    if (m_mhwInterfacesXeHpm)
    {
        if (m_mhwInterfacesXeHpm->m_cpInterface)
        {
            if (m_osInterface)
            {
                m_osInterface->pfnDeleteMhwCpInterface(m_mhwInterfacesXeHpm->m_cpInterface);
                m_mhwInterfacesXeHpm->m_cpInterface = nullptr;
            }
            else
            {
                MCPY_ASSERTMESSAGE("Failed to destroy cpInterface.");
            }
        }
        MOS_Delete(m_mhwInterfacesXeHpm->m_miInterface);
        MOS_Delete(m_mhwInterfacesXeHpm->m_veboxInterface);
        MOS_Delete(m_mhwInterfacesXeHpm->m_bltInterface);
        MOS_Delete(m_mhwInterfacesXeHpm->m_renderInterface);
        MOS_Delete(m_mhwInterfacesXeHpm);
        m_mhwInterfacesXeHpm = nullptr;
    }
}

bool MediaCopyState_Xe_Hpm::RenderFormatSupportCheck(PMOS_RESOURCE src, PMOS_RESOURCE dst)
{
    MOS_STATUS              eStatus1, eStatus2;
    VPHAL_GET_SURFACE_INFO  Info;
    VPHAL_SURFACE           Source;
    VPHAL_SURFACE           Target;
    MOS_ZeroMemory(&Info, sizeof(VPHAL_GET_SURFACE_INFO));
    MOS_ZeroMemory(&Source, sizeof(VPHAL_SURFACE));
    MOS_ZeroMemory(&Target, sizeof(VPHAL_SURFACE));

    Source.OsResource = *src;
    Source.Format     = Format_Invalid;
    eStatus1 = VpHal_GetSurfaceInfo(
                   m_osInterface,
                   &Info,
                   &Source);
    Target.OsResource = *dst;
    Target.Format     = Format_Invalid;
    eStatus2 = VpHal_GetSurfaceInfo(
                   m_osInterface,
                   &Info,
                   &Target);

    if ((Source.Format != Target.Format) || (eStatus1 != MOS_STATUS_SUCCESS) || (eStatus2 != MOS_STATUS_SUCCESS))
    {
         MCPY_ASSERTMESSAGE("wrong input parmaters or src format don't match the dest format ");
         return false;
    }

    if ((Source.Format != Format_RGBP) && (Source.Format != Format_NV12) && (Source.Format != Format_RGB)
     && (Source.Format != Format_P010) && (Source.Format != Format_P016) && (Source.Format != Format_YUY2)
     && (Source.Format != Format_Y210) && (Source.Format != Format_Y216) && (Source.Format != Format_AYUV)
     && (Source.Format != Format_Y410) && (Source.Format != Format_A8R8G8B8))
    {
         MCPY_NORMALMESSAGE("render copy doesn't suppport format %d ", Source.Format);
         return false;
    }

    if ((MOS_TILE_LINEAR == Source.TileType) && (MOS_TILE_LINEAR == Target.TileType))
    {
         MCPY_NORMALMESSAGE("render copy doesn't suppport  linear to linear copy");
         return false;
    }

    return true;
}

MOS_STATUS MediaCopyState_Xe_Hpm::MediaVeboxCopy(PMOS_RESOURCE src, PMOS_RESOURCE dst)
{
    // implementation
    MCPY_CHK_NULL_RETURN(m_veboxCopyState);
    return m_veboxCopyState->CopyMainSurface(src, dst);
}

bool MediaCopyState_Xe_Hpm::IsVeboxCopySupported(PMOS_RESOURCE src, PMOS_RESOURCE dst)
{
    bool supported = false;

    if (m_osInterface &&
        !MEDIA_IS_SKU(m_osInterface->pfnGetSkuTable(m_osInterface), FtrVERing))
    {
        return false;
    }

    if (m_veboxCopyState)
    {
        supported = m_veboxCopyState->IsFormatSupported(src) && m_veboxCopyState->IsFormatSupported(dst);
    }

    if (src->TileType == MOS_TILE_LINEAR &&
        dst->TileType == MOS_TILE_LINEAR)
    {
        supported = false;
    }

    return supported;
}

MOS_STATUS MediaCopyState_Xe_Hpm::FeatureSupport(PMOS_RESOURCE src, PMOS_RESOURCE dst, MCPY_ENGINE_CAPS& caps)
{
    caps.engineVebox  = true;
    caps.engineBlt    = true;
    caps.engineRender = true;
    return MOS_STATUS_SUCCESS;
}

MOS_STATUS MediaCopyState_Xe_Hpm::MediaBltCopy(PMOS_RESOURCE src, PMOS_RESOURCE dst)
{
    // implementation
    if (m_bltState != nullptr)
    {
        return m_bltState->CopyMainSurface(src, dst);
    }
    else
    {
        return MOS_STATUS_UNIMPLEMENTED;
    }
}

MOS_STATUS MediaCopyState_Xe_Hpm::MediaRenderCopy(PMOS_RESOURCE src, PMOS_RESOURCE dst)
{
    // implementation
     if (m_renderCopy != nullptr)
     {
      return m_renderCopy->CopySurface(src, dst);
     }
     else
     {
      return MOS_STATUS_UNIMPLEMENTED;
     }
}

MOS_STATUS MediaCopyState_Xe_Hpm::PreCheckCpCopy(
    MCPY_STATE_PARAMS src, MCPY_STATE_PARAMS dest, MCPY_METHOD preferMethod)
{
    if ((preferMethod == MCPY_METHOD_POWERSAVING)
        && (src.CpMode == MCPY_CPMODE_CP)
        && (dest.CpMode == MCPY_CPMODE_CLEAR))
    {
        //Allow blt engine to do copy when dst buffer is staging buffer and allocate in system mem, since protection off with blt engine.
        //Current only used for small localbar cases.
        m_allowCPBltCopy = true;
    }
    else
    {
        m_allowCPBltCopy = false;
    }

    return MOS_STATUS_SUCCESS;
}
