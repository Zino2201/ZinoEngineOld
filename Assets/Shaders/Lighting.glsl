// Lighting structures

// Directional light
struct DirectionalLight
{
    vec3 Position;
};

// A point light
struct PointLight
{
    vec3 Position;
    float Constant;
    float Linear;
    float Quadratic;
};