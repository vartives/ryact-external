#pragma once
#include "../../imgui.h"


namespace c {
    inline ImVec4 rect;
    inline ImVec4 main_color;
    inline ImVec4 main_color_1;
    inline ImVec4 line_in_active;
    inline ImVec4 background;
    inline ImVec4 text_active;
    inline ImVec4 text_active_in_1;
    inline ImVec4 text_active_in_active;
    inline ImVec4 text_red;
    inline ImVec4 text_green;
    inline ImVec4 text_active_in;
    inline ImVec4 text_active_in_alpha;
    inline ImVec4 text_group_tab;
    inline ImVec4 line;
    inline ImVec4 rect_window;
    inline ImVec4 icon_active;
    inline ImVec4 icon_inactive;
    inline ImVec4 background_active;
    inline ImVec4 background_inactive;
    inline ImVec4 select_tab_active;
    inline ImVec4 select_tab_inactive;
    inline ImVec4 background_icon;
    inline ImVec4 background_icon_in;
    inline ImVec4 icon_exit;
    inline ImVec4 icon_exit_in;
    inline ImVec4 child;
    inline ImVec4 child_rect;
    inline ImVec4 window_bg;
    inline ImVec4 checkbox_bg;
    inline ImVec4 checkbox_bg_in;
    inline ImVec4 rect_checkbox_bg;
    inline ImVec4 rect_checkbox_bg_in;
    inline ImVec4 separator;
    inline ImVec4 background_slider;
    inline ImVec4 text_slider_value;
    inline ImVec4 settings_checkbox;
    inline ImVec4 rect_input;
    inline ImVec4 bg_input;
    inline ImVec4 bg_selectable;
    inline ImVec4 bg_selectable_in;
    inline ImVec4 bg_notif;
    inline ImVec4 input_scale;
    inline ImVec4 keybind_bg;
    inline ImVec4 selected_bg;
    inline ImVec4 selected_rect;
    inline ImVec4 selected_inactiv;
    inline ImColor main_color_tabs;
    inline ImVec4 icon_in_activ;
    inline ImVec4 bg_scrollbar;
    inline ImVec4 bg_element;
    inline ImVec4 bg_element_1;
    inline ImVec4 esp_bg;
    inline ImVec4 input;
    inline ImVec4 black;
    inline float columns;
    inline float hue_color[4] = { 0.f, 1.f, 1.f, 1.f }; // default
}

