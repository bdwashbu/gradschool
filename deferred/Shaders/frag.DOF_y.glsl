// Depth of field y pass
// Brian Washburn
// 12-12-09

uniform sampler2D linearDepth;
uniform sampler2D currentFrame;
uniform float stepY;
uniform int kernelSize;

void main( void )
{
	float centerDepth = texture2D(linearDepth, vec2(0.5, 0.5)).a;
	float thisDepth = texture2D(linearDepth, gl_TexCoord[0].st).a;
	vec4 thisColor = texture2D(currentFrame, gl_TexCoord[0].st);

	float blurriness = abs(centerDepth - thisDepth) * 2.0  * kernelSize;
	
	int count = 1;
	
	for (float i = 0.0; i < blurriness; i++) {
	
		float otherDepth = texture2D(linearDepth, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t + stepY * i - stepY * blurriness * 0.5)).a;
	
		if (abs(float(otherDepth - thisDepth)) < 0.05 || (abs(centerDepth - thisDepth) > 0.1 && otherDepth > thisDepth)) {
			count += 1;
			thisColor += texture2D(currentFrame, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t + stepY * i - stepY * blurriness * 0.5));
		}
	
	}

	gl_FragColor = thisColor / count;
}