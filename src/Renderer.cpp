#include "GUIEngine/Renderer.hpp"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stb_image.h>
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstring>

namespace GUIEngine {

static const char* VERTEX_SHADER = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aUV;
layout (location = 3) in float aTextured;

out vec4 vColor;
out vec2 vUV;
out float vTextured;

uniform mat4 uProjection;

void main() {
    vColor = aColor;
    vUV = aUV;
    vTextured = aTextured;
    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
}
)";

static const char* FRAGMENT_SHADER = R"(
#version 330 core
in vec4 vColor;
in vec2 vUV;
in float vTextured;

out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    if (vTextured > 0.5) {
        vec4 texColor = texture(uTexture, vUV);
        FragColor = vec4(vColor.rgb, vColor.a * texColor.a);
    } else {
        FragColor = vColor;
    }
}
)";

static unsigned int compileShader(GLenum type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "[GUIEngine] Shader compile error: " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static unsigned int createShaderProgram(const char* vSrc, const char* fSrc) {
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vSrc);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fSrc);
    if (!vs || !fs) return 0;

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "[GUIEngine] Shader link error: " << infoLog << std::endl;
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

static void orthoProjection(float* mat, float left, float right, float bottom, float top) {
    std::memset(mat, 0, sizeof(float) * 16);
    mat[0] = 2.0f / (right - left);
    mat[5] = 2.0f / (top - bottom);
    mat[10] = -1.0f;
    mat[12] = -(right + left) / (right - left);
    mat[13] = -(top + bottom) / (top - bottom);
    mat[15] = 1.0f;
}

Renderer::Renderer() {}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::init(int viewportWidth, int viewportHeight) {
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "[GUIEngine] Erro ao inicializar GLEW: " << glewGetErrorString(err) << std::endl;
        return false;
    }

    m_shaderProgram = createShaderProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    if (!m_shaderProgram) return false;

    m_projLoc = glGetUniformLocation(m_shaderProgram, "uProjection");
    m_texLoc = glGetUniformLocation(m_shaderProgram, "uTexture");

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ibo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 10000, nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 20000, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(float)));

    glBindVertexArray(0);

    m_vertices.reserve(10000);
    m_indices.reserve(20000);

    setViewport(viewportWidth, viewportHeight);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);

    // Criar textura dummy para evitar crashes se nenhuma fonte for carregada
    if (!m_fontTexture) {
        glGenTextures(1, &m_fontTexture);
        glBindTexture(GL_TEXTURE_2D, m_fontTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        unsigned char dummy[4] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1, 1, 0, GL_RED, GL_UNSIGNED_BYTE, dummy);
    }

    return true;
}

void Renderer::shutdown() {
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_ibo) glDeleteBuffers(1, &m_ibo);
    if (m_shaderProgram) glDeleteProgram(m_shaderProgram);
    if (m_fontTexture) glDeleteTextures(1, &m_fontTexture);

    for (auto& [name, font] : m_fonts) {
        if (font.atlasData) delete[] font.atlasData;
    }
    m_fonts.clear();

    for (auto& [name, tex] : m_textures) {
        if (tex.glId) glDeleteTextures(1, &tex.glId);
    }
    m_textures.clear();

    if (m_fontBuffer) free(m_fontBuffer);

    m_vao = m_vbo = m_ibo = m_shaderProgram = m_fontTexture = 0;
}

void Renderer::setProjection(int width, int height) {
    float mat[16];
    orthoProjection(mat, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);
    glUniformMatrix4fv(m_projLoc, 1, GL_FALSE, mat);
}

void Renderer::setViewport(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;
    glViewport(0, 0, width, height);
}

void Renderer::getViewport(int& width, int& height) const {
    width = m_viewportWidth;
    height = m_viewportHeight;
}

