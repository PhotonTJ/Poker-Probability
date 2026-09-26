#include "poker/hand.hpp"

#include <algorithm>
#include <stdexcept>

namespace poker {

std::string to_string(HandRank r) {
    switch (r) {
        case HandRank::HighCard: return "High Card";
        case HandRank::OnePair: return "One Pair";
        case HandRank::TwoPair: return "Two Pair";
        case HandRank::ThreeOfAKind: return "Three of a Kind";
        case HandRank::Straight: return "Straight";
        case HandRank::Flush: return "Flush";
        case HandRank::FullHouse: return "Full House";
        case HandRank::FourOfAKind: return "Four of a Kind";
        case HandRank::StraightFlush: return "Straight Flush";
        case HandRank::RoyalFlush: return "Royal Flush";
        default: return "Unknown";
    }
}

Hand::Hand(std::vector<Card> cards) : cards_(std::move(cards)) {
    if (cards_.size() != 5) {
        throw std::invalid_argument("Hand requires exactly 5 cards");
    }
}

std::array<int, 14> Hand::rankCounts() const {
    std::array<int, 14> counts{};
    for (const auto& c : cards_) {
        counts[static_cast<std::size_t>(c.rank().value())]++;
    }
    return counts;
}

std::array<int, 4> Hand::suitCounts() const {
    std::array<int, 4> counts{};
    for (const auto& c : cards_) {
        counts[static_cast<std::size_t>(c.suit())]++;
    }
    return counts;
}

bool Hand::isFlush() const {
    const Suit s = cards_.front().suit();
    return std::all_of(cards_.begin(), cards_.end(), [s](const Card& c) { return c.suit() == s; });
}

bool Hand::isStraight() const {
    std::array<int, 5> v{};
    for (std::size_t i = 0; i < 5; ++i) {
        v[i] = cards_[i].rank().value();
    }
    std::sort(v.begin(), v.end());

    const bool consecutive =
        v[0] == v[1] - 1 && v[1] == v[2] - 1 && v[2] == v[3] - 1 && v[3] == v[4] - 1;
    // Ace can also play high, as in 10-J-Q-K-A, which sorts to 1,10,11,12,13.
    const bool aceHigh = v[0] == 1 && v[1] == 10 && v[2] == 11 && v[3] == 12 && v[4] == 13;

    return consecutive || aceHigh;
}

HandRank Hand::classify() const {
    const auto ranks = rankCounts();
    const bool flush = isFlush();
    const bool straight = isStraight();

    int pairs = 0, trips = 0, quads = 0;
    for (int c : ranks) {
        if (c == 2) pairs++;
        else if (c == 3) trips++;
        else if (c == 4) quads++;
    }

    if (flush && straight) {
        const bool aceHighStraight =
            ranks[1] > 0 && ranks[10] > 0 && ranks[11] > 0 && ranks[12] > 0 && ranks[13] > 0;
        return aceHighStraight ? HandRank::RoyalFlush : HandRank::StraightFlush;
    }
    if (quads == 1) return HandRank::FourOfAKind;
    if (trips == 1 && pairs == 1) return HandRank::FullHouse;
    if (flush) return HandRank::Flush;
    if (straight) return HandRank::Straight;
    if (trips == 1) return HandRank::ThreeOfAKind;
    if (pairs == 2) return HandRank::TwoPair;
    if (pairs == 1) return HandRank::OnePair;
    return HandRank::HighCard;
}

} // namespace poker
