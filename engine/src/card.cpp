#include "poker/card.hpp"

#include <sstream>

namespace poker {

std::string to_string(Suit s) {
    switch (s) {
        case Suit::Spade: return "spade";
        case Suit::Heart: return "heart";
        case Suit::Diamond: return "diamond";
        case Suit::Club: return "club";
    }
    return "unknown suit";
}

std::ostream& operator<<(std::ostream& os, Suit s) { return os << to_string(s); }

std::string to_string(const Rank& r) {
    switch (r.value()) {
        case 1: return "ace";
        case 11: return "jack";
        case 12: return "queen";
        case 13: return "king";
        default: return std::to_string(r.value());
    }
}

std::ostream& operator<<(std::ostream& os, const Rank& r) { return os << to_string(r); }

std::string to_string(const Card& c) {
    std::ostringstream oss;
    oss << c.rank() << " of " << c.suit() << 's';
    return oss.str();
}

std::ostream& operator<<(std::ostream& os, const Card& c) { return os << to_string(c); }

} // namespace poker
