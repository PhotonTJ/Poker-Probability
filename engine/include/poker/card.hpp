#pragma once

#include <array>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>

namespace poker {

enum class Suit : std::uint8_t { Spade, Heart, Diamond, Club };

constexpr std::array<Suit, 4> kAllSuits{Suit::Spade, Suit::Heart, Suit::Diamond, Suit::Club};

std::string to_string(Suit s);
std::ostream& operator<<(std::ostream& os, Suit s);

// A card rank, 1..13 (1 = Ace, 11 = Jack, 12 = Queen, 13 = King). This is the
// same range-validated value type as the original PokerProbability.cpp's
// `pips` class, kept intact because the invariant (assert(v > 0 && v < 14))
// was already the right idea -- it is just promoted from assert() to a
// thrown exception so a bad value fails loudly through the Python bindings
// too, instead of aborting the whole process.
class Rank {
public:
    explicit Rank(int value) : value_(value) {
        if (value_ < 1 || value_ > 13) {
            throw std::out_of_range("Rank must be in [1, 13]");
        }
    }

    int value() const noexcept { return value_; }

    friend bool operator==(const Rank& a, const Rank& b) { return a.value_ == b.value_; }
    friend bool operator!=(const Rank& a, const Rank& b) { return !(a == b); }
    friend bool operator<(const Rank& a, const Rank& b) { return a.value_ < b.value_; }

private:
    int value_;
};

std::string to_string(const Rank& r);
std::ostream& operator<<(std::ostream& os, const Rank& r);

class Card {
public:
    Card() : suit_(Suit::Spade), rank_(1) {}
    Card(Suit s, Rank r) : suit_(s), rank_(r) {}

    Suit suit() const noexcept { return suit_; }
    Rank rank() const noexcept { return rank_; }

private:
    Suit suit_;
    Rank rank_;
};

std::string to_string(const Card& c);
std::ostream& operator<<(std::ostream& os, const Card& c);

} // namespace poker
