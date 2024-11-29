#version 400

uniform sampler2D diffuse_tex;  // 纹理采样器
uniform vec3 light_dir;         // 光照方向（视图空间）
uniform vec3 light_color;       // 光的颜色
uniform vec3 ambient_color;     // 环境光颜色

in vec2 tex_coord;              // 从顶点着色器传来的纹理坐标
in vec3 frag_normal;            // 从顶点着色器传来的法向量
in vec3 frag_pos;               // 从顶点着色器传来的片段位置

out vec4 fragcolor;             // 输出颜色

void main(void)
{
    vec3 normal = normalize(frag_normal); // 归一化法向量
    vec3 diffuse = max(dot(normal, -light_dir), 0.0) * light_color; // 漫反射光
    vec3 ambient = ambient_color; // 环境光

    vec3 lighting = ambient + diffuse; // 光照计算
    vec4 tex_color = texture(diffuse_tex, tex_coord); // 从纹理读取颜色
    fragcolor = vec4(tex_color.rgb * lighting, tex_color.a); // 合成最终颜色
}