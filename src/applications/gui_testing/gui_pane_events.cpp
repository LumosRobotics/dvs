#include <iostream>

#include "gui_pane.h"

/*
The state can only be changed in keyPressed, keyReleased, mouseLeftPressed, mouseLeftReleased
EditingObject
Interacting
Hoovering
Dragging

Hoovering -> EditingObject: Control pressed, and mouse is already pressed, and mouse is within object, or the other way
around Hoovering -> Interacting: Mouse pressed, and mouse is within object Hoovering -> Dragging: Mouse pressed outside
any object


EditingObject -> Interacting: When control is released
EditingObject -> Hoovering:
EditingObject -> Dragging:

*/

int i = 0;

void GuiPane::keyPressedCallback(wxKeyEvent& evt)
{
    if (evt.GetKeyCode() == 308)
    {
        control_pressed_ = true;

        for(const auto& element : gui_elements_)
        {
            element->setRenderSizingRectangles(true);
        }

        if (interaction_state_ == InteractionState::Hoovering)
        {
            if (current_hovered_element_ != nullptr)
            {
                const wxPoint mouse_pos = wxGetMousePosition() - this->GetScreenPosition();
                const ChangeDirection change_direction = current_hovered_element_->GetDirectionFromMouse(mouse_pos);
                setCursor(change_direction);
            }
        }
        else if (interaction_state_ == InteractionState::Interacting)
        {
            /*
            Mouse must be pressed
            Two options:
             * Mouse is outside object, if so, do nothing
             * Mouse is inside object, switch to changing object
            */

            if (left_mouse_pressed_)  // Must be true
            {
                const wxPoint mouse_pos = wxGetMousePosition() - this->GetScreenPosition();

                change_direction_at_press_ = current_pressed_element_->GetDirectionFromMouse(mouse_pos);
                setCursor(change_direction_at_press_);
                interaction_state_ = InteractionState::EditingObject;
            }
        }
        else if (interaction_state_ == InteractionState::EditingObject)
        {
            assert(false && "Should not be able to go here!");
        }
        else if (interaction_state_ == InteractionState::Dragging)
        {
            // Do nothing
        }
        else
        {
            assert(false && "Unhandled state");
        }
    }
}

void GuiPane::keyReleasedCallback(wxKeyEvent& evt)
{
    if (evt.GetKeyCode() == 308)
    {
        for(const auto& element : gui_elements_)
        {
            element->setRenderSizingRectangles(false);
        }

        control_pressed_ = false;
        if (current_hovered_element_ != nullptr)
        {
            wxSetCursor(wxCursor(wxCURSOR_HAND));
        }
        else
        {
            wxSetCursor(wxCursor(wxCURSOR_ARROW));
        }

        if (interaction_state_ == InteractionState::EditingObject)
        {
            interaction_state_ = InteractionState::Interacting;
        }
        else if (interaction_state_ == InteractionState::Hoovering)
        {
            // Do nothing
        }
        else if (interaction_state_ == InteractionState::Interacting)
        {
            // Do nothing
        }
        else if (interaction_state_ == InteractionState::Dragging)
        {
            // Do nothing
        }
        // interaction_state_ = InteractionState::Hoovering;
    }
    change_direction_at_press_ = ChangeDirection::NONE;
}

void GuiPane::mouseLeftPressed(wxMouseEvent& event)
{
    left_mouse_pressed_ = true;

    const std::vector<std::shared_ptr<GuiElement>>::reverse_iterator pressed_object =
        std::find_if(gui_elements_.rbegin(), gui_elements_.rend(), [&event](const auto& element) {
            return element->PointIsWithin(event.GetPosition());
        });

    if (pressed_object != gui_elements_.rend())
    {
        current_pressed_element_ = *pressed_object;
        current_hovered_element_ = nullptr;

        if (control_pressed_)
        {
            // Editing element
            change_direction_at_press_ = current_pressed_element_->GetDirectionFromMouse(event.GetPosition());
            interaction_state_ = InteractionState::EditingObject;
            setCursor(change_direction_at_press_);

            for(const auto& element : gui_elements_)
            {
                element->setRenderSizingRectangles(false);
            }
        }
        else
        {
            // Interacting with element
            interaction_state_ = InteractionState::Interacting;
            current_pressed_element_->mousePressed(event);
        }
    }
    else
    {
        change_direction_at_press_ = ChangeDirection::NONE;
        interaction_state_ = InteractionState::Dragging;
    }
}

