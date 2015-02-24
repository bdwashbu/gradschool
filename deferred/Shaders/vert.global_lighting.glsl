// Global light
// Brian Washburn
// 10-20-9

varying vec3 theLightDir;
uniform vec3 lightDir;
varying vec3 LightPos;
//uniform vec3 LightCoord;

void main()
{	

	theLightDir = gl_ModelViewMatrix * vec4(lightDir, 0.0);
	//LightPos = gl_ModelViewMatrix * vec4(LightCoord, 1.0);

	gl_Position = gl_ProjectionMatrix * gl_Vertex;

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] =  vec4(gl_Normal, 0.0);

}