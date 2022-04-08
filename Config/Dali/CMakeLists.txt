find_package(Qt5Widgets REQUIRED)
file(GLOB_RECURSE header_files ${DALI_INCLUDE_PATH}/Dali/*.hpp)
file(GLOB_RECURSE source_files ${DALI_SOURCE_PATH}/Dali/*.cpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
if(WIN32)
  set(app_type WIN32)
endif(WIN32)
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()
add_library(Library ${header_files} ${source_files})

#if(WIN32)
#  target_link_libraries(Library
#    debug ${Z3_LIBRARY_DEBUG_PATH}
#    optimized ${Z3_LIBRARY_OPTIMIZED_PATH})
#endif(WIN32)

set_source_files_properties(${header_files} PROPERTIES HEADER_FILE_ONLY TRUE)
set_target_properties(Library PROPERTIES STATIC_LIBRARY_FLAGS_RELEASE
  "${CMAKE_LIBRARY_FLAGS}" OUTPUT_NAME Spire)

install(TARGETS Library CONFIGURATIONS Debug
  DESTINATION ${DALI_LIBRARY_INSTALL_DIRECTORY}/Debug)
install(TARGETS Library CONFIGURATIONS Release
  DESTINATION ${DALI_LIBRARY_INSTALL_DIRECTORY}/Release)