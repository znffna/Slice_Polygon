#include "Default.hpp"

#ifndef MESH_HPP
#define MESH_HPP

//SetMesh에 사용되는 번호
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

//---- Mesh 사용법 ----
// 1. 선언 Mesh
// 2. Mesh를 불러옴 =>  setMesh( ^ 번호 ^); 
// ex) setMesh(MESH_AXIS) - 축을 담당하는 mesh 불러옴.
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

	bool existVao{ false };
	bool existVbo{ false };
	bool existEbo{ false };


public:
	//---멤버 변수
	GLuint vao;
	GLuint vbo[2];
	GLuint ebo;

	std::vector<glm::vec3> vertex;	//회전 적용 안하는 기본 정점
	std::vector<glm::vec3> color;	//회전 적용 안하는 기본 정점
	std::vector<unsigned int> index;	//회전 적용 안하는 기본 정점

	GLuint vertexnum;	//정점 개수
	GLuint indexnum;	//인덱스 크기
	GLuint polygonnum;	//폴리곤 개수(삼각형 개수)

	glm::vec3 origin_translation;	//도형의 중심으로 이동시키는 벡터
	glm::vec3 origin_scale;			//도형의 크기를 정규화 하는 과정

	glm::vec3* polygon_center;		//실습 18번용 폴리곤의 중심을 저장

	//---멤버 함수
	//생성자
	Mesh();
	//복사 생성자
	Mesh(const Mesh& other);
	//복사 할당 연산자
	Mesh& operator=(const Mesh& other);
	//이동 생성자
	Mesh(Mesh&& other) noexcept;
	//이동 할당 생성자
	Mesh& operator=(Mesh&& other) noexcept;
	//소멸자
	~Mesh();

	void genVao();
	void genVbo();
	void genEbo();

	void delVao();
	void delVbo();
	void delEbo();

	void genGPUbuffers();
	void delGPUbuffers();

	std::string get_name() {
		return name;
	}

	void set_name(const char* string) {
		name = string;
	}

	//method
	void Fill_Draw(const int&) const;
	void LINE_Draw(const int&) const;
	void AUTO_Draw(const bool& TRIANGLE = true) const;

	void clear();
	void setMesh(const int&, const float& radius = 10.0f);
	void push_GPU();
	bool exist() const;

	std::string getName() const;
	void Object_Space_Transform(glm::mat4& ) const;

	//마우스가 클릭했을 때부터 땔때까지의 직선
	void line_initBuffers(const glm::vec3& start, const glm::vec3& end);

};


#endif //
