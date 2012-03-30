/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <cmath>
#include <numeric>
#include <vector>

#include "voice_engine/main/test/auto_test/fixtures/after_streaming_fixture.h"

#ifdef MAC_IPHONE
  const int kMinimumReasonableDelayEstimateMs = 30;
#else
  const int kMinimumReasonableDelayEstimateMs = 45;
#endif  // !MAC_IPHONE

class VideoSyncTest : public AfterStreamingFixture {
 protected:
  // This test will verify that delay estimates converge (e.g. the standard
  // deviation for the last five seconds' estimates is less than 20) without
  // manual observation. The test runs for 15 seconds, sampling once per second.
  // All samples are checked so they are greater than |min_estimate|.
  int CollectEstimatesDuring15Seconds(int min_estimate) {
    Sleep(1000);

    std::vector<int> all_delay_estimates;
    for (int second = 0; second < 15; second++) {
      int delay_estimate = 0;
      EXPECT_EQ(0, voe_vsync_->GetDelayEstimate(channel_, delay_estimate));

      EXPECT_GT(delay_estimate, min_estimate) <<
          "The delay estimate can not conceivably get lower than " <<
          min_estimate << " ms, it's unrealistic.";

      all_delay_estimates.push_back(delay_estimate);
      Sleep(1000);
    }

    return ComputeStandardDeviation(
        all_delay_estimates.begin() + 10, all_delay_estimates.end());
  }

  void CheckEstimatesConvergeReasonablyWell(int min_estimate) {
    int standard_deviation = CollectEstimatesDuring15Seconds(min_estimate);
    EXPECT_LT(standard_deviation, 20);
  }

  // Computes the standard deviation by first estimating the sample variance
  // with an unbiased estimator.
  int ComputeStandardDeviation(std::vector<int>::const_iterator start,
                               std::vector<int>::const_iterator end) const {
    int num_elements = end - start;
    int mean = std::accumulate(start, end, 0) / num_elements;
    assert(num_elements > 1);

    int variance = 0;
    for (; start != end; ++start) {
      variance += (*start - mean) * (*start - mean) / (num_elements - 1);
    }
    return std::sqrt(variance);
  }
};

TEST_F(VideoSyncTest, CanGetPlayoutTimestampWhilePlayingWithoutSettingItFirst) {
  unsigned int ignored;
  EXPECT_EQ(0, voe_vsync_->GetPlayoutTimestamp(channel_, ignored));
}

TEST_F(VideoSyncTest, CannotSetInitTimestampWhilePlaying) {
  EXPECT_EQ(-1, voe_vsync_->SetInitTimestamp(channel_, 12345));
}

TEST_F(VideoSyncTest, CannotSetInitSequenceNumberWhilePlaying) {
  EXPECT_EQ(-1, voe_vsync_->SetInitSequenceNumber(channel_, 123));
}

TEST_F(VideoSyncTest, CanSetInitTimestampWhileStopped) {
  EXPECT_EQ(0, voe_base_->StopSend(channel_));
  EXPECT_EQ(0, voe_vsync_->SetInitTimestamp(channel_, 12345));
}

TEST_F(VideoSyncTest, CanSetInitSequenceNumberWhileStopped) {
  EXPECT_EQ(0, voe_base_->StopSend(channel_));
  EXPECT_EQ(0, voe_vsync_->SetInitSequenceNumber(channel_, 123));
}

TEST_F(VideoSyncTest, DelayEstimatesStabilizeDuring15sAndAreNotTooLow) {
  EXPECT_EQ(0, voe_base_->StopSend(channel_));
  EXPECT_EQ(0, voe_vsync_->SetInitTimestamp(channel_, 12345));
  EXPECT_EQ(0, voe_vsync_->SetInitSequenceNumber(channel_, 123));
  EXPECT_EQ(0, voe_base_->StartSend(channel_));

  CheckEstimatesConvergeReasonablyWell(kMinimumReasonableDelayEstimateMs);
}

TEST_F(VideoSyncTest, DelayEstimatesStabilizeAfterNetEqMinDelayChanges45s) {
  EXPECT_EQ(0, voe_base_->StopSend(channel_));
  EXPECT_EQ(0, voe_vsync_->SetInitTimestamp(channel_, 12345));
  EXPECT_EQ(0, voe_vsync_->SetInitSequenceNumber(channel_, 123));
  EXPECT_EQ(0, voe_base_->StartSend(channel_));

  CheckEstimatesConvergeReasonablyWell(kMinimumReasonableDelayEstimateMs);
  EXPECT_EQ(0, voe_vsync_->SetMinimumPlayoutDelay(channel_, 200));
  CheckEstimatesConvergeReasonablyWell(kMinimumReasonableDelayEstimateMs);
  EXPECT_EQ(0, voe_vsync_->SetMinimumPlayoutDelay(channel_, 0));
  CheckEstimatesConvergeReasonablyWell(kMinimumReasonableDelayEstimateMs);
}

#if !defined(WEBRTC_ANDROID)
TEST_F(VideoSyncTest, CanGetPlayoutBufferSize) {
  int ignored;
  EXPECT_EQ(0, voe_vsync_->GetPlayoutBufferSize(ignored));
}
#endif  // !ANDROID
