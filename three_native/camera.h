#pragma once

#include <memory>

#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/modules/three_native/object_3d.h"
#include "third_party/blink/renderer/modules/three_native/internal/GLUtils.h"

namespace blink {
	class Camera : public Object3D
	{
		DEFINE_WRAPPERTYPEINFO();
	public:
		Camera();
		static Camera* Create();
		void dispose() override;
		
		ScriptValue matrixWorldInverse(ScriptState* script_state) const;
		void getMatrixWorldInverse(ScriptState* script_state, ScriptValue matrix);
		
		ScriptValue projectionMatrix(ScriptState* script_state) const;
		void getProjectionMatrix(ScriptState* script_state, ScriptValue matrix);
		
		ScriptValue projectionMatrixInverse(ScriptState* script_state) const;
		void getProjectionMatrixInverse(ScriptState* script_state, ScriptValue matrix);
		
		
		// Internal
		glm::mat4 m_matrixWorldInverse;
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_projectionMatrixInverse;
		
		void updateMatrixWorld(bool force) override;
		void updateWorldMatrix(bool updateParents, bool updateChildren) override;
		void lookAt(const glm::vec3& target) override;
		glm::vec3 getWorldDirection() override;
		
		std::unique_ptr<three::GLBuffer> m_constant;
		void updateConstant();
	};

}

