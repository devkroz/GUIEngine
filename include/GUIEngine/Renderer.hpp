#pragma once

#include "Types.hpp"
#include <vector>
#include <string>
#include <unordered_map>

namespace GUIEngine {

struct Vertex {
    float x, y;
    float r, g, b, a;
    float u, v;
    float textured;

    Vertex() : x(0), y(0), r(1), g(1), b(1), a(1), u(0), v(0), textured(0) {}
    Vertex(float x, float y, const Color& color)
        : x(x), y(y), r(color.r), g(color.g), b(color.b), a(color.a), u(0), v(0), textured(0) {}
    Vertex(float x, float y, float u, float v)
        : x(x), y(y), r(1), g(1), b(1), a(1), u(u), v(v), textured(1) {}
};

struct Glyph {
    uint32_t charCode;
    int width, height;
    int bearingX, bearingY;
    int advance;
    float uvX, uvY, uvW, uvH;
    int atlasX, atlasY;
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool init(int viewportWidth, int viewportHeight);
    void shutdown();

    void beginFrame();
    void endFrame();
    void flush();

    void setViewport(int width, int height);
    void getViewport(int& width, int& height) const;

    void pushScissor(const Rect& rect);
    void popScissor();
    bool isClipped(const Rect& rect) const;

    void drawRect(const Rect& rect, const Color& color, float borderRadius = 0);
    void drawRectOutline(const Rect& rect, const Color& color, float borderWidth = 1, float borderRadius = 0);
    void drawRoundedRect(const Rect& rect, const Color& color, float radius);
    void drawLine(float x1, float y1, float x2, float y2, const Color& color, float width = 1);
    void drawTriangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color& color);
    void drawShadow(const Rect& rect, float blur, const Color& color);

    void loadFont(const std::string& name, const std::string& path, float pixelSize);
    void useFont(const std::string& name);
    void reloadFonts();

    void setFontSize(float size);
    float getFontSize() const { return m_currentFontSize; }
    float getTextWidth(const std::string& text);
    float getTextHeight() const;

    uint32_t loadTexture(const std::string& name, const std::string& path);
    uint32_t loadTextureFromData(const std::string& name, int w, int h, const unsigned char* data);
    void useTexture(const std::string& name);
    void drawText(const std::string& text, float x, float y, const Color& color, float maxWidth = 0);
    void drawTextCentered(const std::string& text, float x, float y, const Color& color, float maxWidth = 0);
    void drawImage(const std::string& name, const Rect& rect, const Color& tint = Color::White());

    void clear(const Color& color);
    void setAlpha(float alpha);
    float getAlpha() const;

private:
    void ensureBatchSpace(int vertexCount, int indexCount);
    void addVertex(const Vertex& v);
    void addQuad(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3);
    void addTriangleIndices(int i0, int i1, int i2);

    void drawCharQuad(float x, float y, float w, float h, float uvX, float uvY, float uvW, float uvH, const Color& color);

    void createFontAtlas(const std::string& fontName, float pixelSize);
    void buildFontAtlasTexture();

    void setProjection(int width, int height);

    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_ibo = 0;
    unsigned int m_shaderProgram = 0;
    unsigned int m_fontTexture = 0;
    unsigned int m_projLoc = 0;
    unsigned int m_texLoc = 0;

    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    int m_viewportWidth = 800;
    int m_viewportHeight = 600;

    std::vector<Rect> m_scissorStack;

    struct FontData {
        float pixelSize;
        std::unordered_map<uint32_t, Glyph> glyphs;
        unsigned char* atlasData = nullptr;
        int atlasWidth = 0;
        int atlasHeight = 0;
        float lineSpacing = 1.0f;
        int ascent = 0;
        int descent = 0;
        int lineGap = 0;
        bool loaded = false;
    };

    std::unordered_map<std::string, FontData> m_fonts;
    std::string m_currentFont;
    float m_currentFontSize = 14.0f;
    float m_baseFontSize = 14.0f;
    float m_fontScale = 1.0f;
    uint32_t m_nextTextureId = 1;

    struct TextureData {
        unsigned int glId = 0;
        int width = 0;
        int height = 0;
    };
    std::unordered_map<std::string, TextureData> m_textures;
    std::string m_currentTexture;

    void* m_fontBuffer = nullptr;

    uint32_t decodeUTF8(const std::string& text, size_t& i);

    const Glyph* getGlyph(uint32_t codepoint, FontData& font);
    void drawRoundedRectImpl(const Rect& rect, const Color& color, float radius, bool outline, float borderWidth);
};

}
