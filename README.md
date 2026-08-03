# GUIEngine

<p align="center">
  <strong>Uma engine de interface grafica moderna em C++ com SDL2 + OpenGL (retained-mode)</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg" alt="C++17">
  <img src="https://img.shields.io/badge/SDL2-2.28-green.svg" alt="SDL2">
  <img src="https://img.shields.io/badge/OpenGL-3.3-red.svg" alt="OpenGL 3.3">
  <img src="https://img.shields.io/badge/CMake-3.16+-orange.svg" alt="CMake">
  <img src="https://img.shields.io/badge/Licenca-MIT-yellow.svg" alt="Licenca">
</p>

---

## Sumario

- [Visao Geral](#visao-geral)
- [Funcionalidades](#funcionalidades)
- [Arquitetura](#arquitetura)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Compilacao](#compilacao)
- [Primeiros Passos](#primeiros-passos)
- [Widgets](#widgets)
- [Sistema de Layout](#sistema-de-layout)
- [Estilos e Temas](#estilos-e-temas)
- [Sistema de Eventos](#sistema-de-eventos)
- [Exemplos](#exemplos)
- [Dependencias](#dependencias)
- [Licenca](#licenca)

---

## Visao Geral

**GUIEngine** e uma framework de interface grafica leve e moderna para C++17. Ela oferece uma hierarquia completa de widgets com um sistema de layout flexivel (inspirado no CSS Flexbox), um sistema de eventos poderoso e temas customizaveis — tudo renderizado com OpenGL 3.3 via SDL2.

Diferente de bibliotecas de GUI em modo imediato (como Dear ImGui), a GUIEngine mantem uma arvore de widgets (retained-mode), o que a torna ideal para construir aplicacoes completas com interfaces complexas e com estado, como editores, ferramentas, dashboards e jogos.

### Por que usar a GUIEngine?

- **Arvore de widgets retida** — Os widgets persistem entre frames, cada um com seu proprio estado
- **Layout estilo Flexbox** — Modelo de layout familiar com `Row`, `Column`, `grow`, `shrink`, alinhamento e justificacao
- **Conjunto rico de widgets** — Botoes, campos de texto, sliders, checkboxes, dropdowns, abas, split views, areas rolaveis, janelas e muito mais
- **Temas** — Sistema centralizado de `Theme` com sobrescrita de estilo por widget e estilos baseados em estado (hover, pressionado, focado, desabilitado)
- **Sistema de eventos** — Disparo de eventos com callbacks (clique, hover, foco, tecla, redimensionamento, scroll)
- **Renderizacao OpenGL** — Renderer em lote customizado com retangulos arredondados, sombras, renderizacao de texto via stb_truetype e suporte a texturas
- **API limpa** — Interface C++ intuitiva com smart pointers e recursos modernos do C++

---

## Funcionalidades

| Categoria | Funcionalidades |
|-----------|----------------|
| **Widgets** | Button, IconButton, Label, TextField, Checkbox, Slider, ProgressBar, Dropdown, RadioButton, Panel, ScrollView, WindowWidget, TabView, SplitView, Separator, Spacer, Image |
| **Layout** | Estilo Flexbox (Row/Column), fatores grow/shrink, margin/padding, alinhamento (Start/Center/End/Stretch), justificacao (Start/Center/End/SpaceBetween/SpaceEvenly/SpaceAround), dimensoes min/max |
| **Eventos** | Mouse press/release/move/enter/leave/scroll, teclado press/release, entrada de texto, Focus in/out, Resize, Close |
| **Estilos** | Objetos Style com valores tipados (Color, float, int, string, bool), estilos por estado (Normal/Hovered/Pressed/Focused/Disabled), Theme global singleton |
| **Renderizacao** | Renderer OpenGL 3.3 em lote, retangulos arredondados, sombras, contornos, desenho de linhas, renderizacao de fontes TTF via stb_truetype, carregamento de texturas via stb_image, recorte com scissor |
| **Janela** | Janela SDL2 com contexto OpenGL, VSync, fullscreen, redimensionavel, gerenciamento de cursor |

---

## Arquitetura

```
+-----------------------------------------------------+
|                     Application                      |
|  +-----------+  +----------+  +------------------+  |
|  |  Window   |  | Renderer |  |   Widget Tree    |  |
|  |  (SDL2)   |  | (OpenGL) |  |  (Retained Mode) |  |
|  +-----+-----+  +----+-----+  +--------+---------+  |
|        |              |                  |           |
|        |   Eventos    |   Draw Calls     | Layout    |
|        +--------------+------------------+           |
|                         |                            |
|                   +-----+-----+                      |
|                   |   Theme  |                      |
|                   |  Style   |                      |
|                   +-----------+                      |
+-----------------------------------------------------+
```

### Componentes Principais

| Componente | Descricao |
|------------|-----------|
| `Application` | O controlador do loop principal. Gerencia o disparo de eventos, a arvore de widgets, foco, invalidacao de layout, renderizacao e o ciclo de update. |
| `Window` | Encapsula a criacao de janela SDL2, contexto OpenGL, gerenciamento de cursor e.polling de eventos (traduz eventos SDL para eventos da GUIEngine). |
| `Renderer` | Um renderer OpenGL 3.3 em lote. Trata todo o desenho: retangulos, retangulos arredondados, contornos, sombras, triangulos, texto (via stb_truetype) e texturas. |
| `Widget` | A classe base de todos os elementos da UI. Fornece geometria, parametros de layout, estilo, tratamento de eventos, gerenciamento de filhos e o ciclo de vida measure/layout/render. |
| `Style` & `Theme` | Sistema de propriedades tipadas para cores, tamanhos, fontes. O Theme fornece um estilo padrao global. Os widgets podem sobrescrever qualquer propriedade de estilo. |
| `Event` & `EventDispatcher` | Um sistema de eventos flexivel com eventos tipados e manipuladores baseados em callback. |

---

## Estrutura do Projeto

```
GUIEngine/
+-- CMakeLists.txt           # Arquivo principal de build
+-- cmake/
|   +-- Dependencies.cmake   # Busca de dependencias de terceiros
+-- include/
|   +-- GUIEngine/           # Headers publicos
|       +-- GUIEngine.hpp    # Header guarda-chuva (inclui tudo)
|       +-- Types.hpp        # Color, Vec2, Rect
|       +-- Event.hpp        # Tipos de evento e dispatcher
|       +-- Layout.hpp       # Parametros de layout (estilo flexbox)
|       +-- Style.hpp        # Sistema de Style e Theme
|       +-- Widget.hpp       # Classe base de widgets
|       +-- Window.hpp       # Wrapper de janela SDL2
|       +-- Renderer.hpp     # Renderer OpenGL em lote
|       +-- Application.hpp  # Loop principal da aplicacao
|       +-- GUI.hpp          # Facade de alto nivel para a GUI
|       +-- Containers.hpp   # HBox, VBox, StackPanel, Container
|       +-- Label.hpp        # Widget de texto
|       +-- Button.hpp       # Widget de botao
|       +-- IconButton.hpp   # Botao apenas com icone
|       +-- TextField.hpp    # Entrada de texto de uma linha
|       +-- Checkbox.hpp     # Widget de checkbox
|       +-- Slider.hpp       # Widget de slider
|       +-- ProgressBar.hpp  # Barra de progresso
|       +-- Panel.hpp        # Painel conteiner com fundo
|       +-- ScrollView.hpp   # Conteiner rolavel
|       +-- Decorations.hpp  # Separator, Spacer, Image
|       +-- WindowWidget.hpp # Janela flutuante arrastavel
|       +-- Dropdown.hpp     # Select dropdown + RadioButton
|       +-- TabView.hpp      # Conteiner com abas
|       +-- SplitView.hpp    # Painel divisivel redimensionavel
+-- src/                     # Arquivos de implementacao
|   +-- *.cpp
+-- examples/
|   +-- CMakeLists.txt
|   +-- demo.cpp             # Aplicacao demo completa
+-- assets/
    +-- fonts/
```

---

## Compilacao

### Pre-requisitos

- **Compilador compativel com C++17** (GCC 9+, Clang 10+, MSVC 2019+)
- **CMake 3.16+**
- **Git** (para buscar as dependencias via FetchContent)

### Instrucoes de Build

```bash
# Clonar o repositorio
git clone https://github.com/devkroz/GUIEngine.git
cd GUIEngine

# Criar diretorio de build
mkdir build && cd build

# Configurar (as dependencias sao baixadas automaticamente)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Compilar
cmake --build . -j$(nproc)

# Rodar o demo
./examples/guiengine_demo
```

### Opcoes de Build

| Opcao | Padrao | Descricao |
|-------|--------|-----------|
| `GUIENGINE_BUILD_EXAMPLES` | `ON` | Compilar a aplicacao demo |
| `GUIENGINE_BUILD_SHARED` | `OFF` | Compilar a GUIEngine como biblioteca compartilhada |

### Notas por Plataforma

- **Linux**: Instale os pacotes de desenvolvimento do SDL2 (`sudo apt install libsdl2-dev`)
- **macOS**: Use Homebrew (`brew install sdl2`)
- **Windows**: As dependencias sao baixadas automaticamente via CMake FetchContent

---

## Primeiros Passos

```cpp
#include "GUIEngine/GUIEngine.hpp"
using namespace GUIEngine;

int main() {
    // Inicializar a engine
    GUI::instance().init(800, 600, "Minha App");

    // Construir a sua UI
    auto* root = new VBox();
    root->setPadding(Padding(20));
    root->getLayout().spacing = 12;

    auto* title = root->add<Label>("Ola, GUIEngine!");
    title->setFontSize(24);

    auto* button = root->add<Button>("Clica em Mim");
    button->setPrimary(true);
    button->setOnClickListener([]() {
        printf("Botao clicado!\n");
    });

    auto* input = root->add<TextField>();
    input->setPlaceholder("Escreve algo...");

    // Definir a raiz e executar
    GUI::instance().setRoot(root);
    GUI::instance().run();

    return 0;
}
```

---

## Widgets

### Button

```cpp
auto* button = panel->add<Button>("Guardar");
button->setPrimary(true);
button->setBorderRadius(8);
button->setOnClickListener([]() { guardarFicheiro(); });
```

### Label

```cpp
auto* label = panel->add<Label>("Ola, Mundo!");
label->setFontSize(18);
label->setTextColor(Color(0.2f, 0.5f, 0.95f));
label->setAlign(Label::Alignment::Center);
```

### TextField

```cpp
auto* input = panel->add<TextField>();
input->setPlaceholder("Introduz o teu nome...");
input->setPasswordMode(true);
input->setOnChange([](const std::string& text) {
    printf("Input alterado: %s\n", text.c_str());
});
input->setOnSubmit([](const std::string& text) {
    printf("Submetido: %s\n", text.c_str());
});
```

### Checkbox

```cpp
auto* checkbox = panel->add<Checkbox>("Ativar notificacoes");
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
    printf("Selecionado: %s\n", value.c_str());
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

### WindowWidget (Janela Flutuante)

```cpp
auto* window = root->add<WindowWidget>("Definicoes");
window->setSize(400, 300);
window->setMovable(true);
window->setClosable(true);

auto* content = window->getContentPanel();
content->add<Label>("O conteudo da janela fica aqui");
```

### TabView

```cpp
auto* tabs = root->add<TabView>();
Tab* tab1 = tabs->addTab("home", "Inicio", false);
tab1->content = std::make_unique<Panel>();
Tab* tab2 = tabs->addTab("settings", "Definicoes", true);
tab2->content = std::make_unique<Panel>();
```

### SplitView

```cpp
auto* split = root->add<SplitView>();
split->setOrientation(LayoutDirection::Row);
split->setSplit(0.3f);
split->setResizable(true);

split->add<Panel>();  // Painel esquerdo
split->add<Panel>();  // Painel direito
```

---

## Sistema de Layout

A GUIEngine usa um **sistema de layout inspirado no Flexbox**. Cada widget tem `LayoutParams` que controlam como ele participa no layout do seu parent.

### Direcao do Layout

```cpp
// Layout horizontal (filhos lado a lado)
container->getLayout().direction = LayoutDirection::Row;

// Layout vertical (filhos empilhados)
container->getLayout().direction = LayoutDirection::Column;
```

### Grow & Shrink

```cpp
widget->getLayout().grow = 1;    // Preencher o espaco disponivel
widget->getLayout().shrink = 1;  // Encolher quando o espaco e limitado
widget->setFixedWidth(200);       // Nao crescer nem encolher
```

### Alinhamento

```cpp
// Alinhamento no eixo cruzado (perpendicular a direcao)
container->getLayout().itemAlignment = Alignment::Center;  // | Start | End | Stretch
```

### Justify Content (Distribuicao no eixo principal)

```cpp
container->getLayout().justifyContent = Justify::SpaceBetween;
// Opcoes: Start | Center | End | SpaceBetween | SpaceEvenly | SpaceAround
```

### Margin & Padding

```cpp
widget->setMargin(Margin(10));          // Todos os lados
widget->setMargin(Margin(10, 20));      // Vertical, Horizontal
widget->setMargin(Margin(10, 20, 5, 15)); // Topo, Direita, Baixo, Esquerda
widget->setPadding(Padding(12));
```

### Dimensoes Min/Max

```cpp
widget->setMinWidth(100);
widget->setMinHeight(40);
widget->getLayout().maxWidth = 500;
widget->getLayout().maxHeight = 300;
```

---

## Estilos e Temas

### Propriedades de Estilo

Cada widget tem um objeto `Style` que guarda propriedades tipadas:

```cpp
widget->getStyle().set("backgroundColor", Color(0.1f, 0.1f, 0.15f));
widget->getStyle().set("borderColor", Color(0.3f, 0.3f, 0.35f));
widget->getStyle().set("borderRadius", 8.0f);
widget->getStyle().set("fontSize", 14);
widget->getStyle().set("textColor", Color::White());
```

### Temas

O `Theme` global fornece valores padrao para todas as propriedades de estilo:

```cpp
auto& theme = Theme::defaultTheme();
theme.set("primaryColor", Color(0.2f, 0.5f, 0.95f));
theme.set("backgroundColor", Color(0.12f, 0.12f, 0.14f));
theme.set("textColor", Color(0.88f, 0.88f, 0.92f));
theme.set("buttonColor", Color(0.22f, 0.22f, 0.26f));
theme.set("borderRadius", 6.0f);
theme.set("fontSize", 14);
```

### Cores Pre-definidas

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

## Sistema de Eventos

A GUIEngine dispara eventos tipados atraves da arvore de widgets. Os widgets tratam os eventos via callbacks:

```cpp
// Eventos de rato (mouse)
widget->setOnClickListener([]() { /* clique */ });
widget->setOnHoverEnterListener([]() { /* rato entrou */ });
widget->setOnHoverLeaveListener([]() { /* rato saiu */ });
widget->setOnPressedListener([]() { /* rato pressionado */ });
widget->setOnReleasedListener([]() { /* rato largado */ });

// Eventos de teclado
widget->setOnKeyListener([](const Event& e) -> bool {
    if (e.key == 13) { /* Enter pressionado */ }
    return false;
});

// Eventos de foco
widget->setOnFocusChangeListener([](bool focused) {
    printf(focused ? "Focado\n" : "Sem foco\n");
});

// Eventos de redimensionamento
widget->setOnResizeListener([](float w, float h) {
    printf("Redimensionado para %.0f x %.0f\n", w, h);
});
```

### Tipos de Evento

| Tipo de Evento | Descricao |
|----------------|-----------|
| `MouseButtonPress` | Botao do rato pressionado (esquerdo/direito) |
| `MouseButtonRelease` | Botao do rato largado |
| `MouseMove` | Rato movido sobre o widget |
| `MouseEnter` | Rato entrou nos limites do widget |
| `MouseLeave` | Rato saiu dos limites do widget |
| `MouseScroll` | Roda do rato rodada |
| `KeyPress` | Tecla do teclado pressionada |
| `KeyRelease` | Tecla do teclado largada |
| `TextInput` | Entrada de texto (para TextField) |
| `FocusIn` | Widget ganhou foco |
| `FocusOut` | Widget perdeu foco |
| `Resize` | Janela redimensionada |
| `Close` | Fecho da janela solicitado |

---

## Exemplos

### Aplicacao Demo

O ficheiro `examples/demo.cpp` contem um demo abrangente que mostra todos os widgets:

```bash
# Compilar e executar
cd build
cmake --build . --target guiengine_demo
./examples/guiengine_demo
```

O demo inclui:
- Botoes (primario, secundario, desabilitado, icone)
- Campos de texto (normal e password)
- Checkboxes e radio buttons
- Slider com exibicao de valor em tempo real
- Barra de progresso
- Seletor dropdown
- Janela flutuante arrastavel
- E muito mais!

---

## Dependencias

Todas as dependencias sao baixadas automaticamente via `FetchContent` do CMake:

| Dependencia | Versao | Proposito |
|-------------|--------|-----------|
| [SDL2](https://github.com/libsdl-org/SDL) | 2.28.0 | Janela, input, contexto OpenGL |
| [glad](https://github.com/Dav1dde/glad) | 0.1.36 | Carregador de funcoes OpenGL |
| [stb](https://github.com/nothings/stb) | latest | Carregamento de imagens (stb_image) e renderizacao de fontes TTF (stb_truetype) |

> **Nota**: Se o SDL2 ja estiver instalado no seu sistema, sera usado em vez de ser baixado.

---

## Licenca

Este projeto esta licenciado sob a Licenca MIT. Veja o ficheiro [LICENSE](LICENSE) para mais detalhes.

---

<p align="center">
  Feito com cuidado por <a href="https://github.com/devkroz">devkroz</a>
</p>
