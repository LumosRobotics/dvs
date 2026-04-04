#ifndef DUOPLOT_GUI_INTERNAL_H
#define DUOPLOT_GUI_INTERNAL_H

#include "socket_compat.h"

#include "duoplot/constants.h"
#include "duoplot/gui_element_map.h"
#include "duoplot/internal.h"
#include "duoplot/pp.h"

DUOPLOT_WEAK int* duoplot_internal_getTcpSocket()
{
    static int tcp_sockfd;

    return &tcp_sockfd;
}

DUOPLOT_WEAK void duoplot_internal_initTcpSocket()
{
    int* tcp_sockfd = duoplot_internal_getTcpSocket();
    struct sockaddr_in tcp_servaddr;

    *tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Set reuse address that's already in use (probably by exited duoplot instance)
    int true_val = 1;
    setsockopt(*tcp_sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&true_val, sizeof(int));

    bzero(&tcp_servaddr, sizeof(tcp_servaddr));

    tcp_servaddr.sin_family = AF_INET;
    tcp_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    tcp_servaddr.sin_port = htons(DUOPLOT_INTERNAL_GUI_TCP_PORT_NUM);

    int ret_val = bind(*tcp_sockfd, (struct sockaddr*)&tcp_servaddr, sizeof(tcp_servaddr));

    if (ret_val != 0)
    {
        printf("Socket bind failed...\n");
        exit(-1);
    }

    if ((listen(*tcp_sockfd, 5)) != 0)
    {
        printf("Socket listen failed...\n");
        exit(-1);
    }
}

DUOPLOT_WEAK void duoplot_internal_queryForSyncOfGuiData()
{
    duoplot_internal_CommunicationHeader hdr;
    duoplot_internal_initCommunicationHeader(&hdr, DUOPLOT_INTERNAL_F_QUERY_FOR_SYNC_OF_GUI_DATA);

    duoplot_internal_sendHeader(duoplot_internal_getSendFunction(), &hdr);
}

typedef struct S_duoplot_internal_ReceivedGuiData
{
    uint8_t* data;
    uint64_t num_data_bytes;
} duoplot_internal_ReceivedGuiData;

DUOPLOT_WEAK duoplot_internal_ReceivedGuiData duoplot_internal_receiveGuiData()
{
    const int* const tcp_sockfd = duoplot_internal_getTcpSocket();

    struct sockaddr_in tcp_cli;
    socklen_t tcp_len = sizeof(tcp_cli);

    const int tcp_connfd = accept(*tcp_sockfd, (struct sockaddr*)&tcp_cli, &tcp_len);
    if (tcp_connfd < 0)
    {
        printf("Server accept failed...\n");
    }

    uint64_t num_expected_bytes;
    socket_recv(tcp_connfd, &num_expected_bytes, sizeof(uint64_t));

    duoplot_internal_ReceivedGuiData received_data;

    received_data.data = malloc(num_expected_bytes);
    received_data.num_data_bytes = num_expected_bytes;

    char* rec_buffer = (char*)(received_data.data);

    size_t total_num_received_bytes = 0U;
    size_t num_bytes_left = num_expected_bytes;

    while (true)
    {
        const ssize_t num_received_bytes = socket_recv(tcp_connfd, rec_buffer + total_num_received_bytes, num_bytes_left);

        total_num_received_bytes += num_received_bytes;
        num_bytes_left -= (size_t)(num_received_bytes);

        if (total_num_received_bytes >= num_expected_bytes)
        {
            break;
        }
    }

    socket_close(tcp_connfd);

    return received_data;
}

DUOPLOT_WEAK duoplot_internal_GuiElementMap* duoplot_internal_getGuiElementHandles()
{
    static duoplot_internal_GuiElementMap gui_element_handles;

    return &gui_element_handles;
}

DUOPLOT_WEAK duoplot_internal_ButtonCallbackFunctionMap* duoplot_internal_getButtonCallbackFunctionMap()
{
    static duoplot_internal_ButtonCallbackFunctionMap button_callback_function_map;

    return &button_callback_function_map;
}

DUOPLOT_WEAK duoplot_internal_SliderCallbackFunctionMap* duoplot_internal_getSliderCallbackFunctionMap()
{
    static duoplot_internal_SliderCallbackFunctionMap slider_callback_function_map;

    return &slider_callback_function_map;
}

DUOPLOT_WEAK duoplot_internal_ListBoxCallbackFunctionMap* duoplot_internal_getListBoxCallbackFunctionMap()
{
    static duoplot_internal_ListBoxCallbackFunctionMap list_box_handle_callback_function_map;

    return &list_box_handle_callback_function_map;
}

DUOPLOT_WEAK duoplot_internal_DropdownMenuCallbackFunctionMap* duoplot_internal_getDropdownMenuCallbackFunctionMap()
{
    static duoplot_internal_DropdownMenuCallbackFunctionMap dropdown_menu_handle_callback_function_map;

    return &dropdown_menu_handle_callback_function_map;
}

DUOPLOT_WEAK duoplot_internal_CheckboxCallbackFunctionMap* duoplot_internal_getCheckboxCallbackFunctionMap()
{
    static duoplot_internal_CheckboxCallbackFunctionMap checkbox_handle_callback_function_map;

    return &checkbox_handle_callback_function_map;
}

DUOPLOT_WEAK duoplot_internal_RadioButtonGroupCallbackFunctionMap*
duoplot_internal_getRadioButtonGroupCallbackFunctionMap()
{
    static duoplot_internal_RadioButtonGroupCallbackFunctionMap radiobutton_group_handle_callback_function_map;

    return &radiobutton_group_handle_callback_function_map;
}

DUOPLOT_WEAK duoplot_internal_EditableTextCallbackFunctionMap* duoplot_internal_getEditableTextCallbackFunctionMap()
{
    static duoplot_internal_EditableTextCallbackFunctionMap editable_text_handle_callback_function_map;

    return &editable_text_handle_callback_function_map;
}

