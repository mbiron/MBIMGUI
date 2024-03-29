# ##########################################################
# This file is used by modules using MBIMGUI to locate MBIMGUI
# and its dependencies
# ##########################################################
include(GNUInstallDirs)

# Global variables
set(MBIMGUI_DIR ${CMAKE_INSTALL_PREFIX}/../MBIMGUI/${CMAKE_INSTALL_INCLUDEDIR})
set(MBIMGUI_LIB_DIR ${CMAKE_INSTALL_PREFIX}/../MBIMGUI/${CMAKE_INSTALL_LIBDIR})
set(SUB_MOD_DIR ${MBIMGUI_DIR}/imgui)

set(IMGUI_DIR ${SUB_MOD_DIR}/Imgui/)
set(IMPLOT_DIR ${SUB_MOD_DIR}/implot/)
set(IMEMEDIT_DIR ${SUB_MOD_DIR}/imgui_club/imgui_memory_editor)
set(IMSPINNER_DIR ${SUB_MOD_DIR}/imspinner/)
set(IMTOGGLE_DIR ${SUB_MOD_DIR}/imgui_toggle)
set(ICONFONT_DIR ${SUB_MOD_DIR}/IconFontCppHeaders)

set(MBIMGUI_LIB_RELEASE_DIR ${MBIMGUI_LIB_DIR}/Release/)
set(MBIMGUI_LIB_DEBUG_DIR ${MBIMGUI_LIB_DIR}/Debug/)
set(MBIMGUI_MANIFEST_DIR ${MBIMGUI_LIB_DIR}/cmake/MBIMGUI/)

set(MBIMGUI_LIB_DEPENDENCIES
    d3d12.lib # DX12
    dxgi.lib # DX
    Comctl32.lib #For filebrowser
    Propsys.lib
    Shlwapi.lib)

message("${config_suffix}")

# import lib
add_library(MBIMGUI STATIC IMPORTED)

# Set link and includes path
find_library(MBIMGUI_LIBRARY_PATH_RELEASE MBIMGUI HINTS ${MBIMGUI_LIB_RELEASE_DIR})
find_library(MBIMGUI_LIBRARY_PATH_DEBUG MBIMGUI HINTS ${MBIMGUI_LIB_DEBUG_DIR})

set_target_properties(MBIMGUI PROPERTIES
    IMPORTED_LOCATION_RELEASE ${MBIMGUI_LIBRARY_PATH_RELEASE}
    IMPORTED_LOCATION_DEBUG ${MBIMGUI_LIBRARY_PATH_DEBUG}
    IMPORTED_LOCATION_RELWITHDEBINFO ${MBIMGUI_LIBRARY_PATH_DEBUG}
    IMPORTED_LOCATION_MINSIZEREL ${MBIMGUI_LIBRARY_PATH_RELEASE})

set_target_properties(MBIMGUI PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${MBIMGUI_DIR};${IMGUI_DIR};${IMPLOT_DIR};${IMEMEDIT_DIR};${IMSPINNER_DIR};${IMTOGGLE_DIR};${ICONFONT_DIR}")

### Manifest for Comctl32.lib version for file open api
target_sources(MBIMGUI INTERFACE ${MBIMGUI_MANIFEST_DIR}/components.manifest)

target_link_libraries(MBIMGUI INTERFACE ${MBIMGUI_LIB_DEPENDENCIES})

message("${MBIMGUI_LIBRARY_PATH}")