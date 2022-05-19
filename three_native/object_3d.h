#pragma once

#include <string>
#include <vector>
#include <glm.hpp>
#include <gtx/quaternion.hpp>

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/heap/collection_support/heap_vector.h"
#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"


namespace blink {
	

	class Object3D : public ScriptWrappable
	{
		DEFINE_WRAPPERTYPEINFO();
	public:
		Object3D();
		static Object3D* Create();
		virtual void dispose() {}
		
		void Trace(Visitor* visitor) const override;
		
		String name();
		void setName(String name);
		
		Object3D* parent();
		void setParent(Object3D* parent);
		HeapVector<Member<Object3D>> children() const;
		
		Object3D* add(Object3D* object);
		Object3D* remove(Object3D* object);
		Object3D* removeFromParent();
		Object3D* clear();
		
		Object3D* getObjectByName(String name);
		
		ScriptValue up(ScriptState* script_state) const;
		void getUp(ScriptState* script_state, ScriptValue up);
		void setUp(ScriptState* script_state, ScriptValue up);
		void setUp(float x, float y, float z);
		
		ScriptValue position(ScriptState* script_state) const;
		void getPosition(ScriptState* script_state, ScriptValue position);
		void setPosition(ScriptState* script_state, ScriptValue position);
		void setPosition(float x, float y, float z);
		
		ScriptValue rotation(ScriptState* script_state) const;
		void getRotation(ScriptState* script_state, ScriptValue rotation);
		void setRotation(ScriptState* script_state, ScriptValue rotation);
		void setRotation(float x, float y, float z);
		
		ScriptValue quaternion(ScriptState* script_state) const;
		void getQuaternion(ScriptState* script_state, ScriptValue quaternion);
		void setQuaternion(ScriptState* script_state, ScriptValue quaternion);
		void setQuaternion(float x, float y, float z, float w);
		
		ScriptValue scale(ScriptState* script_state) const;
		void getScale(ScriptState* script_state, ScriptValue scale);
		void setScale(ScriptState* script_state, ScriptValue scale);
		void setScale(float x, float y, float z);
		
		ScriptValue matrix(ScriptState* script_state) const;
		void getMatrix(ScriptState* script_state, ScriptValue matrix);
		
		ScriptValue matrixWorld(ScriptState* script_state) const;
		void getMatrixWorld(ScriptState* script_state, ScriptValue matrix);
		
		void updateMatrix();
		virtual void updateMatrixWorld(bool force);
		virtual void updateWorldMatrix(bool updateParents, bool updateChildren);
		
		Object3D* applyMatrix4(ScriptState* script_state, ScriptValue matrix);
		Object3D* applyQuaternion(ScriptState* script_state, ScriptValue quat);
		void setRotationFromAxisAngle(ScriptState* script_state, ScriptValue axis, float angle);
		void setRotationFromMatrix(ScriptState* script_state, ScriptValue matrix);
		
		Object3D* rotateOnAxis(ScriptState* script_state, ScriptValue axis, float angle);
		Object3D* rotateOnWorldAxis(ScriptState* script_state, ScriptValue axis, float angle);
		Object3D* rotateX(float angle);
		Object3D* rotateY(float angle);
		Object3D* rotateZ(float angle);
		
		Object3D* translateOnAxis(ScriptState* script_state, ScriptValue axis, float distance);
		Object3D* translateX(float distance);
		Object3D* translateY(float distance);
		Object3D* translateZ(float distance);
		
		ScriptValue localToWorld(ScriptState* script_state, ScriptValue vector) const;
		ScriptValue worldToLocal(ScriptState* script_state, ScriptValue vector) const;
		
		ScriptValue getWorldPosition(ScriptState* script_state, ScriptValue position);
		ScriptValue getWorldQuaternion(ScriptState* script_state, ScriptValue quat);
		ScriptValue getWorldScale(ScriptState* script_state, ScriptValue scale);
		ScriptValue getWorldDirection(ScriptState* script_state, ScriptValue dir);
		
		void lookAt(ScriptState* script_state, ScriptValue target);
		
		// Internal
		String m_name;
		Member<Object3D> m_parent;
		HeapVector<Member<Object3D>> m_children;
		
		glm::vec3 m_up = { 0.0f, 1.0f, 0.0f };
		glm::vec3 m_position;
		glm::vec3 m_rotation;
		glm::quat m_quaternion;
		glm::vec3 m_scale;

		void set_rotation(const glm::vec3& rotation);
		void set_quaternion(const glm::quat& quaternion);
		
		glm::mat4 m_matrix;
		glm::mat4 m_matrixWorld;
		bool m_matrixWorldNeedsUpdate = false;
		
		Object3D* applyMatrix4(const glm::mat4& matrix);
		Object3D* applyQuaternion(const glm::quat& q);
		void setRotationFromAxisAngle(const glm::vec3 axis, float angle);
		void setRotationFromMatrix(const glm::mat4& m);
		
		Object3D* rotateOnAxis(const glm::vec3& axis, float angle);
		Object3D* rotateOnWorldAxis(const glm::vec3& axis, float angle);
		
		Object3D* translateOnAxis(const glm::vec3& axis, float distance);
		
		glm::vec3 localToWorld(const glm::vec3& vector) const;
		glm::vec3 worldToLocal(const glm::vec3& vector) const;
		
		glm::vec3 getWorldPosition();
		glm::quat getWorldQuaternion();
		glm::vec3 getWorldScale();
		virtual glm::vec3 getWorldDirection();
		
		virtual void lookAt(const glm::vec3& target);
	
		template <typename TCallBack>
		void traverse(TCallBack callback)
		{
			callback(this);
			for (size_t i = 0; i < m_children.size(); i++)
			{
				m_children[(unsigned)i]->traverse(callback);
			}
		}
	};

}

