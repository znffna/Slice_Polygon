
#ifndef DEFAULT_HPP
#define DEFAULT_HPP

// Cpp header include
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
// OpenGL header include
#include <gl/glew.h> 
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
// glm header include
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

//���� ���� ����
const glm::vec3 x_axis{ 1.0f, 0.0f, 0.0f };
const glm::vec3 y_axis{ 0.0f, 1.0f, 0.0f };
const glm::vec3 z_axis{ 0.0f, 0.0f, 1.0f };
const glm::vec3 sign{ -1.0f , -1.0f, -1.0f };

//������ ũ�� ����
const float window_row(800);			//������ ũ��(����)
const float window_col(600);			//������ ũ��(����)

// mt19937�� ���� ���� �Ǽ��� ����
float random_number(const float& , const float& );
// mt19937�� ���� ���� ������ ����
int random_number(const int& , const int& );	
// ������ 0~360 ���̷� �����ϴ� ����ȭ �Լ�
void degree_range_normalization(float& );	

#endif //