include(../cmake/TestProjectGenerator.cmake)
file(GLOB_RECURSE PRIV_SRC src/*.cpp)


add_executable(simulator_v2  ${PRIV_SRC})
target_include_directories(simulator_v2 PUBLIC include)
target_link_libraries(simulator_v2  ${Boost_LIBRARIES} fmt::fmt NESLib)
generate_gtest(PROJECT_NAME ssqsimulator TEST_DIR "test" INCLUDE_DIR "include" SRC_DIR "src" MAIN_SRC_NAME "main.cpp" ADDITIONAL_TARGET_LIBS "${Boost_LIBRARIES}" "fmt::fmt" "NESLib")


configure_file(./doc/ssq2.dat  ${CMAKE_BINARY_DIR}/ssq2.dat)
configure_file(./doc/ssq2_short.dat  ${CMAKE_BINARY_DIR}/ssq2_short.dat)
configure_file(./doc/ssq1.dat  ${CMAKE_BINARY_DIR}/ssq1.dat)
