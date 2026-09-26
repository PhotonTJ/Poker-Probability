#include "poker/deck.hpp"

#include <algorithm>
#include <stdexcept>

namespace poker {

namespace {

std::vector<Card> makeOrderedDeck() {
    std::vector<Card> cards;
    cards.reserve(52);
    for (Suit s : kAllSuits) {
        for (int v = 1; v <= 13; ++v) {
            cards.emplace_back(s, Rank(v));
        }
    }
    return cards;
}

} // namespace

Deck::Deck() : cards_(makeOrderedDeck()), rng_(std::random_device{}()) {}

Deck::Deck(std::mt19937::result_type seed) : cards_(makeOrderedDeck()), rng_(seed) {}

void Deck::reset() { cards_ = makeOrderedDeck(); }

void Deck::shuffle() { std::shuffle(cards_.begin(), cards_.end(), rng_); }

std::vector<Card> Deck::deal(std::size_t n) {
    if (n > cards_.size()) {
        throw std::out_of_range("Deck::deal: not enough cards remaining");
    }
    std::vector<Card> hand(cards_.end() - static_cast<std::ptrdiff_t>(n), cards_.end());
    cards_.erase(cards_.end() - static_cast<std::ptrdiff_t>(n), cards_.end());
    return hand;
}

} // namespace poker
