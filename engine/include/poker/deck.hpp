#pragma once

#include <cstddef>
#include <random>
#include <vector>

#include "poker/card.hpp"

namespace poker {

// Owns the 52-card deck and every operation that touches it. The original
// PokerProbability.cpp built the deck with a free `init_deck(vector<card>&)`
// function and then shuffled/dealt by hand in main() with std::random_shuffle
// (deprecated, and removed from the language in C++17). This class replaces
// both: construction always yields an ordered 52-card deck, shuffle() uses a
// std::mt19937 engine with std::shuffle, and deal(n) is the only way cards
// leave the deck, so "52 cards, no duplicates, no leaks" is a class
// invariant instead of something main() has to get right by hand.
class Deck {
public:
    Deck();
    explicit Deck(std::mt19937::result_type seed);

    void reset();   // restore all 52 cards, in order
    void shuffle();

    // Removes and returns the top n cards.
    std::vector<Card> deal(std::size_t n);

    std::size_t remaining() const noexcept { return cards_.size(); }

private:
    std::vector<Card> cards_;
    std::mt19937 rng_;
};

} // namespace poker
