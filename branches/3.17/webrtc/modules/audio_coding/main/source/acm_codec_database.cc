/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

/*
 * This file generates databases with information about all supported audio
 * codecs.
 */

// TODO(tlegrand): Change constant input pointers in all functions to constant
// references, where appropriate.
#include "acm_codec_database.h"

#include <stdio.h>

#include "acm_common_defs.h"
#include "trace.h"

// Includes needed to create the codecs.
// G.711, PCM mu-law and A-law.
#include "acm_pcma.h"
#include "acm_pcmu.h"
#include "g711_interface.h"
// CNG.
#include "acm_cng.h"
#include "webrtc_cng.h"
// NetEQ.
#include "webrtc_neteq.h"
#ifdef WEBRTC_CODEC_ISAC
    #include "acm_isac.h"
    #include "acm_isac_macros.h"
    #include "isac.h"
#endif
#ifdef WEBRTC_CODEC_ISACFX
    #include "acm_isac.h"
    #include "acm_isac_macros.h"
    #include "isacfix.h"
#endif
#ifdef WEBRTC_CODEC_PCM16
    #include "pcm16b.h"
    #include "acm_pcm16b.h"
#endif
#ifdef WEBRTC_CODEC_ILBC
    #include "acm_ilbc.h"
    #include "ilbc.h"
#endif
#ifdef WEBRTC_CODEC_AMR
    #include "acm_amr.h"
    #include "amr_interface.h"
#endif
#ifdef WEBRTC_CODEC_AMRWB
    #include "acm_amrwb.h"
    #include "amrwb_interface.h"
#endif
#ifdef WEBRTC_CODEC_CELT
    #include "acm_celt.h"
    #include "celt_interface.h"
#endif
#ifdef WEBRTC_CODEC_G722
    #include "acm_g722.h"
    #include "g722_interface.h"
#endif
#ifdef WEBRTC_CODEC_G722_1
    #include "acm_g7221.h"
    #include "g7221_interface.h"
#endif
#ifdef WEBRTC_CODEC_G722_1C
    #include "acm_g7221c.h"
    #include "g7221c_interface.h"
#endif
#ifdef WEBRTC_CODEC_G729
    #include "acm_g729.h"
    #include "g729_interface.h"
#endif
#ifdef WEBRTC_CODEC_G729_1
    #include "acm_g7291.h"
    #include "g7291_interface.h"
#endif
#ifdef WEBRTC_CODEC_GSMFR
    #include "acm_gsmfr.h"
    #include "gsmfr_interface.h"
#endif
#ifdef WEBRTC_CODEC_OPUS
    #include "acm_opus.h"
    #include "modules/audio_coding/codecs/opus/interface/opus_interface.h"
#endif
#ifdef WEBRTC_CODEC_SPEEX
    #include "acm_speex.h"
    #include "speex_interface.h"
#endif
#ifdef WEBRTC_CODEC_AVT
    #include "acm_dtmf_playout.h"
#endif
#ifdef WEBRTC_CODEC_RED
    #include "acm_red.h"
#endif

namespace webrtc {

// We dynamically allocate some of the dynamic payload types to the defined
// codecs. Note! There are a limited number of payload types. If more codecs
// are defined they will receive reserved fixed payload types (values 69-95).
const int kDynamicPayloadtypes[ACMCodecDB::kMaxNumCodecs] = {
  105, 107, 108, 109, 111, 112, 113, 114, 115, 116, 117, 121,
   92,  91,  90,  89,  88,  87,  86,  85,  84,  83,  82,  81,
   80,  79,  78,  77,  76,  75,  74,  73,  72,  71,  70,  69,
   68, 67
};

// Creates database with all supported codecs at compile time.
// Each entry needs the following parameters in the given order:
// payload type, name, sampling frequency, packet size in samples,
// number of channels, and default rate.
#if (defined(WEBRTC_CODEC_AMR) || defined(WEBRTC_CODEC_AMRWB) \
  || defined(WEBRTC_CODEC_CELT) || defined(WEBRTC_CODEC_G722_1) \
  || defined(WEBRTC_CODEC_G722_1C) || defined(WEBRTC_CODEC_G729_1) \
  || defined(WEBRTC_CODEC_PCM16) || defined(WEBRTC_CODEC_SPEEX))
static int count_database = 0;
#endif

