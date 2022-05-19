#pragma once

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"

namespace blink {
	class Background : public ScriptWrappable
	{
		DEFINE_WRAPPERTYPEINFO();
	public:
		Background();
		virtual void dispose() {}
	};
}


