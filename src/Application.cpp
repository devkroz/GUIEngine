#include "GUIEngine/Application.hpp"
#include "GUIEngine/Window.hpp"
#include "GUIEngine/Renderer.hpp"
#include "GUIEngine/Widget.hpp"
#include "GUIEngine/Style.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

namespace GUIEngine {

Application::Application() {}

Application::~Application() {
    shutdown();
}

bool Application::init(Window* window) {
    m_window = window;
    if (!m_window) return false;

    m_renderer = new Renderer();
    if (!m_renderer->init(m_window->getWidth(), m_window->getHeight())) {
        return false;
    }

    m_lastTime = static_cast<float>(SDL_GetTicks()) / 1000.0f;

    return true;
}

void Application::shutdown() {
    if (m_renderer) {
        m_renderer->shutdown();
        delete m_renderer;
        m_renderer = nullptr;
    }
    m_root = nullptr;
    m_focusedWidget = nullptr;
    m_hoveredWidget = nullptr;
    m_pressedWidget = nullptr;
}

void Application::setRoot(Widget* root) {
    m_root = root;
    m_layoutDirty = true;
}

void Application::removeRoot() {
    m_root = nullptr;
}

void Application::processEvents() {
    std::vector<Event> events;
    if (!m_window->pollEvents(events)) return;

    for (const Event& event : events) {
        dispatchEvent(event);
    }
}

void Application::dispatchEvent(const Event& event) {
    if (event.type == EventType::Close) {
        m_running = false;
        return;
    }

    if (event.type == EventType::Resize) {
        m_renderer->setViewport(static_cast<int>(event.width), static_cast<int>(event.height));
        m_layoutDirty = true;
        if (m_resizeCallback) m_resizeCallback(static_cast<int>(event.width), static_cast<int>(event.height));
    }

    if (!m_root) return;

    switch (event.type) {
        case EventType::MouseMove: {
            Widget* target = findWidgetAt(m_root, event.mouseX, event.mouseY);
            if (target != m_hoveredWidget) {
                if (m_hoveredWidget) {
                    Event leave = event;
                    leave.type = EventType::MouseLeave;
                    m_hoveredWidget->handleEvent(leave);
                }
                m_hoveredWidget = target;
                if (target) {
                    Event enter = event;
                    enter.type = EventType::MouseEnter;
                    target->handleEvent(enter);
                }
            }

            if (m_hoveredWidget) {
                m_hoveredWidget->handleEvent(event);
            }

            m_lastMouse = Vec2(event.mouseX, event.mouseY);
            break;
        }
        case EventType::MouseButtonPress: {
            Widget* target = findWidgetAt(m_root, event.mouseX, event.mouseY);
            if (target) {
                m_pressedWidget = target;
                target->handleEvent(event);
                if (target->isFocusable() && event.mouseButton == 1) {
                    requestFocus(target);
                }
            } else {
                if (event.mouseButton == 1) clearFocus();
            }
            break;
        }
        case EventType::MouseButtonRelease: {
            if (m_pressedWidget) {
                m_pressedWidget->handleEvent(event);
                if (m_pressedWidget && m_pressedWidget != m_hoveredWidget) {
                }
                m_pressedWidget = nullptr;
            }
            if (m_hoveredWidget) {
                m_hoveredWidget->handleEvent(event);
            }
            break;
        }
        case EventType::MouseScroll: {
            Widget* target = findWidgetAt(m_root, event.mouseX, event.mouseY);
            if (target) {
                target->handleEvent(event);
            }
            break;
        }
        case EventType::KeyPress: {
            if (m_focusedWidget) {
                m_focusedWidget->handleEvent(event);
            } else if (m_root) {
                m_root->handleEvent(event);
            }
            break;
        }
        case EventType::KeyRelease: {
            if (m_focusedWidget) {
                m_focusedWidget->handleEvent(event);
            }
            break;
        }
        case EventType::TextInput: {
            if (m_focusedWidget) {
                m_focusedWidget->handleEvent(event);
            }
            break;
        }
        default:
            break;
    }
}

Widget* Application::findWidgetAt(Widget* root, float x, float y) {
    if (!root || !root->isVisible()) return nullptr;
    if (!root->hitTest(x, y)) return nullptr;

    const auto& children = root->getChildren();
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        Widget* child = *it;
        Widget* found = findWidgetAt(child, x, y);
        if (found) return found;
    }

    return root;
}

void Application::setHovered(Widget* widget) {
    if (m_hoveredWidget == widget) return;
    if (m_hoveredWidget) {
        Event e;
        e.type = EventType::MouseLeave;
        m_hoveredWidget->handleEvent(e);
    }
    m_hoveredWidget = widget;
    if (widget) {
        Event e;
        e.type = EventType::MouseEnter;
        widget->handleEvent(e);
    }
}

void Application::requestFocus(Widget* widget) {
    if (m_focusedWidget == widget) return;
    if (m_focusedWidget) {
        Event e;
        e.type = EventType::FocusOut;
        m_focusedWidget->handleEvent(e);
    }
    m_focusedWidget = widget;
    if (widget) {
        Event e;
        e.type = EventType::FocusIn;
        widget->handleEvent(e);
    }
}

void Application::clearFocus() {
    if (m_focusedWidget) {
        Event e;
        e.type = EventType::FocusOut;
        m_focusedWidget->handleEvent(e);
        m_focusedWidget = nullptr;
    }
}

void Application::doLayout() {
    if (!m_root) return;

    int w, h;
    m_window->getSize(w, h);

    Rect rootRect(0, 0, static_cast<float>(w), static_cast<float>(h));
    m_root->measure(static_cast<float>(w), static_cast<float>(h));
    m_root->layout(rootRect);

    m_layoutDirty = false;
}

void Application::update() {
    float currentTime = static_cast<float>(SDL_GetTicks()) / 1000.0f;
    float deltaTime = currentTime - m_lastTime;
    m_lastTime = currentTime;

    if (m_layoutDirty) {
        doLayout();
    }

    if (m_updateCallback) m_updateCallback(deltaTime);

    std::function<void(Widget*)> updateRecursive = [&](Widget* widget) {
        if (!widget || !widget->isVisible()) return;
        widget->update(deltaTime);
        for (Widget* child : widget->getChildren()) {
            updateRecursive(child);
        }
    };

    if (m_root) updateRecursive(m_root);
}

void Application::render() {
    if (!m_root || !m_renderer) return;
    m_root->render(*m_renderer);
}

void Application::resize(int width, int height) {
    if (m_renderer) m_renderer->setViewport(width, height);
    m_layoutDirty = true;
}

}
