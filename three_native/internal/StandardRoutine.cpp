#include "third_party/blink/renderer/modules/three_native/three_context.h"
#include "ModelComponents.h"
#include "StandardRoutine.h"

static const char g_vertex[] =
R"(#version 310 es
precision highp float;

#DEFINES#

layout (location = LOCATION_ATTRIB_POS) in vec3 aPos;
layout (location = LOCATION_ATTRIB_NORM) in vec3 aNorm;

layout (std140, binding = BINDING_CAMERA) uniform Camera
{
	mat4 uProjMat;
	mat4 uViewMat;	
	mat4 uInvProjMat;
	mat4 uInvViewMat;	
	vec3 uEyePos;
};

layout (std140, binding = BINDING_MODEL) uniform Model
{
	mat4 uModelMat;
	mat4 uNormalMat;
};

layout (location = LOCATION_VARYING_VIEWDIR) out vec3 vViewDir;
layout (location = LOCATION_VARYING_NORM) out vec3 vNorm;

#if HAS_COLOR
layout (location = LOCATION_ATTRIB_COLOR) in vec4 aColor;
layout (location = LOCATION_VARYING_COLOR) out vec4 vColor;
#endif

#if HAS_UV
layout (location = LOCATION_ATTRIB_UV) in vec2 aUV;
layout (location = LOCATION_VARYING_UV) out vec2 vUV;
#endif

#if HAS_NORMAL_MAP
layout (location = LOCATION_ATTRIB_TANGENT) in vec3 aTangent;
layout (location = LOCATION_VARYING_TANGENT) out vec3 vTangent;
layout (location = LOCATION_ATTRIB_BITANGENT) in vec3 aBitangent;
layout (location = LOCATION_VARYING_BITANGENT) out vec3 vBitangent;
#endif

layout (location = LOCATION_VARYING_WORLD_POS) out vec3 vWorldPos;

void main()
{
	vec4 wolrd_pos = uModelMat * vec4(aPos, 1.0);
	gl_Position = uProjMat*(uViewMat*wolrd_pos);
	vWorldPos = wolrd_pos.xyz;
	vViewDir = uEyePos - wolrd_pos.xyz;
	vec4 world_norm = uNormalMat * vec4(aNorm, 0.0);
	vNorm = world_norm.xyz;

#if HAS_COLOR
	vColor = aColor;
#endif

#if HAS_UV
	vUV = aUV;
#endif

#if HAS_NORMAL_MAP
	vec4 world_tangent = uModelMat * vec4(aTangent, 0.0);
	vTangent =  world_tangent.xyz;

	vec4 world_bitangent = uModelMat * vec4(aBitangent, 0.0);
	vBitangent =  world_bitangent.xyz;
#endif
}
)";

static const char g_frag[] =
R"(#version 310 es
precision highp float;

#DEFINES#

layout (location = LOCATION_VARYING_VIEWDIR) in vec3 vViewDir;
layout (location = LOCATION_VARYING_NORM) in vec3 vNorm;

layout (std140, binding = BINDING_MATERIAL) uniform Material
{
	vec4 uColor;
	vec4 uEmissive;
	vec2 uNormalScale;
	float uMetallicFactor;
	float uRoughnessFactor;
	float uAlphaCutoff;
	int uDoubleSided;
};

#if HAS_COLOR
layout (location = LOCATION_VARYING_COLOR) in vec4 vColor;
#endif

#if HAS_UV
layout (location = LOCATION_VARYING_UV) in vec2 vUV;
#endif

#if HAS_COLOR_TEX
layout (location = LOCATION_TEX_COLOR) uniform sampler2D uTexColor;
#endif

#if HAS_METALNESS_MAP
layout (location = LOCATION_TEX_METALNESS) uniform sampler2D uTexMetalness;
#endif

#if HAS_ROUGHNESS_MAP
layout (location = LOCATION_TEX_ROUGHNESS) uniform sampler2D uTexRoughness;
#endif

#if HAS_NORMAL_MAP
layout (location = LOCATION_TEX_NORMAL) uniform sampler2D uTexNormal;
layout (location = LOCATION_VARYING_TANGENT) in vec3 vTangent;
layout (location = LOCATION_VARYING_BITANGENT) in vec3 vBitangent;
#endif

layout (location = LOCATION_VARYING_WORLD_POS) in vec3 vWorldPos;

#if HAS_EMISSIVE_MAP
layout (location = LOCATION_TEX_EMISSIVE) uniform sampler2D uTexEmissive;
#endif

struct IncidentLight {
	vec3 color;
	vec3 direction;
	bool visible;
};

#define EPSILON 1e-6
#define RECIPROCAL_PI 0.3183098861837907

#ifndef saturate
#define saturate( a ) clamp( a, 0.0, 1.0 )
#endif

float pow2( const in float x ) { return x*x; }

struct PhysicalMaterial 
{
	vec3 diffuseColor;
	float roughness;
	vec3 specularColor;
	float specularF90;
};


vec3 F_Schlick(const in vec3 f0, const in float f90, const in float dotVH) 
{
	float fresnel = exp2( ( - 5.55473 * dotVH - 6.98316 ) * dotVH );
	return f0 * ( 1.0 - fresnel ) + ( f90 * fresnel );
}

