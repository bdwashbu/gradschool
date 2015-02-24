// Depth Darkening x pass
// Brian Washburn
// 12-8-09

uniform sampler2D theTexture;
uniform float xSize;

void main( void )
{
	float Texel;
	float finalColor;
	vec4 finalTexel;
	float delta= 0.0;
	float temp;
	float xStep = 1.0/xSize;
	int start = -4, end = 5;
	
	float thisTexel = texture2D(theTexture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y)).a;

	for(int j = start; j < end; j++) {
		Texel = texture2D(theTexture, vec2(gl_TexCoord[0].x + xStep * float(j), gl_TexCoord[0].y)).a;
		if (abs(float(thisTexel-Texel)) > 0.08 * thisTexel) {
			continue;
		}
		temp = exp(-(abs(float(j)))) * (0.08 * thisTexel-abs(float(thisTexel-Texel)));
		finalTexel += Texel*temp;
		delta += temp;
	}

	finalTexel = finalTexel/delta;
	gl_FragColor = finalTexel;
}