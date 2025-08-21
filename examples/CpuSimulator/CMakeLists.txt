
project(CpuSimulator CXX C)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_FILES)
add_executable(CpuSimulator ${SRC_FILES}
)

add_executable(CpuSimulator_mva 

    src/MvaSolver.cpp
)

target_link_libraries(CpuSimulator PUBLIC  fmt::fmt NESLib argparse::argparse)
target_link_libraries(CpuSimulator_mva PUBLIC fmt::fmt NESLib)
target_include_directories(CpuSimulator PUBLIC include)
target_compile_definitions(CpuSimulator_mva PUBLIC USE_MAIN)

add_subdirectory(test)