const CodecInst ACMCodecDB::database_[] = {
#if (defined(WEBRTC_CODEC_ISAC) || defined(WEBRTC_CODEC_ISACFX))
  {103, "ISAC", 16000, kIsacPacSize480, 1, kIsacWbDefaultRate},
# if (defined(WEBRTC_CODEC_ISAC))
  {104, "ISAC", 32000, kIsacPacSize960, 1, kIsacSwbDefaultRate},
# endif
#endif
#ifdef WEBRTC_CODEC_PCM16
  // Mono
  {kDynamicPayloadtypes[count_database++], "L16", 8000, 80, 1, 128000},
  {kDynamicPayloadtypes[count_database++], "L16", 16000, 160, 1, 256000},
  {kDynamicPayloadtypes[count_database++], "L16", 32000, 320, 1, 512000},
  // Stereo
  {kDynamicPayloadtypes[count_database++], "L16", 8000, 80, 2, 128000},
  {kDynamicPayloadtypes[count_database++], "L16", 16000, 160, 2, 256000},
  {kDynamicPayloadtypes[count_database++], "L16", 32000, 320, 2, 512000},
#endif
  // G.711, PCM mu-law and A-law.
  // Mono
  {0, "PCMU", 8000, 160, 1, 64000},
  {8, "PCMA", 8000, 160, 1, 64000},
  // Stereo
  {110, "PCMU", 8000, 160, 2, 64000},
  {118, "PCMA", 8000, 160, 2, 64000},
#ifdef WEBRTC_CODEC_ILBC
  {102, "ILBC", 8000, 240, 1, 13300},
#endif
#ifdef WEBRTC_CODEC_AMR
  {kDynamicPayloadtypes[count_database++], "AMR", 8000, 160, 1, 12200},
#endif
#ifdef WEBRTC_CODEC_AMRWB
  {kDynamicPayloadtypes[count_database++], "AMR-WB", 16000, 320, 1, 20000},
#endif
#ifdef WEBRTC_CODEC_CELT
  // Mono
  {kDynamicPayloadtypes[count_database++], "CELT", 32000, 640, 1, 64000},
  // Stereo
  {kDynamicPayloadtypes[count_database++], "CELT", 32000, 640, 2, 64000},
#endif
#ifdef WEBRTC_CODEC_G722
  // Mono
  {9, "G722", 16000, 320, 1, 64000},
  // Stereo
  {119, "G722", 16000, 320, 2, 64000},
#endif
#ifdef WEBRTC_CODEC_G722_1
  {kDynamicPayloadtypes[count_database++], "G7221", 16000, 320, 1, 32000},
  {kDynamicPayloadtypes[count_database++], "G7221", 16000, 320, 1, 24000},
  {kDynamicPayloadtypes[count_database++], "G7221", 16000, 320, 1, 16000},
#endif
#ifdef WEBRTC_CODEC_G722_1C
  {kDynamicPayloadtypes[count_database++], "G7221", 32000, 640, 1, 48000},
  {kDynamicPayloadtypes[count_database++], "G7221", 32000, 640, 1, 32000},
  {kDynamicPayloadtypes[count_database++], "G7221", 32000, 640, 1, 24000},
#endif
#ifdef WEBRTC_CODEC_G729
  {18, "G729", 8000, 240, 1, 8000},
#endif
#ifdef WEBRTC_CODEC_G729_1
  {kDynamicPayloadtypes[count_database++], "G7291", 16000, 320, 1, 32000},
#endif
#ifdef WEBRTC_CODEC_GSMFR
  {3, "GSM", 8000, 160, 1, 13200},
#endif
#ifdef WEBRTC_CODEC_OPUS
  // Opus supports 48, 24, 16, 12, 8 kHz.
  {120, "opus", 48000, 960, 1, 32000},
#endif
#ifdef WEBRTC_CODEC_SPEEX
  {kDynamicPayloadtypes[count_database++], "speex", 8000, 160, 1, 11000},
  {kDynamicPayloadtypes[count_database++], "speex", 16000, 320, 1, 22000},
#endif
  // Comfort noise for four different sampling frequencies.
  {13, "CN", 8000, 240, 1, 0},
  {98, "CN", 16000, 480, 1, 0},
  {99, "CN", 32000, 960, 1, 0},
  {100, "CN", 48000, 1440, 1, 0},
#ifdef WEBRTC_CODEC_AVT
  {106, "telephone-event", 8000, 240, 1, 0},
#endif
#ifdef WEBRTC_CODEC_RED
  {127, "red", 8000, 0, 1, 0},
#endif
  // To prevent compile errors due to trailing commas.
  {-1, "Null", -1, -1, -1, -1}
};

