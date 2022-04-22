find_package(Qt5Widgets REQUIRED)
file(GLOB_RECURSE header_files ${DALI_INCLUDE_PATH}/LayoutViewer/*.hpp)
file(GLOB_RECURSE source_files ${DALI_SOURCE_PATH}/LayoutViewer/*.cpp)
# source_group("Header Files" FILES ${header_files})
# source_group("Source Files" FILES ${source_files})
include_directories(SYSTEM ${JSON_SCHEMA_VALIDATOR_INCLUDE_PATH})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party/valijson)

if(WIN32)
  set(app_type WIN32)
endif(WIN32)
add_executable(LayoutViewer ${app_type} ${header_files} ${source_files})

# set_source_files_properties(${header_files} PROPERTIES HEADER_FILE_ONLY TRUE)
target_link_libraries(LayoutViewer Dali Resources
  debug ${Z3_LIBRARY_DEBUG_PATH}
  optimized ${Z3_LIBRARY_OPTIMIZED_PATH}
  debug ${QT_GIF_LIBRARY_DEBUG_PATH}
  optimized ${QT_GIF_LIBRARY_OPTIMIZED_PATH}
  debug ${QT_SVG_ICON_PLUGIN_LIBRARY_DEBUG_PATH}
  optimized ${QT_SVG_ICON_PLUGIN_LIBRARY_OPTIMIZED_PATH}
  debug ${QT_SVG_LIBRARY_DEBUG_PATH}
  optimized ${QT_SVG_LIBRARY_OPTIMIZED_PATH}
  debug ${QT_WIDGETS_LIBRARY_DEBUG_PATH}
  optimized ${QT_WIDGETS_LIBRARY_OPTIMIZED_PATH}
  debug ${QT_FONT_LIBRARY_DEBUG_PATH}
  optimized ${QT_FONT_LIBRARY_OPTIMIZED_PATH}
  debug ${QT_GUI_LIBRARY_DEBUG_PATH}
  optimized ${QT_GUI_LIBRARY_OPTIMIZED_PATH}
  debug ${QT_PNG_LIBRARY_DEBUG_PATH}
  optimized ${QT_PNG_LIBRARY_OPTIMIZED_PATH}
  debug ${QT_EVENT_LIBRARY_DEBUG_PATH}
  optimized ${QT_EVENT_LIBRARY_OPTIMIZED_PATH}
  debug ${QT_ACCESS_LIBRARY_DEBUG_PATH}
  optimized ${QT_ACCESS_LIBRARY_OPTIMIZED_PATH}
  debug ${QT_HBNG_LIBRARY_DEBUG_PATH}
  optimized ${QT_HBNG_LIBRARY_OPTIMIZED_PATH}
  debug ${QT_CORE_LIBRARY_DEBUG_PATH}
  optimized ${QT_CORE_LIBRARY_OPTIMIZED_PATH}
  debug ${QT_PCRE_LIBRARY_DEBUG_PATH}
  optimized ${QT_PCRE_LIBRARY_OPTIMIZED_PATH})
if(WIN32)
  target_link_libraries(LayoutViewer
    debug ${QT_THEME_LIBRARY_DEBUG_PATH}
    optimized ${QT_THEME_LIBRARY_OPTIMIZED_PATH}
    debug ${QT_FREETYPE_LIBRARY_DEBUG_PATH}
    optimized ${QT_FREETYPE_LIBRARY_OPTIMIZED_PATH}
    debug ${QT_MAIN_LIBRARY_DEBUG_PATH}
    optimized ${QT_MAIN_LIBRARY_OPTIMIZED_PATH}
    debug ${QT_WINDOWS_ACCESSIBILITY_LIBRARY_DEBUG_PATH}
    optimized ${QT_WINDOWS_ACCESSIBILITY_LIBRARY_OPTIMIZED_PATH}
    debug ${QT_WINDOWS_LIBRARY_DEBUG_PATH}
    optimized ${QT_WINDOWS_LIBRARY_OPTIMIZED_PATH}
    debug ${QT_WINDOWS_VISTA_STYLE_DEBUG_PATH}
    optimized ${QT_WINDOWS_VISTA_STYLE_OPTIMIZED_PATH}
    dwmapi.lib imm32.lib netapi32.lib opengl32.lib Userenv.lib
    version.lib winmm.lib ws2_32.lib Wtsapi32.lib uxtheme.lib)
endif(WIN32)

install(TARGETS LayoutViewer DESTINATION ${DALI_TESTS_INSTALL_DIRECTORY})