float V_GGX_SmithCorrelated( const in float alpha, const in float dotNL, const in float dotNV ) 
{
	float a2 = pow2( alpha );
	float gv = dotNL * sqrt( a2 + ( 1.0 - a2 ) * pow2( dotNV ) );
	float gl = dotNV * sqrt( a2 + ( 1.0 - a2 ) * pow2( dotNL ) );
	return 0.5 / max( gv + gl, EPSILON );
}

float D_GGX( const in float alpha, const in float dotNH ) 
{
	float a2 = pow2( alpha );
	float denom = pow2( dotNH ) * ( a2 - 1.0 ) + 1.0; 
	return RECIPROCAL_PI * a2 / pow2( denom );
}

vec3 BRDF_Lambert(const in vec3 diffuseColor) 
{
	return RECIPROCAL_PI * diffuseColor;
}

float BRDF_Lambert_Toon()
{
	return RECIPROCAL_PI;
}

vec3 BRDF_GGX( const in vec3 lightDir, const in vec3 viewDir, const in vec3 normal, const in vec3 f0, const in float f90, const in float roughness ) 
{
	float alpha = pow2(roughness);

	vec3 halfDir = normalize(lightDir + viewDir);

	float dotNL = saturate(dot(normal, lightDir));
	float dotNV = saturate(dot(normal, viewDir));
	float dotNH = saturate(dot(normal, halfDir));
	float dotVH = saturate(dot(viewDir, halfDir));

	vec3 F = F_Schlick(f0, f90, dotVH);
	float V = V_GGX_SmithCorrelated(alpha, dotNL, dotNV);
	float D = D_GGX( alpha, dotNH );
	return F*(V*D);
}

float BRDF_GGX_Toon(const in vec3 lightDir, const in vec3 viewDir, const in vec3 normal, const in float roughness )
{
	float alpha = pow2(roughness);

	vec3 halfDir = normalize(lightDir + viewDir);

	float dotNL = saturate(dot(normal, lightDir));
	float dotNV = saturate(dot(normal, viewDir));
	float dotNH = saturate(dot(normal, halfDir));	

	float V = V_GGX_SmithCorrelated(alpha, dotNL, dotNV);
	float D = D_GGX( alpha, dotNH );
	return V*D;
}

float luminance(in vec3 color)
{
	return color.x * 0.2126 + color.y * 0.7152 + color.z *0.0722;
}

struct DirectionalLight
{
	mat4 shadowVPSBMatrix;
	vec4 color;
	vec4 direction;
	int has_shadow;
	float diffuse_thresh;
	float diffuse_high;
	float diffuse_low;
	float specular_thresh;
	float specular_high;
	float specular_low;
};

#if NUM_DIRECTIONAL_LIGHTS>0
layout (std140, binding = BINDING_DIRECTIONAL_LIGHTS) uniform DirectionalLights
{
	DirectionalLight uDirectionalLights[NUM_DIRECTIONAL_LIGHTS];
};
#endif

#if NUM_DIRECTIONAL_SHADOWS>0
layout (location = LOCATION_TEX_DIRECTIONAL_SHADOW) uniform sampler2D uDirectionalShadowTex[NUM_DIRECTIONAL_SHADOWS];

vec3 computeShadowCoords(in mat4 VPSB)
{
	vec4 shadowCoords = VPSB * vec4(vWorldPos, 1.0);
	return shadowCoords.xyz;
}

float computeShadowCoef(in mat4 VPSB, sampler2D shadowTex)
{
	vec3 shadowCoords;
	shadowCoords = computeShadowCoords(VPSB);
	float d = texture(shadowTex, shadowCoords.xy).x;
	return clamp(1.0 - (shadowCoords.z - d)*1000.0, 0.0, 1.0);
}
#endif

#if HAS_ENVIRONMENT_MAP
layout (std140, binding = BINDING_ENVIRONMEN_MAP) uniform EnvironmentMap
{
	vec4 uSHCoefficients[9];
	float uDiffuseThresh;
	float uDiffuseHigh;
	float uDiffuseLow;
	float uSpecularThresh;
	float uSpecularHigh;
	float uSpecularLow;
};

vec3 shGetIrradianceAt( in vec3 normal, in vec4 shCoefficients[ 9 ] ) {

	// normal is assumed to have unit length

	float x = normal.x, y = normal.y, z = normal.z;

	// band 0
	vec3 result = shCoefficients[ 0 ].xyz * 0.886227;

	// band 1
	result += shCoefficients[ 1 ].xyz * 2.0 * 0.511664 * y;
	result += shCoefficients[ 2 ].xyz * 2.0 * 0.511664 * z;
	result += shCoefficients[ 3 ].xyz * 2.0 * 0.511664 * x;

	// band 2
	result += shCoefficients[ 4 ].xyz * 2.0 * 0.429043 * x * y;
	result += shCoefficients[ 5 ].xyz * 2.0 * 0.429043 * y * z;
	result += shCoefficients[ 6 ].xyz * ( 0.743125 * z * z - 0.247708 );
	result += shCoefficients[ 7 ].xyz * 2.0 * 0.429043 * x * z;
	result += shCoefficients[ 8 ].xyz * 0.429043 * ( x * x - y * y );

	return result;
}

