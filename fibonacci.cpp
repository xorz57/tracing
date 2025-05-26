#include "tracing.hpp"

static tracing::Tracer s_tracer{"fibonacci.json"};

static std::uint64_t fibonacci(std::uint64_t n) {
  if (n <= 1) {
    tracing::InstantEvent event{s_tracer, "fibonacci.case0"};
    return n;
  }
  tracing::DurationEvent event{s_tracer, "fibonacci.case1"};
  return fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
  fibonacci(20);
  return 0;
}
