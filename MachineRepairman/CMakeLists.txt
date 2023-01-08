project(MachineRepairman)

file(GLOB_RECURSE PRIV_SRC src/*.cpp)

add_executable(machine_repairman ${PRIV_SRC})
target_link_libraries(machine_repairman PUBLIC ${Boost_LIBRARIES} fmt::fmt NESLib)
target_include_directories(machine_repairman PUBLIC include)

generate_gtest(TEST_DIR "test" INCLUDE_DIR "include" SRC_DIR "src" MAIN_SRC_NAME "main.cpp" ADDITIONAL_TARGET_LIBS "${Boost_LIBRARIES}" "fmt::fmt" "NESLib")