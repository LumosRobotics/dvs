#pragma once
#include <iostream>
#if defined(__GLEW_H__) || defined(GLEW_H)
#else
    #include <OpenGL/gl3.h>
#endif

class IndexBuffer
{
public:
	IndexBuffer();
	~IndexBuffer();
	void createIndexBuffer(const uint32_t* data, uint32_t count);
	void Bind() const;
	void UnBind() const;
private:
	uint32_t ib_id;
	uint32_t m_count;
};
