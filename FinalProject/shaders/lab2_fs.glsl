#version 400

uniform sampler2D diffuse_tex;  // 纹理采样器
uniform vec3 light_dir;         // 光照方向（视图空间）
uniform vec3 light_color;       // 光的颜色
uniform vec3 ambient_color;     // 环境光颜色
uniform float specular_strength; // 镜面反射强度
uniform vec3 specular_pos;      // 镜面反射生成的位置

in vec2 tex_coord;              // 从顶点着色器传来的纹理坐标
in vec3 frag_normal;            // 从顶点着色器传来的法向量
in vec3 frag_pos;               // 从顶点着色器传来的片段位置

out vec4 fragcolor;             // 输出颜色

void main(void)
{
    vec3 normal = normalize(frag_normal); // 归一化法向量

    // 漫反射光计算
    float diff = max(dot(normal, -light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    // 镜面反射光计算
    vec3 view_dir = normalize(-frag_pos); // 观察方向（假设视点在原点）
    vec3 reflect_dir = reflect(light_dir, normal); // 反射方向
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 64.0); // 高光分量（调整指数为 64 提高光泽度）
    vec3 specular = specular_strength * spec * light_color;

    // 调整镜面反射影响范围（扩大影响范围）
    float distance = length(frag_pos - specular_pos);
    float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.01 * (distance * distance)); // 调整衰减公式
    specular *= attenuation; // 根据距离调整镜面反射

    // 环境光
    vec3 ambient = ambient_color;

    // 最终光照
    vec3 lighting = ambient + diffuse + specular;

    // 采样纹理颜色并叠加光照
    vec4 tex_color = texture(diffuse_tex, tex_coord);
    vec3 final_color = tex_color.rgb * lighting;

    fragcolor = vec4(final_color, tex_color.a);
}