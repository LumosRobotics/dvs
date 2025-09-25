import numpy as np
from typing import List, Union, Tuple
import psutil
import os

import enums
import internal

import properties
from structures import *
from item_id import ItemId


def plot(
    x: Union[np.array, List, Tuple],
    y: Union[np.array, List, Tuple],
    **props_kw,
):
    x = internal.clean_input_vec(x)
    y = internal.clean_input_vec(y)

    if x.shape != y.shape:
        raise Exception(
            "x and y must have same shape! Shapes are x: {x.shape}, y: {y.shape}"
        )

    hdr = internal.CommunicationHeader(enums.Function.PLOT2)
    hdr.append(enums.CommunicationHeaderObjectType.NUM_ELEMENTS, x.size)

    hdr.append(
        enums.CommunicationHeaderObjectType.DATA_TYPE,
        internal.np_data_type_to_data_type(x[0].__class__),
    )

    hdr.append_properties(props_kw)

    internal.send_header_and_data(internal.send_with_tcp, hdr, x, y)


def plot3(
    x: Union[np.array, List, Tuple],
    y: Union[np.array, List, Tuple],
    z: Union[np.array, List, Tuple],
    **props_kw,
):
    x = internal.clean_input_vec(x)
    y = internal.clean_input_vec(y)
    z = internal.clean_input_vec(z)

    if x.shape != y.shape or x.shape != z.shape:
        raise Exception(
            "x and y must have same shape! Shapes are x: {x.shape}, y: {y.shape}, z: {z.shape}"
        )

    hdr = internal.CommunicationHeader(enums.Function.PLOT3)
    hdr.append(enums.CommunicationHeaderObjectType.NUM_ELEMENTS, x.size)

    hdr.append(
        enums.CommunicationHeaderObjectType.DATA_TYPE,
        internal.np_data_type_to_data_type(x[0].__class__),
    )

    hdr.append_properties(props_kw)

    internal.send_header_and_data(internal.send_with_tcp, hdr, x, y, z)


def scatter(
    x: Union[np.array, List, Tuple], y: Union[np.array, List, Tuple], **props_kw
):
    x = internal.clean_input_vec(x)
    y = internal.clean_input_vec(y)

    if x.shape != y.shape:
        raise Exception(
            "x and y must have same shape! Shapes are x: {x.shape}, y: {y.shape}"
        )

    hdr = internal.CommunicationHeader(enums.Function.SCATTER2)
    hdr.append(enums.CommunicationHeaderObjectType.NUM_ELEMENTS, x.size)

    hdr.append(
        enums.CommunicationHeaderObjectType.DATA_TYPE,
        internal.np_data_type_to_data_type(x[0].__class__),
    )

    hdr.append_properties(props_kw)

    internal.send_header_and_data(internal.send_with_tcp, hdr, x, y)


def scatter3(
    x: Union[np.array, List, Tuple],
    y: Union[np.array, List, Tuple],
    z: Union[np.array, List, Tuple],
    **props_kw,
):
    x = internal.clean_input_vec(x)
    y = internal.clean_input_vec(y)
    z = internal.clean_input_vec(z)

    if x.shape != y.shape or x.shape != z.shape:
        raise Exception(
            "x and y must have same shape! Shapes are x: {x.shape}, y: {y.shape}, z: {z.shape}"
        )

    hdr = internal.CommunicationHeader(enums.Function.SCATTER3)
    hdr.append(enums.CommunicationHeaderObjectType.NUM_ELEMENTS, x.size)

    if len(x.shape) == 2:
        hdr.append(
            enums.CommunicationHeaderObjectType.DATA_TYPE,
            internal.np_data_type_to_data_type(x[0][0].__class__),
        )
    else:
        hdr.append(
            enums.CommunicationHeaderObjectType.DATA_TYPE,
            internal.np_data_type_to_data_type(x[0].__class__),
        )

    new_props = {}

    colors = None

    for itm in props_kw.items():
        if itm[0] == "colors":
            colors = itm[1]
            hdr.append(enums.CommunicationHeaderObjectType.HAS_COLOR, 1)
        else:
            new_props[itm[0]] = itm[1]

    hdr.append_properties(new_props)

    if colors is None:
        internal.send_header_and_data(internal.send_with_tcp, hdr, x, y, z)
    else:
        internal.send_header_and_data(internal.send_with_tcp, hdr, x, y, z, colors)


