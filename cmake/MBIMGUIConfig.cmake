###########################################################
# This file is used by modules using MBIMGUI to locate MBIMGUI 
# and its dependencies
###########################################################
include(GNUInstallDirs)

#Global variables
set(MBIMGUI_DIR ${CMAKE_INSTALL_PREFIX}/../MBIMGUI/${CMAKE_INSTALL_INCLUDEDIR})
set(SUB_MOD_DIR ${MBIMGUI_DIR}/imgui)

set(IMGUI_DIR ${SUB_MOD_DIR}/Imgui/)
set(IMGUIBROWSER_DIR ${SUB_MOD_DIR}/imgui-filebrowser/)
set(IMPLOT_DIR ${SUB_MOD_DIR}/implot/)
set(IMEMEDIT_DIR ${SUB_MOD_DIR}/imgui_club/imgui_memory_editor)
set(IMSPINNER_DIR ${SUB_MOD_DIR}/imspinner/)
set(IMTOGGLE_DIR ${SUB_MOD_DIR}/imgui_toggle)

#import lib
add_library(MBIMGUI STATIC IMPORTED)

#Set link and includes path
find_library(MBIMGUI_LIBRARY_PATH MBIMGUI HINTS "${CMAKE_CURRENT_LIST_DIR}/../../")
set_target_properties(MBIMGUI PROPERTIES IMPORTED_LOCATION "${MBIMGUI_LIBRARY_PATH}")
set_target_properties(MBIMGUI PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${MBIMGUI_DIR};${IMGUI_DIR};${IMPLOT_DIR};${IMGUIBROWSER_DIR};${IMEMEDIT_DIR};${IMSPINNER_DIR};${IMTOGGLE_DIR}")
