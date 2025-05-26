#pragma once

#include <cstdint>
#include <fstream>
#include <mutex>

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
  explicit Tracer(const char *filename) : m_ofstream(filename, std::ios::out) { m_ofstream << "{\"traceEvents\": ["; };

  ~Tracer() { m_ofstream << "]}"; }

  void writeBeginDurationEvent(const char *name) {
    writeDurationEvent(name, "function", "B", getProcessID(), getThreadID(), getTimeStamp());
  }

  void writeEndDurationEvent(const char *name) {
    writeDurationEvent(name, "function", "E", getProcessID(), getThreadID(), getTimeStamp());
  }

  void writeInstantEvent(const char *name) {
    writeInstantEvent(name, "function", "i", getProcessID(), getThreadID(), getTimeStamp(), "t");
  }

private:
  void writeDurationEvent(const char *name, const char *cat, const char *ph, std::uint64_t pid, std::uint64_t tid,
                          std::uint64_t ts) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_flag) {
      m_flag = false;
    } else {
      m_ofstream << ",";
    }
    m_ofstream << "{\"name\":\"" << name << "\",\"cat\":\"" << cat << "\",\"ph\":\"" << ph << "\",\"pid\":" << pid
               << ",\"tid\":" << tid << ",\"ts\":" << ts << "}";
  }

  void writeInstantEvent(const char *name, const char *cat, const char *ph, std::uint64_t pid, std::uint64_t tid,
                         std::uint64_t ts, const char *s) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_flag) {
      m_flag = false;
    } else {
      m_ofstream << ",";
    }
    m_ofstream << "{\"name\":\"" << name << "\",\"cat\":\"" << cat << "\",\"ph\":\"" << ph << "\",\"pid\":" << pid
               << ",\"tid\":" << tid << ",\"ts\":" << ts << ",\"s\":\"" << s << "\"}";
  }

  static uint64_t getThreadID() {
#if defined(_WIN32) || defined(_WIN64)
    return static_cast<uint64_t>(GetCurrentThreadId());
#elif defined(__linux__)
    return static_cast<uint64_t>(syscall(SYS_gettid));
#else
#error "Unknown Platform"
#endif
  }

  static uint64_t getProcessID() {
#if defined(_WIN32) || defined(_WIN64)
    return static_cast<uint64_t>(GetCurrentProcessId());
#elif defined(__linux__)
    return static_cast<uint64_t>(syscall(SYS_getpid));
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
  DurationEvent(Tracer &tracer, const char *name) : m_tracer(tracer), m_name(name) {
    m_tracer.writeBeginDurationEvent(m_name);
  }

  ~DurationEvent() { m_tracer.writeEndDurationEvent(m_name); }

private:
  Tracer &m_tracer;
  const char *m_name;
};

class InstantEvent {
public:
  InstantEvent(Tracer &tracer, const char *name) { tracer.writeInstantEvent(name); }
};
} // namespace tracing
