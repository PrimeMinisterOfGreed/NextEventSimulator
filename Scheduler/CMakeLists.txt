#this is your final project
set(SRC_FILES 
    src/CPU.cpp
    src/IOStation.cpp
    src/OperativeSystem.cpp
    src/ReserveStation.cpp
    src/SwapIn.cpp
    src/SwapOut.cpp
    src/SimulationEnv.cpp
    src/Scenario.cpp
)


add_executable(scheduler 
    src/main.cpp 
    ${SRC_FILES}
)

add_executable(scheduler_mva 

    src/mvaSolver.cpp
)

target_link_libraries(scheduler PUBLIC  fmt::fmt NESLib)
target_link_libraries(scheduler_mva PUBLIC fmt::fmt NESLib)
target_include_directories(scheduler PUBLIC include)

file(GLOB_RECURSE TEST_FILES test/*.cpp)
generate_gtest(PROJECT_NAME "Scheduler" SRC_FILES ${SRC_FILES} TEST_SRC_FILES ${TEST_FILES} INCLUDE_DIRS  include test/include  ADDITIONAL_TARGET_LIBS  "fmt::fmt" "NESLib")