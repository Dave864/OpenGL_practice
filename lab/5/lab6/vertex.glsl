uniform sampler2D tex, tex_ao;
varying vec3 N;


void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	N = gl_NormalMatrix*gl_Normal;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}
