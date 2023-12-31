#include "Object.hpp"

//생성자
Object::Object() {
	radius = 1.0f;

	after_translation = { 0.0f ,0.0f, 0.0f };
	after_rotate = { 0.0f ,0.0f, 0.0f };
	after_scale = { 1.0f ,1.0f, 1.0f };
	scale = { 1.0f ,1.0f, 1.0f };
	rotate = { 0.0f ,0.0f, 0.0f };
	translation = { 0.0f ,0.0f, 0.0f };

	own_point = { 0.0f ,0.0f, 0.0f };
}
// 복사 생성자
Object::Object(const Object& other) {
	radius = other.radius;
	after_translation = other.after_translation;
	after_rotate = other.after_rotate;
	after_scale = other.after_scale;

	after_rotate_origin = other.after_rotate_origin;

	translation = other.translation;
	rotate = other.rotate;
	scale = other.scale;
	own_point = other.own_point;

	mesh = other.mesh;

	t = other.t;
	depth = other.depth;
}
// 복사 할당 연산자
Object& Object::operator=(const Object& other) {
	if (this != &other) {
		radius = other.radius;
		after_translation = other.after_translation;
		after_rotate = other.after_rotate;
		after_scale = other.after_scale;

		after_rotate_origin = other.after_rotate_origin;

		translation = other.translation;
		rotate = other.rotate;
		scale = other.scale;
		own_point = other.own_point;

		mesh = other.mesh;

		t = other.t;
		depth = other.depth;
	}
	return *this;
}
// 이동 생성자
Object::Object(Object&& other) noexcept {
	radius = other.radius;
	after_translation = other.after_translation;
	after_rotate = other.after_rotate;
	after_scale = other.after_scale;

	after_rotate_origin = other.after_rotate_origin;

	translation = other.translation;
	rotate = other.rotate;
	scale = other.scale;
	own_point = other.own_point;

	mesh = std::move(other.mesh);

	t = other.t;
	depth = other.depth;
}
// 이동 할당 연산자
Object& Object::operator=(Object&& other) noexcept {
	if (this != &other) {
		radius = other.radius;
		after_translation = other.after_translation;
		after_rotate = other.after_rotate;
		after_scale = other.after_scale;

		after_rotate_origin = other.after_rotate_origin;

		translation = other.translation;
		rotate = other.rotate;
		scale = other.scale;
		own_point = other.own_point;

		mesh = std::move(other.mesh);

		t = other.t;
		depth = other.depth;
	}
	return *this;
}
// 소멸자
Object::~Object() {

}
//Object 설정 초기화


void Object::reset() {
	radius = 1.0f;
	after_translation = { 0.0f ,0.0f, 0.0f };
	after_rotate = { 0.0f ,0.0f, 0.0f };
	after_scale = { 1.0f ,1.0f, 1.0f };
	scale = { 0.3f ,0.3f, 0.3f };
	rotate = { 0.0f ,0.0f, 0.0f };
	translation = { 0.0f ,0.0f, 0.0f };
	after_rotate_origin= { 0.0f ,0.0f, 0.0f };
	mesh.setMesh(MESH_TRIANGLE);
}

void Object::reset(const int& polygon) {
	radius = 1.0f;
	after_translation = { 0.0f ,0.0f, 0.0f };
	after_rotate = { 0.0f ,0.0f, 0.0f };
	after_scale = { 1.0f ,1.0f, 1.0f };
	scale = { 0.3f ,0.3f, 0.3f };
	rotate = { 0.0f ,0.0f, 0.0f };
	translation = { 0.0f ,0.0f, 0.0f };
	after_rotate_origin = { 0.0f ,0.0f, 0.0f };
	mesh.setMesh(polygon);
}

void Object::transform_reset() {
	radius = 1.0f;

	after_translation = { 0.0f ,0.0f, 0.0f };
	after_rotate = { 0.0f ,0.0f, 0.0f };
	after_scale = { 1.0f ,1.0f, 1.0f };
	scale = { 0.3f ,0.3f, 0.3f };
	rotate = { 0.0f ,0.0f, 0.0f };
	translation = { 0.0f ,0.0f, 0.0f };
}

void Object::changemesh(const int& number) {
	//mesh 변경
	mesh.setMesh(number);
	//object 변수 초기화
	transform_reset();	
}

void Object::show_state() const {
	std::cout << "--------------Object-------------------" << '\n';
	std::cout << "Translation : {" << translation.x << ", " << translation.y << ", " << translation.z << "} " << '\n';
	std::cout << "Rotate : {" << rotate.x << ", " << rotate.y << ", " << rotate.z << "} " << '\n';
	std::cout << "Scale : {" << scale.x << ", " << scale.y << ", " << scale.z << "} " << '\n';
	std::cout << "---------------------------------------" << '\n';
}


//World Transform 설정 함수
void Object::setTranslation(const glm::vec3& vector = {0.0f, 0.0f, 0.0f}) {
	translation = vector;
}

void Object::setRotate(const glm::vec3& vector = { 0.0f, 0.0f, 0.0f }) {
	rotate = vector;
}

void Object::setScale(const glm::vec3& vector = { 1.0f, 1.0f, 1.0f }) {
	scale = vector;
}