def surf(
    x: Union[np.array, List[List], Tuple[Tuple]],
    y: Union[np.array, List[List], Tuple[Tuple]],
    z: Union[np.array, List[List], Tuple[Tuple]],
    **props_kw,
):
    x = internal.clean_surf_input(x)
    y = internal.clean_surf_input(y)
    z = internal.clean_surf_input(z)

    if x.shape != y.shape or x.shape != z.shape:
        raise Exception("x, y, and z must have same shape!")

    if x.shape[0] == 1 or x.shape[1] == 1:
        Exception(
            f"Invalid shape for input vector: {x.shape}! Both dimensions must be greater than 1."
        )

    hdr = internal.CommunicationHeader(enums.Function.SURF)
    hdr.append(enums.CommunicationHeaderObjectType.NUM_ELEMENTS, x.size)
    hdr.append(enums.CommunicationHeaderObjectType.DIMENSION_2D, x.shape)

    hdr.append(
        enums.CommunicationHeaderObjectType.DATA_TYPE,
        internal.np_data_type_to_data_type(x[0][0].__class__),
    )

    hdr.append_properties(props_kw)

    internal.send_header_and_data(internal.send_with_tcp, hdr, x, y, z)


def imshow(img: np.array, **props_kw):
    hdr = internal.CommunicationHeader(enums.Function.IM_SHOW)

    if len(img.shape) == 2:
        hdr.append(enums.CommunicationHeaderObjectType.NUM_CHANNELS, 1)
        hdr.append(enums.CommunicationHeaderObjectType.DIMENSION_2D, img.shape)
    else:
        hdr.append(enums.CommunicationHeaderObjectType.NUM_CHANNELS, 3)
        hdr.append(enums.CommunicationHeaderObjectType.DIMENSION_2D, img[0].shape)
    hdr.append(enums.CommunicationHeaderObjectType.NUM_ELEMENTS, img.size)

    hdr.append(
        enums.CommunicationHeaderObjectType.DATA_TYPE,
        internal.np_data_type_to_data_type(img[0][0][0].__class__),
    )

    hdr.append_properties(props_kw)

    internal.send_header_and_data(internal.send_with_tcp, hdr, img)


def draw_mesh(
    vertices: Union[List[Point3D], np.ndarray],
    indices: Union[List[IndexTriplet], np.ndarray],
    **props_kw,
):
    if isinstance(vertices, np.ndarray):
        if vertices.shape[0] == 3:
            pass
        elif vertices.shape[1] == 3:
            vertices = vertices.T
        elif vertices.shape == (3, 3):
            print("Warning, vertices has ambiguous shape (3, 3), assuming (3, N)")
        else:
            raise Exception(
                f"Invalid shape for vertices: {vertices.shape}! Must be (3, N) or (N, 3)."
            )
    elif isinstance(vertices, list):
        vertices = np.array([[v.x, v.y, v.z] for v in vertices]).T

    if isinstance(indices, list):
        indices = np.array([[i.i0, i.i1, i.i2] for i in indices]).T
    elif isinstance(indices, np.ndarray):
        if indices.shape[0] == 3:
            pass
        elif indices.shape[1] == 3:
            indices = indices.T
        elif indices.shape == (3, 3):
            print("Warning, indices has ambiguous shape (3, 3), assuming (3, N)")
        else:
            raise Exception(
                f"Invalid shape for indices: {indices.shape}! Must be (3, N) or (N, 3)."
            )

    if indices[0][0].__class__ != np.uint32:
        indices = indices.astype(np.uint32)

    hdr = internal.CommunicationHeader(enums.Function.DRAW_MESH_SEPARATE_VECTORS)
    hdr.append(enums.CommunicationHeaderObjectType.NUM_ELEMENTS, indices.shape[1])

    hdr.append(enums.CommunicationHeaderObjectType.NUM_VERTICES, vertices.shape[1])
    hdr.append(enums.CommunicationHeaderObjectType.NUM_INDICES, indices.shape[1])

    hdr.append(
        enums.CommunicationHeaderObjectType.DATA_TYPE,
        internal.np_data_type_to_data_type(vertices[0][0].__class__),
    )

    new_props = {}

    colors = None

    for itm in props_kw.items():
        if itm[0] == "colors":
            colors = itm[1]
            hdr.append(enums.CommunicationHeaderObjectType.HAS_COLOR, 1)
        else:
            new_props[itm[0]] = itm[1]

    hdr.append_properties(new_props)

    if colors is None:
        internal.send_header_and_data(
            internal.send_with_tcp,
            hdr,
            vertices[0, :],
            vertices[1, :],
            vertices[2, :],
            indices.T.flatten(),
        )
    else:
        internal.send_header_and_data(
            internal.send_with_tcp,
            hdr,
            vertices[0, :],
            vertices[1, :],
            vertices[2, :],
            indices.T.flatten(),
            colors.flatten(),
        )