DUOPLOT_WEAK void duoplot_internal_initDataStructures(const size_t initial_size)
{
    duoplot_internal_GuiElementMap* const gui_element_handles = duoplot_internal_getGuiElementHandles();
    duoplot_internal_initGuiElementHandleContainerMap(gui_element_handles, initial_size);

    duoplot_internal_ButtonCallbackFunctionMap* const button_callback_function_map =
        duoplot_internal_getButtonCallbackFunctionMap();
    duoplot_internal_initButtonCallbackFunctionMap(button_callback_function_map, initial_size);

    duoplot_internal_SliderCallbackFunctionMap* const slider_callback_function_map =
        duoplot_internal_getSliderCallbackFunctionMap();
    duoplot_internal_initSliderCallbackFunctionMap(slider_callback_function_map, initial_size);

    duoplot_internal_ListBoxCallbackFunctionMap* const list_box_handle_callback_function_map =
        duoplot_internal_getListBoxCallbackFunctionMap();
    duoplot_internal_initListBoxCallbackFunctionMap(list_box_handle_callback_function_map, initial_size);

    duoplot_internal_DropdownMenuCallbackFunctionMap* const dropdown_menu_handle_callback_function_map =
        duoplot_internal_getDropdownMenuCallbackFunctionMap();
    duoplot_internal_initDropdownMenuCallbackFunctionMap(dropdown_menu_handle_callback_function_map, initial_size);

    duoplot_internal_CheckboxCallbackFunctionMap* const checkbox_handle_callback_function_map =
        duoplot_internal_getCheckboxCallbackFunctionMap();
    duoplot_internal_initCheckboxCallbackFunctionMap(checkbox_handle_callback_function_map, initial_size);

    duoplot_internal_RadioButtonGroupCallbackFunctionMap* const radiobutton_group_handle_callback_function_map =
        duoplot_internal_getRadioButtonGroupCallbackFunctionMap();
    duoplot_internal_initRadioButtonGroupCallbackFunctionMap(radiobutton_group_handle_callback_function_map,
                                                             initial_size);

    duoplot_internal_EditableTextCallbackFunctionMap* const editable_text_handle_callback_function_map =
        duoplot_internal_getEditableTextCallbackFunctionMap();
    duoplot_internal_initEditableTextCallbackFunctionMap(editable_text_handle_callback_function_map, initial_size);
}

DUOPLOT_WEAK void duoplot_internal_updateButtonState(duoplot_internal_ButtonInternalHandle* const handle,
                                                     const duoplot_internal_UInt8Array* data_view)
{
    handle->is_pressed = data_view->data[0];
}

DUOPLOT_WEAK void duoplot_internal_updateSliderState(duoplot_internal_SliderInternalHandle* const handle,
                                                     const duoplot_internal_UInt8Array* data_view)
{
    memcpy(&handle->state.min_value, data_view->data, sizeof(int32_t));
    memcpy(&handle->state.max_value, data_view->data + sizeof(int32_t), sizeof(int32_t));
    memcpy(&handle->state.step_size, data_view->data + 2U * sizeof(int32_t), sizeof(int32_t));
    memcpy(&handle->state.value, data_view->data + 3U * sizeof(int32_t), sizeof(int32_t));
}

DUOPLOT_WEAK void duoplot_internal_updateTextLabelState(duoplot_internal_TextLabelInternalHandle* const handle,
                                                        const duoplot_internal_UInt8Array* data_view)
{
    const uint8_t text_length = data_view->data[0];

    handle->text = (char*)malloc(text_length + 1U);

    size_t idx = 1U;

    memcpy(handle->text, data_view->data + idx, text_length);
    handle->text[text_length] = '\0';
}

DUOPLOT_WEAK void duoplot_internal_updateListBoxState(duoplot_internal_ListBoxInternalHandle* const handle,
                                                      const duoplot_internal_UInt8Array* data_view)
{
    duoplot_ListBoxState* const state = &(handle->state);
    duoplot_destroyListOfStrings(&(state->elements));
    free(state->selected_element);

    const uint8_t selected_element_length = data_view->data[0];

    size_t idx = 1U;

    if (selected_element_length > 0U)
    {
        state->selected_element = (char*)malloc(selected_element_length + 1U);
        memcpy(state->selected_element, data_view->data + idx, selected_element_length);
        state->selected_element[selected_element_length] = '\0';
        idx += selected_element_length;
    }
    else
    {
        state->selected_element = (char*)malloc(1U);
        state->selected_element[0] = '\0';
    }

    uint16_t num_elements;

    memcpy(&num_elements, data_view->data + idx, sizeof(uint16_t));

    idx += sizeof(uint16_t);

    state->elements = duoplot_createListOfStrings(num_elements);

    for (uint16_t i = 0U; i < num_elements; i++)
    {
        const uint8_t element_length = data_view->data[idx];
        idx++;

        state->elements.elements[i] = (char*)malloc(element_length + 1U);
        memcpy(state->elements.elements[i], data_view->data + idx, element_length);
        state->elements.elements[i][element_length] = '\0';

        idx += element_length;
    }
}

DUOPLOT_WEAK void duoplot_internal_updateDropdownMenuState(duoplot_internal_DropdownMenuInternalHandle* const handle,
                                                           const duoplot_internal_UInt8Array* data_view)
{
    duoplot_DropdownMenuState* const state = &(handle->state);
    duoplot_destroyListOfStrings(&(state->elements));
    free(state->selected_element);

    const uint8_t selected_element_length = data_view->data[0];

    size_t idx = 1U;

    if (selected_element_length > 0U)
    {
        state->selected_element = (char*)malloc(selected_element_length + 1U);
        memcpy(state->selected_element, data_view->data + idx, selected_element_length);
        state->selected_element[selected_element_length] = '\0';
        idx += selected_element_length;
    }
    else
    {
        state->selected_element = (char*)malloc(1U);
        state->selected_element[0] = '\0';
    }

    uint16_t num_elements;

    memcpy(&num_elements, data_view->data + idx, sizeof(uint16_t));

    idx += sizeof(uint16_t);

    state->elements = duoplot_createListOfStrings(num_elements);

    for (uint16_t i = 0U; i < num_elements; i++)
    {
        const uint8_t element_length = data_view->data[idx];
        idx++;

        state->elements.elements[i] = (char*)malloc(element_length + 1U);
        memcpy(state->elements.elements[i], data_view->data + idx, element_length);
        state->elements.elements[i][element_length] = '\0';

        idx += element_length;
    }
}

DUOPLOT_WEAK void duoplot_internal_updateCheckboxState(duoplot_internal_CheckboxInternalHandle* const handle,
                                                       const duoplot_internal_UInt8Array* data_view)
{
    handle->is_checked = data_view->data[0];
}

