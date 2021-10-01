/*
* Copyright (c) 2019-2021, Intel Corporation
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
//! \file     hal_oca_interface.cpp
//! \brief    Implementation of functions for Hal OCA Interface
//! \details  Implementation of functions for Hal OCA Interface
//!

#include "stdint.h"
#include "mos_os.h"
#include "hal_oca_interface.h"
#include "mhw_mmio.h"
#include "mos_interface.h"
#include "mos_oca_interface_specific.h"
#include "vphal.h"
#include "vphal_debug.h"

std::map<uint32_t*, MOS_OCA_BUFFER_HANDLE> HalOcaInterface::s_hOcaMap;

/****************************************************************************************************/
/*                                      HalOcaInterface                                             */
/****************************************************************************************************/

//!
//! \brief  Oca operation which should be called at the beginning of 1st level batch buffer start.
//! \param  [in/out] cmdBuffer
//!         Command buffer for current BB. ocaBufHandle in cmdBuffer will be updated.
//! \param  [in] mosContext
//!         Reference to MOS_CONTEXT.
//! \param  [in] gpuContextHandle
//!         Gpu context handle 
//! \param  [in] mhwMiInterface
//!         Reference to MhwMiInterface.
//! \param  [in] mmioRegisters
//!         mmio registers for current engine.
//! \param  [in] offsetOf1stLevelBB
//!         Offset for current BB in cmdBuffer.
//! \param  [in] bUseSizeOfCmdBuf
//!         If true, use size of cmdBuffer for batch buffer, else use sizeOf1stLevelBB.
//! \param  [in] sizeOf1stLevelBB
//!         Size of BB. Ignore if bUseSizeOfResource == true.
//! \return void
//!         No return value. Handle all exception inside the function.
//!
void HalOcaInterface::On1stLevelBBStart(MOS_COMMAND_BUFFER &cmdBuffer, MOS_CONTEXT &mosContext,
        uint32_t gpuContextHandle, MhwMiInterface &mhwMiInterface, MHW_MI_MMIOREGISTERS &mmioRegisters,
        uint32_t offsetOf1stLevelBB, bool bUseSizeOfCmdBuf, uint32_t sizeOf1stLevelBB)
{
    MosInterface::SetObjectCapture(&cmdBuffer.OsResource);
    MOS_STATUS status                   = MOS_STATUS_SUCCESS;
    MosOcaInterface *pOcaInterface      = &MosOcaInterfaceSpecific::GetInstance();
    MOS_OCA_BUFFER_HANDLE ocaBufHandle  = 0;
    uint64_t  ocaBase                   = 0;
    
    if (nullptr == pOcaInterface || !((MosOcaInterfaceSpecific*)pOcaInterface)->IsOcaEnabled())
    {
        return;
    }
    PMOS_MUTEX mutex = pOcaInterface->GetMutex();
    if (mutex == nullptr)
    {
        OnOcaError(&mosContext, MOS_STATUS_NULL_POINTER, __FUNCTION__, __LINE__);
        return;
    }
    ocaBufHandle = GetOcaBufferHandle(cmdBuffer, mosContext);
    if (ocaBufHandle)
    {
        OnOcaError(&mosContext, status, __FUNCTION__, __LINE__);
        return;
    }
    else
    {
        ocaBufHandle = pOcaInterface->LockOcaBufAvailable(&mosContext, gpuContextHandle);
        if (MOS_OCA_INVALID_BUFFER_HANDLE == ocaBufHandle)
        {
            OnOcaError(&mosContext, MOS_STATUS_UNKNOWN, __FUNCTION__, __LINE__);
            return;
        }
        MosOcaAutoLock autoLock(mutex);
        s_hOcaMap.insert(std::make_pair(cmdBuffer.pCmdBase, ocaBufHandle));
    }
    
    status = pOcaInterface->On1stLevelBBStart(ocaBase, ocaBufHandle, &mosContext, &cmdBuffer.OsResource, 0, true, 0);
    if (MOS_FAILED(status))
    {
        RemoveOcaBufferHandle(cmdBuffer, mosContext);
        OnOcaError(&mosContext, status, __FUNCTION__, __LINE__);
    }
}

