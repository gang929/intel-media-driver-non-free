/*
* Copyright (c) 2021-2023, Intel Corporation
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
//! \file     media_perf_profiler.h
//! \brief    Defines data structures and interfaces for media performance profiler.

#ifndef __MEDIA_PERF_PROFILER_H__
#define __MEDIA_PERF_PROFILER_H__

#include <map>
#include <unordered_map>
#include <stdint.h>
#include <memory>
#include "mos_defs.h"
#include "mos_os.h"
#include "media_class_trace.h"
#include "mhw_mi_itf.h"
#include "igfxfmid.h"
#include "mos_defs_specific.h"
#include "mos_os_specific.h"
namespace mhw
{
    namespace mi
    {
        class Itf;
    }
}  // namespace mhw

using Map = std::map<void*, uint32_t>;

/*! \brief In order to align GPU node value for all of OS,
*   we redifine the GPU node value here.
*/
typedef enum _PerfGPUNode
{
    PERF_GPU_NODE_3D     = 0,
    PERF_GPU_NODE_VIDEO  = 1,
    PERF_GPU_NODE_BLT    = 2,
    PERF_GPU_NODE_VE     = 3,
    PERF_GPU_NODE_VIDEO2 = 4,
    PERF_GPU_NODE_UNKNOW = 0xFF
}PerfGPUNode;

class MediaPerfProfiler
{
public:
    //!
    //! \brief    Get the instance of the profiler
    //!
    //! \return   pointer of profiler
    //!
    static MediaPerfProfiler *Instance();

    //!
    //! \brief    Destroy the resurces of profiler
    //!
    //! \param    [in] profiler
    //!           Pointer of profiler
    //! \param    [in] context
    //!           Pointer of Codechal/VPHal
    //! \param    [in] osInterface
    //!           Pointer of OS interface
    //!
    //! \return   void
    //!
    static void Destroy(MediaPerfProfiler* profiler, void* context, MOS_INTERFACE *osInterface);

    //!
    //! \brief    Initialize profiler and allcoate resources
    //!
    //! \param    [in] context
    //!           Pointer of Codechal/VPHal
    //! \param    [in] osInterface
    //!           Pointer of OS interface
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS Initialize(void* context, MOS_INTERFACE *osInterface);

    //!
    //! \brief    Insert start command of storing performance data
    //!
    //! \param    [in] context
    //!           Pointer of Codechal/VPHal
    //! \param    [in] osInterface
    //!           Pointer of OS interface
    //! \param    [in] miItf
    //!           Reference to Mhw MiItf.
    //! \param    [in] cmdBuffer
    //!           Pointer of OS command buffer
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS AddPerfCollectStartCmd(
        void* context,
        MOS_INTERFACE *osInterface,
        std::shared_ptr<mhw::mi::Itf> miItf,
        MOS_COMMAND_BUFFER *cmdBuffer);

    //!
    //! \brief    Insert end command of storing performance data
    //!
    //! \param    [in] context
    //!           Pointer of Codechal/VPHal
    //! \param    [in] osInterface
    //!           Pointer of OS interface
    //! \param    [in] miItf
    //!           Reference to Mhw MiItf.
    //! \param    [in] cmdBuffer
    //!           Pointer of OS command buffer
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS AddPerfCollectEndCmd(
        void* context,
        MOS_INTERFACE *osInterface,
        std::shared_ptr<mhw::mi::Itf> miItf,
        MOS_COMMAND_BUFFER *cmdBuffer);

    //!
    //! \brief    Constructor
    //!
    MediaPerfProfiler();

    //!
    //! \brief    Deconstructor
    //!
    virtual ~MediaPerfProfiler();

private:
    //!
    //! \brief    Save data to the buffer which store the performance data
    //!
    //! \param    [in] miItf
    //!           Reference to Mhw MiItf.
    //! \param    [in] cmdBuffer
    //!           Pointer of OS command buffer
    //! \param    [in] pOsContext
    //!           Pointer of DEVICE CONTEXT
    //! \param    [in] offset
    //!           Offset in the buffer
    //! \param    [in] value
    //!           Value of data
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS StoreData(
        std::shared_ptr<mhw::mi::Itf> miItf,
        PMOS_COMMAND_BUFFER cmdBuffer,
        MOS_CONTEXT_HANDLE  pOsContext,
        uint32_t            offset,
        uint32_t            value);

