#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <imgui.h>

using namespace geode::prelude;

// Variables globales para guardar el estado de los hacks
bool g_godMode = false;
bool g_antiCrash = false;
bool g_noClip = false;
bool g_showMenu = false;

// ==========================================
// 1. LÓGICA DE LOS HACKS (Hooks de PlayLayer)
// ==========================================
class $modify(MyPlayLayer, PlayLayer) {
    
    // Hack: God Mode / No Clip
    void destroyPlayer(PlayerObject* player, GameObject* object) {
        // Si God Mode o No Clip están activados, ignoramos el daño
        if (g_godMode || g_noClip) {
            return;
        }
        // Si no, el juego sigue su curso normal y el jugador muere
        PlayLayer::destroyPlayer(player, object);
    }

    // Hack: Anti-Crash básico
    void update(float dt) {
        // Evitamos que un delta time corrupto o un lagazo extremo crasheen el juego
        if (g_antiCrash && dt > 2.0f) {
            dt = 1.0f / 60.0f; 
        }
        PlayLayer::update(dt);
    }
};

// ==========================================
// 2. INTERFAZ DEL MOD MENU (ImGui)
// ==========================================
// Esta función dibuja el menú flotante en la pantalla
void drawModMenu() {
    if (!g_showMenu) return;

    // Creamos la ventana de nuestro Mod Menu
    ImGui::Begin("Oc Blox Menu v1.0", &g_showMenu);

    ImGui::Text("¡Bienvenido a tu Mod Menu!");
    ImGui::Separator();

    // Checkboxes para activar o desactivar cada hack
    if (ImGui::Checkbox("God Mode", &g_godMode)) {
        FLAlertLayer::create("Mod Menu", g_godMode ? "God Mode Activado" : "God Mode Desactivado", "OK")->show();
    }

    if (ImGui::Checkbox("No Clip", &g_noClip)) {
        log::info("No Clip cambiado a: {}", g_noClip);
    }

    if (ImGui::Checkbox("Anti-Crash", &g_antiCrash)) {
        log::info("Anti-Crash cambiado a: {}", g_antiCrash);
    }

    ImGui::Separator();
    if (ImGui::Button("Cerrar Menú")) {
        g_showMenu = false;
    }

    ImGui::End();
}

// ==========================================
// 3. INYECCIÓN DEL BOTÓN EN EL MENÚ PRINCIPAL
// ==========================================
class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        // Creamos un botón personalizado en la pantalla de inicio para abrir el menú
        auto sprite = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png"); // Botón con icono de '+'
        auto btn = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(MyMenuLayer::onOpenModMenu)
        );

        // Buscamos el menú del juego para añadir nuestro botón
        auto menu = this->getChildByID("bottom-menu");
        if (menu) {
            menu->addChild(btn);
            menu->updateLayout();
        }

        return true;
    }

    // Función que se ejecuta al presionar el botón de '+'
    void onOpenModMenu(CCObject* sender) {
        // Cambiamos el estado (si estaba cerrado se abre, y viceversa)
        g_showMenu = !g_showMenu;
    }
};

// ==========================================
// 4. REGISTRO DE LAPAFA DE IMGUI (Geode)
// ==========================================
// Le decimos a Geode que dibuje nuestra interfaz de ImGui en cada frame
$execute {
    ImGuiLayer::get()->addRenderCallback(&drawModMenu);
}