void Renderer::beginFrame(const Color& clearColor) {
    glUseProgram(m_shaderProgram);
    setProjection(m_viewportWidth, m_viewportHeight);
    glBindVertexArray(m_vao);

    m_vertices.clear();
    m_indices.clear();
    m_scissorStack.clear();

    Rect fullRect(0, 0, static_cast<float>(m_viewportWidth), static_cast<float>(m_viewportHeight));
    m_scissorStack.push_back(fullRect);

    int sx = static_cast<int>(fullRect.x);
    int sy = static_cast<int>(m_viewportHeight - fullRect.y - fullRect.height);
    int sw = static_cast<int>(fullRect.width);
    int sh = static_cast<int>(fullRect.height);
    glScissor(sx, sy, sw, sh);

    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::endFrame() {
    flush();

    SDL_GL_SwapWindow(SDL_GL_GetCurrentWindow());
}

void Renderer::flush() {
    if (m_vertices.empty() || m_indices.empty()) return;

    glUseProgram(m_shaderProgram);
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(Vertex), m_vertices.data());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_indices.size() * sizeof(unsigned int), m_indices.data());

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, 0);

    m_vertices.clear();
    m_indices.clear();
}

void Renderer::ensureBatchSpace(int vertexCount, int indexCount) {
    if ((int)m_vertices.size() + vertexCount > 9000 || (int)m_indices.size() + indexCount > 18000) {
        flush();
    }
}

void Renderer::addVertex(const Vertex& v) {
    m_vertices.push_back(v);
}

void Renderer::addQuad(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3) {
    ensureBatchSpace(4, 6);
    unsigned int base = static_cast<unsigned int>(m_vertices.size());
    addVertex(v0); addVertex(v1); addVertex(v2); addVertex(v3);
    m_indices.push_back(base + 0);
    m_indices.push_back(base + 1);
    m_indices.push_back(base + 2);
    m_indices.push_back(base + 0);
    m_indices.push_back(base + 2);
    m_indices.push_back(base + 3);
}

void Renderer::addTriangleIndices(int i0, int i1, int i2) {
    m_indices.push_back(static_cast<unsigned int>(i0));
    m_indices.push_back(static_cast<unsigned int>(i1));
    m_indices.push_back(static_cast<unsigned int>(i2));
}

void Renderer::pushScissor(const Rect& rect) {
    Rect current;
    if (m_scissorStack.empty()) {
        current = Rect(0, 0, static_cast<float>(m_viewportWidth), static_cast<float>(m_viewportHeight));
    } else {
        current = m_scissorStack.back();
    }
    float x1 = std::max(current.x, rect.x);
    float y1 = std::max(current.y, rect.y);
    float x2 = std::min(current.x + current.width, rect.x + rect.width);
    float y2 = std::min(current.y + current.height, rect.y + rect.height);
    Rect clipped(x1, y1, std::max(0.0f, x2 - x1), std::max(0.0f, y2 - y1));
    m_scissorStack.push_back(clipped);

    int sx = static_cast<int>(clipped.x);
    int sy = static_cast<int>(m_viewportHeight - clipped.y - clipped.height);
    int sw = static_cast<int>(clipped.width);
    int sh = static_cast<int>(clipped.height);
    glScissor(sx, sy, sw, sh);

    flush();
}

void Renderer::popScissor() {
    if (m_scissorStack.size() > 1) {
        m_scissorStack.pop_back();
    }
    const Rect& r = m_scissorStack.back();
    int sx = static_cast<int>(r.x);
    int sy = static_cast<int>(m_viewportHeight - r.y - r.height);
    int sw = static_cast<int>(r.width);
    int sh = static_cast<int>(r.height);
    glScissor(sx, sy, sw, sh);

    flush();
}

bool Renderer::isClipped(const Rect& rect) const {
    if (m_scissorStack.empty()) return false;
    const Rect& clip = m_scissorStack.back();
    return !rect.intersects(clip);
}

void Renderer::drawRect(const Rect& rect, const Color& color, float borderRadius) {
    if (borderRadius > 0) {
        drawRoundedRectImpl(rect, color, borderRadius, false, 0);
    } else {
        Vertex v0(rect.x, rect.y, color);
        Vertex v1(rect.x + rect.width, rect.y, color);
        Vertex v2(rect.x + rect.width, rect.y + rect.height, color);
        Vertex v3(rect.x, rect.y + rect.height, color);
        addQuad(v0, v1, v2, v3);
    }
}