void GuiPane::mouseLeftReleased(wxMouseEvent& event)
{
    if (interaction_state_ == InteractionState::Interacting)
    {
        current_pressed_element_->mouseReleased(event);
    }

    current_pressed_element_ = nullptr;

    left_mouse_pressed_ = false;
    interaction_state_ = InteractionState::Hoovering;

    const std::vector<std::shared_ptr<GuiElement>>::reverse_iterator potentially_hovered_element =
        std::find_if(gui_elements_.rbegin(), gui_elements_.rend(), [&event](const auto& element) {
            return element->PointIsWithin(event.GetPosition());
        });

    if (potentially_hovered_element != gui_elements_.rend())
    {
        current_hovered_element_ = *potentially_hovered_element;

        if (control_pressed_)
        {
            const ChangeDirection change_dir = current_hovered_element_->GetDirectionFromMouse(event.GetPosition());
            setCursor(change_dir);
            for(const auto& element : gui_elements_)
            {
                element->setRenderSizingRectangles(true);
            }
        }
        else
        {
            wxSetCursor(wxCursor(wxCURSOR_HAND));
        }
    }
    else
    {
        wxSetCursor(wxCursor(wxCURSOR_ARROW));
    }
}

void GuiPane::mouseLeftDoubleClicked(wxMouseEvent& event)
{
    left_mouse_pressed_ = true;

    const std::vector<std::shared_ptr<GuiElement>>::reverse_iterator pressed_object =
        std::find_if(gui_elements_.rbegin(), gui_elements_.rend(), [&event](const auto& element) {
            return element->PointIsWithin(event.GetPosition());
        });

    if (pressed_object != gui_elements_.rend())
    {
        current_pressed_element_ = *pressed_object;
        current_hovered_element_ = nullptr;

        if (control_pressed_)
        {
            // Editing element
            change_direction_at_press_ = current_pressed_element_->GetDirectionFromMouse(event.GetPosition());
            interaction_state_ = InteractionState::EditingObject;
            setCursor(change_direction_at_press_);
        }
        else
        {
            // Interacting with element
            interaction_state_ = InteractionState::Interacting;
            current_pressed_element_->mousePressed(event);
        }
    }
    else
    {
        change_direction_at_press_ = ChangeDirection::NONE;
        interaction_state_ = InteractionState::Dragging;
    }
}

void GuiPane::mouseMoved(wxMouseEvent& event)
{
    const wxPoint delta_vec = event.GetPosition() - previous_mouse_position_;
    previous_mouse_position_ = event.GetPosition();

    if (interaction_state_ == InteractionState::EditingObject)
    {
        wxGLCanvas::SetCurrent(*m_context_);
        wxPaintDC(this);  // TODO: Can be removed?

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader_collection_.simple_shader.use();
        current_pressed_element_->ChangePositionOrSize(delta_vec, change_direction_at_press_);
    }
    else if (interaction_state_ == InteractionState::Interacting)
    {
        current_pressed_element_->mouseDragged(event, delta_vec);
    }
    else if (interaction_state_ == InteractionState::Dragging)
    {
    }
    else if (interaction_state_ == InteractionState::Hoovering)
    {
        const std::vector<std::shared_ptr<GuiElement>>::reverse_iterator possibly_hoovered_element =
            std::find_if(gui_elements_.rbegin(), gui_elements_.rend(), [&event](const auto& element) {
                return element->PointIsWithin(event.GetPosition());
            });

        if (possibly_hoovered_element != gui_elements_.rend())
        {
            // Found one valid element
            if (current_hovered_element_ != nullptr)
            {
                // If we are already hovering over an element
                if (current_hovered_element_ != *possibly_hoovered_element)
                {
                    // If we didn't found the same element, we have exited the old element, and entered a new one
                    current_hovered_element_->mouseExited(event);
                    current_hovered_element_ = *possibly_hoovered_element;
                    current_hovered_element_->mouseEntered(event);
                }
                else
                {
                    // We simply found the old element, and thus mouseMoved should be called for it
                    current_hovered_element_->mouseMoved(event, delta_vec);
                }
            }
            else
            {
                // If we are not already hovering over an element
                current_hovered_element_ = *possibly_hoovered_element;
                current_hovered_element_->mouseEntered(event);
            }

            if (control_pressed_)
            {
                const ChangeDirection change_direction =
                    current_hovered_element_->GetDirectionFromMouse(event.GetPosition());
                setCursor(change_direction);
            }
            else
            {
                wxSetCursor(wxCursor(wxCURSOR_HAND));
            }
        }
        else
        {
            if (current_hovered_element_ != nullptr)
            {
                // We exited the element
                current_hovered_element_->mouseExited(event);
                current_hovered_element_ = nullptr;
                wxSetCursor(wxCursor(wxCURSOR_ARROW));
            }
        }
    }
}

