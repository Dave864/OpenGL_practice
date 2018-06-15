varying vec4 diffuse_0, diffuse_1, ambient_0, ambient_1;
varying vec3 normal, halfVector_0, halfVector_1;

void main()
{
    vec3 n, halfV, lightDir_0, lightDir_1;
    float NdotL_0, NdotL_1, NdotHV;
    vec4 color = gl_LightModel.ambient * gl_FrontMaterial.ambient;

    lightDir_0 = vec3(gl_LightSource[0].position);
    lightDir_1 = vec3(gl_LightSource[1].position);

    n = normalize(normal);
    NdotL_0 = max(dot(n, lightDir_0), 0.0);
    NdotL_1 = max(dot(n, lightDir_1), 0.0);

    if(NdotL_0 > 0.0){
	color = ambient_0;
	color += diffuse_0 * NdotL_0;
	halfV = normalize(halfVector_0);
	NdotHV = max(dot(n,halfV), 0.0);
	color += gl_FrontMaterial.specular * 
		 gl_LightSource[0].specular * 
		 pow(NdotHV, gl_FrontMaterial.shininess);
    }
    if(NdotL_1 > 0.0){
	color += ambient_1;
	color += diffuse_1 * NdotL_1;
	halfV = normalize(halfVector_1);
	NdotHV = max(dot(n,halfV), 0.0);
	color += gl_FrontMaterial.specular * 
		 gl_LightSource[1].specular * 
		 pow(NdotHV, gl_FrontMaterial.shininess);
    }
    gl_FragColor = color; 
}
