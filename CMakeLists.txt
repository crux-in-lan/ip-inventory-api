cmake_minimum_required(VERSION 3.20)
project(ip_inventory_api LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Drogon CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

enable_testing()

add_executable(ip_inventory_api
    src/main.cpp
    src/db/Db.cpp
    src/jobs/ReservationCleanupJob.cpp
    src/controllers/IpInventoryController.cpp
    src/services/IpInventoryService.cpp
)

add_custom_command(TARGET ip_inventory_api POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/config
        $<TARGET_FILE_DIR:ip_inventory_api>/config
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/web
        $<TARGET_FILE_DIR:ip_inventory_api>/web
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/swagger
        $<TARGET_FILE_DIR:ip_inventory_api>/swagger
)

target_include_directories(ip_inventory_api PRIVATE src)

target_link_libraries(ip_inventory_api PRIVATE
    Drogon::Drogon
    libpqxx::pqxx
	spdlog::spdlog
)

add_executable(ip_inventory_tests
    tests/ValidationTests.cpp
)

target_include_directories(ip_inventory_tests PRIVATE src)

target_link_libraries(ip_inventory_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
	spdlog::spdlog
)

include(GoogleTest)
gtest_discover_tests(ip_inventory_tests)

if (MSVC)
    target_compile_options(ip_inventory_api PRIVATE /W4 /permissive-)
    target_compile_options(ip_inventory_tests PRIVATE /W4 /permissive-)
else()
    target_compile_options(ip_inventory_api PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(ip_inventory_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()