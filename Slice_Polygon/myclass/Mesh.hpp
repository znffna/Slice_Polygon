#include "Default.hpp"

#ifndef MESH_HPP
#define MESH_HPP

//SetMesh�� ���Ǵ� ��ȣ
#define MESH_AXIS 0x00
#define MESH_TETRAHEDRON 0x01
#define MESH_SPIRAL 0x02
#define MESH_CUBE 0x03
#define MESH_PYRAMID 0x04

#define MESH_CIRCLE 0x10
#define MESH_TRIANGLE 0x11
#define MESH_SQUARE 0x12
#define MESH_PENTAGON 0x13
#define MESH_HEXAGON 0x14
#define MESH_HEPTAGON 0x15
#define MESH_OCTAGON 0x16

//---- Mesh ���� ----
// 1. ���� Mesh
// 2. Mesh�� �ҷ��� =>  setMesh( ^ ��ȣ ^); 
// ex) setMesh(MESH_AXIS) - ���� ����ϴ� mesh �ҷ���.
// 3. 

class Mesh {
private:
	std::string name;

	void spiral();
	void axis();
	void tetrahedron();
	void circle(const float&);
	void polygon(const int&);
	void ReadObj(const char*);
public:
	//---��� ����
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;

	std::vector<glm::vec3> vertex;	//ȸ�� ���� ���ϴ� �⺻ ����
	GLuint vertexnum;	//���� ����
	GLuint indexnum;	//�ε��� ũ��
	GLuint polygonnum;	//������ ����(�ﰢ�� ����)

	glm::vec3 origin_translation;	//������ �߽����� �̵���Ű�� ����
	glm::vec3 origin_scale;			//������ ũ�⸦ ����ȭ �ϴ� ����

	glm::vec3* polygon_center;		//�ǽ� 18���� �������� �߽��� ����

	//---��� �Լ�
	//������
	Mesh();

	//method
	void Fill_Draw(const int&) const;
	void LINE_Draw(const int&) const;

	void clear();
	void setMesh(const int&, const float& radius = 10.0f);
	bool exist() const;

	std::string getName() const;
	void Object_Space_Transform(glm::mat4& ) const;

	void line_initBuffers(const glm::vec3& start, const glm::vec3& end);

};


#endif //
