##shader vertexShader
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normal_matrix;
uniform vec3 camera_position;

out vec3 out_normal;
out vec3 out_camera_position;
out vec3 out_frag_position;

void main()
{
   vec3 normal = vec3(0, aPos.y, 0);
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   out_frag_position = (model * vec4(aPos, 1.0)).xyz;
   out_normal = (normal_matrix * vec4(normal, 0.0)).xyz;
   out_camera_position = camera_position;
}

#shader fragmentShader
#version 330 core

uniform vec3 diffuseVal;
uniform vec3 ambientVal;
uniform vec3 specularVal;
uniform vec3 light_color;
uniform vec3 light_position;

in vec3 out_normal;
in vec3 out_camera_position;
in vec3 out_frag_position;

out vec4 colour;

void main()
{
   vec3 normal = normalize(out_normal);
   vec3 light_direction = normalize(light_position - out_frag_position);
   vec3 view_direction = normalize(out_camera_position - out_frag_position);
   vec3 ambient = ambientVal * light_color;

   float diff = max(dot(light_direction, normal), 0.0);
   vec3 diffuse = diffuseVal * light_color * diff;

   vec3 halfVec = normalize(light_direction + view_direction);
   float spec = max(dot(halfVec, normal), 0.0);
   vec3 specular = specularVal * light_color * pow(spec, 10);

   colour = vec4(diffuse + specular + ambient, 1.0);
}