// Theme 1 – SAPPHIRE FN
namespace theme1 {
    inline ImVec4 rect = ImColor(52, 52, 52, 255);
    inline ImVec4 main_color = ImColor(129, 99, 255, 255);
    inline ImVec4 main_color_1 = ImColor(129, 99, 255, 255);
    inline ImVec4 line_in_active = ImColor(129, 99, 255, 0);
    inline ImVec4 background = ImColor(26, 25, 33, 255);
    inline ImVec4 text_active = ImColor(255, 255, 255, 255);
    inline ImVec4 text_active_in_1 = ImColor(255, 255, 255, 127);
    inline ImVec4 text_active_in_active = ImColor(97, 94, 114, 255);
    inline ImVec4 text_red = ImColor(208, 48, 47, 255);
    inline ImVec4 text_green = ImColor(47, 208, 48, 255);
    inline ImVec4 text_active_in = ImColor(255, 255, 255, 175);
    inline ImVec4 text_active_in_alpha = ImColor(255, 255, 255, 0);
    inline ImVec4 text_group_tab = ImColor(54, 54, 72, 255);
    inline ImVec4 line = ImColor(23, 23, 30, 255);
    inline ImVec4 rect_window = ImColor(34, 34, 43, 255);
    inline ImVec4 icon_active = ImColor(129, 99, 255, 255);
    inline ImVec4 icon_inactive = ImColor(129, 99, 255, 175);
    inline ImVec4 background_active = ImColor(22, 22, 30, 255);
    inline ImVec4 background_inactive = ImColor(22, 22, 30, 0);
    inline ImVec4 select_tab_active = ImColor(129, 99, 255, 255);
    inline ImVec4 select_tab_inactive = ImColor(129, 99, 255, 0);
    inline ImVec4 background_icon = ImColor(22, 22, 30, 255);
    inline ImVec4 background_icon_in = ImColor(22, 22, 30, 175);
    inline ImVec4 icon_exit = ImColor(255, 129, 99, 255);
    inline ImVec4 icon_exit_in = ImColor(255, 129, 99, 175);
    inline ImVec4 child = ImColor(21, 21, 28, 255);
    inline ImVec4 child_rect = ImColor(24, 24, 32, 255);
    inline ImVec4 window_bg = ImColor(21, 20, 26, 255);
    inline ImVec4 checkbox_bg = ImColor(129, 99, 255, 127);
    inline ImVec4 checkbox_bg_in = ImColor(32, 32, 42, 127);
    inline ImVec4 rect_checkbox_bg = ImColor(129, 99, 255, 255);
    inline ImVec4 rect_checkbox_bg_in = ImColor(32, 32, 42, 255);
    inline ImVec4 separator = ImColor(33, 31, 41, 255);
    inline ImVec4 background_slider = ImColor(32, 32, 42, 255);
    inline ImVec4 text_slider_value = ImColor(255, 255, 255, 127);
    inline ImVec4 settings_checkbox = ImColor(40, 40, 53, 255);
    inline ImVec4 rect_input = ImColor(32, 32, 42, 255);
    inline ImVec4 bg_input = ImColor(32, 32, 42, 127);
    inline ImVec4 bg_selectable = ImColor(42, 42, 52, 255);
    inline ImVec4 bg_selectable_in = ImColor(32, 32, 42, 127);
    inline ImVec4 bg_notif = ImColor(18, 18, 24, 255);
    inline ImVec4 input_scale = ImColor(32, 32, 42, 255);
    inline ImVec4 keybind_bg = ImColor(32, 32, 42, 255);
    inline ImVec4 selected_bg = ImColor(129, 99, 255, 127);
    inline ImVec4 selected_rect = ImColor(129, 99, 255, 255);
    inline ImVec4 selected_inactiv = ImColor(129, 99, 255, 0);
    inline ImColor main_color_tabs = ImColor(129, 99, 255, 255);
    inline ImVec4 icon_in_activ = ImColor(128, 128, 128, 255);
    inline ImVec4 bg_scrollbar = ImColor(26, 25, 33, 255);
    inline ImVec4 bg_element = ImColor(32, 32, 42, 127);
    inline ImVec4 bg_element_1 = ImColor(32, 32, 42, 255);
    inline ImVec4 esp_bg = ImColor(129, 99, 255, 64);
    inline ImVec4 input = ImColor(32, 32, 42, 127);
    inline ImVec4 black = ImColor(0, 0, 0, 255);
    inline float columns = 3;
    inline float hue_color[4] = { 0, 255, 255, 1.f };
}

// Theme 2 – Test
namespace theme2 {
    // Core background colors
    inline ImVec4 rect = ImColor(20, 20, 20, 255);
    inline ImVec4 background = ImColor(10, 10, 10, 255);
    inline ImVec4 background_active = ImColor(10, 10, 10, 255);
    inline ImVec4 background_inactive = ImColor(10, 10, 10, 0);
    inline ImVec4 background_icon = ImColor(10, 10, 10, 255);
    inline ImVec4 background_icon_in = ImColor(10, 10, 10, 175);
    inline ImVec4 child = ImColor(12, 12, 12, 255);
    inline ImVec4 child_rect = ImColor(15, 15, 15, 255);
    inline ImVec4 window_bg = ImColor(10, 10, 10, 255);
    inline ImVec4 rect_window = ImColor(25, 25, 25, 255);
    inline ImVec4 bg_notif = ImColor(16, 16, 22, 255);