// Create database with all codec settings at compile time.
// Each entry needs the following parameters in the given order:
// Number of allowed packet sizes, a vector with the allowed packet sizes,
// Basic block samples, max number of channels that are supported.
const ACMCodecDB::CodecSettings ACMCodecDB::codec_settings_[] = {
#if (defined(WEBRTC_CODEC_ISAC) || defined(WEBRTC_CODEC_ISACFX))
  {2, {kIsacPacSize480, kIsacPacSize960}, 0, 1},
# if (defined(WEBRTC_CODEC_ISAC))
  {1, {kIsacPacSize960}, 0, 1},
# endif
#endif
#ifdef WEBRTC_CODEC_PCM16
  // Mono
  {4, {80, 160, 240, 320}, 0, 2},
  {4, {160, 320, 480, 640}, 0, 2},
  {2, {320, 640}, 0, 2},
  // Stereo
  {4, {80, 160, 240, 320}, 0, 2},
  {4, {160, 320, 480, 640}, 0, 2},
  {2, {320, 640}, 0, 2},
#endif
  // G.711, PCM mu-law and A-law.
  // Mono
  {6, {80, 160, 240, 320, 400, 480}, 0, 2},
  {6, {80, 160, 240, 320, 400, 480}, 0, 2},
  // Stereo
  {6, {80, 160, 240, 320, 400, 480}, 0, 2},
  {6, {80, 160, 240, 320, 400, 480}, 0, 2},
#ifdef WEBRTC_CODEC_ILBC
  {4, {160, 240, 320, 480}, 0, 1},
#endif
#ifdef WEBRTC_CODEC_AMR
  {3, {160, 320, 480}, 0, 1},
#endif
#ifdef WEBRTC_CODEC_AMRWB
  {3, {320, 640, 960}, 0, 1},
#endif
#ifdef WEBRTC_CODEC_CELT
  // Mono
  {1, {640}, 0, 2},
  // Stereo
  {1, {640}, 0, 2},
#endif
#ifdef WEBRTC_CODEC_G722
  // Mono
  {6, {160, 320, 480, 640, 800, 960}, 0, 2},
  // Stereo
  {6, {160, 320, 480, 640, 800, 960}, 0, 2},
#endif
#ifdef WEBRTC_CODEC_G722_1
  {1, {320}, 320, 1},
  {1, {320}, 320, 1},
  {1, {320}, 320, 1},
#endif
#ifdef WEBRTC_CODEC_G722_1C
  {1, {640}, 640, 1},
  {1, {640}, 640, 1},
  {1, {640}, 640, 1},
#endif
#ifdef WEBRTC_CODEC_G729
  {6, {80, 160, 240, 320, 400, 480}, 0, 1},
#endif
#ifdef WEBRTC_CODEC_G729_1
  {3, {320, 640, 960}, 0, 1},
#endif
#ifdef WEBRTC_CODEC_GSMFR
  {3, {160, 320, 480}, 160, 1},
#endif
#ifdef WEBRTC_CODEC_OPUS
  // Opus supports frames shorter than 10ms,
  // but it doesn't help us to use them.
  {1, {960}, 0, 2},
#endif
#ifdef WEBRTC_CODEC_SPEEX
  {3, {160, 320, 480}, 0, 1},
  {3, {320, 640, 960}, 0, 1},
#endif
  // Comfort noise for three different sampling frequencies.
  {1, {240}, 240, 1},
  {1, {480}, 480, 1},
  {1, {960}, 960, 1},
  {1, {1440}, 1440, 1},
#ifdef WEBRTC_CODEC_AVT
  {1, {240}, 240, 1},
#endif
#ifdef WEBRTC_CODEC_RED
  {1, {0}, 0, 1},
#endif
  // To prevent compile errors due to trailing commas.
  {-1, {-1}, -1, -1}
};

