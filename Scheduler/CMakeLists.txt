#this is your final project

file(GLOB_RECURSE PRIV_SRC src/*.cpp)

add_executable(scheduler ${PRIV_SRC})
target_link_libraries(scheduler PUBLIC ${Boost_LIBRARIES} fmt::fmt NESLib)
target_include_directories(scheduler PUBLIC include)

generate_gtest(PROJECT_NAME "Scheduler" TEST_DIR "test" INCLUDE_DIR "include" SRC_DIR "src" MAIN_SRC_NAME "main.cpp" ADDITIONAL_TARGET_LIBS "${Boost_LIBRARIES}" "fmt::fmt" "NESLib")