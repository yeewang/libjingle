/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// MSVC++ requires this to be set before any other includes to get M_PI.
#define _USE_MATH_DEFINES

#include <cmath>
#include <limits>

#include "testing/gtest/include/gtest/gtest.h"
#include "webrtc/base/compile_assert.h"
#include "webrtc/common_audio/wav_header.h"
#include "webrtc/common_audio/wav_writer.h"
#include "webrtc/test/testsupport/fileutils.h"

static const float kSamples[] = {0.0, 10.0, 4e4, -1e9};

// Write a tiny WAV file with the C++ interface and verify the result.
TEST(WavWriterTest, CPP) {
  const std::string outfile = webrtc::test::OutputPath() + "wavtest1.wav";
  static const uint32_t kNumSamples = 3;
  {
    webrtc::WavFile w(outfile, 14099, 1);
    EXPECT_EQ(14099, w.sample_rate());
    EXPECT_EQ(1, w.num_channels());
    EXPECT_EQ(0u, w.num_samples());
    w.WriteSamples(kSamples, kNumSamples);
    EXPECT_EQ(kNumSamples, w.num_samples());
  }
  static const uint8_t kExpectedContents[] = {
    'R', 'I', 'F', 'F',
    42, 0, 0, 0,  // size of whole file - 8: 6 + 44 - 8
    'W', 'A', 'V', 'E',
    'f', 'm', 't', ' ',
    16, 0, 0, 0,  // size of fmt block - 8: 24 - 8
    1, 0,  // format: PCM (1)
    1, 0,  // channels: 1
    0x13, 0x37, 0, 0,  // sample rate: 14099
    0x26, 0x6e, 0, 0,  // byte rate: 2 * 14099
    2, 0,  // block align: NumChannels * BytesPerSample
    16, 0,  // bits per sample: 2 * 8
    'd', 'a', 't', 'a',
    6, 0, 0, 0,  // size of payload: 6
    0, 0,  // first sample: 0.0
    10, 0,  // second sample: 10.0
    0xff, 0x7f,  // third sample: 4e4 (saturated)
  };
  static const int kContentSize =
      webrtc::kWavHeaderSize + kNumSamples * sizeof(int16_t);
  COMPILE_ASSERT(sizeof(kExpectedContents) == kContentSize, content_size);
  EXPECT_EQ(size_t(kContentSize), webrtc::test::GetFileSize(outfile));
  FILE* f = fopen(outfile.c_str(), "rb");
  ASSERT_TRUE(f);
  uint8_t contents[kContentSize];
  ASSERT_EQ(1u, fread(contents, kContentSize, 1, f));
  EXPECT_EQ(0, fclose(f));
  EXPECT_EQ(0, memcmp(kExpectedContents, contents, kContentSize));
}

// Write a tiny WAV file with the C interface and verify the result.
TEST(WavWriterTest, C) {
  const std::string outfile = webrtc::test::OutputPath() + "wavtest2.wav";
  rtc_WavFile *w = rtc_WavOpen(outfile.c_str(), 11904, 2);
  EXPECT_EQ(11904, rtc_WavSampleRate(w));
  EXPECT_EQ(2, rtc_WavNumChannels(w));
  EXPECT_EQ(0u, rtc_WavNumSamples(w));
  static const uint32_t kNumSamples = 4;
  rtc_WavWriteSamples(w, &kSamples[0], 2);
  EXPECT_EQ(2u, rtc_WavNumSamples(w));
  rtc_WavWriteSamples(w, &kSamples[2], kNumSamples - 2);
  EXPECT_EQ(kNumSamples, rtc_WavNumSamples(w));
  rtc_WavClose(w);
  static const uint8_t kExpectedContents[] = {
    'R', 'I', 'F', 'F',
    44, 0, 0, 0,  // size of whole file - 8: 8 + 44 - 8
    'W', 'A', 'V', 'E',
    'f', 'm', 't', ' ',
    16, 0, 0, 0,  // size of fmt block - 8: 24 - 8
    1, 0,  // format: PCM (1)
    2, 0,  // channels: 2
    0x80, 0x2e, 0, 0,  // sample rate: 11904
    0, 0xba, 0, 0,  // byte rate: 2 * 2 * 11904
    4, 0,  // block align: NumChannels * BytesPerSample
    16, 0,  // bits per sample: 2 * 8
    'd', 'a', 't', 'a',
    8, 0, 0, 0,  // size of payload: 8
    0, 0,  // first sample: 0.0
    10, 0,  // second sample: 10.0
    0xff, 0x7f,  // third sample: 4e4 (saturated)
    0, 0x80,  // fourth sample: -1e9 (saturated)
  };
  static const int kContentSize =
      webrtc::kWavHeaderSize + kNumSamples * sizeof(int16_t);
  COMPILE_ASSERT(sizeof(kExpectedContents) == kContentSize, content_size);
  EXPECT_EQ(size_t(kContentSize), webrtc::test::GetFileSize(outfile));
  FILE* f = fopen(outfile.c_str(), "rb");
  ASSERT_TRUE(f);
  uint8_t contents[kContentSize];
  ASSERT_EQ(1u, fread(contents, kContentSize, 1, f));
  EXPECT_EQ(0, fclose(f));
  EXPECT_EQ(0, memcmp(kExpectedContents, contents, kContentSize));
}

// Write a larger WAV file. You can listen to this file to sanity-check it.
TEST(WavWriterTest, LargeFile) {
  std::string outfile = webrtc::test::OutputPath() + "wavtest3.wav";
  static const int kSampleRate = 8000;
  static const int kNumChannels = 2;
  static const uint32_t kNumSamples = 3 * kSampleRate * kNumChannels;
  float samples[kNumSamples];
  for (uint32_t i = 0; i < kNumSamples; i += kNumChannels) {
    // A nice periodic beeping sound.
    static const double kToneHz = 440;
    const double t = static_cast<double>(i) / (kNumChannels * kSampleRate);
    const double x =
        std::numeric_limits<int16_t>::max() * std::sin(t * kToneHz * 2 * M_PI);
    samples[i] = std::pow(std::sin(t * 2 * 2 * M_PI), 10) * x;
    samples[i + 1] = std::pow(std::cos(t * 2 * 2 * M_PI), 10) * x;
  }
  {
    webrtc::WavFile w(outfile, kSampleRate, kNumChannels);
    EXPECT_EQ(kSampleRate, w.sample_rate());
    EXPECT_EQ(kNumChannels, w.num_channels());
    EXPECT_EQ(0u, w.num_samples());
    w.WriteSamples(samples, kNumSamples);
    EXPECT_EQ(kNumSamples, w.num_samples());
  }
  EXPECT_EQ(sizeof(int16_t) * kNumSamples + webrtc::kWavHeaderSize,
            webrtc::test::GetFileSize(outfile));
}
