// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/platform/scheduler/common/thread_load_tracker.h"
//rrw
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
// rrw
#include <algorithm>

namespace blink {
namespace scheduler {

ThreadLoadTracker::ThreadLoadTracker(base::TimeTicks now,
                                     const Callback& callback,
                                     base::TimeDelta reporting_interval)
    : time_(now),
      thread_state_(ThreadState::kPaused),
      last_state_change_time_(now),
      reporting_interval_(reporting_interval),
      callback_(callback) {
  next_reporting_time_ = now + reporting_interval_;
}

ThreadLoadTracker::~ThreadLoadTracker() = default;

void ThreadLoadTracker::Pause(base::TimeTicks now) {
  Advance(now, TaskState::kIdle);
  thread_state_ = ThreadState::kPaused;

  Reset(now);
}

void ThreadLoadTracker::Resume(base::TimeTicks now) {
  Advance(now, TaskState::kIdle);
  thread_state_ = ThreadState::kActive;

  Reset(now);
}

void ThreadLoadTracker::Reset(base::TimeTicks now) {
  last_state_change_time_ = now;
  next_reporting_time_ = now + reporting_interval_;
  run_time_inside_window_ = base::TimeDelta();
}

void ThreadLoadTracker::RecordTaskTime(base::TimeTicks start_time,
                                       base::TimeTicks end_time) {
  start_time = std::max(last_state_change_time_, start_time);
  end_time = std::max(last_state_change_time_, end_time);

  Advance(start_time, TaskState::kIdle);
  Advance(end_time, TaskState::kTaskRunning);
}

void ThreadLoadTracker::RecordIdle(base::TimeTicks now) {
  Advance(now, TaskState::kIdle);
}

namespace {

// Calculates length of intersection of two time intervals.
base::TimeDelta Intersection(base::TimeTicks left1,
                             base::TimeTicks right1,
                             base::TimeTicks left2,
                             base::TimeTicks right2) {
  DCHECK_LT(left1, right1);
  DCHECK_LT(left2, right2);
  base::TimeTicks left = std::max(left1, left2);
  base::TimeTicks right = std::min(right1, right2);

  if (left <= right)
    return right - left;

  return base::TimeDelta();
}

}  // namespace

//rrw
double tmpLoad;
char loadStr[512];
wchar_t MsgInfo[512];
int sampleCnt;
// rrw

void ThreadLoadTracker::Advance(base::TimeTicks now, TaskState task_state) {
  // This function advances |time_| to now and calls |callback_|
  // when appropriate.
  DCHECK_LE(time_, now);

  if (thread_state_ == ThreadState::kPaused) {
    // If the load tracker is paused, bail out early.
    time_ = now;
    return;
  }

  while (time_ < now) {
    // Advance time_ to the earliest of following:
    // a) time to call |callback_|
    // b) requested time to forward (|now|).
    base::TimeTicks next_current_time = std::min(next_reporting_time_, now);

    base::TimeDelta delta = next_current_time - time_;

    // Keep a running total of the time spent running tasks within the window
    // and the total time.
    if (task_state == TaskState::kTaskRunning) {
      run_time_inside_window_ +=
          Intersection(next_reporting_time_ - reporting_interval_,
                       next_reporting_time_, time_, time_ + delta);
    }

    time_ = next_current_time;
    // rrw
    tmpLoad = Load();
    if (tmpLoad >= 0.10) {
      if (0 != sampleCnt) {
        itoa(sampleCnt, loadStr, 10);
        strcat(loadStr, " less 10 percent capacity used.\n");
        mbstowcs(MsgInfo, loadStr, 128);
        OutputDebugStringW(MsgInfo);
      }
      sampleCnt = 0;

      gcvt(tmpLoad, 3, loadStr);
      strcat(loadStr, " percent capacity used.\n");
      OutputDebugStringA(loadStr);
    } else {
      sampleCnt++;
    }

    if (time_ == next_reporting_time_) {
      // Call |callback_| if need and update next callback time.
      if (thread_state_ == ThreadState::kActive) {
        //rrw
        callback_.Run(time_, tmpLoad); // Load());
        DCHECK_EQ(thread_state_, ThreadState::kActive);
      }
      next_reporting_time_ += reporting_interval_;
      run_time_inside_window_ = base::TimeDelta();
    }
  }
}

double ThreadLoadTracker::Load() {
  return run_time_inside_window_ / reporting_interval_;
}

}  // namespace scheduler
}  // namespace blink