def line_collection(
    x: Union[np.array, List, Tuple],
    y: Union[np.array, List, Tuple],
    **props_kw,
):
    x = internal.clean_input_vec(x)
    y = internal.clean_input_vec(y)

    if x.shape != y.shape:
        raise Exception(
            "x and y must have same shape! Shapes are x: {x.shape}, y: {y.shape}"
        )

    hdr = internal.CommunicationHeader(enums.Function.LINE_COLLECTION2)
    hdr.append(enums.CommunicationHeaderObjectType.NUM_ELEMENTS, x.size)

    hdr.append(
        enums.CommunicationHeaderObjectType.DATA_TYPE,
        internal.np_data_type_to_data_type(x[0].__class__),
    )

    hdr.append_properties(props_kw)

    internal.send_header_and_data(internal.send_with_tcp, hdr, x, y)


def line_collection3(
    x: Union[np.array, List, Tuple],
    y: Union[np.array, List, Tuple],
    z: Union[np.array, List, Tuple],
    **props_kw,
):
    x = internal.clean_input_vec(x)
    y = internal.clean_input_vec(y)
    z = internal.clean_input_vec(z)

    if x.shape != y.shape or x.shape != z.shape:
        raise Exception(
            "x, y and z must have same shape! Shapes are x: {x.shape}, y: {y.shape}, z: {z.shape}"
        )

    hdr = internal.CommunicationHeader(enums.Function.LINE_COLLECTION3)
    hdr.append(enums.CommunicationHeaderObjectType.NUM_ELEMENTS, x.size)

    hdr.append(
        enums.CommunicationHeaderObjectType.DATA_TYPE,
        internal.np_data_type_to_data_type(x[0].__class__),
    )

    hdr.append_properties(props_kw)

    internal.send_header_and_data(internal.send_with_tcp, hdr, x, y, z)


