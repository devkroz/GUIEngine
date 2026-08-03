#pragma once

#include "Application.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
#include <fstream>

namespace GUIEngine {

class GUI {
public:
    static GUI& instance() {
        static GUI instance;
        return instance;
    }

    bool init(int width = 1280, int height = 720, const std::string& title = "GUIEngine App") {
        m_window = std::make_unique<Window>();
        if (!m_window->create(title, width, height)) return false;

        m_renderer = std::make_unique<Renderer>();
        if (!m_renderer->init(width, height)) return false;

        m_app = std::make_unique<Application>();
        if (!m_app->init(m_window.get())) return false;

        setupDefaultFonts();

        return true;
    }

    void shutdown() {
        if (m_app) m_app->shutdown();
        if (m_renderer) m_renderer->shutdown();
        if (m_window) m_window->close();
    }

    void run() {
        while (m_app->isRunning() && !m_window->shouldClose()) {
            m_app->processEvents();
            m_app->update();
            m_renderer->beginFrame();
            m_app->render();
            if (m_renderCallback) m_renderCallback();
            m_renderer->endFrame();
            m_window->setCursorVisible(true);
        }
    }

    void setRoot(Widget* root) { m_app->setRoot(root); }
    void setUpdateCallback(std::function<void(float)> cb) { m_app->setUpdateCallback(std::move(cb)); }
    void setRenderAfterCallback(std::function<void()> cb) { m_renderCallback = std::move(cb); }
    void setOnResize(std::function<void(int, int)> cb) { m_app->setOnResize(std::move(cb)); }

    Window* getWindow() { return m_window.get(); }
    Renderer* getRenderer() { return m_renderer.get(); }
    Application* getApplication() { return m_app.get(); }

private:
    GUI() = default;
    ~GUI() { shutdown(); }

    void setupDefaultFonts() {
        const char* fontPaths[] = {
            "/usr/share/fonts/TTF/DejaVuSans.ttf",
            "/usr/share/fonts/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/Adwaita/AdwaitaSans-Regular.ttf",
            "/usr/share/fonts/noto/NotoSans-Regular.ttf",
            "/usr/share/fonts/liberation-fonts/LiberationSans-Regular.ttf",
            "/usr/share/fonts/gnu-free/FreeSans.ttf",
            "/usr/share/fonts/droid/DroidSans.ttf",
            nullptr
        };

        for (int i = 0; fontPaths[i]; ++i) {
            std::ifstream f(fontPaths[i]);
            if (f.good()) {
                f.close();
                m_renderer->loadFont("default", fontPaths[i], 14.0f);
                m_renderer->useFont("default");
                return;
            }
        }
    }

    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Application> m_app;
    std::function<void()> m_renderCallback;
};

}
