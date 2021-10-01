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
//! \file     mos_oca_interface_specific.h
//! \brief    Linux specfic OCA interface class
//!

#ifndef __MOS_OCA_INTERFACE_SPECIFIC_H__
#define __MOS_OCA_INTERFACE_SPECIFIC_H__

#include "mos_oca_interface.h"
#include "mos_interface.h"

#define MOS_OCA_INVALID_BUFFER_HANDLE -1

typedef enum _MOS_OCA_LOG_TYPE
{
    MOS_OCA_LOG_TYPE_INVALID = 0,
    MOS_OCA_LOG_TYPE_STRING,
    MOS_OCA_LOG_TYPE_VP_KERNEL_INFO,
    MOS_OCA_LOG_TYPE_VPHAL_PARAM,
    MOS_OCA_LOG_TYPE_CP_PARAM,
    MOS_OCA_LOG_TYPE_RESOURCE_INFO,
    MOS_OCA_LOG_TYPE_FENCE_INFO,
    MOS_OCA_LOG_TYPE_COUNT
}MOS_OCA_LOG_TYPE;

typedef struct _MOS_OCA_BUFFER_CONFIG
{
    uint32_t            maxResInfoCount = 0;                                                                //!< Max resource info count.
}MOS_OCA_BUFFER_CONFIG;

/****************************************************************************************************/
/*                                      OCA LOG HEADERS                                             */
/****************************************************************************************************/
typedef struct _MOS_OCA_LOG_HEADER_VP_KERNEL_INFO
{
    MOS_OCA_LOG_HEADER          header;
    int                         vpKernelID;
    int                         fcKernelCount;
}MOS_OCA_LOG_HEADER_VP_KERNEL_INFO, *PMOS_OCA_LOG_HEADER_VP_KERNEL_INFO;

typedef struct _MOS_OCA_LOG_HEADER_VPHAL_PARAM
{
    MOS_OCA_LOG_HEADER          header;
}MOS_OCA_LOG_HEADER_VPHAL_PARAM, *PMOS_OCA_LOG_HEADER_VPHAL_PARAM;

typedef struct _MOS_OCA_RESOURCE_INFO
{
    uint64_t                    sizeAllocation;
    uint64_t                    sizeSurface;
    uint64_t                    sizeSurfacePhy;
    uint64_t                    sizeMainSurface;
    uint64_t                    allocationHandle;
    uint32_t                    hwCmdType;
    uint32_t                    locationInCmd;
    uint32_t                    offsetInRes;
    uint32_t                    pitch;
    uint32_t                    width;
    uint32_t                    height;
    uint32_t                    format;
    uint32_t                    gmmFormat;
    uint32_t                    gmmTileMode;
    uint32_t                    gmmClient;
    uint32_t                    gmmResUsageType;
    uint32_t                    mmcMode;
    uint32_t                    mmcHint;
    uint64_t                    auxYOffset;
    uint64_t                    auxUVOffset;
    uint64_t                    auxCCSOffset;
    uint64_t                    auxCCOffset;
    struct {
        uint32_t                isLocalOnly         : 1;
        uint32_t                isNonLocalOnly      : 1;
        uint32_t                isNotLockable       : 1;
        uint32_t                isShared            : 1;
        uint32_t                isCameraCapture     : 1;
        uint32_t                isRenderTarget      : 1;
    } flags;
}MOS_OCA_RESOURCE_INFO, *PMOS_OCA_RESOURCE_INFO;

typedef struct _MOS_OCA_LOG_HEADER_RESOURCE_INFO
{
    MOS_OCA_LOG_HEADER          header;
    uint32_t                    resCount;         // Resource count dumped.
    uint32_t                    resCountSkipped;  // Resource count skiped to be dumped as total count exceeding MOS_OCA_MAX_RESOURCE_INFO_COUNT.
    // Followed by MOS_OCA_RESOURCE_INFO lists.
}MOS_OCA_LOG_HEADER_RESOURCE_INFO, *PMOS_OCA_LOG_HEADER_RESOURCE_INFO;

struct MOS_OCA_BUF_CONTEXT
{
    bool                                inUse                 = false;
    bool                                is1stLevelBBStarted   = false;
    struct
    {
        uint64_t                        *base                 = nullptr;
        uint32_t                        offset                = 0;
        struct {
            uint32_t                    maxResInfoCount       = 0;
            MOS_OCA_RESOURCE_INFO       *resInfoList          = nullptr;
            uint32_t                    resCount              = 0;
            uint32_t                    resCountSkipped       = 0;
        } resInfo;   
    } logSection;
           
};

