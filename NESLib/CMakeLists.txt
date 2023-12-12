file(GLOB_RECURSE PRIV_SRC src/*.cpp)
include(../cmake/TestProjectGenerator.cmake)

add_library(NESLib OBJECT ${PRIV_SRC})
target_include_directories(NESLib PUBLIC include)
target_link_libraries(NESLib PRIVATE  fmt::fmt)
generate_gtest(PROJECT_NAME "Nes" TEST_DIR "test" INCLUDE_DIR "include" SRC_DIR "test" MAIN_SRC_NAME "main.cpp" ADDITIONAL_TARGET_LIBS fmt::fmt "NESLib" )