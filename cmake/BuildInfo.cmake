find_package(Git REQUIRED)

execute_process(
  COMMAND ${GIT_EXECUTABLE} describe --always --abbrev=8
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET)
execute_process(
  COMMAND ${GIT_EXECUTABLE} status --short
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_STATUS
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET)
if(("${GIT_COMMIT}" STREQUAL "") OR (NOT "${GIT_STATUS}" STREQUAL ""))
	if(NOT "${GIT_STATUS}" STREQUAL "")
	set(GIT_COMMIT "${GIT_COMMIT}-uncommited")
	else()
		set(GIT_COMMIT "N/A")
	endif()
endif()

execute_process(
  COMMAND ${GIT_EXECUTABLE} describe --exact-match --tags
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_TAG
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET)
if("${GIT_TAG}" STREQUAL "")
	set(GIT_TAG "N/A")
endif()

execute_process(
  COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET)
if("${GIT_BRANCH}" STREQUAL "")
	set(GIT_BRANCH "N/A")
endif()

execute_process(
  COMMAND ${GIT_EXECUTABLE} log -n 1 --pretty=%cd --pretty=%cI
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_DATE
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET)
if("${GIT_DATE}" STREQUAL "")
	set(GIT_DATE "N/A")
endif()