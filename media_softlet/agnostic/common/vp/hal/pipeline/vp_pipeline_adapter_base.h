/*
* Copyright (c) 2021, Intel Corporation
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
//! \file     vp_pipeline_adapter_base.h
//! \brief    vp pipeline adapter base clarification
//! \details  vp pipeline adapter base clarification inlcuding:
//!           some marcro, enum, structure, function
//!
#ifndef __VP_PIPELINE_ADAPTER_BASE_H__
#define __VP_PIPELINE_ADAPTER_BASE_H__
#include "vphal_common.h"
#include "vphal_common_tools.h"
#include "mhw_vebox.h"
#include "mhw_sfc.h"
#include "vp_utils.h"
#include "media_interfaces_mhw.h"

namespace vp
{
class VpPlatformInterface;
}

//-----------------------------------------------------------------------------
// VPHAL-DDI RENDERING INTERFACE
//
//      Params that may apply to more than one layer are part of VPHAL_SURFACE
//      DDI layers must set this interface before calling pfnRender
//-----------------------------------------------------------------------------
//!
//! Structure VphalSettings
//! \brief VPHAL Settings - controls allocation of internal resources in VPHAL
//!
struct VpSettings
{
    //!
    //! \brief    VphalSettings Constructor
    //! \details  Creates instance of VphalSettings
    //!
    VpSettings()    : maxPhases(0),
                      mediaStates(0),
                      sameSampleThreshold(0),
                      disableDnDi(0),
                      kernelUpdate(0),
                      disableHdr(0),
                      veboxParallelExecution(0){};

    int32_t  maxPhases;
    int32_t  mediaStates;
    int32_t  sameSampleThreshold;
    uint32_t disableDnDi;             //!< For validation purpose
    uint32_t kernelUpdate;            //!< For VEBox Copy and Update kernels
    uint32_t disableHdr;              //!< Disable Hdr
    uint32_t veboxParallelExecution;  //!< Control VEBox parallel execution with render engine
};

//!
//! Structure VphalFeatureReport
//! \brief    Vphal Feature Report Structure
//!
struct VpFeatureReport
{
    //!
    //! \brief    VphalFeatureReport Constructor
    //! \details  Creates instance of VphalFeatureReport
    //!
    VpFeatureReport(void *owner = nullptr)
    {
        this->owner = owner;
        // call InitReportValue() to initialize report value
        InitReportValue();
    };

    //!
    //! \brief    initialize VphalFeatureReport value
    //! \details  initialize VphalFeatureReport value, can use it to reset report value
    //!
    void                          InitReportValue()
    {
        IECP                = false;
        IEF                 = false;
        Denoise             = false;
        ChromaDenoise       = false;
        DeinterlaceMode     = VPHAL_DI_REPORT_PROGRESSIVE;
        ScalingMode         = VPHAL_SCALING_NEAREST;
        OutputPipeMode      = VPHAL_OUTPUT_PIPE_MODE_COMP;
        VPMMCInUse          = false;
        RTCompressible      = false;
        RTCompressMode      = 0;
        FFDICompressible    = false;
        FFDICompressMode    = 0;
        FFDNCompressible    = false;
        FFDNCompressMode    = 0;
        STMMCompressible    = false;
        STMMCompressMode    = 0;
        ScalerCompressible  = false;
        ScalerCompressMode  = 0;
        PrimaryCompressible = false;
        PrimaryCompressMode = 0;
        CompositionMode     = VPHAL_NO_COMPOSITION;
        DiScdMode           = false;
        VEFeatureInUse      = false;
        HDRMode             = VPHAL_HDR_MODE_NONE;
    }

    void *                        owner = nullptr;      //!< Pointer to object creating the report
    bool                          IECP;                 //!< IECP enable/disable
    bool                          IEF;                  //!< Enhancement filter
    bool                          Denoise;              //!< Denoise
    bool                          ChromaDenoise;        //!< Chroma Denoise
    VPHAL_DI_REPORT_MODE          DeinterlaceMode;      //!< Deinterlace mode
    VPHAL_SCALING_MODE            ScalingMode;          //!< Scaling mode
    VPHAL_OUTPUT_PIPE_MODE        OutputPipeMode;       //!< Output Pipe
    bool                          VPMMCInUse;           //!< MMC enable flag
    bool                          RTCompressible;       //!< RT MMC Compressible flag
    uint8_t                       RTCompressMode;       //!< RT MMC Compression mode
    bool                          FFDICompressible;     //!< FFDI MMC Compressible flag
    uint8_t                       FFDICompressMode;     //!< FFDI MMC Compression mode
    bool                          FFDNCompressible;     //!< FFDN MMC Compressible flag
    uint8_t                       FFDNCompressMode;     //!< FFDN MMC Compression mode
    bool                          STMMCompressible;     //!< STMM MMC Compressible flag
    uint8_t                       STMMCompressMode;     //!< STMM MMC Compression mode
    bool                          ScalerCompressible;   //!< Scaler MMC Compressible flag for Gen10
    uint8_t                       ScalerCompressMode;   //!< Scaler MMC Compression mode for Gen10
    bool                          PrimaryCompressible;  //!< Input Primary Surface Compressible flag
    uint8_t                       PrimaryCompressMode;  //!< Input Primary Surface Compression mode
    VPHAL_COMPOSITION_REPORT_MODE CompositionMode;      //!< Inplace/Legacy Compostion flag
    bool                          VEFeatureInUse;       //!< If any VEBOX feature is in use, excluding pure bypass for SFC
    bool                          DiScdMode;            //!< Scene change detection
    VPHAL_HDR_MODE                HDRMode;              //!< HDR mode
};

struct _VP_MHWINTERFACE
{
    // Internals
    PLATFORM             m_platform;
    MEDIA_FEATURE_TABLE *m_skuTable;
    MEDIA_WA_TABLE *     m_waTable;

    // States
    PMOS_INTERFACE           m_osInterface;
    PRENDERHAL_INTERFACE     m_renderHal;
    PMHW_VEBOX_INTERFACE     m_veboxInterface;
    MhwCpInterface *         m_cpInterface;
    PMHW_SFC_INTERFACE       m_sfcInterface;
    PMHW_MI_INTERFACE        m_mhwMiInterface;
    vp::VpPlatformInterface *m_vpPlatformInterface;
    void *                   m_settings;
    VpFeatureReport *        m_reporting;

    // Render GPU context/node
    MOS_GPU_NODE    m_renderGpuNode;
    MOS_GPU_CONTEXT m_renderGpuContext;

    // vp Pipeline workload status report
    PVPHAL_STATUS_TABLE m_statusTable;

    void *m_debugInterface;
};

using VP_MHWINTERFACE  = _VP_MHWINTERFACE;

//!
//! Class VpPipelineAdapterBase
//! \brief VP_INTERFACE class definition
//!
class VpPipelineAdapterBase
{
public:
    // factory function
    static VpPipelineAdapterBase *VphalStateFactory(
        PMOS_INTERFACE pOsInterface,
        PMOS_CONTEXT   pOsDriverContext,
        MOS_STATUS *   peStatus);

    //!
    //! \brief    VpPipelineAdapterBase Constructor
    //! \details  Creates instance of VpPipelineAdapterBase
    //!           - Caller must call Allocate to allocate all VPHAL states and objects.
    //! \param    [in] pOsInterface
    //!           OS interface, if provided externally - may be nullptr
    //! \param    [in] pOsDriverContext
    //!           OS driver context (UMD context, pShared, ...)
    //! \param    [in,out] peStatus
    //!           Pointer to the MOS_STATUS flag.
    //!           Will assign this flag to MOS_STATUS_SUCCESS if successful, otherwise failed
    //!
    VpPipelineAdapterBase(
        vp::VpPlatformInterface &vpPlatformInterface,
        MOS_STATUS &eStatus);

    virtual MOS_STATUS GetVpMhwInterface(
        VP_MHWINTERFACE &vpMhwinterface);

    //!
    //! \brief    Allocate VpPipelineAdapterBase Resources
    //! \details  Allocate VpPipelineAdapterBase Resources
    //!           - Allocate and initialize HW states
    //!           - Allocate and initialize renderer states
    //! \param    [in] pVpHalSettings
    //!           Pointer to VPHAL Settings
    //! \return   MOS_STATUS
    //!           Return MOS_STATUS_SUCCESS if successful, otherwise failed
    //!
    virtual MOS_STATUS Allocate(
        const VpSettings *pVpSettings) = 0;

    //!
    //! \brief    Performs VP Rendering
    //! \details  Performs VP Rendering
    //!           - call default render of video
    //! \param    [in] pcRenderParams
    //!           Pointer to Render Params
    //! \return   MOS_STATUS
    //!           Return MOS_STATUS_SUCCESS if successful, otherwise failed
    //!
    virtual MOS_STATUS Render(
        PCVPHAL_RENDER_PARAMS pcRenderParams) = 0;

    //!
    //! \brief    Get Status Report
    //! \details  Get Status Report, will return back to app indicating if related frame id is done by gpu
    //! \param    [out] pQueryReport
    //!           Pointer to pQueryReport, the status query report array.
    //! \param    [in] wStatusNum
    //!           The size of array pQueryReport.
    //! \return   MOS_STATUS
    //!           Return MOS_STATUS_SUCCESS if successful, otherwise failed
    virtual MOS_STATUS GetStatusReport(
        PQUERY_STATUS_REPORT_APP pQueryReport,
        uint16_t                 numStatus);

    virtual VpFeatureReport *GetRenderFeatureReport() = 0;

    //!
    //! \brief    VpPipelineAdapterBase Destuctor
    //! \details  Destroys VpPipelineAdapterBase and all internal states and objects
    //! \return   void
    //!
    virtual ~VpPipelineAdapterBase();

    PMOS_INTERFACE GetOsInterface()
    {
        return m_pOsInterface;
    }

    void SetMhwVeboxInterface(MhwVeboxInterface *veboxInterface)
    {
        if (veboxInterface == nullptr)
        {
            return;
        }

        if (m_veboxInterface != nullptr)
        {
            MOS_STATUS eStatus = m_veboxInterface->DestroyHeap();
            MOS_Delete(m_veboxInterface);
            m_veboxInterface = nullptr;
            if (eStatus != MOS_STATUS_SUCCESS)
            {
                VP_PUBLIC_ASSERTMESSAGE("Failed to destroy Vebox Interface, eStatus:%d.\n", eStatus);
            }
        }

        m_veboxInterface = veboxInterface;
    }

    void SetMhwSfcInterface(MhwSfcInterface *sfcInterface)
    {
        if (sfcInterface == nullptr)
        {
            return;
        }

        if (m_sfcInterface != nullptr)
        {
            MOS_Delete(m_sfcInterface);
            m_sfcInterface = nullptr;
        }

        m_sfcInterface = sfcInterface;
    }

    HANDLE m_gpuAppTaskEvent = nullptr;

protected:
    // Internals
    PLATFORM             m_platform = {};
    MEDIA_FEATURE_TABLE *m_skuTable = nullptr;
    MEDIA_WA_TABLE *     m_waTable  = nullptr;

    // States
    PMOS_INTERFACE       m_pOsInterface = nullptr;
    PRENDERHAL_INTERFACE m_vprenderHal   = nullptr;
    PMHW_VEBOX_INTERFACE m_veboxInterface = nullptr;
    MhwCpInterface *     m_cpInterface    = nullptr;
    PMHW_SFC_INTERFACE   m_sfcInterface   = nullptr;

    // StatusTable indicating if command is done by gpu or not
    VPHAL_STATUS_TABLE       m_statusTable = {};
    vp::VpPlatformInterface &m_vpPlatformInterface;  //!< vp platform interface. Should be destroyed during deconstruction.
};

#endif  // __VP_PIPELINE_ADAPTER_BASE_H__