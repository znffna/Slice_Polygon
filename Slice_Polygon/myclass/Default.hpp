
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

//축의 방향 벡터
const glm::vec3 x_axis{ 1.0f, 0.0f, 0.0f };
const glm::vec3 y_axis{ 0.0f, 1.0f, 0.0f };
const glm::vec3 z_axis{ 0.0f, 0.0f, 1.0f };
const glm::vec3 sign{ -1.0f , -1.0f, -1.0f };

//윈도우 크기 설정
const float window_row(800);			//윈도우 크기(가로)
const float window_col(600);			//윈도우 크기(세로)

// mt19937에 의한 랜덤 실수값 생성
float random_number(const float& , const float& );
// mt19937에 의한 랜덤 정수값 생성
int random_number(const int& , const int& );	
// 각도를 0~360 사이로 유지하는 정규화 함수
void degree_range_normalization(float& );	

#endif //