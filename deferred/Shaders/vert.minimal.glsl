// Minimal vertex shader
// Brian Washburn
// 10-20-9


void main()
{	

	gl_Position = ftransform();

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] =  vec4(gl_Normal, 0.0);
	
}