// Create a database of all NetEQ decoders at compile time.
const WebRtcNetEQDecoder ACMCodecDB::neteq_decoders_[] = {
#if (defined(WEBRTC_CODEC_ISAC) || defined(WEBRTC_CODEC_ISACFX))
  kDecoderISAC,
# if (defined(WEBRTC_CODEC_ISAC))
  kDecoderISACswb,
# endif
#endif
#ifdef WEBRTC_CODEC_PCM16
  // Mono
  kDecoderPCM16B,
  kDecoderPCM16Bwb,
  kDecoderPCM16Bswb32kHz,
  // Stereo
  kDecoderPCM16B_2ch,
  kDecoderPCM16Bwb_2ch,
  kDecoderPCM16Bswb32kHz_2ch,
#endif
  // G.711, PCM mu-las and A-law.
  // Mono
  kDecoderPCMu,
  kDecoderPCMa,
  // Stereo
  kDecoderPCMu_2ch,
  kDecoderPCMa_2ch,
#ifdef WEBRTC_CODEC_ILBC
  kDecoderILBC,
#endif
#ifdef WEBRTC_CODEC_AMR
  kDecoderAMR,
#endif
#ifdef WEBRTC_CODEC_AMRWB
  kDecoderAMRWB,
#endif
#ifdef WEBRTC_CODEC_CELT
  // Mono
  kDecoderCELT_32,
  // Stereo
  kDecoderCELT_32_2ch,
#endif
#ifdef WEBRTC_CODEC_G722
  // Mono
  kDecoderG722,
  // Stereo
  kDecoderG722_2ch,
#endif
#ifdef WEBRTC_CODEC_G722_1
  kDecoderG722_1_32,
  kDecoderG722_1_24,
  kDecoderG722_1_16,
#endif
#ifdef WEBRTC_CODEC_G722_1C
  kDecoderG722_1C_48,
  kDecoderG722_1C_32,
  kDecoderG722_1C_24,
#endif
#ifdef WEBRTC_CODEC_G729
  kDecoderG729,
#endif
#ifdef WEBRTC_CODEC_G729_1
  kDecoderG729_1,
#endif
#ifdef WEBRTC_CODEC_GSMFR
  kDecoderGSMFR,
#endif
#ifdef WEBRTC_CODEC_OPUS
  kDecoderOpus,
#endif
#ifdef WEBRTC_CODEC_SPEEX
  kDecoderSPEEX_8,
  kDecoderSPEEX_16,
#endif
  // Comfort noise for three different sampling frequencies.
  kDecoderCNG,
  kDecoderCNG,
  kDecoderCNG,
  kDecoderCNG,
#ifdef WEBRTC_CODEC_AVT
  kDecoderAVT,
#endif
#ifdef WEBRTC_CODEC_RED
  kDecoderRED,
#endif
  kDecoderReservedEnd
};

// Get codec information from database.
// TODO(tlegrand): replace memcpy with a pointer to the data base memory.
int ACMCodecDB::Codec(int codec_id, CodecInst* codec_inst) {
  // Error check to see that codec_id is not out of bounds.
  if ((codec_id < 0) || (codec_id >= kNumCodecs)) {
    return -1;
  }

  // Copy database information for the codec to the output.
  memcpy(codec_inst, &database_[codec_id], sizeof(CodecInst));

  return 0;
}

// Enumerator for error codes when asking for codec database id.
enum {
  kInvalidCodec = -10,
  kInvalidPayloadtype = -30,
  kInvalidPacketSize = -40,
  kInvalidRate = -50
};

// Gets the codec id number from the database. If there is some mismatch in
// the codec settings, an error message will be recorded in the error string.
// NOTE! Only the first mismatch found will be recorded in the error string.
int ACMCodecDB::CodecNumber(const CodecInst* codec_inst, int* mirror_id,
                            char* err_message, int max_message_len_byte) {
  int codec_id = ACMCodecDB::CodecNumber(codec_inst, mirror_id);

  // Write error message if ACMCodecDB::CodecNumber() returned error.
  if ((codec_id < 0) && (err_message != NULL)) {
    char my_err_msg[1000];

    if (codec_id == kInvalidCodec) {
      sprintf(my_err_msg, "Call to ACMCodecDB::CodecNumber failed, Codec not "
              "found");
    } else if (codec_id == kInvalidPayloadtype) {
      sprintf(my_err_msg, "Call to ACMCodecDB::CodecNumber failed, payload "
              "number %d is out of range for %s", codec_inst->pltype,
              codec_inst->plname);
    } else if (codec_id == kInvalidPacketSize) {
      sprintf(my_err_msg, "Call to ACMCodecDB::CodecNumber failed, Packet "
              "size is out of range for %s", codec_inst->plname);
    } else if (codec_id == kInvalidRate) {
      sprintf(my_err_msg, "Call to ACMCodecDB::CodecNumber failed, rate=%d "
              "is not a valid rate for %s", codec_inst->rate,
              codec_inst->plname);
    } else {
      // Other error
      sprintf(my_err_msg, "invalid codec parameters to be registered, "
              "ACMCodecDB::CodecNumber failed");
    }

    strncpy(err_message, my_err_msg, max_message_len_byte - 1);
    // make sure that the message is null-terminated.
    err_message[max_message_len_byte - 1] = '\0';
  }

  return codec_id;
}