//!
//! \brief  Oca operation which should be called at the beginning of 1st level batch buffer start.
//! \param  [in/out] cmdBuffer
//!         Command buffer for current BB. ocaBufHandle in cmdBuffer will be updated.
//! \param  [in] mosContext
//!         Reference to MOS_CONTEXT.
//! \param  [in] gpuContextHandle
//!         Gpu context handle
//! \param  [in] mhwMiInterface
//!         Reference to MhwMiInterface.
//! \param  [in] mmioRegisters
//!         mmio registers for current engine.
//! \param  [in] offsetOf1stLevelBB
//!         Offset for current BB in cmdBuffer.
//! \param  [in] bUseSizeOfCmdBuf
//!         If true, use size of cmdBuffer for batch buffer, else use sizeOf1stLevelBB.
//! \param  [in] sizeOf1stLevelBB
//!         Size of BB. Ignore if bUseSizeOfResource == true.
//! \return void
//!         No return value. Handle all exception inside the function.
//!
void HalOcaInterface::On1stLevelBBStart(MOS_COMMAND_BUFFER &cmdBuffer, MOS_CONTEXT &mosContext,
        uint32_t gpuContextHandle, MhwMiInterface &mhwMiInterface, MmioRegistersMfx &mmioRegisters,
        uint32_t offsetOf1stLevelBB, bool bUseSizeOfCmdBuf, uint32_t sizeOf1stLevelBB)
{
    MosInterface::SetObjectCapture(&cmdBuffer.OsResource);
    MHW_MI_MMIOREGISTERS regs = {};
    regs.generalPurposeRegister0LoOffset = mmioRegisters.generalPurposeRegister0LoOffset;
    regs.generalPurposeRegister0HiOffset = mmioRegisters.generalPurposeRegister0HiOffset;
    regs.generalPurposeRegister4LoOffset = mmioRegisters.generalPurposeRegister4LoOffset;
    regs.generalPurposeRegister4HiOffset = mmioRegisters.generalPurposeRegister4HiOffset;
    regs.generalPurposeRegister11LoOffset = mmioRegisters.generalPurposeRegister11LoOffset;
    regs.generalPurposeRegister11HiOffset = mmioRegisters.generalPurposeRegister11HiOffset;
    regs.generalPurposeRegister12LoOffset = mmioRegisters.generalPurposeRegister12LoOffset;
    regs.generalPurposeRegister12HiOffset = mmioRegisters.generalPurposeRegister12HiOffset;

    On1stLevelBBStart(cmdBuffer, mosContext, gpuContextHandle, mhwMiInterface, regs,
        offsetOf1stLevelBB, bUseSizeOfCmdBuf, sizeOf1stLevelBB);
}

//!
//! \brief  Oca operation which should be called before adding batch buffer end command for 1st
//!         level batch buffer.
//! \param  [in/out] cmdBuffer
//!         Command buffer for current BB. ocaBufHandle in cmdBuffer will be updated.
//! \param  [in] osInterface
//!         Reference to MOS_INTERFACE.
//! \return void
//!         No return value. Handle all exception inside the function.
//!
void HalOcaInterface::On1stLevelBBEnd(MOS_COMMAND_BUFFER &cmdBuffer, MOS_INTERFACE &osInterface)
{
    if (nullptr == osInterface.pOsContext)
    {
        OnOcaError(nullptr, MOS_STATUS_INVALID_PARAMETER, __FUNCTION__, __LINE__);
        return;
    }
    MOS_CONTEXT &           mosContext     = *osInterface.pOsContext;
    MosOcaInterface *pOcaInterface         = &MosOcaInterfaceSpecific::GetInstance();
    MOS_OCA_BUFFER_HANDLE   ocaBufHandle   = 0;
    MOS_STATUS              status         = MOS_STATUS_SUCCESS;

    if (nullptr == pOcaInterface || !((MosOcaInterfaceSpecific*)pOcaInterface)->IsOcaEnabled() || (ocaBufHandle = GetOcaBufferHandle(cmdBuffer, mosContext)) >= MAX_NUM_OF_OCA_BUF_CONTEXT)
    {
        // Will come here for UMD_OCA not being enabled case.
        return;
    }

    status = pOcaInterface->On1stLevelBBEnd(ocaBufHandle, osInterface.pOsContext);
    if (MOS_FAILED(status))
    {
        // UnlockOcaBuf and RemoveOcaBufferHandle should also be called
        // for failure case to avoid oca buffer leak.
        OnOcaError(osInterface.pOsContext, status, __FUNCTION__, __LINE__);
    }
    status = pOcaInterface->UnlockOcaBuf(ocaBufHandle);
    if (MOS_FAILED(status))
    {
        OnOcaError(osInterface.pOsContext, status, __FUNCTION__, __LINE__);
    }

    // remove the oca buffer handle
    RemoveOcaBufferHandle(cmdBuffer, mosContext);
}