void Renderer::drawRectOutline(const Rect& rect, const Color& color, float borderWidth, float borderRadius) {
    if (borderRadius > 0) {
        drawRoundedRectImpl(rect, color, borderRadius, true, borderWidth);
    } else {
        float bw = borderWidth;
        drawRect(Rect(rect.x, rect.y, rect.width, bw), color);
        drawRect(Rect(rect.x, rect.y + rect.height - bw, rect.width, bw), color);
        drawRect(Rect(rect.x, rect.y + bw, bw, rect.height - bw * 2), color);
        drawRect(Rect(rect.x + rect.width - bw, rect.y + bw, bw, rect.height - bw * 2), color);
    }
}

void Renderer::drawRoundedRect(const Rect& rect, const Color& color, float radius) {
    drawRoundedRectImpl(rect, color, radius, false, 0);
}

static void addCornerTriangles(std::vector<Vertex>& verts, std::vector<unsigned int>& indices,
                               float cx, float cy, float radius, float startAngle, float endAngle,
                               const Color& color, int segments) {
    float step = (endAngle - startAngle) / segments;
    unsigned int base = static_cast<unsigned int>(verts.size());
    verts.push_back(Vertex(cx, cy, color));
    for (int i = 0; i <= segments; ++i) {
        float angle = startAngle + step * i;
        float x = cx + std::cos(angle) * radius;
        float y = cy + std::sin(angle) * radius;
        verts.push_back(Vertex(x, y, color));
    }
    for (int i = 0; i < segments; ++i) {
        indices.push_back(base);
        indices.push_back(base + 1 + i);
        indices.push_back(base + 2 + i);
    }
}

void Renderer::drawRoundedRectImpl(const Rect& rect, const Color& color, float radius, bool outline, float borderWidth) {
    radius = std::min(radius, std::min(rect.width, rect.height) * 0.5f);
    if (radius < 0.5f) {
        if (outline) drawRectOutline(rect, color, borderWidth, 0);
        else drawRect(rect, color, 0);
        return;
    }

    int segments = static_cast<int>(radius * 0.5f) + 4;

    float x = rect.x, y = rect.y, w = rect.width, h = rect.height;

    if (!outline) {
        drawRect(Rect(x + radius, y, w - radius * 2, h), color);
        drawRect(Rect(x, y + radius, radius, h - radius * 2), color);
        drawRect(Rect(x + w - radius, y + radius, radius, h - radius * 2), color);

        addCornerTriangles(m_vertices, m_indices,
            x + radius, y + radius, radius,
            M_PI, 1.5f * M_PI, color, segments);
        addCornerTriangles(m_vertices, m_indices,
            x + w - radius, y + radius, radius,
            1.5f * M_PI, 2.0f * M_PI, color, segments);
        addCornerTriangles(m_vertices, m_indices,
            x + w - radius, y + h - radius, radius,
            0, 0.5f * M_PI, color, segments);
        addCornerTriangles(m_vertices, m_indices,
            x + radius, y + h - radius, radius,
            0.5f * M_PI, M_PI, color, segments);
    } else {
        float bw = borderWidth;
        Color c = color;
        drawRect(Rect(x + radius, y, w - radius * 2, bw), c);
        drawRect(Rect(x + radius, y + h - bw, w - radius * 2, bw), c);
        drawRect(Rect(x, y + radius, bw, h - radius * 2), c);
        drawRect(Rect(x + w - bw, y + radius, bw, h - radius * 2), c);

        auto arcVertices = [&](float ccx, float ccy, float r, float startAngle, float endAngle) {
            float step = (endAngle - startAngle) / segments;
            for (int i = 0; i < segments; ++i) {
                float a1 = startAngle + step * i;
                float a2 = startAngle + step * (i + 1);
                float x1 = ccx + std::cos(a1) * r;
                float y1 = ccy + std::sin(a1) * r;
                float x2 = ccx + std::cos(a2) * r;
                float y2 = ccy + std::sin(a2) * r;
                float x3 = ccx + std::cos(a2) * (r - bw);
                float y3 = ccy + std::sin(a2) * (r - bw);
                float x4 = ccx + std::cos(a1) * (r - bw);
                float y4 = ccy + std::sin(a1) * (r - bw);
                unsigned int b = static_cast<unsigned int>(m_vertices.size());
                ensureBatchSpace(4, 6);
                addVertex(Vertex(x1, y1, c));
                addVertex(Vertex(x2, y2, c));
                addVertex(Vertex(x3, y3, c));
                addVertex(Vertex(x4, y4, c));
                m_indices.push_back(b); m_indices.push_back(b+1); m_indices.push_back(b+2);
                m_indices.push_back(b); m_indices.push_back(b+2); m_indices.push_back(b+3);
            }
        };

        arcVertices(x + radius, y + radius, radius, M_PI, 1.5f * M_PI);
        arcVertices(x + w - radius, y + radius, radius, 1.5f * M_PI, 2.0f * M_PI);
        arcVertices(x + w - radius, y + h - radius, radius, 0, 0.5f * M_PI);
        arcVertices(x + radius, y + h - radius, radius, 0.5f * M_PI, M_PI);
    }
}

