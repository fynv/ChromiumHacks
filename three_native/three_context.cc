#include "third_party/blink/renderer/modules/three_native/three_context.h"

namespace blink {
	
	WebGL2RenderingContext* ThreeContext::m_current_context = nullptr;
	
	WebGL2RenderingContext* ThreeContext::currentContext()
	{
		return m_current_context;
	}
	
	void ThreeContext::setCurrentContext(WebGL2RenderingContext* ctx)
	{
		m_current_context = ctx;
	}

	
	std::string& ThreeContext::log()
	{
		static std::string s_log;
		return s_log;
	}
	
	String ThreeContext::getLog()
	{
		return String::FromUTF8(log().c_str());
	}
	
	void ThreeContext::cleanLog()
	{
		log() = "";
	}
	
}
