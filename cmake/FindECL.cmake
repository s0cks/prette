if(APPLE)
  set(ECL_SEARCH_PATHS
      ~/Library/Frameworks
      /Library/Frameworks
      ${CMAKE_SOURCE_DIR}/lib
      /usr/local
      /usr
      /sw
      /opt/local
      /opt/csw)
else()
  set(ECL_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/lib /usr/local /usr)
endif()

find_path(
  ECL_INCLUDE_DIR
  NAMES ecl/ecl.h
  PATHS ${ECL_SEARCH_PATHS})

find_library(ECL_LIBECL
  NAMES ecl
  PATHS ${ECL_SEARCH_PATHS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ecl DEFAULT_MSG ECL_INCLUDE_DIR)
if(ECL_FOUND)
  set(ECL_INCLUDE_DIRS ${ECL_INCLUDE_DIR})
  set(ECL_LIBRARIES ${ECL_LIBECL})
  message(STATUS "Found ECL (include: ${ECL_INCLUDE_DIRS}; libs: ${ECL_LIBRARIES})")

  add_library(ecl INTERFACE)
  target_include_directories(ecl INTERFACE ${ECL_INCLUDE_DIRS})
  target_link_libraries(ecl INTERFACE ${ECL_LIBRARIES})

  add_library(ecl::ecl ALIAS ecl)
endif()