layout (location = LOCATION_TEX_REFLECTION_MAP) uniform samplerCube uReflectionMap;

vec3 GetReflectionAt(in vec3 reflectVec, in samplerCube reflectMap, float roughness)
{
	float gloss;
	if (roughness<0.053) 
	{
		gloss = 1.0;
	}
	else
	{
		float r2 = roughness*roughness;
		float r4 = r2*r2;
		gloss = log(2.0/r4 - 1.0)/log(2.0)/18.0;
	}

	float mip = (1.0-gloss)*6.0;
	return textureLod(reflectMap, reflectVec, mip).xyz;
}

#endif

#if HAS_AMBIENT_LIGHT
layout (std140, binding = BINDING_AMBIENT_LIGHT) uniform AmbientLight
{
	vec4 uAmbientColor;
	float uDiffuseThresh;
	float uDiffuseHigh;
	float uDiffuseLow;
	float uSpecularThresh;
	float uSpecularHigh;
	float uSpecularLow;
};
#endif

#if HAS_HEMISPHERE_LIGHT
layout (std140, binding = BINDING_HEMISPHERE_LIGHT) uniform HemisphereLight
{
	vec4 uHemisphereSkyColor;
	vec4 uHemisphereGroundColor;
	float uDiffuseThresh;
	float uDiffuseHigh;
	float uDiffuseLow;
	float uSpecularThresh;
	float uSpecularHigh;
	float uSpecularLow;
};

vec3 HemisphereColor(in vec3 dir)
{
	float k = dir.y * 0.5 + 0.5;
	return mix( uHemisphereGroundColor.xyz, uHemisphereSkyColor.xyz, k);
}
#endif

layout (location = 0) out vec4 out0;

#if ALPHA_BLEND
layout (location = 1) out float out1;
#endif

