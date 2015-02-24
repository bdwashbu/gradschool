// Shadow Mapping - writes to the shadowmap
// Brian Washburn
// 11-18-09

varying float Depth;

void main (void)
{
	gl_Position = ftransform();
	Depth = length((gl_ModelViewMatrix * gl_Vertex).xyz) / 200.0;

	gl_TexCoord[0] = gl_MultiTexCoord0;
}