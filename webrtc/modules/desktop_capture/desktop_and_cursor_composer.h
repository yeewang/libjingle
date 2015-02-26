/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_DESKTOP_CAPTURE_DESKTOP_AND_CURSOR_COMPOSER_H_
#define WEBRTC_MODULES_DESKTOP_CAPTURE_DESKTOP_AND_CURSOR_COMPOSER_H_

#include "webrtc/base/scoped_ptr.h"
#include "webrtc/modules/desktop_capture/desktop_capturer.h"
#include "webrtc/modules/desktop_capture/mouse_cursor_monitor.h"

namespace webrtc {

// A wrapper for DesktopCapturer that also captures mouse using specified
// MouseCursorMonitor and renders it on the generated streams.
class DesktopAndCursorComposer : public DesktopCapturer,
                            public DesktopCapturer::Callback,
                            public MouseCursorMonitor::Callback {
 public:
  // Creates a new blender that captures mouse cursor using |mouse_monitor| and
  // renders it into the frames generated by |desktop_capturer|. If
  // |mouse_monitor| is NULL the frames are passed unmodified. Takes ownership
  // of both arguments.
  DesktopAndCursorComposer(DesktopCapturer* desktop_capturer,
                      MouseCursorMonitor* mouse_monitor);
  virtual ~DesktopAndCursorComposer();

  // DesktopCapturer interface.
  virtual void Start(DesktopCapturer::Callback* callback) OVERRIDE;
  virtual void Capture(const DesktopRegion& region) OVERRIDE;
  virtual void SetExcludedWindow(WindowId window) OVERRIDE;

 private:
  // DesktopCapturer::Callback interface.
  virtual SharedMemory* CreateSharedMemory(size_t size) OVERRIDE;
  virtual void OnCaptureCompleted(DesktopFrame* frame) OVERRIDE;

  // MouseCursorMonitor::Callback interface.
  virtual void OnMouseCursor(MouseCursor* cursor) OVERRIDE;
  virtual void OnMouseCursorPosition(MouseCursorMonitor::CursorState state,
                                     const DesktopVector& position) OVERRIDE;

  rtc::scoped_ptr<DesktopCapturer> desktop_capturer_;
  rtc::scoped_ptr<MouseCursorMonitor> mouse_monitor_;

  DesktopCapturer::Callback* callback_;

  rtc::scoped_ptr<MouseCursor> cursor_;
  MouseCursorMonitor::CursorState cursor_state_;
  DesktopVector cursor_position_;

  DISALLOW_COPY_AND_ASSIGN(DesktopAndCursorComposer);
};

}  // namespace webrtc

#endif  // WEBRTC_MODULES_DESKTOP_CAPTURE_DESKTOP_AND_CURSOR_COMPOSER_H_
