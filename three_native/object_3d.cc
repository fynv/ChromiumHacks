#include "third_party/blink/renderer/modules/three_native/object_3d.h"
#include <gtx/euler_angles.hpp>
#include <gtx/matrix_decompose.hpp>
#include <algorithm>
#include "wrapper_utils.hpp"

namespace blink {
	
	Object3D::Object3D()
		: m_position({ 0.0f, 0.0f, 0.0f })
		, m_rotation({ 0.0f, 0.0f, 0.0f })
		, m_quaternion(glm::identity<glm::quat>())
		, m_scale({ 1.0f, 1.0f, 1.0f })
		, m_matrix(glm::identity<glm::mat4>())
		, m_matrixWorld(glm::identity<glm::mat4>())
	{

	}
	
	Object3D* Object3D::Create()
	{
		return MakeGarbageCollected<Object3D>();
	}
	
	void Object3D::Trace(Visitor* visitor) const
	{
		ScriptWrappable::Trace(visitor);
		visitor->Trace(m_parent); 
		visitor->Trace(m_children);
	}
	
	String Object3D::name()
	{
		return m_name;
	}
	
	void Object3D::setName(String name)
	{
		m_name = name;
	}
	
	Object3D* Object3D::parent()
	{
		return m_parent;
	}
	
	void Object3D::setParent(Object3D* parent)
	{
		m_parent = parent;
	}
	
	HeapVector<Member<Object3D>> Object3D::children() const
	{
		return m_children;
	}
	
	Object3D* Object3D::add(Object3D* object)
	{
		if (object->m_parent != nullptr)
		{
			object->m_parent->remove(object);
		}
		object->m_parent = this;
		this->m_children.push_back(object);
		return this;
	}	
	
	Object3D* Object3D::remove(Object3D* object)
	{
		wtf_size_t pos = m_children.Find(object);
		if (pos<m_children.size())
		{
			
			object->m_parent = nullptr;
			m_children.EraseAt(pos);
		}		
		return this;
	}
	
	Object3D* Object3D::removeFromParent()
	{
		if (m_parent != nullptr)
		{
			m_parent->remove(this);
		}
		return this;
	}
	
	Object3D* Object3D::clear()
	{
		for (size_t i = 0; i < m_children.size(); i++)
		{
			Object3D* object = m_children[(unsigned)i];
			object->m_parent = nullptr;
		}
		m_children.clear();
		return this;
	}
	
	Object3D* Object3D::getObjectByName(String name)
	{
		if (this->m_name == name) return this;

		for (size_t i = 0; i < m_children.size(); i++)
		{
			Object3D* obj = m_children[(unsigned)i]->getObjectByName(name);
			if (obj != nullptr) return obj;
		}
		return nullptr;
	}
	
	ScriptValue Object3D::up(ScriptState* script_state) const
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> ret = v8::Object::New(isolate);
		vec3_to_jvec3(isolate, m_up, ret);
			