void GuiPane::mouseEntered(wxMouseEvent& event)
{
    if (interaction_state_ == InteractionState::Hoovering)
    {
        const std::vector<std::shared_ptr<GuiElement>>::reverse_iterator potentially_hovered_element =
            std::find_if(gui_elements_.rbegin(), gui_elements_.rend(), [&event](const auto& element) {
                return element->PointIsWithin(event.GetPosition());
            });

        if (potentially_hovered_element != gui_elements_.rend())
        {
            current_hovered_element_ = *potentially_hovered_element;

            if (control_pressed_)
            {
                const ChangeDirection change_dir = current_hovered_element_->GetDirectionFromMouse(event.GetPosition());
                setCursor(change_dir);
            }
            else
            {
                wxSetCursor(wxCursor(wxCURSOR_HAND));
            }
        }
        else
        {
            wxSetCursor(wxCursor(wxCURSOR_ARROW));
        }
    }
    else if (interaction_state_ == InteractionState::EditingObject)
    {
    }
    else if (interaction_state_ == InteractionState::Interacting)
    {
        // Do nothing??
    }
    else if (interaction_state_ == InteractionState::Dragging)
    {
        // Do nothing
    }

    previous_mouse_position_ = event.GetPosition();
}

void GuiPane::mouseExited(wxMouseEvent& event)
{
    if (interaction_state_ == InteractionState::Hoovering)
    {
        if (current_hovered_element_ != nullptr)
        {
            current_hovered_element_->mouseExited(event);
            current_hovered_element_ = nullptr;
        }
        wxSetCursor(wxCursor(wxCURSOR_ARROW));
    }
    else if (interaction_state_ == InteractionState::EditingObject)
    {
    }
    else if (interaction_state_ == InteractionState::Interacting)
    {
    }
    else if (interaction_state_ == InteractionState::Dragging)
    {
    }
}

void GuiPane::setCursor(const ChangeDirection change_direction)
{
    switch (change_direction)
    {
        case ChangeDirection::LEFT:
            wxSetCursor(wxCursor(wxCURSOR_SIZEWE));
            break;
        case ChangeDirection::RIGHT:
            wxSetCursor(wxCursor(wxCURSOR_SIZEWE));
            break;
        case ChangeDirection::UP:
            wxSetCursor(wxCursor(wxCURSOR_SIZENS));
            break;
        case ChangeDirection::DOWN:
            wxSetCursor(wxCursor(wxCURSOR_SIZENS));
            break;
        case ChangeDirection::RIGHT_DOWN:
            wxSetCursor(wxCursor(wxCURSOR_SIZENWSE));
            break;
        case ChangeDirection::LEFT_DOWN:
            wxSetCursor(wxCursor(wxCURSOR_SIZENESW));
            break;
        case ChangeDirection::RIGHT_UP:
            wxSetCursor(wxCursor(wxCURSOR_SIZENESW));
            break;
        case ChangeDirection::LEFT_UP:
            wxSetCursor(wxCursor(wxCURSOR_SIZENWSE));
            break;
        case ChangeDirection::MIDDLE:
            wxSetCursor(wxCursor(wxCURSOR_SIZING));
            break;
        default:
            wxSetCursor(wxCursor(wxCURSOR_ARROW));
    }
}

void GuiPane::mouseRightPressed(wxMouseEvent& event)
{
    right_mouse_pressed_ = true;

    const std::vector<std::shared_ptr<GuiElement>>::reverse_iterator pressed_object =
        std::find_if(gui_elements_.rbegin(), gui_elements_.rend(), [&event](const auto& element) {
            return element->PointIsWithin(event.GetPosition());
        });

    if (pressed_object != gui_elements_.rend())
    {
        right_clicked_element_ = *pressed_object;
    }
}

void GuiPane::mouseRightReleased(wxMouseEvent& event)
{
    right_mouse_pressed_ = false;

    if (right_clicked_element_ != nullptr)
    {
        PopupMenu(popup_menu_, event.GetPosition());
        // right_clicked_element_->mouseReleased(event);
        right_clicked_element_ = nullptr;
    }
}
