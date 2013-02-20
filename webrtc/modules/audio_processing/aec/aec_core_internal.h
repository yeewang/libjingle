/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_PROCESSING_AEC_AEC_CORE_INTERNAL_H_
#define WEBRTC_MODULES_AUDIO_PROCESSING_AEC_AEC_CORE_INTERNAL_H_

#include "webrtc/modules/audio_processing/aec/aec_core.h"

#ifdef WEBRTC_AEC_DEBUG_DUMP
#include <stdio.h>
#endif

#include "webrtc/typedefs.h"

typedef struct PowerLevel {
  float sfrsum;
  int sfrcounter;
  float framelevel;
  float frsum;
  int frcounter;
  float minlevel;
  float averagelevel;
} PowerLevel;

struct AecCore {
  int farBufWritePos, farBufReadPos;

  int knownDelay;
  int inSamples, outSamples;
  int delayEstCtr;

  void *nearFrBuf, *outFrBuf;

  void *nearFrBufH;
  void *outFrBufH;

  float dBuf[PART_LEN2];  // nearend
  float eBuf[PART_LEN2];  // error

  float dBufH[PART_LEN2];  // nearend

  float xPow[PART_LEN1];
  float dPow[PART_LEN1];
  float dMinPow[PART_LEN1];
  float dInitMinPow[PART_LEN1];
  float *noisePow;

  float xfBuf[2][NR_PART * PART_LEN1];  // farend fft buffer
  float wfBuf[2][NR_PART * PART_LEN1];  // filter fft
  complex_t sde[PART_LEN1];  // cross-psd of nearend and error
  complex_t sxd[PART_LEN1];  // cross-psd of farend and nearend
  complex_t xfwBuf[NR_PART * PART_LEN1];  // farend windowed fft buffer

  float sx[PART_LEN1], sd[PART_LEN1], se[PART_LEN1];  // far, near, error psd
  float hNs[PART_LEN1];
  float hNlFbMin, hNlFbLocalMin;
  float hNlXdAvgMin;
  int hNlNewMin, hNlMinCtr;
  float overDrive, overDriveSm;
  int nlp_mode;
  float outBuf[PART_LEN];
  int delayIdx;

  short stNearState, echoState;
  short divergeState;

  int xfBufBlockPos;

  void* far_buf;
  void* far_buf_windowed;
  int system_delay;  // Current system delay buffered in AEC.

  int mult;  // sampling frequency multiple
  int sampFreq;
  WebRtc_UWord32 seed;

  float mu;  // stepsize
  float errThresh;  // error threshold

  int noiseEstCtr;

  PowerLevel farlevel;
  PowerLevel nearlevel;
  PowerLevel linoutlevel;
  PowerLevel nlpoutlevel;

  int metricsMode;
  int stateCounter;
  Stats erl;
  Stats erle;
  Stats aNlp;
  Stats rerl;

  // Quantities to control H band scaling for SWB input
  int freq_avg_ic;  // initial bin for averaging nlp gain
  int flag_Hband_cn;  // for comfort noise
  float cn_scale_Hband;  // scale for comfort noise in H band

  int delay_histogram[kHistorySizeBlocks];
  int delay_logging_enabled;
  void* delay_estimator_farend;
  void* delay_estimator;

#ifdef WEBRTC_AEC_DEBUG_DUMP
  void* far_time_buf;
  FILE *farFile;
  FILE *nearFile;
  FILE *outFile;
  FILE *outLinearFile;
#endif
};

extern WebRtcAec_FilterFar_t WebRtcAec_FilterFar;
extern WebRtcAec_ScaleErrorSignal_t WebRtcAec_ScaleErrorSignal;
extern WebRtcAec_FilterAdaptation_t WebRtcAec_FilterAdaptation;
extern WebRtcAec_OverdriveAndSuppress_t WebRtcAec_OverdriveAndSuppress;

#endif  // WEBRTC_MODULES_AUDIO_PROCESSING_AEC_AEC_CORE_INTERNAL_H_
