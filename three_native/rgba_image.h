#pragma once

#include <vector>
#include <memory>

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"

namespace blink {
	class RGBAImage final : public ScriptWrappable
	{
		DEFINE_WRAPPERTYPEINFO();
	public:
		RGBAImage();
		RGBAImage(int width, int height);
		static RGBAImage* Create(int width, int height);
		virtual void dispose();
		
		int width() const { return m_width; }
		int height() const { return m_height; }
		
		// internal		
		const uint8_t* data() const { return m_buffer.data(); }
		uint8_t* data() { return m_buffer.data(); }

		const uint8_t* get_data(int& width, int& height) const;
		
		friend class ImageLoader;
	
	private:
		int m_width = 0;
		int m_height = 0;
		std::vector<uint8_t> m_buffer;
	};
	
}
