uniform vec2 uSize;
uniform float uCorner;
uniform float uOutline;
uniform vec4 uOutlineColor;

void main() {
	vec2 pixelCoord = (gl_TexCoord[0].xy - 0.5) * uSize;

	vec2 cornerOffset = abs(pixelCoord) - (uSize * 0.5) + vec2(uCorner);
	float distance = length(max(cornerOffset, 0.0)) + min(max(cornerOffset.x, cornerOffset.y), 0.0) - uCorner;

	float pixelWidth = fwidth(distance);

	float alpha = 1.0 - smoothstep(-pixelWidth, 0.0, distance);
	if (alpha <= 0.0)
		discard;

	vec4 finalColor = gl_Color;
	if (uOutline > 0.0)
	{
		float outlineFactor = smoothstep(-uOutline - pixelWidth, -uOutline, distance);
		finalColor = mix(finalColor, uOutlineColor, outlineFactor);
	}

	gl_FragColor = vec4(finalColor.rgb, finalColor.a * alpha);
}
