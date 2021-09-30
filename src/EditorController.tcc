
namespace VA {

EditorController::EditorController(SE::TSceneTree::TSceneNodeExact * pNewNode) : pNode(pNewNode), sName(pNode->GetFullName() + "|EditorController") {

}

EditorController::~EditorController() noexcept {

        Disable();
}

bool EditorController::keyPressed( const OIS::KeyEvent &ev) {

        switch (ev.key) {

                case OIS::KC_ESCAPE:
                        log_i("stop program");
                        exit(0);
                        //TODO write correct exit

                default:
                        break;

        }

        return true;
}



bool EditorController::keyReleased( const OIS::KeyEvent &ev) {

        return true;
}

bool EditorController::mouseMoved( const OIS::MouseEvent &ev) {

        if (ev.state.buttonDown(OIS::MB_Left)) { //MOVE XY

                translation_speed.y += (cursor_y - ev.state.Y.abs) * 0.25;
                translation_speed.x += - (cursor_x - ev.state.X.abs) * 0.25;
        }
        else if (ev.state.buttonDown(OIS::MB_Middle)) { //ROTATE XY

                rotation_speed.x += (cursor_y - ev.state.Y.abs) * 0.05;
                rotation_speed.y += (cursor_x - ev.state.X.abs) * 0.05;

        }
        else if (ev.state.buttonDown(OIS::MB_Right)) { //MOVE Z

                translation_speed.z += - (cursor_y - ev.state.Y.abs) * 0.25;

        }

        cursor_x = ev.state.X.abs;
        cursor_y = ev.state.Y.abs;

        //FIXME after switching from OIS to SDL2
        OIS::MouseState &MutableMouseState = const_cast<OIS::MouseState &>(SE::TInputManager::Instance().GetMouse()->getMouseState());
        const auto & screen_size = SE::GetSystem<SE::GraphicsState>().GetScreenSize();

        if (ev.state.X.abs >= screen_size.x - 1.0) {

                MutableMouseState.X.abs = 1;
                cursor_x                = 1;
        }

        if (ev.state.Y.abs >= screen_size.y - 1.0) {
                MutableMouseState.Y.abs = 1;
                cursor_y                = 1;
        }
        if (ev.state.X.abs == 0.0) {
                MutableMouseState.X.abs = screen_size.x;
                cursor_x                = screen_size.x;
        }

        if (ev.state.Y.abs == 0.0) {
                MutableMouseState.Y.abs = screen_size.y;
                cursor_y                = screen_size.y;
        }

        return true;
}

bool EditorController::mousePressed( const OIS::MouseEvent &ev, OIS::MouseButtonID id) {

        return true;
}

bool EditorController::mouseReleased( const OIS::MouseEvent &ev, OIS::MouseButtonID id) {

        return true;
}

void EditorController::Update(const SE::Event & oEvent) {

        //So we can fix the first-person camera with the mantra "Pitch Locally, Yaw Globally"
        //https://gamedev.stackexchange.com/questions/136174/im-rotating-an-object-on-two-axes-so-why-does-it-keep-twisting-around-the-thir

        const auto & oEventData = oEvent.Get<SE::EInputUpdate>();

        pNode->Rotate     (glm::vec3(0.0f, rotation_speed.y, 0.0f)); //yaw
        pNode->RotateLocal(glm::vec3(rotation_speed.x, 0.0f, 0.0f)); //pitch

        pNode->TranslateLocal(translation_speed * oEventData.last_frame_time);

        rotation_speed = translation_speed = {0.0f, 0.0f, 0.0f};
}

void EditorController::Enable() {

        using namespace SE;

        TInputManager::Instance().AddKeyListener   (this, sName);
        TInputManager::Instance().AddMouseListener (this, sName);

        GetSystem<EventManager>().AddListener<EInputUpdate, &EditorController::Update>(this);
}

void EditorController::Disable() {

        using namespace SE;

        TInputManager::Instance().RemoveKeyListener   (sName);
        TInputManager::Instance().RemoveMouseListener (sName);

        GetSystem<EventManager>().RemoveListener<EInputUpdate, &EditorController::Update>(this);
}

std::string EditorController::Str() const {

        return fmt::format("EditorController: delta trans: ({}, {}, {}), rot: ({}, {}, {})",
                        translation_speed.x,
                        translation_speed.y,
                        translation_speed.z,
                        rotation_speed.x,
                        rotation_speed.y,
                        rotation_speed.z);
}

void EditorController::DrawDebug() const { ;; }

}
