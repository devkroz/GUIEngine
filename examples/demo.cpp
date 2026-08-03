#include "GUIEngine/GUIEngine.hpp"

using namespace GUIEngine;

class DemoApp {
public:
    void run() {
        if (!GUI::instance().init(1280, 720, "GUIEngine - Demo Application")) {
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

        auto* header = root->add<Label>("GUIEngine Demo");
        header->setFontSize(24);
        header->setTextColor(Color(1, 1, 1));

        auto* subtitle = root->add<Label>("A modern C++ GUI engine with SDL2 + OpenGL");
        subtitle->setFontSize(13);

        auto* separator = root->add<Separator>(Separator::Orientation::Horizontal);

        auto* panel = root->add<Panel>();
        panel->setPadding(Padding(20));
        panel->getLayout().grow = 1;
        panel->getLayout().direction = LayoutDirection::Column;
        panel->getLayout().spacing = 12;
        panel->setShadow(true);

        auto* buttonsRow = panel->add<HBox>();
        buttonsRow->getLayout().spacing = 8;

        auto* btn1 = buttonsRow->add<Button>("Primary Button");
        btn1->setPrimary(true);
        int clickCount = 0;
        btn1->setOnClickListener([this, clickCount]() mutable {
            clickCount++;
        });

        auto* btn2 = buttonsRow->add<Button>("Secondary");
        btn2->setOnClickListener([]() {});

        auto* btn3 = buttonsRow->add<Button>("Disabled");
        btn3->setEnabled(false);

        auto* iconBtn = buttonsRow->add<IconButton>("+");
        iconBtn->setBackgroundColor(Color(0.2f, 0.75f, 0.3f, 1.0f));

        auto* inputRow = panel->add<HBox>();
        inputRow->getLayout().spacing = 8;

        auto* nameLabel = inputRow->add<Label>("Name:");
        nameLabel->setFixedWidth(80);

        auto* nameInput = inputRow->add<TextField>();
        nameInput->setPlaceholder("Enter your name...");
        nameInput->getLayout().grow = 1;

        auto* passwordRow = panel->add<HBox>();
        passwordRow->getLayout().spacing = 8;

        auto* passLabel = passwordRow->add<Label>("Password:");
        passLabel->setFixedWidth(80);

        auto* passInput = passwordRow->add<TextField>();
        passInput->setPlaceholder("Enter password...");
        passInput->setPasswordMode(true);
        passInput->getLayout().grow = 1;

        auto* checkboxRow = panel->add<HBox>();
        checkboxRow->getLayout().spacing = 16;

        auto* cb1 = checkboxRow->add<Checkbox>("Remember me");
        cb1->setChecked(true);

        auto* cb2 = checkboxRow->add<Checkbox>("Subscribe to newsletter");
        cb2->setChecked(false);

        auto* radioRow = panel->add<HBox>();
        radioRow->getLayout().spacing = 16;

        auto* rb1 = radioRow->add<RadioButton>("Light theme");
        rb1->setSelected(true);
        rb1->setGroup("theme");

        auto* rb2 = radioRow->add<RadioButton>("Dark theme");
        rb2->setGroup("theme");

        auto* sliderRow = panel->add<HBox>();
        sliderRow->getLayout().spacing = 8;

        auto* sliderLabel = sliderRow->add<Label>("Volume:");
        sliderLabel->setFixedWidth(80);

        auto* slider = sliderRow->add<Slider>();
        slider->setRange(0, 100);
        slider->setValue(75);
        slider->getLayout().grow = 1;

        auto* sliderValueLabel = sliderRow->add<Label>("75%");
        sliderValueLabel->setFixedWidth(50);
        slider->setOnChangeCallback([sliderValueLabel](float value) {
            int v = static_cast<int>(value);
            sliderValueLabel->setText(std::to_string(v) + "%");
        });

        auto* progressRow = panel->add<HBox>();
        progressRow->getLayout().spacing = 8;

        auto* progressLabel = progressRow->add<Label>("Progress:");
        progressLabel->setFixedWidth(80);

        auto* progressBar = progressRow->add<ProgressBar>();
        progressBar->setProgress(65);
        progressBar->getLayout().grow = 1;

        auto* dropdownRow = panel->add<HBox>();
        dropdownRow->getLayout().spacing = 8;

        auto* dropdownLabel = dropdownRow->add<Label>("Language:");
        dropdownLabel->setFixedWidth(80);

        auto* dropdown = dropdownRow->add<Dropdown>();
        dropdown->addItem("en", "English");
        dropdown->addItem("pt", "Portugues");
        dropdown->addItem("es", "Espanol");
        dropdown->addItem("fr", "Francais");
        dropdown->addItem("de", "Deutsch");
        dropdown->setSelectedIndex(0);
        dropdown->getLayout().grow = 1;

        auto* windowWidget = root->add<WindowWidget>("Floating Window");
        windowWidget->setPosition(400, 200);
        windowWidget->setSize(350, 200);
        windowWidget->getLayout().position = PositionType::Absolute;

        auto* winContent = windowWidget->getContentPanel();
        winContent->getLayout().spacing = 8;
        auto* winLabel = winContent->add<Label>("This is a floating, draggable window!");
        winLabel->setFontSize(14);
        auto* winButton = winContent->add<Button>("Close Window");
        winButton->setOnClick([windowWidget]() {
            windowWidget->setVisible(false);
        });

        GUI::instance().setRoot(root);
    }
};

int main(int argc, char* argv[]) {
    DemoApp app;
    app.run();
    return 0;
}