//!
//! \brief  Oca operation which should be called before sending start sub level batch buffer command.
//! \param  [in] cmdBuffer
//!         Command buffer for current BB.
//! \param  [in] mosContext
//!         Reference to MOS_CONTEXT.
//! \param  [in] pMosResource
//!         Pointer to the MOS_RESOURCE.
//! \param  [in] offsetOfSubLevelBB
//!         Offset for current BB in pMosResource.
//! \param  [in] bUseSizeOfResource
//!         If true, use size of pMosResource for batch buffer, else use sizeOfIndirectState.
//! \param  [in] sizeOfSubLevelBB
//!         Size of BB. Ignore if bUseSizeOfResource == true.
//! \return void
//!         No return value. Handle all exception inside the function.
//!
void HalOcaInterface::OnSubLevelBBStart(MOS_COMMAND_BUFFER &cmdBuffer, MOS_CONTEXT &mosContext, void *pMosResource, uint32_t offsetOfSubLevelBB, bool bUseSizeOfResource, uint32_t sizeOfSubLevelBB)
{
    MosInterface::SetObjectCapture((PMOS_RESOURCE)pMosResource);
}

//!
//! \brief  Oca operation which should be called when indirect states being added.
//! \param  [in] cmdBuffer
//!         Command buffer for current BB.
//! \param  [in] mosContext
//!         Reference to MOS_CONTEXT.
//! \param  [in] pMosResource
//!         Pointer to the MOS_RESOURCE.
//! \param  [in] offsetOfIndirectState
//!         Offset for current state in pMosResource.
//! \param  [in] bUseSizeOfResource
//!         If true, use size of pMosResource for indirect state, else use sizeOfIndirectState.
//! \param  [in] sizeOfIndirectState
//!         Size of indirect state. Ignore if bUseSizeOfResource == true.
//! \return void
//!         No return value. Handle all exception inside the function.
//!
void HalOcaInterface::OnIndirectState(MOS_COMMAND_BUFFER &cmdBuffer, MOS_CONTEXT &mosContext, void *pMosResource, uint32_t offsetOfIndirectState, bool bUseSizeOfResource, uint32_t sizeOfIndirectState)
{
    MosInterface::SetObjectCapture((PMOS_RESOURCE)pMosResource);
}

//!
//! \brief  Oca operation which should be called before adding dispatch states,
//!         e.g. VEB_DI_IECP_STATE and MEDIA_OBJECT_WALKER.
//! \param  [in] cmdBuffer
//!         Command buffer for current BB.
//! \param  [in] mosContext
//!         Reference to MOS_CONTEXT.
//! \param  [in] mhwMiInterface
//!         Reference to MhwMiInterface.
//! \param  [in] mmioRegisters
//!         mmio registers for current engine.
//! \return void
//!         No return value. Handle all exception inside the function.
//!
void HalOcaInterface::OnDispatch(MOS_COMMAND_BUFFER &cmdBuffer, MOS_CONTEXT &mosContext, MhwMiInterface &mhwMiInterface, MHW_MI_MMIOREGISTERS &mmioRegisters)
{
}

