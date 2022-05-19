#pragma once

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/typed_arrays/array_buffer_view_helpers.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {
	class RGBAImage;
	class DOMArrayBufferBase;
	class ImageLoader final : public ScriptWrappable
	{
		DEFINE_WRAPPERTYPEINFO();
	public:
		static RGBAImage* loadMemory(DOMArrayBufferBase* buf);
		
		// internal
		static void LoadMemory(RGBAImage* image, unsigned char* data, size_t size);
	
	};
	
}