//Translation
void Object::addTranslation_x(const float& value) {
	translation.x += value;
}
void Object::addTranslation_y(const float& value) {
	translation.y += value;
}
void Object::addTranslation_z(const float& value) {
	translation.z += value;
}
void Object::addAfter_Translation_x(const float& value) {
	after_translation.x += value;
}
void Object::addAfter_Translation_y(const float& value) {
	after_translation.y += value;
}
void Object::addAfter_Translation_z(const float& value) {
	after_translation.z += value;
}
//Rotate
void Object::addRotate_x(const float& degree) {
	rotate.x += degree;
}
void Object::addRotate_y(const float& degree) {
	rotate.y += degree;
}
void Object::addRotate_z(const float& degree) {
	rotate.z += degree;
}
void Object::addAfter_Rotate_x(const float& degree) {
	after_rotate.x += degree;
}
void Object::addAfter_Rotate_y(const float& degree) {
	after_rotate.y += degree;
}
void Object::addAfter_Rotate_z(const float& degree) {
	after_rotate.z += degree;
}
void Object::addAfter_Rotate_origin_x(const float& degree) {
	after_rotate_origin.x += degree;
}
void Object::addAfter_Rotate_origin_y(const float& degree) {
	after_rotate_origin.y += degree;
}
void Object::addAfter_Rotate_origin_z(const float& degree) {
	after_rotate_origin.z += degree;
}
//Scale
void Object::addScale_x(const float& value) {
	scale.x += value;
}
void Object::addScale_y(const float& value) {
	scale.y += value;
}
void Object::addScale_z(const float& value) {
	scale.z += value;
}

//--- 위치를 주어진 각도만큼 회전해서 저장함.
void Object::translation_rotate(const glm::vec3& degrees) {
	glm::mat4 transform{ 1.0f };
	transform = glm::rotate(transform, glm::radians(degrees.x), x_axis);
	transform = glm::rotate(transform, glm::radians(degrees.y), y_axis);
	transform = glm::rotate(transform, glm::radians(degrees.z), z_axis);
	translation = transform * glm::vec4{ translation, 1.0f };
}

void Object::setPoint(const glm::vec3& point) {
	own_point = point;
}

bool Object::Collision(const Object& other) const {

	float other_l = other.translation.x - other.scale.x;
	float other_r = other.translation.x + other.scale.x;
	float other_b = other.translation.y - other.scale.y;
	float other_t = other.translation.y + other.scale.y;

	float this_l = translation.x - scale.x;
	float this_r = translation.x + scale.x;
	float this_b = translation.y - scale.y;
	float this_t = translation.y + scale.y;

	// 왼쪽 끝보다 우측끝이 더 왼쪽.
	if (other_l > this_r) {	return false; }
	// 우측 끝보다 좌측끝이 더 우측.
	if (other_r < this_l) {	return false; }
	// 마우스의 상단보다 폴리곤 하단이 더 위
	if (other_b > this_t) {	return false; }
	// 마우스의 하단보다 폴리곤 상단이 더 아래
	if (other_t < this_b) {	return false; }
	return true;
}


void Object::World_Transform(glm::mat4& transformMatrix) const {
	World_after_Translation(transformMatrix);
	World_after_rotate_origin(transformMatrix);
	World_after_rotate(transformMatrix);

	{
		transformMatrix = glm::translate(transformMatrix, translation); //--- 이동(Translation)	
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.x), x_axis);	//--- x축 기준 회전(Rotate)
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.y), y_axis);	//--- y축 기준 회전(Rotate)
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rotate.z), z_axis);	//--- y축 기준 회전(Rotate)
		transformMatrix = glm::scale(transformMatrix, scale);		//--- 확대축소 변환(Scaling)			
	}
}



void Object::World_after_Scale(glm::mat4& transformMatrix) const {
	transformMatrix = glm::translate(transformMatrix, own_point); //--- 이동(Translation)	
	transformMatrix = glm::scale(transformMatrix, scale);		//--- 확대축소 변환(Scaling)					
	transformMatrix = glm::translate(transformMatrix, sign * own_point); //--- 이동(Translation)	
}

void Object::World_after_Translation(glm::mat4& transformMatrix) const {
	transformMatrix = glm::translate(transformMatrix, after_translation); //--- 이동(Translation)	
}

void Object::World_after_rotate(glm::mat4& transformMatrix) const {
	transformMatrix = glm::translate(transformMatrix, own_point); //--- 이동(Translation)	
	transformMatrix = glm::rotate(transformMatrix, glm::radians(after_rotate.x), x_axis);	//--- x축 기준 회전(Rotate)
	transformMatrix = glm::rotate(transformMatrix, glm::radians(after_rotate.y), y_axis);	//--- y축 기준 회전(Rotate)
	transformMatrix = glm::rotate(transformMatrix, glm::radians(after_rotate.z), z_axis);	//--- y축 기준 회전(Rotate)
	transformMatrix = glm::translate(transformMatrix, sign * own_point); //--- 이동(Translation)	
}


void Object::World_after_rotate_origin(glm::mat4& transformMatrix) const {
	transformMatrix = glm::rotate(transformMatrix, glm::radians(after_rotate_origin.x), x_axis);	//--- x축 기준 회전(Rotate)
	transformMatrix = glm::rotate(transformMatrix, glm::radians(after_rotate_origin.y), y_axis);	//--- y축 기준 회전(Rotate)
	transformMatrix = glm::rotate(transformMatrix, glm::radians(after_rotate_origin.z), z_axis);	//--- y축 기준 회전(Rotate)
}

const GLuint& Object::getVao() const {
	return mesh.vao;
}



Axis::Axis() {
	radius = 1.0f;
	after_translation = { 0.0f ,0.0f, 0.0f };
	after_rotate = { 0.0f ,0.0f, 0.0f };
	after_scale = { 1.0f ,1.0f, 1.0f };
	scale = { 10.0f, 10.0f, 10.0f };
	rotate = { 0.0f ,0.0f, 0.0f };
	translation = { 0.0f ,0.0f, 0.0f };
}

void Axis::draw() const {
	glDrawArrays(GL_LINES, 0, mesh.indexnum);
}