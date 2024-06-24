// calculating the probability of finding 5 card flush stud in poker
// using abstract data types to have card
// will use vectors and shuffles from stl
// middle level straight flush is 6,7,8,9,10
// royal straight flush is 10,jack,king,queen,ace
#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <ctime>

using namespace std;


enum class suit : short { SPADE, HEART, DIAMOND, CLUB };


ostream& operator<<(ostream& os, const suit& s) {
	switch (s) {
	case suit::SPADE: os << "spade"; break;
	case suit::HEART: os << "heart"; break;
	case suit::DIAMOND: os << "diamond"; break;
	case suit::CLUB: os << "club"; break;
	default: os << "unknown suit"; break;
	}
	return os;
}

/*
a king=13, Queen=12, etc */

class pips {
public:
	pips(int val) : v(val) { assert(v > 0 && v < 14); }
	friend ostream& operator<<(ostream& os, const pips& p);
	int get_pips() const { return v; }
private:
	int v;
};

class card {
public:
	card() : s(suit::SPADE), v(1) {}
	card(suit s, pips v) : s(s), v(v) {}
	friend ostream& operator<<(ostream& os, const card& p);
	suit get_suit() const { return s; }
	pips get_pips() const { return v; }
private:
	suit s;
	pips v;
};

ostream& operator<<(ostream& os, const card& c) {
	os << c.v << " of " << c.s << 's'; //  presumes << overloaded for pips and suit
	return os;
}

ostream& operator<<(ostream& os, const pips& p) {
	switch (p.get_pips()) {
	case 13: os << "king"; break;
	case 12: os << "queen"; break;
	case 11: os << "jack"; break;
	case 1: os << "ace"; break;
	default: os << p.get_pips(); break;
	}
	return os;
}

void init_deck(vector<card>& d) {
	int suitsize = 13;
	int packsize = 52;
	for(int i = 1; i <= packsize; i += suitsize) {
		suit type;
		if (i < suitsize * 1)
			type = suit::SPADE;
		else if(i < suitsize * 2)
			type = suit::HEART;
		else if (i < suitsize * 3)
			type = suit::DIAMOND;
		else if (i < suitsize * 4)
			type = suit::CLUB;

		for (int j = 0; j < suitsize; ++j) {
			card c(type, j+1);
			d[i + j - 1] = c;
		}
	}
}

void print(vector<card>& deck) {
	for (const auto& p : deck) {
		cout << p;
	}
	cout << endl;
}


// a hand is 5 cards
bool is_flush(vector<card> hand) {
	const suit s = hand[0].get_suit();
	for (auto p = hand.begin() + 1; p != hand.end(); ++p) {
		if (s != p->get_suit()) {
			return false;
		}
	}
	return true;
}

bool is_straight(vector<card>& hand) {
	int pips_v[5], i = 0;
	for (auto& p : hand) {
		pips_v[i++] = (p.get_pips()).get_pips();
	}
	sort(pips_v, pips_v + 5);
	if (pips_v[0] != 1) // non-aces
		return (pips_v[0] == pips_v[1] - 1 && pips_v[1] == pips_v[2] - 1)
		&& (pips_v[2] == pips_v[3] - 1 && pips_v[3] == pips_v[4] - 1);
	else {  // aces have a special logic
		return (pips_v[0] == pips_v[1] - 1 && pips_v[1] == pips_v[2] - 1)
			&& (pips_v[2] == pips_v[3] - 1 && pips_v[3] == pips_v[4] - 1)
			|| (pips_v[1] == 10) && (pips_v[2] == 11) && (pips_v[3] == 12)
			&& (pips_v[4] == 13);
	}
}

// so straight flush is easy
bool is_straight_flush(vector<card>& hand) {
	return is_flush(hand) && is_straight(hand);
}

// Exercise - implement is_4of_akind
bool is_4of_akind(vector<card>& hand) {
	return hand[0].get_pips().get_pips() == hand[1].get_pips().get_pips()
		&& hand[1].get_pips().get_pips() == hand[2].get_pips().get_pips()
		&& hand[2].get_pips().get_pips() == hand[3].get_pips().get_pips()
		&& hand[3].get_pips().get_pips() == hand[4].get_pips().get_pips();
}

int main() {

	// setup simulation
	vector<card> deck(52);
	srand(static_cast<unsigned int>(time(0)));
	init_deck(deck);
	int how_many;
	int flush_count = 0;
	int str_count = 0;
	int str_flush_count = 0;
	int four_of_akind_count = 0;
	cout << "How many shuffles? ";
	cin >> how_many;

	for (int loop = 0; loop < how_many; ++loop) {
		random_shuffle(deck.begin(), deck.end());
		vector<card> hand(5);
		int i = 0;
		for (auto p = deck.begin(); i < 5; ++p) {
			hand[i] = *p;
			cout << "hand: " << hand[i] << endl;
			++i;
		}

		if (is_flush(hand))
			flush_count++;
		if (is_straight(hand))
			str_count++;
		if (is_straight_flush(hand))
			str_flush_count++;
		if (is_4of_akind(hand))
			four_of_akind_count++;
	}

	cout << "Flushes " << flush_count << " out of " << how_many << endl;
	cout << "Staights " << str_count << " out of " << how_many << endl;
	cout << "Straight flushes " << str_flush_count << " out of " << how_many << endl;
	cout << "Four of a kinds " << four_of_akind_count << " out of " << how_many << endl;
}