def plot_collection(
    x: List[Union[List, np.ndarray]],
    y: List[Union[List, np.ndarray]],
    **props_kw,
):
    if len(x) != len(y):
        raise Exception(
            f"x and y must have same length! Lengths are x: {len(x)}, y: {len(y)}"
        )
    if not isinstance(x, list) or not isinstance(y, list):
        raise Exception("x and y must be lists!")

    x_new = []
    y_new = []

    num_elements = 0
    vector_lengths = []

    if isinstance(x[0], list):
        first_dt = np.array([x[0][0]])[0].__class__
    elif isinstance(x[0], np.ndarray):
        if len(x[0].shape) == 2:
            first_dt = x[0][0][0].__class__
        else:
            first_dt = x[0][0].__class__

    num_bytes_to_send = 0

    for x_e, y_e in zip(x, y):
        if isinstance(x_e, list):
            x_res = np.array(x_e)
        elif isinstance(x_e, np.ndarray):
            x_res = x_e
        else:
            raise Exception(
                f"x must be a list of lists or a list of numpy arrays! Type is {type(x_e)}"
            )

        if isinstance(y_e, list):
            y_res = np.array(y_e)
        elif isinstance(y_e, np.ndarray):
            y_res = y_e
        else:
            raise Exception(
                f"y must be a list of lists or a list of numpy arrays! Type is {type(y_e)}"
            )

        if x_res.shape != y_res.shape:
            raise Exception(
                f"x and y must have same shape! Shapes are x: {x_res.shape}, y: {y_res.shape}"
            )
        if len(x_res.shape) == 2:
            if x_res.shape[0] != 1 and x_res.shape[1] != 1:
                raise Exception("x must a vector!")
            else:
                x_res = x_res.flatten()

        if len(y_res.shape) == 2:
            if y_res.shape[0] != 1 and y_res.shape[1] != 1:
                raise Exception("y must be a vector!")
            else:
                y_res = y_res.flatten()

        x_new.append(x_res)
        y_new.append(y_res)

        num_elements += x_res.size
        vector_lengths.append(x_res.size)

        current_dt_x = x_res[0].__class__
        current_dt_y = y_res[0].__class__
        if current_dt_x != first_dt or current_dt_y != first_dt:
            raise Exception(
                f"All elements in x must have same data type! Data types are {first_dt} and {current_dt_x}, {current_dt_y}"
            )

        num_elements += x_res.size

    num_bytes_to_send += num_elements * 2 * internal.np_data_type_to_size(first_dt)

    hdr = internal.CommunicationHeader(enums.Function.PLOT_COLLECTION2)
    hdr.append(enums.CommunicationHeaderObjectType.NUM_ELEMENTS, num_elements)
    hdr.append(
        enums.CommunicationHeaderObjectType.DATA_TYPE,
        internal.np_data_type_to_data_type(first_dt),
    )
    hdr.append(enums.CommunicationHeaderObjectType.NUM_OBJECTS, len(x))

    hdr.append_properties(props_kw)

    internal.send_header_and_vector_collection(
        internal.send_with_tcp,
        hdr,
        num_bytes_to_send,
        vector_lengths,
        np.concatenate(x_new, axis=0),
        np.concatenate(y_new, axis=0),
    )


def plot_collection3(
    x: List[Union[List, np.ndarray]],
    y: List[Union[List, np.ndarray]],
    z: List[Union[List, np.ndarray]],
    **props_kw,
):
    if len(x) != len(y) or len(x) != len(z):
        raise Exception(
            f"x, y and z must have same length! Lengths are x: {len(x)}, y: {len(y)}, z: {len(z)}"
        )
    if not isinstance(x, list) or not isinstance(y, list) or not isinstance(z, list):
        raise Exception("x, y and z must be lists!")

    x_new = []
    y_new = []
    z_new = []

    num_elements = 0
    vector_lengths = []

    if isinstance(x[0], list):
        first_dt = np.array([x[0][0]])[0].__class__
    elif isinstance(x[0], np.ndarray):
        if len(x[0].shape) == 2:
            first_dt = x[0][0][0].__class__
        else:
            first_dt = x[0][0].__class__

    num_bytes_to_send = 0

    for x_e, y_e, z_e in zip(x, y, z):
        if isinstance(x_e, list):
            x_res = np.array(x_e)
        elif isinstance(x_e, np.ndarray):
            x_res = x_e
        else:
            raise Exception(
                f"x must be a list of lists or a list of numpy arrays! Type is {type(x_e)}"
            )

        if isinstance(y_e, list):
            y_res = np.array(y_e)
        elif isinstance(y_e, np.ndarray):
            y_res = y_e
        else:
            raise Exception(
                f"y must be a list of lists or a list of numpy arrays! Type is {type(y_e)}"
            )

        if isinstance(z_e, list):
            z_res = np.array(z_e)
        elif isinstance(z_e, np.ndarray):
            z_res = z_e
        else:
            raise Exception(
                f"z must be a list of lists or a list of numpy arrays! Type is {type(y_e)}"
            )

        if x_res.shape != y_res.shape or x_res.shape != z_res.shape:
            raise Exception(
                f"x, y and must have same shape! Shapes are x: {x_res.shape}, y: {y_res.shape}, z: {z_res.shape}"
            )
        if len(x_res.shape) == 2:
            if x_res.shape[0] != 1 and x_res.shape[1] != 1:
                raise Exception("x must a vector!")
            else:
                x_res = x_res.flatten()

        if len(y_res.shape) == 2:
            if y_res.shape[0] != 1 and y_res.shape[1] != 1:
                raise Exception("y must be a vector!")
            else:
                y_res = y_res.flatten()

        if len(z_res.shape) == 2:
            if z_res.shape[0] != 1 and z_res.shape[1] != 1:
                raise Exception("z must be a vector!")
            else:
                z_res = z_res.flatten()

        x_new.append(x_res)
        y_new.append(y_res)
        z_new.append(z_res)

        num_elements += x_res.size
        vector_lengths.append(x_res.size)

        current_dt_x = x_res[0].__class__
        current_dt_y = y_res[0].__class__
        current_dt_z = z_res[0].__class__
        if (
            current_dt_x != first_dt
            or current_dt_y != first_dt
            or current_dt_z != first_dt
        ):
            raise Exception(
                f"All elements in x, y and z must have same data type! First data type in x was {first_dt}, and current x: {current_dt_x}, y: {current_dt_y}, z: {current_dt_z}"
            )

        num_elements += x_res.size

    num_bytes_to_send += num_elements * 3 * internal.np_data_type_to_size(first_dt)

    hdr = internal.CommunicationHeader(enums.Function.PLOT_COLLECTION3)
    hdr.append(enums.CommunicationHeaderObjectType.NUM_ELEMENTS, num_elements)
    hdr.append(
        enums.CommunicationHeaderObjectType.DATA_TYPE,
        internal.np_data_type_to_data_type(first_dt),
    )
    hdr.append(enums.CommunicationHeaderObjectType.NUM_OBJECTS, len(x))

    hdr.append_properties(props_kw)

    internal.send_header_and_vector_collection(
        internal.send_with_tcp,
        hdr,
        num_bytes_to_send,
        vector_lengths,
        np.concatenate(x_new, axis=0),
        np.concatenate(y_new, axis=0),
        np.concatenate(z_new, axis=0),
    )


