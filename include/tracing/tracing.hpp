// MIT License
//
// Copyright (c) 2025 Georgios Fotopoulos
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstdint>
#include <fstream>
#include <mutex>
#include <string_view>

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#elif defined(__linux__)
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#else
#error "Unknown Platform"
#endif

namespace tracing {
class Tracer {
public:
  explicit Tracer(const std::string_view filename) : m_ofstream(filename.data(), std::ios::out) { m_ofstream << "{\"traceEvents\": ["; };

  ~Tracer() { m_ofstream << "]}"; }

  void writeBeginDurationEvent(const std::string_view name) {
    writeDurationEvent(name, "function", "B", getProcessID(), getThreadID(), getTimeStamp());
  }

  void writeEndDurationEvent(const std::string_view name) {
    writeDurationEvent(name, "function", "E", getProcessID(), getThreadID(), getTimeStamp());
  }

  void writeInstantEvent(const std::string_view name) {
    writeInstantEvent(name, "function", "i", getProcessID(), getThreadID(), getTimeStamp(), "t");
  }

private:
  void writeDurationEvent(const std::string_view name, const std::string_view cat, const std::string_view ph, std::uint64_t pid, std::uint64_t tid, std::uint64_t ts) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_flag) {
      m_flag = false;
    } else {
      m_ofstream << ",";
    }
    m_ofstream << "{\"name\":\"" << name << "\",\"cat\":\"" << cat << "\",\"ph\":\"" << ph << "\",\"pid\":" << pid << ",\"tid\":" << tid << ",\"ts\":" << ts << "}";
  }

  void writeInstantEvent(const std::string_view name, const std::string_view cat, const std::string_view ph, std::uint64_t pid, std::uint64_t tid, std::uint64_t ts, const std::string_view s) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_flag) {
      m_flag = false;
    } else {
      m_ofstream << ",";
    }
    m_ofstream << "{\"name\":\"" << name << "\",\"cat\":\"" << cat << "\",\"ph\":\"" << ph << "\",\"pid\":" << pid << ",\"tid\":" << tid << ",\"ts\":" << ts << ",\"s\":\"" << s << "\"}";
  }

  static std::uint64_t getThreadID() {
#if defined(_WIN32) || defined(_WIN64)
    return static_cast<std::uint64_t>(GetCurrentThreadId());
#elif defined(__linux__)
    return static_cast<std::uint64_t>(syscall(SYS_gettid));
#else
#error "Unknown Platform"
#endif
  }

  static std::uint64_t getProcessID() {
#if defined(_WIN32) || defined(_WIN64)
    return static_cast<std::uint64_t>(GetCurrentProcessId());
#elif defined(__linux__)
    return static_cast<std::uint64_t>(syscall(SYS_getpid));
#else
#error "Unknown Platform"
#endif
  }

  static std::uint64_t getTimeStamp() {
    const auto now = std::chrono::steady_clock::now();
    const auto duration = now.time_since_epoch();
    const auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    return static_cast<std::uint64_t>(duration_us.count());
  }

  bool m_flag{true};
  std::ofstream m_ofstream;
  std::mutex m_mutex;
};

class DurationEvent {
public:
  DurationEvent(Tracer &tracer, const std::string_view name) : m_tracer(tracer), m_name(name) {
    m_tracer.writeBeginDurationEvent(m_name);
  }

  ~DurationEvent() { m_tracer.writeEndDurationEvent(m_name); }

private:
  Tracer &m_tracer;
  const std::string_view m_name;
};

class InstantEvent {
public:
  InstantEvent(Tracer &tracer, const std::string_view name) { tracer.writeInstantEvent(name); }
};
} // namespace tracing