void main()
{
	vec4 base_color = uColor;
#if HAS_COLOR
	base_color *= vColor;
#endif

#if HAS_COLOR_TEX
	base_color *= texture(uTexColor, vUV);
#endif

#if ALPHA_MASK
	base_color.w = base_color.w > uAlphaCutoff ? 1.0 : 0.0;
#endif

	if (base_color.w == 0.0) discard;

	float metallicFactor = uMetallicFactor;
	float roughnessFactor = uRoughnessFactor;

#if HAS_METALNESS_MAP
	metallicFactor *= texture(uTexMetalness, vUV).z;
#endif

#if HAS_ROUGHNESS_MAP
	roughnessFactor *= texture(uTexRoughness, vUV).y;
#endif

	vec3 viewDir = normalize(vViewDir);
	vec3 norm = normalize(vNorm);

#if HAS_NORMAL_MAP
	{
		vec3 T = normalize(vTangent);
		vec3 B = normalize(vBitangent);
		vec3 bump =  texture(uTexNormal, vUV).xyz;
		bump = (2.0 * bump - 1.0) * vec3(uNormalScale.x, uNormalScale.y, 1.0);
		norm = normalize(bump.x*T + bump.y*B + bump.z*norm);
	}
#endif

	if (uDoubleSided!=0)
	{
		if (dot(viewDir,norm)<0.0) norm = -norm;
	}

	PhysicalMaterial material;
	material.diffuseColor = base_color.xyz * ( 1.0 - metallicFactor );

	vec3 dxy = max(abs(dFdx(norm)), abs(dFdy(norm)));
	float geometryRoughness = max(max(dxy.x, dxy.y), dxy.z);

	material.roughness = max( roughnessFactor, 0.0525 );
	material.roughness += geometryRoughness;
	material.roughness = min( material.roughness, 1.0 );
	
	material.specularColor = mix( vec3( 0.04 ), base_color.xyz, metallicFactor );
	material.specularF90 = 1.0;

	vec3 emissive = uEmissive.xyz;
#if HAS_EMISSIVE_MAP
	emissive *= texture(uTexEmissive, vUV).xyz;
#endif

	vec3 specular = vec3(0.0);
	vec3 diffuse = vec3(0.0);

#if NUM_DIRECTIONAL_LIGHTS>0
	int shadow_id = 0;
	for (int i=0; i< NUM_DIRECTIONAL_LIGHTS; i++)
	{	
		DirectionalLight light_source = uDirectionalLights[i];
		float l_shadow = 1.0;
#if NUM_DIRECTIONAL_SHADOWS>0
		if (light_source.has_shadow!=0)
		{
			l_shadow = computeShadowCoef(light_source.shadowVPSBMatrix, uDirectionalShadowTex[shadow_id]);
			shadow_id++;
		}
#endif
		IncidentLight directLight = IncidentLight(light_source.color.xyz * l_shadow, light_source.direction.xyz, true);	

		float dotNL =  saturate(dot(norm, directLight.direction));
		vec3 irradiance = dotNL * directLight.color;

#if (TONE_SHADING & 1) == 0
		diffuse += irradiance * BRDF_Lambert( material.diffuseColor );
#else
		float diffuse_thresh = light_source.diffuse_thresh;
		float diffuse_high = light_source.diffuse_high;
		float diffuse_low = light_source.diffuse_low;

		vec3 diffuse_light = irradiance * BRDF_Lambert_Toon();
		float lum_diffuse = luminance(diffuse_light);
		if (lum_diffuse > diffuse_thresh)
		{
			diffuse_light *= diffuse_high/lum_diffuse;			
		}
		else if (lum_diffuse>0.0)
		{
			diffuse_light *= diffuse_low/lum_diffuse;
		}
		else
		{
			diffuse_light = vec3(diffuse_low);
		}

		diffuse += diffuse_light* material.diffuseColor;
#endif

#if (TONE_SHADING & 2) == 0
		specular += irradiance * BRDF_GGX( directLight.direction, viewDir, norm, material.specularColor, material.specularF90, material.roughness );
#else
		float specular_thresh = light_source.specular_thresh;
		float specular_high =  light_source.specular_high;
		float specular_low =  light_source.specular_low;

		vec3 specular_light = irradiance * BRDF_GGX_Toon(directLight.direction, viewDir, norm, material.roughness);
		float lum_specular = luminance(specular_light);
		if (lum_specular > specular_thresh)
		{
			specular_light *= specular_high/lum_specular;
		}
		else if (lum_specular>0.0)
		{
			specular_light *= specular_low/lum_specular;
		}
		else
		{
			specular_light = vec3(specular_low);
		}
		specular += specular_light* material.specularColor;
#endif
	}
#endif

	{
		vec3 light_dir = normalize(vec3(1.0, 2.0, 1.5));
		IncidentLight directLight = IncidentLight(vec3(4.0, 4.0, 4.0), light_dir, true);	

		float dotNL =  saturate(dot(norm, directLight.direction));
		vec3 irradiance = dotNL * directLight.color;

		diffuse += irradiance * BRDF_Lambert( material.diffuseColor );
		specular += irradiance * BRDF_GGX( directLight.direction, viewDir, norm, material.specularColor, material.specularF90, material.roughness );
	}

#if HAS_INDIRECT_LIGHT
	{
		vec3 reflectVec = reflect(-viewDir, norm);
		reflectVec = normalize( mix( reflectVec, norm, material.roughness * material.roughness) );
#if HAS_ENVIRONMENT_MAP
		vec3 irradiance = shGetIrradianceAt(norm, uSHCoefficients);		
		vec3 radiance = GetReflectionAt(reflectVec, uReflectionMap, material.roughness);
#elif HAS_AMBIENT_LIGHT
		vec3 irradiance = uAmbientColor.xyz;
		vec3 radiance = irradiance;
#elif HAS_HEMISPHERE_LIGHT
		vec3 irradiance = HemisphereColor(norm);
		vec3 radiance = HemisphereColor(reflectVec);
#endif

#if (TONE_SHADING & 4) == 0
		diffuse += material.diffuseColor * irradiance * RECIPROCAL_PI;
#else
		float diffuse_thresh = uDiffuseThresh;
		float diffuse_high = uDiffuseHigh;
		float diffuse_low = uDiffuseLow;
		
		vec3 diffuse_light =  irradiance * BRDF_Lambert_Toon();
		float lum_diffuse = luminance(diffuse_light);
		if (lum_diffuse > diffuse_thresh)
		{
			diffuse_light *= diffuse_high/lum_diffuse;			
		}
		else if (lum_diffuse>0.0)
		{
			diffuse_light *= diffuse_low/lum_diffuse;
		}
		else
		{
			diffuse_light = vec3(diffuse_low);
		}
		
		diffuse += diffuse_light* material.diffuseColor;
#endif

#if (TONE_SHADING & 8) == 0
		specular +=  material.specularColor * radiance;
#else		
		float specular_thresh = uSpecularThresh;
		float specular_high = uSpecularHigh;
		float specular_low = uSpecularLow;

		vec3 specular_light = radiance;
		float lum_specular = luminance(specular_light);
		if (lum_specular > specular_thresh)
		{
			specular_light *= specular_high/lum_specular;
		}
		else if (lum_specular>0.0)
		{
			specular_light *= specular_low/lum_specular;
		}
		else
		{
			specular_light = vec3(specular_low);
		}
		specular += specular_light* material.specularColor;		

#endif
	}
#endif

	vec3 col = emissive + specular;
#if !IS_HIGHTLIGHT
	col += diffuse;
#endif
	col = clamp(col, 0.0, 1.0);	

#if ALPHA_BLEND
	float alpha = base_color.w;
	float a = min(1.0, alpha) * 8.0 + 0.01;
	float b = -gl_FragCoord.z * 0.95 + 1.0;
	float weight = clamp(a * a * a * 1e8 * b * b * b, 1e-2, 3e2);
	out0 = vec4(col * alpha, alpha) * weight;
	out1 = alpha;
#elif IS_HIGHTLIGHT
	out0 = vec4(col, 0.0);
#else
	out0 = vec4(col, 1.0);
#endif
}
)";

