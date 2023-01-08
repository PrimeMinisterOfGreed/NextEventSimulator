function(my_function)
  set(options IS_ACTIVATED)
  set(oneValueArgs NAME)
  set(multiValueArgs ARRAY_OF_PARAM)
  cmake_parse_arguments(MY_FUNCTION_REQUIRED "${options}"
                        "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  # If the name is not valid
  if(MY_FUNCTION_NAME STREQUAL "Bad name")
    message(FATAL_ERROR "Error : The name passed as an argument is not valid.")
  endif()

  # If zero param are passed
  if(NOT DEFINED MY_FUNCTION_ARRAY_OF_PARAM)
    message(FATAL_ERROR "Missing ARRAY_OF_PARAM, please add one")
  endif()

  list(LENGTH MY_FUNCTION_ARRAY_OF_PARAM NUMBER_OF_PARAM_IN_ARRAY)

  # If more than three param are passed
  if(${NUMBER_OF_PARAM_IN_ARRAY} GREATER_EQUAL 3)
    message(
      FATAL_ERROR
        "Too much param (${NUMBER_OF_PARAM_IN_ARRAY}) given. Max : 3"
    )
  endif()


  
endfunction()

function(generate_gtest)
      set(oneValueArgs SRC_DIR TEST_DIR INCLUDE_DIR MAIN_SRC_NAME)
      set(multiValueArgs ADDITIONAL_TARGET_LIBS)
      set(options OPTIONAL NONE)
      cmake_parse_arguments(G "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
      
      file(GLOB_RECURSE _TEST_SRC ${G_TEST_DIR}/*.cpp)
  file(GLOB_RECURSE _SRC_MAIN ${G_SRC_DIR}/*.cpp)
  message("Imported source files: " + ${_SRC_MAIN})
  list(FILTER _SRC_MAIN EXCLUDE REGEX ${G_MAIN_SRC_NAME})
  add_executable(${PROJECT_NAME}_test ${_TEST_SRC} ${_SRC_MAIN})
  target_link_libraries(${PROJECT_NAME}_test PRIVATE GTest::gtest GTest::gtest_main ${G_ADDITIONAL_TARGET_LIBS})
  target_include_directories(${PROJECT_NAME}_test PRIVATE ${G_INCLUDE_DIR})
endfunction()