    //!
    //! \brief    Save register value to the buffer which store the performance data
    //! \param    [in] osInterface
    //!           Pointer of MOS_INTERFACE
    //!
    //! \param    [in] miItf
    //!           Reference to Mhw MiItf.
    //! \param    [in] cmdBuffer
    //!           Pointer of OS command buffer
    //! \param    [in] offset
    //!           Offset in the buffer
    //! \param    [in] reg
    //!           Address of register
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS StoreRegister(
        MOS_INTERFACE *osInterface,
        std::shared_ptr<mhw::mi::Itf> miItf,
        PMOS_COMMAND_BUFFER cmdBuffer,
        uint32_t offset,
        uint32_t reg);

    //!
    //! \brief    Save timestamp to the buffer by Pipe control command
    //!
    //! \param    [in] miItf
    //!            Reference to Mhw MiItf.
    //! \param    [in] cmdBuffer
    //!           Pointer of OS command buffer
    //! \param    [in] pOsContext
    //!           Pointer of DEVICE CONTEXT
    //! \param    [in] offset
    //!           Offset in the buffer
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS StoreTSByPipeCtrl(
        std::shared_ptr<mhw::mi::Itf> miItf,
        PMOS_COMMAND_BUFFER cmdBuffer,
        MOS_CONTEXT_HANDLE  pOsContext,
        uint32_t            offset);

    //!
    //! \brief    Save timestamp to the buffer by MI command
    //!
    //! \param    [in] miItf
    //!           Reference to Mhw MiItf.
    //! \param    [in] pOsContext
    //!           Pointer of DEVICE CONTEXT
    //! \param    [in] cmdBuffer
    //!           Pointer of OS command buffer
    //! \param    [in] offset
    //!           Offset in the buffer
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS StoreTSByMiFlush(
        std::shared_ptr<mhw::mi::Itf> miItf,
        PMOS_COMMAND_BUFFER cmdBuffer,
        MOS_CONTEXT_HANDLE  pOsContext,
        uint32_t offset);

    //!
    //! \brief    Save performance data in to a file
    //!
    //! \param    [in] osInterface
    //!           Pointer of OS interface
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS SavePerfData(MOS_INTERFACE *osInterface);

    //!
    //! \brief    Convert GPU context to GPU node
    //!
    //! \param    [in] context
    //!           GPU context
    //!
    //! \return   PerfGPUNode
    //!
    PerfGPUNode GpuContextToGpuNode(MOS_GPU_CONTEXT context);

    //!
    //! \brief    Check the performance mode
    //!
    //! \param    [in] regs
    //!           The registers' array
    //!
    //! \return   bool
    //!           true if include the memory information
    //!
    bool IsPerfModeWidthMemInfo(uint32_t *regs);

    //!
    //! \brief    Map the platform ID
    //!
    //! \param    [in] platform
    //!
    //! \return   uint32_t
    //!           mapped platform id used to fill node header
    //!
    uint32_t PlatFormIdMap(PLATFORM platform);

    //!
    //! \brief    Save data to the buffer which store the performance data 
    //!
    //! \param    [in] miInterface
    //!           Pointer of MI interface
    //! \param    [in] cmdBuffer
    //!           Pointer of OS command buffer
    //! \param    [in] pOsContext
    //!           Pointer of DEVICE CONTEXT
    //! \param    [in] offset
    //!           Offset in the buffer
    //! \param    [in] value       
    //!           Value of data
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS StoreData(
        MhwMiInterface *miInterface,
        PMOS_COMMAND_BUFFER cmdBuffer,
        PMOS_CONTEXT pOsContext,
        uint32_t offset,
        uint32_t value);

    //!
    //! \brief    Save data to the buffer which store the performance data 
    //!
    //! \param    [in] miInterface
    //!           Pointer of MI interface
    //! \param    [in] cmdBuffer
    //!           Pointer of OS command buffer
    //! \param    [in] pOsContext
    //!           Pointer of DEVICE CONTEXT
    //! \param    [in] offset
    //!           Offset in the buffer
    //! \param    [in] value       
    //!           Value of data
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS StoreDataNext(
        MhwMiInterface *miInterface, 
        PMOS_COMMAND_BUFFER cmdBuffer,
        PMOS_CONTEXT pOsContext,
        uint32_t offset,
        uint32_t value);