#define MAX_NUM_OF_OCA_BUF_CONTEXT 32

class MosOcaAutoLock
{
public:
    MosOcaAutoLock(PMOS_MUTEX pMutex) : m_pMutex(pMutex)
    {
        if (m_pMutex)
        {
            MOS_LockMutex(m_pMutex);
        }
        else
        {
            MOS_OS_ASSERTMESSAGE("Unexpected, m_pMutex cannot be null");
        }
    }
    virtual ~MosOcaAutoLock()
    {
        if (m_pMutex)
        {
            MOS_UnlockMutex(m_pMutex);
        }
        else
        {
            MOS_OS_ASSERTMESSAGE("Unexpected, m_pMutex cannot be null");
        }
    }
private:
    PMOS_MUTEX m_pMutex = nullptr;
};

class MosOcaInterfaceSpecific : public MosOcaInterface
{
public:
    //!
    //! \brief  Destructor
    //!
    virtual ~MosOcaInterfaceSpecific();

    //!
    //! \brief  Get the idle oca buffer, which is neither used by hw nor locked, and lock it for edit.
    //! \param  [in] pMosContext
    //!         pointer to MOS_INTERFACE
    //! \param  [in] CurrentGpuContextHandle
    //!         Gpu context handle 
    //! \return MOS_OCA_BUFFER_HANDLE
    //!         return the handle for oca buffer
    //!
    virtual MOS_OCA_BUFFER_HANDLE LockOcaBufAvailable(PMOS_CONTEXT pMosContext, uint32_t CurrentGpuContextHandle);

    //!
    //! \brief  Unlock the oca buffer when edit complete.
    //! \param  [in] ocaBufHandle
    //!         Oca buffer handle.
    //! \return MOS_STATUS
    //!         Return MOS_STATUS_SUCCESS if successful, otherwise failed
    //!
    virtual MOS_STATUS UnlockOcaBuf(MOS_OCA_BUFFER_HANDLE ocaBufHandle);

    //!
    //! \brief  Oca operation which should be called at the beginning of 1st level batch buffer start.
    //! \param  [out] gpuVaOcaBuffer
    //!         The gfx virtual address of oca buffer, which should be set to GPR11 by LRI at the
    //!         beginning of 1st level batch buffer no matter return value is MOS_STATUS_SUCCESS or not.
    //! \param  [in] ocaBufHandle
    //!         Oca buffer handle.
    //! \param  [in] pMosContext
    //!         Pointer to MOS_CONTEXT.
    //! \param  [in] pMosResource
    //!         Pointer to the MOS_RESOURCE.
    //! \param  [in] offsetOf1stLevelBB
    //!         Offset for current BB in pMosResource.
    //! \param  [in] bUseSizeOfResource
    //!         If true, use size of pMosResource for batch buffer, else use sizeOf1stLevelBB.
    //! \param  [in] sizeOf1stLevelBB
    //!         Size of BB. Ignore if bUseSizeOfResource == true.
    //! \return MOS_STATUS
    //!         Return MOS_STATUS_SUCCESS if successful, otherwise failed
    //!
    virtual MOS_STATUS On1stLevelBBStart(uint64_t &gpuVaOcaBuffer, MOS_OCA_BUFFER_HANDLE ocaBufHandle, PMOS_CONTEXT pMosContext, void *pMosResource,
        uint32_t offsetOf1stLevelBB, bool bUseSizeOfResource, uint32_t sizeOf1stLevelBB);

    //!
    //! \brief  Oca operation which should be called before adding batch buffer end command for 1st
    //!         level batch buffer.
    //! \param  [in] ocaBufHandle
    //!         Oca buffer handle.
    //! \param  [in] mosCtx
    //!         DDI device context.
    //! \return MOS_STATUS
    //!         Return MOS_STATUS_SUCCESS if successful, otherwise failed
    //!
    virtual MOS_STATUS On1stLevelBBEnd(MOS_OCA_BUFFER_HANDLE ocaBufHandle, PMOS_CONTEXT mosCtx);

