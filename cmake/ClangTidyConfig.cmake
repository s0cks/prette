find_program(CLANG_TIDY_BIN 
  NAMES "clang-tidy"
  REQUIRED)
if(CLANG_TIDY_BIN)
  execute_process(COMMAND ${CLANG_TIDY_BIN} --version
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    ERROR_QUIET
    OUTPUT_VARIABLE CLANG_TIDY_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(REGEX MATCH "[0-9]+\.[0-9]+\.[0-9]+"
    CLANG_TIDY_VERSION "${CLANG_TIDY_VERSION}")

  set(CLANG_TIDY_OPTS
    "-checks=-*,modernize-*")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(clang_tidy DEFAULT_MSG CLANG_TIDY_BIN)

mark_as_advanced(CLANG_TIDY_BIN)

if(CLANG_TIDY_FOUND)
  message(STATUS "found clang-tidy v${CLANG_TIDY_VERSION}: ${CLANG_TIDY_BIN}")
  message(STATUS "clang-tidy options: ${CLANG_TIDY_OPTS}")
  set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_BIN} ${CLANG_TIDY_OPTS})
endif()