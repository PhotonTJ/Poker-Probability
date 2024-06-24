This project calculates the probability of obtaining a flush in 5-card stud poker using Monte Carlo simulation. It employs abstract data types (ADTs) to model cards and utilizes vectors and shuffle operations from the C++ Standard Template Library (STL) for deck manipulation and card distribution. The Monte Carlo approach enables precise probability estimation by simulating a vast number of poker hands.

Features
Card ADT: Represents individual playing cards, encapsulating suit and rank for a clean and modular design.
Deck Operations: Uses vectors to represent and manipulate a deck of cards efficiently.
Shuffling: Employs the std::shuffle function for realistic card shuffling.
Monte Carlo Simulation: Utilizes Monte Carlo methods to estimate the probability of obtaining a flush by simulating numerous poker hands.
Configurable Simulations: Allows adjustment of the number of Monte Carlo iterations for more precise probability estimates.

Dependencies
C++ Standard Library: Utilizes vectors and shuffle functions.
C++11 or Later: Requires modern C++ features for random number generation and ADT support.

Usage
The simulation will output the estimated probability of drawing a flush in 5-card stud poker after running a specified number of trials. Modify the number of iterations in the source code to increase accuracy or speed up the computation.

Contribution
Contributions are welcome! Feel free to fork the repository and submit pull requests with improvements or additional features.

License
This project is licensed under the MIT License. See the LICENSE file for more details.
