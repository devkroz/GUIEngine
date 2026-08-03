#include "GUIEngine/Window.hpp"

#include <SDL2/SDL.h>
#include <iostream>

namespace GUIEngine {

Window::Window() {
}

Window::~Window() {
    close();
    freeCursors();
}

bool Window::create(const std::string& title, int width, int height, bool fullscreen, bool resizable) {
    if (m_window) {
        SDL_DestroyWindow(m_window);
    }

    uint32_t flags = SDL_WINDOW_OPENGL;
    if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    if (resizable) flags |= SDL_WINDOW_RESIZABLE;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    m_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        flags
    );

    if (!m_window) {
        std::cerr << "[GUIEngine] Failed to create window: " << SDL_GetError() << std::endl;
        return false;
    }

    m_width = width;
    m_height = height;
    m_fullscreen = fullscreen;
    m_shouldClose = false;

    initCursors();

    SDL_GLContext glContext = SDL_GL_GetCurrentContext();
    if (!glContext) {
        glContext = SDL_GL_CreateContext(m_window);
    }

    if (!glContext) {
        std::cerr << "[GUIEngine] Failed to create GL context: " << SDL_GetError() << std::endl;
        return false;
    }

    setVSync(m_vsync);

    int actualW, actualH;
    SDL_GetWindowSize(m_window, &actualW, &actualH);
    m_width = actualW;
    m_height = actualH;

    return true;
}

void Window::close() {
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    m_shouldClose = true;
}

void Window::setTitle(const std::string& title) {
    if (m_window) SDL_SetWindowTitle(m_window, title.c_str());
}

void Window::setSize(int width, int height) {
    if (m_window) SDL_SetWindowSize(m_window, width, height);
    m_width = width;
    m_height = height;
}

void Window::getSize(int& width, int& height) const {
    if (m_window) SDL_GetWindowSize(m_window, &width, &height);
    else { width = m_width; height = m_height; }
}

void Window::setPosition(int x, int y) {
    if (m_window) SDL_SetWindowPosition(m_window, x, y);
}

void Window::setVSync(bool enabled) {
    m_vsync = enabled;
    int result = SDL_GL_SetSwapInterval(enabled ? 1 : 0);
    if (result != 0 && enabled) {
        // Tentar VSync adaptativo se o normal falhar
        SDL_GL_SetSwapInterval(-1);
    }
}

void Window::setFullscreen(bool enabled) {
    m_fullscreen = enabled;
    if (m_window) SDL_SetWindowFullscreen(m_window, enabled ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

void Window::show() {
    if (m_window) SDL_ShowWindow(m_window);
}

void Window::hide() {
    if (m_window) SDL_HideWindow(m_window);
}

void Window::setCursor(CursorType type) {
    if (type == m_currentCursor) return;
    m_currentCursor = type;
    int index = static_cast<int>(type);
    if (index < (int)m_cursors.size() && m_cursors[index]) {
        SDL_SetCursor(m_cursors[index]);
    }
}

void Window::setCursorVisible(bool visible) {
    SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
}

void Window::initCursors() {
    freeCursors();
    m_cursors.resize(10, nullptr);
    m_cursors[static_cast<int>(CursorType::Arrow)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    m_cursors[static_cast<int>(CursorType::IBeam)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    m_cursors[static_cast<int>(CursorType::Hand)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    m_cursors[static_cast<int>(CursorType::Wait)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
    m_cursors[static_cast<int>(CursorType::Crosshair)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    m_cursors[static_cast<int>(CursorType::ResizeNS)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
    m_cursors[static_cast<int>(CursorType::ResizeEW)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
    m_cursors[static_cast<int>(CursorType::ResizeNWSE)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
    m_cursors[static_cast<int>(CursorType::ResizeNESW)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
    m_cursors[static_cast<int>(CursorType::Forbidden)] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
}

void Window::freeCursors() {
    for (auto* c : m_cursors) {
        if (c) SDL_FreeCursor(c);
    }
    m_cursors.clear();
}

bool Window::pollEvents(std::vector<Event>& events) {
    events.clear();
    SDL_Event sdlEvent;

    while (SDL_PollEvent(&sdlEvent)) {
        processSDLEvent(sdlEvent, events);
    }

    return !events.empty();
}

void Window::processSDLEvent(const SDL_Event& sdlEvent, std::vector<Event>& events) {
    int modifiers = 0;
    SDL_Keymod km = SDL_GetModState();
    if (km & KMOD_SHIFT) modifiers |= 0x01;
    if (km & KMOD_CTRL) modifiers |= 0x02;
    if (km & KMOD_ALT) modifiers |= 0x04;
    if (km & KMOD_GUI) modifiers |= 0x08;

    switch (sdlEvent.type) {
        case SDL_QUIT: {
            Event e;
            e.type = EventType::Close;
            events.push_back(e);
            m_shouldClose = true;
            break;
        }
        case SDL_WINDOWEVENT: {
            if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED ||
                sdlEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                m_width = sdlEvent.window.data1;
                m_height = sdlEvent.window.data2;
                Event e;
                e.type = EventType::Resize;
                e.width = static_cast<float>(m_width);
                e.height = static_cast<float>(m_height);
                events.push_back(e);
            }
            break;
        }
        case SDL_MOUSEMOTION: {
            Event e;
            e.type = EventType::MouseMove;
            e.mouseX = static_cast<float>(sdlEvent.motion.x);
            e.mouseY = static_cast<float>(sdlEvent.motion.y);
            e.mouseDeltaX = static_cast<float>(sdlEvent.motion.xrel);
            e.mouseDeltaY = static_cast<float>(sdlEvent.motion.yrel);
            e.modifiers = modifiers;
            events.push_back(e);
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            Event e;
            e.type = EventType::MouseButtonPress;
            e.mouseX = static_cast<float>(sdlEvent.button.x);
            e.mouseY = static_cast<float>(sdlEvent.button.y);
            e.mouseButton = sdlEvent.button.button;
            e.modifiers = modifiers;
            events.push_back(e);
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            Event e;
            e.type = EventType::MouseButtonRelease;
            e.mouseX = static_cast<float>(sdlEvent.button.x);
            e.mouseY = static_cast<float>(sdlEvent.button.y);
            e.mouseButton = sdlEvent.button.button;
            e.modifiers = modifiers;
            events.push_back(e);
            break;
        }
        case SDL_MOUSEWHEEL: {
            Event e;
            e.type = EventType::MouseScroll;
            e.scrollX = static_cast<float>(sdlEvent.wheel.x);
            e.scrollY = static_cast<float>(sdlEvent.wheel.y);
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            e.mouseX = static_cast<float>(mx);
            e.mouseY = static_cast<float>(my);
            e.modifiers = modifiers;
            events.push_back(e);
            break;
        }
        case SDL_KEYDOWN: {
            Event e;
            e.type = EventType::KeyPress;
            e.key = sdlEvent.key.keysym.sym;
            e.modifiers = modifiers;
            events.push_back(e);
            break;
        }
        case SDL_KEYUP: {
            Event e;
            e.type = EventType::KeyRelease;
            e.key = sdlEvent.key.keysym.sym;
            e.modifiers = modifiers;
            events.push_back(e);
            break;
        }
        case SDL_TEXTINPUT: {
            Event e;
            e.type = EventType::TextInput;
            e.text = sdlEvent.text.text;
            e.modifiers = modifiers;
            events.push_back(e);
            break;
        }
    }
}

}
