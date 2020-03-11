// Material test
// Fragment shader

// Scalars
layout(set = MATERIAL_SET, binding = 0) uniform ScalarsUBO {
    float MonScalarDeOuf;
    float MonAutreScalarDeOuf;
};

// Vector3s
layout(set = MATERIAL_SET, binding = 1) uniform Vec3sUBO {
    vec3 MonVec3;
    vec3 MonAutreVec3DeOuf;
};

/** VERTEX FUNCTIONS */

// Normal
#ifdef VERTEX_SHADER
vec3 GetMaterialNormal()
{
    return InNormal;
}

// Material vertex offset
vec3 GetMaterialVertexOffset()
{
    return vec3(0.0);
}
#endif

/** FRAGMENTS FUNCTIONS */

#ifdef FRAGMENT_SHADER
// Material out color
vec3 GetMaterialOutColor()
{
    return MonVec3;
}
#endif
