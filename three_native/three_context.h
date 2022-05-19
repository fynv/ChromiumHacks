#pragma once

#include <string>
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/modules/webgl/webgl2_rendering_context.h"


namespace blink {
	class ThreeContext final : public ScriptWrappable
	{
		DEFINE_WRAPPERTYPEINFO();
	public:		
		static WebGL2RenderingContext* currentContext();
		static void setCurrentContext(WebGL2RenderingContext*);
		
		static String getLog();
		static void cleanLog();
		
		// Internal
		static WebGL2RenderingContext* m_current_context;
		static std::string& log();
		
		
	};

}
