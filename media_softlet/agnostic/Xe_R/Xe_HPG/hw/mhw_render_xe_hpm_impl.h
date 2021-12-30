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
//! \file     mhw_render_xe_hpg_impl.h
//! \brief    MHW render interface common base for Xe_HPG
//! \details
//!

#ifndef __MHW_RENDER_XE_HPG_IMPL_H__
#define __MHW_RENDER_XE_HPG_IMPL_H__

#include "mhw_render_impl.h"
#include "mhw_render_hwcmd_xe_hpg.h"
#include "mhw_render_itf.h"
#include "mhw_impl.h"
#include "mhw_mmio_g12.h"
#include "mhw_render.h"

namespace mhw
{
namespace render
{
namespace xe_hpg
{
class Impl : public render::Impl<mhw::render::xe_hpg::Cmd>
{
public:

    struct MHW_RENDER_ENGINE_L3_CACHE_SETTINGS : mhw::render::MHW_RENDER_ENGINE_L3_CACHE_SETTINGS
    {
        uint32_t   dwTcCntlReg = 0;
        uint32_t   dwAllocReg = 0;
        bool       bUpdateDefault = 0;
    };

    struct MHW_RENDER_ENGINE_L3_CACHE_CONFIG : mhw::render::MHW_RENDER_ENGINE_L3_CACHE_CONFIG
    {
        uint32_t   dwRcsL3CacheTcCntlReg_Register = 0;
        uint32_t   dwL3CacheTcCntlReg_Setting = 0;
        uint32_t   dwRcsL3CacheAllocReg_Register = 0;
        uint32_t   dwL3CacheAllocReg_Setting = 0;
        uint32_t   dwCcs0L3CacheTcCntlReg_Register = 0;
        uint32_t   dwCcs0L3CacheAllocReg_Register = 0;
    };

    struct MHW_VFE_PARAMS : mhw::render::MHW_VFE_PARAMS
    {
        bool  bFusedEuDispatch = 0;
        uint32_t numOfWalkers = 0;
        bool  enableSingleSliceDispatchCcsMode = 0;

        // Surface state offset of scratch space buffer.
        uint32_t scratchStateOffset = 0;
    };

protected:
    using base_t = render::Impl<mhw::render::xe_hpm::Cmd>;
    MHW_RENDER_ENGINE_L3_CACHE_SETTINGS   m_l3CacheConfig = {};
    uint32_t    m_l3CacheTcCntlRegisterValueDefault = 0;
    uint32_t    m_l3CacheAllocRegisterValueDefault = 0xD0000020;
public:
    Impl(PMOS_INTERFACE osItf) : base_t(osItf)
    {
        MHW_FUNCTION_ENTER;
        m_l3CacheTcCntlRegisterValueDefault = 0x80000080;

        InitMmioRegisters();
    };

    void InitMmioRegisters() override
    {
        MHW_FUNCTION_ENTER;
        MHW_MI_MMIOREGISTERS* mmioRegisters = &m_mmioRegisters;
        mmioRegisters->generalPurposeRegister0LoOffset  = CS_GENERAL_PURPOSE_REGISTER0_LO_OFFSET_G12;
        mmioRegisters->generalPurposeRegister0HiOffset  = CS_GENERAL_PURPOSE_REGISTER0_HI_OFFSET_G12;
        mmioRegisters->generalPurposeRegister4LoOffset  = CS_GENERAL_PURPOSE_REGISTER4_LO_OFFSET_G12;
        mmioRegisters->generalPurposeRegister4HiOffset  = CS_GENERAL_PURPOSE_REGISTER4_HI_OFFSET_G12;
        mmioRegisters->generalPurposeRegister11LoOffset = CS_GENERAL_PURPOSE_REGISTER11_LO_OFFSET_G12;
        mmioRegisters->generalPurposeRegister11HiOffset = CS_GENERAL_PURPOSE_REGISTER11_HI_OFFSET_G12;
        mmioRegisters->generalPurposeRegister12LoOffset = CS_GENERAL_PURPOSE_REGISTER12_LO_OFFSET_G12;
        mmioRegisters->generalPurposeRegister12HiOffset = CS_GENERAL_PURPOSE_REGISTER12_HI_OFFSET_G12;
    }