DUOPLOT_WEAK void duoplot_internal_updateRadioButtonGroupState(RadioButtonGroupInternalHandle* const handle,
                                                               const duoplot_internal_UInt8Array* data_view)
{
    size_t idx = 0U;

    memcpy(&handle->selected_button_idx, data_view->data + idx, sizeof(int32_t));
    idx += sizeof(int32_t);

    uint16_t num_buttons;

    memcpy(&num_buttons, data_view->data + idx, sizeof(uint16_t));
    idx += sizeof(uint16_t);

    handle->buttons = duoplot_createListOfStrings(num_buttons);

    for (uint16_t k = 0U; k < num_buttons; k++)
    {
        const uint8_t button_length = data_view->data[idx];
        idx += sizeof(uint8_t);

        handle->buttons.elements[k] = (char*)malloc(button_length + 1U);
        memcpy(handle->buttons.elements[k], data_view->data + idx, button_length);
        handle->buttons.elements[k][button_length] = '\0';

        idx += button_length;
    }
}

DUOPLOT_WEAK void duoplot_internal_updateEditableTextState(duoplot_internal_EditableTextInternalHandle* const handle,
                                                           const duoplot_internal_UInt8Array* data_view)
{
    const uint8_t* const data_ptr = data_view->data;
    handle->enter_pressed = data_ptr[0];

    const uint8_t text_length = data_ptr[1];

    handle->text = (char*)malloc(text_length + 1U);

    size_t idx = 2U;

    memcpy(handle->text, data_ptr + idx, text_length);
}

DUOPLOT_WEAK duoplot_internal_BaseHandle* duoplot_internal_createButton(
    const char* const handle_string, const duoplot_internal_UInt8Array* const data_view)
{
    duoplot_internal_ButtonInternalHandle* const button =
        (duoplot_internal_ButtonInternalHandle*)malloc(sizeof(duoplot_internal_ButtonInternalHandle));

    size_t handle_string_length = strlen(handle_string);

    button->handle_string = (char*)malloc(handle_string_length + 1U);
    strcpy(button->handle_string, handle_string);
    button->handle_string[handle_string_length] = '\0';  // Null-terminate string

    button->type = DUOPLOT_INTERNAL_GUI_ET_BUTTON;

    duoplot_internal_updateButtonState(button, data_view);

    duoplot_internal_BaseHandle* const handle = (duoplot_internal_BaseHandle*)button;

    return handle;
}

DUOPLOT_WEAK duoplot_internal_BaseHandle* duoplot_internal_createSlider(
    const char* const handle_string, const duoplot_internal_UInt8Array* const data_view)
{
    duoplot_internal_SliderInternalHandle* const slider =
        (duoplot_internal_SliderInternalHandle*)malloc(sizeof(duoplot_internal_SliderInternalHandle));

    size_t handle_string_length = strlen(handle_string);

    slider->handle_string = (char*)malloc(handle_string_length + 1U);
    strcpy(slider->handle_string, handle_string);
    slider->handle_string[handle_string_length] = '\0';  // Null-terminate string

    slider->type = DUOPLOT_INTERNAL_GUI_ET_SLIDER;

    duoplot_internal_updateSliderState(slider, data_view);

    duoplot_internal_BaseHandle* const handle = (duoplot_internal_BaseHandle*)slider;

    return handle;
}

DUOPLOT_WEAK duoplot_internal_BaseHandle* duoplot_internal_createListBox(
    const char* const handle_string, const duoplot_internal_UInt8Array* const data_view)
{
    duoplot_internal_ListBoxInternalHandle* const list_box =
        (duoplot_internal_ListBoxInternalHandle*)malloc(sizeof(duoplot_internal_ListBoxInternalHandle));

    size_t handle_string_length = strlen(handle_string);

    list_box->handle_string = (char*)malloc(handle_string_length + 1U);
    strcpy(list_box->handle_string, handle_string);
    list_box->handle_string[handle_string_length] = '\0';  // Null-terminate string

    list_box->type = DUOPLOT_INTERNAL_GUI_ET_LIST_BOX;

    duoplot_internal_BaseHandle* const handle = (duoplot_internal_BaseHandle*)list_box;

    list_box->state.elements = duoplot_createListOfStrings(10U);
    list_box->state.selected_element = (char*)malloc(1U);
    list_box->state.selected_element[0] = '\0';

    duoplot_internal_updateListBoxState((duoplot_internal_ListBoxInternalHandle*)handle, data_view);

    return handle;
}

DUOPLOT_WEAK duoplot_internal_BaseHandle* duoplot_internal_createDropdownMenu(
    const char* const handle_string, const duoplot_internal_UInt8Array* const data_view)
{
    duoplot_internal_DropdownMenuInternalHandle* const dropdown_menu =
        (duoplot_internal_DropdownMenuInternalHandle*)malloc(sizeof(duoplot_internal_DropdownMenuInternalHandle));

    size_t handle_string_length = strlen(handle_string);

    dropdown_menu->handle_string = (char*)malloc(handle_string_length + 1U);
    strcpy(dropdown_menu->handle_string, handle_string);
    dropdown_menu->handle_string[handle_string_length] = '\0';  // Null-terminate string

    dropdown_menu->type = DUOPLOT_INTERNAL_GUI_ET_DROPDOWN_MENU;

    duoplot_internal_BaseHandle* const handle = (duoplot_internal_BaseHandle*)dropdown_menu;

    dropdown_menu->state.elements = duoplot_createListOfStrings(10U);
    dropdown_menu->state.selected_element = (char*)malloc(1U);
    dropdown_menu->state.selected_element[0] = '\0';

    duoplot_internal_updateDropdownMenuState((duoplot_internal_DropdownMenuInternalHandle*)handle, data_view);

    return handle;
}

