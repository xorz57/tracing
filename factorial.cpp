#include "tracing.hpp"

#include <cstdlib>

static tracing::Tracer s_tracer{"factorial.json"};

static std::uint64_t factorial(std::uint64_t n) {
  if (n <= 1) {
    const tracing::InstantEvent event{s_tracer, "factorial.case0"};
    return 1;
  }
  const tracing::DurationEvent event{s_tracer, "factorial.case1"};
  return n * factorial(n - 1);
}

int main(int argc, const char *argv[]) {
  if (argc != 2)
    return -1;
  const std::uint64_t n{std::stoull(argv[1])};
  factorial(n);
  return 0;
}