    MOS_STATUS EnableL3Caching(mhw::render::MHW_RENDER_ENGINE_L3_CACHE_SETTINGS *cacheSettings) override
    {
        // L3 Caching enabled by default
        m_l3CacheConfig.bL3CachingEnabled               = true;
        m_l3CacheConfig.dwRcsL3CacheAllocReg_Register   = M_MMIO_RCS_L3ALLOCREG;
        m_l3CacheConfig.dwRcsL3CacheTcCntlReg_Register  = M_MMIO_RCS_TCCNTLREG;
        m_l3CacheConfig.dwCcs0L3CacheAllocReg_Register  = M_MMIO_CCS0_L3ALLOCREG;
        m_l3CacheConfig.dwCcs0L3CacheTcCntlReg_Register = M_MMIO_CCS0_TCCNTLREG;
        if (cacheSettings)
        {
            MHW_RENDER_ENGINE_L3_CACHE_SETTINGS *cacheSettingsHpg = (MHW_RENDER_ENGINE_L3_CACHE_SETTINGS*)cacheSettings;
            if (cacheSettingsHpg == nullptr)
            {
                MHW_ASSERTMESSAGE("Hpg-Specific Params are needed.");
                return MOS_STATUS_INVALID_PARAMETER;
            }
            m_l3CacheConfig.dwL3CacheAllocReg_Setting  = cacheSettingsHpg->dwAllocReg;
            m_l3CacheConfig.dwL3CacheTcCntlReg_Setting = cacheSettingsHpg->dwTcCntlReg;
            // update default settings is needed from CM HAL call
            if (cacheSettingsHpg->bUpdateDefault)
            {
                m_l3CacheAllocRegisterValueDefault  = cacheSettingsHpg->dwAllocReg;
                m_l3CacheTcCntlRegisterValueDefault = cacheSettingsHpg->dwTcCntlReg;
            }
        }
        else // Use the default setting if regkey is not set
        {
            // different default settings after CM HAL call
            m_l3CacheConfig.dwL3CacheAllocReg_Setting  = m_l3CacheAllocRegisterValueDefault;
            m_l3CacheConfig.dwL3CacheTcCntlReg_Setting = m_l3CacheTcCntlRegisterValueDefault;
        }

        return MOS_STATUS_SUCCESS;
    }

    MOS_STATUS SetL3Cache(PMOS_COMMAND_BUFFER cmdBuffer, MhwMiInterface* pMhwMiInterface) override
    {
        MOS_STATUS eStatus = MOS_STATUS_SUCCESS;

        MHW_MI_CHK_NULL(cmdBuffer);

        if (m_l3CacheConfig.bL3CachingEnabled)
        {
            MHW_MI_LOAD_REGISTER_IMM_PARAMS loadRegisterParams;

            //L3CacheAllocReg_Setting and L3CacheTcCntlReg_Setting
            if ((m_l3CacheConfig.dwL3CacheAllocReg_Setting != 0) || (m_l3CacheConfig.dwL3CacheTcCntlReg_Setting != 0))
            {
                //update L3 AllocReg setting for RCS; CCS L3 AllocReg setting will be dulicated from RCS
                MOS_ZeroMemory(&loadRegisterParams, sizeof(loadRegisterParams));
                loadRegisterParams.dwRegister = m_l3CacheConfig.dwRcsL3CacheAllocReg_Register;
                loadRegisterParams.dwData     = m_l3CacheConfig.dwL3CacheAllocReg_Setting;
                MHW_MI_CHK_STATUS(pMhwMiInterface->AddMiLoadRegisterImmCmd(cmdBuffer, &loadRegisterParams));

                //update L3 TcCntlReg setting for RCS; CCS L3 TcCntlReg setting will be dulicated from RCS
                MOS_ZeroMemory(&loadRegisterParams, sizeof(loadRegisterParams));
                loadRegisterParams.dwRegister = m_l3CacheConfig.dwRcsL3CacheTcCntlReg_Register;
                loadRegisterParams.dwData     = m_l3CacheConfig.dwL3CacheTcCntlReg_Setting;
                MHW_MI_CHK_STATUS(pMhwMiInterface->AddMiLoadRegisterImmCmd(cmdBuffer, &loadRegisterParams));
            }
        }

        return eStatus;
    }

