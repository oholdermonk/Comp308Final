#version 330 core

in vec3 aPosition;

out vec3 vPosition;

uniform mat4 projection;
uniform mat4 view;


void main() {

	mat4 rotView = mat4(mat3(view));
	vec4 clipPos = projection * rotView * vec4(aPosition, 1.0);

    gl_Position = clipPos.xyww;
    vPosition = aPosition;
}