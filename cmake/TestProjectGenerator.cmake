
include(GoogleTest)
function(generate_gtest)
      set(oneValueArgs SRC_DIR TEST_DIR INCLUDE_DIR MAIN_SRC_NAME PROJECT_NAME)
      set(multiValueArgs ADDITIONAL_TARGET_LIBS)
      set(options OPTIONAL NONE)
      cmake_parse_arguments(G "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
      
      file(GLOB_RECURSE _TEST_SRC ${G_TEST_DIR}/*.cpp)
  file(GLOB_RECURSE _SRC_MAIN ${G_SRC_DIR}/*.cpp)
  message("Imported source files: " + ${_SRC_MAIN})
  list(FILTER _SRC_MAIN EXCLUDE REGEX ${G_MAIN_SRC_NAME})
  add_executable(${G_PROJECT_NAME}_test ${_TEST_SRC} ${_SRC_MAIN})
  target_link_libraries(${G_PROJECT_NAME}_test PRIVATE GTest::gtest GTest::gtest_main ${G_ADDITIONAL_TARGET_LIBS})
  target_include_directories(${G_PROJECT_NAME}_test PRIVATE ${G_INCLUDE_DIR})
  gtest_discover_tests(${G_PROJECT_NAME}_test)
endfunction()


