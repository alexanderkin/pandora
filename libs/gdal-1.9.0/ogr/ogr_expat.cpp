/******************************************************************************
 * $Id: ogr_expat.cpp 18832 2010-02-14 12:57:50Z rouault $
 *
 * Project:  OGR
 * Purpose:  Convenience function for parsing with Expat library
 * Author:   Even Rouault, even dot rouault at mines dash paris dot org
 *
 ******************************************************************************
 * Copyright (c) 2009, Even Rouault
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
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#ifdef HAVE_EXPAT

#include "ogr_expat.h"
#include "cpl_error.h"

CPL_CVSID("$Id: ogr_expat.cpp 18832 2010-02-14 12:57:50Z rouault $");

#define OGR_EXPAT_MAX_ALLOWED_ALLOC 10000000

/************************************************************************/
/*                          OGRExpatMalloc()                            */
/************************************************************************/

static void* OGRExpatMalloc(size_t size)
{
    if (size < OGR_EXPAT_MAX_ALLOWED_ALLOC)
        return malloc(size);
    else
    {
        CPLError(CE_Failure, CPLE_OutOfMemory,
                 "Expat tried to malloc %d bytes. File probably corrupted", (int)size);
        return NULL;
    }
}

/************************************************************************/
/*                         OGRExpatRealloc()                            */
/************************************************************************/

static void* OGRExpatRealloc(void *ptr, size_t size)
{
    if (size < OGR_EXPAT_MAX_ALLOWED_ALLOC)
        return realloc(ptr, size);
    else
    {
        CPLError(CE_Failure, CPLE_OutOfMemory,
                 "Expat tried to realloc %d bytes. File probably corrupted", (int)size);
        free(ptr);
        return NULL;
    }
}

/************************************************************************/
/*                  OGRExpatUnknownEncodingHandler()                    */
/************************************************************************/

static int OGRExpatUnknownEncodingHandler (void *unused_encodingHandlerData,
                                           const XML_Char *name,
                                           XML_Encoding *info)
{
    if (!(EQUAL(name, "WINDOWS-1252")))
        return XML_STATUS_ERROR;

    /* Map CP1252 bytes to Unicode values */
    int i;
    for(i=0;i<0x80;i++)
        info->map[i] = i;

    info->map[0x80] = 0x20AC;
    info->map[0x81] = -1;
    info->map[0x82] = 0x201A;
    info->map[0x83] = 0x0192;
    info->map[0x84] = 0x201E;
    info->map[0x85] = 0x2026;
    info->map[0x86] = 0x2020;
    info->map[0x87] = 0x2021;
    info->map[0x88] = 0x02C6;
    info->map[0x89] = 0x2030;
    info->map[0x8A] = 0x0160;
    info->map[0x8B] = 0x2039;
    info->map[0x8C] = 0x0152;
    info->map[0x8D] = -1;
    info->map[0x8E] = 0x017D;
    info->map[0x8F] = -1;
    info->map[0x90] = -1;
    info->map[0x91] = 0x2018;
    info->map[0x92] = 0x2019;
    info->map[0x93] = 0x201C;
    info->map[0x94] = 0x201D;
    info->map[0x95] = 0x2022;
    info->map[0x96] = 0x2013;
    info->map[0x97] = 0x2014;
    info->map[0x98] = 0x02DC;
    info->map[0x99] = 0x2122;
    info->map[0x9A] = 0x0161;
    info->map[0x9B] = 0x203A;
    info->map[0x9C] = 0x0153;
    info->map[0x9D] = -1;
    info->map[0x9E] = 0x017E;
    info->map[0x9F] = 0x0178;

    for(i=0xA0;i<=0xFF;i++)
        info->map[i] = i;

    info->data    = NULL;
    info->convert = NULL;
    info->release = NULL;

    return XML_STATUS_OK;
}

/************************************************************************/
/*                       OGRCreateExpatXMLParser()                      */
/************************************************************************/

XML_Parser OGRCreateExpatXMLParser()
{
    XML_Memory_Handling_Suite memsuite;
    memsuite.malloc_fcn = OGRExpatMalloc;
    memsuite.realloc_fcn = OGRExpatRealloc;
    memsuite.free_fcn = free;
    XML_Parser hParser = XML_ParserCreate_MM(NULL, &memsuite, NULL);

    XML_SetUnknownEncodingHandler(hParser,
                                  OGRExpatUnknownEncodingHandler,
                                  NULL);

    return hParser;
}

#endif