DUOPLOT_WEAK duoplot_internal_BaseHandle* duoplot_internal_createCheckbox(
    const char* const handle_string, const duoplot_internal_UInt8Array* const data_view)
{
    duoplot_internal_CheckboxInternalHandle* const checkbox =
        (duoplot_internal_CheckboxInternalHandle*)malloc(sizeof(duoplot_internal_CheckboxInternalHandle));

    size_t handle_string_length = strlen(handle_string);

    checkbox->handle_string = (char*)malloc(handle_string_length + 1U);
    strcpy(checkbox->handle_string, handle_string);
    checkbox->handle_string[handle_string_length] = '\0';  // Null-terminate string

    checkbox->type = DUOPLOT_INTERNAL_GUI_ET_CHECKBOX;

    duoplot_internal_BaseHandle* const handle = (duoplot_internal_BaseHandle*)checkbox;

    duoplot_internal_updateCheckboxState((duoplot_internal_CheckboxInternalHandle*)handle, data_view);

    return handle;
}

DUOPLOT_WEAK duoplot_internal_BaseHandle* duoplot_internal_createRadioButtonGroup(
    const char* const handle_string, const duoplot_internal_UInt8Array* const data_view)
{
    RadioButtonGroupInternalHandle* const radiobutton_group =
        (RadioButtonGroupInternalHandle*)malloc(sizeof(RadioButtonGroupInternalHandle));

    size_t handle_string_length = strlen(handle_string);

    radiobutton_group->handle_string = (char*)malloc(handle_string_length + 1U);
    strcpy(radiobutton_group->handle_string, handle_string);
    radiobutton_group->handle_string[handle_string_length] = '\0';  // Null-terminate string

    radiobutton_group->type = DUOPLOT_INTERNAL_GUI_ET_RADIO_BUTTON_GROUP;

    duoplot_internal_BaseHandle* const handle = (duoplot_internal_BaseHandle*)radiobutton_group;

    duoplot_internal_updateRadioButtonGroupState((RadioButtonGroupInternalHandle*)handle, data_view);

    return handle;
}

DUOPLOT_WEAK duoplot_internal_BaseHandle* duoplot_internal_createEditableText(
    const char* const handle_string, const duoplot_internal_UInt8Array* const data_view)
{
    duoplot_internal_EditableTextInternalHandle* const editable_text =
        (duoplot_internal_EditableTextInternalHandle*)malloc(sizeof(duoplot_internal_EditableTextInternalHandle));

    size_t handle_string_length = strlen(handle_string);

    editable_text->handle_string = (char*)malloc(handle_string_length + 1U);
    strcpy(editable_text->handle_string, handle_string);
    editable_text->handle_string[handle_string_length] = '\0';  // Null-terminate string

    editable_text->type = DUOPLOT_INTERNAL_GUI_ET_EDITABLE_TEXT;

    duoplot_internal_BaseHandle* const handle = (duoplot_internal_BaseHandle*)editable_text;

    duoplot_internal_updateEditableTextState((duoplot_internal_EditableTextInternalHandle*)handle, data_view);

    return handle;
}

DUOPLOT_WEAK duoplot_internal_BaseHandle* duoplot_internal_createTextLabel(
    const char* const handle_string, const duoplot_internal_UInt8Array* const data_view)
{
    duoplot_internal_TextLabelInternalHandle* const text_label =
        (duoplot_internal_TextLabelInternalHandle*)malloc(sizeof(duoplot_internal_TextLabelInternalHandle));

    size_t handle_string_length = strlen(handle_string);

    text_label->handle_string = (char*)malloc(handle_string_length + 1U);
    strcpy(text_label->handle_string, handle_string);
    text_label->handle_string[handle_string_length] = '\0';  // Null-terminate string

    text_label->type = DUOPLOT_INTERNAL_GUI_ET_TEXT_LABEL;

    duoplot_internal_BaseHandle* const handle = (duoplot_internal_BaseHandle*)text_label;

    duoplot_internal_updateTextLabelState((duoplot_internal_TextLabelInternalHandle*)handle, data_view);

    return handle;
}

DUOPLOT_WEAK void duoplot_internal_populateGuiElementWithData(const duoplot_internal_GuiElementType type,
                                                              const char* const handle_string,
                                                              const duoplot_internal_UInt8Array* const data_view)
{
    duoplot_internal_GuiElementMap* gui_element_handles = duoplot_internal_getGuiElementHandles();

    if (duoplot_internal_isGuiElementHandleContainerKeyInMap(handle_string, gui_element_handles))
    {
        // Key is already in map, update state
        duoplot_internal_BaseHandle* const handle =
            (duoplot_internal_BaseHandle*)duoplot_internal_getGuiElementHandleContainer(handle_string,
                                                                                        gui_element_handles);
        if (handle != NULL)
        {
            if (handle->type != type)
            {
                printf("Gui element with handle string %s already exists, but has different type!\n", handle_string);
                return;
            }

            if (type == DUOPLOT_INTERNAL_GUI_ET_BUTTON)
            {
                duoplot_internal_updateButtonState((duoplot_internal_ButtonInternalHandle*)handle, data_view);
            }
            else if (type == DUOPLOT_INTERNAL_GUI_ET_CHECKBOX)
            {
                duoplot_internal_updateCheckboxState((duoplot_internal_CheckboxInternalHandle*)handle, data_view);
            }
            else if (type == DUOPLOT_INTERNAL_GUI_ET_SLIDER)
            {
                duoplot_internal_updateSliderState((duoplot_internal_SliderInternalHandle*)handle, data_view);
            }
            else if (type == DUOPLOT_INTERNAL_GUI_ET_TEXT_LABEL)
            {
                duoplot_internal_updateTextLabelState((duoplot_internal_TextLabelInternalHandle*)handle, data_view);
            }
            else if (type == DUOPLOT_INTERNAL_GUI_ET_LIST_BOX)
            {
                duoplot_internal_updateListBoxState((duoplot_internal_ListBoxInternalHandle*)handle, data_view);
            }
            else if (type == DUOPLOT_INTERNAL_GUI_ET_EDITABLE_TEXT)
            {
                duoplot_internal_updateEditableTextState((duoplot_internal_EditableTextInternalHandle*)handle,
                                                         data_view);
            }
            else if (type == DUOPLOT_INTERNAL_GUI_ET_DROPDOWN_MENU)
            {
                duoplot_internal_updateDropdownMenuState((duoplot_internal_DropdownMenuInternalHandle*)handle,
                                                         data_view);
            }
            else if (type == DUOPLOT_INTERNAL_GUI_ET_RADIO_BUTTON_GROUP)
            {
                duoplot_internal_updateRadioButtonGroupState((RadioButtonGroupInternalHandle*)handle, data_view);
            }
        }
    }
    else
    {
        duoplot_internal_BaseHandle* handle = NULL;
        // Key is not in map, create new element
        if (type == DUOPLOT_INTERNAL_GUI_ET_BUTTON)
        {
            handle = (duoplot_internal_BaseHandle*)duoplot_internal_createButton(handle_string, data_view);
        }
        else if (type == DUOPLOT_INTERNAL_GUI_ET_SLIDER)
        {
            handle = (duoplot_internal_BaseHandle*)duoplot_internal_createSlider(handle_string, data_view);
        }
        else if (type == DUOPLOT_INTERNAL_GUI_ET_LIST_BOX)
        {
            handle = (duoplot_internal_BaseHandle*)duoplot_internal_createListBox(handle_string, data_view);
        }
        else if (type == DUOPLOT_INTERNAL_GUI_ET_DROPDOWN_MENU)
        {
            handle = (duoplot_internal_BaseHandle*)duoplot_internal_createDropdownMenu(handle_string, data_view);
        }
        else if (type == DUOPLOT_INTERNAL_GUI_ET_CHECKBOX)
        {
            handle = (duoplot_internal_BaseHandle*)duoplot_internal_createCheckbox(handle_string, data_view);
        }
        else if (type == DUOPLOT_INTERNAL_GUI_ET_RADIO_BUTTON_GROUP)
        {
            handle = (duoplot_internal_BaseHandle*)duoplot_internal_createRadioButtonGroup(handle_string, data_view);
        }
        else if (type == DUOPLOT_INTERNAL_GUI_ET_EDITABLE_TEXT)
        {
            handle = (duoplot_internal_BaseHandle*)duoplot_internal_createEditableText(handle_string, data_view);
        }
        else if (type == DUOPLOT_INTERNAL_GUI_ET_TEXT_LABEL)
        {
            handle = (duoplot_internal_BaseHandle*)duoplot_internal_createTextLabel(handle_string, data_view);
        }

        if (handle != NULL)
        {
            duoplot_internal_insertElementIntoGuiElementHandleContainerMap(gui_element_handles, handle_string, handle);
        }
    }
}

