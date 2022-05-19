#include "third_party/blink/renderer/modules/three_native/perspective_camera.h"
#include "wrapper_utils.hpp"

const double PI = 3.14159265359;
const double DEG2RAD = PI / 180.0;

namespace blink {
	PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float z_near, float z_far)
		: m_fov(fov), m_aspect(aspect), m_z_near(z_near), m_z_far(z_far)
	{
		updateProjectionMatrix();
	}
	
	PerspectiveCamera* PerspectiveCamera::Create(float fov, float aspect, float z_near, float z_far)
	{
		return MakeGarbageCollected<PerspectiveCamera>(fov, aspect, z_near, z_far);
	}

	void PerspectiveCamera::updateProjectionMatrix()
	{
		m_projectionMatrix = glm::perspective(m_fov * (float)DEG2RAD, m_aspect, m_z_near, m_z_far);
		m_projectionMatrixInverse = glm::inverse(m_projectionMatrix);
	}
	
	
}

