# GUIEngine

<p align="center">
  <strong>A modern, retained-mode GUI engine for C++ with SDL2 + OpenGL backend</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg" alt="C++17">
  <img src="https://img.shields.io/badge/SDL2-2.28-green.svg" alt="SDL2">
  <img src="https://img.shields.io/badge/OpenGL-3.3-red.svg" alt="OpenGL 3.3">
  <img src="https://img.shields.io/badge/CMake-3.16+-orange.svg" alt="CMake">
  <img src="https://img.shields.io/badge/License-MIT-yellow.svg" alt="License">
</p>

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Project Structure](#project-structure)
- [Building](#building)
- [Quick Start](#quick-start)
- [Widgets](#widgets)
- [Layout System](#layout-system)
- [Styling & Theming](#styling--theming)
- [Event System](#event-system)
- [Examples](#examples)
- [Dependencies](#dependencies)
- [License](#license)

---

## Overview

**GUIEngine** is a lightweight, modern, retained-mode GUI framework for C++17. It provides a complete widget hierarchy with a flexible layout system (inspired by CSS Flexbox), a powerful event system, and customizable themes — all rendered with OpenGL 3.3 via SDL2.

Unlike immediate-mode GUI libraries (like Dear ImGui), GUIEngine retains a widget tree, making it ideal for building full applications with complex, stateful interfaces such as editors, tools, dashboards, and games.

### Why GUIEngine?

- **Retained widget tree** — Widgets persist across frames, each with their own state
- **Flexbox-style layout** — Familiar layout model with `Row`, `Column`, `grow`, `shrink`, alignment and justification
- **Rich widget set** — Buttons, text fields, sliders, checkboxes, dropdowns, tabs, split views, scrollable areas, windows, and more
- **Theming** — Centralized `Theme` system with per-widget style overrides and state-based styling (hover, pressed, focused, disabled)
- **Event system** — Event dispatch with callbacks (click, hover, focus, key, resize, scroll)
- **OpenGL rendering** — Custom batched renderer with rounded rectangles, shadows, text rendering via stb_truetype, and texture support
- **Header-friendly API** — Clean, intuitive C++ interface with smart pointers and modern C++ features

---

## Features

| Category | Features |
|----------|----------|
| **Widgets** | Button, IconButton, Label, TextField, Checkbox, Slider, ProgressBar, Dropdown, RadioButton, Panel, ScrollView, WindowWidget, TabView, SplitView, Separator, Spacer, Image |
| **Layout** | Flexbox-style (Row/Column), grow/shrink factors, margin/padding, alignment (Start/Center/End/Stretch), justification (Start/Center/End/SpaceBetween/SpaceEvenly/SpaceAround), min/max dimensions |
| **Events** | Mouse press/release/move/enter/leave/scroll, Key press/release, Text input, Focus in/out, Resize, Close |
| **Styling** | Style objects with typed values (Color, float, int, string, bool), state-based styles (Normal/Hovered/Pressed/Focused/Disabled), global Theme singleton |
| **Rendering** | Batched OpenGL 3.3 renderer, rounded rectangles, shadows, outline rendering, line drawing, TTF font rendering via stb_truetype, texture loading via stb_image, scissor clipping |
| **Window** | SDL2 window with OpenGL context, VSync, fullscreen, resizable, cursor management |

---

## Architecture

```
┌─────────────────────────────────────────────────────┐
│                     Application                      │
│  ┌───────────┐  ┌──────────┐  ┌──────────────────┐ │
│  │  Window   │  │ Renderer │  │   Widget Tree    │ │
│  │  (SDL2)   │  │ (OpenGL) │  │  (Retained Mode) │ │
│  └─────┬─────┘  └────┬─────┘  └────────┬─────────┘ │
│        │              │                  │           │
│        │   Events     │   Draw Calls    │ Layout    │
│        └──────────────┴──────────────────┘           │
│                         │                            │
│                   ┌─────┴─────┐                      │
│                   │   Theme   │                      │
│                   │  Style    │                      │
│                   └───────────┘                      │
└─────────────────────────────────────────────────────┘
```

### Core Components

| Component | Description |
|-----------|-------------|
| `Application` | The main loop controller. Manages event dispatch, widget tree, focus, layout invalidation, rendering, and the update cycle. |
| `Window` | Wraps SDL2 window creation, OpenGL context, cursor management, and event polling (translates SDL events to GUIEngine events). |
| `Renderer` | A batched OpenGL 3.3 renderer. Handles all drawing: rectangles, rounded rects, outlines, shadows, triangles, text (via stb_truetype), and textures. |
| `Widget` | The base class for all UI elements. Provides geometry, layout parameters, style, event handling, child management, and the measure/layout/render lifecycle. |
| `Style` & `Theme` | Typed property system for colors, sizes, fonts. Theme provides a global default style. Widgets can override any style property. |
| `Event` & `EventDispatcher` | A flexible event system with typed events and callback-based handlers. |

---

## Project Structure

```
GUIEngine/
├── CMakeLists.txt           # Main build file
├── cmake/
│   └── Dependencies.cmake   # Third-party dependency fetching
├── include/
│   └── GUIEngine/           # Public headers
│       ├── GUIEngine.hpp    # Umbrella header (include everything)
│       ├── Types.hpp         # Color, Vec2, Rect
│       ├── Event.hpp         # Event types and dispatcher
│       ├── Layout.hpp        # Layout params (flexbox-style)
│       ├── Style.hpp         # Style and Theme system
│       ├── Widget.hpp        # Base widget class
│       ├── Window.hpp        # SDL2 window wrapper
│       ├── Renderer.hpp      # OpenGL batched renderer
│       ├── Application.hpp   # Main application loop
│       ├── GUI.hpp           # High-level GUI facade
│       ├── Containers.hpp    # HBox, VBox, StackPanel, Container
│       ├── Label.hpp         # Text label widget
│       ├── Button.hpp        # Button widget
│       ├── IconButton.hpp    # Icon-only button
│       ├── TextField.hpp     # Single-line text input
│       ├── Checkbox.hpp      # Checkbox widget
│       ├── Slider.hpp        # Slider widget
│       ├── ProgressBar.hpp   # Progress bar widget
│       ├── Panel.hpp         # Container panel with background
│       ├── ScrollView.hpp    # Scrollable container
│       ├── Decorations.hpp   # Separator, Spacer, Image
│       ├── WindowWidget.hpp  # Draggable floating window
│       ├── Dropdown.hpp      # Dropdown select + RadioButton
│       ├── TabView.hpp       # Tabbed container
│       └── SplitView.hpp     # Resizable split panel
├── src/                     # Implementation files
│   └── *.cpp
├── examples/
│   ├── CMakeLists.txt
│   └── demo.cpp              # Full demo application
└── assets/
    └── fonts/
```

---

## Building

### Prerequisites

- **C++17 compatible compiler** (GCC 9+, Clang 10+, MSVC 2019+)
- **CMake 3.16+**
- **Git** (for FetchContent dependencies)

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/devkroz/GUIEngine.git
cd GUIEngine

# Create build directory
mkdir build && cd build

# Configure (dependencies are fetched automatically)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . -j$(nproc)

# Run the demo
./examples/guiengine_demo
```

### Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `GUIENGINE_BUILD_EXAMPLES` | `ON` | Build the demo application |
| `GUIENGINE_BUILD_SHARED` | `OFF` | Build GUIEngine as a shared library |

### Platform Notes

- **Linux**: Install SDL2 development packages (`sudo apt install libsdl2-dev`)
- **macOS**: Use Homebrew (`brew install sdl2`)
- **Windows**: Dependencies are fetched automatically via CMake FetchContent

---

## Quick Start

```cpp
#include "GUIEngine/GUIEngine.hpp"
using namespace GUIEngine;

int main() {
    // Initialize the engine
    GUI::instance().init(800, 600, "My App");

    // Build your UI
    auto* root = new VBox();
    root->setPadding(Padding(20));
    root->getLayout().spacing = 12;

    auto* title = root->add<Label>("Hello, GUIEngine!");
    title->setFontSize(24);

    auto* button = root->add<Button>("Click Me");
    button->setPrimary(true);
    button->setOnClickListener([]() {
        printf("Button clicked!\n");
    });

    auto* input = root->add<TextField>();
    input->setPlaceholder("Type something...");

    // Set root and run
    GUI::instance().setRoot(root);
    GUI::instance().run();

    return 0;
}
```

---

## Widgets

### Button

```cpp
auto* button = panel->add<Button>("Save");
button->setPrimary(true);
button->setBorderRadius(8);
button->setOnClickListener([]() { saveFile(); });
```

### Label

```cpp
auto* label = panel->add<Label>("Hello, World!");
label->setFontSize(18);
label->setTextColor(Color(0.2f, 0.5f, 0.95f));
label->setAlign(Label::Alignment::Center);
```

### TextField

```cpp
auto* input = panel->add<TextField>();
input->setPlaceholder("Enter your name...");
input->setPasswordMode(true);
input->setOnChange([](const std::string& text) {
    printf("Input changed: %s\n", text.c_str());
});
input->setOnSubmit([](const std::string& text) {
    printf("Submitted: %s\n", text.c_str());
});
```

### Checkbox

```cpp
auto* checkbox = panel->add<Checkbox>("Enable notifications");
checkbox->setChecked(true);
checkbox->setOnChange([](bool checked) {
    printf("Checkbox: %s\n", checked ? "ON" : "OFF");
});
```

### Slider

```cpp
auto* slider = panel->add<Slider>();
slider->setRange(0, 100);
slider->setValue(50);
slider->setOnChange([](float value) {
    printf("Slider: %.1f\n", value);
});
```

### Dropdown

```cpp
auto* dropdown = panel->add<Dropdown>();
dropdown->addItem("en", "English");
dropdown->addItem("pt", "Portugues");
dropdown->setSelectedIndex(0);
dropdown->setOnChange([](const std::string& value) {
    printf("Selected: %s\n", value.c_str());
});
```

### ProgressBar

```cpp
auto* progress = panel->add<ProgressBar>();
progress->setProgress(65.0f);
progress->setIndeterminate(false);
```

### Panel & Containers

```cpp
auto* panel = root->add<Panel>();
panel->setPadding(Padding(16));
panel->setBorderRadius(12);
panel->setShadow(true);
panel->getLayout().spacing = 8;
panel->getLayout().direction = LayoutDirection::Column;

auto* hbox = panel->add<HBox>();
hbox->getLayout().spacing = 8;
hbox->getLayout().itemAlignment = Alignment::Center;
```

### ScrollView

```cpp
auto* scroll = root->add<ScrollView>();
scroll->setContentSize(0, 2000);
scroll->setStickToBottom(true);
```

### WindowWidget (Floating Window)

```cpp
auto* window = root->add<WindowWidget>("Settings");
window->setSize(400, 300);
window->setMovable(true);
window->setClosable(true);

auto* content = window->getContentPanel();
content->add<Label>("Window content goes here");
```

### TabView

```cpp
auto* tabs = root->add<TabView>();
Tab* tab1 = tabs->addTab("home", "Home", false);
tab1->content = std::make_unique<Panel>();
Tab* tab2 = tabs->addTab("settings", "Settings", true);
tab2->content = std::make_unique<Panel>();
```

### SplitView

```cpp
auto* split = root->add<SplitView>();
split->setOrientation(LayoutDirection::Row);
split->setSplit(0.3f);
split->setResizable(true);

split->add<Panel>();  // Left pane
split->add<Panel>();  // Right pane
```

---

## Layout System

GUIEngine uses a **Flexbox-inspired layout system**. Each widget has `LayoutParams` that control how it participates in its parent's layout.

### Layout Direction

```cpp
// Horizontal layout (children side by side)
container->getLayout().direction = LayoutDirection::Row;

// Vertical layout (children stacked)
container->getLayout().direction = LayoutDirection::Column;
```

### Grow & Shrink

```cpp
widget->getLayout().grow = 1;    // Fill available space
widget->getLayout().shrink = 1;  // Shrink when space is limited
widget->setFixedWidth(200);       // Don't grow or shrink
```

### Alignment

```cpp
// Cross-axis alignment (perpendicular to direction)
container->getLayout().itemAlignment = Alignment::Center;  // | Start | End | Stretch
```

### Justify Content (Main-axis distribution)

```cpp
container->getLayout().justifyContent = Justify::SpaceBetween;
// Options: Start | Center | End | SpaceBetween | SpaceEvenly | SpaceAround
```

### Margin & Padding

```cpp
widget->setMargin(Margin(10));          // All sides
widget->setMargin(Margin(10, 20));      // Vertical, Horizontal
widget->setMargin(Margin(10, 20, 5, 15)); // Top, Right, Bottom, Left
widget->setPadding(Padding(12));
```

### Min/Max Dimensions

```cpp
widget->setMinWidth(100);
widget->setMinHeight(40);
widget->getLayout().maxWidth = 500;
widget->getLayout().maxHeight = 300;
```

---

## Styling & Theming

### Style Properties

Each widget has a `Style` object that holds typed properties:

```cpp
widget->getStyle().set("backgroundColor", Color(0.1f, 0.1f, 0.15f));
widget->getStyle().set("borderColor", Color(0.3f, 0.3f, 0.35f));
widget->getStyle().set("borderRadius", 8.0f);
widget->getStyle().set("fontSize", 14);
widget->getStyle().set("textColor", Color::White());
```

### Themes

The global `Theme` provides default values for all style properties:

```cpp
auto& theme = Theme::defaultTheme();
theme.set("primaryColor", Color(0.2f, 0.5f, 0.95f));
theme.set("backgroundColor", Color(0.12f, 0.12f, 0.14f));
theme.set("textColor", Color(0.88f, 0.88f, 0.92f));
theme.set("buttonColor", Color(0.22f, 0.22f, 0.26f));
theme.set("borderRadius", 6.0f);
theme.set("fontSize", 14);
```

### Built-in Colors

```cpp
Color::Red()        // (1, 0, 0)
Color::Green()      // (0, 1, 0)
Color::Blue()       // (0, 0, 1)
Color::White()      // (1, 1, 1)
Color::Black()      // (0, 0, 0)
Color::Gray()       // (0.5, 0.5, 0.5)
Color::Dark()       // (0.15, 0.15, 0.15)
Color::Light()      // (0.85, 0.85, 0.85)
Color::Transparent()// (0, 0, 0, 0)
```

---

## Event System

GUIEngine dispatches typed events through the widget tree. Widgets handle events via callbacks:

```cpp
// Mouse events
widget->setOnClickListener([]() { /* click */ });
widget->setOnHoverEnterListener([]() { /* mouse enter */ });
widget->setOnHoverLeaveListener([]() { /* mouse leave */ });
widget->setOnPressedListener([]() { /* mouse down */ });
widget->setOnReleasedListener([]() { /* mouse up */ });

// Keyboard events
widget->setOnKeyListener([](const Event& e) -> bool {
    if (e.key == 13) { /* Enter pressed */ }
    return false;
});

// Focus events
widget->setOnFocusChangeListener([](bool focused) {
    printf(focused ? "Focused\n" : "Unfocused\n");
});

// Resize events
widget->setOnResizeListener([](float w, float h) {
    printf("Resized to %.0f x %.0f\n", w, h);
});
```

### Event Types

| Event Type | Description |
|------------|-------------|
| `MouseButtonPress` | Mouse button pressed (left/right) |
| `MouseButtonRelease` | Mouse button released |
| `MouseMove` | Mouse moved over the widget |
| `MouseEnter` | Mouse entered the widget bounds |
| `MouseLeave` | Mouse left the widget bounds |
| `MouseScroll` | Mouse wheel scrolled |
| `KeyPress` | Keyboard key pressed |
| `KeyRelease` | Keyboard key released |
| `TextInput` | Text input (for TextField) |
| `FocusIn` | Widget gained focus |
| `FocusOut` | Widget lost focus |
| `Resize` | Window resized |
| `Close` | Window close requested |

---

## Examples

### Demo Application

The `examples/demo.cpp` file contains a comprehensive demo showcasing all widgets:

```bash
# Build and run
cd build
cmake --build . --target guiengine_demo
./examples/guiengine_demo
```

The demo includes:
- Buttons (primary, secondary, disabled, icon)
- Text fields (normal and password)
- Checkboxes and radio buttons
- Slider with live value display
- Progress bar
- Dropdown selector
- Floating draggable window
- And more!

---

## Dependencies

All dependencies are fetched automatically via CMake's `FetchContent`:

| Dependency | Version | Purpose |
|------------|---------|---------|
| [SDL2](https://github.com/libsdl-org/SDL) | 2.28.0 | Windowing, input, OpenGL context |
| [glad](https://github.com/Dav1dde/glad) | 2.0.6 | OpenGL function loader |
| [stb](https://github.com/nothings/stb) | latest | Image loading (stb_image) and TTF font rendering (stb_truetype) |

> **Note**: If SDL2 is already installed on your system, it will be used instead of being fetched.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

<p align="center">
  Made with care by <a href="https://github.com/devkroz">devkroz</a>
</p>
