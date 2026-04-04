
find_package(wxWidgets COMPONENTS core base gl adv)
include(${wxWidgets_USE_FILE})

# GL_GLEXT_PROTOTYPES ensures that glext.h declares extension function prototypes
# (glGenVertexArrays, glUniform*, etc.) whenever it is first included, even when
# pulled in transitively via gl.h before our own opengl_header.h runs.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64 -D__WXGTK__ -DGL_GLEXT_PROTOTYPES")

set(WX_CUSTOM_INC_PATH ${REPO_DIR}/cpp/externals/wxwidgets/include)
set(WX_CUSTOM_LIB_PATH ${REPO_DIR}/cpp/externals/wxwidgets/lib)

set(PLATFORM_LIBRARIES -lstdc++fs)