DUOPLOT_WEAK void duoplot_internal_updateGuiState(const duoplot_internal_ReceivedGuiData* received_gui_data)
{
    size_t idx = 0U;

    uint8_t* const raw_data = received_gui_data->data;

    // Receive[1]: Gui element type (uint8_t)
    const duoplot_internal_GuiElementType type = (duoplot_internal_GuiElementType)(raw_data[idx]);
    idx += sizeof(uint8_t);

    // Receive[2]: Handle string length (uint8_t)
    const uint8_t handle_string_length = (uint8_t)(raw_data[idx]);
    idx += sizeof(uint8_t);

    char* const handle_string = (char*)malloc(handle_string_length + 1U);

    // Receive[3]: Handle string (variable)
    for (uint8_t i = 0; i < handle_string_length; i++)
    {
        handle_string[i] = raw_data[idx];
        idx++;
    }
    handle_string[handle_string_length] = '\0';  // Null-terminate string

    uint32_t size_of_current_gui_element;

    // Receive[4]: Current gui element payload size (uint32_t)
    memcpy(&size_of_current_gui_element, raw_data + idx, sizeof(uint32_t));
    idx += sizeof(uint32_t);

    const duoplot_internal_UInt8Array gui_element_data = {raw_data + idx, size_of_current_gui_element};

    // Receive[5]: Gui element data (variable)
    duoplot_internal_populateGuiElementWithData(type, handle_string, &gui_element_data);

    free(handle_string);
}

DUOPLOT_WEAK duoplot_ButtonHandle duoplot_getButtonHandle(const char* const handle_string)
{
    duoplot_internal_GuiElementMap* const gui_element_map = duoplot_internal_getGuiElementHandles();

    duoplot_ButtonHandle button_handle;
    button_handle.__handle = NULL;

    if (!duoplot_internal_isGuiElementHandleContainerKeyInMap(handle_string, gui_element_map))
    {
        printf("Gui element with handle string %s does not exist!\n", handle_string);
        return button_handle;
    }

    duoplot_internal_ButtonInternalHandle* const gui_elem =
        (duoplot_internal_ButtonInternalHandle*)duoplot_internal_getGuiElementHandleContainer(handle_string,
                                                                                              gui_element_map);

    if (gui_elem->type != DUOPLOT_INTERNAL_GUI_ET_BUTTON)
    {
        printf("Gui element with handle string %s is not a button!\n", handle_string);
        return button_handle;
    }

    button_handle.__handle = gui_elem;

    return button_handle;
}

DUOPLOT_WEAK duoplot_SliderHandle duoplot_getSliderHandle(const char* const handle_string)
{
    duoplot_internal_GuiElementMap* const gui_element_map = duoplot_internal_getGuiElementHandles();

    duoplot_SliderHandle slider_handle;
    slider_handle.__handle = NULL;

    if (!duoplot_internal_isGuiElementHandleContainerKeyInMap(handle_string, gui_element_map))
    {
        printf("Gui element with handle string %s does not exist!\n", handle_string);
        return slider_handle;
    }

    duoplot_internal_SliderInternalHandle* const gui_elem =
        (duoplot_internal_SliderInternalHandle*)duoplot_internal_getGuiElementHandleContainer(handle_string,
                                                                                              gui_element_map);

    if (gui_elem->type != DUOPLOT_INTERNAL_GUI_ET_SLIDER)
    {
        printf("Gui element with handle string %s is not a slider!\n", handle_string);
        return slider_handle;
    }

    slider_handle.__handle = gui_elem;

    return slider_handle;
}

DUOPLOT_WEAK duoplot_ListBoxHandle duoplot_getListBoxHandle(const char* const handle_string)
{
    duoplot_internal_GuiElementMap* const gui_element_map = duoplot_internal_getGuiElementHandles();

    duoplot_ListBoxHandle list_box_handle;
    list_box_handle.__handle = NULL;

    if (!duoplot_internal_isGuiElementHandleContainerKeyInMap(handle_string, gui_element_map))
    {
        printf("Gui element with handle string %s does not exist!\n", handle_string);
        return list_box_handle;
    }

    duoplot_internal_ListBoxInternalHandle* const gui_elem =
        (duoplot_internal_ListBoxInternalHandle*)duoplot_internal_getGuiElementHandleContainer(handle_string,
                                                                                               gui_element_map);

    if (gui_elem->type != DUOPLOT_INTERNAL_GUI_ET_LIST_BOX)
    {
        printf("Gui element with handle string %s is not a slider!\n", handle_string);
        return list_box_handle;
    }

    list_box_handle.__handle = gui_elem;

    return list_box_handle;
}

