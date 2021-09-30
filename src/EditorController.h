#ifndef __EDITOR_CONTROLLER_H__
#define __EDITOR_CONTROLLER_H__ 1

#include <InputManager.h>

namespace VA {

class EditorController : public OIS::KeyListener, public OIS::MouseListener {

        SE::TSceneTree::TSceneNodeExact       * pNode;
        std::string                             sName;
        glm::vec3                               translation_speed{0, 0, 0};
        glm::vec3                               rotation_speed   {0, 0, 0};
        int32_t	                                cursor_x{0},
                                                cursor_y{0};

        public:

        EditorController(SE::TSceneTree::TSceneNodeExact * pNewNode);
        ~EditorController() noexcept;

        bool keyPressed( const OIS::KeyEvent &ev);
        bool keyReleased( const OIS::KeyEvent &ev);

        bool mouseMoved( const OIS::MouseEvent &ev);
        bool mousePressed( const OIS::MouseEvent &ev, OIS::MouseButtonID id);
        bool mouseReleased( const OIS::MouseEvent &ev, OIS::MouseButtonID id);

        void                    Update(const SE::Event & oEvent);
        void                    Enable();
        void                    Disable();
        void                    Print(const size_t indent);
        std::string             Str() const;
        void                    DrawDebug() const;
};

} //namespace VA


#endif