void Renderer::drawLine(float x1, float y1, float x2, float y2, const Color& color, float width) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.001f) return;

    float nx = -dy / len * width * 0.5f;
    float ny = dx / len * width * 0.5f;

    ensureBatchSpace(4, 6);
    unsigned int base = static_cast<unsigned int>(m_vertices.size());
    addVertex(Vertex(x1 + nx, y1 + ny, color));
    addVertex(Vertex(x2 + nx, y2 + ny, color));
    addVertex(Vertex(x2 - nx, y2 - ny, color));
    addVertex(Vertex(x1 - nx, y1 - ny, color));
    m_indices.push_back(base);
    m_indices.push_back(base + 1);
    m_indices.push_back(base + 2);
    m_indices.push_back(base);
    m_indices.push_back(base + 2);
    m_indices.push_back(base + 3);
}

void Renderer::drawShadow(const Rect& rect, float blur, const Color& color) {
    for (int i = 0; i < 4; ++i) {
        float offset = blur * (i + 1) * 0.25f;
        float alpha = color.a * (1.0f - i / 4.0f) * 0.5f;
        Color c(color.r, color.g, color.b, alpha);
        drawRect(Rect(rect.x - offset, rect.y - offset + blur * 0.3f,
                       rect.width + offset * 2, rect.height + offset * 2), c, blur);
    }
}

void Renderer::drawTriangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color& color) {
    ensureBatchSpace(3, 3);
    unsigned int base = static_cast<unsigned int>(m_vertices.size());
    addVertex(Vertex(p1.x, p1.y, color));
    addVertex(Vertex(p2.x, p2.y, color));
    addVertex(Vertex(p3.x, p3.y, color));
    m_indices.push_back(base);
    m_indices.push_back(base + 1);
    m_indices.push_back(base + 2);
}

