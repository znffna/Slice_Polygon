#include "Default.hpp"
#include "Mesh.hpp"


#ifndef OBJECT_HPP
#define OBJECT_HPP

class Object {
private:
	void World_after_Scale(glm::mat4&) const;
	void World_after_Translation(glm::mat4&) const;
	void World_after_rotate(glm::mat4&) const;
	void World_after_rotate_origin(glm::mat4&) const;
public:
	//월드 변환
	GLfloat radius;	//이동 벡터에 길이를 조절	(1.0 = 기존 좌표로 이동)
	glm::vec3 after_translation;			//이동 백터
	glm::vec3 after_rotate;			//회전 백터(각 축에대한 degree값)
	glm::vec3 after_scale;			//축소/확대 백터
	
	glm::vec3 after_rotate_origin;			//원점 중심 회전 백터

	glm::vec3 translation;	//이동 백터
	glm::vec3 rotate;			//회전 백터
	glm::vec3 scale;			//축소/확대 백터

	glm::vec3 own_point;	//새로운 기준점(커스텀용)

	//출력할 3D 오브젝트
	Mesh mesh;		//해당 오브젝트가 가지는 3D 객체 넘버

	//애니메이션 카운트
	int t;	//이동한 수치
	int depth;
	//---멤버 함수
	
	//--- 특수한 상황 
	Object();	//생성자
	~Object();

	Object(const Object& other);
	Object& operator=(const Object& other);

	Object(Object&& other) noexcept;
	Object& operator=(Object&& other) noexcept;
	//---


	//Object 설정 초기화
	void changemesh(const int& number);
	void reset();
	void reset(const int& polygon);
	void transform_reset();
	//상태 출력
	void show_state() const;

	//World Transform 설정 함수
	void setTranslation(const glm::vec3& vector);
	void setRotate(const glm::vec3& vector);
	void setScale(const glm::vec3& vector);

	void addTranslation_x(const float& );
	void addTranslation_y(const float& );
	void addTranslation_z(const float& );

	void addAfter_Translation_x(const float&);
	void addAfter_Translation_y(const float&);
	void addAfter_Translation_z(const float&);

	void addRotate_x(const float& );
	void addRotate_y(const float& );
	void addRotate_z(const float& );

	void addAfter_Rotate_x(const float&);
	void addAfter_Rotate_y(const float&);
	void addAfter_Rotate_z(const float&);

	void addAfter_Rotate_origin_x(const float&);
	void addAfter_Rotate_origin_y(const float&);
	void addAfter_Rotate_origin_z(const float&);

	void addScale_x(const float& );
	void addScale_y(const float& );
	void addScale_z(const float& );

	
	void translation_rotate(const glm::vec3&);

	//각 과제마다 쓰는 임시 method
	void setPoint(const glm::vec3&);
	bool Collision(const Object& other) const ;

	//World Transform 적용 함수
	void World_Transform(glm::mat4&) const;
	const GLuint& getVao() const;


};

class Axis : public Object {
public:
	Axis();

	void draw() const;
};

#endif