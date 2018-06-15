uniform sampler2D tex, tex_ao;
varying vec3 N;

void main()
{
	vec4 L = normalize(gl_LightSource[0].position);
	vec3 normal = normalize(N);
	
	float intensity = dot(L, normal);
	vec4 texture_0 = texture2D(tex, gl_TexCoord[0].xy);
	vec4 texture_1 = texture2D(tex_ao, gl_TexCoord[0].xy);	

	gl_FragColor.xyz = intensity*gl_FrontMaterial.diffuse.rgb + 2*gl_FrontMaterial.ambient.rgb;
	gl_FragColor.xyz *= texture_0.rgb * texture_1.rgb;
	
	gl_FragColor.a = 1.0 * texture_0.a * texture_1.a;
}
