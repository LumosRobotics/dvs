cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

project(main-app)

include_directories(${REPO_DIR}/src/main_application)
include_directories(${REPO_DIR}/third_party/LumosAlgo/src)

# HarfBuzz headers (macOS: vendored; Linux: resolved after pkg_check_modules below).
if(PLATFORM_APPLE)
    include_directories(${REPO_DIR}/src/externals/harfbuzz/build/include/harfbuzz)
endif()

set(CPP_SOURCE_FILES main.cpp
                     axes/axes_interactor.cpp

                     axes/axes_renderer.cpp
                     axes/plot_box_walls.cpp
                     axes/plot_box_silhouette.cpp
                     axes/plot_pane_background.cpp
                     axes/zoom_rect.cpp
                     axes/grid_numbers.cpp
                     axes/plot_box_grid.cpp
                     axes/text_rendering.cpp
                     axes/text_rendering_impl/font_atlas.cpp
                     axes/text_rendering_impl/label_text_store.cpp
                     axes/text_rendering_impl/buffers/VertexBufferLayout.cpp
                     axes/text_rendering_impl/buffers/VertexBuffer.cpp
                     axes/text_rendering_impl/buffers/VertexArray.cpp
                     axes/text_rendering_impl/buffers/IndexBuffer.cpp
                     axes/text_rendering_impl/buffers/gBuffers.cpp
                     axes/legend_renderer.cpp
                     axes/point_selection_box.cpp

                     axes/axes_side_configuration.cpp

                     project_state/project_settings.cpp
                     project_state/element_settings.cpp
                     project_state/plot_pane_settings.cpp
                     project_state/other_gui_settings.cpp

                     shader.cpp

                     settings_window.cpp

                     gui_elements.cpp
                     gui_element.cpp

                     editing_silhouette.cpp

                     cmdl_output_window.cpp

                     platform_paths.cpp

                     axes/structures/axes_limits.cpp
                     axes/structures/axes_settings.cpp
                     axes/structures/view_angles.cpp

                     misc/number_formatting.cpp

                     plot_objects/plot_object_base/plot_object_base.cpp
                     plot_objects/draw_mesh/draw_mesh.cpp
                     plot_objects/im_show/im_show.cpp
                     plot_objects/plot_collection2/plot_collection2.cpp
                     plot_objects/plot_collection3/plot_collection3.cpp
                     plot_objects/plot2d/plot2d.cpp
                     plot_objects/plot3d/plot3d.cpp
                     plot_objects/fast_plot2d/fast_plot2d.cpp
                     plot_objects/fast_plot3d/fast_plot3d.cpp
                     plot_objects/line_collection2/line_collection2.cpp
                     plot_objects/line_collection3/line_collection3.cpp
                     plot_objects/stem/stem.cpp
                     plot_objects/scatter/scatter.cpp
                     plot_objects/scatter3/scatter3.cpp
                     plot_objects/stairs/stairs.cpp
                     plot_objects/surf/surf.cpp
                     plot_objects/scrolling_plot2d/scrolling_plot2d.cpp
                     plot_objects/screen_space_primitive/screen_space_primitive.cpp

                     plot_objects/stream_object_base/stream_object_base.cpp
                     plot_objects/stream_objects/plot2d/plot2d.cpp
                     plot_objects/stream_objects/scatter/scatter.cpp
                     plot_objects/stream_objects/stairs/stairs.cpp

                     globals.cpp

                     project_state/configuration_agent.cpp

                     communication/received_data.cpp
                     communication/data_receiver.cpp
                     input_data.cpp
                     point_selection.cpp

                     tray_icon.cpp
                     tab_button.cpp
                     close_button.cpp
                     window_button.cpp
                     gui_window.cpp
                     gui_tab.cpp
                     events.cpp
                     tab_buttons.cpp
                     help_pane.cpp
                     plot_data_handler.cpp
                     plot_pane.cpp
                     color_picker.cpp
                     main_window.cpp
                     main_window_receive.cpp
                     main_window_serial.cpp

                     serial_interface/serial_port.cpp
                     serial_interface/serial_interface.cpp
                     serial_interface/raw_data_frame.cpp

                     topic_text_output_window.cpp

                     text_stream_objects/text_stream_object_base.cpp

                     project_state/scrolling_text_settings.cpp

                     user_supplied_properties.cpp

                     graphic_window.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${REPO_DIR}/src/build/)

add_executable(duoplot ${CPP_SOURCE_FILES})
# The vendored libfreetype.a is built with FT_DISABLE_BZIP2/BROTLI/ZLIB/PNG so
# only HarfBuzz needs to be linked alongside it.
if(PLATFORM_LINUX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(HARFBUZZ REQUIRED harfbuzz)
    include_directories(${HARFBUZZ_INCLUDE_DIRS})
    set(FREETYPE_EXTRA_LIBS ${HARFBUZZ_LIBRARIES})
elseif(PLATFORM_APPLE)
    set(FREETYPE_EXTRA_LIBS ${REPO_DIR}/src/externals/harfbuzz/build/lib/libharfbuzz.a)
endif()

target_link_libraries(duoplot ${wxWidgets_LIBRARIES}
                          ${OPENGL_LIBRARIES}
                          $<IF:$<TARGET_EXISTS:GLUT::GLUT>,GLUT::GLUT,${GLUT_LIBRARY}>
                          ${PLATFORM_LIBRARIES}
                          ${REPO_DIR}/src/externals/freetype/build/libfreetype.a
                          ${FREETYPE_EXTRA_LIBS})