    // Accent color (magenta)
    inline ImVec4 main_color = ImColor(255, 0, 255, 255);
    inline ImVec4 main_color_1 = main_color;
    inline ImVec4 select_tab_active = main_color;
    inline ImVec4 select_tab_inactive = ImColor(255, 0, 255, 0);
    inline ImVec4 rect_checkbox_bg = main_color;
    inline ImVec4 selected_rect = main_color;
    inline ImVec4 selected_bg = ImColor(255, 0, 255, 127);
    inline ImVec4 selected_inactiv = ImColor(255, 0, 255, 0);
    inline ImVec4 esp_bg = ImColor(255, 0, 255, 64);
    inline ImColor main_color_tabs = main_color;

    // Text colors (white only)
    inline ImVec4 text_active = ImColor(255, 255, 255, 255);
    inline ImVec4 text_active_in = ImColor(255, 255, 255, 200);
    inline ImVec4 text_active_in_1 = ImColor(255, 255, 255, 127);
    inline ImVec4 text_active_in_alpha = ImColor(255, 255, 255, 0);
    inline ImVec4 text_active_in_active = ImColor(255, 255, 255, 255);
    inline ImVec4 text_red = ImColor(255, 64, 64, 255);
    inline ImVec4 text_green = ImColor(80, 255, 80, 255);
    inline ImVec4 text_slider_value = ImColor(255, 255, 255, 127);

    // Icons
    inline ImVec4 icon_active = ImColor(255, 0, 255, 255);     // magenta
    inline ImVec4 icon_inactive = ImColor(255, 0, 255, 175);
    inline ImVec4 icon_in_activ = ImColor(255, 0, 255, 255);
    inline ImVec4 icon_exit = ImColor(255, 64, 64, 255);       // red
    inline ImVec4 icon_exit_in = ImColor(255, 64, 64, 175);

    // Checkboxes
    inline ImVec4 checkbox_bg = ImColor(255, 0, 255, 127);     // magenta overlay
    inline ImVec4 checkbox_bg_in = ImColor(0, 0, 0, 127);      // black background
    inline ImVec4 rect_checkbox_bg_in = ImColor(0, 0, 0, 255);

    // Inputs and sliders
    inline ImVec4 rect_input = ImColor(20, 20, 20, 255);        // dark gray
    inline ImVec4 bg_input = ImColor(20, 20, 20, 127);
    inline ImVec4 input = bg_input;
    inline ImVec4 input_scale = rect_input;
    inline ImVec4 keybind_bg = ImColor(20, 20, 20, 255);        // not pure black
    inline ImVec4 background_slider = ImColor(25, 25, 25, 255);
    inline ImVec4 settings_checkbox = ImColor(20, 20, 20, 255);

    // Tabs and group blocks
    inline ImVec4 text_group_tab = ImColor(40, 40, 40, 255);
    inline ImVec4 bg_selectable = ImColor(30, 30, 30, 255);
    inline ImVec4 bg_selectable_in = ImColor(25, 25, 25, 127);
    inline ImVec4 bg_element = ImColor(25, 25, 25, 127);
    inline ImVec4 bg_element_1 = ImColor(30, 30, 30, 255);

    // Scrollbars and separators
    inline ImVec4 bg_scrollbar = background;
    inline ImVec4 line = ImColor(30, 30, 30, 255);
    inline ImVec4 line_in_active = ImColor(255, 0, 255, 0);
    inline ImVec4 separator = ImColor(38, 38, 38, 255);

    // Misc
    inline ImVec4 black = ImColor(0, 0, 0, 255);
    inline float columns = 3;
    inline float hue_color[4] = { 300, 255, 255, 1.f };

}

