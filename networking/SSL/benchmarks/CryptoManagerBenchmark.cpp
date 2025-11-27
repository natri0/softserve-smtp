#include <benchmark/benchmark.h>
#include "../include/CryptoManager.h"

static void Encryption(benchmark::State& state) {
  const std::vector<unsigned char> key(32, 0xAB);
  const smtp::ssl::CryptoManager manager(key);
  const std::string plaintext(state.range(0), 'A');
  
  for (auto _ : state) {
    auto ciphertext = manager.encrypt(plaintext);
    benchmark::DoNotOptimize(ciphertext);
  }
}
BENCHMARK(Encryption)->Range(64, 8<<10);

static void Decryption(benchmark::State& state) {
  const std::vector<unsigned char> key(32, 0xAB);
  const smtp::ssl::CryptoManager manager(key);
  const std::string plaintext(state.range(0), 'A');
  const auto ciphertext = manager.encrypt(plaintext);
  
  for (auto _ : state) {
    auto decrypted = manager.decrypt(ciphertext);
    benchmark::DoNotOptimize(decrypted);
  }
}
BENCHMARK(Decryption)->Range(64, 8<<10);

BENCHMARK_MAIN();