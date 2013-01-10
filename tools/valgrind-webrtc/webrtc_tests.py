#!/usr/bin/env python
# Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
#
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file in the root of the source
# tree. An additional intellectual property rights grant can be found
# in the file PATENTS.  All contributing project authors may
# be found in the AUTHORS file in the root of the source tree.

"""Runs various WebRTC tests through valgrind_test.py.

This script inherits the chrome_tests.py in Chrome, but allows running any test
instead of only the hard-coded ones. It uses the -t cmdline flag to do this, and
only supports specifying a single test for each run.

Suppression files:
The Chrome valgrind directory we use as a DEPS dependency contains the following
suppression files:
  valgrind/memcheck/suppressions.txt
  valgrind/memcheck/suppressions_mac.txt
  valgrind/tsan/suppressions.txt
  valgrind/tsan/suppressions_mac.txt
  valgrind/tsan/suppressions_win32.txt
Since they're referenced from the chrome_tests.py script, we have similar files
below the directory of this script. When executing, this script will setup both
Chrome's suppression files and our own, so we can easily maintain WebRTC
specific suppressions in our own files.
"""

import logging
import optparse
import os
import sys

import logging_utils
import path_utils

import chrome_tests


class WebRTCTest(chrome_tests.ChromeTests):
  """Class that handles setup of suppressions for WebRTC.

  Everything else is inherited from chrome_tests.ChromeTests.
  """

  def _DefaultCommand(self, tool, exe=None, valgrind_test_args=None):
    """Override command-building method so we can add more suppressions."""
    cmd = chrome_tests.ChromeTests._DefaultCommand(self, tool, exe,
                                                   valgrind_test_args)
    # When ChromeTests._DefaultCommand has executed, it has setup suppression
    # files based on what's found in the memcheck/ or tsan/ subdirectories of
    # this script's location. If Mac or Windows is executing, additional
    # platform specific files have also been added.
    # Since only the ones located below this directory is added, we must also
    # add the ones maintained by Chrome, located in ../valgrind.

    # The idea is to look for --suppression arguments in the cmd list and add a
    # modified copy of each suppression file, for the corresponding file in
    # ../valgrind. If we would simply replace 'valgrind-webrtc' with 'valgrind'
    # we may produce invalid paths if other parts of the path contain that
    # string. That's why the code below only replaces the end of the path.
    script_dir = path_utils.ScriptDir()
    old_base, _ = os.path.split(script_dir)
    new_dir = os.path.join(old_base, 'valgrind')
    add_suppressions = []
    for token in cmd:
      if '--suppressions' in token:
        add_suppressions.append(token.replace(script_dir, new_dir))
    return add_suppressions + cmd


def main(_):
  parser = optparse.OptionParser('usage: %prog -b <dir> -t <test> <test args>')
  parser.disable_interspersed_args()
  parser.add_option('-b', '--build_dir',
                    help=('Location of the compiler output. Can only be used '
                          'when the test argument does not contain this path.'))
  parser.add_option('-t', '--test', help='Test to run.')
  parser.add_option('', '--baseline', action='store_true', default=False,
                    help='Generate baseline data instead of validating')
  parser.add_option('', '--gtest_filter',
                    help='Additional arguments to --gtest_filter')
  parser.add_option('', '--gtest_repeat',
                    help='Argument for --gtest_repeat')
  parser.add_option('-v', '--verbose', action='store_true', default=False,
                    help='Verbose output - enable debug log messages')
  parser.add_option('', '--tool', dest='valgrind_tool', default='memcheck',
                    help='Specify a valgrind tool to run the tests under')
  parser.add_option('', '--tool_flags', dest='valgrind_tool_flags', default='',
                    help='Specify custom flags for the selected valgrind tool')
  parser.add_option('', '--keep_logs', action='store_true', default=False,
                    help=('Store memory tool logs in the <tool>.logs directory '
                          'instead of /tmp.\nThis can be useful for tool '
                          'developers/maintainers.\nPlease note that the <tool>'
                          '.logs directory will be clobbered on tool startup.'))
  options, args = parser.parse_args()

  if options.verbose:
    logging_utils.config_root(logging.DEBUG)
  else:
    logging_utils.config_root()

  if not options.test:
    parser.error('--test not specified')

  # If --build_dir is provided, prepend it to the test executable if needed.
  if options.build_dir and not args[0].startswith(options.build_dir):
    args[0] = os.path.join(options.build_dir, args[0])

  test = WebRTCTest(options, args, options.test)
  return test.Run()

if __name__ == '__main__':
  # We do this so the user can write -t <binary> instead of -t cmdline <binary>.
  if '-t' in sys.argv:
    sys.argv.insert(sys.argv.index('-t') + 1, 'cmdline')
  elif '--test' in sys.argv:
    sys.argv.insert(sys.argv.index('--test') + 1, 'cmdline')
  return_code = main(sys.argv)
  sys.exit(return_code)