void Renderer::clear(const Color& color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::setAlpha(float alpha) {}
float Renderer::getAlpha() const { return 1.0f; }

void Renderer::loadFont(const std::string& name, const std::string& path, float pixelSize) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[GUIEngine] Failed to load font: " << path << std::endl;
        return;
    }

    std::vector<char> fileData((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
    file.close();

    auto* ttfBuffer = new unsigned char[fileData.size()];
    std::memcpy(ttfBuffer, fileData.data(), fileData.size());

    FontData font;
    font.pixelSize = pixelSize;
    font.loaded = true;

    stbtt_fontinfo info;
    stbtt_InitFont(&info, ttfBuffer, stbtt_GetFontOffsetForIndex(ttfBuffer, 0));

    float scale = stbtt_ScaleForPixelHeight(&info, pixelSize);
    int iAscent, iDescent, iLineGap;

    stbtt_GetFontVMetrics(&info, &iAscent, &iDescent, &iLineGap);
    font.ascent = (int)(iAscent * scale);
    font.descent = (int)(iDescent * scale);
    font.lineGap = (int)(iLineGap * scale);

    int atlasSize = 1024;
    font.atlasWidth = atlasSize;
    font.atlasHeight = atlasSize;
    font.atlasData = new unsigned char[atlasSize * atlasSize];
    std::memset(font.atlasData, 0, atlasSize * atlasSize);

    int x = 0, y = 0;
    int rowHeight = 0;

    for (uint32_t c = 32; c < 128; ++c) {
        int advance, lsb, x0, y0, x1, y1;
        stbtt_GetCodepointHMetrics(&info, c, &advance, &lsb);
        stbtt_GetCodepointBitmapBox(&info, c, scale, scale, &x0, &y0, &x1, &y1);

        int w = x1 - x0;
        int h = y1 - y0;

        if (x + w + 2 > atlasSize) {
            x = 0;
            y += rowHeight + 2;
            rowHeight = 0;
        }
        if (h > rowHeight) rowHeight = h;

        stbtt_MakeCodepointBitmap(&info, font.atlasData + y * atlasSize + x,
                                   atlasSize, w, h, atlasSize, scale, c);

        Glyph glyph;
        glyph.charCode = c;
        glyph.width = w;
        glyph.height = h;
        glyph.bearingX = x0;
        glyph.bearingY = -y0;
        glyph.advance = (int)(advance * scale);
        glyph.atlasX = x;
        glyph.atlasY = y;
        glyph.uvX = (float)x / atlasSize;
        glyph.uvY = (float)y / atlasSize;
        glyph.uvW = (float)w / atlasSize;
        glyph.uvH = (float)h / atlasSize;
        font.glyphs[c] = glyph;

        x += w + 2;
    }

    m_fonts[name] = std::move(font);
    m_currentFont = name;
    m_currentFontSize = pixelSize;
    m_baseFontSize = pixelSize;

    if (!m_fontTexture) {
        buildFontAtlasTexture();
    }

    delete[] ttfBuffer;
}

void Renderer::buildFontAtlasTexture() {
    if (m_fontTexture) glDeleteTextures(1, &m_fontTexture);
    glGenTextures(1, &m_fontTexture);
    glBindTexture(GL_TEXTURE_2D, m_fontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (!m_currentFont.empty()) {
        auto& font = m_fonts[m_currentFont];
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font.atlasWidth, font.atlasHeight,
                      0, GL_RED, GL_UNSIGNED_BYTE, font.atlasData);
    } else {
        unsigned char dummy[4] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1, 1, 0, GL_RED, GL_UNSIGNED_BYTE, dummy);
    }
}

void Renderer::useFont(const std::string& name) {
    m_currentFont = name;
    auto it = m_fonts.find(name);
    if (it != m_fonts.end()) {
        m_currentFontSize = it->second.pixelSize * m_fontScale;
        buildFontAtlasTexture();
    }
}

void Renderer::reloadFonts() {
    buildFontAtlasTexture();
}

void Renderer::setFontSize(float size) {
    m_fontScale = m_baseFontSize > 0 ? size / m_baseFontSize : 1.0f;
    m_currentFontSize = size;
}

uint32_t Renderer::decodeUTF8(const std::string& text, size_t& i) {
    uint32_t ch = 0;
    unsigned char c = text[i];
    if (c < 0x80) { ch = c; i += 1; }
    else if ((c & 0xE0) == 0xC0) {
        ch = (c & 0x1F) << 6;
        if (i + 1 < text.size()) ch |= (text[i+1] & 0x3F);
        i += 2;
    } else if ((c & 0xF0) == 0xE0) {
        ch = (c & 0x0F) << 12;
        if (i + 1 < text.size()) ch |= (text[i+1] & 0x3F) << 6;
        if (i + 2 < text.size()) ch |= (text[i+2] & 0x3F);
        i += 3;
    } else if ((c & 0xF8) == 0xF0) {
        ch = (c & 0x07) << 18;
        if (i + 1 < text.size()) ch |= (text[i+1] & 0x3F) << 12;
        if (i + 2 < text.size()) ch |= (text[i+2] & 0x3F) << 6;
        if (i + 3 < text.size()) ch |= (text[i+3] & 0x3F);
        i += 4;
    } else { i += 1; }
    return ch;
}

