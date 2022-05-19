#include "simple_model.h"
#include "rgba_image.h"
#include "three_context.h"
#include "internal/GeometryCreator.h"

#include "wrapper_utils.hpp"

namespace blink {	
	struct ModelConst
	{
		glm::mat4 ModelMat;
		glm::mat4 NormalMat;
	};

	SimpleModel::SimpleModel()
	{
		m_constant = std::unique_ptr<three::GLBuffer>(new three::GLBuffer(sizeof(ModelConst), GL_UNIFORM_BUFFER));
		glm::u8vec4 white = { 255, 255, 255, 255 };
		m_texture = std::unique_ptr<three::GLTexture2D>(new three::GLTexture2D);
		m_texture->load_memory_rgba(1, 1, (uint8_t*)&white, true);
		m_material = std::unique_ptr<three::MeshStandardMaterial>(new three::MeshStandardMaterial);
		m_material->tex_idx_map = 0;
		this->set_color({ 0.8f, 0.8f, 0.8f});		
		m_geometry = std::unique_ptr<three::Primitive>(new three::Primitive);
		m_geometry->material_idx = 0;
	}
	
	SimpleModel* SimpleModel::Create()
	{
		return MakeGarbageCollected<SimpleModel>();
	}
	
	void SimpleModel::dispose()
	{
		m_constant = nullptr;
		m_texture = nullptr;
		m_material = nullptr;
		m_geometry = nullptr;
		Object3D::dispose();
	}
	
	void SimpleModel::createBox(float width, float height, float depth)
	{
		three::GeometryCreator::CreateBox(m_geometry.get(), width, height, depth);
	}
	
	void SimpleModel::createSphere(float radius, int widthSegments, int heightSegments)
	{
		three::GeometryCreator::CreateSphere(m_geometry.get(), radius, widthSegments, heightSegments);
	}
	
	void SimpleModel::createPlane(float width, float height)
	{
		three::GeometryCreator::CreatePlane(m_geometry.get(), width, height);
	}
	
	ScriptValue SimpleModel::color(ScriptState* script_state) const
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		v8::Local<v8::Object> ret = v8::Object::New(isolate);
		vec3_to_jvec3(isolate, m_material->color, ret);
			
		return ScriptValue(isolate, ret);
	}
	
	void SimpleModel::getColor(ScriptState* script_state, ScriptValue color)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		v8::Local<v8::Object> j_color = color.V8Value().As<v8::Object>();
		vec3_to_jvec3(isolate, m_material->color, j_color);
	}
	
	void SimpleModel::setColor(ScriptState* script_state, ScriptValue color)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		v8::Local<v8::Object> j_color = color.V8Value().As<v8::Object>();
		glm::vec3 color3;
		jvec3_to_vec3(isolate, j_color, color3);
		this->set_color(color3);
	}
	
	void SimpleModel::setColor(float r, float g, float b)
	{
		this->set_color({r,g,b});
	}
	
	void SimpleModel::setColorTexture(RGBAImage* image)
	{
		m_texture->load_memory_rgba(image->width(), image->height(), image->data(), true);
	}
	
	void SimpleModel::setMetalness(float metalness)
	{
		m_material->metallicFactor = metalness;
		m_material->update_uniform();
	}


	void SimpleModel::setRoughness(float roughness)
	{
		m_material->roughnessFactor = roughness;
		m_material->update_uniform();
	}

	void SimpleModel::setToonShading(int mode, float wire_width)
	{
		m_material->tone_shading = mode;
		m_material->wire_width = wire_width;
	}
	
	// Internal	
	void SimpleModel::updateConstant()
	{
		ModelConst c;
		c.ModelMat = m_matrixWorld;
		c.NormalMat = glm::transpose(glm::inverse(m_matrixWorld));
		m_constant->upload(&c);
	}

	void SimpleModel::set_color(const glm::vec3& color)
	{
		m_material->color = glm::vec4(color, 1.0f);
		m_material->update_uniform();
	}

	

}

