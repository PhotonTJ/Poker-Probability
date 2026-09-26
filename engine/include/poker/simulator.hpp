#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <random>

#include "poker/deck.hpp"
#include "poker/hand.hpp"

namespace poker {

struct SimulationResult {
    std::size_t trials = 0;
    std::array<std::size_t, static_cast<std::size_t>(HandRank::Count)> counts{};

    double frequency(HandRank r) const;
};

// The exact combinatorial probability of each category in 5-card stud
// (52-choose-5 = 2,598,960 equally likely hands), for comparison against the
// Monte Carlo estimate.
double theoreticalProbability(HandRank r);

// Repeatedly shuffles a full 52-card deck and deals a 5-card hand, tallying
// which category each hand falls into. This is the same experiment as the
// original PokerProbability.cpp's main() loop (which only tracked flush,
// straight, straight-flush and the broken four-of-a-kind check via four
// hand-picked counters), generalized to all ten categories and moved out of
// main() so it can be driven from tests, from a CLI, or from the pybind11
// bindings.
class MonteCarloSimulator {
public:
    explicit MonteCarloSimulator(std::mt19937::result_type seed = std::random_device{}());

    // Runs `trials` independent deals. If `onBatch` is set and batchSize > 0,
    // it is invoked every `batchSize` trials with the running totals so a
    // caller can stream convergence progress (the FastAPI backend uses this,
    // releasing the GIL around the call, to push live updates to the
    // frontend chart).
    SimulationResult run(std::size_t trials,
                          std::size_t batchSize = 0,
                          const std::function<void(const SimulationResult&)>& onBatch = nullptr);

private:
    Deck deck_;
};

} // namespace poker
