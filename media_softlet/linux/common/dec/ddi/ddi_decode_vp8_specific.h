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
//! \file     ddi_decode_vp8_specific.h
//! \brief    Defines class for DDI media VP8 decode
//!

#ifndef __DDI_DECODE_VP8_SPECIFIC_H__
#define __DDI_DECODE_VP8_SPECIFIC_H__

#include "ddi_decode_base_specific.h"

//namespace decode
//{
struct _DDI_MEDIA_BUFFER;

//!
//! \class  DdiDecodeVp8
//! \brief  Ddi decode VP8
//!
class DdiDecodeVp8 : public DdiDecodeBase {
public:
    //!
    //! \brief Constructor
    //!
    DdiDecodeVp8() : DdiDecodeBase() {m_withDpb = false;};

    //!
    //! \brief Destructor
    //!
    virtual ~DdiDecodeVp8(){};

    // inherited virtual function
    virtual void DestroyContext (
        VADriverContextP ctx) override;

    virtual VAStatus RenderPicture (
        VADriverContextP ctx,
        VAContextID      context,
        VABufferID       *buffers,
        int32_t          numBuffers) override;

    virtual VAStatus SetDecodeParams() override;

    virtual void ContextInit(
        int32_t picWidth,
        int32_t picHeight) override;

    virtual VAStatus CodecHalInit(
        DDI_MEDIA_CONTEXT *mediaCtx,
        void              *ptr) override;

    virtual VAStatus AllocSliceControlBuffer(
        DDI_MEDIA_BUFFER *buf) override;

    virtual uint8_t* GetPicParamBuf(
        DDI_CODEC_COM_BUFFER_MGR *bufMgr) override;

private:
    //!
    //! \brief   ParseIQMatrixParam for VP8
    //! \details parse the QMatrix info required by VP8 decoding
    //!
    //! \param   [in] *mediaCtx
    //!          DDI_MEDIA_CONTEXT
    //! \param   [in] *qMatrix
    //!          VAIQMatrixBufferHEVC
    //!
    //! \return  VA_STATUS_SUCCESS is returned if it is parsed successfully.
    //!          else fail reason
    VAStatus ParseIQMatrix(
        DDI_MEDIA_CONTEXT   *mediaCtx,
        VAIQMatrixBufferVP8 *matrix);

    //!
    //! \brief   ParseSliceParam for VP8
    //! \details parse the sliceParam info required by VP8 decoding for
    //!          each slice
    //!
    //! \param   [in] *mediaCtx
    //!          DDI_MEDIA_CONTEXT
    //! \param   [in] *slcParam
    //!          VASliceParameterBufferVP8
    //!
    //! \return  VA_STATUS_SUCCESS is returned if it is parsed successfully.
    //!          else fail reason
    VAStatus ParseSliceParams(
        DDI_MEDIA_CONTEXT         *mediaCtx,
        VASliceParameterBufferVP8 *slcParam);

    //! \brief   ParsePicParam for VP8
    //! \details parse the PicParam info required by VP8 decoding
    //!
    //! \param   [in] *mediaCtx
    //!          DDI_MEDIA_CONTEXT
    //! \param   [in] *qMatrix
    //!          VAPictureParameterBufferVP8
    //!
    //! \return  VA_STATUS_SUCCESS is returned if it is parsed successfully.
    //!          else fail reason
    VAStatus ParsePicParams(
        DDI_MEDIA_CONTEXT           *mediaCtx,
        VAPictureParameterBufferVP8 *picParam);

    //! \brief   ParseProbabilityData for VP8
    //! \details parse the VP8 Prob table required by VP8 decoding
    //!
    //! \param   [in] *vp8ProbDataBuffer
    //!          struct _DDI_MEDIA_BUFFER
    //! \param   [in] *probBuffer
    //!          VAProbabilityDataBufferVP8
    //!
    //! \return  VA_STATUS_SUCCESS is returned if it is parsed successfully.
    //!          else fail reason
    VAStatus ParseProbabilityData(
        struct _DDI_MEDIA_BUFFER   *vp8ProbDataBuff,
        VAProbabilityDataBufferVP8 *probInputBuf);

    //! \brief   Init Resource buffer for VP8
    //! \details Initialize and allocate the Resource buffer for VP8
    //!
    //! \return  VA_STATUS_SUCCESS is returned if it is parsed successfully.
    //!          else fail reason
    VAStatus InitResourceBuffer(DDI_MEDIA_CONTEXT *mediaCtx);

    //! \brief   Free Resource buffer for VP8
    //!
    void FreeResourceBuffer();

    void FreeResource();

    //!
    //! \brief    Check if the resolution is valid for a given decode codec mode
    //!
    //! \param    [in] codecMode
    //!           Specify the codec mode
    //!
    //! \param    [in] profile
    //!           VA profile
    //!
    //! \param    [in] width
    //!           Specify the width for checking
    //!
    //! \param    [in] height
    //!           Specify the height for checking
    //!
    //! \return   VAStatus
    //!           VA_STATUS_SUCCESS if the resolution is supported
    //!           VA_STATUS_ERROR_RESOLUTION_NOT_SUPPORTED if the resolution isn't valid
    //!
    VAStatus CheckDecodeResolution(
            int32_t   codecMode,
            VAProfile profile,
            uint32_t  width,
            uint32_t  height) override;

    //!
    //! \brief    Return internal decode mode for given profile
    //!
    //! \param    [in] profile
    //!           Specify the VAProfile
    //!
    //! \return   Codehal mode: decode codec mode
    //!
    CODECHAL_MODE GetDecodeCodecMode(VAProfile profile) override;

    MOS_RESOURCE m_resNoneRegLastRefFrame;
    MOS_RESOURCE m_resNoneRegGoldenRefFrame;
    MOS_RESOURCE m_resNoneRegAltRefFrame;

    static const uint32_t m_decVp8MaxWidth  = 4096; //!< Maximum width for Vp8 decode
    static const uint32_t m_decVp8MaxHeight = 4096; //!< Maximum height for Vp8 decode

    MEDIA_CLASS_DEFINE_END(DdiDecodeVp8)
};
// } namespace decode

#endif /* __DDI_DECODE_VP8_SPECIFIC_H__ */
