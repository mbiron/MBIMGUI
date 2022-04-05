#include "Imgui.h"
#include "MBIMGUI.h"
#include "Win32Renderer.h"

MBIMGUI::MBIMGUI(std::string name, MBIMGUI_Callback cb, void *arg, int width, int height) : m_name(name), m_cb(cb), m_cbArg(arg), m_width(width), m_height(height)
{
    m_pRenderer = new Win32Renderer(name,width, height);
	// Default config flags
	m_windowFlags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
}

MBIMGUI::~MBIMGUI()
{
    delete m_pRenderer;
}

bool MBIMGUI::Init()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
	
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
	
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();
	
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
      
      
  #pragma message("TODO !")
   // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    return m_pRenderer->Init();
}

void MBIMGUI::SetWindowFlags(ImGuiWindowFlags flags)
{
	m_windowFlags = flags;
}

void MBIMGUI::Display()
{
    bool show_demo_window = true;
    bool show_another_window = false;
	bool bOpened = true;
    // Main loop
    bool done = false;
    while (!done && bOpened)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        m_pRenderer->NewFrame();
        ImGui::NewFrame();

// Ensures ImGui fits the window
#if 1
        RECT rect;
        if (GetWindowRect(((Win32Renderer *)m_pRenderer)->getWindowHandle(), &rect))
        {
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            // For i don't now why 
            ImGui::SetNextWindowSize(ImVec2(width, height)); 
        }
        ImGui::SetNextWindowPos(ImVec2(0, 0));
#endif
			
		ImGui::Begin(m_name.c_str(),&bOpened, m_windowFlags); 

        // CALL USER FONCTION
        m_cb(m_cbArg);
		
		ImGui::End();
		
        // Rendering
        ImGui::Render();
        m_pRenderer->Render();
    }

    // Cleanup
    m_pRenderer->Shutdown();
    ImGui::DestroyContext();

    m_pRenderer->Destroy();

    return;
}
