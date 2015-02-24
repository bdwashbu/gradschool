// Depth Darkening y pass
// Brian Washburn
// 12-8-09

uniform sampler2D theTexture;
uniform sampler2D original;
uniform sampler2D litScene;
uniform sampler2D diffuse;
uniform float ySize;
vec4 lit;

void main( void )
{
	vec4 Texel;
	float finalColor;
	vec4 finalTexel;
	float delta = 0.0;
	float temp;
	float tempscaled;
	float yStep = 1.0/ySize;
	
	int start = -4, end = 5;
	
	float thisTexel = texture2D(theTexture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y)).a;

	for(int j = start; j < end; j++) {
		Texel = texture2D(theTexture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + yStep * j)).a;
		if (abs(float(thisTexel-Texel)) > 0.08 * thisTexel) {
			continue;
		}
		temp = exp(-(abs(float(j)))) * (0.08 * thisTexel-abs(float(thisTexel-Texel)));
		finalTexel += Texel*temp;
		delta += temp;
	}

	finalTexel = finalTexel/delta;
	lit = texture2D(litScene, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y));
	
	temp = finalTexel - texture2D(original, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y)).a;
	tempscaled = temp * 1000.0 * pow((1.0 - thisTexel), 3.5);
	
	if (tempscaled < 0.0) {
	
		gl_FragColor = lit - -tempscaled;
		
	} else {
	
	    gl_FragColor = lit;
	    
	}
		
}