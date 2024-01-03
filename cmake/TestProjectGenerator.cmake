
find_package(GTest REQUIRED CONFIG)
include(GoogleTest)
function(generate_gtest)
      set(oneValueArgs   PROJECT_NAME)
      set(multiValueArgs ADDITIONAL_TARGET_LIBS SRC_FILES TEST_SRC_FILES INCLUDE_DIRS)
      set(options OPTIONAL NONE)
      cmake_parse_arguments(G "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
      

  add_executable(${G_PROJECT_NAME}_unit ${G_SRC_FILES} ${G_TEST_SRC_FILES} )
  target_link_libraries(${G_PROJECT_NAME}_unit PUBLIC GTest::gtest GTest::gtest_main ${G_ADDITIONAL_TARGET_LIBS})
  target_include_directories(${G_PROJECT_NAME}_unit PUBLIC ${G_INCLUDE_DIRS})
  gtest_discover_tests(${G_PROJECT_NAME}_unit)
endfunction()


