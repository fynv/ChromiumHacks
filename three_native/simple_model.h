#pragma once

#include <memory>

#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/modules/three_native/object_3d.h"
#include "third_party/blink/renderer/modules/three_native/internal/ModelComponents.h"
#include "third_party/blink/renderer/modules/three_native/internal/MeshStandardMaterial.h"
#include "third_party/blink/renderer/modules/three_native/internal/GLUtils.h"

namespace blink {
	class RGBAImage;
	class SimpleModel final: public Object3D
	{
		DEFINE_WRAPPERTYPEINFO();
	public:
		SimpleModel();
		static SimpleModel* Create();
		void dispose() override;
		
		void createBox(float width, float height, float depth);
		void createSphere(float radius, int widthSegments, int heightSegments);
		void createPlane(float width, float height);
		
		ScriptValue color(ScriptState* script_state) const;
		void getColor(ScriptState* script_state, ScriptValue color);
		void setColor(ScriptState* script_state, ScriptValue color);
		void setColor(float r, float g, float b);
		
		void setColorTexture(RGBAImage* image);
		
		void setMetalness(float metalness);
		void setRoughness(float roughness);

		void setToonShading(int mode, float wire_width);
		
		// Internal
		std::unique_ptr<three::GLBuffer> m_constant;
		void updateConstant();
		
		std::unique_ptr<three::GLTexture2D> m_texture;
		std::unique_ptr<three::MeshStandardMaterial> m_material;
		std::unique_ptr<three::Primitive> m_geometry;
		void set_color(const glm::vec3& color);
		
	};
}

