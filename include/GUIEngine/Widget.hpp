#pragma once

#include "Types.hpp"
#include "Event.hpp"
#include "Layout.hpp"
#include "Style.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace GUIEngine {

class Renderer;
class Container;

enum class WidgetState {
    Normal,
    Hovered,
    Pressed,
    Focused,
    Disabled,
    Active
};

class Widget {
public:
    Widget(const std::string& id = "");
    virtual ~Widget();

    const std::string& getId() const { return m_id; }
    void setId(const std::string& id) { m_id = id; }

    const Rect& getGeometry() const { return m_geometry; }
    Vec2 getPosition() const { return Vec2(m_geometry.x, m_geometry.y); }
    Vec2 getSize() const { return Vec2(m_geometry.width, m_geometry.height); }

    void setPosition(float x, float y) { m_geometry.x = x; m_geometry.y = y; }
    void setSize(float w, float h) { m_geometry.width = w; m_geometry.height = h; }

    void setWidth(float w) { m_geometry.width = w; }
    void setHeight(float h) { m_geometry.height = h; }

    float getX() const { return m_geometry.x; }
    float getY() const { return m_geometry.y; }
    float getWidth() const { return m_geometry.width; }
    float getHeight() const { return m_geometry.height; }

    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }

    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled);

    bool isHovered() const { return m_hovered; }
    bool isPressed() const { return m_pressed; }
    bool isFocused() const { return m_focused; }
    bool hasState(WidgetState state) const { return m_state == state; }

    virtual void setState(WidgetState state);
    WidgetState getState() const { return m_state; }

    LayoutParams& getLayout() { return m_layout; }
    const LayoutParams& getLayout() const { return m_layout; }
    void setLayout(const LayoutParams& params) { m_layout = params; }

    void setSizePolicy(LayoutDirection dir, float grow, float shrink = 1) {
        if (dir == LayoutDirection::Row) { m_layout.grow = grow; m_layout.shrink = shrink; }
        else { m_layout.grow = grow; m_layout.shrink = shrink; }
    }

    void setMargin(const Margin& m) { m_layout.margin = m; }
    void setPadding(const Padding& p) { m_layout.padding = p; }
    void setFixedWidth(float w) { m_layout.fixedWidth = w; m_layout.minWidth = w; }
    void setFixedHeight(float h) { m_layout.fixedHeight = h; m_layout.minHeight = h; }
    void setMinWidth(float w) { m_layout.minWidth = w; }
    void setMinHeight(float h) { m_layout.minHeight = h; }

    Style& getStyle() { return m_style; }
    const Style& getStyle() const { return m_style; }
    void setStyle(const Style& style) { m_style = style; }

    void setClass(const std::string& cls) { m_className = cls; }
    const std::string& getClass() const { return m_className; }

    Widget* parent() { return m_parent; }
    const Widget* parent() const { return m_parent; }
    void setParent(Widget* parent) { m_parent = parent; }

    virtual Vec2 measure(float availableWidth, float availableHeight);
    virtual void layout(const Rect& bounds);
    virtual void render(Renderer& renderer) = 0;
    virtual void update(float deltaTime) {}

    virtual bool onEvent(const Event& event) { return false; }

    virtual bool hitTest(float x, float y) const {
        if (!m_visible || !m_enabled) return false;
        return m_geometry.contains(x, y);
    }

    template<typename T, typename... Args>
    T* add(Args&&... args) {
        static_assert(std::is_base_of<Widget, T>::value, "T must inherit from Widget");
        auto child = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = child.get();
        addChild(std::move(child));
        return ptr;
    }

    virtual void addChild(std::unique_ptr<Widget> child);
    virtual void removeChild(Widget* child);
    virtual void removeAllChildren();
    virtual const std::vector<Widget*>& getChildren() const;
    virtual std::vector<Widget*>& getChildren();

    virtual bool isContainer() const { return false; }

    void setOnClickListener(std::function<void()> cb) { m_onClick = std::move(cb); }
    void setOnPressedListener(std::function<void()> cb) { m_onPressed = std::move(cb); }
    void setOnReleasedListener(std::function<void()> cb) { m_onReleased = std::move(cb); }
    void setOnHoverEnterListener(std::function<void()> cb) { m_onHoverEnter = std::move(cb); }
    void setOnHoverLeaveListener(std::function<void()> cb) { m_onHoverLeave = std::move(cb); }
    void setOnKeyListener(std::function<bool(const Event&)> cb) { m_onKey = std::move(cb); }
    void setOnFocusChangeListener(std::function<void(bool)> cb) { m_onFocusChange = std::move(cb); }
    void setOnResizeListener(std::function<void(float, float)> cb) { m_onResize = std::move(cb); }

    void focus();
    void unfocus();
    void setFocusable(bool focusable) { m_focusable = focusable; }
    bool isFocusable() const { return m_focusable; }

protected:
    std::string m_id;
    std::string m_className;
    Rect m_geometry;
    LayoutParams m_layout;
    Style m_style;
    Widget* m_parent = nullptr;
    std::vector<std::unique_ptr<Widget>> m_children;
    std::vector<Widget*> m_childRefs;

    bool m_visible = true;
    bool m_enabled = true;
    bool m_hovered = false;
    bool m_pressed = false;
    bool m_focused = false;
    bool m_focusable = false;
    WidgetState m_state = WidgetState::Normal;

    virtual Vec2 measureContent(float availableWidth, float availableHeight);
    virtual void handleEvent(const Event& event);

    friend class Application;

private:
    void notifyParentLayoutChanged();

    std::function<void()> m_onClick;
    std::function<void()> m_onPressed;
    std::function<void()> m_onReleased;
    std::function<void()> m_onHoverEnter;
    std::function<void()> m_onHoverLeave;
    std::function<bool(const Event&)> m_onKey;
    std::function<void(bool)> m_onFocusChange;
    std::function<void(float, float)> m_onResize;
};

}
