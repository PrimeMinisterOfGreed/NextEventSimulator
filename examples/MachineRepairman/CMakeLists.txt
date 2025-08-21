file(GLOB_RECURSE PRIV_SRC src/*.cpp)

add_executable(machine_repairman ${PRIV_SRC})
target_link_libraries(machine_repairman PUBLIC fmt::fmt NESLib)
target_include_directories(machine_repairman PUBLIC include)
