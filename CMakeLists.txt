project(Simulator)

cmake_minimum_required(VERSION 3.04)
find_package(Boost COMPONENTS ALL REQUIRED)
find_package(GTest REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(./include)
add_executable(simulator_v1 "src/NESssq.cpp" "src/NESssq_List_Manager.cpp" "src/DataProvider.cpp")
add_executable(single_server  "src/ssq1.cpp" "src/DataProvider.cpp" "src/main.cpp")
add_executable(simulator_v2   "src/DataProvider.cpp" "src/ssq1.cpp" "src/NESssqv2.cpp"  "src/main.cpp"  "src/NES/LinkedList.cpp")

target_link_libraries(single_server  ${Boost_LIBRARIES})
target_link_libraries(simulator_v2  ${Boost_LIBRARIES})
target_link_libraries(simulator_v1  ${Boost_LIBRARIES})

add_executable(tests  "src/DataProvider.cpp" "src/NESssqv2.cpp" "src/ssq1.cpp" "src/NES/LinkedList.cpp" "test/testMain.cpp" "test/testLinkedList.cpp")
target_link_libraries(tests PUBLIC  GTest::gtest GTest::gtest_main ${Boost_LIBRARIES}  )
target_include_directories(tests PRIVATE ./include)

configure_file(./doc/ssq2.dat  ${CMAKE_BINARY_DIR}/ssq2.dat)
configure_file(./doc/ssq2_short.dat  ${CMAKE_BINARY_DIR}/ssq2_short.dat)
configure_file(./doc/ssq1.dat  ${CMAKE_BINARY_DIR}/ssq1.dat)