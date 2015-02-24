// Display Depth
// Brian Washburn
// 12-12-09

uniform sampler2D depthTexture;

void main (void)
{
	float depth = texture2D(depthTexture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y)).a;
	
	gl_FragColor = vec4(depth, depth, depth, 0.0);
			
}