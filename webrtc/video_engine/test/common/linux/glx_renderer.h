/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_VIDEO_ENGINE_TEST_COMMON_LINUX_GLX_RENDERER_H_
#define WEBRTC_VIDEO_ENGINE_TEST_COMMON_LINUX_GLX_RENDERER_H_

#include <GL/glx.h>
#include <X11/Xlib.h>

#include "webrtc/video_engine/test/common/gl/gl_renderer.h"

namespace webrtc {
namespace test {

class GlxRenderer : public GlRenderer {
 public:
  static GlxRenderer* Create(const char* window_title, size_t width,
                             size_t height);
  virtual ~GlxRenderer();

  virtual void RenderFrame(const webrtc::I420VideoFrame& frame, int delta)
      OVERRIDE;
 private:
  GlxRenderer(size_t width, size_t height);

  bool Init(const char* window_title);
  void Resize(size_t width, size_t height);
  void Destroy();

  bool is_init_;
  size_t width_, height_;

  Display* display_;
  Window window_;
  GLXContext context_;
};
}  // test
}  // webrtc

#endif  // WEBRTC_VIDEO_ENGINE_TEST_COMMON_LINUX_GLX_RENDERER_H_
