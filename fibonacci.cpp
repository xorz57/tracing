#include "tracing.hpp"

#include <string>

static tracing::Tracer s_tracer{"fibonacci.json"};

static std::uint64_t fibonacci(std::uint64_t n) {
  if (n <= 1) {
    const tracing::InstantEvent event{s_tracer, "fibonacci.case0"};
    return n;
  }
  const tracing::DurationEvent event{s_tracer, "fibonacci.case1"};
  return fibonacci(n - 1) + fibonacci(n - 2);
}

int main(int argc, const char *argv[]) {
  if (argc != 2)
    return -1;
  const std::uint64_t n{std::stoull(argv[1])};
  fibonacci(n);
  return 0;
}
