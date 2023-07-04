cmake_minimum_required(VERSION 3.9)

# ## Project
project(MBIMGUI VERSION 0.1.0)

# Set parent dir
set(MBIMGUI_DIR ../MBIMGUI/)

# Include Imgui and co
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Defines IMGUI_DEP_DIRS and IMGUI_DEP_SRCS
find_package(Imgui REQUIRED)

# ## Global variables
set(INC_DIR Includes/)
set(SRC_DIR Sources/)
set(SRC_DIR_REND Sources/Renderers)
set(SRC_DIR_WIDGET Sources/Widgets/)

# ## Sources
aux_source_directory(${SRC_DIR_REND} RENDERERS_SRC)
set(SRC_FILES ${IMGUI_DEP_SRCS}
    ${SRC_DIR}MBIMNG.cpp
    ${SRC_DIR}MBIMGUI.cpp
    ${SRC_DIR}MBIWindow.cpp
    ${SRC_DIR}MBILogger.cpp
    ${SRC_DIR}MBIPlotChart.cpp
    ${SRC_DIR}MBIRealtimePlotChart.cpp
    ${SRC_DIR_WIDGET}imgui_combowithfilter.cpp
    ${SRC_DIR_WIDGET}imspinner.cpp
    ${RENDERERS_SRC})

set(HEADER_FILES ${INC_DIR}MBIMGUI.h)

# ## Output
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(${PROJECT_NAME} STATIC
    ${SRC_FILES}
    ${HEADER_FILES})

# # Options
# Enable C++ 17 (IMFILEBROWSER compatibility)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# ## Preproc
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR} ${SRC_DIR_REND}
    ${INC_DIR}
    ${IMGUI_DEP_DIRS})

# ## Linker
target_link_libraries(${PROJECT_NAME} PRIVATE d3d12.lib)
target_link_libraries(${PROJECT_NAME} PRIVATE dxgi.lib)

# Introduce variables:
# * CMAKE_INSTALL_LIBDIR
# * CMAKE_INSTALL_BINDIR
# * CMAKE_INSTALL_INCLUDEDIR
include(GNUInstallDirs)

### Change default install dir (C:/programmesFiles/)
#set(CMAKE_INSTALL_PREFIX "C:/MBIMGUI")

# Layout. This works for all platforms:
# * <prefix>/lib*/cmake/<PROJECT-NAME>
# * <prefix>/lib*/
# * <prefix>/include/
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

# Configuration
set(project_config "cmake/MBIMGUIConfig.cmake")
set(config_suffix "$<CONFIG>")

# Export lib
install(
    TARGETS ${PROJECT_NAME}
    EXPORT "${TARGETS_EXPORT_NAME}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}/${config_suffix}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# Export headers
install(
    DIRECTORY ${INC_DIR}
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN "*.h"
)

install(
    DIRECTORY ${SUB_MOD_DIR}
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "docs" EXCLUDE
    PATTERN "misc" EXCLUDE
    PATTERN "examples" EXCLUDE
    PATTERN "backends" EXCLUDE
    PATTERN "*github" EXCLUDE
)

# Config
# * <prefix>/lib/cmake/MBIMGUI/MBIMGUIConfig.cmake
install(
    FILES "${project_config}"
    DESTINATION "${config_install_dir}"
)

message("${CMAKE_CONFIGURATION_TYPES}")

# PDB files
set(pdb_output_dir "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
set(pdb_dst ${CMAKE_INSTALL_LIBDIR}/${config_suffix})

install(
    DIRECTORY "${pdb_output_dir}/${config_suffix}/"
    DESTINATION ${pdb_dst}
    FILES_MATCHING PATTERN "*.pdb"
)