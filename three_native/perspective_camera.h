#pragma once

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/modules/three_native/camera.h"

namespace blink {
	class PerspectiveCamera final: public Camera
	{
		DEFINE_WRAPPERTYPEINFO();
	public:
		PerspectiveCamera(float fov, float aspect, float z_near, float z_far);
		static PerspectiveCamera* Create(float fov, float aspect, float z_near, float z_far);
		
		bool isPerspectiveCamera(){return true;}
		float fov() { return m_fov;}
		void setFov(float fov) { m_fov = fov;}
		
		float aspect() { return m_aspect;}
		void setAspect(float aspect) { m_aspect = aspect;}
		
		float near() { return m_z_near;}
		void setNear(float near) { m_z_near = near;}
		
		float far() { return m_z_far;}
		void setFar(float far) { m_z_far = far;}
		
		void updateProjectionMatrix();
		
		// Internal
		float m_fov, m_aspect, m_z_near, m_z_far;
		
		
	};
	
}
		
