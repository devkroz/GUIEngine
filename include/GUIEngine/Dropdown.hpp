#pragma once

#include "Widget.hpp"
#include "Label.hpp"
#include <functional>

namespace GUIEngine {

struct Choice {
    std::string value;
    std::string label;
};

class Dropdown : public Widget {
public:
    Dropdown();
    ~Dropdown() override = default;

    void setItems(const std::vector<Choice>& items);
    void addItem(const Choice& item);
    void removeItem(const std::string& value);
    void addItem(const std::string& value, const std::string& label);
    void clearItems();

    void setSelected(const std::string& value);
    void setSelectedIndex(int index);
    const std::string& getSelected() const { return m_selectedValue; }
    int getSelectedIndex() const;
    const std::vector<Choice>& getItems() const { return m_items; }

    void setOnChange(std::function<void(const std::string&)> cb) { m_onChange = std::move(cb); }

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;
    void handleEvent(const Event& event) override;

private:
    std::vector<Choice> m_items;
    std::string m_selectedValue;
    int m_selectedIndex = -1;
    bool m_open = false;
    float m_fontSize = 0;
    float m_maxWidth = 200;

    std::function<void(const std::string&)> m_onChange;

    Rect getPopupRect() const;
    int getHoveredIndex(float mouseY) const;
    int m_hoveredIndex = -1;
};

class RadioButton : public Widget {
public:
    RadioButton(const std::string& label = "");
    ~RadioButton() override = default;

    void setLabel(const std::string& text);
    const std::string& getLabel() const { return m_label ? m_label->getText() : m_labelText; }

    void setSelected(bool selected) { m_selected = selected; }
    bool isSelected() const { return m_selected; }

    void setGroup(const std::string& group) { m_group = group; }
    const std::string& getGroup() const { return m_group; }

    void setOnChange(std::function<void(bool)> cb) { m_onChange = std::move(cb); }

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;
    void handleEvent(const Event& event) override;

private:
    std::string m_labelText;
    std::string m_group;
    Label* m_label = nullptr;
    bool m_selected = false;
    float m_circleSize = 18;
    std::function<void(bool)> m_onChange;
};

}