// Gets the codec id number from the database. If there is some mismatch in
// the codec settings, the function will return an error code.
// NOTE! The first mismatch found will generate the return value.
int ACMCodecDB::CodecNumber(const CodecInst* codec_inst, int* mirror_id) {
  // Look for a matching codec in the database.
  int codec_id = CodecId(codec_inst);

  // Checks if we found a matching codec.
  if (codec_id == -1) {
    return kInvalidCodec;
  }

  // Checks the validity of payload type
  if (!ValidPayloadType(codec_inst->pltype)) {
    return kInvalidPayloadtype;
  }

  // Comfort Noise is special case, packet-size & rate is not checked.
  if (STR_CASE_CMP(database_[codec_id].plname, "CN") == 0) {
    *mirror_id = codec_id;
    return codec_id;
  }

  // RED is special case, packet-size & rate is not checked.
  if (STR_CASE_CMP(database_[codec_id].plname, "red") == 0) {
    *mirror_id = codec_id;
    return codec_id;
  }

  // Checks the validity of packet size.
  if (codec_settings_[codec_id].num_packet_sizes > 0) {
    bool packet_size_ok = false;
    int i;
    int packet_size_samples;
    for (i = 0; i < codec_settings_[codec_id].num_packet_sizes; i++) {
      packet_size_samples =
          codec_settings_[codec_id].packet_sizes_samples[i];
      if (codec_inst->pacsize == packet_size_samples) {
        packet_size_ok = true;
        break;
      }
    }

    if (!packet_size_ok) {
      return kInvalidPacketSize;
    }
  }

  if (codec_inst->pacsize < 1) {
    return kInvalidPacketSize;
  }

  // Check the validity of rate. Codecs with multiple rates have their own
  // function for this.
  *mirror_id = codec_id;
  if (STR_CASE_CMP("isac", codec_inst->plname) == 0) {
    if (IsISACRateValid(codec_inst->rate)) {
      // Set mirrorID to iSAC WB which is only created once to be used both for
      // iSAC WB and SWB, because they need to share struct.
      *mirror_id = kISAC;
      return  codec_id;
    } else {
      return kInvalidRate;
    }
  } else if (STR_CASE_CMP("ilbc", codec_inst->plname) == 0) {
    return IsILBCRateValid(codec_inst->rate, codec_inst->pacsize)
        ? codec_id : kInvalidRate;
  } else if (STR_CASE_CMP("amr", codec_inst->plname) == 0) {
    return IsAMRRateValid(codec_inst->rate)
        ? codec_id : kInvalidRate;
  } else if (STR_CASE_CMP("amr-wb", codec_inst->plname) == 0) {
    return IsAMRwbRateValid(codec_inst->rate)
        ? codec_id : kInvalidRate;
  } else if (STR_CASE_CMP("g7291", codec_inst->plname) == 0) {
    return IsG7291RateValid(codec_inst->rate)
        ? codec_id : kInvalidRate;
  } else if (STR_CASE_CMP("opus", codec_inst->plname) == 0) {
    return IsOpusRateValid(codec_inst->rate)
        ? codec_id : kInvalidRate;
  } else if (STR_CASE_CMP("speex", codec_inst->plname) == 0) {
    return IsSpeexRateValid(codec_inst->rate)
        ? codec_id : kInvalidRate;
  } else if (STR_CASE_CMP("celt", codec_inst->plname) == 0) {
    return IsCeltRateValid(codec_inst->rate)
        ? codec_id : kInvalidRate;
  }

  return IsRateValid(codec_id, codec_inst->rate) ?
      codec_id : kInvalidRate;
}