const Glyph* Renderer::getGlyph(uint32_t codepoint, FontData& font) {
    auto it = font.glyphs.find(codepoint);
    if (it == font.glyphs.end()) {
        it = font.glyphs.find((uint32_t)'?');
        if (it == font.glyphs.end()) return nullptr;
    }
    return &it->second;
}

void Renderer::drawCharQuad(float x, float y, float w, float h, float uvX, float uvY, float uvW, float uvH, const Color& color) {
    ensureBatchSpace(4, 6);
    unsigned int base = static_cast<unsigned int>(m_vertices.size());

    Vertex v0; v0.x = x; v0.y = y; v0.r = color.r; v0.g = color.g; v0.b = color.b; v0.a = color.a; v0.u = uvX; v0.v = uvY; v0.textured = 1;
    Vertex v1; v1.x = x + w; v1.y = y; v1.r = color.r; v1.g = color.g; v1.b = color.b; v1.a = color.a; v1.u = uvX + uvW; v1.v = uvY; v1.textured = 1;
    Vertex v2; v2.x = x + w; v2.y = y + h; v2.r = color.r; v2.g = color.g; v2.b = color.b; v2.a = color.a; v2.u = uvX + uvW; v2.v = uvY + uvH; v2.textured = 1;
    Vertex v3; v3.x = x; v3.y = y + h; v3.r = color.r; v3.g = color.g; v3.b = color.b; v3.a = color.a; v3.u = uvX; v3.v = uvY + uvH; v3.textured = 1;

    addVertex(v0); addVertex(v1); addVertex(v2); addVertex(v3);
    m_indices.push_back(base);
    m_indices.push_back(base + 1);
    m_indices.push_back(base + 2);
    m_indices.push_back(base);
    m_indices.push_back(base + 2);
    m_indices.push_back(base + 3);
}

float Renderer::getTextWidth(const std::string& text) {
    if (m_currentFont.empty()) return 0;
    auto it = m_fonts.find(m_currentFont);
    if (it == m_fonts.end()) return 0;

    float totalWidth = 0;
    size_t i = 0;
    while (i < text.size()) {
        uint32_t ch = decodeUTF8(text, i);
        if (ch == '\n') continue;
        const Glyph* glyph = getGlyph(ch, it->second);
        if (glyph) {
            totalWidth += glyph->advance * m_fontScale;
        }
    }
    return totalWidth;
}

float Renderer::getTextHeight() const {
    if (m_currentFont.empty()) return m_currentFontSize;
    auto it = m_fonts.find(m_currentFont);
    if (it == m_fonts.end()) return m_currentFontSize;

    return (it->second.ascent - it->second.descent) * m_fontScale;
}

