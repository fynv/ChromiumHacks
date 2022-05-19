 #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "image_loader.h"
#include "rgba_image.h"
#include "third_party/blink/renderer/core/typed_arrays/dom_array_buffer_base.h"


namespace blink {
	RGBAImage* ImageLoader::loadMemory(DOMArrayBufferBase* buf)
	{
		size_t size = buf->ByteLength();
		unsigned char* data = (unsigned char*)buf->DataMaybeShared();		
		RGBAImage* image = MakeGarbageCollected<RGBAImage>();
		LoadMemory(image, data, size);		
		return image;
	}
	
	void ImageLoader::LoadMemory(RGBAImage* image, unsigned char* data, size_t size)
	{
		int chn;
		stbi_uc* rgba = stbi_load_from_memory(data, (int)size, &image->m_width, &image->m_height, &chn, 4);
		size_t buf_size = (size_t)image->m_width * (size_t)image->m_height * 4;
		image->m_buffer.resize(buf_size);
		memcpy(image->m_buffer.data(), rgba, buf_size);
		stbi_image_free(rgba);
	}
}
