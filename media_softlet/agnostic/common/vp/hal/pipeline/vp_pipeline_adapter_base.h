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
#include "vp_base.h"

//!
//! \brief Deinterlace Mode enum
//!
typedef enum
{
    VPDDI_PROGRESSIVE = 0,  //!< Progressive mode
    VPDDI_BOB         = 1,  //!< BOB DI mode
    VPDDI_ADI         = 2   //!< ADI mode
} DI_MODE;

//!
//! \brief Scaling Mode enum
//!
typedef enum
{
    VPDDI_SCALING                = 0,  //!< Bilinear scaling
    VPDDI_ADVANCEDSCALING        = 1,  //!< AVS scaling
    VPDDI_SUPERRESOLUTIONSCALING = 2   //!< Super scaling
} SCALING_MODE;

//!
//! Class VpPipelineAdapterBase
//! \brief VP_INTERFACE class definition
//!
class VpPipelineAdapterBase : public VpBase
{
public:
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
    
    virtual MOS_STATUS GetStatusReportEntryLength(
        uint32_t                         *puiLength);

    virtual VpFeatureReport *GetRenderFeatureReport() = 0;

    //!
    //! \brief    VpPipelineAdapterBase Destuctor
    //! \details  Destroys VpPipelineAdapterBase and all internal states and objects
    //! \return   void
    //!
    virtual ~VpPipelineAdapterBase();

    virtual PMOS_INTERFACE GetOsInterface()
    {
        return m_osInterface;
    }

    virtual MEDIA_FEATURE_TABLE *GetSkuTable()
    {
        return m_skuTable;
    }

    virtual PLATFORM &GetPlatform()
    {
        return m_platform;
    }

    virtual PRENDERHAL_INTERFACE GetRenderHal()
    {
        return m_vprenderHal;
    }

    void SetMhwVeboxInterface(MhwVeboxInterface *veboxInterface)
    {
        MOS_STATUS                  eStatus = MOS_STATUS_SUCCESS;
        if (veboxInterface == nullptr)
        {
            return;
        }

        if (m_veboxInterface != nullptr)
        {
            m_veboxItf = std::static_pointer_cast<mhw::vebox::Itf>(m_veboxInterface->GetNewVeboxInterface());

            if (m_veboxItf)
            {
                eStatus = m_veboxItf->DestroyHeap();
            }
            else
            {
                eStatus = m_veboxInterface->DestroyHeap();
            }

            MOS_Delete(m_veboxInterface);
            m_veboxInterface = nullptr;
            m_veboxItf       = nullptr;
            if (eStatus != MOS_STATUS_SUCCESS)
            {
                VP_PUBLIC_ASSERTMESSAGE("Failed to destroy Vebox Interface, eStatus:%d.\n", eStatus);
            }
        }

        m_veboxInterface = veboxInterface;
        m_veboxItf = std::static_pointer_cast<mhw::vebox::Itf>(veboxInterface->GetNewVeboxInterface());
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

protected:
    // Internals
    PLATFORM             m_platform = {};
    MEDIA_FEATURE_TABLE *m_skuTable = nullptr;
    MEDIA_WA_TABLE *     m_waTable  = nullptr;

    // States
    PMOS_INTERFACE       m_osInterface   = nullptr;
    PRENDERHAL_INTERFACE m_vprenderHal   = nullptr;
    PMHW_VEBOX_INTERFACE m_veboxInterface = nullptr;
    MhwCpInterface *     m_cpInterface    = nullptr;
    PMHW_SFC_INTERFACE   m_sfcInterface   = nullptr;
    std::shared_ptr<mhw::vebox::Itf> m_veboxItf = nullptr;

    // StatusTable indicating if command is done by gpu or not
    VPHAL_STATUS_TABLE       m_statusTable = {};
    vp::VpPlatformInterface &m_vpPlatformInterface;  //!< vp platform interface. Should be destroyed during deconstruction.

MEDIA_CLASS_DEFINE_END(VpPipelineAdapterBase)
};

#endif  // __VP_PIPELINE_ADAPTER_BASE_H__
