cmake_minimum_required(VERSION 3.0.0)

### Project 
project(MBIMGUI_TEST VERSION 0.1.0)

# ## Dependencies
find_package(MBIMGUI CONFIG REQUIRED)

# ## Global variables
set(SRC_DIR Sources/)

### Sources
set(SRC_FILES   ${SRC_DIR}/main.cpp  )

### Output
add_executable(${PROJECT_NAME}
                    WIN32
                    ${SRC_FILES}
                    ${HEADER_FILES})

## Options
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
## Make it a WIN32 app and not a console application (which is default) 
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

### Preproc
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR})
                  
### Linker
target_link_libraries(${PROJECT_NAME} 
                        d3d12.lib # DX12
                        dxgi.lib # DX
                        MBIMGUI) 