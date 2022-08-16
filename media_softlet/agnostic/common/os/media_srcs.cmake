# Copyright (c) 2019-2022, Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.

media_include_subdirectory(private)

set(TMP_SOURCES_
    ${CMAKE_CURRENT_LIST_DIR}/mos_context_next.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mos_graphicsresource_next.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mos_os_next.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mos_util_debug.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mos_utilities_next.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mos_gpucontext_next.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mos_gpucontextmgr_next.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mos_cmdbufmgr_next.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mos_commandbuffer_next.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mos_user_setting.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mos_os_mock_adaptor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mos_os_mock_adaptor_ext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mos_os.cpp
)

set(TMP_HEADERS_
    ${CMAKE_CURRENT_LIST_DIR}/mos_context_next.h
    ${CMAKE_CURRENT_LIST_DIR}/mos_graphicsresource_next.h
    ${CMAKE_CURRENT_LIST_DIR}/mos_os_next.h
    ${CMAKE_CURRENT_LIST_DIR}/mos_util_debug.h
    ${CMAKE_CURRENT_LIST_DIR}/mos_gpucontext_next.h
    ${CMAKE_CURRENT_LIST_DIR}/mos_gpucontextmgr_next.h
    ${CMAKE_CURRENT_LIST_DIR}/mos_cmdbufmgr_next.h
    ${CMAKE_CURRENT_LIST_DIR}/mos_commandbuffer_next.h
    ${CMAKE_CURRENT_LIST_DIR}/mos_interface.h
    ${CMAKE_CURRENT_LIST_DIR}/mos_user_setting.h
    ${CMAKE_CURRENT_LIST_DIR}/mos_utilities.h
    ${CMAKE_CURRENT_LIST_DIR}/mos_solo_generic.h
    ${CMAKE_CURRENT_LIST_DIR}/mos_mediacopy.h
    ${CMAKE_CURRENT_LIST_DIR}/mos_mediacopy_base.h
    ${CMAKE_CURRENT_LIST_DIR}/mos_os_mock_adaptor.h
)

if(${Media_Scalability_Supported} STREQUAL "yes")
set(TMP_SOURCES_
    ${TMP_SOURCES_}
    ${CMAKE_CURRENT_LIST_DIR}/mos_os_virtualengine_next.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mos_os_virtualengine_scalability_next.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mos_os_virtualengine_singlepipe_next.cpp
)

set(TMP_HEADERS_
    ${TMP_HEADERS_}
    ${CMAKE_CURRENT_LIST_DIR}/mos_os_virtualengine_next.h
    ${CMAKE_CURRENT_LIST_DIR}/mos_os_virtualengine_scalability_next.h
    ${CMAKE_CURRENT_LIST_DIR}/mos_os_virtualengine_singlepipe_next.h
)
endif() #if(${Media_Scalability_Supported} STREQUAL "yes")

set(SOFTLET_MOS_COMMON_SOURCES_
    ${SOFTLET_MOS_COMMON_SOURCES_}
    ${TMP_SOURCES_}
 )

set(SOFTLET_MOS_COMMON_HEADERS_
    ${SOFTLET_MOS_COMMON_HEADERS_}
    ${TMP_HEADERS_}
)

set(SOFTLET_MOS_PUBLIC_INCLUDE_DIRS_
    ${SOFTLET_MOS_PUBLIC_INCLUDE_DIRS_}
    ${CMAKE_CURRENT_LIST_DIR}
)

source_group( "mos_softlet" FILES ${TMP_SOURCES_} ${TMP_HEADERS_} )
