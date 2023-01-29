project(NESLib)

file(GLOB_RECURSE PRIV_SRC src/*.cpp)
include(../cmake/TestProjectGenerator.cmake)

add_library(NESLib SHARED ${PRIV_SRC})
target_include_directories(NESLib PUBLIC include)
target_link_libraries(NESLib PRIVATE "${Boost_LIBRARIES}" fmt::fmt)
generate_gtest(TEST_DIR "test" INCLUDE_DIR "include" SRC_DIR "src" MAIN_SRC_NAME "main.cpp" ADDITIONAL_TARGET_LIBS "${Boost_LIBRARIES}" fmt::fmt "NESLib" )