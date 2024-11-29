#version 400            

uniform mat4 PVM;          // ͶӰ��ͼģ�;���
uniform mat4 ModelView;    // ģ����ͼ����
uniform mat3 NormalMatrix; // ���߾���

in vec3 pos_attrib;        // ����λ��
in vec2 tex_coord_attrib;  // ��������
in vec3 normal_attrib;     // ������

out vec2 tex_coord;        // ���ݵ�Ƭ����ɫ������������
out vec3 frag_normal;      // ���ݵ�Ƭ����ɫ���ķ�����
out vec3 frag_pos;         // ���ݵ�Ƭ����ɫ����Ƭ��λ��

void main(void)
{
    gl_Position = PVM * vec4(pos_attrib, 1.0); // ����λ�ñ任
    tex_coord = tex_coord_attrib;              // ������������
    frag_pos = vec3(ModelView * vec4(pos_attrib, 1.0)); // Ƭ��λ�ã���ͼ�ռ䣩
    frag_normal = normalize(NormalMatrix * normal_attrib); // ת������������ͼ�ռ䣩
}