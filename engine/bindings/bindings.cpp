// Python bindings for the poker engine. The engine itself (card/deck/hand/
// simulator) has no idea Python exists; this file is the only place that
// does, which is the point of pybind11: the FastAPI backend gets the same
// C++ implementation that the doctest suite verified, not a reimplementation.
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <random>
#include <utility>
#include <vector>

#include "poker/card.hpp"
#include "poker/deck.hpp"
#include "poker/hand.hpp"
#include "poker/simulator.hpp"

namespace py = pybind11;
using namespace poker;

namespace {

py::dict resultToDict(const SimulationResult& r) {
    py::dict counts, frequencies, theoretical;
    for (int i = 0; i < static_cast<int>(HandRank::Count); ++i) {
        auto rank = static_cast<HandRank>(i);
        std::string name = to_string(rank);
        counts[py::str(name)] = r.counts[static_cast<std::size_t>(rank)];
        frequencies[py::str(name)] = r.frequency(rank);
        theoretical[py::str(name)] = theoreticalProbability(rank);
    }
    py::dict out;
    out["trials"] = r.trials;
    out["counts"] = counts;
    out["frequencies"] = frequencies;
    out["theoretical"] = theoretical;
    return out;
}

} // namespace

PYBIND11_MODULE(poker_engine, m) {
    m.doc() = "C++ poker hand-classification and Monte Carlo simulation engine";

    py::enum_<Suit>(m, "Suit")
        .value("SPADE", Suit::Spade)
        .value("HEART", Suit::Heart)
        .value("DIAMOND", Suit::Diamond)
        .value("CLUB", Suit::Club);

    py::enum_<HandRank>(m, "HandRank")
        .value("HIGH_CARD", HandRank::HighCard)
        .value("ONE_PAIR", HandRank::OnePair)
        .value("TWO_PAIR", HandRank::TwoPair)
        .value("THREE_OF_A_KIND", HandRank::ThreeOfAKind)
        .value("STRAIGHT", HandRank::Straight)
        .value("FLUSH", HandRank::Flush)
        .value("FULL_HOUSE", HandRank::FullHouse)
        .value("FOUR_OF_A_KIND", HandRank::FourOfAKind)
        .value("STRAIGHT_FLUSH", HandRank::StraightFlush)
        .value("ROYAL_FLUSH", HandRank::RoyalFlush);

    py::class_<Card>(m, "Card")
        .def(py::init([](Suit s, int rank) { return Card(s, Rank(rank)); }), py::arg("suit"),
             py::arg("rank"))
        .def_property_readonly("suit", &Card::suit)
        .def_property_readonly("rank", [](const Card& c) { return c.rank().value(); })
        .def("__repr__", [](const Card& c) { return to_string(c); });

    py::class_<Hand>(m, "Hand")
        .def(py::init([](std::vector<std::pair<Suit, int>> cards) {
            std::vector<Card> built;
            built.reserve(cards.size());
            for (auto& sr : cards) built.emplace_back(sr.first, Rank(sr.second));
            return Hand(std::move(built));
        }))
        .def("is_flush", &Hand::isFlush)
        .def("is_straight", &Hand::isStraight)
        .def("classify", &Hand::classify)
        .def("classify_name", [](const Hand& h) { return to_string(h.classify()); });

    py::class_<Deck>(m, "Deck")
        .def(py::init<>())
        .def(py::init<std::mt19937::result_type>(), py::arg("seed"))
        .def("reset", &Deck::reset)
        .def("shuffle", &Deck::shuffle)
        .def("remaining", &Deck::remaining)
        .def("deal", [](Deck& d, std::size_t n) {
            auto cards = d.deal(n);
            py::list out;
            for (auto& c : cards) {
                py::dict card;
                card["suit"] = c.suit();
                card["rank"] = c.rank().value();
                card["label"] = to_string(c);
                out.append(card);
            }
            return out;
        }, py::arg("n"));

    m.def("theoretical_probability", &theoreticalProbability, py::arg("rank"));
    m.def("hand_rank_name", [](HandRank r) { return to_string(r); }, py::arg("rank"));

    py::class_<MonteCarloSimulator>(m, "MonteCarloSimulator")
        .def(py::init<>())
        .def(py::init<std::mt19937::result_type>(), py::arg("seed"))
        .def(
            "run",
            [](MonteCarloSimulator& self, std::size_t trials, std::size_t batchSize,
               py::object onBatch) {
                SimulationResult result;
                {
                    // Release the GIL for the whole C++ loop -- this is the
                    // point of doing the heavy lifting in C++ at all -- and
                    // reacquire it only for the brief moments the periodic
                    // progress callback runs back in Python, so a FastAPI
                    // event loop calling this from a worker thread stays
                    // responsive to other requests while trials run.
                    py::gil_scoped_release release;
                    result = self.run(trials, batchSize, [&](const SimulationResult& r) {
                        if (!onBatch.is_none()) {
                            py::gil_scoped_acquire acquire;
                            onBatch(resultToDict(r));
                        }
                    });
                }
                return resultToDict(result);
            },
            py::arg("trials"), py::arg("batch_size") = 0, py::arg("on_batch") = py::none());
}
