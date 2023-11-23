file(GLOB SRC_FILES ./src/*.cpp)

add_executable(markov ${SRC_FILES})
target_link_libraries(markov PUBLIC NESLib fmt::fmt )
target_include_directories(markov PUBLIC ./include)