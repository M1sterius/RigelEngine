#include "Editor.hpp"
#include "Engine.hpp"
#include "Subsystems/Time.hpp"
#include "Subsystems/Renderer/Renderer.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "ImGui/VK_ImGUI_Renderer.hpp"

#include "imgui/imgui.h"

namespace Rigel::Backend::Editor
{
    Editor::Editor()
    {

    }

    Editor::~Editor()
    {

    }

    void Editor::Draw()
    {
        GetRenderer()->GetImGuiImpl().BeginNewFrame();

        ImGui::Begin("Hello, Vulkan!");
        ImGui::Text("This is a simple text");
        ImGui::Text("FPS: %.2f", 1.0 / Time::GetDeltaTime());
        
        if (ImGui::Button("Click me"))
        {
            Debug::Message("Click!");
        }
        ImGui::End();

        ImGui::Render();
    }
}