def stairs(
    x: Union[np.array, List, Tuple],
    y: Union[np.array, List, Tuple],
    **props_kw,
):
    x = internal.clean_input_vec(x)
    y = internal.clean_input_vec(y)

    if x.shape != y.shape:
        raise Exception(
            "x and y must have same shape! Shapes are x: {x.shape}, y: {y.shape}"
        )

    hdr = internal.CommunicationHeader(enums.Function.STAIRS)
    hdr.append(enums.CommunicationHeaderObjectType.NUM_ELEMENTS, x.size)

    hdr.append(
        enums.CommunicationHeaderObjectType.DATA_TYPE,
        internal.np_data_type_to_data_type(x[0].__class__),
    )

    hdr.append_properties(props_kw)

    internal.send_header_and_data(internal.send_with_tcp, hdr, x, y)


def stem(
    x: Union[np.array, List, Tuple],
    y: Union[np.array, List, Tuple],
    **props_kw,
):
    x = internal.clean_input_vec(x)
    y = internal.clean_input_vec(y)

    if x.shape != y.shape:
        raise Exception(
            "x and y must have same shape! Shapes are x: {x.shape}, y: {y.shape}"
        )

    hdr = internal.CommunicationHeader(enums.Function.STEM)
    hdr.append(enums.CommunicationHeaderObjectType.NUM_ELEMENTS, x.size)

    hdr.append(
        enums.CommunicationHeaderObjectType.DATA_TYPE,
        internal.np_data_type_to_data_type(x[0].__class__),
    )

    hdr.append_properties(props_kw)

    internal.send_header_and_data(internal.send_with_tcp, hdr, x, y)


def draw_point(
    pt: Union[np.array, List, Tuple, Point2D, Point3D, Vec2D, Vec3D],
    **props_kw,
):
    if isinstance(pt, Point2D) or isinstance(pt, Vec2D):
        x = pt.x
        y = pt.y
        z = 0
    elif isinstance(pt, Point3D) or isinstance(pt, Vec3D):
        x = pt.x
        y = pt.y
        z = pt.z
    elif isinstance(pt, list) or isinstance(pt, tuple):
        pass


