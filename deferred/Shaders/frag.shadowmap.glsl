// Shadowmap generator
// Brian Washburn
// 11-18-09

varying float Depth;

void main (void)
{
	gl_FragColor = vec4(Depth, 0.0, 0.0, 0.0);
}