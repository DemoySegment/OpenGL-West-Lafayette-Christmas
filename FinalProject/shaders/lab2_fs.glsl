#version 400

uniform sampler2D diffuse_tex;  // ���������
uniform vec3 light_dir;         // ���շ�����ͼ�ռ䣩
uniform vec3 light_color;       // �����ɫ
uniform vec3 ambient_color;     // ��������ɫ
uniform float specular_strength; // ���淴��ǿ��
uniform vec3 specular_pos;      // ���淴�����ɵ�λ��

in vec2 tex_coord;              // �Ӷ�����ɫ����������������
in vec3 frag_normal;            // �Ӷ�����ɫ�������ķ�����
in vec3 frag_pos;               // �Ӷ�����ɫ��������Ƭ��λ��

out vec4 fragcolor;             // �����ɫ

void main(void)
{
    vec3 normal = normalize(frag_normal); // ��һ��������

    // ����������
    float diff = max(dot(normal, -light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    // ���淴������
    vec3 view_dir = normalize(-frag_pos); // �۲췽�򣨼����ӵ���ԭ�㣩
    vec3 reflect_dir = reflect(light_dir, normal); // ���䷽��
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 64.0); // �߹����������ָ��Ϊ 64 ��߹���ȣ�
    vec3 specular = specular_strength * spec * light_color;

    // �������淴��Ӱ�췶Χ������Ӱ�췶Χ��
    float distance = length(frag_pos - specular_pos);
    float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.01 * (distance * distance)); // ����˥����ʽ
    specular *= attenuation; // ���ݾ���������淴��

    // ������
    vec3 ambient = ambient_color;

    // ���չ���
    vec3 lighting = ambient + diffuse + specular;

    // ����������ɫ�����ӹ���
    vec4 tex_color = texture(diffuse_tex, tex_coord);
    vec3 final_color = tex_color.rgb * lighting;

    fragcolor = vec4(final_color, tex_color.a);
}