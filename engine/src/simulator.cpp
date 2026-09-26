#include "poker/simulator.hpp"

namespace poker {

double SimulationResult::frequency(HandRank r) const {
    if (trials == 0) return 0.0;
    return static_cast<double>(counts[static_cast<std::size_t>(r)]) / static_cast<double>(trials);
}

double theoreticalProbability(HandRank r) {
    // Standard 5-card-stud combinatorics, out of C(52,5) = 2,598,960 hands.
    static constexpr double kTotal = 2598960.0;
    switch (r) {
        case HandRank::RoyalFlush: return 4.0 / kTotal;
        case HandRank::StraightFlush: return 36.0 / kTotal;
        case HandRank::FourOfAKind: return 624.0 / kTotal;
        case HandRank::FullHouse: return 3744.0 / kTotal;
        case HandRank::Flush: return 5108.0 / kTotal;
        case HandRank::Straight: return 10200.0 / kTotal;
        case HandRank::ThreeOfAKind: return 54912.0 / kTotal;
        case HandRank::TwoPair: return 123552.0 / kTotal;
        case HandRank::OnePair: return 1098240.0 / kTotal;
        case HandRank::HighCard: return 1302540.0 / kTotal;
        default: return 0.0;
    }
}

MonteCarloSimulator::MonteCarloSimulator(std::mt19937::result_type seed) : deck_(seed) {}

SimulationResult MonteCarloSimulator::run(std::size_t trials, std::size_t batchSize,
                                           const std::function<void(const SimulationResult&)>& onBatch) {
    SimulationResult result;
    for (std::size_t t = 1; t <= trials; ++t) {
        deck_.reset();
        deck_.shuffle();
        Hand hand(deck_.deal(5));
        result.counts[static_cast<std::size_t>(hand.classify())]++;
        result.trials = t;

        if (onBatch && batchSize > 0 && (t % batchSize == 0 || t == trials)) {
            onBatch(result);
        }
    }
    return result;
}

} // namespace poker
