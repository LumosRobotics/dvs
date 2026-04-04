#pragma once
#include <iostream>
#if defined(__GLEW_H__) || defined(GLEW_H)
#else
    #include <OpenGL/gl3.h>
#endif
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{
public:
	VertexArray();
	~VertexArray();
	void createVertexArray();
	void AddBuffer(const VertexBuffer2& vbo, const VertexBufferLayout& layout);
	void Bind() const;
	void UnBind() const;
private:
	uint32_t va_id;
};