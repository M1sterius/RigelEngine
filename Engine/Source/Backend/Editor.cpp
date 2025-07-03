#include "Editor.hpp"
#include "imgui/imgui.h"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "ImGui/VK_ImGUI_Renderer.hpp"

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
        Engine::Get().GetRenderer().GetImGuiImpl().BeginNewFrame();

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