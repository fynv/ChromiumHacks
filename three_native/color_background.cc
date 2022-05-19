#include "third_party/blink/renderer/modules/three_native/color_background.h"
#include "wrapper_utils.hpp"

namespace blink {
	ColorBackground::ColorBackground()
	{

	}
	
	ColorBackground* ColorBackground::Create()
	{
		return MakeGarbageCollected<ColorBackground>();
	}
	
	ScriptValue ColorBackground::color(ScriptState* script_state) const
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);
		
		v8::Local<v8::Object> ret = v8::Object::New(isolate);
		vec3_to_jvec3(isolate, m_color, ret);
			
		return ScriptValue(isolate, ret);
	}
	
	void ColorBackground::getColor(ScriptState* script_state, ScriptValue color)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);		
		
		v8::Local<v8::Object> jcolor = color.V8Value().As<v8::Object>();
		vec3_to_jvec3(isolate, m_color, jcolor);
	}

	void ColorBackground::setColor(ScriptState* script_state, ScriptValue color)
	{
		v8::Isolate* isolate = script_state->GetIsolate();
		v8::HandleScope handle_scope(isolate);		
		
		v8::Local<v8::Object> jcolor = color.V8Value().As<v8::Object>();
		jvec3_to_vec3(isolate, jcolor, m_color);
		
	}
	
	void ColorBackground::setColor(float r, float g, float b)
	{
		m_color.x = r;
		m_color.y = g;
		m_color.z = b;
	}
}

