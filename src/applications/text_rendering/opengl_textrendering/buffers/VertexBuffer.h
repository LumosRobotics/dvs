#pragma once
#include <iostream>
// Include GLEW if it has been included by the translation unit; otherwise fall
// back to the plain OpenGL 3 core-profile header.  Both expose the same GL
// API but cannot coexist in the same translation unit on macOS (Apple's gl3.h
// emits a #warning when gl.h is already present, causing glew to refuse to
// compile).  By deferring to whichever header was already pulled in we avoid
// the conflict without requiring every caller to change its include order.
#if defined(__GLEW_H__) || defined(GLEW_H)
    // Nothing — glew already provides all GL declarations.
#else
    #include <OpenGL/gl3.h>
#endif


class VertexBuffer2
{
public:
	VertexBuffer2();
	~VertexBuffer2();
	void createVertexBuffer(const void* const data, const uint32_t size);
	void Bind() const;
	void UnBind() const;
private:
	uint32_t vb_id;
};