//!
//! \brief  Add string to oca log section
//! \param  [in] cmdBuffer
//!         Command buffer for current BB.
//! \param  [in] mosContext
//!         Reference to MOS_CONTEXT.
//! \param  [in] str
//!         string to be added.
//! \param  [in] maxCount
//!         size of the buffer pointed by str.
//! \return void
//!         No return value. Handle all exception inside the function.
//!
void HalOcaInterface::TraceMessage(MOS_COMMAND_BUFFER &cmdBuffer, MOS_CONTEXT &mosContext, const char *str, uint32_t maxCount)
{
    MosOcaInterface *pOcaInterface          = &MosOcaInterfaceSpecific::GetInstance();
    MOS_OCA_BUFFER_HANDLE   ocaBufHandle    = 0;
    MOS_STATUS              status          = MOS_STATUS_SUCCESS;
    
    if (nullptr == pOcaInterface || !((MosOcaInterfaceSpecific*)pOcaInterface)->IsOcaEnabled() || (ocaBufHandle = GetOcaBufferHandle(cmdBuffer, mosContext)) >= MAX_NUM_OF_OCA_BUF_CONTEXT)
    {
        return;
    }

    status = pOcaInterface->TraceMessage(ocaBufHandle, &mosContext, str, maxCount);
    if (MOS_FAILED(status))
    {
        OnOcaError(&mosContext, status, __FUNCTION__, __LINE__);
    }
}

//!
//! \brief  Add vp kernel info to oca log section.
//! \param  [in] cmdBuffer
//!         Command buffer for current BB.
//! \param  [in] osInterface
//!         Reference to MOS_INTERFACE.
//! \param  [in] res
//!         Reference to MOS_RESOURCE.
//! \param  [in] hwCmdType
//!         Hw command type.
//! \param  [in] locationInCmd
//!         Location in command.
//! \param  [in] offsetInRes
//!         Offset in resource.
//! \return void
//!         No return value. Handle all exception inside the function.
//!
void HalOcaInterface::DumpResourceInfo(MOS_COMMAND_BUFFER &cmdBuffer, MOS_INTERFACE &osInterface, MOS_RESOURCE &res, MOS_HW_COMMAND hwCmdType, uint32_t locationInCmd, uint32_t offsetInRes)
{
    if (nullptr == osInterface.pOsContext)
    {
        OnOcaError(nullptr, MOS_STATUS_INVALID_PARAMETER, __FUNCTION__, __LINE__);
        return;
    }

    MOS_CONTEXT &           mosContext      = *osInterface.pOsContext;
    MosOcaInterface         *pOcaInterface  = &MosOcaInterfaceSpecific::GetInstance();
    MOS_STATUS              status          = MOS_STATUS_SUCCESS;
    MOS_OCA_BUFFER_HANDLE   ocaBufHandle    = 0;

    if (nullptr == pOcaInterface || !((MosOcaInterfaceSpecific*)pOcaInterface)->IsOcaEnabled() || (ocaBufHandle = GetOcaBufferHandle(cmdBuffer, mosContext)) >= MAX_NUM_OF_OCA_BUF_CONTEXT)
    {
        return;
    }

    if (Mos_ResourceIsNull(&res))
    {
        OnOcaError(&mosContext, MOS_STATUS_INVALID_PARAMETER, __FUNCTION__, __LINE__);
        return;
    }

    status = pOcaInterface->AddResourceToDumpList(ocaBufHandle, &mosContext, res, hwCmdType, locationInCmd, offsetInRes);
    if (MOS_FAILED(status))
    {
        OnOcaError(&mosContext, status, __FUNCTION__, __LINE__);
    }
}

    //!
//! \brief  Trace OCA Sku Value.
//! \param  [in] cmdBuffer
//!         Command buffer for current BB.
//! \param  [in] osInterface
//!         Reference to MOS_INTERFACE.
//! \return void
//!         No return value. Handle all exception inside the function.
//!
void HalOcaInterface::TraceOcaSkuValue(MOS_COMMAND_BUFFER &cmdBuffer, MOS_INTERFACE &osInterface)
{
}

