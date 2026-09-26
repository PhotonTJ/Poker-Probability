# The engine is the whole domain model: cards, deck, hand classification and
# the Monte Carlo simulator. It has zero dependency on Python or the web
# stack, so it builds and tests standalone -- the pybind11 module and the
# FastAPI backend are just two different consumers of this same library.
add_library(poker_engine_core STATIC
  src/card.cpp
  src/deck.cpp
  src/hand.cpp
  src/simulator.cpp
)
target_include_directories(poker_engine_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(poker_engine_core PUBLIC cxx_std_17)

if(POKER_BUILD_TESTS)
  add_executable(poker_engine_tests tests/test_main.cpp)
  target_link_libraries(poker_engine_tests PRIVATE poker_engine_core)
  target_include_directories(poker_engine_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests)

  enable_testing()
  add_test(NAME poker_engine_tests COMMAND poker_engine_tests)
endif()

if(POKER_BUILD_PYTHON_BINDINGS)
  add_subdirectory(bindings)
endif()
