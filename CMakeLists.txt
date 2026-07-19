

cmake_minimum_required(VERSION 3.16)

project(StockPredictionSystem VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.2.1
)

FetchContent_MakeAvailable(Crow)

add_executable(stock_prediction
    asmiproject.cpp
)

target_link_libraries(stock_prediction
    PRIVATE Crow::Crow
)

if(MSVC)
    target_compile_options(stock_prediction PRIVATE /W4)
else()
    target_compile_options(stock_prediction PRIVATE -Wall -Wextra -O2)
endif()