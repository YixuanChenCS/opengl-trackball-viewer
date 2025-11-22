#version 120
attribute vec4 vPosition;
attribute vec4 vColor;
uniform   mat4 ctm;
varying   vec4 color;

void main() {
    gl_Position = ctm * vPosition;
    color = vColor;
}