    //!
    //! \brief    Save register value to the buffer which store the performance data 
    //! \param    [in] osInterface
    //!           Pointer of MOS_INTERFACE
    //!
    //! \param    [in] miInterface
    //!           Pointer of MI interface
    //! \param    [in] cmdBuffer
    //!           Pointer of OS command buffer
    //! \param    [in] offset
    //!           Offset in the buffer
    //! \param    [in] reg
    //!           Address of register
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS StoreRegister(
        MOS_INTERFACE *osInterface,
        MhwMiInterface *miInterface,
        PMOS_COMMAND_BUFFER cmdBuffer,
        uint32_t offset,
        uint32_t reg);

    //!
    //! \brief    Save register value to the buffer which store the performance data 
    //! \param    [in] osInterface
    //!           Pointer of MOS_INTERFACE
    //!
    //! \param    [in] miInterface
    //!           Pointer of MI interface
    //! \param    [in] cmdBuffer
    //!           Pointer of OS command buffer
    //! \param    [in] offset
    //!           Offset in the buffer
    //! \param    [in] reg
    //!           Address of register
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS StoreRegisterNext(
        MOS_INTERFACE *osInterface,
        MhwMiInterface *miInterface,
        PMOS_COMMAND_BUFFER cmdBuffer,
        uint32_t offset,
        uint32_t reg);

    //!
    //! \brief    Save timestamp to the buffer by Pipe control command 
    //!
    //! \param    [in] miInterface 
    //!           Pointer of MI interface
    //! \param    [in] cmdBuffer
    //!           Pointer of OS command buffer
    //! \param    [in] pOsContext
    //!           Pointer of DEVICE CONTEXT
    //! \param    [in] offset
    //!           Offset in the buffer
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS StoreTSByPipeCtrl(
        MhwMiInterface *miInterface,
        PMOS_COMMAND_BUFFER cmdBuffer,
        PMOS_CONTEXT pOsContext,
        uint32_t offset);

    //!
    //! \brief    Save timestamp to the buffer by Pipe control command 
    //!
    //! \param    [in] miInterface 
    //!           Pointer of MI interface
    //! \param    [in] cmdBuffer
    //!           Pointer of OS command buffer
    //! \param    [in] pOsContext
    //!           Pointer of DEVICE CONTEXT
    //! \param    [in] offset
    //!           Offset in the buffer
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS StoreTSByPipeCtrlNext(
        MhwMiInterface *miInterface,
        PMOS_COMMAND_BUFFER cmdBuffer,
        PMOS_CONTEXT pOsContext,
        uint32_t offset);

    //!
    //! \brief    Save timestamp to the buffer by MI command 
    //!
    //! \param    [in] miInterface 
    //!           Pointer of MI interface
    //! \param    [in] cmdBuffer
    //!           Pointer of OS command buffer
    //! \param    [in] pOsContext
    //!           Pointer of DEVICE CONTEXT
    //! \param    [in] offset
    //!           Offset in the buffer
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS StoreTSByMiFlush(
        MhwMiInterface *miInterface,
        PMOS_COMMAND_BUFFER cmdBuffer,
        PMOS_CONTEXT pOsContext,
        uint32_t offset);

    //!
    //! \brief    Save timestamp to the buffer by MI command 
    //!
    //! \param    [in] miInterface 
    //!           Pointer of MI interface
    //! \param    [in] cmdBuffer
    //!           Pointer of OS command buffer
    //! \param    [in] pOsContext
    //!           Pointer of DEVICE CONTEXT
    //! \param    [in] offset
    //!           Offset in the buffer
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS StoreTSByMiFlushNext(
        MhwMiInterface* miInterface,
        PMOS_COMMAND_BUFFER cmdBuffer,
        PMOS_CONTEXT pOsContext,
        uint32_t offset);

public:
    //!
    //! \brief    Insert start command of storing performance data
    //!
    //! \param    [in] context
    //!           Pointer of Codechal/VPHal
    //! \param    [in] osInterface
    //!           Pointer of OS interface
    //! \param    [in] miInterface
    //!           pointer of MI interface
    //! \param    [in] cmdBuffer
    //!           Pointer of OS command buffer
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS AddPerfCollectStartCmd(
        void* context,
        MOS_INTERFACE *osInterface,
        MhwMiInterface *miInterface,
        MOS_COMMAND_BUFFER *cmdBuffer);