    //!
    //! \brief  Oca operation which should be called before sending start sub level batch buffer command.
    //! \param  [in] ocaBufHandle
    //!         Oca buffer handle.
    //! \param  [in] pMosContext
    //!         Pointer to MOS_CONTEXT.
    //! \param  [in] pMosResource
    //!         Pointer to the MOS_RESOURCE.
    //! \param  [in] offsetOfSubLevelBB
    //!         Offset for current BB in pMosResource.
    //! \param  [in] bUseSizeOfResource
    //!         If true, use size of pMosResource for batch buffer, else use sizeOfIndirectState.
    //! \param  [in] sizeOfSubLevelBB
    //!         Size of BB. Ignore if bUseSizeOfResource == true.
    //! \return MOS_STATUS
    //!         Return MOS_STATUS_SUCCESS if successful, otherwise failed
    //!
    virtual MOS_STATUS OnSubLevelBBStart(MOS_OCA_BUFFER_HANDLE ocaBufHandle, PMOS_CONTEXT pMosContext, void *pMosResource, uint32_t offsetOfSubLevelBB, bool bUseSizeOfResource, uint32_t sizeOfSubLevelBB);

    //!
    //! \brief  Oca operation which should be called when indirect states being added.
    //! \param  [in] ocaBufHandle
    //!         Oca buffer handle.
    //! \param  [in] pMosContext
    //!         Pointer to MOS_CONTEXT.
    //! \param  [in] pMosResource
    //!         Pointer to the MOS_RESOURCE.
    //! \param  [in] offsetOfIndirectState
    //!         Offset for current state in pMosResource.
    //! \param  [in] bUseSizeOfResource
    //!         If true, use size of pMosResource for indirect state, else use sizeOfIndirectState.
    //! \param  [in] sizeOfIndirectState
    //!         Size of indirect state. Ignore if bUseSizeOfResource == true.
    //! \return MOS_STATUS
    //!         Return MOS_STATUS_SUCCESS if successful, otherwise failed
    //!
    virtual MOS_STATUS OnIndirectState(MOS_OCA_BUFFER_HANDLE ocaBufHandle, PMOS_CONTEXT pMosContext, void *pMosResource, uint32_t offsetOfIndirectState, bool bUseSizeOfResource, uint32_t sizeOfIndirectState);

    //!
    //! \brief  Oca operation which should be called before adding dispatch states,
    //!         e.g. VEB_DI_IECP_STATE and MEDIA_OBJECT_WALKER.
    //! \param  [out] offsetInIndirectStateHeap
    //!         The start offset of current dispatch in indirect state heap, which should be set to low 32 bits
    //!         of GPR12 by LRI before dispatch commands being added.
    //!         OCA_HEAP_INVALID_OFFSET means no need to configure GPR12, otherwise the register need be configured
    //!         no matter return value being MOS_STATUS_SUCCESS or not.
    //! \param  [in] ocaBufHandle
    //!         Oca buffer handle.
    //! \param  [in] mosCtx
    //!         DDI device context.
    //! \return MOS_STATUS
    //!         Return MOS_STATUS_SUCCESS if successful, otherwise failed
    //!
    virtual MOS_STATUS OnDispatch(uint32_t &offsetInIndirectStateHeap, MOS_OCA_BUFFER_HANDLE ocaBufHandle, PMOS_CONTEXT mosCtx);

    //!
    //! \brief  Add string to oca log section
    //! \param  [in] ocaBufHandle
    //!         Oca buffer handle.
    //! \param  [in] mosCtx
    //!         DDI device context.
    //! \param  [in] str
    //!         string to be added.
    //! \param  [in] maxCount
    //!         size of the buffer pointed by str.
    //! \return MOS_STATUS
    //!         Return MOS_STATUS_SUCCESS if successful, otherwise failed
    //!
    virtual MOS_STATUS TraceMessage(MOS_OCA_BUFFER_HANDLE ocaBufHandle, PMOS_CONTEXT mosCtx, const char *str, uint32_t maxCount);

    //!
    //! \brief  Add resource to dump list.
    //! \param  [in] ocaBufHandle
    //!         Oca buffer handle.
    //! \param  [in] mosCtx
    //!         DDI device context.
    //! \param  [in] resource
    //!         Reference to MOS_RESOURCE.
    //! \param  [in] hwCmdType
    //!         Hw command Type.
    //! \param  [in] locationInCmd
    //!         Location in command.
    //! \param  [in] offsetInRes
    //!         Offset in resource.
    //! \return MOS_STATUS
    //!         Return MOS_STATUS_SUCCESS if successful, otherwise failed
    //!
    virtual MOS_STATUS AddResourceToDumpList(MOS_OCA_BUFFER_HANDLE ocaBufHandle, PMOS_CONTEXT mosCtx, MOS_RESOURCE &resource, MOS_HW_COMMAND hwCmdType, uint32_t locationInCmd, uint32_t offsetInRes);

