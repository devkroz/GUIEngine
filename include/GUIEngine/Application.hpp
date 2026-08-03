#pragma once

#include "Widget.hpp"
#include "Style.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace GUIEngine {

class Window;
class Renderer;

enum class MouseCursor {
    Arrow,
    Text,
    Hand,
    ResizeLeftRight,
    ResizeUpDown,
    Loading
};

class Application {
public:
    Application();
    ~Application();

    bool init(Window* window);
    void shutdown();

    void setRoot(Widget* root);
    void removeRoot();

    void processEvents();

    void update();
    void render();
    void resize(int width, int height);

    void quit() { m_running = false; }
    bool isRunning() const { return m_running; }

    Window* getWindow() { return m_window; }
    Renderer* getRenderer() { return m_renderer; }
    void setRenderer(Renderer* r) { m_renderer = r; }

    void setTheme(const Style& theme) { m_theme = theme; }
    const Style& getTheme() const { return m_theme; }

    void requestFocus(Widget* widget);
    void clearFocus();

    void setUpdateCallback(std::function<void(float)> cb) { m_updateCallback = std::move(cb); }
    void setRenderCallback(std::function<void()> cb) { m_renderCallback = std::move(cb); }
    void setOnResize(std::function<void(int, int)> cb) { m_resizeCallback = std::move(cb); }

    void scheduleLayout() { m_layoutDirty = true; }

private:
    Window* m_window = nullptr;
    Renderer* m_renderer = nullptr;
    Widget* m_root = nullptr;
    Widget* m_focusedWidget = nullptr;
    Widget* m_hoveredWidget = nullptr;
    Widget* m_pressedWidget = nullptr;

    bool m_running = true;
    bool m_layoutDirty = true;

    float m_lastTime = 0;

    Vec2 m_lastMouse;

    Style m_theme;

    std::function<void(float)> m_updateCallback;
    std::function<void()> m_renderCallback;
    std::function<void(int, int)> m_resizeCallback;

    void dispatchEvent(const Event& event);
    void propagateMouseEnterLeave(Widget* widget, float x, float y);
    void doLayout();
    Widget* findWidgetAt(Widget* root, float x, float y);
    Widget* findFocusable(Widget* root, bool forward);
    void setHovered(Widget* widget);
};

}
