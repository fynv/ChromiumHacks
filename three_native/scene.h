#pragma once

#include <memory>

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/modules/three_native/object_3d.h"

namespace blink {
	class Background;
	class Scene final : public Object3D
	{
		DEFINE_WRAPPERTYPEINFO();
	public:
		Scene();
		static Scene* Create();
		void dispose() override;
		
		void Trace(Visitor* visitor) const override;
		
		Background* background() { return m_background; }
		void setBackground(Background* bg) { m_background = bg; }
		
		// internal
		Member<Background> m_background;
				
		bool has_opaque = false;
		bool has_alpha = false;
	};
}