namespace theme3 {
    // Core background colors
    inline ImVec4 rect = ImColor(20, 20, 20, 255);
    inline ImVec4 background = ImColor(10, 10, 10, 255);
    inline ImVec4 background_active = ImColor(10, 10, 10, 255);
    inline ImVec4 background_inactive = ImColor(10, 10, 10, 0);
    inline ImVec4 background_icon = ImColor(10, 10, 10, 255);
    inline ImVec4 background_icon_in = ImColor(10, 10, 10, 175);
    inline ImVec4 child = ImColor(12, 12, 12, 255);
    inline ImVec4 child_rect = ImColor(15, 15, 15, 255);
    inline ImVec4 window_bg = ImColor(10, 10, 10, 255);
    inline ImVec4 rect_window = ImColor(25, 25, 25, 255);
    inline ImVec4 bg_notif = ImColor(16, 16, 22, 255);

    // Accent color (blue)
    inline ImVec4 main_color = ImColor(0, 128, 255, 255);
    inline ImVec4 main_color_1 = main_color;
    inline ImVec4 select_tab_active = main_color;
    inline ImVec4 select_tab_inactive = ImColor(0, 128, 255, 0);
    inline ImVec4 rect_checkbox_bg = main_color;
    inline ImVec4 selected_rect = main_color;
    inline ImVec4 selected_bg = ImColor(0, 128, 255, 127);
    inline ImVec4 selected_inactiv = ImColor(0, 128, 255, 0);
    inline ImVec4 esp_bg = ImColor(0, 128, 255, 64);
    inline ImColor main_color_tabs = main_color;

    // Text colors
    inline ImVec4 text_active = ImColor(255, 255, 255, 255);
    inline ImVec4 text_active_in = ImColor(255, 255, 255, 200);
    inline ImVec4 text_active_in_1 = ImColor(255, 255, 255, 127);
    inline ImVec4 text_active_in_alpha = ImColor(255, 255, 255, 0);
    inline ImVec4 text_active_in_active = ImColor(255, 255, 255, 255);
    inline ImVec4 text_red = ImColor(255, 64, 64, 255);
    inline ImVec4 text_green = ImColor(80, 255, 80, 255);
    inline ImVec4 text_slider_value = ImColor(255, 255, 255, 127);

    // Icons
    inline ImVec4 icon_active = ImColor(0, 128, 255, 255);       // blue
    inline ImVec4 icon_inactive = ImColor(0, 128, 255, 175);
    inline ImVec4 icon_in_activ = ImColor(0, 128, 255, 255);
    inline ImVec4 icon_exit = ImColor(255, 64, 64, 255);         // red
    inline ImVec4 icon_exit_in = ImColor(255, 64, 64, 175);

    // Checkboxes
    inline ImVec4 checkbox_bg = ImColor(0, 128, 255, 127);       // blue
    inline ImVec4 checkbox_bg_in = ImColor(0, 0, 0, 127);        // black background
    inline ImVec4 rect_checkbox_bg_in = ImColor(0, 0, 0, 255);

    // Inputs and sliders
    inline ImVec4 rect_input = ImColor(20, 20, 20, 255);
    inline ImVec4 bg_input = ImColor(20, 20, 20, 127);
    inline ImVec4 input = bg_input;
    inline ImVec4 input_scale = rect_input;
    inline ImVec4 keybind_bg = ImColor(20, 20, 20, 255);
    inline ImVec4 background_slider = ImColor(25, 25, 25, 255);
    inline ImVec4 settings_checkbox = ImColor(20, 20, 20, 255);

    // Tabs and group blocks
    inline ImVec4 text_group_tab = ImColor(40, 40, 40, 255);
    inline ImVec4 bg_selectable = ImColor(30, 30, 30, 255);
    inline ImVec4 bg_selectable_in = ImColor(25, 25, 25, 127);
    inline ImVec4 bg_element = ImColor(25, 25, 25, 127);
    inline ImVec4 bg_element_1 = ImColor(30, 30, 30, 255);

    // Scrollbars and separators
    inline ImVec4 bg_scrollbar = background;
    inline ImVec4 line = ImColor(30, 30, 30, 255);
    inline ImVec4 line_in_active = ImColor(0, 128, 255, 0);
    inline ImVec4 separator = ImColor(38, 38, 38, 255);

    // Misc
    inline ImVec4 black = ImColor(0, 0, 0, 255);

}