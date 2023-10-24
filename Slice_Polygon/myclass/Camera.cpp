#include "Camera.hpp"

Camera::Camera() {
	lat = 0.0f;

	camera_rotate = { 0.0f, 0.0f, 0.0f };

	//�� ��ȯ�� ī�޶� �Ķ����
	cameraPos = glm::vec3(0.0f, 10.0f, 20.0f); //--- ī�޶� ��ġ
	cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� �ٶ󺸴� ����
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����

	
	//���� ��ȯ�� ī�޶� �Ķ����
	fovy = 45.0f;
	aspect = static_cast<float> (window_row) / static_cast<float>(window_col);
	n = 0.05f;
	f = 200.0f;
	//p_t = glm::vec3(0.0f, 0.0f, 0.0f);
	sensitivity = 10.0f;
}

void Camera::show_state() const {
	std::cout << "--------------ī�޶� �ܺ� �Ķ����--------------------------" << '\n';
	std::cout << "cameraPos : {" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << "} " << '\n';
	std::cout << "cameraDir : {" << cameraDirection.x << ", " << cameraDirection.y << ", " << cameraDirection.z << "} " << '\n';
	std::cout << "cameraUp : {" << cameraUp.x << ", " << cameraUp.y << ", " << cameraUp.z << "} " << '\n';
	//std::cout << "P_t : {" << p_t.x << ", " << p_t.y << ", " << p_t.z << "} " << '\n';
	std::cout << "-----------------------------------------------------" << '\n';
}

void Camera::World_transform(glm::mat4& transformMatrix) const {
	transformMatrix = glm::translate(transformMatrix, cameraPos); //--- �̵�(Translation)	
}



glm::vec3 Camera::getPos() const {
	//std::cout << "CameraPos = {" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << "} " << '\n';
	return cameraPos;
}
glm::vec3 Camera::getDir() const {
	return cameraDirection;
}
glm::vec3 Camera::getUp() const {
	return cameraUp;
}

GLfloat Camera::getSensitivity() const {
	return sensitivity;
}


void Camera::trans_Pos_x(const float& value) {
	cameraPos.x += value;
}
void Camera::trans_Pos_y(const float& value) {
	cameraPos.y += value;
}
void Camera::trans_Pos_z(const float& value) {
	cameraPos.z += value;
}

void Camera::trans_Dir_x(const float& value) {
	cameraDirection.x += value;
}
void Camera::trans_Dir_y(const float& value) {
	cameraDirection.y += value;
}
void Camera::trans_Dir_z(const float& value) {
	cameraDirection.z += value;
}


void Camera::rotate_Pos_x(const float& degree) {
	glm::mat4 transform{ 1.0f };
	transform = glm::rotate(transform, glm::radians(degree), x_axis);
	cameraPos = transform * glm::vec4{ cameraPos, 1.0f };
}

void Camera::rotate_Pos_y(const float& degree) {
	glm::mat4 transform{ 1.0f };
	transform = glm::translate(transform, cameraDirection);
	transform = glm::rotate(transform, glm::radians(degree), y_axis);
	transform = glm::translate(transform, glm::vec3{ -1.0f } * cameraDirection);
	cameraPos = transform * glm::vec4{ cameraPos, 1.0f };
}

void Camera::rotate_Pos_z(const float& degree) {
	glm::mat4 transform{ 1.0f };
	transform = glm::rotate(transform, glm::radians(degree), z_axis);
	cameraPos = transform * glm::vec4{ cameraPos, 1.0f };
}

