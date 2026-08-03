#pragma once

#include "Types.hpp"
#include "Layout.hpp"
#include <string>
#include <unordered_map>
#include <variant>

namespace GUIEngine {

using StyleValue = std::variant<std::monostate, Color, float, int, std::string, bool>;

class Style {
public:
    Style() = default;

    void set(const std::string& key, const StyleValue& value) { m_values[key] = value; }
    void remove(const std::string& key) { m_values.erase(key); }
    bool has(const std::string& key) const { return m_values.count(key) > 0; }

    Color getColor(const std::string& key, const Color& def = Color::Gray()) const {
        auto it = m_values.find(key);
        if (it == m_values.end()) return def;
        if (std::holds_alternative<Color>(it->second)) return std::get<Color>(it->second);
        return def;
    }

    float getFloat(const std::string& key, float def = 0) const {
        auto it = m_values.find(key);
        if (it == m_values.end()) return def;
        if (std::holds_alternative<float>(it->second)) return std::get<float>(it->second);
        if (std::holds_alternative<int>(it->second)) return static_cast<float>(std::get<int>(it->second));
        return def;
    }

    int getInt(const std::string& key, int def = 0) const {
        auto it = m_values.find(key);
        if (it == m_values.end()) return def;
        if (std::holds_alternative<int>(it->second)) return std::get<int>(it->second);
        if (std::holds_alternative<float>(it->second)) return static_cast<int>(std::get<float>(it->second));
        return def;
    }

    std::string getString(const std::string& key, const std::string& def = "") const {
        auto it = m_values.find(key);
        if (it == m_values.end()) return def;
        if (std::holds_alternative<std::string>(it->second)) return std::get<std::string>(it->second);
        return def;
    }

    bool getBool(const std::string& key, bool def = false) const {
        auto it = m_values.find(key);
        if (it == m_values.end()) return def;
        if (std::holds_alternative<bool>(it->second)) return std::get<bool>(it->second);
        return def;
    }

    void merge(const Style& other) {
        for (const auto& [k, v] : other.m_values) {
            if (!std::holds_alternative<std::monostate>(v)) {
                m_values[k] = v;
            }
        }
    }

    const Style& operator[](const std::string& state) const {
        auto it = m_stateStyles.find(state);
        if (it != m_stateStyles.end()) return it->second;
        return *this;
    }

    Style& state(const std::string& stateName) {
        return m_stateStyles[stateName];
    }

    bool hasState(const std::string& stateName) const {
        return m_stateStyles.count(stateName) > 0;
    }

    const Style& resolveState(const std::vector<std::string>& states) const {
        const Style* result = this;
        for (const auto& s : states) {
            auto it = m_stateStyles.find(s);
            if (it != m_stateStyles.end()) return it->second;
        }
        return *result;
    }

private:
    std::unordered_map<std::string, StyleValue> m_values;
    std::unordered_map<std::string, Style> m_stateStyles;
};

class Theme {
public:
    Theme() {
        m_default.set("backgroundColor", Color(0.12f, 0.12f, 0.14f, 1.0f));
        m_default.set("foregroundColor", Color(0.88f, 0.88f, 0.92f, 1.0f));
        m_default.set("primaryColor", Color(0.2f, 0.5f, 0.95f, 1.0f));
        m_default.set("primaryColorHovered", Color(0.3f, 0.6f, 1.0f, 1.0f));
        m_default.set("primaryColorPressed", Color(0.15f, 0.4f, 0.85f, 1.0f));
        m_default.set("accentColor", Color(0.95f, 0.55f, 0.15f, 1.0f));
        m_default.set("dangerColor", Color(0.85f, 0.2f, 0.2f, 1.0f));
        m_default.set("successColor", Color(0.2f, 0.75f, 0.3f, 1.0f));
        m_default.set("borderColor", Color(0.3f, 0.3f, 0.35f, 1.0f));
        m_default.set("textColor", Color(0.88f, 0.88f, 0.92f, 1.0f));
        m_default.set("textColorMuted", Color(0.55f, 0.55f, 0.6f, 1.0f));
        m_default.set("buttonColor", Color(0.22f, 0.22f, 0.26f, 1.0f));
        m_default.set("buttonColorHovered", Color(0.28f, 0.28f, 0.33f, 1.0f));
        m_default.set("buttonColorPressed", Color(0.18f, 0.18f, 0.22f, 1.0f));
        m_default.set("inputColor", Color(0.16f, 0.16f, 0.19f, 1.0f));
        m_default.set("inputColorFocused", Color(0.18f, 0.18f, 0.22f, 1.0f));
        m_default.set("panelColor", Color(0.16f, 0.16f, 0.19f, 1.0f));
        m_default.set("fontFamily", std::string("default"));
        m_default.set("fontSize", 14);
        m_default.set("fontSizeSmall", 11);
        m_default.set("fontSizeLarge", 18);
        m_default.set("fontSizeTitle", 24);
        m_default.set("borderRadius", 6.0f);
        m_default.set("borderRadiusSmall", 3.0f);
        m_default.set("borderRadiusLarge", 12.0f);
        m_default.set("borderWidth", 1.0f);
        m_default.set("shadowColor", Color(0.0f, 0.0f, 0.0f, 0.3f));
        m_default.set("shadowBlur", 12.0f);
    }

    static Theme& defaultTheme() {
        static Theme instance;
        return instance;
    }

    const Style& get() const { return m_default; }
    Style& get() { return m_default; }

    Color get(const std::string& key, const Color& def = Color::Gray()) const {
        return m_default.getColor(key, def);
    }

    float getFloat(const std::string& key, float def = 0) const {
        return m_default.getFloat(key, def);
    }

    int getInt(const std::string& key, int def = 0) const {
        return m_default.getInt(key, def);
    }

    void set(const std::string& key, const StyleValue& value) {
        m_default.set(key, value);
    }

private:
    Style m_default;
};

}