void Renderer::drawText(const std::string& text, float x, float y, const Color& color, float maxWidth) {
    if (m_currentFont.empty()) return;
    auto it = m_fonts.find(m_currentFont);
    if (it == m_fonts.end()) return;
    FontData& font = it->second;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fontTexture);
    glUniform1i(m_texLoc, 0);

    float startX = x;
    float startY = y;
    float lineHeight = (font.ascent - font.descent + font.lineGap) * m_fontScale * font.lineSpacing;
    float currentX = x;
    float currentY = y;

    size_t i = 0;
    while (i < text.size()) {
        uint32_t ch = decodeUTF8(text, i);

        if (ch == '\n') {
            currentX = startX;
            currentY += lineHeight;
            continue;
        }

        if (maxWidth > 0 && ch == ' ') {
            float lineEndX = currentX;
            size_t j = i;
            while (j < text.size()) {
                uint32_t c2 = decodeUTF8(text, j);
                if (c2 == ' ' || c2 == '\n') break;
                const Glyph* g = getGlyph(c2, font);
                if (g) lineEndX += g->advance * m_fontScale;
            }
            if (lineEndX > x + maxWidth) {
                currentX = startX;
                currentY += lineHeight;
                continue;
            }
        }

        const Glyph* glyph = getGlyph(ch, font);
        if (!glyph) continue;

        float px = currentX + glyph->bearingX * m_fontScale;
        float py = currentY + glyph->bearingY * m_fontScale;
        float pw = glyph->width * m_fontScale;
        float ph = glyph->height * m_fontScale;

        drawCharQuad(px, py, pw, ph, glyph->uvX, glyph->uvY, glyph->uvW, glyph->uvH, color);

        currentX += glyph->advance * m_fontScale;
    }
}

void Renderer::drawTextCentered(const std::string& text, float x, float y, const Color& color, float maxWidth) {
    if (m_currentFont.empty()) return;
    auto it = m_fonts.find(m_currentFont);
    if (it == m_fonts.end()) return;
    FontData& font = it->second;

    float lineHeight = (font.ascent - font.descent + font.lineGap) * m_fontScale * font.lineSpacing;
    float textWidth = getTextWidth(text);
    float textHeight = lineHeight;

    drawText(text, x - textWidth * 0.5f, y - textHeight * 0.5f + font.ascent * m_fontScale * 0.5f, color, maxWidth);
}

uint32_t Renderer::loadTexture(const std::string& name, const std::string& path) {
    int w, h, channels;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, STBI_rgb_alpha);
    if (!data) {
        std::cerr << "[GUIEngine] Failed to load texture: " << path << std::endl;
        return 0;
    }

    uint32_t id = loadTextureFromData(name, w, h, data);
    stbi_image_free(data);
    return id;
}

uint32_t Renderer::loadTextureFromData(const std::string& name, int w, int h, const unsigned char* data) {
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    TextureData td;
    td.glId = tex;
    td.width = w;
    td.height = h;
    m_textures[name] = td;
    m_nextTextureId++;

    return m_nextTextureId;
}

void Renderer::useTexture(const std::string& name) {
    m_currentTexture = name;
}

void Renderer::drawImage(const std::string& name, const Rect& rect, const Color& tint) {
    auto it = m_textures.find(name);
    if (it == m_textures.end()) return;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, it->second.glId);
    glUniform1i(m_texLoc, 0);

    ensureBatchSpace(4, 6);
    unsigned int base = static_cast<unsigned int>(m_vertices.size());

    Vertex v0; v0.x = rect.x; v0.y = rect.y; v0.r = tint.r; v0.g = tint.g; v0.b = tint.b; v0.a = tint.a; v0.u = 0; v0.v = 0; v0.textured = 1;
    Vertex v1; v1.x = rect.x + rect.width; v1.y = rect.y; v1.r = tint.r; v1.g = tint.g; v1.b = tint.b; v1.a = tint.a; v1.u = 1; v1.v = 0; v1.textured = 1;
    Vertex v2; v2.x = rect.x + rect.width; v2.y = rect.y + rect.height; v2.r = tint.r; v2.g = tint.g; v2.b = tint.b; v2.a = tint.a; v2.u = 1; v2.v = 1; v2.textured = 1;
    Vertex v3; v3.x = rect.x; v3.y = rect.y + rect.height; v3.r = tint.r; v3.g = tint.g; v3.b = tint.b; v3.a = tint.a; v3.u = 0; v3.v = 1; v3.textured = 1;

    addVertex(v0); addVertex(v1); addVertex(v2); addVertex(v3);
    m_indices.push_back(base);
    m_indices.push_back(base + 1);
    m_indices.push_back(base + 2);
    m_indices.push_back(base);
    m_indices.push_back(base + 2);
    m_indices.push_back(base + 3);
}

}
