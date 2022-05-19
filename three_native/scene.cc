#include "third_party/blink/renderer/modules/three_native/scene.h"
#include "wrapper_utils.hpp"
#include "background.h"

namespace blink {
	Scene::Scene()
	{
	}
	
	Scene* Scene::Create()
	{
		return MakeGarbageCollected<Scene>();
	}
	
	void Scene::dispose()
	{
		
	}
	
	void Scene::Trace(Visitor* visitor) const
	{
		Object3D::Trace(visitor);
		visitor->Trace(m_background);
	}
}
