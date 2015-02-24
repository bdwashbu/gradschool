// G-buffer render
// Brian Washburn
// 10-20-9

varying vec3 normal;

void main()
{	
	normal = normalize(gl_NormalMatrix * gl_Normal);

	gl_Position = ftransform();
	
}