		return ScriptValue(isolate, ret);
	}
	
	void Object3D::getUp(ScriptState* script_state, ScriptValue up)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> j_up = up.V8Value().As<v8::Object>();
		vec3_to_jvec3(isolate, m_up, j_up);
	}
	
	void Object3D::setUp(ScriptState* script_state, ScriptValue up)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> j_up = up.V8Value().As<v8::Object>();
		jvec3_to_vec3(isolate, j_up, m_up);
	}
	
	void Object3D::setUp(float x, float y, float z)
	{
		m_up.x = x;
		m_up.y = y;
		m_up.z = z;
	}
	
	ScriptValue Object3D::position(ScriptState* script_state) const
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> ret = v8::Object::New(isolate);
		vec3_to_jvec3(isolate, m_position, ret);
			
		return ScriptValue(isolate, ret);
	}
	
	void Object3D::getPosition(ScriptState* script_state, ScriptValue position)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> j_position = position.V8Value().As<v8::Object>();
		vec3_to_jvec3(isolate, m_position, j_position);
	}
	
	void Object3D::setPosition(ScriptState* script_state, ScriptValue position)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> j_position = position.V8Value().As<v8::Object>();
		jvec3_to_vec3(isolate, j_position, m_position);
	}
	
	void Object3D::setPosition(float x, float y, float z)
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = z;
	}
	
	ScriptValue Object3D::rotation(ScriptState* script_state) const
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> ret = v8::Object::New(isolate);
		vec3_to_jvec3(isolate, m_rotation, ret);
			
		return ScriptValue(isolate, ret);
	}
	
	void Object3D::getRotation(ScriptState* script_state, ScriptValue rotation)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> j_rotation = rotation.V8Value().As<v8::Object>();
		vec3_to_jvec3(isolate, m_rotation, j_rotation);
	}
	
	void Object3D::setRotation(ScriptState* script_state, ScriptValue rotation)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> j_rotation = rotation.V8Value().As<v8::Object>();
		glm::vec3 v_rotation;
		jvec3_to_vec3(isolate, j_rotation, v_rotation);
		this->set_rotation(v_rotation);
	}
	
	void Object3D::setRotation(float x, float y, float z)
	{
		glm::vec3 v_rotation(x,y,z);
		this->set_rotation(v_rotation);
	}	
	
	ScriptValue Object3D::quaternion(ScriptState* script_state) const
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> ret = v8::Object::New(isolate);
		quat_to_jquat(isolate, m_quaternion, ret);
			
		return ScriptValue(isolate, ret);
	}
	
	void Object3D::getQuaternion(ScriptState* script_state, ScriptValue quaternion)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> j_quat = quaternion.V8Value().As<v8::Object>();
		quat_to_jquat(isolate, m_quaternion, j_quat);
	}
	
	void Object3D::setQuaternion(ScriptState* script_state, ScriptValue quaternion)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> j_quat = quaternion.V8Value().As<v8::Object>();
		glm::quat v_quat;
		jquat_to_quat(isolate, j_quat, v_quat);
		this->set_quaternion(v_quat);
	}
	
	void Object3D::setQuaternion(float x, float y, float z, float w)
	{
		glm::quat v_quat(w,x,y,z);
		this->set_quaternion(v_quat);
	}
	
	ScriptValue Object3D::scale(ScriptState* script_state) const
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> ret = v8::Object::New(isolate);
		vec3_to_jvec3(isolate, m_scale, ret);
			
		return ScriptValue(isolate, ret);
	}
	
	void Object3D::getScale(ScriptState* script_state, ScriptValue scale)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> j_scale = scale.V8Value().As<v8::Object>();
		vec3_to_jvec3(isolate, m_scale, j_scale);
	}
	
	void Object3D::setScale(ScriptState* script_state, ScriptValue scale)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> j_scale = scale.V8Value().As<v8::Object>();
		jvec3_to_vec3(isolate, j_scale, m_scale);
	}
	
	void Object3D::setScale(float x, float y, float z)
	{
		m_scale.x = x;
		m_scale.y = y;
		m_scale.z = z;
	}
	
	ScriptValue Object3D::matrix(ScriptState* script_state) const
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> ret = v8::Object::New(isolate);
		mat4_to_jmat4(isolate, m_matrix, ret);
			
		return ScriptValue(isolate, ret);
	}
	
	void Object3D::getMatrix(ScriptState* script_state, ScriptValue matrix)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> j_matrix = matrix.V8Value().As<v8::Object>();
		mat4_to_jmat4(isolate, m_matrix, j_matrix);
	}
	
	ScriptValue Object3D::matrixWorld(ScriptState* script_state) const
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> ret = v8::Object::New(isolate);
		mat4_to_jmat4(isolate, m_matrixWorld, ret);
			
		return ScriptValue(isolate, ret);
	}
	
	void Object3D::getMatrixWorld(ScriptState* script_state, ScriptValue matrix)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> j_matrix = matrix.V8Value().As<v8::Object>();
		mat4_to_jmat4(isolate, m_matrixWorld, j_matrix);
	}	
	
	void Object3D::updateMatrix()
	{
		m_matrix = glm::identity<glm::mat4>();
		m_matrix = glm::translate(m_matrix, m_position);
		m_matrix *= glm::toMat4(m_quaternion);
		m_matrix = glm::scale(m_matrix, m_scale);
		m_matrixWorldNeedsUpdate = true;
	}

	void Object3D::updateMatrixWorld(bool force)
	{
		this->updateMatrix();
		if (m_matrixWorldNeedsUpdate || force)
		{
			if (m_parent == nullptr)
			{
				m_matrixWorld = m_matrix;
			}
			else
			{
				m_matrixWorld =  m_parent->m_matrixWorld * m_matrix;
			}
			m_matrixWorldNeedsUpdate = false;
			force = true;
		}

		for (size_t i = 0; i < m_children.size(); i++)
		{
			m_children[(unsigned)i]->updateMatrixWorld(force);
		}
	}


	void Object3D::updateWorldMatrix(bool updateParents, bool updateChildren)
	{
		if (updateParents && m_parent != nullptr) 
		{
			m_parent->updateWorldMatrix(true, false);
		}

		this->updateMatrix();

		if (m_parent == nullptr)
		{
			m_matrixWorld = m_matrix;
		}
		else
		{
			m_matrixWorld = m_parent->m_matrixWorld * m_matrix;
		}

		if (updateChildren)
		{
			for (size_t i = 0; i < m_children.size(); i++)
			{
				m_children[(unsigned)i]->updateWorldMatrix(false, true);
			}
		}
	}
	
	Object3D* Object3D::applyMatrix4(ScriptState* script_state, ScriptValue matrix)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);		
		v8::Local<v8::Object> j_matrix = matrix.V8Value().As<v8::Object>();
		glm::mat4 v_matrix;
		jmat4_to_mat4(isolate, j_matrix, v_matrix);
		return this->applyMatrix4(v_matrix);		
	}
	
	Object3D* Object3D::applyQuaternion(ScriptState* script_state, ScriptValue quat)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);		
		v8::Local<v8::Object> j_quat = quat.V8Value().As<v8::Object>();
		glm::quat v_quat;
		jquat_to_quat(isolate, j_quat, v_quat);
		return this->applyQuaternion(v_quat);		
	}
	
	void Object3D::setRotationFromAxisAngle(ScriptState* script_state, ScriptValue axis, float angle)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);		
		v8::Local<v8::Object> j_axis = axis.V8Value().As<v8::Object>();
		glm::vec3 v_axis;
		jvec3_to_vec3(isolate, j_axis, v_axis);
		this->setRotationFromAxisAngle(v_axis, angle);
	}
	
	void Object3D::setRotationFromMatrix(ScriptState* script_state, ScriptValue matrix)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);		
		v8::Local<v8::Object> j_matrix = matrix.V8Value().As<v8::Object>();
		glm::mat4 v_matrix;
		jmat4_to_mat4(isolate, j_matrix, v_matrix);
		this->setRotationFromMatrix(v_matrix);
	}
	
	Object3D* Object3D::rotateOnAxis(ScriptState* script_state, ScriptValue axis, float angle)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);		
		v8::Local<v8::Object> j_axis = axis.V8Value().As<v8::Object>();
		glm::vec3 v_axis;
		jvec3_to_vec3(isolate, j_axis, v_axis);
		return this->rotateOnAxis(v_axis, angle);
	}
	
	Object3D* Object3D::rotateOnWorldAxis(ScriptState* script_state, ScriptValue axis, float angle)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);		
		v8::Local<v8::Object> j_axis = axis.V8Value().As<v8::Object>();
		glm::vec3 v_axis;
		jvec3_to_vec3(isolate, j_axis, v_axis);
		return this->rotateOnWorldAxis(v_axis, angle);
	}
	
	Object3D* Object3D::rotateX(float angle)
	{
		return this->rotateOnAxis({ 1.0f, 0.0f, 0.0f }, angle);
	}

	Object3D* Object3D::rotateY(float angle)
	{
		return this->rotateOnAxis({ 0.0f, 1.0f, 0.0f }, angle);
	}

	Object3D* Object3D::rotateZ(float angle)
	{
		return this->rotateOnAxis({ 0.0f, 0.0f, 1.0f }, angle);
	}
	
	Object3D* Object3D::translateOnAxis(ScriptState* script_state, ScriptValue axis, float distance)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		v8::Local<v8::Object> j_axis = axis.V8Value().As<v8::Object>();
		glm::vec3 v_axis;
		jvec3_to_vec3(isolate, j_axis, v_axis);
		return this->translateOnAxis(v_axis, distance);
	}
	
	Object3D* Object3D::translateX(float distance)
	{
		return translateOnAxis({ 1.0f, 0.0f, 0.0f }, distance);
	}
	
	Object3D* Object3D::translateY(float distance)
	{
		return translateOnAxis({ 0.0f, 1.0f, 0.0f }, distance);
	}
		
	Object3D* Object3D::translateZ(float distance)
	{
		return translateOnAxis({ 0.0f, 0.0f, 1.0f }, distance);
	}
	
	ScriptValue Object3D::localToWorld(ScriptState* script_state, ScriptValue vector) const
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		v8::Local<v8::Object> j_vector = vector.V8Value().As<v8::Object>();
		glm::vec3 v_vector;
		jvec3_to_vec3(isolate, j_vector, v_vector);
		v_vector = this->localToWorld(v_vector);
		vec3_to_jvec3(isolate, v_vector, j_vector);		
		return vector;
	}
	
	ScriptValue Object3D::worldToLocal(ScriptState* script_state, ScriptValue vector) const
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		v8::Local<v8::Object> j_vector = vector.V8Value().As<v8::Object>();
		glm::vec3 v_vector;
		jvec3_to_vec3(isolate, j_vector, v_vector);
		v_vector = this->worldToLocal(v_vector);
		vec3_to_jvec3(isolate, v_vector, j_vector);
		return vector;
	}
	
	ScriptValue Object3D::getWorldPosition(ScriptState* script_state, ScriptValue position)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		glm::vec3 v_position = this->getWorldPosition();
		v8::Local<v8::Object> j_position = position.V8Value().As<v8::Object>();	
		vec3_to_jvec3(isolate, v_position, j_position);
		return ScriptValue(isolate, j_position);
	}
	
	ScriptValue Object3D::getWorldQuaternion(ScriptState* script_state, ScriptValue quat)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		glm::quat v_quat = this->getWorldQuaternion();
		v8::Local<v8::Object> j_quat = quat.V8Value().As<v8::Object>();
		quat_to_jquat(isolate, v_quat, j_quat);
		return ScriptValue(isolate, j_quat);
	}
	
	ScriptValue Object3D::getWorldScale(ScriptState* script_state, ScriptValue scale)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		glm::vec3 v_scale = this->getWorldScale();
		v8::Local<v8::Object> j_scale = scale.V8Value().As<v8::Object>();		
		vec3_to_jvec3(isolate, v_scale, j_scale);
		return ScriptValue(isolate, j_scale);
	}
	
	ScriptValue Object3D::getWorldDirection(ScriptState* script_state, ScriptValue dir)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		glm::vec3 v_dir = this->getWorldDirection();
		v8::Local<v8::Object> j_dir = dir.V8Value().As<v8::Object>();	
		vec3_to_jvec3(isolate, v_dir, j_dir);
		return ScriptValue(isolate, j_dir);
	}
	
	void Object3D::lookAt(ScriptState* script_state, ScriptValue target)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		v8::Local<v8::Object> j_target = target.V8Value().As<v8::Object>();
		glm::vec3 v_target;
		jvec3_to_vec3(isolate, j_target, v_target);
		this->lookAt(v_target);
	}
	
	// internal
	void Object3D::set_rotation(const glm::vec3& rotation)
	{
		m_rotation = rotation;
		m_quaternion = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
	}

	void Object3D::set_quaternion(const glm::quat& quaternion)
	{
		m_quaternion = quaternion;
		glm::extractEulerAngleXYZ(glm::mat4(quaternion), m_rotation.x, m_rotation.y, m_rotation.z);
	}
	
		
	Object3D* Object3D::applyMatrix4(const glm::mat4& matrix)
	{
		this->updateMatrix();
		m_matrix = matrix * m_matrix;
		glm::quat rot;
		glm::vec3 skew;
		glm::vec4 persp;
		glm::decompose(m_matrix, m_scale, rot, m_position, skew, persp);
		this->set_quaternion(rot);
		return this;
	}

	Object3D* Object3D::applyQuaternion(const glm::quat& q)
	{
		this->set_quaternion(q * m_quaternion);
		return this;
	}

	void Object3D::setRotationFromAxisAngle(const glm::vec3 axis, float angle)
	{
		this->set_quaternion(glm::angleAxis(angle, axis));
	}

	void Object3D::setRotationFromMatrix(const glm::mat4& m)
	{
		this->set_quaternion(m);
	}
	
		
	Object3D* Object3D::rotateOnAxis(const glm::vec3& axis, float angle)
	{
		glm::quat _q1 = glm::angleAxis(angle, axis);
		this->set_quaternion(m_quaternion*_q1);
		return this;
	}

	Object3D* Object3D::rotateOnWorldAxis(const glm::vec3& axis, float angle)
	{
		glm::quat _q1 = glm::angleAxis(angle, axis);
		this->set_quaternion(_q1*m_quaternion);
		return this;
	}	
	
	Object3D* Object3D::translateOnAxis(const glm::vec3& axis, float distance)
	{	
		m_position += m_quaternion * axis * distance;
		return this;
	}
	
	glm::vec3 Object3D::localToWorld(const glm::vec3& vector) const
	{
		return m_matrixWorld * glm::vec4(vector, 1.0f);
	}

	glm::vec3 Object3D::worldToLocal(const glm::vec3& vector) const
	{
		return glm::inverse(m_matrixWorld) * glm::vec4(vector, 1.0f);
	}
		
	glm::vec3 Object3D::getWorldPosition()
	{
		this->updateWorldMatrix(true, false);
		return m_matrixWorld[3];
	}

	glm::quat Object3D::getWorldQuaternion()
	{
		this->updateWorldMatrix(true, false);
		return m_matrixWorld;
	}

	glm::vec3 Object3D::getWorldScale()
	{
		this->updateWorldMatrix(true, false);
		glm::vec3 scale;
		glm::quat rot;
		glm::vec3 trans;
		glm::vec3 skew;
		glm::vec4 persp;
		glm::decompose(m_matrixWorld, scale, rot, trans, skew, persp);
		return scale;
	}

	glm::vec3 Object3D::getWorldDirection()
	{
		this->updateWorldMatrix(true, false);
		glm::vec3 z = m_matrixWorld[2];
		return glm::normalize(z);
	}
	
	void Object3D::lookAt(const glm::vec3& target)
	{
		this->updateWorldMatrix(true, false);
		glm::vec3 position = m_matrixWorld[3];
		glm::mat4 m1 = glm::inverse(glm::lookAt(target, position, m_up));
		this->set_quaternion(m1);
		if (m_parent != nullptr)
		{
			glm::quat q = m_parent->m_matrixWorld;
			this->set_quaternion(q * m_quaternion);
		}
	}
	
}
