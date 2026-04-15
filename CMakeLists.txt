cmake_minimum_required(VERSION 3.20)
project(Distributed_Monte_Carlo_Simulations VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
add_executable(worker src/main.cpp src/EuropeanCall.cpp)
target_include_directories(worker PRIVATE include/)
include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)
target_link_libraries(worker PRIVATE nlohmann_json::nlohmann_json)