DUOPLOT_WEAK duoplot_DropdownMenuHandle duoplot_getDropdownMenuHandle(const char* const handle_string)
{
    duoplot_internal_GuiElementMap* const gui_element_map = duoplot_internal_getGuiElementHandles();

    duoplot_DropdownMenuHandle dropdown_menu_handle;
    dropdown_menu_handle.__handle = NULL;

    if (!duoplot_internal_isGuiElementHandleContainerKeyInMap(handle_string, gui_element_map))
    {
        printf("Gui element with handle string %s does not exist!\n", handle_string);
        return dropdown_menu_handle;
    }

    duoplot_internal_DropdownMenuInternalHandle* const gui_elem =
        (duoplot_internal_DropdownMenuInternalHandle*)duoplot_internal_getGuiElementHandleContainer(handle_string,
                                                                                                    gui_element_map);

    if (gui_elem->type != DUOPLOT_INTERNAL_GUI_ET_DROPDOWN_MENU)
    {
        printf("Gui element with handle string %s is not a slider!\n", handle_string);
        return dropdown_menu_handle;
    }

    dropdown_menu_handle.__handle = gui_elem;

    return dropdown_menu_handle;
}

DUOPLOT_WEAK duoplot_CheckboxHandle duoplot_getCheckboxHandle(const char* const handle_string)
{
    duoplot_internal_GuiElementMap* const gui_element_map = duoplot_internal_getGuiElementHandles();

    duoplot_CheckboxHandle checkbox_handle;
    checkbox_handle.__handle = NULL;

    if (!duoplot_internal_isGuiElementHandleContainerKeyInMap(handle_string, gui_element_map))
    {
        printf("Gui element with handle string %s does not exist!\n", handle_string);
        return checkbox_handle;
    }

    duoplot_internal_CheckboxInternalHandle* const gui_elem =
        (duoplot_internal_CheckboxInternalHandle*)duoplot_internal_getGuiElementHandleContainer(handle_string,
                                                                                                gui_element_map);

    if (gui_elem->type != DUOPLOT_INTERNAL_GUI_ET_CHECKBOX)
    {
        printf("Gui element with handle string %s is not a slider!\n", handle_string);
        return checkbox_handle;
    }

    checkbox_handle.__handle = gui_elem;

    return checkbox_handle;
}

DUOPLOT_WEAK duoplot_RadioButtonGroupHandle duoplot_getRadioButtonGroupHandle(const char* const handle_string)
{
    duoplot_internal_GuiElementMap* const gui_element_map = duoplot_internal_getGuiElementHandles();

    duoplot_RadioButtonGroupHandle radiobutton_group_handle;
    radiobutton_group_handle.__handle = NULL;

    if (!duoplot_internal_isGuiElementHandleContainerKeyInMap(handle_string, gui_element_map))
    {
        printf("Gui element with handle string %s does not exist!\n", handle_string);
        return radiobutton_group_handle;
    }

    RadioButtonGroupInternalHandle* const gui_elem =
        (RadioButtonGroupInternalHandle*)duoplot_internal_getGuiElementHandleContainer(handle_string, gui_element_map);

    if (gui_elem->type != DUOPLOT_INTERNAL_GUI_ET_RADIO_BUTTON_GROUP)
    {
        printf("Gui element with handle string %s is not a slider!\n", handle_string);
        return radiobutton_group_handle;
    }

    radiobutton_group_handle.__handle = gui_elem;

    return radiobutton_group_handle;
}

DUOPLOT_WEAK duoplot_EditableTextHandle duoplot_getEditableTextHandle(const char* const handle_string)
{
    duoplot_internal_GuiElementMap* const gui_element_map = duoplot_internal_getGuiElementHandles();

    duoplot_EditableTextHandle editable_text_handle;
    editable_text_handle.__handle = NULL;

    if (!duoplot_internal_isGuiElementHandleContainerKeyInMap(handle_string, gui_element_map))
    {
        printf("Gui element with handle string %s does not exist!\n", handle_string);
        return editable_text_handle;
    }

    duoplot_internal_EditableTextInternalHandle* const gui_elem =
        (duoplot_internal_EditableTextInternalHandle*)duoplot_internal_getGuiElementHandleContainer(handle_string,
                                                                                                    gui_element_map);

    if (gui_elem->type != DUOPLOT_INTERNAL_GUI_ET_EDITABLE_TEXT)
    {
        printf("Gui element with handle string %s is not a slider!\n", handle_string);
        return editable_text_handle;
    }

    editable_text_handle.__handle = gui_elem;

    return editable_text_handle;
}

DUOPLOT_WEAK duoplot_TextLabelHandle duoplot_getTextLabelHandle(const char* const handle_string)
{
    duoplot_internal_GuiElementMap* const gui_element_map = duoplot_internal_getGuiElementHandles();

    duoplot_TextLabelHandle text_label_handle;
    text_label_handle.__handle = NULL;

    if (!duoplot_internal_isGuiElementHandleContainerKeyInMap(handle_string, gui_element_map))
    {
        printf("Gui element with handle string %s does not exist!\n", handle_string);
        return text_label_handle;
    }

    duoplot_internal_TextLabelInternalHandle* const gui_elem =
        (duoplot_internal_TextLabelInternalHandle*)duoplot_internal_getGuiElementHandleContainer(handle_string,
                                                                                                 gui_element_map);

    if (gui_elem->type != DUOPLOT_INTERNAL_GUI_ET_TEXT_LABEL)
    {
        printf("Gui element with handle string %s is not a slider!\n", handle_string);
        return text_label_handle;
    }

    text_label_handle.__handle = gui_elem;

    return text_label_handle;
}

