uniform float uRadius;
uniform float uOutline;
uniform vec4 uOutlineColor;

void main() {
	vec2 pixelCoord = (gl_TexCoord[0].xy - 0.5) * uRadius * 2.0;

	float distance = length(pixelCoord) - uRadius;
	float pixelWidth = fwidth(distance);

	float alpha = 1.0 - smoothstep(-pixelWidth, 0.0, distance);
	if (alpha <= 0.0)
		discard;

	vec4 finalColor = gl_Color;
	if (uOutline > 0.0)
	{
		float outlineFactor = smoothstep(-uOutline - pixelWidth, -uOutline, distance);
		finalColor = mix(gl_Color, uOutlineColor, outlineFactor);
	}

	gl_FragColor = vec4(finalColor.rgb, finalColor.a * alpha);
}
