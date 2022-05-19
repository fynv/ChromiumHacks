#include "third_party/blink/renderer/modules/three_native/camera.h"
#include "third_party/blink/renderer/modules/three_native/three_context.h"

#include "wrapper_utils.hpp"

namespace blink {
	struct CameraConst
	{
		glm::mat4 ProjMat;
		glm::mat4 ViewMat;
		glm::mat4 InvProjMat;
		glm::mat4 InvViewMat;
		glm::vec4 EyePos;
	};

	Camera::Camera()
		: m_matrixWorldInverse(glm::identity<glm::mat4>())
		, m_projectionMatrix(glm::identity<glm::mat4>())
		, m_projectionMatrixInverse(glm::identity<glm::mat4>())
	{
		m_constant = std::unique_ptr<three::GLBuffer>(new three::GLBuffer(sizeof(CameraConst), GL_UNIFORM_BUFFER));
	}
	
	Camera* Camera::Create()
	{
		return MakeGarbageCollected<Camera>();
	}
	
	void Camera::dispose()
	{
		m_constant = nullptr;
		Object3D::dispose();
	}
		
	ScriptValue Camera::matrixWorldInverse(ScriptState* script_state) const
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> ret = v8::Object::New(isolate);
		mat4_to_jmat4(isolate, m_matrixWorldInverse, ret);
			
		return ScriptValue(isolate, ret);
	}
	
	void Camera::getMatrixWorldInverse(ScriptState* script_state, ScriptValue matrix)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> j_matrix = matrix.V8Value().As<v8::Object>();
		mat4_to_jmat4(isolate, m_matrixWorldInverse, j_matrix);
	}
	
	ScriptValue Camera::projectionMatrix(ScriptState* script_state) const
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> ret = v8::Object::New(isolate);
		mat4_to_jmat4(isolate, m_projectionMatrix, ret);
			
		return ScriptValue(isolate, ret);
	}
	
	void Camera::getProjectionMatrix(ScriptState* script_state, ScriptValue matrix)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> j_matrix = matrix.V8Value().As<v8::Object>();
		mat4_to_jmat4(isolate, m_projectionMatrix, j_matrix);
	}
	
	ScriptValue Camera::projectionMatrixInverse(ScriptState* script_state) const
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> ret = v8::Object::New(isolate);
		mat4_to_jmat4(isolate, m_projectionMatrixInverse, ret);
			
		return ScriptValue(isolate, ret);
	}
	
	void Camera::getProjectionMatrixInverse(ScriptState* script_state, ScriptValue matrix)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> j_matrix = matrix.V8Value().As<v8::Object>();
		mat4_to_jmat4(isolate, m_projectionMatrixInverse, j_matrix);
	}
	
	
	// internal	
	void Camera::updateMatrixWorld(bool force)
	{
		Object3D::updateMatrixWorld(force);
		m_matrixWorldInverse = glm::inverse(m_matrixWorld);
	}

	void Camera::updateWorldMatrix(bool updateParents, bool updateChildren)
	{
		Object3D::updateWorldMatrix(updateParents, updateChildren);
		m_matrixWorldInverse = glm::inverse(m_matrixWorld);
	}

	void Camera::lookAt(const glm::vec3& target)
	{	
		this->updateWorldMatrix(true, false);
		glm::vec3 position = m_matrixWorld[3];	
		glm::mat4 m1 = glm::inverse(glm::lookAt(position, target, m_up));
		this->set_quaternion(m1);
		if (m_parent != nullptr)
		{
			glm::quat q = m_parent->m_matrixWorld;
			this->set_quaternion(q * m_quaternion);
		}
	}

	glm::vec3 Camera::getWorldDirection()
	{
		this->updateWorldMatrix(true, false);
		glm::vec3 z =  -m_matrixWorld[2];
		return glm::normalize(z);
	}

	void Camera::updateConstant()
	{
		CameraConst c;
		c.ProjMat = m_projectionMatrix;
		c.ViewMat = m_matrixWorldInverse;
		c.InvProjMat = m_projectionMatrixInverse;
		c.InvViewMat = m_matrixWorld;
		c.EyePos = m_matrixWorld[3];
		m_constant->upload(&c);
	}
}

