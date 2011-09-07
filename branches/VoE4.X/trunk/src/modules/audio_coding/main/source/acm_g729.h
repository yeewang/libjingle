/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef ACM_G729_H
#define ACM_G729_H

#include "acm_generic_codec.h"

// forward declaration
struct G729_encinst_t_;
struct G729_decinst_t_;

namespace webrtc {

class ACMG729 : public ACMGenericCodec
{
public:
    ACMG729(WebRtc_Word16 codecID);
    ~ACMG729();
    // for FEC
    ACMGenericCodec* CreateInstance(void);

    WebRtc_Word16 InternalEncode(
        WebRtc_UWord8* bitstream,
        WebRtc_Word16* bitStreamLenByte);

    WebRtc_Word16 InternalInitEncoder(
        WebRtcACMCodecParams *codecParams);

    WebRtc_Word16 InternalInitDecoder(
        WebRtcACMCodecParams *codecParams);

protected:
    WebRtc_Word16 DecodeSafe(
        WebRtc_UWord8* bitStream,
        WebRtc_Word16  bitStreamLenByte, 
        WebRtc_Word16* audio, 
        WebRtc_Word16* audioSamples, 
        WebRtc_Word8*  speechType);

    WebRtc_Word32 CodecDef(
        WebRtcNetEQ_CodecDef& codecDef, 
        const CodecInst&      codecInst);

    void DestructEncoderSafe();
    
    void DestructDecoderSafe();

    WebRtc_Word16 InternalCreateEncoder();
    
    WebRtc_Word16 InternalCreateDecoder();
    
    void InternalDestructEncoderInst(
        void* ptrInst);

    WebRtc_Word16 EnableDTX();
    
    WebRtc_Word16 DisableDTX();

    WebRtc_Word32 ReplaceInternalDTXSafe(
        const bool replaceInternalDTX);

    WebRtc_Word32 IsInternalDTXReplacedSafe(
        bool* internalDTXReplaced);

    WebRtc_Word16 UnregisterFromNetEqSafe(
        ACMNetEQ*     netEq,
        WebRtc_Word16 payloadType);

    G729_encinst_t_* _encoderInstPtr;
    G729_decinst_t_* _decoderInstPtr;

};

} // namespace webrtc

#endif //ACM_G729_H

