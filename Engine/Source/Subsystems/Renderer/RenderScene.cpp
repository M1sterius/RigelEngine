#include "Subsystems/Renderer/RenderScene.hpp"
#include "ECS/Scene.hpp"
#include "Components/Camera.hpp"
#include "Handles/SceneHandle.hpp"
#include "Components/ModelRenderer.hpp"

namespace Rigel
{
    RenderScene RenderScene::Extract(const SceneHandle& scene)
    {
        auto renderScene = RenderScene();

        if (scene.IsNull())
            return renderScene;

        // Camera
        if (const auto cameras = scene->FindComponentsOfType<Rigel::Camera>(); cameras.empty())
            return renderScene;
        else
        {
            auto camera = cameras[0];
            renderScene.Camera = {
                .Position = camera->GetGameObject()->GetTransform()->GetPosition(),
                .ProjView = camera->GetProjection() * camera->GetView()
            };
        }

        // Model renderer
        for (const auto& mr : scene->FindComponentsOfType<ModelRenderer>())
        {
            if (const auto asset = mr->GetModelAsset(); !asset.IsNull() && asset->IsOK())
            {
                renderScene.Models.emplace_back(asset, mr->GetGameObject()->GetTransform()->GetWorldMatrix());
            }
        }

        return renderScene;
    }
}