void Camera::rotate_Dir_y(const float& degree) {
	//camera_rotate.y += degree;
	//degree_range_normalization(camera_rotate.y);
	glm::mat4 transform{ 1.0f };
	glm::vec3 initialDirection = cameraDirection - cameraPos;
	//glm::vec3 initialDirection = glm::normalize(cameraDirection - cameraPos);

	// ���ϴ� ȸ�� ���� (���� ����)
	float rotationAngleRadians = glm::radians(degree);

	// Y�� ���� ȸ���� ��Ÿ���� ���ʹϾ� ����
	glm::quat rotationQuat = glm::angleAxis(rotationAngleRadians, glm::vec3(0.0f, 1.0f, 0.0f));

	// �ʱ� ���� ���͸� ���ʹϾ����� ȸ��
	glm::vec3 rotatedDirection = rotationQuat * initialDirection;

	// ���ο� �ٶ󺸴� ���� ���͸� ��� ���� EYE�� ����
	glm::vec3 newAT = cameraPos + rotatedDirection;
	cameraDirection = newAT;
}

void Camera::rotate_Up(const float& degree) {
	//camera_rotate.y += degree;
	//degree_range_normalization(camera_rotate.y);
	glm::mat4 transform{ 1.0f };
	glm::vec3 initialDirection = glm::normalize(cameraDirection - cameraPos);
	//glm::vec3 initialDirection = glm::normalize(cameraDirection - cameraPos);

	// ���ϴ� ȸ�� ���� (���� ����)
	float rotationAngleRadians = glm::radians(degree);

	// Y�� ���� ȸ���� ��Ÿ���� ���ʹϾ� ����
	glm::quat rotationQuat = glm::angleAxis(rotationAngleRadians, initialDirection);

	// �ʱ� ���� ���͸� ���ʹϾ����� ȸ��
	glm::vec3 rotatedDirection = rotationQuat * cameraUp;

	// ���ο� �ٶ󺸴� ���� ���͸� ��� ���� EYE�� ����
	cameraUp = rotatedDirection;
}

void Camera::Pos_translation(const glm::vec3& vector) {
	trans_Pos_x(vector.x);
	trans_Pos_y(vector.y);
	trans_Pos_z(vector.z);
}

void Camera::Pos_rotate(const glm::vec3& degrees) {
	//�¿� �̵�
	rotate_Pos_y(degrees.y);

	//���� �̵�
	lat += degrees.x;
	if (-90.0f < lat and lat < 90.0f) {	//y��� �������� �ʰ� ����
		glm::vec3 axis = glm::cross(cameraUp, cameraPos);
		//std::cout << "axis = {" << axis.x << ", " << axis.y << ", " << axis.z << "} " << '\n';
		glm::mat4 t{ 1.0f };
		t = glm::rotate(t, glm::radians(degrees.x), axis);
		cameraPos = t * glm::vec4{ cameraPos, 1.0f };
	}
	else {
		lat -= degrees.x;
	}
}

void Camera::Pos_scale(const glm::vec3& degrees) {
	glm::mat4 transform = glm::translate(glm::mat4{ 1.0f }, degrees);
	cameraPos = glm::mat3{ transform } * cameraPos;
	//cameraPos.z = cameraPos.x > 0? 1 - cameraPos.x : cameraPos.x ;
}

void Camera::focus(const glm::vec3& focus) {
	cameraDirection = focus;
}

void Camera::adj_sensitivity(const float& value) {
	sensitivity = value;
}

void Camera::addP_t_z(const float& value) {

	//p_t.z += value;
	//if (p_t.z < 0.0f) {
	//	p_t.z = 0.0f;
	//}
}


glm::mat4 Camera::View_transform() const {
	return  glm::lookAt(cameraPos, cameraDirection, cameraUp);
}

glm::mat4 Camera::perspective_transform() const {
	glm::mat4 perspective{ 1.0f };
	//perspective = glm::scale(perspective, glm::vec3{ 1.0f, 1.0f, -1.0f });
	perspective = glm::perspective(glm::radians(fovy), aspect, n, f);
	//perspective = glm::translate(perspective, p_t);
	return perspective;
}

glm::mat4 Camera::ortho_transform() const {
	float max = 100.0f;
	glm::mat4 ortho{ 1.0f };
	ortho = glm::ortho(-max, max, -max, max, -max, max); //--- ���� ���� ����: [-100.0, 100.0]
	return ortho;
	//unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform"); //--- ���� ��ȯ �� ����
}

