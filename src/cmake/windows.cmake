# Windows cross-compilation via MinGW-w64.
# Only test targets (src/test) are supported on Windows for now —
# no wxWidgets, OpenGL, GLUT, or Qt required.

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601")

# MinGW links stdc++fs automatically; no extra platform libs needed.
set(PLATFORM_LIBRARIES "")
