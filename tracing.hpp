/*
  MIT License

  Copyright(c) 2024 George Fotopoulos

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files(the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions :

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.
*/

#pragma once

#include <chrono>
#include <fstream>
#include <mutex>

#include <nlohmann/json.hpp>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace tracing {
class Tracer final {
public:
  Tracer(const Tracer &) = delete;
  Tracer(Tracer &&) = delete;

  static Tracer &get_instance() {
    static Tracer instance;
    return instance;
  }

  static std::uint64_t get_timestamp() {
    const auto now = std::chrono::steady_clock::now();
    const auto duration = now.time_since_epoch();
    const auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    return static_cast<std::uint64_t>(microseconds.count());
  }

  static std::uint64_t get_pid() {
#ifdef _WIN32
    return GetCurrentProcessId();
#else
    return static_cast<std::uint64_t>(getpid());
#endif
  }

  static std::uint64_t get_tid() {
#ifdef _WIN32
    return GetCurrentThreadId();
#else
    return static_cast<std::uint64_t>(gettid());
#endif
  }

  void trace_instant_event(const char *name, const char *phase, std::uint64_t pid, std::uint64_t tid, std::uint64_t timestamp) {
    std::lock_guard<std::mutex> lock(m_data_mutex);
    nlohmann::json event;
    event["name"] = name;
    event["cat"] = "function";
    event["ph"] = phase;
    event["pid"] = pid;
    event["tid"] = tid;
    event["ts"] = timestamp;
    event["s"] = "t";
    m_data["traceEvents"].emplace_back(event);
  }

  void trace_duration_event(const char *name, const char *phase, std::uint64_t pid, std::uint64_t tid, std::uint64_t timestamp, nlohmann::json args) {
    std::lock_guard<std::mutex> lock(m_data_mutex);
    nlohmann::json event;
    event["name"] = name;
    event["cat"] = "function";
    event["ph"] = phase;
    event["pid"] = pid;
    event["tid"] = tid;
    event["ts"] = timestamp;
    event["args"] = std::move(args);
    m_data["traceEvents"].emplace_back(event);
  }

  void trace_duration_event(const char *name, const char *phase, std::uint64_t pid, std::uint64_t tid, std::uint64_t timestamp) {
    std::lock_guard<std::mutex> lock(m_data_mutex);
    nlohmann::json event;
    event["name"] = name;
    event["cat"] = "function";
    event["ph"] = phase;
    event["pid"] = pid;
    event["tid"] = tid;
    event["ts"] = timestamp;
    m_data["traceEvents"].emplace_back(event);
  }

  void dump(const char *filename, int indent = 4) {
    std::lock_guard<std::mutex> lock(m_data_mutex);
    std::ofstream ofs(filename);
    ofs << m_data.dump(indent);
  }

private:
  Tracer() = default;
  ~Tracer() = default;

  nlohmann::json m_data;
  std::mutex m_data_mutex;
};

static void instant_event(const char *name) {
  Tracer::get_instance().trace_instant_event(name, "i", Tracer::get_pid(), Tracer::get_tid(), Tracer::get_timestamp());
}

class DurationEvent final {
public:
  DurationEvent(const char *name, nlohmann::json args) : m_name(std::move(name)) {
    Tracer::get_instance().trace_duration_event(name, "B", Tracer::get_pid(), Tracer::get_tid(), Tracer::get_timestamp(), std::move(args));
  }

  DurationEvent(const char *name) : m_name(name) {
    Tracer::get_instance().trace_duration_event(name, "B", Tracer::get_pid(), Tracer::get_tid(), Tracer::get_timestamp());
  }

  ~DurationEvent() {
    Tracer::get_instance().trace_duration_event(m_name, "E", Tracer::get_pid(), Tracer::get_tid(), Tracer::get_timestamp());
  }

private:
  const char *m_name;
};
} // namespace tracing