    //!
    //! \brief    Insert end command of storing performance data
    //!
    //! \param    [in] context
    //!           Pointer of Codechal/VPHal
    //! \param    [in] osInterface
    //!           Pointer of OS interface
    //! \param    [in] miInterface
    //!           pointer of MI interface
    //! \param    [in] cmdBuffer
    //!           Pointer of OS command buffer
    //!
    //! \return   MOS_STATUS
    //!           MOS_STATUS_SUCCESS if success, else fail reason
    //!
    MOS_STATUS AddPerfCollectEndCmd(
        void* context,
        MOS_INTERFACE *osInterface,
        MhwMiInterface *miInterface,
        MOS_COMMAND_BUFFER *cmdBuffer);

private:
    std::unordered_map<PMOS_CONTEXT, PMOS_RESOURCE>  m_perfStoreBufferMap;   //!< Buffer for perf data collection
    std::unordered_map<PMOS_CONTEXT,uint32_t>        m_refMap;               //!< The number of refereces
    std::unordered_map<PMOS_CONTEXT,uint32_t>        m_perfDataIndexMap;     //!< The index of performance data node in buffer
    std::unordered_map<PMOS_CONTEXT,bool>            m_initializedMap;       //!< Indicate whether profiler was initialized

    Map                           m_contextIndexMap;       //!< Map between CodecHal/VPHal and PerfDataContext
    PMOS_MUTEX                    m_mutex = nullptr;       //!< Mutex for protecting data of profiler when refereced multi times
    uint32_t                      m_bufferSize = 10000000; //!< The size of perf data buffer
    uint32_t                      m_timerBase  = 0;        //!< time frequency
    int32_t                       m_multiprocess = 0;      //!< multi process support
    uint32_t                      m_registers[8] = { 0 };  //!< registers of Memory information
    const std::string             m_registersKey[8] = {__MEDIA_USER_FEATURE_VALUE_PERF_PROFILER_REGISTER_KEY_1,
                                                       __MEDIA_USER_FEATURE_VALUE_PERF_PROFILER_REGISTER_KEY_2,
                                                       __MEDIA_USER_FEATURE_VALUE_PERF_PROFILER_REGISTER_KEY_3,
                                                       __MEDIA_USER_FEATURE_VALUE_PERF_PROFILER_REGISTER_KEY_4,
                                                       __MEDIA_USER_FEATURE_VALUE_PERF_PROFILER_REGISTER_KEY_5,
                                                       __MEDIA_USER_FEATURE_VALUE_PERF_PROFILER_REGISTER_KEY_6,
                                                       __MEDIA_USER_FEATURE_VALUE_PERF_PROFILER_REGISTER_KEY_7,
                                                       __MEDIA_USER_FEATURE_VALUE_PERF_PROFILER_REGISTER_KEY_8};  //!< registers key
    int32_t                       m_profilerEnabled;             //!< UMD Perf Profiler enable or not
    std::string                   m_outputFileName = "";         //!< Name of output file
    bool                          m_enableProfilerDump = true;   //!< Indicate whether enable UMD Profiler dump
    std::shared_ptr<mhw::mi::Itf> m_miItf = nullptr;
    int32_t                       m_multiprocesssinglebin = 0;   //!< multi process single binary flag
    uint32_t*                     m_perfDataCombined = nullptr;  //!< Combined perf data pointer
    uint32_t                      m_perfDataCombinedSize = 0;    //!< Combined perf data size
    uint32_t                      m_perfDataCombinedIndex = 0;   //!< Combined perf data index
    uint32_t                      m_perfDataCombinedOffset = 0;  //!< Combined perf data offset
MEDIA_CLASS_DEFINE_END(MediaPerfProfiler)
};

#endif // __MEDIA_PERF_PROFILER_H__