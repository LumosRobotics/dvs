#pragma once
#include <iostream>
#if defined(__GLEW_H__) || defined(GLEW_H)
    // Nothing — glew already provides all GL declarations.
#else
    #include <OpenGL/gl3.h>
#endif

#include <vector>
#include <assert.h>
// Thanks to The Cherno

// This struct defines a single element in the vertex buffer layout
struct  VertexBufferElement
{
public:

	uint32_t type;          // The data type of the element (e.g. GL_FLOAT)
	uint32_t count;         // The number of elements of this type (e.g. 3 for a 3-component vector)
	unsigned char normalized;   // Whether the dstruct VertexBufferLayout::VertexBufferElement

	static uint32_t GetSizeOfType(uint32_t type)
	{
		//This function returns the size of a single element of this type in bytes
		switch (type)
		{
		case GL_FLOAT:          return 4;
		case GL_UNSIGNED_INT:   return 4;
		case GL_UNSIGNED_BYTE:  return 1;
		}
		assert(false);
		return 0;
	}
private:

};

class VertexBufferLayout
{
public:
	VertexBufferLayout();

	// Specialized version of the Push() function for adding elements of type float
	void AddFloat(const uint32_t count);

	// Specialized version of the Push() function for adding elements of type uint32_t
	void AddUnsignedInt(uint32_t count);

	// Specialized version of the Push() function for adding elements of type unsigned char
	void AddUnsignedChar(uint32_t count);

	// Getter function that returns the vector of vertex buffer elements
	inline const  std::vector<VertexBufferElement> GetElements() const { return m_Elements; }

	// Getter function that returns the stride of the vertex buffer layout in bytes
	inline  uint32_t GetStride() const { return m_Stride; }

private:
	std::vector<VertexBufferElement> m_Elements;  // The vector of vertex buffer elements
	uint32_t m_Stride;                        // The stride of the vertex buffer layout in bytes
	void Push(uint32_t type, uint32_t count, unsigned char normalized);
};