DUOPLOT_WEAK void duoplot_internal_callGuiCallbackFunction(const duoplot_internal_ReceivedGuiData* received_gui_data)
{
    size_t idx = 0U;

    uint8_t* const raw_data = received_gui_data->data;

    const duoplot_internal_GuiElementType type = (duoplot_internal_GuiElementType)(raw_data[idx]);
    idx += sizeof(uint8_t);

    const uint8_t handle_string_length = (uint8_t)(raw_data[idx]);
    idx += sizeof(uint8_t);

    char* const handle_string = (char*)malloc(handle_string_length + 1U);
    strcpy(handle_string, (char*)(raw_data + idx));
    handle_string[handle_string_length] = '\0';  // Null-terminate string
    idx += handle_string_length;

    uint32_t payload_size;
    memcpy(&payload_size, raw_data + idx, sizeof(uint32_t));
    idx += sizeof(uint32_t);

    duoplot_internal_UInt8Array payload_data_view;
    payload_data_view.data = raw_data + idx;
    payload_data_view.size = payload_size;

    if (type == DUOPLOT_INTERNAL_GUI_ET_BUTTON)
    {
        duoplot_internal_ButtonCallbackFunctionMap* const button_callback_function_map =
            duoplot_internal_getButtonCallbackFunctionMap();

        if (duoplot_internal_isButtonCallbackFunctionKeyInMap(handle_string, button_callback_function_map))
        {
            duoplot_ButtonCallbackFunction cb_fun =
                duoplot_internal_getButtonCallbackFunction(handle_string, button_callback_function_map);
            cb_fun(duoplot_getButtonHandle(handle_string));
        }
    }
    else if (type == DUOPLOT_INTERNAL_GUI_ET_SLIDER)
    {
        duoplot_internal_SliderCallbackFunctionMap* const slider_callback_function_map =
            duoplot_internal_getSliderCallbackFunctionMap();

        if (duoplot_internal_isSliderCallbackFunctionKeyInMap(handle_string, slider_callback_function_map))
        {
            duoplot_SliderCallbackFunction cb_fun =
                duoplot_internal_getSliderCallbackFunction(handle_string, slider_callback_function_map);
            cb_fun(duoplot_getSliderHandle(handle_string));
        }
    }
    else if (type == DUOPLOT_INTERNAL_GUI_ET_LIST_BOX)
    {
        duoplot_internal_ListBoxCallbackFunctionMap* const list_box_callback_function_map =
            duoplot_internal_getListBoxCallbackFunctionMap();

        if (duoplot_internal_isListBoxCallbackFunctionKeyInMap(handle_string, list_box_callback_function_map))
        {
            duoplot_ListBoxCallbackFunction cb_fun =
                duoplot_internal_getListBoxCallbackFunction(handle_string, list_box_callback_function_map);
            cb_fun(duoplot_getListBoxHandle(handle_string));
        }
    }
    else if (type == DUOPLOT_INTERNAL_GUI_ET_DROPDOWN_MENU)
    {
        duoplot_internal_DropdownMenuCallbackFunctionMap* const dropdown_menu_callback_function_map =
            duoplot_internal_getDropdownMenuCallbackFunctionMap();

        if (duoplot_internal_isDropdownMenuCallbackFunctionKeyInMap(handle_string, dropdown_menu_callback_function_map))
        {
            duoplot_DropdownMenuCallbackFunction cb_fun =
                duoplot_internal_getDropdownMenuCallbackFunction(handle_string, dropdown_menu_callback_function_map);
            cb_fun(duoplot_getDropdownMenuHandle(handle_string));
        }
    }
    else if (type == DUOPLOT_INTERNAL_GUI_ET_CHECKBOX)
    {
        duoplot_internal_CheckboxCallbackFunctionMap* const checkbox_callback_function_map =
            duoplot_internal_getCheckboxCallbackFunctionMap();

        if (duoplot_internal_isCheckboxCallbackFunctionKeyInMap(handle_string, checkbox_callback_function_map))
        {
            duoplot_CheckboxCallbackFunction cb_fun =
                duoplot_internal_getCheckboxCallbackFunction(handle_string, checkbox_callback_function_map);
            cb_fun(duoplot_getCheckboxHandle(handle_string));
        }
    }
    else if (type == DUOPLOT_INTERNAL_GUI_ET_RADIO_BUTTON_GROUP)
    {
        duoplot_internal_RadioButtonGroupCallbackFunctionMap* const radiobutton_group_callback_function_map =
            duoplot_internal_getRadioButtonGroupCallbackFunctionMap();

        if (duoplot_internal_isRadioButtonGroupCallbackFunctionKeyInMap(handle_string,
                                                                        radiobutton_group_callback_function_map))
        {
            duoplot_RadioButtonGroupCallbackFunction cb_fun = duoplot_internal_getRadioButtonGroupCallbackFunction(
                handle_string, radiobutton_group_callback_function_map);
            cb_fun(duoplot_getRadioButtonGroupHandle(handle_string));
        }
    }
    else if (type == DUOPLOT_INTERNAL_GUI_ET_EDITABLE_TEXT)
    {
        duoplot_internal_EditableTextCallbackFunctionMap* const editable_text_callback_function_map =
            duoplot_internal_getEditableTextCallbackFunctionMap();

        if (duoplot_internal_isEditableTextCallbackFunctionKeyInMap(handle_string, editable_text_callback_function_map))
        {
            duoplot_EditableTextCallbackFunction cb_fun =
                duoplot_internal_getEditableTextCallbackFunction(handle_string, editable_text_callback_function_map);
            cb_fun(duoplot_getEditableTextHandle(handle_string));
        }
    }
}

