#pragma once

#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/modules/three_native/background.h"

#include <glm.hpp>

namespace blink {
	class ColorBackground final : public Background
	{
		DEFINE_WRAPPERTYPEINFO();
	public:
		ColorBackground();
		static ColorBackground* Create();
				
		ScriptValue color(ScriptState* script_state) const;	
		void getColor(ScriptState* script_state, ScriptValue color);
		void setColor(ScriptState* script_state, ScriptValue color);
		void setColor(float r, float g, float b);
		
		// Internal
		glm::vec3 m_color = { 1.0f, 1.0f, 1.0f };
		
		
	};
}


