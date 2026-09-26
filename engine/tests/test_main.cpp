#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <cmath>
#include <set>
#include <string>

#include "poker/card.hpp"
#include "poker/deck.hpp"
#include "poker/hand.hpp"
#include "poker/simulator.hpp"

using namespace poker;

namespace {

Card C(Suit s, int r) { return Card(s, Rank(r)); }

Hand makeHand(std::initializer_list<Card> cards) { return Hand(std::vector<Card>(cards)); }

} // namespace

TEST_CASE("Rank rejects out-of-range values") {
    CHECK_NOTHROW(Rank(1));
    CHECK_NOTHROW(Rank(13));
    CHECK_THROWS_AS(Rank(0), std::out_of_range);
    CHECK_THROWS_AS(Rank(14), std::out_of_range);
}

TEST_CASE("Deck deals 52 unique cards and no more") {
    Deck deck(/*seed=*/12345);
    deck.shuffle();

    std::set<std::pair<int, int>> seen;
    for (int i = 0; i < 10; ++i) {
        auto hand = deck.deal(5);
        REQUIRE(hand.size() == 5);
        for (const auto& c : hand) {
            auto key = std::make_pair(static_cast<int>(c.suit()), c.rank().value());
            CHECK(seen.insert(key).second); // must not have been dealt already
        }
    }
    CHECK(deck.remaining() == 2);
    CHECK_THROWS_AS(deck.deal(5), std::out_of_range);
}

TEST_CASE("Hand::classify recognizes every category") {
    SUBCASE("royal flush") {
        auto h = makeHand({C(Suit::Spade, 1), C(Suit::Spade, 13), C(Suit::Spade, 12),
                            C(Suit::Spade, 11), C(Suit::Spade, 10)});
        CHECK(h.classify() == HandRank::RoyalFlush);
    }
    SUBCASE("straight flush, not ace-high") {
        auto h = makeHand({C(Suit::Heart, 4), C(Suit::Heart, 5), C(Suit::Heart, 6),
                            C(Suit::Heart, 7), C(Suit::Heart, 8)});
        CHECK(h.classify() == HandRank::StraightFlush);
    }
    SUBCASE("four of a kind") {
        auto h = makeHand({C(Suit::Spade, 9), C(Suit::Heart, 9), C(Suit::Diamond, 9),
                            C(Suit::Club, 9), C(Suit::Heart, 2)});
        CHECK(h.classify() == HandRank::FourOfAKind);
    }
    SUBCASE("full house") {
        auto h = makeHand({C(Suit::Spade, 3), C(Suit::Heart, 3), C(Suit::Diamond, 3),
                            C(Suit::Club, 8), C(Suit::Heart, 8)});
        CHECK(h.classify() == HandRank::FullHouse);
    }
    SUBCASE("flush") {
        auto h = makeHand({C(Suit::Club, 2), C(Suit::Club, 5), C(Suit::Club, 9),
                            C(Suit::Club, 11), C(Suit::Club, 13)});
        CHECK(h.classify() == HandRank::Flush);
    }
    SUBCASE("ace-low straight (wheel)") {
        auto h = makeHand({C(Suit::Spade, 1), C(Suit::Heart, 2), C(Suit::Diamond, 3),
                            C(Suit::Club, 4), C(Suit::Spade, 5)});
        CHECK(h.isStraight());
        CHECK(h.classify() == HandRank::Straight);
    }
    SUBCASE("ace-high straight (broadway)") {
        auto h = makeHand({C(Suit::Spade, 10), C(Suit::Heart, 11), C(Suit::Diamond, 12),
                            C(Suit::Club, 13), C(Suit::Spade, 1)});
        CHECK(h.isStraight());
        CHECK(h.classify() == HandRank::Straight);
    }
    SUBCASE("three of a kind") {
        auto h = makeHand({C(Suit::Spade, 6), C(Suit::Heart, 6), C(Suit::Diamond, 6),
                            C(Suit::Club, 2), C(Suit::Heart, 9)});
        CHECK(h.classify() == HandRank::ThreeOfAKind);
    }
    SUBCASE("two pair") {
        auto h = makeHand({C(Suit::Spade, 6), C(Suit::Heart, 6), C(Suit::Diamond, 9),
                            C(Suit::Club, 9), C(Suit::Heart, 2)});
        CHECK(h.classify() == HandRank::TwoPair);
    }
    SUBCASE("one pair") {
        auto h = makeHand({C(Suit::Spade, 6), C(Suit::Heart, 6), C(Suit::Diamond, 9),
                            C(Suit::Club, 4), C(Suit::Heart, 2)});
        CHECK(h.classify() == HandRank::OnePair);
    }
    SUBCASE("high card") {
        auto h = makeHand({C(Suit::Spade, 2), C(Suit::Heart, 5), C(Suit::Diamond, 9),
                            C(Suit::Club, 11), C(Suit::Heart, 13)});
        CHECK(h.classify() == HandRank::HighCard);
    }
    SUBCASE("not a straight: four of a kind can't fool isStraight") {
        auto h = makeHand({C(Suit::Spade, 9), C(Suit::Heart, 9), C(Suit::Diamond, 9),
                            C(Suit::Club, 9), C(Suit::Heart, 2)});
        CHECK_FALSE(h.isStraight());
    }
}

TEST_CASE("theoreticalProbability sums to 1 over all ten categories") {
    double total = 0.0;
    for (int i = 0; i < static_cast<int>(HandRank::Count); ++i) {
        total += theoreticalProbability(static_cast<HandRank>(i));
    }
    CHECK(total == doctest::Approx(1.0).epsilon(1e-9));
}

TEST_CASE("Monte Carlo simulation converges to the closed-form probabilities") {
    // Large enough trial count that an 8-standard-deviation band around the
    // binomial expectation is both statistically meaningful and, for every
    // category (including the rarest, the royal flush), astronomically
    // unlikely to fail by chance -- this is a convergence check, not an
    // exact-count check, so it stays stable across compilers/stdlibs even
    // though std::shuffle's exact sequence for a given seed is not part of
    // the C++ standard.
    constexpr std::size_t kTrials = 2'000'000;
    MonteCarloSimulator sim(/*seed=*/2026);
    SimulationResult result = sim.run(kTrials);

    REQUIRE(result.trials == kTrials);

    std::size_t total = 0;
    for (auto c : result.counts) total += c;
    CHECK(total == kTrials);

    for (int i = 0; i < static_cast<int>(HandRank::Count); ++i) {
        auto rank = static_cast<HandRank>(i);
        double p = theoreticalProbability(rank);
        double expected = p * static_cast<double>(kTrials);
        double stderrOfCount = std::sqrt(static_cast<double>(kTrials) * p * (1.0 - p));
        double tolerance = std::max(50.0, 8.0 * stderrOfCount);

        double observed = static_cast<double>(result.counts[static_cast<std::size_t>(rank)]);
        INFO("category = ", to_string(rank), ", observed = ", observed, ", expected = ", expected,
             ", tolerance = ", tolerance);
        CHECK(std::abs(observed - expected) <= tolerance);
    }
}
