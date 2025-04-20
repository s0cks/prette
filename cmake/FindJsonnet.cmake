if(APPLE)
  set(JSONNET_SEARCH_PATHS
      ~/Library/Frameworks
      /Library/Frameworks
      ${CMAKE_SOURCE_DIR}/lib
      /usr/local
      /usr
      /sw
      /opt/local
      /opt/csw)
else()
  set(JSONNET_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/lib /usr/local /usr)
endif()

find_path(
  JSONNET_INCLUDE_DIR
  NAMES libjsonnet++.h
  PATHS ${JSONNET_SEARCH_PATHS})
find_library(JSONNET_LIB
  NAME libjsonnet++.dylib
  PATHS ${JSONNET_SEARCH_PATHS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(jsonnet DEFAULT_MSG JSONNET_INCLUDE_DIR)
if(JSONNET_FOUND)
  set(JSONNET_INCLUDE_DIRS ${JSONNET_INCLUDE_DIR})
  set(JSONNET_LIBRARIES ${JSONNET_LIB})
  message(STATUS "Found jsonnet (include: ${JSONNET_INCLUDE_DIRS}; lib: ${JSONNET_LIB})")
endif()

add_library(jsonnet INTERFACE)
target_include_directories(jsonnet INTERFACE ${JSONNET_INCLUDE_DIRS})
target_link_libraries(jsonnet INTERFACE ${JSONNET_LIBRARIES})

add_library(jsonnet::jsonnet++ ALIAS jsonnet)