    //!
    //! \brief  Add data block to oca log section.
    //! \param  [in] ocaBufHandle
    //!         Oca buffer handle.
    //! \param  [in] mosCtx
    //!         DDI device context.
    //! \param  [in] pHeader
    //!         Log header. It can be extended by user. The acutal size of header is pHeader->m_HeaderSize.
    //! \param  [in] pData
    //!         Data block without log header. The acutal size of data block is pHeader->m_DataSize.
    //! \return MOS_STATUS
    //!         Return MOS_STATUS_SUCCESS if successful, otherwise failed
    //!
    virtual MOS_STATUS DumpDataBlock(MOS_OCA_BUFFER_HANDLE ocaBufHandle, PMOS_CONTEXT mosCtx, PMOS_OCA_LOG_HEADER pHeader, void *pData);

    //!
    //! \brief  Get the mutex for oca buffer handles protection.
    //! \return PMOS_MUTEX
    //!         Return PMOS_MUTEX if successful, otherwise nullptr
    //!
    virtual PMOS_MUTEX GetMutex()
    {
        return m_ocaMutex;
    }

    //!
    //! \brief  Oca Interface Initialize.
    //!
    static void InitInterface();

    //!
    //! \brief  Oca Interface uninitialize.
    //!
    static void UninitInterface();

    //!
    //! \brief  Initialize oca error handler related items.
    //!
    static void InitOcaErrorHandler(MOS_CONTEXT *mosCtx);

    //!
    //! \brief  Oca Error Handler.
    //! \param  [in] mosCtx
    //!         ddi device context.
    //! \param  [in] status
    //!         status for error.
    //! \param  [in] functionName
    //!         The failure function name.
    //! \param  [in] lineNumber
    //!         The line number where the failure happened.
    //!
    static void OnOcaError(PMOS_CONTEXT mosCtx, MOS_STATUS status, const char *functionName, uint32_t lineNumber);

    //!
    //! \brief  Initialize oca error handler related items.
    //!
    static void InitOcaErrorHandler();

    bool IsOcaEnabled()
    {
         return m_isOcaEnabled;
    }

    static MosOcaInterface& GetInstance();
    
    static uint32_t IncreaseSize(uint32_t cmdBufSize);

    static void InitOcaLogSection(MOS_LINUX_BO *bo);

private:
    //!
    //! \brief  Oca Interface Initialize.
    //!
    void Initialize();

    virtual MOS_STATUS InsertData(MOS_OCA_BUFFER_HANDLE ocaBufHandle, uint8_t *p, uint32_t size);

    MOS_STATUS DumpDataBlock(MOS_OCA_BUFFER_HANDLE ocaBufHandle, PMOS_OCA_LOG_HEADER pHeader, void *pData);

    void AddResourceInfoToLogSection(MOS_OCA_BUFFER_HANDLE ocaBufHandle, PMOS_CONTEXT mosCtx);

    void InitLogSection(MOS_OCA_BUFFER_HANDLE ocaBufHandle, PMOS_RESOURCE resCmdBuf);

    bool IsLogSectionEnabled(MOS_OCA_BUFFER_HANDLE ocaBufHandle);

    //!
    //! \brief  Oca Interface uninitialize.
    //!
    void Uninitialize();

    MosOcaInterfaceSpecific();
    MosOcaInterfaceSpecific(MosOcaInterfaceSpecific &);
    MosOcaInterfaceSpecific& operator= (MosOcaInterfaceSpecific &);

    PMOS_MUTEX                      m_ocaMutex                                      = nullptr;

    bool                            m_isOcaEnabled                                  = true;
    bool                            m_isInitialized                                 = false;
    MOS_OCA_RESOURCE_INFO           *m_resInfoPool                                  = nullptr;  
    MOS_OCA_BUF_CONTEXT             m_ocaBufContextList[MAX_NUM_OF_OCA_BUF_CONTEXT] = {};
    MOS_OCA_BUFFER_CONFIG           m_config;
    uint32_t                        m_indexOfNextOcaBufContext                      = 0;
    uint32_t                        m_ocaLogSectionSizeLimit                        = OCA_LOG_SECTION_SIZE_MAX;

    static MOS_STATUS               s_ocaStatus;                    //!< The status for first oca error encounterred.
    static uint32_t                 s_lineNumForOcaErr;             //!< The line number for first oca error encounterred.
    static bool                     s_bOcaStatusExistInReg;         //!< ture if "Oca Status" already being added to reg.
    static int32_t                  s_refCount;
};
#endif  // __MOS_OCA_INTERFACE_SPECIFIC_H__