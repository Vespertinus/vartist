
#ifndef __SCENE_H__
#define __SCENE_H__ 1

#include <ImGui.h>

namespace VA {

class Scene : public OIS::MouseListener {

        public:
        struct Settings {
                const std::string & sScenePath;
                const std::string & sMeshPath;
                bool  vdebug;
                bool  enable_all;
                SE::Camera::Settings oCamSettings;
        };

        private:
        SE::Camera                    * pCamera;
        SE::TSceneTree::TSceneNode      pCameraNode;
        SE::TSceneTree                * pSceneTree;
        Settings                        oSettings;
        SE::HELPERS::ImGuiWrapper       oImGui;
        bool                            toggle_controller{false};

        void ShowGUI();

        public:
        Scene(const Settings & oNewSettings);
        ~Scene() noexcept;

        void Process();

        bool mouseMoved( const OIS::MouseEvent &ev);
        bool mousePressed( const OIS::MouseEvent &ev, OIS::MouseButtonID id);
        bool mouseReleased( const OIS::MouseEvent &ev, OIS::MouseButtonID id);


};


} //VA

#endif