// Looks for a matching payload name, frequency, and channels in the
// codec list. Need to check all three since some codecs have several codec
// entries with different frequencies and/or channels.
// Does not check other codec settings, such as payload type and packet size.
// Returns the id of the codec, or -1 if no match is found.
int ACMCodecDB::CodecId(const CodecInst* codec_inst) {
  return (CodecId(codec_inst->plname, codec_inst->plfreq,
                  codec_inst->channels));
}

int ACMCodecDB::CodecId(const char* payload_name, int frequency, int channels) {
  for (int id = 0; id < kNumCodecs; id++) {
    bool name_match = false;
    bool frequency_match = false;
    bool channels_match = false;

    // Payload name, sampling frequency and number of channels need to match.
    // NOTE! If |frequency| is -1, the frequency is not applicable, and is
    // always treated as true, like for RED.
    name_match = (STR_CASE_CMP(database_[id].plname, payload_name) == 0);
    frequency_match = (frequency == database_[id].plfreq) || (frequency == -1);
    channels_match = (channels == database_[id].channels);

    if (name_match && frequency_match && channels_match) {
      // We have found a matching codec in the list.
      return id;
    }
  }

  // We didn't find a matching codec.
  return -1;
}
// Gets codec id number, and mirror id, from database for the receiver.
int ACMCodecDB::ReceiverCodecNumber(const CodecInst* codec_inst,
    int* mirror_id) {
  // Look for a matching codec in the database.
  int codec_id = CodecId(codec_inst);

  // Set |mirror_id| to |codec_id|, except for iSAC. In case of iSAC we always
  // set |mirror_id| to iSAC WB (kISAC) which is only created once to be used
  // both for iSAC WB and SWB, because they need to share struct.
  if (STR_CASE_CMP(codec_inst->plname, "ISAC") != 0) {
    *mirror_id = codec_id;
  } else {
    *mirror_id = kISAC;
  }

  return codec_id;
}

// Returns the codec sampling frequency for codec with id = "codec_id" in
// database.
int ACMCodecDB::CodecFreq(int codec_id) {
  // Error check to see that codec_id is not out of bounds.
  if (codec_id < 0 || codec_id >= kNumCodecs) {
    return -1;
  }

  return database_[codec_id].plfreq;
}

// Returns the codec's basic coding block size in samples.
int ACMCodecDB::BasicCodingBlock(int codec_id) {
  // Error check to see that codec_id is not out of bounds.
  if (codec_id < 0 || codec_id >= kNumCodecs) {
      return -1;
  }

  return codec_settings_[codec_id].basic_block_samples;
}

// Returns the NetEQ decoder database.
const WebRtcNetEQDecoder* ACMCodecDB::NetEQDecoders() {
  return neteq_decoders_;
}

// Gets mirror id. The Id is used for codecs sharing struct for settings that
// need different payload types.
int ACMCodecDB::MirrorID(int codec_id) {
  if (STR_CASE_CMP(database_[codec_id].plname, "isac") == 0) {
    return kISAC;
  } else {
    return codec_id;
  }
}

