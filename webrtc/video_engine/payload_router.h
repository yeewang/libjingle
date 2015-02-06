/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_VIDEO_ENGINE_PAYLOAD_ROUTER_H_
#define WEBRTC_VIDEO_ENGINE_PAYLOAD_ROUTER_H_

#include <list>
#include <vector>

#include "webrtc/base/constructormagic.h"
#include "webrtc/base/thread_annotations.h"
#include "webrtc/common_types.h"
#include "webrtc/system_wrappers/interface/scoped_ptr.h"

namespace webrtc {

class CriticalSectionWrapper;
class RTPFragmentationHeader;
class RtpRtcp;
struct RTPVideoHeader;

// PayloadRouter routes outgoing data to the correct sending RTP module, based
// on the simulcast layer in RTPVideoHeader.
class PayloadRouter {
 public:
  PayloadRouter();
  ~PayloadRouter();

  // Rtp modules are assumed to be sorted in simulcast index order.
  void SetSendingRtpModules(const std::list<RtpRtcp*>& rtp_modules);

  // PayloadRouter will only route packets if being active, all packets will be
  // dropped otherwise.
  void set_active(bool active);
  bool active();

  // Input parameters according to the signature of RtpRtcp::SendOutgoingData.
  // Returns true if the packet was routed / sent, false otherwise.
  bool RoutePayload(FrameType frame_type,
                    int8_t payload_type,
                    uint32_t time_stamp,
                    int64_t capture_time_ms,
                    const uint8_t* payload_data,
                    size_t payload_size,
                    const RTPFragmentationHeader* fragmentation,
                    const RTPVideoHeader* rtp_video_hdr);

 private:
  scoped_ptr<CriticalSectionWrapper> crit_;

  // Active sending RTP modules, in layer order.
  std::vector<RtpRtcp*> rtp_modules_ GUARDED_BY(crit_.get());
  bool active_ GUARDED_BY(crit_.get());

  DISALLOW_COPY_AND_ASSIGN(PayloadRouter);
};

}  // namespace webrtc

#endif  // WEBRTC_VIDEO_ENGINE_PAYLOAD_ROUTER_H_