    _MHW_SETCMD_OVERRIDE_DECL(_3DSTATE_CHROMA_KEY)
    {
        _MHW_SETCMD_CALLBASE(_3DSTATE_CHROMA_KEY);

        cmd.DW1.ChromakeyTableIndex = params.dwIndex;
        cmd.DW2.ChromakeyLowValue   = params.dwLow;
        cmd.DW3.ChromakeyHighValue  = params.dwHigh;

        return MOS_STATUS_SUCCESS;
    }

    _MHW_SETCMD_OVERRIDE_DECL(PIPELINE_SELECT)
    {
        _MHW_SETCMD_CALLBASE(PIPELINE_SELECT);
        cmd.DW0.MaskBits = 0x13;
        return MOS_STATUS_SUCCESS;
    }

    _MHW_SETCMD_OVERRIDE_DECL(STATE_COMPUTE_MODE)
    {
        _MHW_SETCMD_CALLBASE(STATE_COMPUTE_MODE);
        cmd.DW1.MaskBits         = 0xFFFF;
        cmd.DW1.LargeGrfMode     = 0;
        cmd.DW1.ForceNonCoherent = 2;

        return MOS_STATUS_SUCCESS;
    }

    _MHW_SETCMD_OVERRIDE_DECL(CFE_STATE)
    {
        _MHW_SETCMD_CALLBASE(CFE_STATE);

        cmd.DW3.MaximumNumberOfThreads     = params.dwMaximumNumberofThreads;
        cmd.DW1_2.ScratchSpaceBuffer       = params.ScratchSpaceBuffer;
        cmd.DW3.FusedEuDispatch            = params.FusedEuDispatch;  // disabled if DW3.FusedEuDispath = 1
        cmd.DW3.NumberOfWalkers            = params.NumberOfWalkers;
        cmd.DW3.SingleSliceDispatchCcsMode = params.SingleSliceDispatchCcsMode;

        return MOS_STATUS_SUCCESS;
    }

    _MHW_SETCMD_OVERRIDE_DECL(COMPUTE_WALKER)
    {
        _MHW_SETCMD_CALLBASE(COMPUTE_WALKER);

        cmd.DW2.IndirectDataLength = params.IndirectDataLength;
        cmd.DW3.IndirectDataStartAddress = params.IndirectDataStartAddress >> MHW_COMPUTE_INDIRECT_SHIFT;

        cmd.DW4.SIMDSize = 2;

        cmd.DW5.ExecutionMask = 0xffffffff;
        cmd.DW6.LocalXMaximum = params.ThreadWidth - 1;
        cmd.DW6.LocalYMaximum = params.ThreadHeight - 1;
        cmd.DW6.LocalZMaximum = params.ThreadDepth - 1;

        cmd.DW7.ThreadGroupIDXDimension = params.GroupWidth - params.GroupStartingX;
        cmd.DW8.ThreadGroupIDYDimension = params.GroupHeight - params.GroupStartingY;
        cmd.DW9.ThreadGroupIDZDimension = params.GroupDepth;
        cmd.DW10.ThreadGroupIDStartingX = params.GroupStartingX;
        cmd.DW11.ThreadGroupIDStartingY = params.GroupStartingY;
        cmd.DW12.ThreadGroupIDStartingZ = params.GroupStartingZ;

        cmd.interface_descriptor_data.DW0_1.KernelStartPointer = params.dwKernelOffset >> MHW_KERNEL_OFFSET_SHIFT;
        cmd.interface_descriptor_data.DW3.SamplerCount = params.dwSamplerCount;
        cmd.interface_descriptor_data.DW3.SamplerStatePointer = params.dwSamplerOffset >> MHW_SAMPLER_SHIFT;
        cmd.interface_descriptor_data.DW4.BindingTablePointer = MOS_ROUNDUP_SHIFT(params.dwBindingTableOffset, MHW_BINDING_TABLE_ID_SHIFT);
        cmd.interface_descriptor_data.DW5.BarrierEnable = params.bBarrierEnable;
        cmd.interface_descriptor_data.DW5.NumberOfThreadsInGpgpuThreadGroup = params.dwNumberofThreadsInGPGPUGroup;
        cmd.interface_descriptor_data.DW5.SharedLocalMemorySize = params.dwSharedLocalMemorySize;

        return MOS_STATUS_SUCCESS;
    }
};

}  // namespace xe_hpg
}  // namespace render
}  // namespace mhw

#endif  // __MHW_RENDER_XE_HPG_IMPL_H__
