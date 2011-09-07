/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <cassert>
#include <windows.h>
#include <iostream>
#include <tchar.h>

#include "h263_information.h"

#define TEST_STR "Test H263 parser."
#define TEST_PASSED() std::cerr << TEST_STR << " : [OK]" << std::endl
#define PRINT_LINE std::cout << "------------------------------------------" << std::endl;


void PrintInfo(const H263Info* ptr)
{
    std::cout << "info =    GOBs: {";
    for (int i = 0; i < ptr->numOfGOBs; i++)
    {
        std::cout << ptr->ptrGOBbuffer[i] << " ";
    }
    std::cout << "}" << std::endl;
    std::cout << "          sBit: {";
    for (int i = 0; i < ptr->numOfGOBs; i++)
    {
        std::cout << (int)ptr->ptrGOBbufferSBit[i] << " ";
    }
    std::cout << "}" << std::endl;

    std::cout << "uiH263PTypeFmt: " << (int)ptr->uiH263PTypeFmt << std::endl;
    std::cout << "     codecBits: " << (int)ptr->codecBits << std::endl;
    std::cout << "        pQuant: " << (int)ptr->pQuant << std::endl;
    std::cout << "         fType: " << (int)ptr->fType << std::endl;
    std::cout << "        cpmBit: " << (int)ptr->cpmBit << std::endl;
    std::cout << "     numOfGOBs: " << (int)ptr->numOfGOBs << std::endl;
    std::cout << "      numOfMBs: " << (int)ptr->totalNumOfMBs << std::endl;
    std::cout << "                " <<  std::endl;
};

void PrintMBInfo(
    const H263Info* ptr,
    const H263MBInfo* ptrMB)
{
    std::cout << "        gQuant: {";
    for (int i = 0; i < ptr->numOfGOBs; i++)
    {
        std::cout << (int)ptr->ptrGQuant[i] << " ";
    }
    std::cout << "}" << std::endl;
    std::cout << "                " <<  std::endl;

    std::cout << "MBs:{";
    int k = 0;
    for (int i = 0; i < ptr->numOfGOBs; i++)
    {
        std::cout << " {";
        for (int j = 0; j < ptr->ptrNumOfMBs[i]; j++)
        {
            std::cout << ptrMB->ptrBuffer[k++] << " ";
        }
        std::cout << "}" << std::endl;
    }
    std::cout << "}" << std::endl;
    PRINT_LINE;
};

void ValidateResults(
    const H263Info* ptr,
    const H263Info* ptrRef)
{
    assert(ptr->uiH263PTypeFmt == ptrRef->uiH263PTypeFmt);
    assert(ptr->codecBits      == ptrRef->codecBits);
    assert(ptr->pQuant         == ptrRef->pQuant);
    assert(ptr->fType          == ptrRef->fType);
    assert(ptr->numOfGOBs      == ptrRef->numOfGOBs);
    assert(ptr->totalNumOfMBs  == ptrRef->totalNumOfMBs);
    for (int i = 0; i < ptr->numOfGOBs; i++)
    {
        assert(ptr->ptrGOBbuffer[i]     == ptrRef->ptrGOBbuffer[i]);
        assert(ptr->ptrGOBbufferSBit[i] == ptrRef->ptrGOBbufferSBit[i]);
    }
    PrintInfo(ptr);
}

void ValidateMBResults(
    const H263Info* ptr,
    const H263MBInfo* ptrMB,
    const H263Info* ptrRef,
    bool  printRes = true)
{
    int offset = 0;
    int numBytes = 0;
    for (int i = 0; i < ptr->numOfGOBs; i++)
    {
        offset = ptr->CalculateMBOffset(i+1);
        numBytes += ptrMB->ptrBuffer[offset - 1] / 8;
        int numBytesRem = ptrMB->ptrBuffer[offset - 1] % 8;
        if (numBytesRem)
        {
            numBytes++;
        }
        assert(ptr->ptrGQuant[i] == ptrRef->ptrGQuant[i]);
    }
    assert(ptr->ptrGOBbuffer[ptr->numOfGOBs] == numBytes);
    assert(unsigned int( ptr->totalNumOfMBs) <= ptrMB->bufferSize);
    if (printRes)
    {
        PrintMBInfo(ptr, ptrMB);
    }
}


