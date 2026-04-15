cmake_minimum_required(VERSION 3.20)
project(Distributed_Monte_Carlo_Simulations VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- AJOUTEZ CETTE LIGNE ICI ---
find_package(Threads REQUIRED)
# -------------------------------

# Recherche des bibliothèques RabbitMQ
find_library(SIMPLEAMQP_LIB NAMES SimpleAmqpClient REQUIRED)
find_library(RABBITMQ_LIB NAMES rabbitmq REQUIRED)
find_path(SIMPLEAMQP_INCLUDE_DIR NAMES SimpleAmqpClient/SimpleAmqpClient.h REQUIRED)

add_executable(worker 
    src/main.cpp 
    src/EuropeanCall.cpp
)

target_include_directories(worker PRIVATE include/ ${SIMPLEAMQP_INCLUDE_DIR})

# Dépendance JSON (FetchContent)
include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# Lien final
target_link_libraries(worker 
    PRIVATE 
    nlohmann_json::nlohmann_json
    ${SIMPLEAMQP_LIB}
    ${RABBITMQ_LIB}
    Threads::Threads  # Maintenant Threads::Threads sera reconnu
)