// Creates memory/instance for storing codec state.
ACMGenericCodec* ACMCodecDB::CreateCodecInstance(const CodecInst* codec_inst) {
  // All we have support for right now.
  if (!STR_CASE_CMP(codec_inst->plname, "ISAC")) {
#if (defined(WEBRTC_CODEC_ISAC) || defined(WEBRTC_CODEC_ISACFX))
    return new ACMISAC(kISAC);
#endif
  } else if (!STR_CASE_CMP(codec_inst->plname, "PCMU")) {
    if (codec_inst->channels == 1) {
      return new ACMPCMU(kPCMU);
    } else {
      return new ACMPCMU(kPCMU_2ch);
    }
  } else if (!STR_CASE_CMP(codec_inst->plname, "PCMA")) {
    if (codec_inst->channels == 1) {
      return new ACMPCMA(kPCMA);
    } else {
      return new ACMPCMA(kPCMA_2ch);
    }
  } else if (!STR_CASE_CMP(codec_inst->plname, "ILBC")) {
#ifdef WEBRTC_CODEC_ILBC
    return new ACMILBC(kILBC);
#endif
  } else if (!STR_CASE_CMP(codec_inst->plname, "AMR")) {
#ifdef WEBRTC_CODEC_AMR
    return new ACMAMR(kGSMAMR);
#endif
  } else if (!STR_CASE_CMP(codec_inst->plname, "AMR-WB")) {
#ifdef WEBRTC_CODEC_AMRWB
    return new ACMAMRwb(kGSMAMRWB);
#endif
  } else if (!STR_CASE_CMP(codec_inst->plname, "CELT")) {
#ifdef WEBRTC_CODEC_CELT
    if (codec_inst->channels == 1) {
      return new ACMCELT(kCELT32);
    } else {
      return new ACMCELT(kCELT32_2ch);
    }
#endif
  } else if (!STR_CASE_CMP(codec_inst->plname, "G722")) {
#ifdef WEBRTC_CODEC_G722
    if (codec_inst->channels == 1) {
      return new ACMG722(kG722);
    } else {
      return new ACMG722(kG722_2ch);
    }
#endif
  } else if (!STR_CASE_CMP(codec_inst->plname, "G7221")) {
    switch (codec_inst->plfreq) {
      case 16000: {
#ifdef WEBRTC_CODEC_G722_1
        int codec_id;
        switch (codec_inst->rate) {
          case 16000 : {
            codec_id = kG722_1_16;
            break;
          }
          case 24000 : {
            codec_id = kG722_1_24;
            break;
          }
          case 32000 : {
            codec_id = kG722_1_32;
            break;
          }
          default: {
            return NULL;
          }
          return new ACMG722_1(codec_id);
        }
#endif
      }
      case 32000: {
#ifdef WEBRTC_CODEC_G722_1C
        int codec_id;
        switch (codec_inst->rate) {
          case 24000 : {
            codec_id = kG722_1C_24;
            break;
          }
          case 32000 : {
            codec_id = kG722_1C_32;
            break;
          }
          case 48000 : {
            codec_id = kG722_1C_48;
            break;
          }
          default: {
            return NULL;
          }
          return new ACMG722_1C(codec_id);
        }
#endif
      }
    }
  } else if (!STR_CASE_CMP(codec_inst->plname, "CN")) {
    // For CN we need to check sampling frequency to know what codec to create.
    int codec_id;
    switch (codec_inst->plfreq) {
      case 8000: {
        codec_id = kCNNB;
        break;
      }
      case 16000: {
        codec_id = kCNWB;
        break;
      }
      case 32000: {
        codec_id = kCNSWB;
        break;
      }
      case 48000: {
        codec_id = kCNFB;
        break;
      }
      default: {
        return NULL;
      }
    }
    return new ACMCNG(codec_id);
  } else if (!STR_CASE_CMP(codec_inst->plname, "G729")) {
#ifdef WEBRTC_CODEC_G729
    return new ACMG729(kG729);
#endif
  } else if (!STR_CASE_CMP(codec_inst->plname, "G7291")) {
#ifdef WEBRTC_CODEC_G729_1
    return new ACMG729_1(kG729_1);
#endif
  } else if (!STR_CASE_CMP(codec_inst->plname, "opus")) {
#ifdef WEBRTC_CODEC_OPUS
    return new ACMOpus(kOpus);
#endif
  } else if (!STR_CASE_CMP(codec_inst->plname, "speex")) {
#ifdef WEBRTC_CODEC_SPEEX
    int codec_id;
    switch (codec_inst->plfreq) {
      case 8000: {
        codec_id = kSPEEX8;
        break;
      }
      case 16000: {
        codec_id = kSPEEX16;
        break;
      }
      default: {
        return NULL;
      }
    }
    return new ACMSPEEX(codec_id);
#endif
  } else if (!STR_CASE_CMP(codec_inst->plname, "CN")) {
    // For CN we need to check sampling frequency to know what codec to create.
    int codec_id;
    switch (codec_inst->plfreq) {
      case 8000: {
        codec_id = kCNNB;
        break;
      }
      case 16000: {
        codec_id = kCNWB;
        break;
      }
      case 32000: {
        codec_id = kCNSWB;
        break;
      }
      case 48000: {
        codec_id = kCNFB;
        break;
      }
      default: {
        return NULL;
      }
    }
    return new ACMCNG(codec_id);
  } else if (!STR_CASE_CMP(codec_inst->plname, "L16")) {
#ifdef WEBRTC_CODEC_PCM16
    // For L16 we need to check sampling frequency to know what codec to create.
    int codec_id;
    if (codec_inst->channels == 1) {
      switch (codec_inst->plfreq) {
        case 8000: {
          codec_id = kPCM16B;
          break;
        }
        case 16000: {
          codec_id = kPCM16Bwb;
          break;
        }
        case 32000: {
          codec_id = kPCM16Bswb32kHz;
          break;
        }
        default: {
          return NULL;
        }
      }
    } else {
      switch (codec_inst->plfreq) {
        case 8000: {
          codec_id = kPCM16B_2ch;
          break;
        }
        case 16000: {
          codec_id = kPCM16Bwb_2ch;
          break;
        }
        case 32000: {
          codec_id = kPCM16Bswb32kHz_2ch;
          break;
        }
        default: {
          return NULL;
        }
      }
    }
    return new ACMPCM16B(codec_id);
#endif
  } else if (!STR_CASE_CMP(codec_inst->plname, "telephone-event")) {
#ifdef WEBRTC_CODEC_AVT
    return new ACMDTMFPlayout(kAVT);
#endif
  } else if (!STR_CASE_CMP(codec_inst->plname, "red")) {
#ifdef WEBRTC_CODEC_RED
    return new ACMRED(kRED);
#endif
  }
  return NULL;
}

