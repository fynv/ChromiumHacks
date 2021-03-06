#include "ModelComponents.h"
#include <gtx/hash.hpp>
#include <unordered_set>
#include "third_party/blink/renderer/modules/three_native/three_context.h"

namespace blink {
	namespace three 
	{	
		template <typename T>
		inline glm::ivec3 t_get_indices(T* indices, int face_id)
		{
			int i0 = (int)indices[face_id * 3];
			int i1 = (int)indices[face_id * 3 + 1];
			int i2 = (int)indices[face_id * 3 + 2];
			return { i0, i1, i2 };
		}

		inline glm::ivec3 get_indices(void* indices, int type_indices, int face_id)
		{
			if (type_indices == 1)
			{
				return t_get_indices((uint8_t*)indices, face_id);
			}
			else if (type_indices == 2)
			{
				return t_get_indices((uint16_t*)indices, face_id);
			}
			else if (type_indices == 4)
			{
				return t_get_indices((uint32_t*)indices, face_id);
			}
			return {0,0,0};
		}

		void Primitive::compute_wires()
		{
			std::vector<glm::ivec2> wire_indices;
			{
				std::unordered_set<glm::ivec2> wire_index_set;

				if (index_buf != nullptr)
				{
					for (int j = 0; j < num_face; j++)
					{
						glm::ivec3 i_face = get_indices(cpu_indices->data(), type_indices, j);
						{
							glm::ivec2 i_wire = i_face.x > i_face.y ? glm::ivec2(i_face.y, i_face.x) : glm::ivec2(i_face.x, i_face.y);
							wire_index_set.insert(i_wire);
						}
						{
							glm::ivec2 i_wire = i_face.y > i_face.z ? glm::ivec2(i_face.z, i_face.y) : glm::ivec2(i_face.y, i_face.z);
							wire_index_set.insert(i_wire);
						}
						{
							glm::ivec2 i_wire = i_face.z > i_face.x ? glm::ivec2(i_face.x, i_face.z) : glm::ivec2(i_face.z, i_face.x);
							wire_index_set.insert(i_wire);
						}
					}
				}
				else
				{
					for (int j = 0; j < num_pos / 3; j++)
					{
						glm::ivec3 i_face = { j * 3, j * 3 + 1, j * 3 + 2 };
						{
							glm::ivec2 i_wire = glm::ivec2(i_face.x, i_face.y);
							wire_index_set.insert(i_wire);
						}
						{
							glm::ivec2 i_wire = glm::ivec2(i_face.y, i_face.z);
							wire_index_set.insert(i_wire);
						}
						{
							glm::ivec2 i_wire = glm::ivec2(i_face.x, i_face.z);
							wire_index_set.insert(i_wire);
						}
					}
				}
				auto iter = wire_index_set.begin();
				while (iter != wire_index_set.end())
				{
					wire_indices.push_back(*iter);
					iter++;
				}
			}
			num_wires = (int)wire_indices.size();
			wire_ind_buf = (std::unique_ptr<GLBuffer>)(new GLBuffer(sizeof(glm::ivec2) * num_wires, GL_ELEMENT_ARRAY_BUFFER));
			wire_ind_buf->upload(wire_indices.data());
		}
	}
}
