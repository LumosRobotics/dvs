#include "system_test/two_way_comm/two_way_comm.h"

#include <iostream>

#include "lumos/lumos.h"
#include "utils.h"

void printValues()
{
    const lumos::gui::SliderHandle slider = lumos::gui::getGuiElementHandle<lumos::gui::SliderHandle>("slider0");
    const lumos::gui::ButtonHandle button0 = lumos::gui::getGuiElementHandle<lumos::gui::ButtonHandle>("button0");
    const lumos::gui::ButtonHandle button1 = lumos::gui::getGuiElementHandle<lumos::gui::ButtonHandle>("button1");
    const lumos::gui::CheckboxHandle checkbox = lumos::gui::getGuiElementHandle<lumos::gui::CheckboxHandle>("checkbox0");

    std::cout << "slider value: " << slider.getValue() << std::endl;
    std::cout << "checkbox is checked: " << checkbox.getIsChecked() << std::endl;
}

void changeGuiFromClientApp()
{
    const lumos::gui::ButtonHandle button0 = lumos::gui::getGuiElementHandle<lumos::gui::ButtonHandle>("button0");
    button0.setLabel("New label");
}

void firstTest()
{
    std::cout << "Registering callbacks!" << std::endl;

    lumos::gui::registerGuiCallback("slider0", [](const lumos::gui::SliderHandle& gui_element_handle) -> void {
        std::cout << "Callback function: \"slider0\" value: " << gui_element_handle.getValue() << std::endl;
    });

    lumos::gui::registerGuiCallback("button0", [](const lumos::gui::ButtonHandle& gui_element_handle) -> void {
        std::cout << "Callback function: \"button0\" pressed: " << gui_element_handle.getIsPressed() << std::endl;
    });

    lumos::gui::registerGuiCallback("button1", [](const lumos::gui::ButtonHandle& gui_element_handle) -> void {
        std::cout << "Callback function: \"button1\" pressed: " << gui_element_handle.getIsPressed() << std::endl;
    });

    lumos::gui::registerGuiCallback("checkbox0", [](const lumos::gui::CheckboxHandle& gui_element_handle) -> void {
        std::cout << "Callback function: \"checkbox0\" pressed: " << gui_element_handle.getIsChecked() << std::endl;
    });

    lumos::gui::registerGuiCallback("listbox0", [](const lumos::gui::ListBoxHandle& gui_element_handle) -> void {
        std::cout << "Callback function: \"listbox0\" selected: " << gui_element_handle.getSelectedElement()
                  << std::endl;

        for (const std::string elem : gui_element_handle.getElements())
        {
            std::cout << "elem: " << elem << std::endl;
        }
    });

    lumos::gui::registerGuiCallback("text_entry", [](const lumos::gui::EditableTextHandle& gui_element_handle) -> void {
        if (gui_element_handle.getEnterPressed())
        {
            std::cout << "Callback function: \"text_entry\": Enter pressed with text \"" << gui_element_handle.getText()
                      << "\"" << std::endl;
        }
        else
        {
            std::cout << "Callback function: \"text_entry\": " << gui_element_handle.getText() << std::endl;
        }
    });

    lumos::gui::registerGuiCallback("ddm0", [](const lumos::gui::DropdownMenuHandle& gui_element_handle) -> void {
        std::cout << "Callback function: \"ddm0\" selected: " << gui_element_handle.getSelectedElement() << std::endl;

        for (const std::string elem : gui_element_handle.getElements())
        {
            std::cout << "elem: " << elem << std::endl;
        }
    });

    lumos::gui::registerGuiCallback("rbg0", [](const lumos::gui::RadioButtonGroupHandle& gui_element_handle) -> void {
        std::cout << "Callback function: \"rbg0\" selected: " << std::endl;
        std::cout << "Selected idx: " << gui_element_handle.getSelectedButtonIdx() << std::endl;

        const std::vector<std::string> buttons = gui_element_handle.getButtons();

        for (size_t i = 0; i < buttons.size(); ++i)
        {
            std::cout << "Button: " << buttons[i] << std::endl;
        }
    });

    std::cout << "Starting GUI thread!" << std::endl;
    lumos::gui::startGuiReceiveThread();

    std::cout << "Instructions:" << std::endl
              << "\"q\": Exits application" << std::endl
              << "\"v\" print values" << std::endl
              << "\"c\" change gui" << std::endl;

    // Client application loop
    while (true)
    {
        std::string input;
        std::getline(std::cin, input);

        if (input == "q")
        {
            break;
        }
        else if (input == "v")
        {
            printValues();
        }
        else if (input == "c")
        {
            changeGuiFromClientApp();
        }
    }
}

namespace two_way_comm
{

void addTests()
{
    addTest("cpp", "two_way_comm", "first_test", firstTest);
}

}  // namespace two_way_comm
