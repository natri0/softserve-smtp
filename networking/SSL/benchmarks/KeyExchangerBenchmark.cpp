#include <benchmark/benchmark.h>
#include "../include/KeyExchanger.h"

static void KeyPairGeneration(benchmark::State& state) {
  for (auto _ : state) {
    auto keys = smtp::ssl::KeyExchange::generateKeyPair();
    benchmark::DoNotOptimize(keys);
  }
}
BENCHMARK(KeyPairGeneration)->Unit(benchmark::kMillisecond);

static void DHExchange(benchmark::State& state) {
  auto [privKey1, pubKey1] = smtp::ssl::KeyExchange::generateKeyPair();
  auto [privKey2, pubKey2] = smtp::ssl::KeyExchange::generateKeyPair();
  
  for (auto _ : state) {
    auto secret = smtp::ssl::KeyExchange::performDHExchange(pubKey2, privKey1);
    benchmark::DoNotOptimize(secret);
  }
}
BENCHMARK(DHExchange)->Unit(benchmark::kMillisecond);

static void SessionKeyDerivation(benchmark::State& state) {
  const std::vector<unsigned char> sharedSecret(256, 0xFF);
  
  for (auto _ : state) {
    auto sessionKey = smtp::ssl::KeyExchange::deriveSessionKey(sharedSecret);
    benchmark::DoNotOptimize(sessionKey);
  }
}
BENCHMARK(SessionKeyDerivation);