cmake_minimum_required(VERSION 3.0.0)

### Project 
project(MBIMGUI_TEST VERSION 0.1.0)

### Global variables
set(SRC_DIR ./)
set(MBIMGUI_DIR ../)
set(IMGUI_DIR ../../imgui/imgui/)
set(IMGUIBROWSER_DIR ../../imgui/imgui-filebrowser/)
set(IMPLOT_DIR ../../imgui/implot/)
set(IMEMEDIT_DIR ../../Imgui/imgui_club/imgui_memory_editor)

### Includes
include(CTest)
enable_testing()

#message(STATUS ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION})
#message(WARNING ${D3D12_INCLUDE_DIRS})

### Dependencies
add_subdirectory(${MBIMGUI_DIR} ./dep/MBIMGUI)

### Sources
set(SRC_FILES   ${SRC_DIR}/main.cpp  )

### Output
add_executable(${PROJECT_NAME}
                    ${SRC_FILES}
                    ${HEADER_FILES})

## Options
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

### Preproc
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR} 
                  ${MBIMGUI_DIR}
                  ${IMGUI_DIR}
                  ${IMPLOT_DIR}
                  ${IMGUIBROWSER_DIR}
                  ${IMEMEDIT_DIR}
                  )

                  
### Linker
target_link_libraries(${PROJECT_NAME} 
                        d3d12.lib # DX12
                        dxgi.lib # DX
                        MBIMGUI) 

### CPack
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
