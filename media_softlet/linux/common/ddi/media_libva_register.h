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
//! \file     media_libva_register.h
//! \brief    libva component register head file
//!

#ifndef __MEDIA_LIBVA_REGISTER_H__
#define __MEDIA_LIBVA_REGISTER_H__

#include "ddi_cp_functions.h"
#include "ddi_decode_functions.h"
#include "ddi_encode_functions.h"
#include "ddi_vp_functions.h"

static const bool registeredMedia  = FunctionsFactory::Register<DdiMediaFunctions>((CompCommon));
static const bool registeredEncode = FunctionsFactory::Register<DdiEncodeFunctions>((CompEncode));
static const bool registeredDecode = FunctionsFactory::Register<DdiDecodeFunctions>((CompDecode));
static const bool registeredCp     = FunctionsFactory::Register<DdiCpFunctions>((CompCp));
static const bool registeredVp     = FunctionsFactory::Register<DdiVpFunctions>((CompVp));

#endif //__MEDIA_LIBVA_REGISTER_H__