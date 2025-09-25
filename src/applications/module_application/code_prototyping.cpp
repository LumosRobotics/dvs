

/*class LoadedDataBase
{
public:
    LoadedDataBase() = default;
};

class LoadedDataDerived : public LoadedDataBase
{
public:
    LoadedDataDerived() = default;
    LoadedDataDerived(const std::string& path_to_file) {}
};*/

std::vector<std::string> items = getDropDownMenuItems("DropDownMenu0");
std::vector<std::string> items = getListBoxItems("listbox0");

setSliderRange("slider0", 0, 100);
setCheckboxState("checkbox0", true);
setEditableText("text0", "Hello World");

setDropDownMenuSelection("DropDownMenu0", 24);
setDropDownMenuSelection("DropDownMenu0", "item_with_this_string");

setListBoxSelectedIndices("DropDownMenu0", {24, 23, 26});
setListBoxSelectedItems("DropDownMenu0",
                        {"item_with_this_string1", "item_with_this_string2", "item_with_this_string3"});

removeListBoxItem("listbox0", 24);
removeDropDownMenuItem("DropDownMenu0", 24);

setDropDownMenuItems("DropDownMenu0", {"Item 1", "Item 2", "Item 3"});
addDropDownMenuItem("DropDownMenu0", {"Item 1", "Item 2", "Item 3"});
setListBoxItems("listbox0", {"Item 1", "Item 2", "Item 3"});
addListBoxItem("listbox0", {"Item 1", "Item 2", "Item 3"});

setRadioButtonState("radiobutton0", true);
setTextLabel("label_handle", "Label Text");

setEnabled("slider0", false);
setEnabled("slider0", true);

// Loading a file and interact with it using a button
void loadFileAndSetupInteraction(const std::string& path_to_file)
{
    //
    // Load the file
    Vector<double> x_data, y_data;

    // Populate the vectors with data

    setActiveView("plot0");
    plot(x_data, y_data);

    setActiveView("button0");
}

/*
Data must be loaded before any callbacks are set up
*/

std::shared_ptr<LoadedDataBase> loadFileNew(const std::string& path_to_file)
{
    std::shared_ptr<LoadedDataBase> loaded_data = std::make_shared<LoadedDataDerived>(path_to_file);

    return loaded_data;
}

std::shared_ptr<LoadedDataBase> loadFileWithNoPersistentData(const std::string& path_to_file)
{
    // Load file data
    // Plot file data

    std::shared_ptr<LoadedDataBase> loaded_data = nullptr;

    return loaded_data;
}

// Loading a simple file and plotting some data with a 2d plot
void loadFile(const std::string& path_to_file)
{
    // Load the file
    Vector<double> x_data, y_data;

    // Populate the vectors with data

    setActiveView("plot0");
    plot(x_data, y_data);
}
