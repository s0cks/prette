if(APPLE)
  set(UTF8_SEARCH_PATHS
      ~/Library/Frameworks
      /Library/Frameworks
      ${CMAKE_SOURCE_DIR}/lib
      /usr/local
      /usr
      /sw
      /opt/local
      /opt/csw)
else()
  set(UTF8_SEARCH_PATHS
      ${CMAKE_SOURCE_DIR}/lib
      /usr/local
      /usr)
endif()

find_path(UTF8_INCLUDE_DIR
          NAMES utf8/utf8.h
          PATHS ${UTF8_SEARCH_PATHS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(utf8
                                  DEFAULT_MSG 
                                  UTF8_INCLUDE_DIR)
if(UTF8_FOUND)
  set(UTF8_INCLUDE_DIRS ${UTF8_INCLUDE_DIR})
  message(STATUS "Found utf8 (include: ${UTF8_INCLUDE_DIRS}")
endif()