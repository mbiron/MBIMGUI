# ##########################################################
# This file is used by MBIMGUI to locate its dependencies #
# ##########################################################

# ## Global variables
set(SUB_MOD_DIR ${MBIMGUI_DIR}/imgui)

set(IMGUI_DIR ${SUB_MOD_DIR}/Imgui/)
set(IMGUIBROWSER_DIR ${SUB_MOD_DIR}/imgui-filebrowser/)
set(IMPLOT_DIR ${SUB_MOD_DIR}/implot/)
set(IMEMEDIT_DIR ${SUB_MOD_DIR}/imgui_club/imgui_memory_editor)
set(IMSPINNER_DIR ${SUB_MOD_DIR}/imspinner/)
set(IMTOGGLE_DIR ${SUB_MOD_DIR}/imgui_toggle)
set(ICONFONT_DIR ${SUB_MOD_DIR}/IconFontCppHeaders)

# ## IMGUI
set(IMGUI_DIR ${SUB_MOD_DIR}/Imgui)
set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends)

set(IMGUI_SRC ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_win32.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_dx12.cpp
)

# ## IMPLOT
set(IMPLOT_DIR ${SUB_MOD_DIR}/implot)

set(IMPLOT_SRC ${IMPLOT_DIR}/implot.cpp
    ${IMPLOT_DIR}/implot_items.cpp
    ${IMPLOT_DIR}/implot_demo.cpp
)

# ### IMFILEBROWSER
set(IMFILEBROWSER_DIR ${SUB_MOD_DIR}/imgui-filebrowser)

# ### Memory Editor
set(IMEMEDIT_DIR ${SUB_MOD_DIR}/imgui_club/imgui_memory_editor)

# ## Imspinner
set(IMSPINNER_DIR ${SUB_MOD_DIR}/imspinner)

# ### Imgui_toggle
set(IMTOGGLE_DIR ${SUB_MOD_DIR}/imgui_toggle)
set(IMTOGGLE_SRC ${IMTOGGLE_DIR}/imgui_toggle.cpp
    ${IMTOGGLE_DIR}/imgui_toggle_palette.cpp
    ${IMTOGGLE_DIR}/imgui_toggle_presets.cpp
    ${IMTOGGLE_DIR}/imgui_toggle_renderer.cpp)

set(IMGUI_DEP_DIRS ${IMGUI_DIR}
    ${IMGUI_BACKENDS_DIR}
    ${IMPLOT_DIR}
    ${IMFILEBROWSER_DIR}
    ${IMEMEDIT_DIR}
    ${IMSPINNER_DIR}
    ${IMTOGGLE_DIR}
    ${ICONFONT_DIR})
set(IMGUI_DEP_SRCS ${IMGUI_SRC} ${IMPLOT_SRC} ${IMTOGGLE_SRC})