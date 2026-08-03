#include "GUIEngine/GUIEngine.hpp"

using namespace GUIEngine;

class DemoApp {
public:
    void run() {
        if (!GUI::instance().init(1280, 720, "GUIEngine - Aplicacao Demo")) {
            return;
        }

        buildUI();

        GUI::instance().run();
    }

private:
    void buildUI() {
        auto* root = new VBox();
        root->setPadding(Padding(20));
        root->setMargin(Margin(0));
        root->getLayout().spacing = 16;

        auto* cabecalho = root->add<Label>("GUIEngine Demo");
        cabecalho->setFontSize(24);
        cabecalho->setTextColor(Color(1.0f, 1.0f, 1.0f));

        auto* subtitulo = root->add<Label>("Uma engine de interface moderna em C++ com SDL2 + OpenGL");
        subtitulo->setFontSize(13);

        auto* separador = root->add<Separator>(Separator::Orientation::Horizontal);

        auto* painel = root->add<Panel>();
        painel->setPadding(Padding(20));
        painel->getLayout().grow = 1;
        painel->getLayout().direction = LayoutDirection::Column;
        painel->getLayout().spacing = 12;
        painel->setShadow(true);

        auto* linhaBotoes = painel->add<HBox>();
        linhaBotoes->getLayout().spacing = 8;

        auto* btn1 = linhaBotoes->add<Button>("Botao Primario");
        btn1->setPrimary(true);
        int contagemCliques = 0;
        btn1->setOnClickListener([this, contagemCliques]() mutable {
            contagemCliques++;
        });

        auto* btn2 = linhaBotoes->add<Button>("Secundario");
        btn2->setOnClickListener([]() {});

        auto* btn3 = linhaBotoes->add<Button>("Desativado");
        btn3->setEnabled(false);

        auto* btnIcone = linhaBotoes->add<IconButton>("+");
        btnIcone->setBackgroundColor(Color(0.2f, 0.75f, 0.3f, 1.0f));

        auto* linhaInput = painel->add<HBox>();
        linhaInput->getLayout().spacing = 8;

        auto* labelNome = linhaInput->add<Label>("Nome:");
        labelNome->setFixedWidth(80);

        auto* inputNome = linhaInput->add<TextField>();
        inputNome->setPlaceholder("Introduz o teu nome...");
        inputNome->getLayout().grow = 1;

        auto* linhaPassword = painel->add<HBox>();
        linhaPassword->getLayout().spacing = 8;

        auto* labelPass = linhaPassword->add<Label>("Senha:");
        labelPass->setFixedWidth(80);

        auto* inputPass = linhaPassword->add<TextField>();
        inputPass->setPlaceholder("Introduz a senha...");
        inputPass->setPasswordMode(true);
        inputPass->getLayout().grow = 1;

        auto* linhaCheckbox = painel->add<HBox>();
        linhaCheckbox->getLayout().spacing = 16;

        auto* cb1 = linhaCheckbox->add<Checkbox>("Lembrar-me");
        cb1->setChecked(true);

        auto* cb2 = linhaCheckbox->add<Checkbox>("Subscrever newsletter");
        cb2->setChecked(false);

        auto* linhaRadio = painel->add<HBox>();
        linhaRadio->getLayout().spacing = 16;

        auto* rb1 = linhaRadio->add<RadioButton>("Tema claro");
        rb1->setSelected(true);
        rb1->setGroup("tema");

        auto* rb2 = linhaRadio->add<RadioButton>("Tema escuro");
        rb2->setGroup("tema");

        auto* linhaSlider = painel->add<HBox>();
        linhaSlider->getLayout().spacing = 8;

        auto* labelSlider = linhaSlider->add<Label>("Volume:");
        labelSlider->setFixedWidth(80);

        auto* slider = linhaSlider->add<Slider>();
        slider->setRange(0, 100);
        slider->setValue(75);
        slider->getLayout().grow = 1;

        auto* labelValorSlider = linhaSlider->add<Label>("75%");
        labelValorSlider->setFixedWidth(50);
        slider->setOnChangeCallback([labelValorSlider](float value) {
            int v = static_cast<int>(value);
            labelValorSlider->setText(std::to_string(v) + "%");
        });

        auto* linhaProgresso = painel->add<HBox>();
        linhaProgresso->getLayout().spacing = 8;

        auto* labelProgresso = linhaProgresso->add<Label>("Progresso:");
        labelProgresso->setFixedWidth(80);

        auto* barraProgresso = linhaProgresso->add<ProgressBar>();
        barraProgresso->setProgress(65);
        barraProgresso->getLayout().grow = 1;

        auto* linhaDropdown = painel->add<HBox>();
        linhaDropdown->getLayout().spacing = 8;

        auto* labelDropdown = linhaDropdown->add<Label>("Idioma:");
        labelDropdown->setFixedWidth(80);

        auto* dropdown = linhaDropdown->add<Dropdown>();
        dropdown->addItem("en", "English");
        dropdown->addItem("pt", "Portugues");
        dropdown->addItem("es", "Espanol");
        dropdown->addItem("fr", "Francais");
        dropdown->addItem("de", "Deutsch");
        dropdown->setSelectedIndex(0);
        dropdown->getLayout().grow = 1;

        auto* janelaFlutuante = root->add<WindowWidget>("Janela Flutuante");
        janelaFlutuante->setPosition(400, 200);
        janelaFlutuante->setSize(350, 200);
        janelaFlutuante->getLayout().position = PositionType::Absolute;

        auto* conteudoJanela = janelaFlutuante->getContentPanel();
        conteudoJanela->getLayout().spacing = 8;
        auto* labelJanela = conteudoJanela->add<Label>("Esta e uma janela flutuante e arrastavel!");
        labelJanela->setFontSize(14);
        auto* btnFecharJanela = conteudoJanela->add<Button>("Fechar Janela");
        btnFecharJanela->setOnClickListener([janelaFlutuante]() {
            janelaFlutuante->setVisible(false);
        });

        GUI::instance().setRoot(root);
    }
};

int main(int argc, char* argv[]) {
    DemoApp app;
    app.run();
    return 0;
}
