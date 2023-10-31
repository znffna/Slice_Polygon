
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

const glm::vec3 rainbow[] = {
	{1.0f, 0.0f, 0.0f},		//빨강
	{1.0f, 0.2f, 0.0f},		//주황
	{1.0f, 1.0f, 0.0f},		//노랑
	{0.0f, 1.0f, 0.0f},		//초록
	{0.0f, 0.0f, 1.0f},		//파랑
	{0.0f, 0.02f, 1.0f},	//남색
	{0.4f, 0.0f, 1.0f},		//보라
	{0.5f, 0.1f, 0.15f}		//와인
};

//윈도우 크기 설정
const float window_row(800);			//윈도우 크기(가로)
const float window_col(600);			//윈도우 크기(세로)


void show_vec3(const glm::vec3&);
// mt19937에 의한 랜덤 실수값 생성
float random_number(const float& , const float& );
// mt19937에 의한 랜덤 정수값 생성
int random_number(const int& , const int& );	
// 각도를 0~360 사이로 유지하는 정규화 함수
void degree_range_normalization(float& );	

// 2차 베지어 곡선 위치 계산 함수
glm::vec3 CalculateBezierPoint(float t, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);

#endif //