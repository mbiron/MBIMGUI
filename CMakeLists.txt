cmake_minimum_required(VERSION 3.0.0)

### Global variables
set(SRC_DIR Sources/)
set(SRC_DIR_REND Sources/Renderers)
set(TEST_DIR ./)


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

### Project 
project(MBIMGUI VERSION 0.1.0)

### Includes
include(CTest)
enable_testing()

#message(STATUS ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION})
#message(WARNING ${D3D12_INCLUDE_DIRS})

### Sources
aux_source_directory( ${SRC_DIR_REND} RENDERERS_SRC )
set(SRC_FILES ${IMGUI_SRC} main.cpp ${SRC_DIR}MBIMGUI.cpp ${RENDERERS_SRC} )
set(HEADER_FILES ${SRC_DIR}MBIMGUI.h)

### Output
add_executable(${PROJECT_NAME}
                    ${SRC_FILES}
                    ${HEADER_FILES})

### Preproc
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR} ${SRC_DIR_REND} ${TEST_DIR} ${IMGUI_DIR} ${IMGUI_BACKENDS_DIR})

### Linker
target_link_libraries(${PROJECT_NAME} d3d12.lib) 
target_link_libraries(${PROJECT_NAME} dxgi.lib) 

### CPack
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
