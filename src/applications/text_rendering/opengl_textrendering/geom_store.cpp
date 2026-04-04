#include "geom_store.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

geom_store::geom_store()
{
}

geom_store::~geom_store()
{
}

void geom_store::set_geometry()
{
	// Set the geometry
	uint32_t node_vertices_count = 6 * 3;
	float* node_vertices = new float[node_vertices_count];

	uint32_t node_indices_count = 3;
	uint32_t* node_indices = new uint32_t[node_indices_count];

	uint32_t node_vertex_index = 0;
	uint32_t node_indices_index = 0;

	set_simple_triangle(node_vertices, node_vertex_index, node_indices, node_indices_index);

	VertexBufferLayout vb_tri;
	vb_tri.AddFloat(3);
	vb_tri.AddFloat(3);

	// Create the buffers
	uint32_t node_vertices_size = node_vertices_count * sizeof(float);
	tri_buffers.CreateBuffers(node_vertices, node_vertices_size, node_indices, node_indices_count, vb_tri);

	// Create the shader
	tri_shader.create_shader("/Users/danielpi/work/dvs/src/applications/text_rendering/opengl_textrendering/shaders/basic_vert_shader.vert", "/Users/danielpi/work/dvs/src/applications/text_rendering/opengl_textrendering/shaders/basic_frag_shader.frag");

	// Shader for the text
	text_shader.create_shader("/Users/danielpi/work/dvs/src/applications/text_rendering/opengl_textrendering/shaders/text_vert_shader.vert", "/Users/danielpi/work/dvs/src/applications/text_rendering/opengl_textrendering/shaders/text_frag_shader.frag");
	text_shader.setUniform("zoomscale", 1.0f);
	text_shader.setUniform("u_Texture", 0);

	// Initialize labels — mix of plain ASCII, accented Latin, Greek, and maths.
	all_labels.init("/Users/danielpi/work/dvs/src/resources/fonts/Roboto-Regular.ttf");

	// Plain ASCII
	all_labels.add_text("Hello, HarfBuzz!", glm::vec2(-0.85f, 0.15f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.0f, 0.0003f);

	// Accented Latin characters
	all_labels.add_text("café, naïve, résumé, jalapeño", glm::vec2(-0.85f, 0.55f), glm::vec3(0.4f, 1.0f, 0.5f), 1.0f, 0.0f, 0.00025f);

	// More accented / extended Latin
	all_labels.add_text("Ångström, Zürich, São Paulo", glm::vec2(-0.85f, 0.35f), glm::vec3(0.3f, 0.8f, 1.0f), 1.0f, 0.0f, 0.00025f);

	// Greek letters
	all_labels.add_text("α β γ δ ε ζ η θ λ μ π σ φ ψ ω", glm::vec2(-0.85f, 0.35f), glm::vec3(1.0f, 0.85f, 0.2f), 1.0f, 0.0f, 0.00095f);

	// Mathematical symbols and operators
	all_labels.add_text("E = mc²,  ∑ xᵢ,  ∫ f(x) dx", glm::vec2(-0.85f, -0.05f), glm::vec3(1.0f, 0.5f, 0.8f), 1.0f, 0.0f, 0.00025f);

	// Inequalities, arrows, and set notation
	all_labels.add_text("x ≤ y ≥ z,  a ≠ b,  ∞,  ±3.14", glm::vec2(-0.85f, -0.25f), glm::vec3(0.8f, 0.6f, 1.0f), 1.0f, 0.0f, 0.00025f);

	// Currency symbols
	all_labels.add_text("€ 12.50,  £ 9.99,  ¥ 1200,  ¢ 50", glm::vec2(-0.85f, -0.45f), glm::vec3(1.0f, 0.75f, 0.3f), 1.0f, 0.0f, 0.00025f);

	// Ligatures (fi, fl, ff) — HarfBuzz substitutes these automatically
	all_labels.add_text("office, fluffy, efficient, affirm", glm::vec2(-0.85f, -0.65f), glm::vec3(0.7f, 0.9f, 0.7f), 1.0f, 0.0f, 0.00025f);

	all_labels.set_buffers();
}


void geom_store::set_simple_triangle(float* vertices, uint32_t& vertices_index,
	uint32_t* indices, uint32_t& indices_index)
{

	// Point 1
	vertices[vertices_index + 0] = -0.5;
	vertices[vertices_index + 1] = -0.5;
	vertices[vertices_index + 2] = 0.0f;

	vertices[vertices_index + 3] = 1.0f;
	vertices[vertices_index + 4] = 1.0f;
	vertices[vertices_index + 5] = 1.0f;

	vertices_index = vertices_index + 6;

	// Point 2
	vertices[vertices_index + 0] = 0.5;
	vertices[vertices_index + 1] = -0.5;
	vertices[vertices_index + 2] = 0.0f;

	vertices[vertices_index + 3] = 1.0f;
	vertices[vertices_index + 4] = 1.0f;
	vertices[vertices_index + 5] = 1.0f;

	vertices_index = vertices_index + 6;

	// Point 3
	vertices[vertices_index + 0] = 0.0;
	vertices[vertices_index + 1] = 0.5;
	vertices[vertices_index + 2] = 0.0f;

	vertices[vertices_index + 3] = 1.0f;
	vertices[vertices_index + 4] = 1.0f;
	vertices[vertices_index + 5] = 1.0f;

	vertices_index = vertices_index + 6;


	// Set the indices
	indices[indices_index + 0] = 0;
	indices[indices_index + 1] = 1;
	indices[indices_index + 2] = 2;
}

void geom_store::paint_geometry()
{
	// Paint the Triangle
	//tri_shader.Bind();
	//tri_buffers.Bind();

	//// glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

	//tri_buffers.UnBind();
	//tri_shader.UnBind();


	// Paint the Text
	text_shader.Bind();
	all_labels.paint_text();
	 text_shader.UnBind();
}


