#pragma once

#include <memory>
#include <vector>
#include <glm.hpp>
#include <gtx/quaternion.hpp>
#include "third_party/blink/renderer/modules/three_native/internal/GLUtils.h"

namespace blink 
{
	namespace three 
	{
		typedef std::unique_ptr<GLBuffer> Attribute;
		typedef std::unique_ptr<GLBuffer> Index;

		struct GeometrySet
		{
			Attribute pos_buf;
			Attribute normal_buf;
			Attribute tangent_buf;
			Attribute bitangent_buf;
		};

		class Primitive
		{
		public:
			int num_pos = 0;	
			std::vector<GeometrySet> geometry;
			Attribute color_buf;
			Attribute uv_buf;
			Attribute joints_buf;
			Attribute weights_buf;

			int num_face = 0;
			int type_indices = 2; // 1:uchar; 2: ushort; 4: uint
			Index index_buf;

			int num_wires;
			Index wire_ind_buf;
			void compute_wires();

			int num_targets = 0;
			GeometrySet targets;

			int material_idx = -1;

			// keep a cpu copy for ray-cast
			std::unique_ptr<std::vector<glm::vec4>> cpu_pos;
			std::unique_ptr<std::vector<uint8_t>> cpu_indices;
		};
	}
}
