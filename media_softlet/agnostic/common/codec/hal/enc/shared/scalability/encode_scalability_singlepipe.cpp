/*
* Copyright (c) 2018, Intel Corporation
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
//! \file     media_scalability_singlepipe.cpp
//! \brief    Defines the common interface for media scalability singlepipe mode.
//! \details  The media scalability singlepipe interface is further sub-divided by component,
//!           this file is for the base interface which is shared by all components.
//!

#include "codec_hw_next.h"
#include "encode_scalability_defs.h"
#include "encode_scalability_singlepipe.h"

#include "media_context.h"
#include "media_status_report.h"
#include "mhw_utilities.h"
#include "encode_status_report_defs.h"
#include "hal_oca_interface.h"

namespace encode
{

EncodeScalabilitySinglePipe::EncodeScalabilitySinglePipe(void *hwInterface, MediaContext *mediaContext, uint8_t componentType) :
    MediaScalabilitySinglePipe(hwInterface, mediaContext, componentType)
{
    if (hwInterface == nullptr)
    {
        return;
    }
    m_hwInterface = (CodechalHwInterfaceNext *)hwInterface;
    m_osInterface = m_hwInterface->GetOsInterface();
}

MOS_STATUS EncodeScalabilitySinglePipe::Initialize(const MediaScalabilityOption &option)
{
    SCALABILITY_CHK_NULL_RETURN(m_osInterface);

    m_scalabilityOption = MOS_New(EncodeScalabilityOption, (const EncodeScalabilityOption &)option);
    SCALABILITY_CHK_NULL_RETURN(m_scalabilityOption);

    MOS_USER_FEATURE_VALUE_DATA userFeatureData;
    MOS_ZeroMemory(&userFeatureData, sizeof(userFeatureData));
    auto statusKey = MOS_UserFeature_ReadValue_ID(
        nullptr,
        __MEDIA_USER_FEATURE_VALUE_ENCODE_ENABLE_FRAME_TRACKING_ID,
        &userFeatureData,
        m_osInterface->pOsContext);
    if (statusKey == MOS_STATUS_SUCCESS)
    {
        m_frameTrackingEnabled = userFeatureData.i32Data ? true : false;
    }
    else
    {
        m_frameTrackingEnabled = m_osInterface->bEnableKmdMediaFrameTracking ? true : false;
    }

    // !Don't check the return status here, because this function will return fail if there's no regist key in register.
    // But it's normal that regist key not in register.
    Mos_CheckVirtualEngineSupported(m_osInterface, false, true);
    m_miInterface = m_hwInterface->GetMiInterface();
    SCALABILITY_CHK_NULL_RETURN(m_miInterface);

    SCALABILITY_CHK_STATUS_RETURN(MediaScalabilitySinglePipe::Initialize(option));

    PMOS_GPUCTX_CREATOPTIONS_ENHANCED gpuCtxCreateOption = 
                    dynamic_cast<PMOS_GPUCTX_CREATOPTIONS_ENHANCED>(m_gpuCtxCreateOption);
    EncodeScalabilityOption *scalabilityOption  = 
                    dynamic_cast<EncodeScalabilityOption *>(m_scalabilityOption);
    //The changes to indicate vdenc is required when slim vdbox is supported
    if(gpuCtxCreateOption != nullptr &&
       scalabilityOption != nullptr &&
       scalabilityOption->IsVdencEnabled() &&
       MEDIA_IS_SKU(m_osInterface->pfnGetSkuTable(m_osInterface), FtrWithSlimVdbox))
    {
        gpuCtxCreateOption->Flags |=  (1 << 2);
    }

    return MOS_STATUS_SUCCESS;
}

MOS_STATUS EncodeScalabilitySinglePipe::VerifyCmdBuffer(uint32_t requestedSize, uint32_t requestedPatchListSize, bool &singleTaskPhaseSupportedInPak)
{
    SCALABILITY_FUNCTION_ENTER;

    return MediaScalabilitySinglePipe::VerifyCmdBuffer(requestedSize, requestedPatchListSize, singleTaskPhaseSupportedInPak);
}

MOS_STATUS EncodeScalabilitySinglePipe::VerifySpaceAvailable(uint32_t requestedSize, uint32_t requestedPatchListSize, bool &singleTaskPhaseSupportedInPak)
{
    MOS_STATUS eStatus = MOS_STATUS_SUCCESS;

    SCALABILITY_FUNCTION_ENTER;;

    bool bothPatchListAndCmdBufChkSuccess = false;
    uint8_t looptimes = m_singleTaskPhaseSupported ? 2 : 1;

    for(auto i = 0 ; i < looptimes ; i++)
    {
        SCALABILITY_CHK_STATUS_RETURN(MediaScalability::VerifySpaceAvailable(
            requestedSize, requestedPatchListSize, bothPatchListAndCmdBufChkSuccess));
        
        if (bothPatchListAndCmdBufChkSuccess == true)
        {
            singleTaskPhaseSupportedInPak = m_singleTaskPhaseSupported;
            return eStatus;
        }
        
        MOS_STATUS statusPatchList = MOS_STATUS_SUCCESS;
        MOS_STATUS statusCmdBuf    = MOS_STATUS_SUCCESS;
        
        if (requestedPatchListSize > 0)
        {
            statusPatchList = (MOS_STATUS)m_osInterface->pfnVerifyPatchListSize(
                m_osInterface,
                requestedPatchListSize);
        }

        statusCmdBuf = (MOS_STATUS)m_osInterface->pfnVerifyCommandBufferSize(
            m_osInterface,
            requestedSize,
            0);

        if ((statusCmdBuf == MOS_STATUS_SUCCESS) && (statusPatchList == MOS_STATUS_SUCCESS))
        {
            singleTaskPhaseSupportedInPak = m_singleTaskPhaseSupported;
            return eStatus;
        }
    }

    eStatus = MOS_STATUS_NO_SPACE;
    SCALABILITY_ASSERTMESSAGE("Resize Command buffer failed with no space!");
    return eStatus;

}

MOS_STATUS EncodeScalabilitySinglePipe::UpdateState(void *statePars)
{
    SCALABILITY_FUNCTION_ENTER;
    SCALABILITY_CHK_STATUS_RETURN(MediaScalabilitySinglePipe::UpdateState(statePars));

    MOS_STATUS   eStatus         = MOS_STATUS_SUCCESS;

    StateParams *encodeStatePars = (StateParams *)statePars;
    m_singleTaskPhaseSupported   = encodeStatePars->singleTaskPhaseSupported;
    m_statusReport               = encodeStatePars->statusReport;
    m_currentPass                = encodeStatePars->currentPass;
    m_currentRow                 = encodeStatePars->currentRow;
    m_currentSubPass             = encodeStatePars->currentSubPass;
    SCALABILITY_CHK_NULL_RETURN(m_statusReport);

    return eStatus;
}

MOS_STATUS EncodeScalabilitySinglePipe::ResizeCommandBufferAndPatchList(
    uint32_t                    requestedCommandBufferSize,
    uint32_t                    requestedPatchListSize)
{
    SCALABILITY_FUNCTION_ENTER;
    SCALABILITY_CHK_NULL_RETURN(m_hwInterface);

    return m_hwInterface->ResizeCommandBufferAndPatchList(requestedCommandBufferSize, requestedPatchListSize);
}

MOS_STATUS EncodeScalabilitySinglePipe::SendAttrWithFrameTracking(
    MOS_COMMAND_BUFFER &cmdBuffer,
    bool                frameTrackingRequested)
{
    MOS_STATUS eStatus = MOS_STATUS_SUCCESS;

    SCALABILITY_FUNCTION_ENTER;

    bool renderEngineUsed = m_mediaContext->IsRenderEngineUsed();

    // initialize command buffer attributes
    cmdBuffer.Attributes.bTurboMode               = m_hwInterface->m_turboMode;
    cmdBuffer.Attributes.bMediaPreemptionEnabled  = renderEngineUsed ? m_hwInterface->GetRenderInterfaceNext()->IsPreemptionEnabled() : 0;
    cmdBuffer.Attributes.dwNumRequestedEUSlices   = m_hwInterface->m_numRequestedEuSlices;
    cmdBuffer.Attributes.dwNumRequestedSubSlices  = m_hwInterface->m_numRequestedSubSlices;
    cmdBuffer.Attributes.dwNumRequestedEUs        = m_hwInterface->m_numRequestedEus;
    cmdBuffer.Attributes.bValidPowerGatingRequest = true;

    PMOS_RESOURCE resource = nullptr;
    uint32_t      offset   = 0;

    if (frameTrackingRequested && m_frameTrackingEnabled)
    {
        m_statusReport->GetAddress(encode::statusReportGlobalCount, resource, offset);
        cmdBuffer.Attributes.bEnableMediaFrameTracking    = true;
        cmdBuffer.Attributes.resMediaFrameTrackingSurface = resource;
        cmdBuffer.Attributes.dwMediaFrameTrackingTag      = m_statusReport->GetSubmittedCount() + 1;
        // Set media frame tracking address offset(the offset from the encoder status buffer page)
        cmdBuffer.Attributes.dwMediaFrameTrackingAddrOffset = 0;
    }

    return eStatus;
}
//#include "mhw_mi_impl.h"
////VDBOX MFX register offsets
//static constexpr uint32_t MFC_IMAGE_STATUS_MASK_REG_OFFSET_NODE_1_INIT           = 0x1C08B4;
//static constexpr uint32_t MFC_IMAGE_STATUS_CTRL_REG_OFFSET_NODE_1_INIT           = 0x1C08B8;
//static constexpr uint32_t MFC_AVC_NUM_SLICES_REG_OFFSET_NODE_1_INIT              = 0x1C0954;
//static constexpr uint32_t MFC_QP_STATUS_COUNT_OFFSET_NODE_1_INIT                 = 0x1C08BC;
//static constexpr uint32_t MFX_ERROR_FLAG_REG_OFFSET_NODE_1_INIT                  = 0x1C0800;
//static constexpr uint32_t MFX_FRAME_CRC_REG_OFFSET_NODE_1_INIT                   = 0x1C0850;
//static constexpr uint32_t MFX_MB_COUNT_REG_OFFSET_NODE_1_INIT                    = 0x1C0868;
//static constexpr uint32_t MFC_BITSTREAM_BYTECOUNT_FRAME_REG_OFFSET_NODE_1_INIT   = 0x1C08A0;
//static constexpr uint32_t MFC_BITSTREAM_SE_BITCOUNT_FRAME_REG_OFFSET_NODE_1_INIT = 0x1C08A4;
//static constexpr uint32_t MFC_BITSTREAM_BYTECOUNT_SLICE_REG_OFFSET_NODE_1_INIT   = 0x1C08D0;
////VDBOX MFX register initial value
//static constexpr uint32_t MFX_LRA0_REG_OFFSET_NODE_1_INIT = 0;
//static constexpr uint32_t MFX_LRA1_REG_OFFSET_NODE_1_INIT = 0;
//static constexpr uint32_t MFX_LRA2_REG_OFFSET_NODE_1_INIT = 0;
//void EncodeScalabilitySinglePipe::InitMmioRegisters()
//{
//    MmioRegistersMfx *mmioRegisters = &m_mmioRegisters[MHW_VDBOX_NODE_1];
//
//    mmioRegisters->generalPurposeRegister0LoOffset      = mhw::mi::GENERAL_PURPOSE_REGISTER0_LO_OFFSET_NODE_1_INIT;
//    mmioRegisters->generalPurposeRegister0HiOffset      = mhw::mi::GENERAL_PURPOSE_REGISTER0_HI_OFFSET_NODE_1_INIT;
//    mmioRegisters->generalPurposeRegister4LoOffset      = mhw::mi::GENERAL_PURPOSE_REGISTER4_LO_OFFSET_NODE_1_INIT;
//    mmioRegisters->generalPurposeRegister4HiOffset      = mhw::mi::GENERAL_PURPOSE_REGISTER4_HI_OFFSET_NODE_1_INIT;
//    mmioRegisters->generalPurposeRegister11LoOffset     = mhw::mi::GENERAL_PURPOSE_REGISTER11_LO_OFFSET_NODE_1_INIT;
//    mmioRegisters->generalPurposeRegister11HiOffset     = mhw::mi::GENERAL_PURPOSE_REGISTER11_HI_OFFSET_NODE_1_INIT;
//    mmioRegisters->generalPurposeRegister12LoOffset     = mhw::mi::GENERAL_PURPOSE_REGISTER12_LO_OFFSET_NODE_1_INIT;
//    mmioRegisters->generalPurposeRegister12HiOffset     = mhw::mi::GENERAL_PURPOSE_REGISTER12_HI_OFFSET_NODE_1_INIT;
//    mmioRegisters->mfcImageStatusMaskRegOffset          = MFC_IMAGE_STATUS_MASK_REG_OFFSET_NODE_1_INIT;
//    mmioRegisters->mfcImageStatusCtrlRegOffset          = MFC_IMAGE_STATUS_CTRL_REG_OFFSET_NODE_1_INIT;
//    mmioRegisters->mfcAvcNumSlicesRegOffset             = MFC_AVC_NUM_SLICES_REG_OFFSET_NODE_1_INIT;
//    mmioRegisters->mfcQPStatusCountOffset               = MFC_QP_STATUS_COUNT_OFFSET_NODE_1_INIT;
//    mmioRegisters->mfxErrorFlagsRegOffset               = MFX_ERROR_FLAG_REG_OFFSET_NODE_1_INIT;
//    mmioRegisters->mfxFrameCrcRegOffset                 = MFX_FRAME_CRC_REG_OFFSET_NODE_1_INIT;
//    mmioRegisters->mfxMBCountRegOffset                  = MFX_MB_COUNT_REG_OFFSET_NODE_1_INIT;
//    mmioRegisters->mfcBitstreamBytecountFrameRegOffset  = MFC_BITSTREAM_BYTECOUNT_FRAME_REG_OFFSET_NODE_1_INIT;
//    mmioRegisters->mfcBitstreamSeBitcountFrameRegOffset = MFC_BITSTREAM_SE_BITCOUNT_FRAME_REG_OFFSET_NODE_1_INIT;
//    mmioRegisters->mfcBitstreamBytecountSliceRegOffset  = MFC_BITSTREAM_BYTECOUNT_SLICE_REG_OFFSET_NODE_1_INIT;
//    mmioRegisters->mfxLra0RegOffset                     = MFX_LRA0_REG_OFFSET_NODE_1_INIT;
//    mmioRegisters->mfxLra1RegOffset                     = MFX_LRA1_REG_OFFSET_NODE_1_INIT;
//    mmioRegisters->mfxLra2RegOffset                     = MFX_LRA2_REG_OFFSET_NODE_1_INIT;
//
//    m_mmioRegisters[MHW_VDBOX_NODE_2] = m_mmioRegisters[MHW_VDBOX_NODE_1];
//}
//MmioRegistersMfx *EncodeScalabilitySinglePipe::GetMmioRegisters(MHW_VDBOX_NODE_IND index)
//{
//    if (index < MHW_VDBOX_NODE_MAX)
//    {
//        return &m_mmioRegisters[index];
//    }
//    else
//    {
//        MHW_ASSERT("index is out of range!");
//        return &m_mmioRegisters[MHW_VDBOX_NODE_1];
//    }
//}
//bool EncodeScalabilitySinglePipe::ConvertToMiRegister(MHW_VDBOX_NODE_IND index, MHW_MI_MMIOREGISTERS &mmioRegister)
//{
//    MmioRegistersMfx *mfxMmioReg = GetMmioRegisters(index);
//    if (mfxMmioReg)
//    {
//        mmioRegister.generalPurposeRegister0LoOffset  = mfxMmioReg->generalPurposeRegister0LoOffset;
//        mmioRegister.generalPurposeRegister0HiOffset  = mfxMmioReg->generalPurposeRegister0HiOffset;
//        mmioRegister.generalPurposeRegister4LoOffset  = mfxMmioReg->generalPurposeRegister4LoOffset;
//        mmioRegister.generalPurposeRegister4HiOffset  = mfxMmioReg->generalPurposeRegister4HiOffset;
//        mmioRegister.generalPurposeRegister11LoOffset = mfxMmioReg->generalPurposeRegister11LoOffset;
//        mmioRegister.generalPurposeRegister11HiOffset = mfxMmioReg->generalPurposeRegister11HiOffset;
//        mmioRegister.generalPurposeRegister12LoOffset = mfxMmioReg->generalPurposeRegister12LoOffset;
//        mmioRegister.generalPurposeRegister12HiOffset = mfxMmioReg->generalPurposeRegister12HiOffset;
//        return true;
//    }
//    else
//        return false;
//}

MOS_STATUS EncodeScalabilitySinglePipe::Oca1stLevelBBStart(MOS_COMMAND_BUFFER &cmdBuffer)
{
    MHW_MI_MMIOREGISTERS mmioRegister;
    SCALABILITY_CHK_NULL_RETURN(m_hwInterface);

    auto  vdencItf = m_hwInterface->GetVdencInterfaceNext();
    SCALABILITY_CHK_NULL_RETURN(vdencItf);
    bool validMmio = vdencItf->ConvertToMiRegister(MHW_VDBOX_NODE_1, mmioRegister);
    if (validMmio)
    {
        SCALABILITY_CHK_NULL_RETURN(m_osInterface);
        SCALABILITY_CHK_NULL_RETURN(m_osInterface->pOsContext);
        MhwMiInterface *miInterface = m_hwInterface->GetMiInterface();
        SCALABILITY_CHK_NULL_RETURN(miInterface);

        HalOcaInterface::On1stLevelBBStart(
            cmdBuffer,
            *m_osInterface->pOsContext,
            m_osInterface->CurrentGpuContextHandle,
            *miInterface,
            mmioRegister);
    }

    return MOS_STATUS_SUCCESS;
}

MOS_STATUS EncodeScalabilitySinglePipe::Oca1stLevelBBEnd(MOS_COMMAND_BUFFER &cmdBuffer)
{
    SCALABILITY_CHK_NULL_RETURN(m_osInterface);
    HalOcaInterface::On1stLevelBBEnd(cmdBuffer, *m_osInterface);

    return MOS_STATUS_SUCCESS;
}

}


