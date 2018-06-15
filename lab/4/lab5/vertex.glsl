varying vec4 diffuse_0, diffuse_1, ambient_0, ambient_1;
varying vec3 normal, halfVector_0, halfVector_1;

void main()
{
    normal = normalize(gl_NormalMatrix * gl_Normal);

    halfVector_0 = gl_LightSource[0].halfVector.xyz;
    halfVector_1 = gl_LightSource[1].halfVector.xyz;

    diffuse_0 = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    diffuse_1 = gl_FrontMaterial.diffuse * gl_LightSource[1].diffuse;

    ambient_0 = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
    ambient_0 += gl_LightModel.ambient * gl_FrontMaterial.ambient;

    ambient_1 = gl_FrontMaterial.ambient * gl_LightSource[1].ambient;
    ambient_1 += gl_LightModel.ambient * gl_FrontMaterial.ambient;

    gl_Position = ftransform();
}
