#version 330 core

in vec3 vertex;
in vec3 normal;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;
uniform mat3 NormalMatrix;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;
out vec3 color;

out vec3 vertexOut;
out vec3 normalOut;

out vec3 matambOut;
out vec3 matspecOut;
out vec3 matdiffOut;
out float matshinOut;
out vec3 NormSCO;
out vec4 ver;

void main()
{
    color = matdiff;
    matambOut = matamb;
    matspecOut = matspec;
    matshinOut = matshin;
    vertexOut = vertex;
    normalOut = normal;
    matdiffOut = matdiff;
    normalOut = NormalMatrix*normalOut;
    ver = view * TG * vec4(vertexOut, 1.0);

    gl_Position = proj * view * TG * vec4 (vertex, 1.0);

}
