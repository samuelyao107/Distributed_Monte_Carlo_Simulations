cmake_minimum_required(VERSION 3.20)
project(Distributed_Monte_Carlo_Simulations VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Threads ---
find_package(Threads REQUIRED)

# --- RabbitMQ ---
find_library(SIMPLEAMQP_LIB NAMES SimpleAmqpClient REQUIRED)
find_library(RABBITMQ_LIB NAMES rabbitmq REQUIRED)
find_path(SIMPLEAMQP_INCLUDE_DIR NAMES SimpleAmqpClient/SimpleAmqpClient.h REQUIRED)

# --- FetchContent ---
include(FetchContent)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)

FetchContent_MakeAvailable(nlohmann_json googletest)

# --- Librairie commune ---
add_library(pricer_lib
    src/EuropeanCall.cpp
)

target_include_directories(pricer_lib PUBLIC include/)

target_link_libraries(pricer_lib
    PUBLIC
    nlohmann_json::nlohmann_json
)

# --- Exécutable principal ---
add_executable(worker src/main.cpp)

target_include_directories(worker PRIVATE ${SIMPLEAMQP_INCLUDE_DIR})

target_link_libraries(worker
    PRIVATE
    pricer_lib
    ${SIMPLEAMQP_LIB}
    ${RABBITMQ_LIB}
    Threads::Threads
)

# --- Tests ---
add_executable(tests tests/test_pricer.cpp)

target_include_directories(tests PRIVATE include/)

target_link_libraries(tests
    PRIVATE
    pricer_lib
    GTest::gtest_main
)

enable_testing()
add_test(NAME EuropeanCallTest COMMAND tests)