namespace blink 
{
	namespace three 
	{
		inline void replace(std::string& str, const char* target, const char* source)
		{
			size_t start = 0;
			size_t target_len = strlen(target);
			size_t source_len = strlen(source);
			while (true)
			{
				size_t pos = str.find(target, start);
				if (pos == std::string::npos) break;
				str.replace(pos, target_len, source);
				start = pos + source_len;
			}
		}
		
		
		void StandardRoutine::s_generate_shaders(const Options& options, Bindings& bindings, std::string& s_vertex, std::string& s_frag)
		{
			
			s_vertex = g_vertex;
			s_frag = g_frag;
			
			std::string defines = "";

			{
				bindings.location_attrib_pos = 0;
				{
					char line[64];
					sprintf(line, "#define LOCATION_ATTRIB_POS %d\n", bindings.location_attrib_pos);
					defines += line;
				}
			}

			{
				bindings.location_attrib_norm = bindings.location_attrib_pos+1;
				{
					char line[64];
					sprintf(line, "#define LOCATION_ATTRIB_NORM %d\n", bindings.location_attrib_norm);
					defines += line;
				}
			}

			{
				bindings.binding_camera = 0;
				{
					char line[64];
					sprintf(line, "#define BINDING_CAMERA %d\n", bindings.binding_camera);
					defines += line;
				}
			}

			{
				bindings.binding_model = bindings.binding_camera + 1;
				{
					char line[64];
					sprintf(line, "#define BINDING_MODEL %d\n", bindings.binding_model);
					defines += line;
				}
			}

			{
				bindings.location_varying_viewdir = 0;
				{
					char line[64];
					sprintf(line, "#define LOCATION_VARYING_VIEWDIR %d\n", bindings.location_varying_viewdir);
					defines += line;
				}
			}

			{
				bindings.location_varying_norm = bindings.location_varying_viewdir + 1;
				{
					char line[64];
					sprintf(line, "#define LOCATION_VARYING_NORM %d\n", bindings.location_varying_norm);
					defines += line;
				}
			}


			if (options.alpha_mode == AlphaMode::Mask)
			{
				defines += "#define ALPHA_MASK 1\n";
			}
			else
			{
				defines += "#define ALPHA_MASK 0\n";
			}

			if (options.alpha_mode == AlphaMode::Blend)
			{
				if (options.is_highlight_pass)
				{
					defines += "#define ALPHA_BLEND 0\n";
					defines += "#define IS_HIGHTLIGHT 1\n";
				}
				else
				{
					defines += "#define ALPHA_BLEND 1\n";
					defines += "#define IS_HIGHTLIGHT 0\n";
				}
			}
			else
			{
				defines += "#define ALPHA_BLEND 0\n";
				defines += "#define IS_HIGHTLIGHT 0\n";
			}

			{
				bindings.binding_material = bindings.binding_model + 1;
				{
					char line[64];
					sprintf(line, "#define BINDING_MATERIAL %d\n", bindings.binding_material);
					defines += line;
				}
			}

			if (options.has_color)
			{
				defines += "#define HAS_COLOR 1\n";
				bindings.location_attrib_color = bindings.location_attrib_norm + 1;
				bindings.location_varying_color = bindings.location_varying_norm + 1;

				{
					char line[64];
					sprintf(line, "#define LOCATION_ATTRIB_COLOR %d\n", bindings.location_attrib_color);
					defines += line;
				}

				{
					char line[64];
					sprintf(line, "#define LOCATION_VARYING_COLOR %d\n", bindings.location_varying_color);
					defines += line;
				}
			}
			else
			{
				defines += "#define HAS_COLOR 0\n";
				bindings.location_attrib_color = bindings.location_attrib_norm;
				bindings.location_varying_color = bindings.location_varying_norm;
			}

			bool has_uv = options.has_color_texture || options.has_metalness_map || options.has_roughness_map || options.has_normal_map || options.has_emissive_map;

			if (has_uv)
			{
				defines += "#define HAS_UV 1\n";
				bindings.location_attrib_uv = bindings.location_attrib_color + 1;
				bindings.location_varying_uv = bindings.location_varying_color + 1;
				{
					char line[64];
					sprintf(line, "#define LOCATION_ATTRIB_UV %d\n", bindings.location_attrib_uv);
					defines += line;
				}
				{
					char line[64];
					sprintf(line, "#define LOCATION_VARYING_UV %d\n", bindings.location_varying_uv);
					defines += line;
				}		
			}
			else
			{
				defines += "#define HAS_UV 0\n";
				bindings.location_attrib_uv = bindings.location_attrib_color;
				bindings.location_varying_uv = bindings.location_varying_color;
			}
			
			if (options.has_color_texture)
			{
				defines += "#define HAS_COLOR_TEX 1\n";

				bindings.location_tex_color = 0;
				{
					char line[64];
					sprintf(line, "#define LOCATION_TEX_COLOR %d\n", bindings.location_tex_color);
					defines += line;
				}
			}
			else
			{
				defines += "#define HAS_COLOR_TEX 0\n";
				bindings.location_tex_color = -1;
			}

			if (options.has_metalness_map)
			{
				defines += "#define HAS_METALNESS_MAP 1\n";

				bindings.location_tex_metalness = bindings.location_tex_color + 1;
				{
					char line[64];
					sprintf(line, "#define LOCATION_TEX_METALNESS %d\n", bindings.location_tex_metalness);
					defines += line;
				}
			}
			else
			{
				defines += "#define HAS_METALNESS_MAP 0\n";
				bindings.location_tex_metalness = bindings.location_tex_color;
			}

			if (options.has_roughness_map)
			{
				defines += "#define HAS_ROUGHNESS_MAP 1\n";
				bindings.location_tex_roughness = bindings.location_tex_metalness + 1;
				{
					char line[64];
					sprintf(line, "#define LOCATION_TEX_ROUGHNESS %d\n", bindings.location_tex_roughness);
					defines += line;
				}
			}
			else
			{
				defines += "#define HAS_ROUGHNESS_MAP 0\n";
				bindings.location_tex_roughness = bindings.location_tex_metalness;
			}

			if (options.has_normal_map)
			{
				defines += "#define HAS_NORMAL_MAP 1\n";
				bindings.location_tex_normal = bindings.location_tex_roughness + 1;
				bindings.location_attrib_tangent = bindings.location_attrib_uv + 1;
				bindings.location_varying_tangent = bindings.location_varying_uv + 1;
				bindings.location_attrib_bitangent = bindings.location_attrib_tangent + 1;
				bindings.location_varying_bitangent = bindings.location_varying_tangent + 1;

				{
					char line[64];
					sprintf(line, "#define LOCATION_TEX_NORMAL %d\n", bindings.location_tex_normal);
					defines += line;
				}

				{
					char line[64];
					sprintf(line, "#define LOCATION_ATTRIB_TANGENT %d\n", bindings.location_attrib_tangent);
					defines += line;
				}
				{
					char line[64];
					sprintf(line, "#define LOCATION_VARYING_TANGENT %d\n", bindings.location_varying_tangent);
					defines += line;
				}
				{
					char line[64];
					sprintf(line, "#define LOCATION_ATTRIB_BITANGENT %d\n", bindings.location_attrib_bitangent);
					defines += line;
				}
				{
					char line[64];
					sprintf(line, "#define LOCATION_VARYING_BITANGENT %d\n", bindings.location_varying_bitangent);
					defines += line;
				}
			}
			else
			{
				defines += "#define HAS_NORMAL_MAP 0\n";
				bindings.location_tex_normal = bindings.location_tex_roughness;
				bindings.location_attrib_tangent = bindings.location_attrib_uv;
				bindings.location_varying_tangent = bindings.location_varying_uv;
				bindings.location_attrib_bitangent = bindings.location_attrib_tangent;
				bindings.location_varying_bitangent = bindings.location_varying_tangent;
			}

			{
				bindings.location_varying_world_pos = bindings.location_varying_bitangent + 1;
				{
					char line[64];
					sprintf(line, "#define LOCATION_VARYING_WORLD_POS %d\n", bindings.location_varying_world_pos);
					defines += line;
				}
			}

			if (options.has_emissive_map)
			{
				defines += "#define HAS_EMISSIVE_MAP 1\n";
				bindings.location_tex_emissive = bindings.location_tex_normal + 1;
				{
					char line[64];
					sprintf(line, "#define LOCATION_TEX_EMISSIVE %d\n", bindings.location_tex_emissive);
					defines += line;
				}
			}
			else
			{
				defines += "#define HAS_EMISSIVE_MAP 0\n";
				bindings.location_tex_emissive = bindings.location_tex_normal;
			}

			{
				char line[64];
				sprintf(line, "#define NUM_DIRECTIONAL_LIGHTS %d\n", options.num_directional_lights);
				defines += line;
			}

			if (options.num_directional_lights > 0)
			{
				bindings.binding_directional_lights = bindings.binding_material + 1;
				{
					char line[64];
					sprintf(line, "#define BINDING_DIRECTIONAL_LIGHTS %d\n", bindings.binding_directional_lights);
					defines += line;
				}
			}
			else
			{
				bindings.binding_directional_lights = bindings.binding_material;
			}

			{
				char line[64];
				sprintf(line, "#define NUM_DIRECTIONAL_SHADOWS %d\n", options.num_directional_shadows);
				defines += line;
			}

			bindings.location_tex_directional_shadow = bindings.location_tex_emissive + options.num_directional_shadows;

			if (options.num_directional_shadows > 0)
			{		
				char line[64];
				sprintf(line, "#define LOCATION_TEX_DIRECTIONAL_SHADOW %d\n", bindings.location_tex_directional_shadow - options.num_directional_shadows + 1);
				defines += line;
			}

			bool has_indirect_light = options.has_environment_map || options.has_ambient_light || options.has_hemisphere_light;
			if (has_indirect_light)
			{
				defines += "#define HAS_INDIRECT_LIGHT 1\n";
			}
			else
			{
				defines += "#define HAS_INDIRECT_LIGHT 0\n";
			}

			if (options.has_environment_map)
			{
				defines += "#define HAS_ENVIRONMENT_MAP 1\n";
				bindings.binding_environment_map = bindings.binding_directional_lights + 1;
				bindings.location_tex_reflection_map = bindings.location_tex_directional_shadow + 1;
				{
					char line[64];
					sprintf(line, "#define BINDING_ENVIRONMEN_MAP %d\n", bindings.binding_environment_map);
					defines += line;
				}
				{
					char line[64];
					sprintf(line, "#define LOCATION_TEX_REFLECTION_MAP %d\n", bindings.location_tex_reflection_map);
					defines += line;
				}
			}
			else
			{
				defines += "#define HAS_ENVIRONMENT_MAP 0\n";
				bindings.binding_environment_map = bindings.binding_directional_lights;
				bindings.location_tex_reflection_map = bindings.location_tex_directional_shadow;
			}

			if (options.has_ambient_light)
			{
				defines += "#define HAS_AMBIENT_LIGHT 1\n";
				bindings.binding_ambient_light = bindings.binding_environment_map + 1;
				{
					char line[64];
					sprintf(line, "#define BINDING_AMBIENT_LIGHT %d\n", bindings.binding_ambient_light);
					defines += line;
				}
			}
			else
			{
				defines += "#define HAS_AMBIENT_LIGHT 0\n";
				bindings.binding_ambient_light = bindings.binding_environment_map;
			}

			if (options.has_hemisphere_light)
			{
				defines += "#define HAS_HEMISPHERE_LIGHT 1\n";
				bindings.binding_hemisphere_light = bindings.binding_ambient_light + 1;
				{
					char line[64];
					sprintf(line, "#define BINDING_HEMISPHERE_LIGHT %d\n", bindings.binding_hemisphere_light);
					defines += line;
				}
			}
			else
			{
				defines += "#define HAS_HEMISPHERE_LIGHT 0\n";
				bindings.binding_hemisphere_light = bindings.binding_ambient_light;
			}

			{
				char line[64];
				sprintf(line, "#define TONE_SHADING %d\n", options.tone_shading);
				defines += line;
			}

			replace(s_vertex, "#DEFINES#", defines.c_str());
			replace(s_frag, "#DEFINES#", defines.c_str());
		}
		
