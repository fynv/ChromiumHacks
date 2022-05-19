#pragma once

#include <memory>
#include <unordered_map>
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/modules/three_native/internal/StandardRoutine.h"

namespace blink {
	
	class Scene;
	class Camera;
	class SimpleModel;
	class GLRenderer final : public ScriptWrappable
	{
		DEFINE_WRAPPERTYPEINFO();
	public:
		GLRenderer();
		static GLRenderer* Create();
		virtual void dispose();
		
		static void s_test();
		
		void render(int width, int height, Camera* camera, Scene* scene);
		
	private:
		void update_simple_model(SimpleModel* model);
		
		enum class Pass
		{
			Opaque,
			Highlight,
			Alpha
		};
		
		std::unordered_map<uint64_t, std::unique_ptr<three::StandardRoutine>> routine_map;
		three::StandardRoutine* get_routine(const three::StandardRoutine::Options& options);
		
		void render_primitive(const three::StandardRoutine::RenderParams& params, Pass pass);
		void render_model(Camera* p_camera, /*const Lights& lights,*/ SimpleModel* model, Pass pass);

	};

}
