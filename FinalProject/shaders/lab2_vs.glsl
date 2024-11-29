#version 400            

uniform mat4 PVM;          // 投影视图模型矩阵
uniform mat4 ModelView;    // 模型视图矩阵
uniform mat3 NormalMatrix; // 法线矩阵

in vec3 pos_attrib;        // 顶点位置
in vec2 tex_coord_attrib;  // 纹理坐标
in vec3 normal_attrib;     // 法向量

out vec2 tex_coord;        // 传递到片段着色器的纹理坐标
out vec3 frag_normal;      // 传递到片段着色器的法向量
out vec3 frag_pos;         // 传递到片段着色器的片段位置

void main(void)
{
    gl_Position = PVM * vec4(pos_attrib, 1.0); // 顶点位置变换
    tex_coord = tex_coord_attrib;              // 传递纹理坐标
    frag_pos = vec3(ModelView * vec4(pos_attrib, 1.0)); // 片段位置（视图空间）
    frag_normal = normalize(NormalMatrix * normal_attrib); // 转换法向量（视图空间）
}