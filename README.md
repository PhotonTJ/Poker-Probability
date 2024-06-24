# Poker-Probability
This project is designed to calculate the probability of obtaining a flush in a 5-card stud poker game. By simulating the game, it leverages abstract data types (ADTs) to model cards and uses vectors and shuffle operations from the C++ Standard Template Library (STL) to handle deck manipulation and card distribution. This calculator provides an efficient and flexible method to explore and understand the likelihood of specific poker hands in a 5-card setting.

Features
Card ADT: Represents individual playing cards, encapsulating suit and rank, ensuring a clean and modular approach.
Deck Operations: Uses vectors to represent a deck of cards, allowing dynamic and efficient card handling.
Shuffling: Utilizes the std::shuffle function for random deck shuffling, simulating realistic card distributions.
Probability Calculation: Simulates multiple poker hands to statistically estimate the probability of achieving a flush.
Configurable Simulations: Allows adjustment of the number of simulation iterations for more precise probability estimates.
Dependencies

C++ Standard Library: Utilizes vectors and shuffle functions.
C++11 or Later: Requires modern C++ features for random number generation and ADT support.

Usage
The simulation will output the estimated probability of drawing a flush in 5-card stud poker after running a specified number of trials. Modify the number of iterations in the source code to increase accuracy or speed up the computation.

Contribution
Contributions are welcome! Feel free to fork the repository and submit pull requests with improvements or additional features.

License
This project is licensed under the MIT License. See the LICENSE file for more details.