		StandardRoutine::StandardRoutine(const Options& options) : m_options(options)
		{
			std::string s_vertex, s_frag;
			s_generate_shaders(options, m_bindings, s_vertex, s_frag);
			
			m_vert_shader = std::unique_ptr<GLShader>(new GLShader(GL_VERTEX_SHADER, s_vertex.c_str()));
			m_frag_shader = std::unique_ptr<GLShader>(new GLShader(GL_FRAGMENT_SHADER, s_frag.c_str()));
			m_prog = (std::unique_ptr<GLProgram>)(new GLProgram(*m_vert_shader, *m_frag_shader));
		}
		
		void StandardRoutine::render(const RenderParams& params)
		{
			auto *glctx = ThreeContext::m_current_context->ContextGL();			
			
			const MeshStandardMaterial& material = *(MeshStandardMaterial*)params.material_list[params.primitive->material_idx];
			const GeometrySet& geo = params.primitive->geometry[params.primitive->geometry.size() - 1];

			glctx->Enable(GL_DEPTH_TEST);	
			glctx->DepthFunc(GL_LEQUAL);

			if (material.doubleSided)
			{
				glctx->Disable(GL_CULL_FACE);
			}
			else
			{
				glctx->Enable(GL_CULL_FACE);
			}

			glctx->UseProgram(m_prog->m_id);
			glctx->BindBufferBase(GL_UNIFORM_BUFFER, m_bindings.binding_camera, params.constant_camera->m_id);
			glctx->BindBufferBase(GL_UNIFORM_BUFFER, m_bindings.binding_model, params.constant_model->m_id);
			glctx->BindBufferBase(GL_UNIFORM_BUFFER, m_bindings.binding_material, material.constant_material.m_id);

			/*if (m_options.num_directional_lights > 0)
			{		
				glctx->BindBufferBase(GL_UNIFORM_BUFFER, m_bindings.binding_directional_lights, params.lights->constant_directional_lights->m_id);
			} 

			if (m_options.has_environment_map)
			{
				glctx->BindBufferBase(GL_UNIFORM_BUFFER, m_bindings.binding_environment_map, params.lights->environment_map->m_constant.m_id);
			}

			if (m_options.has_ambient_light)
			{
				glctx->BindBufferBase(GL_UNIFORM_BUFFER, m_bindings.binding_ambient_light, params.lights->ambient_light->m_constant.m_id);
			}

			if (m_options.has_hemisphere_light)
			{
				glctx->BindBufferBase(GL_UNIFORM_BUFFER, m_bindings.binding_hemisphere_light, params.lights->hemisphere_light->m_constant.m_id);
			}*/

			glctx->BindBuffer(GL_ARRAY_BUFFER, geo.pos_buf->m_id);
			glctx->VertexAttribPointer(m_bindings.location_attrib_pos, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
			glctx->EnableVertexAttribArray(m_bindings.location_attrib_pos);

			glctx->BindBuffer(GL_ARRAY_BUFFER, geo.normal_buf->m_id);
			glctx->VertexAttribPointer(m_bindings.location_attrib_norm, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
			glctx->EnableVertexAttribArray(m_bindings.location_attrib_norm);

			if (m_options.has_color)
			{
				glctx->BindBuffer(GL_ARRAY_BUFFER, params.primitive->color_buf->m_id);
				glctx->VertexAttribPointer(m_bindings.location_attrib_color, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
				glctx->EnableVertexAttribArray(m_bindings.location_attrib_color);
			}

			bool has_uv = m_options.has_color_texture || m_options.has_metalness_map || m_options.has_roughness_map || m_options.has_normal_map || m_options.has_emissive_map;
			if (has_uv)
			{
				glctx->BindBuffer(GL_ARRAY_BUFFER, params.primitive->uv_buf->m_id);
				glctx->VertexAttribPointer(m_bindings.location_attrib_uv, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
				glctx->EnableVertexAttribArray(m_bindings.location_attrib_uv);
			}

			if (m_options.has_normal_map)
			{
				glctx->BindBuffer(GL_ARRAY_BUFFER, geo.tangent_buf->m_id);
				glctx->VertexAttribPointer(m_bindings.location_attrib_tangent, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
				glctx->EnableVertexAttribArray(m_bindings.location_attrib_tangent);

				glctx->BindBuffer(GL_ARRAY_BUFFER, geo.bitangent_buf->m_id);
				glctx->VertexAttribPointer(m_bindings.location_attrib_bitangent, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
				glctx->EnableVertexAttribArray(m_bindings.location_attrib_bitangent);
			}

			if (m_options.has_color_texture)
			{
				const GLTexture2D& tex = *params.tex_list[material.tex_idx_map];
				glctx->ActiveTexture(GL_TEXTURE0 + m_bindings.location_tex_color);
				glctx->BindTexture(GL_TEXTURE_2D, tex.tex_id);
				glctx->Uniform1i(m_bindings.location_tex_color, m_bindings.location_tex_color);
			}

			if (m_options.has_metalness_map)
			{
				const GLTexture2D& tex = *params.tex_list[material.tex_idx_metalnessMap];
				glctx->ActiveTexture(GL_TEXTURE0 + m_bindings.location_tex_metalness);
				glctx->BindTexture(GL_TEXTURE_2D, tex.tex_id);
				glctx->Uniform1i(m_bindings.location_tex_metalness, m_bindings.location_tex_metalness);
			}

			if (m_options.has_roughness_map)
			{
				const GLTexture2D& tex = *params.tex_list[material.tex_idx_roughnessMap];
				glctx->ActiveTexture(GL_TEXTURE0 + m_bindings.location_tex_roughness);
				glctx->BindTexture(GL_TEXTURE_2D, tex.tex_id);
				glctx->Uniform1i(m_bindings.location_tex_roughness, m_bindings.location_tex_roughness);
			}

			if (m_options.has_normal_map)
			{
				const GLTexture2D& tex = *params.tex_list[material.tex_idx_normalMap];
				glctx->ActiveTexture(GL_TEXTURE0 + m_bindings.location_tex_normal);
				glctx->BindTexture(GL_TEXTURE_2D, tex.tex_id);
				glctx->Uniform1i(m_bindings.location_tex_normal, m_bindings.location_tex_normal);
			}

			if (m_options.has_emissive_map)
			{
				const GLTexture2D& tex = *params.tex_list[material.tex_idx_emissiveMap];
				glctx->ActiveTexture(GL_TEXTURE0 + m_bindings.location_tex_emissive);
				glctx->BindTexture(GL_TEXTURE_2D, tex.tex_id);
				glctx->Uniform1i(m_bindings.location_tex_emissive, m_bindings.location_tex_emissive);
			}	

			/*if (m_options.num_directional_shadows > 0)
			{
				int start_idx = m_bindings.location_tex_directional_shadow - m_options.num_directional_shadows + 1;
				std::vector<int> values(m_options.num_directional_shadows);
				for (int i = 0; i < m_options.num_directional_shadows; i++)
				{
					glctx->ActiveTexture(GL_TEXTURE0 + start_idx + i);
					glctx->BindTexture(GL_TEXTURE_2D, params.lights->directional_shadow_texs[i]);
					values[i] = start_idx + i;
				}
				glctx->Uniform1iv(start_idx, m_options.num_directional_shadows, values.data());
			}	

			if (m_options.has_environment_map)
			{
				glctx->ActiveTexture(GL_TEXTURE0 + m_bindings.location_tex_reflection_map);
				glctx->BindTexture(GL_TEXTURE_CUBE_MAP, params.lights->environment_map->id_cube_reflection);
				glctx->Uniform1i(m_bindings.location_tex_reflection_map, m_bindings.location_tex_reflection_map);
			}*/

			if (params.primitive->index_buf != nullptr)
			{
				glctx->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, params.primitive->index_buf->m_id);
				if (params.primitive->type_indices == 1)
				{
					glctx->DrawElements(GL_TRIANGLES, params.primitive->num_face * 3, GL_UNSIGNED_BYTE, nullptr);
				}
				else if (params.primitive->type_indices == 2)
				{
					glctx->DrawElements(GL_TRIANGLES, params.primitive->num_face * 3, GL_UNSIGNED_SHORT, nullptr);
				}
				else if (params.primitive->type_indices == 4)
				{
					glctx->DrawElements(GL_TRIANGLES, params.primitive->num_face * 3, GL_UNSIGNED_INT, nullptr);
				}
			}
			else
			{
				glctx->DrawArrays(GL_TRIANGLES, 0, params.primitive->num_pos);
			}

			glctx->UseProgram(0);
		}
	}
}

