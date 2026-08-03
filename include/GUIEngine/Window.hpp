#pragma once

#include "Types.hpp"
#include "Event.hpp"
#include <string>
#include <functional>
#include <vector>

struct SDL_Window;
struct SDL_Cursor;
union SDL_Event;

namespace GUIEngine {

enum class CursorType {
    Arrow,
    IBeam,
    Hand,
    Wait,
    Crosshair,
    ResizeNS,
    ResizeEW,
    ResizeNWSE,
    ResizeNESW,
    Forbidden
};

class Window {
public:
    Window();
    ~Window();

    bool create(const std::string& title, int width, int height, bool fullscreen = false, bool resizable = true);
    void close();

    void setTitle(const std::string& title);
    void setSize(int width, int height);
    void getSize(int& width, int& height) const;
    void setPosition(int x, int y);
    void setVSync(bool enabled);
    void setFullscreen(bool enabled);
    bool isFullscreen() const { return m_fullscreen; }

    void show();
    void hide();
    void setCursor(CursorType type);
    void setCursorVisible(bool visible);

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

    bool pollEvents(std::vector<Event>& events);

    SDL_Window* getHandle() { return m_window; }

    bool shouldClose() const { return m_shouldClose; }
    void requestClose() { m_shouldClose = true; }

    void setEventHandler(std::function<void(const Event&)> handler) { m_eventHandler = handler; }

private:
    SDL_Window* m_window = nullptr;
    std::vector<SDL_Cursor*> m_cursors;
    int m_width = 800;
    int m_height = 600;
    bool m_fullscreen = false;
    bool m_shouldClose = false;
    bool m_vsync = true;
    CursorType m_currentCursor = CursorType::Arrow;
    std::function<void(const Event&)> m_eventHandler;

    void initCursors();
    void freeCursors();
    CursorType m_prevCursor = CursorType::Arrow;

    void processSDLEvent(const SDL_Event& sdlEvent, std::vector<Event>& events);
};

}
