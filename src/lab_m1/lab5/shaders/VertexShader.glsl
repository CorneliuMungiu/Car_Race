#version 330

// Input
// TODO(student): Get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_coordinate;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Time;
uniform vec4 Car;
uniform float scale_factor;


// Output
// TODO(student): Output values to fragment shader
out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_coordinate;
out vec3 frag_color;

void main()
{
    // TODO(student): Send output to fragment shader
    frag_position = v_position;
    frag_normal = v_normal;
    frag_coordinate = v_coordinate;
    frag_color = v_color;
    frag_color += Time;

    // TODO(student): Compute gl_Position
    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
    vec4 aux = Car - gl_Position;
    gl_Position[1] = gl_Position[1] - (aux[0] * aux[0] + aux[1] * aux[1] + aux[2] * aux[2] + aux[3] * aux[3]) * (-0.01f);

}
