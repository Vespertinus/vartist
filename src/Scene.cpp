#include <Global.h>
#include <GlobalTypes.h>
#include <Camera.h>
#include "Scene.h"


namespace VA {

static const uint8_t NODE_HIDE = 0x1;

Scene::Scene(const Settings & oNewSettings) :
        pSceneTree(SE::CreateResource<SE::TSceneTree>(oNewSettings.sScenePath)),
        oSettings(oNewSettings) {

        pCameraNode = pSceneTree->Create("MainCamera");
        auto res = pCameraNode->CreateComponent<SE::Camera>(oSettings.oCamSettings);
        if (res != SE::uSUCCESS) {
                throw("failed to create Camera component");
        }
        pCamera = pCameraNode->GetComponent<SE::Camera>();
        se_assert(pCamera);

        res = pCameraNode->CreateComponent<SE::BasicController>();
        if (res != SE::uSUCCESS) {
                throw("failed to create BasicController component");
        }

        //init cam
        pCamera->SetPos(8, 4, 8);
        pCamera->LookAt(0.1, 0.1, 0.1);

        SE::TEngine::Instance().Get<SE::TRenderer>().SetCamera(pCamera);

        if (oSettings.enable_all) {
                pSceneTree->EnableAll();
        }

        pSceneTree->Print();

        SE::TInputManager::Instance().AddMouseListener (this, "SceneViewer");

}



Scene::~Scene() noexcept {

        SE::TInputManager::Instance().RemoveMouseListener ("SceneViewer");
}



void Scene::Process() {

        SE::CheckOpenGLError();

        if (toggle_controller) {
                pCameraNode->ToggleEnabled();
                toggle_controller = false;
        }

        if (oSettings.vdebug) {

                auto & oDebugRenderer = SE::GetSystem<SE::DebugRenderer>();
                oDebugRenderer.DrawGrid(pSceneTree->GetRoot()->GetTransform());

                pSceneTree->GetRoot()->DepthFirstWalk([](SE::TSceneTree::TSceneNodeExact & oNode) {

                        if (oNode.GetFlags() & NODE_HIDE) {
                                return false;
                        }

                        if (oNode.GetComponentsCnt() == 0) {
                                return true;
                        }

                        oNode.DrawDebug();

                        return true;
                });
        }

        //DEBUG
        //SE::GetSystem<SE::TRenderer>().Print();

        ShowGUI();
}

void Scene::ShowGUI() {

        static const uint8_t NODE_HIDE = 0x1;

        //basic info
        const float indent = 10;
        ImVec2 window_pos = ImVec2(ImGui::GetIO().DisplaySize.x - indent, indent);
        ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowBgAlpha(0.7f);
        ImGui::Begin(
                        "Info",
                        nullptr,
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_AlwaysAutoResize |
                        ImGuiWindowFlags_NoSavedSettings |
                        ImGuiWindowFlags_NoFocusOnAppearing |
                        ImGuiWindowFlags_NoNav);
        ImGui::Text("Frame time: %f, FPS: %f", SE::GetSystem<SE::GraphicsState>().GetLastFrameTime(), SE::TSimpleFPS::Instance().GetFPS());
        ImGui::Separator();
        ImGui::Text("Texture cnt: %zu", SE::TResourceManager::Instance().Size<SE::TTexture>());
        ImGui::Text("Mesh cnt: %zu", SE::TResourceManager::Instance().Size<SE::TMesh>());
        ImGui::Text("Materials cnt: %zu", SE::TResourceManager::Instance().Size<SE::Material>());
        ImGui::Text("Shader component cnt: %zu", SE::TResourceManager::Instance().Size<SE::ShaderComponent>());
        ImGui::Text("Shader program cnt: %zu", SE::TResourceManager::Instance().Size<SE::ShaderProgram>());
        ImGui::Separator();
        ImGui::Text("Scene tree: '%s'", oSettings.sScenePath.c_str());
        ImGui::End();

        //scene tree
        static SE::TSceneTree::TSceneNodeExact * pCurNode;
        ImGui::SetNextWindowBgAlpha(0.9);
        ImGui::Begin("Scene tree", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        //ImGui::SetWindowFontScale(2); //for 4K resolution

        if (pCurNode) {

                ImGui::Text("Node: '%s'", pCurNode->GetFullName().c_str());

                const auto & oTransform = pCurNode->GetTransform();
                glm::vec3 local_pos   = oTransform.GetPos();
                glm::vec3 local_rot   = oTransform.GetRotationDeg();
                glm::vec3 prev_local_rot = local_rot;
                glm::vec3 local_scale = oTransform.GetScale();

                static glm::vec3 cur_rot_around(0);
                static glm::vec3 cur_point(0);

                auto [vWorldTranslation, vWorldRotation, vWorldScale] = pCurNode->GetTransform().GetWorldDecomposedEuler();

                ImGui::Text("Global:");
                ImGui::Text("\t %.3f \t%.3f \t%.3f pos",
                                vWorldTranslation.x,
                                vWorldTranslation.y,
                                vWorldTranslation.z);
                ImGui::Text(" \t%.3f \t%.3f \t%.3f rot",
                                vWorldRotation.x,
                                vWorldRotation.y,
                                vWorldRotation.z);
                ImGui::Text(" \t%.3f \t%.3f \t%.3f scale",
                                vWorldScale.x,
                                vWorldScale.y,
                                vWorldScale.z);
                ImGui::Separator();

                ImGui::Text("Local:");

                ImGui::DragFloat3("pos",   &local_pos[0], 0.1, -100, 100);
                ImGui::DragFloat3("rot",   &local_rot[0], 0.2, -180, 180);
                ImGui::DragFloat3("scale", &local_scale[0], 0.1, 0.01, 100);

                ImGui::Separator();
                ImGui::DragFloat3("point", &cur_point[0], 0.1, -100, 100);
                ImGui::DragFloat3("angle", &cur_rot_around[0], 0.2, -180, 180);

                if (local_pos != oTransform.GetPos()) {

                        pCurNode->SetPos(local_pos);
                }
                if (local_rot != prev_local_rot) {
                        pCurNode->SetRotation(local_rot);
                }
                if (local_scale != oTransform.GetScale()) {
                        pCurNode->SetScale(local_scale);
                }

                if (ImGui::Button("rotate around point")) {

                        pCurNode->RotateAround(cur_point, cur_rot_around);
                        cur_point = glm::vec3(0);
                        cur_rot_around = glm::vec3(0);
                }

                //blendshape values:
                if (auto * pComponent = pCurNode->GetComponent<SE::AnimatedModel>(); pComponent) {
                        ImGui::Separator();
                        static std::vector<float> vWeights;
                        vWeights.clear();
                        vWeights.reserve(pComponent->BlendShapesCnt());

                        for (uint8_t i = 0; i < pComponent->BlendShapesCnt(); ++i) {
                                vWeights.emplace_back(pComponent->GetWeight(i));
                                ImGui::DragFloat(fmt::format("blendshape[{}]", i).c_str(),   &vWeights[i], 0.01, 0, 1);
                        }

                        for (uint8_t i = 0; i < pComponent->BlendShapesCnt(); ++i) {
                                if (vWeights[i] != pComponent->GetWeight(i)) {
                                        pComponent->SetWeight(i, vWeights[i]);
                                }
                        }
                }

        }

        ImGui::Separator();
        if (ImGui::TreeNode("Nodes:")) {

                pSceneTree->GetRoot()->DepthFirstWalkEx([this](SE::TSceneTree::TSceneNodeExact & oNode) {

                        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                                        ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                                        ((&oNode == pCurNode) ? ImGuiTreeNodeFlags_Selected : 0);
                        bool node_open = ImGui::TreeNodeEx(oNode.GetFullName().c_str(), node_flags, oNode.GetName().c_str());

                        if (ImGui::IsItemClicked()) {
                                pCurNode = &oNode;
                        }

                        if (!node_open) {
                                return false;
                        }


                        ImGui::Text("components cnt: %u", oNode.GetComponentsCnt());

                        oNode.ForEachComponent(
                                [&oNode](const auto & pComponent) {

                                        ImGui::Text(pComponent->Str().c_str());

                                }
                        );

                        bool show = oNode.GetFlags() & NODE_HIDE;
                        if (ImGui::Button((show) ? "show" : "hide")) {
                                if (show) {
                                        oNode.ClearFlags(NODE_HIDE);
                                        oNode.EnableRecursive();
                                }
                                else {
                                        oNode.SetFlags(NODE_HIDE);
                                        oNode.DisableRecursive();
                                }
                        }
                        if (ImGui::Button("look at")) {
                                pCamera->LookAt(oNode.GetTransform().GetWorldPos());
                        }

                        return true;
                },
                        [](SE::TSceneTree::TSceneNodeExact & oNode) {

                                ImGui::TreePop();
                });

                ImGui::TreePop();
        }

        ImGui::End();
}

bool Scene::mouseMoved( const OIS::MouseEvent &ev [[maybe_unused]]) {
        return true;
}

bool Scene::mousePressed( const OIS::MouseEvent &ev [[maybe_unused]], OIS::MouseButtonID id [[maybe_unused]]) {

        return true;
}


bool Scene::mouseReleased( const OIS::MouseEvent &ev, OIS::MouseButtonID id) {

        switch (id) {
                case OIS::MB_Right:
                        toggle_controller = true;
                        break;
                default:
                        break;
        }

        return true;
}


} //namespace VA




