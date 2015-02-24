// Shadow log filtering x pass
// Brian Washburn
// 12-12-09

uniform sampler2D theTexture;
uniform float step_x;
uniform int kernelSize;

float log_conv ( float x0, float X, float y0, float Y )
{
    return (X + log(x0 + (y0 * exp(Y - X))));
}

void main( void )
{
	vec4 sum = vec4(0.0);
	vec4 tmp = vec4(0.0);
	float half = kernelSize / 2.0;
	
	int i = 0;
	float weight = 1.0 / float(kernelSize);  
    
    vec4 val1 = texture2D(theTexture, vec2(gl_TexCoord[0].s + step_x * float(half - kernelSize), gl_TexCoord[0].t));
    vec4 val2 = texture2D(theTexture, vec2(gl_TexCoord[0].s + step_x * float(half - kernelSize + 1), gl_TexCoord[0].t));
    
	sum = (val1 + log(weight + (weight * exp(val2 - val1))));
	for (i = half - kernelSize + 2; i < half; i++) {
		tmp = texture2D(theTexture, vec2(gl_TexCoord[0].s + step_x * float(i), gl_TexCoord[0].t));
		sum = (sum + log(1.0 + (weight * exp(tmp - sum))));
	}

	gl_FragColor = vec4(sum.rg, texture2D(theTexture, vec2(gl_TexCoord[0].st)).ba);
}