int _tmain(int argc, _TCHAR* argv[])
{

    std::string str;
    std::cout << "--------------------------------" << std::endl;
    std::cout << "------- Test H.263 Parser ------" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    std::cout << "  "  << std::endl;

    // -----------------------------------------------------
    // Input data - H.263 encoded stream SQCIF (P-frame)
    // -----------------------------------------------------
    const int lengthRefSQCIF = 77;
    const unsigned char encodedStreamSQCIF[lengthRefSQCIF] = {
    0x00, 0x00, 0x81, 0xf6, 0x06, 0x04, 0x3f, 0xb7, 0xbc, 0x00, 0x00, 0x86, 0x23,
    0x5b, 0xdb, 0xdf, 0xb1, 0x93, 0xdb, 0xde, 0xd6, 0xf0, 0x00, 0x00, 0x8a, 0x27,
    0xdb, 0xcf, 0xad, 0xbe, 0x00, 0x00, 0x8e, 0x27, 0xed, 0xef, 0x80, 0x00, 0x00,
    0x92, 0x27, 0x6f, 0x7f, 0x80, 0x00, 0x00, 0x96, 0x20, 0xfc, 0xe2, 0xdb, 0xfe,
    0xb7, 0x7d, 0xea, 0x5f, 0xf8, 0xab, 0xd2, 0xff, 0xf6, 0xc9, 0xe5, 0x5e, 0x97,
    0xf7, 0xff, 0xad, 0x4f, 0x49, 0x3b, 0xff, 0xd6, 0xa6, 0x75, 0x82, 0x60};

    // Expected results
    H263Info infoRefSQCIF;
    infoRefSQCIF.uiH263PTypeFmt = 1;
    infoRefSQCIF.codecBits      = 8;
    infoRefSQCIF.pQuant         = 4;
    infoRefSQCIF.fType          = 1;
    infoRefSQCIF.cpmBit         = 0;
    infoRefSQCIF.numOfGOBs      = 6;
    infoRefSQCIF.totalNumOfMBs  = 8*6;

    infoRefSQCIF.ptrGOBbuffer[0] = 0;  infoRefSQCIF.ptrGOBbufferSBit[0] = 0; infoRefSQCIF.ptrGQuant[0] = 0;
    infoRefSQCIF.ptrGOBbuffer[1] = 9;  infoRefSQCIF.ptrGOBbufferSBit[1] = 0; infoRefSQCIF.ptrGQuant[1] = 4;
    infoRefSQCIF.ptrGOBbuffer[2] = 22; infoRefSQCIF.ptrGOBbufferSBit[2] = 0; infoRefSQCIF.ptrGQuant[2] = 4;
    infoRefSQCIF.ptrGOBbuffer[3] = 30; infoRefSQCIF.ptrGOBbufferSBit[3] = 0; infoRefSQCIF.ptrGQuant[3] = 4;
    infoRefSQCIF.ptrGOBbuffer[4] = 37; infoRefSQCIF.ptrGOBbufferSBit[4] = 0; infoRefSQCIF.ptrGQuant[4] = 4;
    infoRefSQCIF.ptrGOBbuffer[5] = 44; infoRefSQCIF.ptrGOBbufferSBit[5] = 0; infoRefSQCIF.ptrGQuant[5] = 4;

    // ----------------------------------------------------
    // Input data - H.263 encoded stream QCIF (P-frame)
    // ----------------------------------------------------
    const int lengthRefQCIF = 123;
    const unsigned char encodedStreamQCIF[lengthRefQCIF] = {
    0x00, 0x00, 0x81, 0x02, 0x0a, 0x04, 0x3f, 0xf8, 0x00, 0x00, 0x86, 0x27, 0x8b,
    0xc6, 0x9f, 0x17, 0x9c, 0x00, 0x00, 0x8a, 0x20, 0xbc, 0x22, 0xf8, 0x5f, 0x46,
    0x03, 0xc1, 0x77, 0x15, 0xe0, 0xb8, 0x38, 0x3f, 0x05, 0xa0, 0xbf, 0x8f, 0x00,
    0x00, 0x8e, 0x27, 0xfc, 0x5e, 0x5a, 0x33, 0x80, 0x00, 0x00, 0x92, 0x25, 0x8c,
    0x1e, 0xbf, 0xfc, 0x7e, 0x35, 0xfc, 0x00, 0x00, 0x96, 0x27, 0xff, 0x00, 0x00,
    0x9a, 0x20, 0xdb, 0x34, 0xef, 0xfc, 0x00, 0x00, 0x9e, 0x20, 0xaf, 0x17, 0x0d,
    0x3e, 0xde, 0x0f, 0x8f, 0xff, 0x80, 0x00, 0x00, 0xa2, 0x22, 0xbb, 0x27, 0x81,
    0xeb, 0xff, 0x5b, 0x07, 0xab, 0xff, 0xad, 0x9e, 0xd8, 0xc9, 0x6b, 0x75, 0x54,
    0xbf, 0xbe, 0x8a, 0xbd, 0xf2, 0xfb, 0xfb, 0x3d, 0x3d, 0x25, 0xb7, 0xf7, 0xfc,
    0x92, 0x4c, 0xdb, 0x6d, 0x69, 0xc0};

    // Expected results
    H263Info infoRefQCIF;
    infoRefQCIF.uiH263PTypeFmt = 2;
    infoRefQCIF.codecBits      = 8;
    infoRefQCIF.pQuant         = 4;
    infoRefQCIF.fType          = 1;
    infoRefQCIF.cpmBit         = 0;
    infoRefQCIF.numOfGOBs      = 9;
    infoRefQCIF.totalNumOfMBs  = 11*9;

    infoRefQCIF.ptrGOBbuffer[0] = 0;   infoRefQCIF.ptrGOBbufferSBit[0] = 0; infoRefQCIF.ptrGQuant[0] = 0;
    infoRefQCIF.ptrGOBbuffer[1] = 8;   infoRefQCIF.ptrGOBbufferSBit[1] = 0; infoRefQCIF.ptrGQuant[1] = 4;
    infoRefQCIF.ptrGOBbuffer[2] = 17;  infoRefQCIF.ptrGOBbufferSBit[2] = 0; infoRefQCIF.ptrGQuant[2] = 4;
    infoRefQCIF.ptrGOBbuffer[3] = 38;  infoRefQCIF.ptrGOBbufferSBit[3] = 0; infoRefQCIF.ptrGQuant[3] = 4;
    infoRefQCIF.ptrGOBbuffer[4] = 47;  infoRefQCIF.ptrGOBbufferSBit[4] = 0; infoRefQCIF.ptrGQuant[4] = 4;
    infoRefQCIF.ptrGOBbuffer[5] = 58;  infoRefQCIF.ptrGOBbufferSBit[5] = 0; infoRefQCIF.ptrGQuant[5] = 4;
    infoRefQCIF.ptrGOBbuffer[6] = 63;  infoRefQCIF.ptrGOBbufferSBit[6] = 0; infoRefQCIF.ptrGQuant[6] = 4;
    infoRefQCIF.ptrGOBbuffer[7] = 71;  infoRefQCIF.ptrGOBbufferSBit[7] = 0; infoRefQCIF.ptrGQuant[7] = 4;
    infoRefQCIF.ptrGOBbuffer[8] = 84;  infoRefQCIF.ptrGOBbufferSBit[8] = 0; infoRefQCIF.ptrGQuant[8] = 4;

    // ---------------------------------------------------
    // Input data - H.263 encoded stream CIF (P-frame)
    // ---------------------------------------------------
    const int lengthRefCIF = 212;
    const unsigned char encodedStreamCIF[lengthRefCIF] = {
    0x00, 0x00, 0x82, 0x9a, 0x0e, 0x04, 0x3f, 0xff, 0xff, 0x00, 0x00, 0x86, 0x27,
    0xff, 0xff, 0xe0, 0x00, 0x00, 0x8a, 0x27, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x8e,
    0x27, 0xff, 0xff, 0x6b, 0x09, 0x70, 0x00, 0x00, 0x92, 0x27, 0xff, 0xff, 0xe0,
    0x00, 0x00, 0x96, 0x27, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x9a, 0x27, 0x6f, 0x7f,
    0xff, 0xfe, 0x00, 0x00, 0x9e, 0x27, 0xff, 0xfe, 0xc6, 0x31, 0xe0, 0x00, 0x00,
    0xa2, 0x27, 0xff, 0xff, 0xe0, 0x00, 0x00, 0xa6, 0x27, 0xf6, 0xb7, 0xfe, 0xa6,
    0x14, 0x95, 0xb4, 0xc6, 0x41, 0x6b, 0x3a, 0x2e, 0x8d, 0x42, 0xef, 0xc0, 0x00,
    0x00, 0xaa, 0x27, 0xff, 0xb1, 0x95, 0x05, 0x0c, 0xe3, 0x4a, 0x17, 0xff, 0x80,
    0x00, 0x00, 0xae, 0x27, 0xff, 0xf6, 0xf7, 0xfe, 0x00, 0x00, 0xb2, 0x27, 0xff,
    0x8b, 0xdf, 0xff, 0x00, 0x00, 0xb6, 0x27, 0xff, 0xff, 0xe0, 0x00, 0x00, 0xba,
    0x26, 0x2f, 0x7f, 0xff, 0xfb, 0x58, 0x5b, 0x80, 0x00, 0x00, 0xbe, 0x20, 0xbc,
    0xe4, 0x5e, 0x6f, 0xff, 0xfe, 0xce, 0xf1, 0x94, 0x00, 0x00, 0xc2, 0x23, 0x18,
    0x4b, 0x17, 0x87, 0x0f, 0xff, 0xb3, 0xb6, 0x09, 0x63, 0x46, 0x73, 0x40, 0xb2,
    0x5f, 0x0a, 0xc6, 0xbe, 0xd7, 0x7a, 0x61, 0xbc, 0x68, 0xde, 0xf0, 0x00, 0x00,
    0xc6, 0x26, 0x2f, 0x0a, 0xff, 0xff, 0x17, 0x9a, 0x17, 0x8c, 0xb1, 0x37, 0x67,
    0xaf, 0xec, 0xf7, 0xa5, 0xbf, 0xb6, 0x49, 0x27, 0x6d, 0xb7, 0x92, 0x49, 0x1b,
    0xb4, 0x9a, 0xe0, 0x62};

    // Expected results
    H263Info infoRefCIF;
    infoRefCIF.uiH263PTypeFmt = 3;
    infoRefCIF.codecBits      = 8;
    infoRefCIF.pQuant         = 4;
    infoRefCIF.fType          = 1;
    infoRefCIF.cpmBit         = 0;
    infoRefCIF.numOfGOBs      = 18;
    infoRefCIF.totalNumOfMBs  = 22*18;

    infoRefCIF.ptrGOBbuffer[0]  = 0;   infoRefCIF.ptrGOBbufferSBit[0]  = 0; infoRefCIF.ptrGQuant[0]  = 0;
    infoRefCIF.ptrGOBbuffer[1]  = 9;   infoRefCIF.ptrGOBbufferSBit[1]  = 0; infoRefCIF.ptrGQuant[1]  = 4;
    infoRefCIF.ptrGOBbuffer[2]  = 16;  infoRefCIF.ptrGOBbufferSBit[2]  = 0; infoRefCIF.ptrGQuant[2]  = 4;
    infoRefCIF.ptrGOBbuffer[3]  = 23;  infoRefCIF.ptrGOBbufferSBit[3]  = 0; infoRefCIF.ptrGQuant[3]  = 4;
    infoRefCIF.ptrGOBbuffer[4]  = 32;  infoRefCIF.ptrGOBbufferSBit[4]  = 0; infoRefCIF.ptrGQuant[4]  = 4;
    infoRefCIF.ptrGOBbuffer[5]  = 39;  infoRefCIF.ptrGOBbufferSBit[5]  = 0; infoRefCIF.ptrGQuant[5]  = 4;
    infoRefCIF.ptrGOBbuffer[6]  = 46;  infoRefCIF.ptrGOBbufferSBit[6]  = 0; infoRefCIF.ptrGQuant[6]  = 4;
    infoRefCIF.ptrGOBbuffer[7]  = 54;  infoRefCIF.ptrGOBbufferSBit[7]  = 0; infoRefCIF.ptrGQuant[7]  = 4;
    infoRefCIF.ptrGOBbuffer[8]  = 63;  infoRefCIF.ptrGOBbufferSBit[8]  = 0; infoRefCIF.ptrGQuant[8]  = 4;
    infoRefCIF.ptrGOBbuffer[9]  = 70;  infoRefCIF.ptrGOBbufferSBit[9]  = 0; infoRefCIF.ptrGQuant[9]  = 4;
    infoRefCIF.ptrGOBbuffer[10] = 90;  infoRefCIF.ptrGOBbufferSBit[10]  = 0; infoRefCIF.ptrGQuant[10]  = 4;
    infoRefCIF.ptrGOBbuffer[11] = 104; infoRefCIF.ptrGOBbufferSBit[11] = 0; infoRefCIF.ptrGQuant[11] = 4;
    infoRefCIF.ptrGOBbuffer[12] = 112; infoRefCIF.ptrGOBbufferSBit[12] = 0; infoRefCIF.ptrGQuant[12] = 4;
    infoRefCIF.ptrGOBbuffer[13] = 120; infoRefCIF.ptrGOBbufferSBit[13] = 0; infoRefCIF.ptrGQuant[13] = 4;
    infoRefCIF.ptrGOBbuffer[14] = 127; infoRefCIF.ptrGOBbufferSBit[14] = 0; infoRefCIF.ptrGQuant[14] = 4;
    infoRefCIF.ptrGOBbuffer[15] = 138; infoRefCIF.ptrGOBbufferSBit[15] = 0; infoRefCIF.ptrGQuant[15] = 4;
    infoRefCIF.ptrGOBbuffer[16] = 151; infoRefCIF.ptrGOBbufferSBit[16] = 0; infoRefCIF.ptrGQuant[16] = 4;
    infoRefCIF.ptrGOBbuffer[17] = 180; infoRefCIF.ptrGOBbufferSBit[17] = 0; infoRefCIF.ptrGQuant[17] = 4;

    // -----------------------------------------------------------------------
    // Input data - H.263 encoded stream QCIF (I-frame). Non byte aligned GOBs
    // -----------------------------------------------------------------------
    const int lengthRefQCIF_N = 2020;
    const unsigned char encodedStreamQCIF_N[lengthRefQCIF_N] = {
    0x00,0x00,0x80,0x06,0x08,0x07,0x34,0xe4,0xf7,0x66,0x75,0x12,0x9b,0x64,0x83,0xe9,0x4c,0xc7,0x3c,0x77,0x83,0xcf,0x67,0x96,
    0xe0,0x06,0x69,0x95,0x70,0x60,0x43,0x53,0x96,0x8a,0xa2,0x9e,0x96,0xf8,0x79,0xf0,0xf2,0xb8,0x30,0x21,0xb0,0x0c,0xc0,0x22,
    0x0e,0x70,0x12,0xb0,0x30,0x21,0x9e,0x03,0x3e,0x02,0x22,0xa8,0x83,0xcf,0x7d,0xe8,0xf3,0x85,0x8d,0x01,0x47,0x29,0x03,0x02,
    0x16,0x5c,0x06,0x7c,0x84,0x56,0x9c,0x0c,0x08,0x49,0x70,0x29,0xe1,0x68,0x39,0xbf,0xd0,0xef,0x6f,0x6e,0x8e,0x64,0x72,0x70,
    0x30,0x21,0x03,0xe0,0x53,0x82,0xab,0xe2,0xd4,0xa8,0x3e,0x12,0x80,0xc8,0x2a,0x9f,0x07,0x23,0xdf,0x4f,0xaa,0x5b,0x72,0xaa,
    0x22,0x81,0x21,0x7a,0x80,0x54,0x85,0x82,0x50,0x17,0x40,0x2a,0x7f,0x3c,0xfe,0xa5,0x49,0x39,0x08,0x18,0x20,0x65,0x95,0x8c,
    0x8f,0x67,0xc7,0xc0,0x63,0xe1,0x1b,0xf1,0xef,0xfd,0x25,0x13,0x24,0x9c,0xa9,0x1f,0x02,0x9b,0xe1,0x19,0xae,0x97,0x02,0x9a,
    0x84,0x6f,0xbf,0xfd,0x14,0xf2,0x09,0xe3,0x10,0x38,0x0a,0x61,0x99,0x6e,0xf8,0x14,0xce,0x7e,0xff,0xe7,0xa7,0x8e,0x4f,0x14,
    0xa9,0x80,0xa6,0x71,0x3a,0x98,0x06,0x1c,0xfc,0xff,0xcd,0x8f,0x18,0x9e,0x24,0x52,0x05,0x9c,0x42,0xa5,0x33,0x9f,0xdf,0x80,
    0x00,0x21,0x0e,0xf6,0x61,0xe2,0x7a,0x35,0x64,0x8e,0x61,0xa2,0x58,0x2a,0x0b,0x45,0x4c,0xe0,0xa1,0xa0,0xa6,0x83,0x58,0xf0,
    0x08,0x77,0x3e,0xb5,0xc6,0x05,0x6b,0x6e,0x69,0x06,0x07,0xe6,0x82,0x9b,0xc8,0x7e,0x2d,0x6d,0x06,0x07,0xe3,0xe0,0xa6,0x2e,
    0x05,0x58,0x39,0xbf,0xd1,0xc7,0x9e,0x15,0x29,0x39,0x31,0xbc,0x18,0x1f,0x62,0xf5,0x95,0x20,0xa0,0xe6,0xff,0x38,0x83,0x03,
    0xea,0x3f,0x02,0xc8,0x05,0x71,0x31,0xef,0x79,0x49,0x48,0x73,0x06,0x07,0xd4,0x4b,0x5f,0xc0,0xe4,0x81,0x3b,0x03,0x03,0xea,
    0x24,0x81,0x90,0x55,0x41,0x64,0x2b,0xea,0xd1,0xcc,0xa5,0x99,0x79,0x06,0x07,0xd8,0x49,0x05,0x39,0x64,0x16,0x3d,0x03,0x03,
    0xee,0x24,0x83,0x0b,0xf9,0x46,0x50,0x27,0xdf,0xd4,0xa6,0xa6,0x3d,0x83,0x03,0xf0,0x25,0x83,0x0b,0xf8,0x19,0xbe,0x83,0x03,
    0xf2,0x3f,0x06,0x17,0xf1,0x58,0xd6,0x04,0xff,0xfa,0x94,0xb4,0x87,0xe0,0x43,0x2f,0x06,0x17,0xf1,0x58,0x11,0x7b,0xe0,0x21,
    0x97,0x82,0x9d,0x58,0x11,0x7b,0xf9,0xf8,0x15,0xa2,0x9f,0x40,0x3e,0xf4,0x07,0xd5,0x81,0x9f,0xa1,0x7b,0xca,0x9a,0xbb,0xdf,
    0xcf,0xc0,0xf3,0xb0,0x09,0x37,0x00,0x97,0x63,0xee,0x47,0xff,0xff,0xe8,0x68,0xa5,0xc6,0x43,0xdf,0x2f,0x06,0xad,0xad,0x3f,
    0xef,0xf6,0x30,0x03,0xea,0x80,0xcf,0x80,0x8b,0x8a,0xe2,0xb5,0x40,0xa7,0x2e,0x06,0x27,0xf7,0xc2,0xd6,0x47,0x2f,0xc1,0x3f,
    0x80,0x00,0x44,0x1f,0xb5,0xbd,0xc5,0xed,0x4f,0x70,0x7c,0x43,0xec,0x8e,0xcf,0x6e,0xcf,0x7b,0x88,0x7d,0x9b,0xfc,0x13,0xfc,
    0x24,0x3b,0xf4,0xe1,0x61,0xe1,0xe2,0x61,0x5f,0xa7,0x13,0x15,0x17,0x1b,0x08,0xff,0x38,0x98,0x98,0xb8,0xb8,0x2f,0xf9,0xc3,
    0xc2,0xc5,0x43,0xc0,0xff,0xce,0x09,0xfa,0x16,0x06,0x07,0xfe,0x6f,0x4e,0xcf,0xaf,0x30,0x3f,0xf3,0x70,0x6c,0x74,0x6e,0x81,
    0xff,0x9b,0x33,0x13,0x53,0x2c,0x0f,0xf8,0x00,0x04,0x61,0xf0,0xd4,0xf7,0x17,0xb6,0x3a,0x3e,0x1d,0xfa,0x6f,0x0f,0xaf,0x4f,
    0xf0,0xef,0xd3,0x81,0x85,0x84,0x88,0x86,0x7e,0x9c,0x4c,0x64,0x64,0x84,0x2b,0xfc,0xe3,0xa4,0x24,0xa4,0xe0,0xdf,0xe7,0x1f,
    0x1d,0x27,0x25,0x03,0xff,0x38,0xd8,0xb9,0x19,0x0f,0xff,0xf9,0xc4,0x42,0xc6,0xc4,0xff,0xc0,0xce,0x05,0xf6,0x16,0x05,0xfe,
    0x06,0x6f,0x0e,0x4f,0x8e,0xef,0xf0,0x59,0x6d,0x68,0x72,0x3e,0xd9,0x1f,0xff,0x02,0x00,0x01,0x20,0x7c,0x3a,0x29,0x4c,0xf7,
    0xa5,0x34,0x0c,0x3d,0xb5,0xae,0x3a,0x1d,0xf5,0x6f,0xc3,0xb0,0x30,0x33,0x82,0x03,0xa0,0x60,0x66,0xbf,0x01,0xc0,0x04,0x5b,
    0x07,0x04,0x16,0x10,0xf3,0xf2,0xe1,0x94,0x17,0x81,0x82,0xd1,0x54,0x40,0xf0,0x7e,0xb4,0x2d,0x16,0x2d,0x83,0x82,0x0a,0xb8,
    0x0e,0x08,0x2c,0x11,0xe7,0xf5,0xc5,0x02,0x10,0x96,0xb7,0x8b,0x45,0x91,0x60,0xc0,0x83,0x89,0x60,0xa8,0x07,0x2b,0xf9,0x70,
    0x1c,0x10,0x65,0xb0,0x70,0x41,0xa0,0x8f,0x3f,0xae,0x28,0x18,0x10,0x91,0xf8,0x19,0x54,0x81,0xf1,0x20,0xc0,0x84,0x8f,0xc0,
    0xcf,0x90,0x3d,0x6c,0x1c,0x10,0x55,0xa0,0x70,0x41,0x7f,0xcf,0x7f,0xf8,0x90,0x60,0x43,0x0b,0xc1,0x85,0xfb,0x80,0xc4,0x81,
    0x8a,0xe1,0x81,0x81,0x0c,0x56,0x0c,0x2f,0xd0,0x31,0x40,0x62,0xb5,0x80,0x70,0x41,0x55,0xc1,0xc1,0x05,0x7e,0xff,0x5c,0x18,
    0x30,0x21,0xbf,0x06,0x17,0xe8,0x18,0xa0,0x31,0x5b,0xec,0x06,0x00,0x34,0x18,0x6f,0xbf,0xa0,0x0c,0x95,0x01,0xc1,0x04,0x51,
    0x07,0x04,0x11,0xf4,0xf4,0x0f,0x77,0x06,0x04,0x34,0x18,0x6f,0xc5,0x60,0xaa,0x07,0x34,0x09,0xd0,0x18,0x10,0xef,0x03,0x0b,
    0xf8,0x5e,0x0a,0xa0,0x73,0x40,0x93,0xc7,0x8e,0x4d,0x51,0xe5,0xdc,0xf9,0x03,0xdc,0x01,0x81,0x0e,0x54,0x0a,0x71,0xf8,0x11,
    0x82,0xc6,0x98,0x0c,0x00,0x69,0x72,0xe5,0xe0,0xe4,0xff,0x25,0x67,0x80,0xcf,0x90,0xb9,0x1f,0x15,0x01,0x9f,0x21,0x73,0xe4,
    0x17,0x63,0x06,0x04,0x34,0x7c,0x05,0xc0,0x8b,0xd7,0x41,0x81,0x0c,0x12,0x00,0xc0,0x2a,0xfe,0x2d,0x42,0x03,0x80,0x66,0x21,
    0x71,0xf0,0x1c,0x03,0x30,0x6a,0xfb,0x05,0xd5,0xc1,0x81,0x0c,0x12,0x01,0x4c,0x0a,0xbf,0x8b,0x54,0xa0,0x06,0x09,0x00,0xa6,
    0x05,0x5f,0xc5,0xa7,0x40,0xa2,0x03,0x2e,0x38,0xd2,0xe2,0x77,0xe8,0x10,0x00,0x09,0x43,0xa6,0x72,0xbf,0x3c,0xea,0x0a,0x1c,
    0x60,0xb4,0x54,0xee,0x5e,0x6a,0x16,0x21,0xcf,0xb4,0x75,0x06,0x07,0xdb,0xec,0x14,0x0b,0x1d,0xc1,0x81,0xf4,0x1f,0x87,0xc2,
    0xc8,0x98,0xe8,0x37,0xfa,0x3d,0x83,0x03,0xe6,0x3f,0x27,0x7f,0x06,0x07,0xd4,0x4b,0x03,0x25,0x62,0xd9,0x19,0x38,0x27,0xfe,
    0xfe,0x0c,0x0f,0xb0,0xfc,0x0c,0xd4,0x3f,0x16,0xbc,0x41,0x24,0xbf,0x04,0x7a,0x04,0x7e,0x2d,0x90,0x74,0x2c,0xfc,0xcc,0x4f,
    0x28,0xc6,0x09,0xfd,0x6e,0x80,0xa1,0x2f,0x03,0x34,0x08,0xfc,0x5a,0xe2,0x07,0xd5,0x81,0x9a,0x85,0xf0,0x60,0x7c,0x32,0x82,
    0x06,0x07,0xe4,0x32,0xff,0x77,0xff,0x6f,0x53,0xf2,0x76,0xdd,0x7c,0x08,0x30,0x3e,0xe0,0x15,0xfe,0x0c,0x0f,0xaa,0xa0,0xc1,
    0xf3,0xff,0x2d,0x6d,0x0f,0xc0,0xc0,0xfa,0x2a,0x0c,0x1f,0x81,0x81,0xf3,0x54,0x20,0xb9,0xee,0x07,0x4c,0x86,0x18,0xdf,0x01,
    0x81,0xf3,0xf0,0x9d,0xf0,0x18,0x1f,0x30,0xc9,0xe6,0x0b,0x2c,0x0b,0xaf,0x40,0xc0,0xf9,0x86,0x4f,0x00,0xc0,0xf9,0xc2,0x67,
    0x98,0x3c,0xac,0xaa,0xba,0x83,0x03,0xe5,0x16,0x01,0xce,0x20,0xe0,0xf9,0xbd,0x41,0xed,0x45,0x2c,0xc3,0x68,0x38,0x3e,0x6d,
    0x00,0xe0,0xfa,0x3d,0xc1,0x00,0x00,0x4c,0x1d,0xab,0xda,0xb3,0x51,0x2f,0xaa,0xcd,0x45,0xc5,0x39,0xf1,0xf3,0x8c,0x90,0x8e,
    0x92,0x86,0x7f,0x9c,0xa4,0xbc,0xb4,0xcc,0x1f,0xfa,0xb9,0x48,0xf1,0xd0,0x8b,0xb8,0x9b,0x94,0xb6,0x51,0x84,0xbb,0xa6,0x2c,
    0x52,0xaf,0xde,0xad,0x58,0x2b,0x82,0x32,0xfe,0xb9,0x50,0x60,0x81,0xc1,0x86,0xfb,0x06,0x27,0xf5,0x50,0x39,0xa0,0x93,0x20,
    0xc1,0x04,0x01,0x81,0xf8,0x31,0x3f,0x7e,0x07,0x34,0x12,0x4a,0x84,0x21,0xf6,0x88,0xa0,0x46,0x0b,0x25,0xc1,0x81,0x08,0x2e,
    0xe8,0x31,0xbf,0xc3,0x60,0x95,0xe5,0x1f,0x90,0x40,0x73,0x40,0xa0,0xd7,0x35,0xa1,0x00,0x18,0x50,0x42,0xf0,0x62,0x7e,0x81,
    0xce,0x03,0x9a,0x52,0x01,0xc0,0xc2,0x83,0x2b,0x06,0x27,0xe2,0x83,0x9a,0x05,0x32,0x0c,0x08,0x41,0x72,0xd7,0x51,0xa4,0x2d,
    0x60,0x59,0x36,0x0c,0x08,0x38,0x94,0xb1,0x6f,0xfa,0xca,0xed,0x62,0x33,0x6f,0x18,0x18,0x42,0xae,0x6c,0x7a,0x01,0x80,0xc2,
    0x83,0xfc,0x18,0x9f,0x7f,0x8b,0x26,0x47,0xa0,0x80,0x0c,0x28,0x47,0xc1,0x89,0xf7,0x56,0x2c,0x9d,0x05,0x00,0x91,0xa2,0x35,
    0x2d,0x1c,0xf0,0x5d,0x3a,0x0a,0x01,0x2b,0xa3,0xaa,0x04,0x41,0x3d,0xdc,0x2f,0x85,0x5c,0xb0,0x21,0x80,0x60,0x30,0xa1,0x00,
    0xc5,0x7c,0x86,0x72,0x80,0xc0,0xfc,0x84,0x00,0x61,0x42,0x3c,0x0c,0x4f,0xd8,0x67,0x36,0x0c,0x08,0x38,0xfb,0x04,0x60,0x55,
    0x83,0x45,0xfe,0x6a,0x5c,0x14,0x02,0x50,0x29,0x86,0x6e,0xfc,0x5c,0x5b,0x0c,0x79,0x10,0x60,0x7e,0x02,0x10,0x30,0xa1,0x05,
    0xc0,0x45,0xf1,0xa0,0xc0,0xfb,0x89,0x00,0xc2,0x84,0x03,0x80,0x0d,0x25,0x04,0xa1,0xf0,0x29,0x86,0x31,0xc0,0x84,0x3e,0x05,
    0x34,0x06,0xb0,0x04,0xd3,0xb4,0x36,0x85,0xcb,0x88,0x06,0x07,0xdc,0x4a,0x06,0x14,0x20,0x21,0x20,0x15,0xc2,0x03,0x03,0xec,
    0x3e,0x06,0x14,0x20,0x20,0x01,0x01,0x5c,0x40,0x1e,0x1f,0x02,0x9b,0xc8,0x01,0x7d,0x07,0x15,0x0f,0x81,0x4d,0xe0,0x20,0x0b,
    0x37,0x74,0x25,0xa0,0xb2,0x86,0x5b,0xf8,0x30,0x3e,0xc5,0xc0,0xc2,0x83,0x84,0x00,0x62,0x40,0xc5,0x6f,0x10,0x03,0x7c,0x0c,
    0x28,0x28,0x91,0x01,0xc8,0x04,0x7e,0x50,0x5c,0x0a,0x65,0x48,0x1c,0xeb,0x15,0x17,0x01,0x8f,0x0d,0x1e,0x11,0xfd,0x08,0xae,
    0x18,0x00,0x02,0x70,0xe5,0x5f,0x95,0x9a,0x8e,0x7e,0x56,0x23,0x3e,0x42,0x31,0xed,0xf3,0x90,0x94,0x92,0x96,0x87,0x7f,0x9c,
    0xc4,0xdc,0xdc,0xf4,0x1f,0xfa,0x79,0xa9,0x78,0x0c,0x1f,0xd9,0x72,0x32,0xaa,0x1f,0x32,0x6a,0x76,0x7e,0xfe,0x79,0x59,0x75,
    0xb7,0x83,0xb0,0x62,0xc0,0x9f,0x03,0x0d,0xc0,0xbe,0x19,0x91,0xbc,0x05,0xd4,0x6e,0xf3,0xe3,0x90,0x46,0x25,0x9a,0x06,0x07,
    0xe0,0x7c,0x06,0x4b,0x28,0x34,0xdf,0xef,0xa2,0xa1,0x05,0x56,0x03,0x07,0xf9,0x00,0x82,0xb4,0x8c,0x26,0x1c,0x12,0xbc,0x23,
    0xa5,0xa3,0x98,0x8c,0x9a,0x14,0x16,0x2f,0x92,0xad,0x25,0xa0,0x5c,0x3e,0xc4,0x13,0x63,0x6f,0x97,0x05,0x28,0xdb,0x8d,0x08,
    0x48,0xcf,0x53,0x03,0x03,0xef,0xe1,0xd0,0x28,0xe0,0x31,0x5f,0x8b,0x32,0x8d,0xb4,0xfd,0x68,0xfd,0x58,0x30,0x3e,0xea,0x94,
    0xe8,0xf6,0x25,0xc5,0x69,0x98,0xc4,0xa1,0x83,0xc6,0x21,0xe2,0xc2,0xb8,0x60,0x68,0x80,0x5c,0xf9,0x75,0x15,0x32,0x05,0xb4,
    0x11,0x74,0x9a,0x55,0xa8,0x21,0x02,0x30,0x13,0x24,0xac,0x06,0x07,0xd9,0x50,0x19,0x2c,0xa0,0xd2,0xff,0x9f,0xae,0x06,0x07,
    0xd5,0x56,0x08,0xe0,0xaa,0x4a,0x0a,0xe3,0x4e,0xa8,0x83,0x96,0x0f,0x43,0x83,0x43,0xfe,0xf5,0xc9,0x83,0x5a,0x03,0x18,0x07,
    0x08,0x0a,0x44,0x1b,0x50,0x12,0x5a,0xe0,0x60,0x7d,0x4b,0x97,0xf8,0xd2,0xb0,0x18,0x1f,0x42,0xe5,0xfe,0x34,0x75,0x0a,0xe2,
    0x3c,0x80,0x3e,0x7f,0xf1,0x80,0xd9,0x80,0x9d,0xac,0x06,0x07,0xd1,0x50,0x17,0x07,0xcf,0xfe,0xa4,0x18,0x1f,0x45,0x40,0x5c,
    0x23,0x76,0x88,0xdc,0x3c,0x18,0x70,0x52,0x20,0x9a,0xa7,0x06,0x07,0xcd,0x52,0x70,0x8e,0x90,0x18,0x1f,0x32,0xe0,0x2e,0x58,
    0x39,0x20,0x76,0x88,0x3b,0xf1,0xf7,0x7d,0x04,0x89,0xf0,0x60,0x7c,0x8b,0x81,0x52,0x82,0x03,0x1b,0xfc,0x1d,0x12,0xcc,0x03,
    0x03,0xe3,0xe5,0x83,0x37,0x88,0x70,0xe0,0x56,0x00,0x01,0x40,0x73,0x4f,0xbf,0x11,0x9f,0x22,0xfe,0xc5,0x22,0x8f,0x20,0x8d,
    0x7b,0x7e,0x65,0x26,0x64,0x78,0x4d,0x2d,0x6c,0x26,0x87,0x7f,0xf4,0x0f,0xa7,0x7c,0xe5,0xb1,0x68,0x5a,0x2d,0xa2,0xdf,0x01,
    0x82,0xd7,0xc1,0x41,0x7a,0xf7,0xdd,0x7f,0xad,0xae,0x2e,0xa3,0x57,0x7c,0x06,0x23,0x40,0x5f,0x89,0x9b,0x46,0x43,0x6b,0xfc,
    0xff,0x32,0x44,0x9a,0x53,0x5e,0xfb,0x0b,0xf2,0x6e,0x03,0x8b,0xfd,0xa3,0x6c,0x07,0x17,0xfb,0x1a,0xaa,0x76,0x60,0xf1,0xcf,
    0x10,0xff,0x5b,0x60,0x38,0xbf,0xdb,0x3b,0x17,0x18,0xe7,0x8f,0x1d,0xa4,0x3c,0x0c,0x9d,0x22,0x3e,0xd7,0x00,0xc9,0x63,0xf7,
    0x38,0xb8,0xc7,0x4c,0x78,0xed,0x31,0xe3,0x9d,0x22,0x3e,0xe7,0x17,0x19,0x6d,0xec,0x27,0xd0,0x8a,0xbc,0x3b,0xe3,0x05,0x2e,
    0xd2,0x51,0xe2,0x67,0x58,0x8f,0xb5,0xb0,0x97,0x63,0x61,0x2e,0x82,0x88,0x4d,0x9a,0xa3,0xc4,0xce,0x91,0x3f,0x57,0x61,0x2e,
    0x96,0xc2,0x5c,0x95,0x10,0x9b,0x13,0x21,0x33,0xa4,0x4f,0xcd,0x9f,0x84,0xd8,0x92,0xc5,0x86,0x77,0xd2,0x3a,0xc7,0xc2,0x32,
    0x38,0x2f,0x73,0x89};

    // Expected results
    H263Info infoRefQCIF_N;
    infoRefQCIF_N.uiH263PTypeFmt = 2;
    infoRefQCIF_N.codecBits      = 0;
    infoRefQCIF_N.pQuant         = 7;
    infoRefQCIF_N.fType          = 0;
    infoRefQCIF_N.cpmBit         = 0;
    infoRefQCIF_N.numOfGOBs      = 9;
    infoRefQCIF_N.totalNumOfMBs  = 11*9;

    infoRefQCIF_N.ptrGOBbuffer[0] = 0;    infoRefQCIF_N.ptrGOBbufferSBit[0] = 0; infoRefQCIF_N.ptrGQuant[0] = 0;
    infoRefQCIF_N.ptrGOBbuffer[1] = 215;  infoRefQCIF_N.ptrGOBbufferSBit[1] = 2; infoRefQCIF_N.ptrGQuant[1] = 7;
    infoRefQCIF_N.ptrGOBbuffer[2] = 456;  infoRefQCIF_N.ptrGOBbufferSBit[2] = 1; infoRefQCIF_N.ptrGQuant[2] = 7;
    infoRefQCIF_N.ptrGOBbuffer[3] = 535;  infoRefQCIF_N.ptrGOBbufferSBit[3] = 5; infoRefQCIF_N.ptrGQuant[3] = 7;
    infoRefQCIF_N.ptrGOBbuffer[4] = 615;  infoRefQCIF_N.ptrGOBbufferSBit[4] = 7; infoRefQCIF_N.ptrGQuant[4] = 7;
    infoRefQCIF_N.ptrGOBbuffer[5] = 925;  infoRefQCIF_N.ptrGOBbufferSBit[5] = 4; infoRefQCIF_N.ptrGQuant[5] = 7;
    infoRefQCIF_N.ptrGOBbuffer[6] = 1133; infoRefQCIF_N.ptrGOBbufferSBit[6] = 1; infoRefQCIF_N.ptrGQuant[6] = 7;
    infoRefQCIF_N.ptrGOBbuffer[7] = 1512; infoRefQCIF_N.ptrGOBbufferSBit[7] = 6; infoRefQCIF_N.ptrGQuant[7] = 7;
    infoRefQCIF_N.ptrGOBbuffer[8] = 1832; infoRefQCIF_N.ptrGOBbufferSBit[8] = 7; infoRefQCIF_N.ptrGQuant[8] = 7;

    // --------------------------------------------------
    // Input data - H.263 encoded stream CIF (I-frame)
    // --------------------------------------------------

    FILE* openFile = fopen("H263Foreman_CIF_Iframe.bin", "rb");

    fseek(openFile, 0, SEEK_END);
    int lengthRefCIF_I = ftell(openFile);
    fseek(openFile, 0, SEEK_SET);

    unsigned char* encodedStreamCIF_I = new unsigned char[lengthRefCIF_I];
    fread(encodedStreamCIF_I, 1, lengthRefCIF_I, openFile);
    fclose(openFile);

    // Expected results
    H263Info infoRefCIF_I;
    infoRefCIF_I.uiH263PTypeFmt = 3;
    infoRefCIF_I.codecBits      = 0;
    infoRefCIF_I.pQuant         = 5;
    infoRefCIF_I.fType          = 0;
    infoRefCIF_I.cpmBit         = 0;
    infoRefCIF_I.numOfGOBs      = 18;
    infoRefCIF_I.totalNumOfMBs  = 22*18;

    infoRefCIF_I.ptrGOBbuffer[0]  = 0;     infoRefCIF_I.ptrGOBbufferSBit[0]  = 0; infoRefCIF_I.ptrGQuant[0]  = 0;
    infoRefCIF_I.ptrGOBbuffer[1]  = 1607;  infoRefCIF_I.ptrGOBbufferSBit[1]  = 0; infoRefCIF_I.ptrGQuant[1]  = 5;
    infoRefCIF_I.ptrGOBbuffer[2]  = 2759;  infoRefCIF_I.ptrGOBbufferSBit[2]  = 0; infoRefCIF_I.ptrGQuant[2]  = 5;
    infoRefCIF_I.ptrGOBbuffer[3]  = 3699;  infoRefCIF_I.ptrGOBbufferSBit[3]  = 0; infoRefCIF_I.ptrGQuant[3]  = 5;
    infoRefCIF_I.ptrGOBbuffer[4]  = 4506;  infoRefCIF_I.ptrGOBbufferSBit[4]  = 0; infoRefCIF_I.ptrGQuant[4]  = 5;
    infoRefCIF_I.ptrGOBbuffer[5]  = 5260;  infoRefCIF_I.ptrGOBbufferSBit[5]  = 0; infoRefCIF_I.ptrGQuant[5]  = 5;
    infoRefCIF_I.ptrGOBbuffer[6]  = 6254;  infoRefCIF_I.ptrGOBbufferSBit[6]  = 0; infoRefCIF_I.ptrGQuant[6]  = 5;
    infoRefCIF_I.ptrGOBbuffer[7]  = 7117;  infoRefCIF_I.ptrGOBbufferSBit[7]  = 0; infoRefCIF_I.ptrGQuant[7]  = 5;
    infoRefCIF_I.ptrGOBbuffer[8]  = 7804;  infoRefCIF_I.ptrGOBbufferSBit[8]  = 0; infoRefCIF_I.ptrGQuant[8]  = 5;
    infoRefCIF_I.ptrGOBbuffer[9]  = 8600;  infoRefCIF_I.ptrGOBbufferSBit[9]  = 0; infoRefCIF_I.ptrGQuant[9]  = 5;
    infoRefCIF_I.ptrGOBbuffer[10]  = 9419; infoRefCIF_I.ptrGOBbufferSBit[10] = 0; infoRefCIF_I.ptrGQuant[10] = 5;
    infoRefCIF_I.ptrGOBbuffer[11] = 10126; infoRefCIF_I.ptrGOBbufferSBit[11] = 0; infoRefCIF_I.ptrGQuant[11] = 5;
    infoRefCIF_I.ptrGOBbuffer[12] = 10723; infoRefCIF_I.ptrGOBbufferSBit[12] = 0; infoRefCIF_I.ptrGQuant[12] = 5;
    infoRefCIF_I.ptrGOBbuffer[13] = 11280; infoRefCIF_I.ptrGOBbufferSBit[13] = 0; infoRefCIF_I.ptrGQuant[13] = 5;
    infoRefCIF_I.ptrGOBbuffer[14] = 11910; infoRefCIF_I.ptrGOBbufferSBit[14] = 0; infoRefCIF_I.ptrGQuant[14] = 5;
    infoRefCIF_I.ptrGOBbuffer[15] = 12430; infoRefCIF_I.ptrGOBbufferSBit[15] = 0; infoRefCIF_I.ptrGQuant[15] = 5;
    infoRefCIF_I.ptrGOBbuffer[16] = 12925; infoRefCIF_I.ptrGOBbufferSBit[16] = 0; infoRefCIF_I.ptrGQuant[16] = 5;
    infoRefCIF_I.ptrGOBbuffer[17] = 13506; infoRefCIF_I.ptrGOBbufferSBit[17] = 0; infoRefCIF_I.ptrGQuant[17] = 5;

    // --------------------------------------------------
    // Input data - H.263 encoded stream CIF (P-frame)
    // --------------------------------------------------

    openFile = fopen("H263Foreman_CIF_Pframe.bin", "rb");

    fseek(openFile, 0, SEEK_END);
    int lengthRefCIF_P = ftell(openFile);
    fseek(openFile, 0, SEEK_SET);

    unsigned char* encodedStreamCIF_P = new unsigned char[lengthRefCIF_P];
    fread(encodedStreamCIF_P, 1, lengthRefCIF_P, openFile);
    fclose(openFile);

    // Expected results
    H263Info infoRefCIF_P;
    infoRefCIF_P.uiH263PTypeFmt = 3;
    infoRefCIF_P.codecBits      = 8;
    infoRefCIF_P.pQuant         = 4;
    infoRefCIF_P.fType          = 1;
    infoRefCIF_P.cpmBit         = 0;
    infoRefCIF_P.numOfGOBs      = 18;
    infoRefCIF_P.totalNumOfMBs  = 22*18;

    infoRefCIF_P.ptrGOBbuffer[0]  = 0;    infoRefCIF_P.ptrGOBbufferSBit[0]  = 0; infoRefCIF_P.ptrGQuant[0]  = 0;
    infoRefCIF_P.ptrGOBbuffer[1]  = 252;  infoRefCIF_P.ptrGOBbufferSBit[1]  = 0; infoRefCIF_P.ptrGQuant[1]  = 5;
    infoRefCIF_P.ptrGOBbuffer[2]  = 482;  infoRefCIF_P.ptrGOBbufferSBit[2]  = 0; infoRefCIF_P.ptrGQuant[2]  = 6;
    infoRefCIF_P.ptrGOBbuffer[3]  = 581;  infoRefCIF_P.ptrGOBbufferSBit[3]  = 0; infoRefCIF_P.ptrGQuant[3]  = 6;
    infoRefCIF_P.ptrGOBbuffer[4]  = 676;  infoRefCIF_P.ptrGOBbufferSBit[4]  = 0; infoRefCIF_P.ptrGQuant[4]  = 7;
    infoRefCIF_P.ptrGOBbuffer[5]  = 756;  infoRefCIF_P.ptrGOBbufferSBit[5]  = 0; infoRefCIF_P.ptrGQuant[5]  = 7;
    infoRefCIF_P.ptrGOBbuffer[6]  = 855;  infoRefCIF_P.ptrGOBbufferSBit[6]  = 0; infoRefCIF_P.ptrGQuant[6]  = 8;
    infoRefCIF_P.ptrGOBbuffer[7]  = 949;  infoRefCIF_P.ptrGOBbufferSBit[7]  = 0; infoRefCIF_P.ptrGQuant[7]  = 9;
    infoRefCIF_P.ptrGOBbuffer[8]  = 1004; infoRefCIF_P.ptrGOBbufferSBit[8]  = 0; infoRefCIF_P.ptrGQuant[8]  = 10;
    infoRefCIF_P.ptrGOBbuffer[9]  = 1062; infoRefCIF_P.ptrGOBbufferSBit[9]  = 0; infoRefCIF_P.ptrGQuant[9]  = 11;
    infoRefCIF_P.ptrGOBbuffer[10] = 1115; infoRefCIF_P.ptrGOBbufferSBit[10] = 0; infoRefCIF_P.ptrGQuant[10] = 11;
    infoRefCIF_P.ptrGOBbuffer[11] = 1152; infoRefCIF_P.ptrGOBbufferSBit[11] = 0; infoRefCIF_P.ptrGQuant[11] = 13;
    infoRefCIF_P.ptrGOBbuffer[12] = 1183; infoRefCIF_P.ptrGOBbufferSBit[12] = 0; infoRefCIF_P.ptrGQuant[12] = 14;
    infoRefCIF_P.ptrGOBbuffer[13] = 1214; infoRefCIF_P.ptrGOBbufferSBit[13] = 0; infoRefCIF_P.ptrGQuant[13] = 15;
    infoRefCIF_P.ptrGOBbuffer[14] = 1257; infoRefCIF_P.ptrGOBbufferSBit[14] = 0; infoRefCIF_P.ptrGQuant[14] = 16;
    infoRefCIF_P.ptrGOBbuffer[15] = 1286; infoRefCIF_P.ptrGOBbufferSBit[15] = 0; infoRefCIF_P.ptrGQuant[15] = 16;
    infoRefCIF_P.ptrGOBbuffer[16] = 1321; infoRefCIF_P.ptrGOBbufferSBit[16] = 0; infoRefCIF_P.ptrGQuant[16] = 16;
    infoRefCIF_P.ptrGOBbuffer[17] = 1352; infoRefCIF_P.ptrGOBbufferSBit[17] = 0; infoRefCIF_P.ptrGQuant[17] = 14;

    //---------------------------------------------------------------
    //---------------------------------------------------------------
    //---------------------------------------------------------------
    // Start test
    const H263Info* ptrInfoSQCIF = NULL;
    const H263MBInfo* ptrMBInfoSQCIF = NULL;
    const H263Info* ptrInfoQCIF = NULL;
    const H263MBInfo* ptrMBInfoQCIF = NULL;
    const H263Info* ptrInfoCIF = NULL;
    const H263MBInfo* ptrMBInfoCIF = NULL;
    const H263Info* ptrInfoQCIF_N = NULL;
    const H263MBInfo* ptrMBInfoQCIF_N = NULL;
    const H263Info* ptrInfoCIF_I = NULL;
    const H263MBInfo* ptrMBInfoCIF_I = NULL;
    const H263Info* ptrInfoCIF_P = NULL;
    const H263MBInfo* ptrMBInfoCIF_P = NULL;
    H263Information h263Information;

    // Input buffer
    const int length = 3000;
    unsigned char* encodedBuffer = new unsigned char[lengthRefCIF_P];

    // Test invalid inputs
    assert(-1 == h263Information.GetInfo(NULL, length, ptrInfoSQCIF));
    assert(-1 == h263Information.GetInfo(encodedBuffer, 0, ptrInfoSQCIF));
    assert(-1 == h263Information.GetInfo(encodedBuffer, length, ptrInfoSQCIF)); // invalid H.263 stream
//    assert(-1 == h263Information.GetInfo(encodedStreamSQCIF, lengthRefSQCIF/2, ptrInfoSQCIF)); // invalid H.263 stream

    assert(-1 == h263Information.GetMBInfo(NULL, length, 0, ptrMBInfoSQCIF));
    assert(-1 == h263Information.GetMBInfo(encodedBuffer, 0, 0, ptrMBInfoSQCIF));
    assert(-1 == h263Information.GetMBInfo(encodedBuffer, length, 0, ptrMBInfoSQCIF));
    assert(-1 == h263Information.GetMBInfo(encodedBuffer, length, -1, ptrMBInfoSQCIF)); // incorrect group number
    assert(-1 == h263Information.GetMBInfo(encodedBuffer, length, 8, ptrMBInfoSQCIF));  // incorrect group number

    // ----------------------------------------------
    // Get info from encoded H.263 stream - SQCIF
    // ----------------------------------------------
    h263Information.Reset();
    assert(0 == h263Information.GetInfo(encodedStreamSQCIF, lengthRefSQCIF, ptrInfoSQCIF));
    ValidateResults(ptrInfoSQCIF, &infoRefSQCIF);

    // Get MB info
    for (int i = 0; i < ptrInfoSQCIF->numOfGOBs; i++)
    {
        assert(0 == h263Information.GetMBInfo(encodedStreamSQCIF, lengthRefSQCIF, i, ptrMBInfoSQCIF));
    }
    ValidateMBResults(ptrInfoSQCIF, ptrMBInfoSQCIF, &infoRefSQCIF);

    // ---------------------------------------------
    // Get info from encoded H.263 stream - QCIF
    // ---------------------------------------------
    h263Information.Reset();
    assert(0 == h263Information.GetInfo(encodedStreamQCIF, lengthRefQCIF, ptrInfoQCIF));
    ValidateResults(ptrInfoQCIF, &infoRefQCIF);

    // Get MB info
    for (int i = 0; i < ptrInfoQCIF->numOfGOBs; i++)
    {
        assert(0 == h263Information.GetMBInfo(encodedStreamQCIF, lengthRefQCIF, i, ptrMBInfoQCIF));
    }
    ValidateMBResults(ptrInfoQCIF, ptrMBInfoQCIF, &infoRefQCIF);

    // --------------------------------------------
    // Get info from encoded H.263 stream - CIF
    // --------------------------------------------
    h263Information.Reset();
    assert(0 == h263Information.GetInfo(encodedStreamCIF, lengthRefCIF, ptrInfoCIF));
    ValidateResults(ptrInfoCIF, &infoRefCIF);

    // Get MB info
    for (int i = 0; i < ptrInfoCIF->numOfGOBs; i++)
    {
        assert(0 == h263Information.GetMBInfo(encodedStreamCIF, lengthRefCIF, i, ptrMBInfoCIF));
    }
    ValidateMBResults(ptrInfoCIF, ptrMBInfoCIF, &infoRefCIF);

    // ----------------------------------------------------------------------
    // Get info from encoded H.263 stream - QCIF - non byte aligned GOBs
    // ----------------------------------------------------------------------
    h263Information.Reset();
    assert(0 == h263Information.GetInfo(encodedStreamQCIF_N, lengthRefQCIF_N, ptrInfoQCIF_N));
    ValidateResults(ptrInfoQCIF_N, &infoRefQCIF_N);

    // Get MB info
    for (int i = 0; i < ptrInfoQCIF_N->numOfGOBs; i++)
    {
        assert(0 == h263Information.GetMBInfo(encodedStreamQCIF_N, lengthRefQCIF_N, i, ptrMBInfoQCIF_N));
    }
    ValidateMBResults(ptrInfoQCIF_N, ptrMBInfoQCIF_N, &infoRefQCIF_N);

    // -------------------------------------------
    // Get info from encoded H.263 stream - CIF
    // -------------------------------------------
    h263Information.Reset();
    assert(0 == h263Information.GetInfo(encodedStreamCIF_I, lengthRefCIF_I, ptrInfoCIF_I));
    ValidateResults(ptrInfoCIF_I, &infoRefCIF_I);

    // Get MB info
    unsigned int start = timeGetTime();
    for (int i = 0; i < ptrInfoCIF_I->numOfGOBs; i++)
    {
        assert(0 == h263Information.GetMBInfo(encodedStreamCIF_I, lengthRefCIF_I, i, ptrMBInfoCIF_I));
    }
    unsigned int endMB = timeGetTime();
    ValidateMBResults(ptrInfoCIF_I, ptrMBInfoCIF_I, &infoRefCIF_I, false);

    std::cout << "I-frame, length: " << lengthRefCIF_I << " bytes. Time: " << endMB - start << " ms." << std::endl;
    PRINT_LINE;

    // -------------------------------------------
    // Get info from encoded H.263 stream - CIF
    // -------------------------------------------
    h263Information.Reset();
    assert(0 == h263Information.GetInfo(encodedStreamCIF_P, lengthRefCIF_P, ptrInfoCIF_P));
    ValidateResults(ptrInfoCIF_P, &infoRefCIF_P);

    // Get MB info
    start = timeGetTime();
    for (int i = 0; i < ptrInfoCIF_P->numOfGOBs; i++)
    {
        assert(0 == h263Information.GetMBInfo(encodedStreamCIF_P, lengthRefCIF_P, i, ptrMBInfoCIF_P));
    }
    endMB = timeGetTime();
    ValidateMBResults(ptrInfoCIF_P, ptrMBInfoCIF_P, &infoRefCIF_P, false);

    std::cout << "P-frame, length:  " << lengthRefCIF_P << " bytes. Time: " << endMB - start << " ms." << std::endl;
    PRINT_LINE;

    delete [] encodedStreamCIF_I;
    delete [] encodedStreamCIF_P;
    delete [] encodedBuffer;

    TEST_PASSED();
    ::Sleep(5000);
    return 0;
}

