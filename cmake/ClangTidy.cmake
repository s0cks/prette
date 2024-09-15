find_program(CLANG_TIDY
  NAMES "clang-tidy")
if(CLANG_TIDY)
  execute_process(COMMAND ${CLANG_TIDY} --version
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    ERROR_QUIET
    OUTPUT_VARIABLE CLANG_TIDY_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(REGEX MATCH "[0-9]+\.[0-9]+\.[0-9]+"
    CLANG_TIDY_VERSION "${CLANG_TIDY_VERSION}")

  list(APPEND CLANG_TIDY_OPTS)
  set(CLANG_TIDY_FILE "${CMAKE_SOURCE_DIR}/.clang-tidy")
  if(EXISTS "${CLANG_TIDY_FILE}")
    message(STATUS "found clang-tidy config: ${CLANG_TIDY_FILE}")
    list(APPEND CLANG_TIDY_OPTS
      "--config-file=${CLANG_TIDY_FILE}")
  else()
    list(APPEND CLANG_TIDY_OPTS
      "-checks=-*,modernize-*")
  endif()

  message(STATUS "found clang-tidy v${CLANG_TIDY_VERSION}: ${CLANG_TIDY}")
  message(STATUS "clang-tidy options: ${CLANG_TIDY_OPTS}")
  set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY} ${CLANG_TIDY_OPTS})
endif()