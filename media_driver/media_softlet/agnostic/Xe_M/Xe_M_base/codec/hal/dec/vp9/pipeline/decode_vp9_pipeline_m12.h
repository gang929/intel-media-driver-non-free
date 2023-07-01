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
//! \file     decode_vp9_pipeline_g12.h
//! \brief    Defines the interface for vp9 decode pipeline
//!
#ifndef __DECODE_VP9_PIPELINE_M12_H__
#define __DECODE_VP9_PIPELINE_M12_H__

#include "decode_vp9_pipeline.h"
#include "codec_def_decode_vp9.h"
#include "decode_huc_packet_creator_g12.h"
#include "decode_huc_prob_update_creator_m12.h"

namespace decode
{

class Vp9PipelineG12 : public Vp9Pipeline, public HucProbUpdatePacketCreatorM12
{
public:
    //!
    //! \brief  DecodePipeline constructor
    //! \param  [in] hwInterface
    //!         Pointer to CodechalHwInterface
    //! \param  [in] debugInterface
    //!         Pointer to CodechalDebugInterface
    //!
    Vp9PipelineG12(
        CodechalHwInterface *   hwInterface,
        CodechalDebugInterface *debugInterface);

    virtual ~Vp9PipelineG12(){};

    virtual MOS_STATUS Init(void *settings) override;

    //!
    //! \brief  Prepare interal parameters, should be invoked for each frame
    //! \param  [in] params
    //!         Pointer to the input parameters
    //! \return MOS_STATUS
    //!         MOS_STATUS_SUCCESS if success, else fail reason
    //!
    virtual MOS_STATUS Prepare(void *params) final;

    //!
    //! \brief  Finish the execution for each frame
    //! \return MOS_STATUS
    //!         MOS_STATUS_SUCCESS if success, else fail reason
    //!
    virtual MOS_STATUS Execute() final;

    virtual MOS_STATUS GetStatusReport(void *status, uint16_t numStatus) override;

    uint32_t GetCompletedReport();

    virtual MOS_STATUS Destroy() override;

    //!
    //! \brief  Create post sub packets
    //! \param  [in] subPipelineManager
    //! \return MOS_STATUS
    //!         MOS_STATUS_SUCCESS if success, else fail reason
    //!
    virtual MOS_STATUS CreatePostSubPipeLines(DecodeSubPipelineManager &subPipelineManager) override;

    //!
    //! \brief  Create pre sub packets
    //! \param  [in] subPipelineManager
    //! \return MOS_STATUS
    //!         MOS_STATUS_SUCCESS if success, else fail reason
    //!
    virtual MOS_STATUS CreatePreSubPipeLines(DecodeSubPipelineManager &subPipelineManager) override;


protected:
    virtual MOS_STATUS Initialize(void *settings) override;
    virtual MOS_STATUS Uninitialize() override;

    //!
    //! \brief  User Feature Key Report
    //! \return MOS_STATUS
    //!         MOS_STATUS_SUCCESS if success, else fail reason
    //!
    virtual MOS_STATUS UserFeatureReport() override;

    //!
    //! \brief  Create sub packets
    //! \param  [in] codecSettings
    //!         Point to codechal settings
    //! \return MOS_STATUS
    //!         MOS_STATUS_SUCCESS if success, else fail reason
    //!
    virtual MOS_STATUS CreateSubPackets(DecodeSubPacketManager &subPacketManager, CodechalSetting &codecSettings) override;

    //!
    //! \brief    Initialize MMC state
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success
    //!
    virtual MOS_STATUS InitMmcState();

    virtual MOS_STATUS CreateFeatureManager() override;

    virtual MOS_STATUS InitContexOption(Vp9BasicFeature &basicFeature) override;

#if USE_CODECHAL_DEBUG_TOOL
    //! \brief    Dump the parameters
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS DumpParams(Vp9BasicFeature &basicFeature);
#endif

    CodechalHwInterface *m_hwInterface = nullptr;
MEDIA_CLASS_DEFINE_END(decode__Vp9PipelineG12)
};
}  // namespace decode
#endif  // !__DECODE_VP9_PIPELINE_M12_H__
