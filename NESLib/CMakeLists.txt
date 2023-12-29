file(GLOB PRIV_SRC src/*.cpp)
include(../cmake/TestProjectGenerator.cmake)

add_library(NESLib OBJECT ${PRIV_SRC})
target_include_directories(NESLib PUBLIC include)
target_link_libraries(NESLib PRIVATE  fmt::fmt)

file(GLOB TEST_SRC test/*.cpp)
generate_gtest(PROJECT_NAME "NesLib" SRC_FILES ${PRIV_SRC} TEST_SRC_FILES ${TEST_SRC} INCLUDE_DIRS include test/include ADDITIONAL_TARGET_LIBS fmt::fmt)
