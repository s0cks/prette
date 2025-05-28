if(APPLE)
  set(LIBWAVE_SEARCH_PATHS
      ~/Library/Frameworks
      /Library/Frameworks
      ${CMAKE_SOURCE_DIR}/lib
      /usr/local
      /usr
      /sw
      /opt/local
      /opt/csw)
else()
  set(LIBWAVE_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/lib /usr/local /usr)
endif()

find_path(
  LIBWAVE_INCLUDE_DIR
  NAMES wave.h
  PATHS ${LIBWAVE_SEARCH_PATHS})

find_library(LIBWAVE_LIB
  NAMES wave
  PATHS ${LIBWAVE_SEARCH_PATHS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libwave DEFAULT_MSG LIBWAVE_INCLUDE_DIR)
if(LIBWAVE_FOUND)
  set(LIBWAVE_INCLUDE_DIRS ${LIBWAVE_INCLUDE_DIR})
  set(LIBWAVE_LIBRARIES ${LIBWAVE_LIB})
  message(STATUS "Found libwave (include: ${LIBWAVE_INCLUDE_DIRS}; libs: ${LIBWAVE_LIBRARIES})")

  add_library(libwave INTERFACE)
  target_include_directories(libwave INTERFACE ${LIBWAVE_INCLUDE_DIRS})
  target_link_libraries(libwave INTERFACE ${LIBWAVE_LIBRARIES})

  add_library(wave::wave ALIAS libwave)
endif()