def axis(
    min_vec: Union[Vec2D, Vec3D, List, np.ndarray, Tuple],
    max_vec: Union[Vec2D, Vec3D, List, np.ndarray, Tuple],
):
    min_vec = internal.ensure_vec3d(min_vec, -1.0)
    max_vec = internal.ensure_vec3d(max_vec, 1.0)

    if min_vec.x >= max_vec.x:
        raise Exception("Invalid axis! x min >= x max")
    if min_vec.y >= max_vec.y:
        raise Exception("Invalid axis! y min >= y max")
    if min_vec.z >= max_vec.z:
        raise Exception("Invalid axis! z min >= z max")

    hdr = internal.CommunicationHeader(enums.Function.AXES_3D)
    hdr.append(enums.CommunicationHeaderObjectType.AXIS_MIN_MAX_VEC, (min_vec, max_vec))
    internal.send_header(internal.send_with_tcp, hdr)


def view(azimuth_deg, elevation_deg):
    hdr = internal.CommunicationHeader(enums.Function.VIEW)
    hdr.append(enums.CommunicationHeaderObjectType.AZIMUTH, np.float32(azimuth_deg))
    hdr.append(enums.CommunicationHeaderObjectType.ELEVATION, np.float32(elevation_deg))
    internal.send_header(internal.send_with_tcp, hdr)


def disable_scale_on_rotation():
    hdr = internal.CommunicationHeader(enums.Function.DISABLE_SCALE_ON_ROTATION)
    internal.send_header(internal.send_with_tcp, hdr)


def soft_clear_view():
    hdr = internal.CommunicationHeader(enums.Function.SOFT_CLEAR)
    internal.send_header(internal.send_with_tcp, hdr)


def clear_view():
    hdr = internal.CommunicationHeader(enums.Function.CLEAR)
    internal.send_header(internal.send_with_tcp, hdr)


def set_current_element(name: str):
    if not isinstance(name, str):
        raise Exception("Name must be a string!")

    hdr = internal.CommunicationHeader(enums.Function.SET_CURRENT_ELEMENT)
    hdr.append(enums.CommunicationHeaderObjectType.ELEMENT_NAME, name)
    internal.send_header(internal.send_with_tcp, hdr)


def set_properties(itm_id: ItemId, **props_kw):
    hdr = internal.CommunicationHeader(enums.Function.PROPERTIES_EXTENSION)
    hdr.append(enums.CommunicationHeaderObjectType.ITEM_ID, itm_id)
    hdr.append_properties(props_kw)
    internal.send_header(internal.send_with_tcp, hdr)


def delete_plot_object(itm_id: ItemId):
    hdr = internal.CommunicationHeader(enums.Function.DELETE_PLOT_OBJECT)
    hdr.append(enums.CommunicationHeaderObjectType.ITEM_ID, itm_id)
    internal.send_header(internal.send_with_tcp, hdr)


def set_current_element_to_image_view():
    hdr = internal.CommunicationHeader(enums.Function.CURRENT_ELEMENT_AS_IMAGE_VIEW)
    internal.send_header(internal.send_with_tcp, hdr)


def wait_for_flush():
    hdr = internal.CommunicationHeader(enums.Function.WAIT_FOR_FLUSH)
    internal.send_header(internal.send_with_tcp, hdr)


def flush_current_element():
    hdr = internal.CommunicationHeader(enums.Function.FLUSH_ELEMENT)
    internal.send_header(internal.send_with_tcp, hdr)


def flush_multiple_elements(*args):
    hdr = internal.CommunicationHeader(enums.Function.FLUSH_MULTIPLE_ELEMENTS)

    name_lengths = np.zeros(len(args), dtype=np.uint8)
    concatenated_names = ""

    for idx, arg in enumerate(args):
        if not isinstance(arg, str):
            raise Exception("All arguments must be strings!")

        name_lengths[idx] = len(arg)
        concatenated_names += arg

    hdr.append(enums.CommunicationHeaderObjectType.NUM_NAMES, len(args))
    internal.send_header_and_data(
        internal.send_with_tcp, hdr, name_lengths, concatenated_names
    )


def global_illumination(light_position: Vec3D):
    hdr = internal.CommunicationHeader(enums.Function.GLOBAL_ILLUMINATION)
    hdr.append(enums.CommunicationHeaderObjectType.VEC3, light_position)
    internal.send_header(internal.send_with_tcp, hdr)


