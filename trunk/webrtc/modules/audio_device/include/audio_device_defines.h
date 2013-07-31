/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_AUDIO_DEVICE_AUDIO_DEVICE_DEFINES_H
#define WEBRTC_AUDIO_DEVICE_AUDIO_DEVICE_DEFINES_H

#include "webrtc/typedefs.h"

namespace webrtc {

static const int kAdmMaxDeviceNameSize = 128;
static const int kAdmMaxFileNameSize = 512;
static const int kAdmMaxGuidSize = 128;

static const int kAdmMinPlayoutBufferSizeMs = 10;
static const int kAdmMaxPlayoutBufferSizeMs = 250;

// ----------------------------------------------------------------------------
//  AudioDeviceObserver
// ----------------------------------------------------------------------------

class AudioDeviceObserver
{
public:
    enum ErrorCode
    {
        kRecordingError = 0,
        kPlayoutError = 1
    };
    enum WarningCode
    {
        kRecordingWarning = 0,
        kPlayoutWarning = 1
    };

    virtual void OnErrorIsReported(const ErrorCode error) = 0;
    virtual void OnWarningIsReported(const WarningCode warning) = 0;

protected:
    virtual ~AudioDeviceObserver() {}
};

// ----------------------------------------------------------------------------
//  AudioTransport
// ----------------------------------------------------------------------------

class AudioTransport
{
public:
    virtual int32_t RecordedDataIsAvailable(const void* audioSamples,
                                            const uint32_t nSamples,
                                            const uint8_t nBytesPerSample,
                                            const uint8_t nChannels,
                                            const uint32_t samplesPerSec,
                                            const uint32_t totalDelayMS,
                                            const int32_t clockDrift,
                                            const uint32_t currentMicLevel,
                                            const bool keyPressed,
                                            uint32_t& newMicLevel) = 0;   

    virtual int32_t NeedMorePlayData(const uint32_t nSamples,
                                     const uint8_t nBytesPerSample,
                                     const uint8_t nChannels,
                                     const uint32_t samplesPerSec,
                                     void* audioSamples,
                                     uint32_t& nSamplesOut) = 0;

    // Method to pass captured data directly and unmixed to network channels.
    // |channel_ids| contains a list of VoE channels which are the
    // sinks to the capture data. |audio_delay_milliseconds| is the sum of
    // recording delay and playout delay of the hardware. |current_volume| is
    // in the range of [0, 255], representing the current microphone analog
    // volume. |key_pressed| is used by the typing detection.
    // |need_audio_processing| specify if the data needs to be processed by APM.
    // Currently WebRtc supports only one APM, and Chrome will make sure only
    // one stream goes through APM. When |need_audio_processing| is false, the
    // values of |audio_delay_milliseconds|, |current_volume| and |key_pressed|
    // will be ignored.
    // The return value is the new microphone volume, in the range of |0, 255].
    // When the volume does not need to be updated, it returns 0.
    // TODO(xians): Make the interface pure virtual after libjingle has its
    // implementation.
    virtual int OnDataAvailable(int voe_channels[],
                                int number_of_voe_channels,
                                const int16_t* audio_data,
                                int sample_rate,
                                int number_of_channels,
                                int number_of_frames,
                                int audio_delay_milliseconds,
                                int current_volume,
                                bool key_pressed,
                                bool need_audio_processing) { return 0; }

protected:
    virtual ~AudioTransport() {}
};

}  // namespace webrtc

#endif  // WEBRTC_AUDIO_DEVICE_AUDIO_DEVICE_DEFINES_H
