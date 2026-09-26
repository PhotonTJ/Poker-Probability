#pragma once

#include <array>
#include <string>
#include <vector>

#include "poker/card.hpp"

namespace poker {

enum class HandRank : int {
    HighCard = 0,
    OnePair,
    TwoPair,
    ThreeOfAKind,
    Straight,
    Flush,
    FullHouse,
    FourOfAKind,
    StraightFlush,
    RoyalFlush,
    Count
};

std::string to_string(HandRank r);

// A 5-card hand and its classification into the standard 10-category poker
// hand hierarchy.
//
// The original PokerProbability.cpp only had a handful of independent
// checks: is_flush, is_straight, is_straight_flush, and an "exercise"
// is_4of_akind that compared all five cards' ranks for equality -- which a
// real 52-card deck can never satisfy, since only 4 cards of any rank exist.
// That function could never return true. classify() replaces all of it with
// a single rank-count / suit-count histogram, the standard approach, which
// correctly distinguishes all ten categories including four-of-a-kind and
// full house, and separates a royal flush from an ordinary straight flush.
class Hand {
public:
    explicit Hand(std::vector<Card> cards);

    bool isFlush() const;
    bool isStraight() const;   // true for both A-2-3-4-5 and 10-J-Q-K-A
    HandRank classify() const;

    const std::vector<Card>& cards() const noexcept { return cards_; }

private:
    std::vector<Card> cards_;

    std::array<int, 14> rankCounts() const;   // indices 1..13 used
    std::array<int, 4> suitCounts() const;
};

} // namespace poker