def show_legend():
    hdr = internal.CommunicationHeader(enums.Function.SHOW_LEGEND)
    internal.send_header(internal.send_with_tcp, hdr)


def disable_automatic_axes_adjustment():
    hdr = internal.CommunicationHeader(enums.Function.DISABLE_AXES_FROM_MIN_MAX)
    internal.send_header(internal.send_with_tcp, hdr)


def axes_square():
    hdr = internal.CommunicationHeader(enums.Function.AXES_SQUARE)
    internal.send_header(internal.send_with_tcp, hdr)


def set_axes_box_scale_factor(scale_vector: Vec3D):
    hdr = internal.CommunicationHeader(enums.Function.SET_AXES_BOX_SCALE_FACTOR)
    hdr.append(enums.CommunicationHeaderObjectType.VEC3, scale_vector)
    internal.send_header(internal.send_with_tcp, hdr)


def set_title(title: str):
    if not isinstance(title, str):
        raise Exception("Title must be a string!")
    elif len(title) == 0:
        raise Exception("Title must have length greater than 0!")

    hdr = internal.CommunicationHeader(enums.Function.SET_TITLE)
    hdr.append(enums.CommunicationHeaderObjectType.TITLE_STRING, title)
    internal.send_header(internal.send_with_tcp, hdr)


def set_transform(
    itm_id: ItemId,
    scale_matrix: np.ndarray,
    rotation_matrix: np.ndarray,
    translation_vec: Union[np.ndarray, Vec3D],
):
    if not isinstance(itm_id, ItemId):
        raise Exception("Item ID must be an ItemId!")

    if isinstance(translation_vec, Vec3D):
        translation_vec = translation_vec
    elif not isinstance(translation_vec, np.ndarray):
        if translation_vec.shape == (3,):
            translation_vec = Vec3D(
                translation_vec[0], translation_vec[1], translation_vec[2]
            )
        elif translation_vec.shape == (1, 3):
            translation_vec = Vec3D(
                translation_vec[0][0], translation_vec[0][1], translation_vec[0][2]
            )
        elif translation_vec.shape == (3, 1):
            translation_vec = Vec3D(
                translation_vec[0][0], translation_vec[1][0], translation_vec[2][0]
            )
        else:
            raise Exception("Translation vector must be a Vec3D or a numpy array!")
    else:
        raise Exception("Translation vector must be a Vec3D or a numpy array!")

    if not scale_matrix.shape == (3, 3):
        raise Exception("Scale matrix must be a 3x3 matrix!")
    if not rotation_matrix.shape == (3, 3):
        raise Exception("Rotation matrix must be a 3x3 matrix!")

    hdr = internal.CommunicationHeader(enums.Function.SET_OBJECT_TRANSFORM)
    hdr.append(enums.CommunicationHeaderObjectType.ROTATION_MATRIX, rotation_matrix)
    hdr.append(enums.CommunicationHeaderObjectType.TRANSLATION_VECTOR, translation_vec)
    hdr.append(enums.CommunicationHeaderObjectType.SCALE_MATRIX, scale_matrix)
    hdr.append(enums.CommunicationHeaderObjectType.ITEM_ID, itm_id)
    internal.send_header(internal.send_with_tcp, hdr)


def open_project_file(file_path: str):
    if not isinstance(file_path, str):
        raise Exception("File path must be a string!")
    elif len(file_path) == 0:
        raise Exception("File path must have length greater than 0!")

    hdr = internal.CommunicationHeader(enums.Function.OPEN_PROJECT_FILE)
    hdr.append(enums.CommunicationHeaderObjectType.PROJECT_FILE_NAME, file_path)
    internal.send_header(internal.send_with_tcp, hdr)


def spawn():
    # TODO: This function does not work...
    return
    if "duoplot" not in [p.name() for p in psutil.process_iter()]:
        os.spawnl(
            os.P_NOWAIT,
            "/Users/danielpi/work/dvs/src/build/main_application/duoplot",
            "/Users/danielpi/work/dvs/src/build/",
        )