// Checks if the bitrate is valid for the codec.
bool ACMCodecDB::IsRateValid(int codec_id, int rate) {
  if (database_[codec_id].rate == rate) {
    return true;
  } else {
    return false;
  }
}

// Checks if the bitrate is valid for iSAC.
bool ACMCodecDB::IsISACRateValid(int rate) {
  if ((rate == -1) || ((rate <= 56000) && (rate >= 10000))) {
    return true;
  } else {
    return false;
  }
}

// Checks if the bitrate is valid for iLBC.
bool ACMCodecDB::IsILBCRateValid(int rate, int frame_size_samples) {
  if (((frame_size_samples == 240) || (frame_size_samples == 480)) &&
      (rate == 13300)) {
    return true;
  } else if (((frame_size_samples == 160) || (frame_size_samples == 320)) &&
      (rate == 15200)) {
    return true;
  } else {
    return false;
  }
}

// Check if the bitrate is valid for the GSM-AMR.
bool ACMCodecDB::IsAMRRateValid(int rate) {
  switch (rate) {
    case 4750:
    case 5150:
    case 5900:
    case 6700:
    case 7400:
    case 7950:
    case 10200:
    case 12200: {
      return true;
    }
    default: {
      return false;
    }
  }
}

// Check if the bitrate is valid for GSM-AMR-WB.
bool ACMCodecDB::IsAMRwbRateValid(int rate) {
  switch (rate) {
    case 7000:
    case 9000:
    case 12000:
    case 14000:
    case 16000:
    case 18000:
    case 20000:
    case 23000:
    case 24000: {
      return true;
    }
    default: {
      return false;
    }
  }
}

// Check if the bitrate is valid for G.729.1.
bool ACMCodecDB::IsG7291RateValid(int rate) {
  switch (rate) {
    case 8000:
    case 12000:
    case 14000:
    case 16000:
    case 18000:
    case 20000:
    case 22000:
    case 24000:
    case 26000:
    case 28000:
    case 30000:
    case 32000: {
      return true;
    }
    default: {
      return false;
    }
  }
}

// Checks if the bitrate is valid for Speex.
bool ACMCodecDB::IsSpeexRateValid(int rate) {
  if (rate > 2000) {
    return true;
  } else {
    return false;
  }
}

// Checks if the bitrate is valid for Opus.
bool ACMCodecDB::IsOpusRateValid(int rate) {
  if ((rate < 6000) || (rate > 510000)) {
    return false;
  }
  return true;
}

// Checks if the bitrate is valid for Celt.
bool ACMCodecDB::IsCeltRateValid(int rate) {
  if ((rate >= 48000) && (rate <= 128000)) {
    return true;
  } else {
    return false;
  }
}

// Checks if the payload type is in the valid range.
bool ACMCodecDB::ValidPayloadType(int payload_type) {
  if ((payload_type < 0) || (payload_type > 127)) {
    return false;
  }
  return true;
}

}  // namespace webrtc
