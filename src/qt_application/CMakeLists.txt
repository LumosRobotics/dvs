cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

project(qt-app)

# Qt-specific settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(${REPO_DIR}/src/qt_application)
include_directories(${REPO_DIR}/third_party/LumosAlgo/src)
include_directories(${REPO_DIR}/src/main_application)
include_directories(${REPO_DIR}/src/common)
include_directories(${REPO_DIR}/src/externals/glm)

# HarfBuzz headers for the text renderer.
# FreeType is built with FT_DISABLE_BZIP2/BROTLI/ZLIB/PNG so only HarfBuzz
# needs to be linked alongside it.
if(PLATFORM_LINUX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(HARFBUZZ REQUIRED harfbuzz)
    include_directories(${HARFBUZZ_INCLUDE_DIRS})
    set(HARFBUZZ_LINK_LIBS ${HARFBUZZ_LIBRARIES})
elseif(PLATFORM_APPLE)
    include_directories(${REPO_DIR}/src/externals/harfbuzz/build/include/harfbuzz)
    set(HARFBUZZ_LINK_LIBS
        ${REPO_DIR}/src/externals/harfbuzz/build/lib/libharfbuzz.a)
endif()

# LumosConfig
include_directories(${REPO_DIR}/third_party/LumosConfig)
include_directories(${REPO_DIR}/third_party/LumosConfig/src/modules)

# Source files
set(CPP_SOURCE_FILES
    # Main application files
    main.cpp
    main_window.cpp
    plot_pane.cpp
    window_tab.cpp
    gui_window.cpp
    gui_elements.cpp
    plot_data_conversion.cpp

    # Communication
    communication/data_receiver.cpp
    communication/received_data.cpp

    # Axes system
    axes/axes_interactor.cpp
    axes/axes_renderer.cpp
    axes/axes_side_configuration.cpp
    axes/grid_numbers.cpp
    axes/legend_renderer.cpp
    axes/plot_box_grid.cpp
    axes/plot_box_silhouette.cpp
    axes/plot_box_walls.cpp
    axes/plot_pane_background.cpp
    axes/point_selection_box.cpp
    axes/text_rendering.cpp
    ${REPO_DIR}/src/main_application/axes/text_rendering_impl/font_atlas.cpp
    ${REPO_DIR}/src/main_application/axes/text_rendering_impl/label_text_store.cpp
    ${REPO_DIR}/src/main_application/axes/text_rendering_impl/buffers/VertexBufferLayout.cpp
    ${REPO_DIR}/src/main_application/axes/text_rendering_impl/buffers/VertexBuffer.cpp
    ${REPO_DIR}/src/main_application/axes/text_rendering_impl/buffers/VertexArray.cpp
    ${REPO_DIR}/src/main_application/axes/text_rendering_impl/buffers/IndexBuffer.cpp
    ${REPO_DIR}/src/main_application/axes/text_rendering_impl/buffers/gBuffers.cpp
    axes/zoom_rect.cpp
    axes/structures/axes_limits.cpp
    axes/structures/axes_settings.cpp
    axes/structures/view_angles.cpp

    # Plot objects
    plot_objects/plot_object_base/plot_object_base.cpp
    plot_objects/plot2d/plot2d.cpp
    plot_objects/plot3d/plot3d.cpp
    plot_objects/fast_plot2d/fast_plot2d.cpp
    plot_objects/fast_plot3d/fast_plot3d.cpp
    plot_objects/scatter/scatter.cpp
    plot_objects/scatter3/scatter3.cpp
    plot_objects/line_collection2/line_collection2.cpp
    plot_objects/line_collection3/line_collection3.cpp
    plot_objects/plot_collection2/plot_collection2.cpp
    plot_objects/plot_collection3/plot_collection3.cpp
    plot_objects/stairs/stairs.cpp
    plot_objects/stem/stem.cpp
    plot_objects/surf/surf.cpp
    plot_objects/im_show/im_show.cpp
    plot_objects/draw_mesh/draw_mesh.cpp
    plot_objects/scrolling_plot2d/scrolling_plot2d.cpp
    plot_objects/screen_space_primitive/screen_space_primitive.cpp
    plot_objects/stream_object_base/stream_object_base.cpp
    plot_objects/stream_objects/plot2d/plot2d.cpp
    plot_objects/stream_objects/scatter/scatter.cpp
    plot_objects/stream_objects/stairs/stairs.cpp

    # Project state
    project_state/configuration_agent.cpp
    project_state/element_settings.cpp
    project_state/other_gui_settings.cpp
    project_state/plot_pane_settings.cpp
    project_state/project_settings.cpp
    project_state/scrolling_text_settings.cpp

    # Serial interface
    serial_interface/raw_data_frame.cpp
    serial_interface/serial_interface.cpp
    serial_interface/serial_port.cpp

    # Text stream objects
    text_stream_objects/text_stream_object_base.cpp

    # Utilities
    shader.cpp
    opengl_debug.cpp
    input_data.cpp
    point_selection.cpp
    plot_data_handler.cpp
    user_supplied_properties.cpp
    platform_paths.cpp
    globals.cpp
    color_picker.cpp
    misc/number_formatting.cpp

    # UI Shell (Phase 3)
    cmdl_output_window.cpp
    topic_text_output_window.cpp
    settings_window.cpp
    help_pane.cpp
    tray_icon.cpp

    # Custom tab button
    tab_button.cpp

    # Edit mode (Phase 5)
    editing_overlay.cpp

    # LumosConfig
    ${REPO_DIR}/third_party/LumosConfig/Settings.cpp
    ${REPO_DIR}/third_party/LumosConfig/src/modules/settings_handler/settings_handler.cpp
    ${REPO_DIR}/third_party/LumosConfig/src/modules/settings_handler/platform_path_macos.cpp
    ${REPO_DIR}/third_party/LumosConfig/src/modules/settings_handler/platform_path_linux.cpp
    ${REPO_DIR}/third_party/LumosConfig/src/modules/settings_handler/platform_path_windows.cpp
)

set(HEADER_FILES
    main_window.h
    plot_pane.h
    gui_element.h
    gui_elements.h
    window_tab.h
    gui_window.h
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${REPO_DIR}/src/build/)

add_executable(qt_duoplot ${CPP_SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(qt_duoplot
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::SerialPort
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARY}
    ${REPO_DIR}/src/externals/freetype/build/libfreetype.a
    ${HARFBUZZ_LINK_LIBS}
)
