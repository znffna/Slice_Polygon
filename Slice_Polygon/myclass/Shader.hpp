#include "Default.hpp"
#include "Camera.hpp"
#include "Object.hpp"

#ifndef SHADER_HPP
#define SHADER_HPP

class Shader {
public:
	GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����
	GLuint vertexShader, fragmentShader;	//--- ���̴� ��ü
	GLuint shaderProgramID;					//--- ���̴� ���α׷�

	Shader();
	//���̴� ��ü ������ ����Լ�
	char* filetobuf(const char* );	//--- ������ �о���� �Լ�
	//void make_shaderProgram();			//--- ���̴� ���α׷� �����ϱ�
	void make_shaderProgram(const char* vertex = "vertex.glsl", const char* fragment = "fragment.glsl");	//--- ���̴� ���α׷� �����ϱ�
	void make_vertexShaders(const char*);			//--- ���ؽ� ���̴� ��ü �����
	void make_fragmentShaders(const char*);		//--- �����׸�Ʈ ���̴� ��ü �����
	// ���̴� ��� �Լ�
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