#version 400

uniform sampler2D diffuse_tex;  // ���������
uniform vec3 light_dir;         // ���շ�����ͼ�ռ䣩
uniform vec3 light_color;       // �����ɫ
uniform vec3 ambient_color;     // ��������ɫ

in vec2 tex_coord;              // �Ӷ�����ɫ����������������
in vec3 frag_normal;            // �Ӷ�����ɫ�������ķ�����
in vec3 frag_pos;               // �Ӷ�����ɫ��������Ƭ��λ��

out vec4 fragcolor;             // �����ɫ

void main(void)
{
    vec3 normal = normalize(frag_normal); // ��һ��������
    vec3 diffuse = max(dot(normal, -light_dir), 0.0) * light_color; // �������
    vec3 ambient = ambient_color; // ������

    vec3 lighting = ambient + diffuse; // ���ռ���
    vec4 tex_color = texture(diffuse_tex, tex_coord); // �������ȡ��ɫ
    fragcolor = vec4(tex_color.rgb * lighting, tex_color.a); // �ϳ�������ɫ
}