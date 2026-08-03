#pragma once

#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <any>
#include <unordered_map>
#include <algorithm>

namespace GUIEngine {

enum class EventType {
    MouseButtonPress,
    MouseButtonRelease,
    MouseMove,
    MouseEnter,
    MouseLeave,
    MouseScroll,
    KeyPress,
    KeyRelease,
    TextInput,
    FocusIn,
    FocusOut,
    Resize,
    Close,
    Custom
};

struct Event {
    EventType type;
    float mouseX = 0;
    float mouseY = 0;
    float mouseDeltaX = 0;
    float mouseDeltaY = 0;
    int mouseButton = 0;
    int key = 0;
    int modifiers = 0;
    std::string text;
    float scrollX = 0;
    float scrollY = 0;
    float width = 0;
    float height = 0;
    std::any userData;

    bool shiftHeld() const { return (modifiers & 0x01) != 0; }
    bool ctrlHeld() const  { return (modifiers & 0x02) != 0; }
    bool altHeld() const   { return (modifiers & 0x04) != 0; }
    bool superHeld() const { return (modifiers & 0x08) != 0; }
};

using EventCallback = std::function<void(const Event&)>;

class EventDispatcher {
public:
    using HandlerId = uint64_t;

    template<typename T>
    HandlerId on(EventType type, T&& callback) {
        HandlerId id = m_nextId++;
        m_handlers[type].push_back({id, EventCallback(std::forward<T>(callback))});
        m_handlerOrder.push_back({id, type});
        return id;
    }

    void off(HandlerId id) {
        for (auto& [type, handlers] : m_handlers) {
            handlers.erase(
                std::remove_if(handlers.begin(), handlers.end(),
                    [id](const std::pair<HandlerId, EventCallback>& h) { return h.first == id; }),
                handlers.end()
            );
        }
        m_handlerOrder.erase(
            std::remove_if(m_handlerOrder.begin(), m_handlerOrder.end(),
                [id](const std::pair<HandlerId, EventType>& h) { return h.first == id; }),
            m_handlerOrder.end()
        );
    }

    void dispatch(const Event& event) {
        auto it = m_handlers.find(event.type);
        if (it == m_handlers.end()) return;
        auto handlers = it->second;
        for (auto& [id, callback] : handlers) {
            callback(event);
        }
    }

    void clear() {
        m_handlers.clear();
        m_handlerOrder.clear();
    }

private:
    std::unordered_map<EventType, std::vector<std::pair<HandlerId, EventCallback>>> m_handlers;
    std::vector<std::pair<HandlerId, EventType>> m_handlerOrder;
    static inline HandlerId m_nextId = 1;
};

}
