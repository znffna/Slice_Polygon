#include "Default.hpp"
#include "Object.hpp"

#ifndef CAMERA_HPP
#define CAMERA_HPP

class Camera {
	float lat;
	float radius;
	//카메라 외부 파라미터
	glm::vec3 cameraPos; //--- 카메라 위치
	glm::vec3 cameraDirection; //--- 카메라 바라보는 방향
	glm::vec3 cameraUp; //--- 카메라 위쪽 방향
	//카메라 내부 파라미터	
	GLfloat fovy;	//--- 수직시야각
	GLfloat aspect;	//--- 종횡비	
	GLfloat n;	//--- near	
	GLfloat f;	//--- far
	//그외 파라미터
	//glm::vec3 p_t;	//원근 투영시 공간 이동

	//카메라 조작 속도 감도
	GLfloat sensitivity;
public:
	Camera();

	glm::vec3 camera_rotate;
	void show_state() const;
	//카메라 파라미터 리턴
	//외부 파라미터
	glm::vec3 getPos() const;
	glm::vec3 getDir() const;
	glm::vec3 getUp() const;

	//조작 감도
	GLfloat getSensitivity() const;

	//카메라 위치 조정
	void trans_Pos_x(const float&);
	void trans_Pos_y(const float&);
	void trans_Pos_z(const float&);

	void trans_Dir_x(const float&);
	void trans_Dir_y(const float&);
	void trans_Dir_z(const float&);

	void rotate_Pos_x(const float&);
	void rotate_Pos_y(const float&);
	void rotate_Pos_z(const float&);

	void rotate_Dir_y(const float&);
	void rotate_Up(const float& );

	void Pos_translation(const glm::vec3&);
	void Pos_rotate(const glm::vec3&);
	void Pos_scale(const glm::vec3&);

	//바라보는 방향 설정
	void focus(const glm::vec3&);

	void adj_sensitivity(const float&);

	void addP_t_z(const float&);

	//카메라 위치 출력용
	void World_transform(glm::mat4&) const;

	glm::mat4 View_transform() const;
	glm::mat4 perspective_transform() const;
	glm::mat4 ortho_transform() const;
};

#endif