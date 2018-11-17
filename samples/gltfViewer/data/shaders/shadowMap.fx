#ifdef VTX_SHADER
layout(location = 0) in vec3 vertex;
layout(location = 0) uniform mat4 world2ShadowProj;

//------------------------------------------------------------------------------
void main ( void )
{
	gl_Position = world2ShadowProj * vec4(vertex, 1.0);
	//gl_Position = vec4(vertex.xy, 0.5, 1.0);
}
#endif

#ifdef PXL_SHADER
out lowp vec3 outColor;

//------------------------------------------------------------------------------	
void main (void) {
	outColor = vec3(1.0);
}

#endif
