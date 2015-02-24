// Differed Shading
// Brian Washburn
// 10-20-9

void main()
{	

	gl_Position = ftransform();

	gl_TexCoord[0] = gl_MultiTexCoord0;

}