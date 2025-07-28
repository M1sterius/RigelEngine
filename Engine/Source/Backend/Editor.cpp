#include "Editor.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "SubsystemGetters.hpp"
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
        if (ImGui::Button("Click me"))
        {
            Debug::Message("Click!");
        }
        ImGui::End();

        ImGui::Render();
    }
}