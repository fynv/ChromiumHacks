#pragma once

namespace blink 
{
	namespace three 
	{
		class Material
		{
		public:
			Material() {}
			virtual ~Material() {}
		};

		enum class MaterialType
		{
			None,
			MeshStandardMaterial
		};
	}
}

