#include "Default.hpp"
#include "Camera.hpp"
#include "Object.hpp"

#ifndef SHADER_HPP
#define SHADER_HPP

class Shader {
public:
	GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
	GLuint vertexShader, fragmentShader;	//--- 세이더 객체
	GLuint shaderProgramID;					//--- 셰이더 프로그램

	Shader();
	//세이더 객체 생성용 멤버함수
	char* filetobuf(const char* );	//--- 파일을 읽어오는 함수
	//void make_shaderProgram();			//--- 세이더 프로그램 생성하기
	void make_shaderProgram(const char* vertex = "vertex.glsl", const char* fragment = "fragment.glsl");	//--- 세이더 프로그램 생성하기
	void make_vertexShaders(const char*);			//--- 버텍스 세이더 객체 만들기
	void make_fragmentShaders(const char*);		//--- 프래그먼트 세이더 객체 만들기
	// 세이더 사용 함수
	void use() const;
	unsigned int getUniformLocate(const char* ) const;

	void select_color(const int& true_to_vertexcolor = 1) const;
	void set_color(const glm::vec3& vector) const;
	void setVec3(const glm::vec3& vector, const char* uniform) const;
	void setInt(const int& value, const char* uniform) const;

	void worldTransform() const;
	void worldTransform(const Object&);
	void worldTransform(const Camera& camera) const;
	void viewTransform(const Camera&);
	void perspectiveTransform(const Camera&);
	void orthoTransform(const Camera&);
};

#endif //