DUOPLOT_WEAK void duoplot_internal_waitForSyncForAllGuiElements()
{
    printf("Waiting for duoplot application to send GUI state...\n");
    const duoplot_internal_ReceivedGuiData received_data = duoplot_internal_receiveGuiData();

    printf("GUI state received!\n");

    size_t idx = 1U;

    uint8_t* const raw_data = received_data.data;

    // Receive[0]: Number of gui objects (std::uint8_t)
    const size_t num_gui_objects = (size_t)(raw_data[0]);

    for (size_t k = 0; k < num_gui_objects; k++)
    {
        // Receive[1]: Gui element type (uint8_t)
        const duoplot_internal_GuiElementType type = (duoplot_internal_GuiElementType)(raw_data[idx]);
        idx += sizeof(uint8_t);

        // Receive[2]: Handle string length (uint8_t)
        const uint8_t handle_string_length = (uint8_t)(raw_data[idx]);
        idx += sizeof(uint8_t);

        char* const handle_string = (char*)malloc(handle_string_length + 1U);

        // Receive[3]: Handle string (variable)
        for (uint8_t i = 0; i < handle_string_length; i++)
        {
            handle_string[i] = raw_data[idx];
            idx++;
        }
        handle_string[handle_string_length] = '\0';  // Null-terminate string

        uint32_t size_of_current_gui_element;

        // Receive[4]: Current gui element payload size (uint32_t)
        memcpy(&size_of_current_gui_element, raw_data + idx, sizeof(uint32_t));
        idx += sizeof(uint32_t);

        const duoplot_internal_UInt8Array gui_element_data = {raw_data + idx, size_of_current_gui_element};

        // Receive[5]: Gui element data (variable)
        duoplot_internal_populateGuiElementWithData(type, handle_string, &gui_element_data);

        free(handle_string);

        idx += size_of_current_gui_element;
    }

    free(received_data.data);
}

DUOPLOT_WEAK void duoplot_registerButtonCallback(const char* const handle_string,
                                                 void (*button_callback_function)(const duoplot_ButtonHandle))
{
    duoplot_internal_ButtonCallbackFunctionMap* const button_callback_function_map =
        duoplot_internal_getButtonCallbackFunctionMap();

    if (duoplot_internal_isButtonCallbackFunctionKeyInMap(handle_string, button_callback_function_map))
    {
        printf("Button callback with name %s already exists! Overwriting old callback...\n", handle_string);
    }

    duoplot_internal_insertElementIntoButtonCallbackFunctionMap(
        button_callback_function_map, handle_string, button_callback_function);
}

DUOPLOT_WEAK void duoplot_registerSliderCallback(const char* const handle_string,
                                                 void (*slider_callback_function)(const duoplot_SliderHandle))
{
    duoplot_internal_SliderCallbackFunctionMap* const slider_callback_function_map =
        duoplot_internal_getSliderCallbackFunctionMap();

    if (duoplot_internal_isSliderCallbackFunctionKeyInMap(handle_string, slider_callback_function_map))
    {
        printf("Slider callback with name %s already exists! Overwriting old callback...\n", handle_string);
    }

    duoplot_internal_insertElementIntoSliderCallbackFunctionMap(
        slider_callback_function_map, handle_string, slider_callback_function);
}

DUOPLOT_WEAK void duoplot_registerListBoxCallback(const char* const handle_string,
                                                  void (*list_box_callback_function)(const duoplot_ListBoxHandle))
{
    duoplot_internal_ListBoxCallbackFunctionMap* const list_box_callback_function_map =
        duoplot_internal_getListBoxCallbackFunctionMap();

    if (duoplot_internal_isListBoxCallbackFunctionKeyInMap(handle_string, list_box_callback_function_map))
    {
        printf("List box callback with name %s already exists! Overwriting old callback...\n", handle_string);
    }

    duoplot_internal_insertElementIntoListBoxCallbackFunctionMap(
        list_box_callback_function_map, handle_string, list_box_callback_function);
}

DUOPLOT_WEAK void duoplot_registerDropdownMenuCallback(
    const char* const handle_string, void (*dropdown_menu_callback_function)(const duoplot_DropdownMenuHandle))
{
    duoplot_internal_DropdownMenuCallbackFunctionMap* const dropdown_menu_callback_function_map =
        duoplot_internal_getDropdownMenuCallbackFunctionMap();

    if (duoplot_internal_isDropdownMenuCallbackFunctionKeyInMap(handle_string, dropdown_menu_callback_function_map))
    {
        printf("Drop down menu callback with name %s already exists! Overwriting old callback...\n", handle_string);
    }

    duoplot_internal_insertElementIntoDropdownMenuCallbackFunctionMap(
        dropdown_menu_callback_function_map, handle_string, dropdown_menu_callback_function);
}

DUOPLOT_WEAK void duoplot_registerCheckboxCallback(const char* const handle_string,
                                                   void (*checkbox_callback_function)(const duoplot_CheckboxHandle))
{
    duoplot_internal_CheckboxCallbackFunctionMap* const checkbox_callback_function_map =
        duoplot_internal_getCheckboxCallbackFunctionMap();

    if (duoplot_internal_isCheckboxCallbackFunctionKeyInMap(handle_string, checkbox_callback_function_map))
    {
        printf("Checkbox callback with name %s already exists! Overwriting old callback...\n", handle_string);
    }

    duoplot_internal_insertElementIntoCheckboxCallbackFunctionMap(
        checkbox_callback_function_map, handle_string, checkbox_callback_function);
}

DUOPLOT_WEAK void duoplot_registerRadioButtonGroupCallback(
    const char* const handle_string, void (*radiobutton_group_callback_function)(const duoplot_RadioButtonGroupHandle))
{
    duoplot_internal_RadioButtonGroupCallbackFunctionMap* const radiobutton_group_callback_function_map =
        duoplot_internal_getRadioButtonGroupCallbackFunctionMap();

    if (duoplot_internal_isRadioButtonGroupCallbackFunctionKeyInMap(handle_string,
                                                                    radiobutton_group_callback_function_map))
    {
        printf("Radio button group callback with name %s already exists! Overwriting old callback...\n", handle_string);
    }

    duoplot_internal_insertElementIntoRadioButtonGroupCallbackFunctionMap(
        radiobutton_group_callback_function_map, handle_string, radiobutton_group_callback_function);
}

DUOPLOT_WEAK void duoplot_registerEditableTextCallback(
    const char* const handle_string, void (*editable_text_callback_function)(const duoplot_EditableTextHandle))
{
    duoplot_internal_EditableTextCallbackFunctionMap* const editable_text_callback_function_map =
        duoplot_internal_getEditableTextCallbackFunctionMap();

    if (duoplot_internal_isEditableTextCallbackFunctionKeyInMap(handle_string, editable_text_callback_function_map))
    {
        printf("Editable text callback with name %s already exists! Overwriting old callback...\n", handle_string);
    }

    duoplot_internal_insertElementIntoEditableTextCallbackFunctionMap(
        editable_text_callback_function_map, handle_string, editable_text_callback_function);
}

#endif  // DUOPLOT_GUI_INTERNAL_H