//!
//! \brief  Add vp kernel info to oca log section.
//! \param  [in] cmdBuffer
//!         Command buffer for current BB.
//! \param  [in] mosContext
//!         Reference to MOS_CONTEXT.
//! \param  [in] vpKernelID
//!         Value of enum VpKernelID.
//! \param  [in] fcKernelCount
//!         If vpKernelID == kernelCombinedFc, fcKernelCount is the kernel count for fc, otherwise, it's not used.
//! \param  [in] fcKernelList
//!         If vpKernelID == kernelCombinedFc, fcKernelList is the kernel list for fc, otherwise, it's not used.
//! \return void
//!         No return value. Handle all exception inside the function.
//!
void HalOcaInterface::DumpVpKernelInfo(MOS_COMMAND_BUFFER &cmdBuffer, MOS_CONTEXT &mosContext, int vpKernelID, int fcKernelCount, int *fcKernelList)
{
    MosOcaInterface *pOcaInterface          = &MosOcaInterfaceSpecific::GetInstance();
    MOS_STATUS              status          = MOS_STATUS_SUCCESS;
    MOS_OCA_BUFFER_HANDLE   ocaBufHandle    = 0;
    uint32_t                updateSize      = 0;

    if (nullptr == pOcaInterface || !((MosOcaInterfaceSpecific*)pOcaInterface)->IsOcaEnabled() || (ocaBufHandle = GetOcaBufferHandle(cmdBuffer, mosContext)) >= MAX_NUM_OF_OCA_BUF_CONTEXT)
    {
        return;
    }

    if (kernelCombinedFc == vpKernelID && (fcKernelCount <= 0 || nullptr == fcKernelList))
    {
        OnOcaError(&mosContext, MOS_STATUS_INVALID_PARAMETER, __FUNCTION__, __LINE__);
        return;
    }

    if (kernelCombinedFc != vpKernelID)
    {
        fcKernelCount = 0;
        fcKernelList = nullptr;
    }

    MOS_OCA_LOG_HEADER_VP_KERNEL_INFO header = {};
    header.header.m_Type                     = MOS_OCA_LOG_TYPE_VP_KERNEL_INFO;
    header.header.m_HeaderSize               = sizeof(MOS_OCA_LOG_HEADER_VP_KERNEL_INFO);
    header.header.m_DataSize                 = fcKernelCount * sizeof(int);
    header.vpKernelID                        = vpKernelID;
    header.fcKernelCount                     = fcKernelCount;
    status = pOcaInterface->DumpDataBlock(ocaBufHandle, &mosContext, (PMOS_OCA_LOG_HEADER)&header, fcKernelList);
    if (MOS_FAILED(status))
    {
        OnOcaError(&mosContext, status, __FUNCTION__, __LINE__);
    }
}

//!
//! \brief  Add vphal parameters to oca log section.
//! \param  [in] mosContext
//!         Reference to MOS_CONTEXT.
//! \param  [in] pVphalDumper
//!         Pointer to vphal dumper object.
//! \return void
//!         No return value. Handle all exception inside the function.
//!
void HalOcaInterface::DumpVphalParam(MOS_COMMAND_BUFFER &cmdBuffer, MOS_CONTEXT &mosContext, void *pVphalDumper)
{
    MosOcaInterface *pOcaInterface       = &MosOcaInterfaceSpecific::GetInstance();
    MOS_STATUS status                    = MOS_STATUS_SUCCESS;
    MOS_OCA_BUFFER_HANDLE ocaBufHandle   = 0;
    uint32_t updateSize                  = 0;

    if (nullptr == pOcaInterface || !((MosOcaInterfaceSpecific*)pOcaInterface)->IsOcaEnabled() || (ocaBufHandle = GetOcaBufferHandle(cmdBuffer, mosContext)) >= MAX_NUM_OF_OCA_BUF_CONTEXT ||
        nullptr == pVphalDumper)
    {
        return;
    }

    VPHAL_OCA_RENDER_PARAM *pVphalParam = ((VphalOcaDumper *)pVphalDumper)->GetRenderParam();
    if (nullptr == pVphalParam)
    {
        return;
    }

    MOS_OCA_LOG_HEADER_VPHAL_PARAM header = {};
    header.header.m_Type                  = MOS_OCA_LOG_TYPE_VPHAL_PARAM;                   // 00000003
    header.header.m_HeaderSize            = sizeof(MOS_OCA_LOG_HEADER_VPHAL_PARAM);
    header.header.m_DataSize              = pVphalParam->Header.size;
    status = pOcaInterface->DumpDataBlock(ocaBufHandle, &mosContext, (PMOS_OCA_LOG_HEADER)&header, pVphalParam);
    if (MOS_FAILED(status))
    {
        OnOcaError(&mosContext, status, __FUNCTION__, __LINE__);
    }
}

