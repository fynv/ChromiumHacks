#include <cstdio>
#include <string>
#include <vector>
#include "crc64.h"
#include "three_context.h"
#include "gl_renderer.h"
#include "camera.h"
#include "scene.h"
#include "color_background.h"
#include "simple_model.h"
#include "internal/MeshStandardMaterial.h"


/*static const char g_vertex[] =
R"(#version 310 es
precision highp float;

layout (location = 0) out vec2 vUV;
void main()
{
	vec2 grid = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	vec2 vpos = grid * vec2(2.0, 2.0) + vec2(-1.0, -1.0);
	gl_Position = vec4(vpos, 1.0, 1.0);
	vUV = vec2(grid.x, grid.y);
}
)";

static const char g_frag[] =
R"(#version 310 es
precision highp float;

layout (location = 0) in vec2 vUV;
layout (location = 0) out vec4 outColor;
layout (location = 0) uniform sampler2D uTex;
void main()
{
	outColor = vec4(texture(uTex, vUV).xyz, 1.0);
	// outColor = vec4(1.0, 1.0, 1.0, 1.0);
}
)";*/


namespace blink {
	GLRenderer::GLRenderer()
	{
	
	}
	
	GLRenderer* GLRenderer::Create()
	{
		return MakeGarbageCollected<GLRenderer>();
	}
	
	void GLRenderer::dispose()
	{
		routine_map.clear();
	}
		
	void GLRenderer::s_test()
	{
		auto *glctx = ThreeContext::m_current_context->ContextGL();
		glctx->ClearColor(1.0f, 1.0f, 0.0f, 1.0f);
		glctx->Clear(GL_COLOR_BUFFER_BIT);
	}
	
	
	void GLRenderer::update_simple_model(SimpleModel* model)
	{
		model->updateConstant();
	}
	
	

	three::StandardRoutine* GLRenderer::get_routine(const three::StandardRoutine::Options& options)
	{
		uint64_t hash = crc64(0, (const unsigned char*)&options, sizeof(three::StandardRoutine::Options));
		auto iter = routine_map.find(hash);
		if (iter == routine_map.end())
		{
			routine_map[hash] = std::unique_ptr<three::StandardRoutine>(new three::StandardRoutine(options));
		}
		return routine_map[hash].get();
	}
	
	void GLRenderer::render_primitive(const three::StandardRoutine::RenderParams& params, Pass pass)
	{
		const three::MeshStandardMaterial* material = params.material_list[params.primitive->material_idx];
		//const Lights* lights = params.lights;	
		
		three::StandardRoutine::Options options;
		options.alpha_mode = material->alphaMode;
		options.is_highlight_pass = pass == Pass::Highlight;
		options.has_color = params.primitive->color_buf != nullptr;
		options.has_color_texture = material->tex_idx_map >= 0;
		options.has_metalness_map = material->tex_idx_metalnessMap >= 0;
		options.has_roughness_map = material->tex_idx_roughnessMap >= 0;
		options.has_normal_map = material->tex_idx_normalMap >= 0;
		options.has_emissive_map = material->tex_idx_emissiveMap >= 0;
		/*options.num_directional_lights = lights->num_directional_lights;
		options.num_directional_shadows = (int)lights->directional_shadow_texs.size();
		options.has_environment_map = lights->environment_map != nullptr;
		options.has_ambient_light = lights->ambient_light != nullptr;
		options.has_hemisphere_light = lights->hemisphere_light != nullptr;
		options.tone_shading = material->tone_shading;*/
		three::StandardRoutine* routine = get_routine(options);
		routine->render(params);
	} 
	
	void GLRenderer::render_model(Camera* p_camera, /*const Lights& lights,*/ SimpleModel* model, Pass pass)
	{	
		const three::GLTexture2D* tex = model->m_texture.get();
		const three::MeshStandardMaterial* material = model->m_material.get();

		if (pass == Pass::Opaque)
		{
			if (material->alphaMode == three::AlphaMode::Blend) return;
		}
		else if (pass == Pass::Opaque || pass == Pass::Highlight)
		{
			if (material->alphaMode != three::AlphaMode::Blend) return;
		}

		if (material->tone_shading > 0 && model->m_geometry->wire_ind_buf==nullptr)
		{
			model->m_geometry->compute_wires();
		}

		three::StandardRoutine::RenderParams params;
		params.tex_list = &tex;
		params.material_list = &material;
		params.constant_camera = p_camera->m_constant.get();
		params.constant_model = model->m_constant.get();
		params.primitive = model->m_geometry.get();
		//params.lights = &lights;
		render_primitive(params, pass);
	}
	
	void GLRenderer::render(int width, int height, Camera* camera, Scene* scene)
	{
		camera->updateMatrixWorld(false);
		camera->updateConstant();
		
		// enumerate objects
		struct Lists
		{
			std::vector<SimpleModel*> simple_models;
		};
		
		Lists lists;
		auto* p_lists = &lists;	
		
		scene->traverse([p_lists](Object3D* obj) {
			obj->updateWorldMatrix(false, false);
			
			auto *type_info = obj->GetWrapperTypeInfo();
			{
				auto* type_simple_model = SimpleModel::GetStaticWrapperTypeInfo();
				if (type_info->Equals(type_simple_model))
				{
					SimpleModel* model = (SimpleModel*)(obj);
					p_lists->simple_models.push_back(model);
					return;
				}
			}
		});
		
		// update models
		for (size_t i = 0; i < lists.simple_models.size(); i++)
		{
			SimpleModel* model = lists.simple_models[i];
			update_simple_model(model);

			const three::MeshStandardMaterial* material = model->m_material.get();
			if (material->alphaMode == three::AlphaMode::Blend)
			{
				scene->has_alpha = true;
			}
			else
			{
				scene->has_opaque = true;
			}
		}
		
		// render scene
		auto *glctx = ThreeContext::m_current_context->ContextGL();
		glctx->Enable(GL_FRAMEBUFFER_SRGB_EXT);
		glctx->Viewport(0, 0, width, height);
		
		while(scene->m_background!=nullptr)
		{
			auto *type_info = scene->m_background->GetWrapperTypeInfo();
			{
				auto* type_color_background = ColorBackground::GetStaticWrapperTypeInfo();
				if (type_info->Equals(type_color_background))
				{
					ColorBackground* bg = (ColorBackground*)(scene->m_background.Get());					
					glctx->ClearColor(bg->m_color.r, bg->m_color.g, bg->m_color.b, 1.0f);
					glctx->Clear(GL_COLOR_BUFFER_BIT);
					break;
				}
			}
		}
		
		glctx->DepthMask(GL_TRUE);
		glctx->ClearDepthf(1.0f);
		glctx->Clear(GL_DEPTH_BUFFER_BIT);

		if (scene->has_opaque)
		{
			glctx->Disable(GL_BLEND);
			glctx->DepthMask(GL_TRUE);

			for (size_t i = 0; i < lists.simple_models.size(); i++)
			{
				SimpleModel* model = lists.simple_models[i];
				render_model(camera, /*&lights,*/ model, Pass::Opaque);
			}
			
		}
	
	}

}
