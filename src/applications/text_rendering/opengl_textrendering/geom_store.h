#pragma once
// shader.h includes <GL/glew.h>; it must come before gBuffers.h which would
// otherwise pull in <OpenGL/gl3.h> and trigger glew's "gltypes.h before glew"
// compile error on macOS.
#include "shaders/shader.h"
#include "buffers/gBuffers.h"
#include "label_text_store.h"

class geom_store
{
public:

	geom_store();
	~geom_store();
	void set_geometry();
	void paint_geometry();
private:
	void set_simple_triangle(float* vertices, uint32_t& vertices_count,
		uint32_t* indices, uint32_t& indices_count);
	gBuffers tri_buffers;
	label_text_store all_labels;

	shader tri_shader;
	shader text_shader;
};
