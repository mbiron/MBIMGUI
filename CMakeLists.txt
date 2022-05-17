cmake_minimum_required(VERSION 3.0.0)

### Global variables
set(SRC_DIR Sources/)
set(SRC_DIR_REND Sources/Renderers)

### IMGUI
set(IMGUI_DIR ../Imgui/imgui)
set(IMGUI_BACKENDS_DIR ../Imgui/imgui/backends)

set(IMGUI_SRC 
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_BACKENDS_DIR}/imgui_impl_win32.cpp
  ${IMGUI_BACKENDS_DIR}/imgui_impl_dx12.cpp
)

### IMPLOT
set(IMPLOT_DIR ../Imgui/implot)

set(IMPLOT_SRC 
  ${IMPLOT_DIR}/implot.cpp
  ${IMPLOT_DIR}/implot_items.cpp
  ${IMPLOT_DIR}/implot_demo.cpp
)

### IMFILEBROWSER
set(IMFILEBROWSER_DIR ../Imgui/imgui-filebrowser)

### Project 
project(MBIMGUI VERSION 0.1.0)

### Includes
include(CTest)
enable_testing()

#message(STATUS ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION})

### Sources
aux_source_directory( ${SRC_DIR_REND} RENDERERS_SRC )
set(SRC_FILES ${IMGUI_SRC} ${IMPLOT_SRC} ${SRC_DIR}MBIMGUI.cpp ${RENDERERS_SRC} )  
set(HEADER_FILES MBIMGUI.h)

### Output
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(${PROJECT_NAME} STATIC
            ${SRC_FILES}
            ${HEADER_FILES})

## Options
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

### Preproc
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR} ${SRC_DIR_REND} 
        ${PROJECT_SOURCE_DIR} 
        ${IMGUI_DIR} ${IMGUI_BACKENDS_DIR}
        ${IMPLOT_DIR}
        ${IMFILEBROWSER_DIR})

### Linker
target_link_libraries(${PROJECT_NAME} d3d12.lib) 
target_link_libraries(${PROJECT_NAME} dxgi.lib) 

### CPack
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