void HalOcaInterface::DumpCpParam(MosOcaInterface &ocaInterface, MOS_OCA_BUFFER_HANDLE &ocaBufHandle, PMOS_CONTEXT mosCtx, void *pCpDumper)
{
}

void HalOcaInterface::OnOcaError(PMOS_CONTEXT mosCtx, MOS_STATUS status, const char *functionName, uint32_t lineNumber)
{
    MosOcaInterfaceSpecific::OnOcaError(mosCtx, status, functionName, lineNumber);
}

MOS_OCA_BUFFER_HANDLE HalOcaInterface::GetOcaBufferHandle(MOS_COMMAND_BUFFER &cmdBuffer, MOS_CONTEXT &mosContext)
{
    MosOcaInterface *pOcaInterface       = &MosOcaInterfaceSpecific::GetInstance();
    if(pOcaInterface == nullptr)
    {
        OnOcaError(&mosContext, MOS_STATUS_NULL_POINTER, __FUNCTION__, __LINE__);
        return -1;
    }
    PMOS_MUTEX      mutex = pOcaInterface->GetMutex();
    if (mutex == nullptr)
    {
        OnOcaError(&mosContext, MOS_STATUS_INVALID_PARAMETER, __FUNCTION__, __LINE__);
        return -1;
    }
    MosOcaAutoLock autoLock(mutex);
    std::map<uint32_t*, MOS_OCA_BUFFER_HANDLE>::iterator it = s_hOcaMap.find(cmdBuffer.pCmdBase);
    return it == s_hOcaMap.end() ? 0 : it->second;
}

void HalOcaInterface::RemoveOcaBufferHandle(MOS_COMMAND_BUFFER &cmdBuffer, MOS_CONTEXT &mosContext)
{
    MosOcaInterface *pOcaInterface       = &MosOcaInterfaceSpecific::GetInstance();
    if(pOcaInterface == nullptr)
    {
        OnOcaError(&mosContext, MOS_STATUS_NULL_POINTER, __FUNCTION__, __LINE__);
        return;
    }
    PMOS_MUTEX      mutex = pOcaInterface->GetMutex();
    if (mutex == nullptr)
    {
        OnOcaError(&mosContext, MOS_STATUS_INVALID_PARAMETER, __FUNCTION__, __LINE__);
        return;
    }
    MosOcaAutoLock autoLock(mutex);
    std::map<uint32_t*, MOS_OCA_BUFFER_HANDLE>::iterator it = s_hOcaMap.find(cmdBuffer.pCmdBase);
    if (it == s_hOcaMap.end())
    {
        return;
    }
    s_hOcaMap.erase(it);
}
/****************************************************************************************************/
/*                         Private functions to ensure class singleton.                             */
/****************************************************************************************************/
HalOcaInterface::HalOcaInterface()
{
}

HalOcaInterface::HalOcaInterface(HalOcaInterface &)
{
}

HalOcaInterface& HalOcaInterface::operator= (HalOcaInterface &)
{
    return *this;
}
