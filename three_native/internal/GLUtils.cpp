#include "GLUtils.h"
#include <cstdio>
#include <vector>
#include <string>
#include "third_party/blink/renderer/modules/three_native/three_context.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {
	namespace three 
	{	
		GLShader::GLShader(unsigned type, const char* code)
		{
			auto *glctx = ThreeContext::m_current_context->ContextGL();
			
			m_type = type;
			m_id = glctx->CreateShader(type);
			glctx->ShaderSource(m_id, 1, &code, nullptr);
			glctx->CompileShader(m_id);

			GLint compileResult = 0;
			glctx->GetShaderiv(m_id, GL_COMPILE_STATUS, &compileResult);
			if (compileResult == 0)
			{
				GLint infoLogLength = 0;
				glctx->GetShaderiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
				std::vector<GLchar> infoLog(infoLogLength+1, 0);
				glctx->GetShaderInfoLog(m_id, (GLsizei)infoLogLength, nullptr, infoLog.data());			
				ThreeContext::log()+= "Shader compilation failed: ";
				ThreeContext::log()+= infoLog.data();
				ThreeContext::log()+= "\n";
			}
		}

		GLShader::~GLShader()
		{
			if (m_id != (unsigned)-1)
			{
				auto *glctx = ThreeContext::m_current_context->ContextGL();
				glctx->DeleteShader(m_id);
			}
		}

		GLProgram::GLProgram(const GLShader& vertexShader, const GLShader& fragmentShader)
		{
			auto *glctx = ThreeContext::m_current_context->ContextGL();
			
			m_id = glctx->CreateProgram();
			glctx->AttachShader(m_id, vertexShader.m_id);
			glctx->AttachShader(m_id, fragmentShader.m_id);
			glctx->LinkProgram(m_id);

			GLint linkResult = 0;
			glctx->GetProgramiv(m_id, GL_LINK_STATUS, &linkResult);
			if (linkResult == 0)
			{
				GLint infoLogLength = 0;
				glctx->GetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
				std::vector<GLchar> infoLog(infoLogLength + 1, 0);
				glctx->GetProgramInfoLog(m_id, (GLsizei)infoLog.size(), nullptr, infoLog.data());
				ThreeContext::log()+= "Shader link failed: ";
				ThreeContext::log()+= infoLog.data();
				ThreeContext::log()+= "\n";
			}
		}

		GLProgram::GLProgram(const GLShader& vertexShader, const GLShader& geometryShader, const GLShader& fragmentShader)
		{
			auto *glctx = ThreeContext::m_current_context->ContextGL();
			
			m_id = glctx->CreateProgram();
			glctx->AttachShader(m_id, vertexShader.m_id);
			glctx->AttachShader(m_id, geometryShader.m_id);
			glctx->AttachShader(m_id, fragmentShader.m_id);
			glctx->LinkProgram(m_id);

			GLint linkResult = 0;
			glctx->GetProgramiv(m_id, GL_LINK_STATUS, &linkResult);
			if (linkResult == 0)
			{
				GLint infoLogLength = 0;
				glctx->GetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
				std::vector<GLchar> infoLog(infoLogLength + 1, 0);
				glctx->GetProgramInfoLog(m_id, (GLsizei)infoLog.size(), nullptr, infoLog.data());
				ThreeContext::log()+= "Shader link failed: ";
				ThreeContext::log()+= infoLog.data();
				ThreeContext::log()+= "\n";
			}
		}

		GLProgram::GLProgram(const GLShader& computeShader)
		{
			auto *glctx = ThreeContext::m_current_context->ContextGL();
			
			m_id = glctx->CreateProgram();
			glctx->AttachShader(m_id, computeShader.m_id);
			glctx->LinkProgram(m_id);

			GLint linkResult = 0;
			glctx->GetProgramiv(m_id, GL_LINK_STATUS, &linkResult);
			if (linkResult == 0)
			{
				GLint infoLogLength = 0;
				glctx->GetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
				std::vector<GLchar> infoLog(infoLogLength + 1, 0);
				glctx->GetProgramInfoLog(m_id, (GLsizei)infoLog.size(), nullptr, infoLog.data());
				ThreeContext::log()+= "Shader link failed: ";
				ThreeContext::log()+= infoLog.data();
				ThreeContext::log()+= "\n";
			}
		}

		GLProgram::~GLProgram()
		{
			if (m_id != (unsigned)(-1))
			{
				auto *glctx = ThreeContext::m_current_context->ContextGL();
				glctx->DeleteProgram(m_id);
			}
		}
		
		GLTexture2D::GLTexture2D()
		{
			auto *glctx = ThreeContext::m_current_context->ContextGL();
			glctx->GenTextures(1, &tex_id);
		}

		GLTexture2D::~GLTexture2D()
		{
			auto *glctx = ThreeContext::m_current_context->ContextGL();
			glctx->DeleteTextures(1, &tex_id);
		}

		void GLTexture2D::load_memory_rgba(int width, int height, const uint8_t* data, bool is_srgb)
		{
			auto *glctx = ThreeContext::m_current_context->ContextGL();
			glctx->BindTexture(GL_TEXTURE_2D, tex_id);
			glctx->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glctx->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glctx->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glctx->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glctx->PixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glctx->TexImage2D(GL_TEXTURE_2D, 0, is_srgb ? GL_SRGB8_ALPHA8 : GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glctx->GenerateMipmap(GL_TEXTURE_2D);
			glctx->BindTexture(GL_TEXTURE_2D, 0);
		}

		void GLTexture2D::load_memory_bgra(int width, int height, const uint8_t* data, bool is_srgb)
		{
			auto *glctx = ThreeContext::m_current_context->ContextGL();
			glctx->BindTexture(GL_TEXTURE_2D, tex_id);
			glctx->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glctx->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glctx->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glctx->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glctx->PixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glctx->TexImage2D(GL_TEXTURE_2D, 0, is_srgb ? GL_SRGB8_ALPHA8 : GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
			glctx->GenerateMipmap(GL_TEXTURE_2D);
			glctx->BindTexture(GL_TEXTURE_2D, 0);
		}
		
			
		GLCubemap::GLCubemap()
		{
			auto *glctx = ThreeContext::m_current_context->ContextGL();
			glctx->GenTextures(1, &tex_id);
		}

		GLCubemap::~GLCubemap()
		{
			auto *glctx = ThreeContext::m_current_context->ContextGL();
			glctx->DeleteTextures(1, &tex_id);
		}
		
		void GLCubemap::load_memory_rgba(int width, int height, const uint8_t* data_xp, const uint8_t* data_xn, const uint8_t* data_yp, const uint8_t* data_yn, const uint8_t* data_zp, const uint8_t* data_zn)
		{
			auto *glctx = ThreeContext::m_current_context->ContextGL();
			glctx->BindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
			glctx->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glctx->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glctx->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glctx->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glctx->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glctx->TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_xp);
			glctx->TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_xn);
			glctx->TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_yp);
			glctx->TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_yn);
			glctx->TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_zp);
			glctx->TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_zn);
			glctx->BindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}

		void GLCubemap::load_memory_bgra(int width, int height, const uint8_t* data_xp, const uint8_t* data_xn, const uint8_t* data_yp, const uint8_t* data_yn, const uint8_t* data_zp, const uint8_t* data_zn)
		{
			auto *glctx = ThreeContext::m_current_context->ContextGL();
			glctx->BindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
			glctx->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glctx->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glctx->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glctx->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glctx->TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glctx->TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data_xp);
			glctx->TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data_xn);
			glctx->TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data_yp);
			glctx->TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data_yn);
			glctx->TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data_zp);
			glctx->TexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data_zn);
			glctx->BindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}
		
			
		GLBuffer::GLBuffer(size_t size, unsigned target)
		{
			auto *glctx = ThreeContext::m_current_context->ContextGL();
			
			m_target = target;
			m_size = size;
			glctx->GenBuffers(1, &m_id);
			glctx->BindBuffer(m_target, m_id);
			glctx->BufferData(m_target, m_size, nullptr, GL_STATIC_COPY);
			glctx->BindBuffer(m_target, 0);
		}

		GLBuffer::~GLBuffer()
		{
			if (m_id != (unsigned)(-1))
			{
				auto *glctx = ThreeContext::m_current_context->ContextGL();
				glctx->DeleteBuffers(1, &m_id);
			}
		}

		void GLBuffer::upload(const void* data)
		{
			auto *glctx = ThreeContext::m_current_context->ContextGL();
			glctx->BindBuffer(m_target, m_id);
			glctx->BufferData(m_target, m_size, data, GL_STATIC_DRAW);
			glctx->BindBuffer(m_target, 0);
		}

		const GLBuffer& GLBuffer::operator = (const GLBuffer& in)
		{
			auto *glctx = ThreeContext::m_current_context->ContextGL();
			glctx->BindBuffer(GL_COPY_READ_BUFFER, in.m_id);
			glctx->BindBuffer(GL_COPY_WRITE_BUFFER, m_id);
			glctx->CopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, m_size);
			glctx->BindBuffer(GL_COPY_READ_BUFFER, 0);
			glctx->BindBuffer(GL_COPY_WRITE_BUFFER, 0);
			return *this;
		}
	}
}
