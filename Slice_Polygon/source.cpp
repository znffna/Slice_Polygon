#define _CRT_SECURE_NO_WARRINGS

#include "myclass/Default.hpp"
#include "myclass/Mesh.hpp"
#include "myclass/Object.hpp"
#include "myclass/Camera.hpp"
#include "myclass/Shader.hpp"

//glew32.lib freeglut.lib
const char title[] = "Let's Slice Polygon";
const std::string User_guide[] = {
"----키보드 명령----",
"도형의 모드 LINE/FILL : Press 'd'",
"경로 출력하기 on/off : Press 'r'",
"날라오는 속도 변경하기: +/- (빨라지기/느려지기)",
"프로그램 종료: q",
//"Paste_here",
"-------------------"
};

bool debug{ false };	//true 시에만 std::cout 으로 정보 출력

//--------------------------------------------------------
//--- 클래스 정의
//--------------------------------------------------------

class Polygons : public Object {
public:
	// 이동 관련 변수
	float speed{ 0.016f };
	float time;		
	glm::vec3 color;
	bool move_way{ false };	 // true : 중력작용, false : 베지어 곡선
	bool is_sliced{ false };

	//2차 베지어 곡선 (이동 루트)
	glm::vec3 start_point;
	glm::vec3 control_point;
	glm::vec3 end_point;

	// 이동 루트 Mesh 저장
	Mesh move_route;

	// Special Function
	Polygons();
	~Polygons();

	Polygons(const Polygons& other);
	Polygons& operator=(const Polygons& other);

	Polygons(Polygons&& other) noexcept;
	Polygons& operator=(Polygons&& other) noexcept;

	// Method
	bool move();
	void reset(const float& speed);

	void set_route();
};

// 생성자
Polygons::Polygons() : Object() {
	scale = glm::vec3 { 0.1f };
	time = 0.0f;
	color = { random_number(0.0f, 1.0f), random_number(0.0f, 1.0f), random_number(0.0f, 1.0f) };
}

// 소멸자
Polygons::~Polygons() {
	Object::~Object();

	time = 0;
}

// 복사 생성자
Polygons::Polygons(const Polygons& other) : Object(other) {
	//std::cout << "Polygons 복사 생성자 불림." << '\n';

	//for (const glm::vec3& v : other.vertex) {
	//	vertex.push_back(v);
	//}
	speed = other.speed;
	time = other.time;
	color = other.color;
	is_sliced = other.is_sliced;

	start_point = other.start_point;
	control_point = other.control_point;
	end_point = other.end_point;

	// route 복사
	move_route = other.move_route;

}
// 복사 할당 연산자
Polygons& Polygons::operator=(const Polygons& other) {
	//std::cout << "Polygons 복사 할당 연산자 불림." << '\n';
	if (this != &other) {
		//vertex.clear();
		//move_route.vertex.clear();
		//move_route.color.clear();
		//move_route.index.clear();

		Object::operator=(other);

		speed = other.speed;
		time = other.time;
		color = other.color;
		is_sliced = other.is_sliced;
		//for (const glm::vec3& v : other.vertex) {
		//	vertex.push_back(v);
		//}

		start_point = other.start_point;
		control_point = other.control_point;
		end_point = other.end_point;

		// route 복사
		move_route = other.move_route;

	}
	return *this;
}
// 이동 생성자
Polygons::Polygons(Polygons&& other) noexcept : Object(other) {
	//Object::Object(other);

	//std::cout << "Polygons 이동 생성자 불림." << '\n';
	speed = other.speed;
	time = other.time;
	color = other.color;
	is_sliced = other.is_sliced;

	start_point = other.start_point;
	control_point = other.control_point;
	end_point = other.end_point;

	// route 복사
	move_route = std::move(other.move_route);
	//move_route.vertex.clear();
	//move_route.color.clear();
	//move_route.index.clear();
}
// 이동 할당 연산자
Polygons& Polygons::operator=(Polygons&& other) noexcept {
	//std::cout << "Polygons 이동 할당 연산자 불림." << '\n';
	if (this != &other) {
		Object::operator=(other);

		speed = other.speed;
		time = other.time;
		color = other.color;
		is_sliced = other.is_sliced;
		/*	for (const glm::vec3& v : other.vertex) {
			vertex.push_back(v);
		}*/

		start_point = other.start_point;
		control_point = other.control_point;
		end_point = other.end_point;

		// route 복사
		move_route = std::move(other.move_route);

	}
	return *this;
}

float instance_speed{ 0.01f };
void Polygons::reset(const float& polygon) {
	Object::reset(polygon);
	//Polygons::Polygons();
	time = 0.0f;
	speed = instance_speed;

	scale = glm::vec3{ 0.2f };

	time = 0.0f;
	start_point = { (random_number(0, 1) == 0 ? -1 : 1) * (1 + scale.x) , random_number(-0.7f, 0.5f), 0.0f };

	end_point = { start_point.x + (start_point.x < 0 ?  random_number(1.5f, 2.0f) : random_number(-2.0f, -1.5f)), -1.0f - scale.y, 0.0f };
	float cp = random_number(0.2f, 0.8f);
	control_point = { (1.0f - cp) * start_point.x + cp * end_point.x, 1.7f - start_point.y, 0.0f};

	translation = start_point;

	// 이동 경로 생성
	move_route.delGPUbuffers();
	move_route.vertex.clear();
	move_route.color.clear();
	move_route.index.clear();


	float num = (1.0f / speed);
	for (int i = 0; i < (1.0f / speed); i++) {
		move_route.vertex.push_back(CalculateBezierPoint((i / num), start_point, control_point, end_point));
		move_route.color.push_back(glm::vec3{ 0.0f });
		move_route.index.push_back(i);
	}

	move_route.push_GPU();

	if (debug) {
		std::cout << "start_point : "; show_vec3(start_point);
		std::cout << "control_point : "; show_vec3(control_point);
		std::cout << "end_point : "; show_vec3(end_point);
		std::cout << "move_route.indexnum :" << move_route.index.size() << '\n';

	}
}

//float speed{ 0.016f }; 

Object basket; // 바구니

bool Polygons::move() {
	//현재 t를 증가시킴.
	time += speed;

	//t에 맞는 베지어 곡선 위치를 대입함.
	translation = CalculateBezierPoint(time, start_point, control_point, end_point);
	if (is_sliced and Collision(basket) and translation.y > basket.translation.y) {
		float basket_left = basket.translation.x - basket.scale.x;
		float basket_right = basket.translation.x + basket.scale.x;
		if (basket_left < translation.x and translation.x < basket_right) {
			move_way = true;
			return true;
		}
	}
		

	//이동하면서 자전하는 값을 키움.
	float r = start_point.x < 0 ? -5.0f : 5.0f;
	rotate.z += r;
	degree_range_normalization(rotate.z);
	return false;
}

void Polygons::set_route() {
	// 이동 경로 생성
	move_route.delGPUbuffers();
	move_route.vertex.clear();
	move_route.color.clear();
	move_route.index.clear();

	if (speed <= 0.002f) {
		speed = 0.002f;
	}
	float num = (1.0f / speed);
	for (int i = 0; i < (1.0f / speed); i++) {
		move_route.vertex.push_back(CalculateBezierPoint((i / num), start_point, control_point, end_point));
		move_route.color.push_back(glm::vec3{ 0.0f });
		move_route.index.push_back(i);
	}

	move_route.push_GPU();
}

//--------------------------------------------------------
//--- 메인 함수
//--------------------------------------------------------
//--- 파일에서 문자열 읽어오는 함수(사용자 정의 함수로 워밍업 자료 사용가능)
GLvoid drawScene();					//--- 그리기 콜백 함수
GLvoid Reshape(int w, int h);		//--- 다시그리기 콜백 함수
GLvoid Mouse(int button, int state, int x, int y);	//--- 마우스 콜백 함수
GLvoid Keyboard(unsigned char key, int x, int y);	//--- 키보드 콜백 함수
GLvoid specialKeyboard(int key, int x, int y);		//--- 키보드 특수키 콜백 함수
GLvoid Timer(int value);	//--- 타이머 콜백 함수
GLvoid Motion(int x, int y);	//--- 마우스 모션 함수
GLvoid handleMouseWheel(int wheel, int direction, int x, int y);

//--------------------------------------------------------
//--- 메인 변수 선언
//--------------------------------------------------------

//flag 및 베이스 변수들
glm::vec3 background_color{ 0.8f };	//--- 배경 색깔
bool leftdown{ false };								//--- 마우스 클릭상황 flag

//출력 옵션
bool drawstyle{ true };	//false : 와이어(line) 객체/ true : 솔리드(triangle) 객체
bool depthcheck{ true };	//은면제거 유무
bool perspective{ true };	//투영 방식(true : 원근 투영)

bool draw_route{ false };	// 도형의 루트 출력

//마우스 좌표값
float mousex{ 0.0f };		//마우스의 x값
float mousey{ 0.0f };		//마우스의 y값
float movex{ 0.0f };		//마우스 클릭중 x값
float movey{ 0.0f };		//마우스 클릭중 y값

Mesh mouse;

//세이더 클래스 생성
Shader shader;

//카메라 클래스 생성
Camera camera;

//오브젝트 클래스 생성
std::vector<Polygons> object;
std::vector<Polygons> on_basket;

//--------------------------------------------------------
//--- 실습용 함수 선언
//--------------------------------------------------------
GLvoid setup();				//--- main에서 최초로 생성할때 부르는 초기화 함수
void change_background();	//--- 배경색을 변경
void DebugPrintVBOContents(GLuint vbo, int numVertices, int vertexSizeInBytes);	//--- GPU에 있는 버퍼에서 현재 바인드된 vao에 바인드된 vbo, ebo를 가져와 콘솔에 출력함.
void Draw_shape(const Object& obj);	//--- object가 가진 변환행렬과 mesh의 3D 객체의 내용을 출력하는 함수.
void Timer_option(const int&, const bool&);
void Change_switch(bool&);


bool move(Polygons& o);
//void slide_polygon();	//1차 시도(mat3, mat4 오류남)
void slice_polygon();	//2차 시도

//--------------------------------------------------------
//--- 실습용 전역변수 선언
//-------------------------------------------------------
//Axis obj_axis;	//--- x, y, z 축 출력용.

//------------------------------------
//메인 함수 정의
//------------------------------------

//--- main
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(window_row, window_col);
	glutCreateWindow(title);
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	//--- 세이더 생성
	shader.make_shaderProgram();

	//--- 기본 셋팅 초기화
	setup();
	mouse.line_initBuffers({mousex, mousey, 0.0f},{movex, movey, 0.0f});

	//키보드 조작 명령어 출력
	for (const std::string& s : User_guide) {
		std::cout << s << '\n';
	}

	//--- 콜업 함수 설정
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(specialKeyboard);
	glutMouseFunc(Mouse);
	glutMouseWheelFunc(handleMouseWheel);
	glutMotionFunc(Motion);

	glutTimerFunc(16, Timer, 0);

	//--- 메인 루프 진행
	glutMainLoop();
}

//--- main에서만 호출하는 최초 초기화 함수
GLvoid setup() {
	//전역변수 초기화	
	leftdown = false;		//--- 마우스 클릭상황 flag
	depthcheck = true;		//은면제거 유무
	drawstyle = true;	//false : 와이어(line) 객체/ true : 솔리드(triangle) 객체

	{	//Shader 초기화
		shader.set_color(glm::vec3{ 1.0f });
		shader.select_color(1);
	}

	
	{	//카메라 초기화
		camera.setPos({ 0.0f, 0.0f, 0.5f });
		camera.setDir({ 0.0f, 0.0f, 0.0f });
		camera.setUp({ 0.0f, 1.0f, 0.0f });
		perspective = false;	//직각투영 사용
	}

	{	//마우스 초기화
		mouse.clear();
		mouse.line_initBuffers({ mousex, mousey, 0.0f }, { movex, movey, 0.0f });

	}

	{	//바구니 초기화
		basket.reset(MESH_SQUARE);
		{
			basket.mesh.vertex.clear();
			basket.mesh.vertex.push_back({1.0f, 1.0f, 0.0f});
			basket.mesh.vertex.push_back({-1.0f, 1.0f, 0.0f});
			basket.mesh.vertex.push_back({-1.0f, -1.0f, 0.0f});
			basket.mesh.vertex.push_back({1.0f, -1.0f, 0.0f});
			basket.mesh.push_GPU();
		}
		basket.translation = glm::vec3{ 0.0f, -0.8f, 0.0f };
		basket.scale = glm::vec3{ 0.25f, 0.05f, 1.0f };
	}

}

//--- mesh 구조체 출력 함수
void Draw_shape(const Object& obj)
{	
	const Mesh& mesh{ obj.mesh };
	//--- obj 에서 읽어온 cube 출력하기
	{
		glBindVertexArray(mesh.vao);
		//--- 면 전체를 출력하는 코드
		if (drawstyle) {			
			shader.worldTransform(obj);
			for (int i = 0; i < mesh.indexnum / 3; i++) {
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(GLuint)));
			}
		}
		//--- 선 전체를 출력하는 코드
		else {
			shader.worldTransform(obj);
			for (int i = 0; i < mesh.indexnum / 3; i++) {
				glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void*)( i * 3 * sizeof(GLuint)));
			}
		}			
	}
}

//--- 그리기 콜백 함수
GLvoid drawScene()
{
	const int uniform_color{ 0 };
	const int vertex_color{ 1 };


	//--- 변경된 배경색 설정
	glClearColor(background_color.r, background_color.g, background_color.b, 1.0f);
	//glClearColor(0.7, 0.7, 0.7, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- 렌더링 파이프라인에 세이더 불러오기
	shader.use();
	//space_shader.use();
	//glUseProgram(shaderProgramID);

	//--- 깊이 비교 켜기	
	depthcheck ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);	
	
	//---- 뷰 변환
	shader.viewTransform(camera);

	//---- 투영 변환	
	if (perspective) {	//원근 투영
		shader.perspectiveTransform(camera);
	}	
	else {	//직각 투영
		shader.orthoTransform(camera);
	}

	//바구니 출력
	{
		shader.select_color(vertex_color);
		
		glBindVertexArray(basket.getVao());
		shader.worldTransform(basket);
		basket.mesh.AUTO_Draw();
	}

	// 마우스 출력
	{
		if (leftdown) {
			shader.select_color(uniform_color);
			shader.set_color(glm::vec3{ 0.0f });

			glBindVertexArray(mouse.vao);

			// 단위행렬 월드변환
			glm::mat4 matrix{ 1.0f };
			glUniformMatrix4fv(shader.getUniformLocate("modelTransform"), 1, GL_FALSE, glm::value_ptr(matrix));

			glDrawArrays(GL_LINES, 0, 2);

		}
	}

	//--- 오브젝트 출력
	{				
		shader.select_color(uniform_color);

		for (Polygons& o : object) {
			glBindVertexArray(o.getVao());
			shader.worldTransform(o);
			shader.set_color(o.color);

			if(debug){	
				std::cout << "정점 위치" << '\n';
				DebugPrintVBOContents(o.mesh.vbo[0], o.mesh.vertexnum, sizeof(glm::vec3));
			}
			//for (int i = 0; i < o.mesh.indexnum; i++) {
			//	//drawstyle? o.mesh.Fill_Draw(i): o.mesh.LINE_Draw(i);
			//	//o.mesh.AUTO_Draw(drawstyle);
			//}
			o.mesh.AUTO_Draw(drawstyle);

			if (draw_route) {	// 이동 루트 그리기
				shader.worldTransform();	//world 변환 초기화
				glBindVertexArray(o.move_route.vao);
				int route_num = o.move_route.index.size();
				int start = static_cast<int>(glm::floor(o.time * route_num));
				glDrawElements(GL_LINE_STRIP, route_num - start, GL_UNSIGNED_INT, (void*)(start * sizeof(unsigned int)));
			}
		

		}

		// 바구니 위의 오브젝트 출력
		for (Polygons& o : on_basket) {
			glBindVertexArray(o.getVao());
			shader.worldTransform(o);
			shader.set_color(o.color);

			if (debug) {
				std::cout << "정점 위치" << '\n';
				DebugPrintVBOContents(o.mesh.vbo[0], o.mesh.vertexnum, sizeof(glm::vec3));
			}
			o.mesh.AUTO_Draw(drawstyle);
		}
	}

	//--- GL 오류시 출력하도록 하는 디버깅코드
	{
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			// 오류 처리 또는 오류 메시지 출력
			std::cerr << "OpenGL 오류 발생: " << error << std::endl;
		}
	}


	//--- 화면에 출력하기
	glutSwapBuffers(); 
}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h)
{
	window_row = w;
	window_col = h;
	glViewport(0, 0, w, h);
}

//--- 키보드 콜백 함수
GLvoid Keyboard(unsigned char key, int x, int y) {
	//std::cout << key << "가 눌림" << std::endl;	
	switch (key) {
	case '+':
		instance_speed += 0.005f;
		instance_speed = glm::clamp(instance_speed, 0.002f, 0.2f);
		for (Polygons& p : object) {
			p.speed += 0.005f;
			p.speed = glm::clamp(p.speed, 0.002f, 0.05f);
		}
		std::cout << "instance_speed increase=" << instance_speed << '\n';
		break;
	case '-':
		instance_speed -= 0.005f;
		instance_speed = glm::clamp(instance_speed, 0.002f, 0.05f);
		for (Polygons& p : object) {
			p.speed -= 0.005f;
			p.speed = glm::clamp(p.speed, 0.002f, 0.2f);
		}
		std::cout << "instance_speed decrease=" << instance_speed << '\n';
		break;
	case 'r': case 'R':
		draw_route = draw_route == false ? true : false;
		break;
	case 'd': case 'D':
		drawstyle = drawstyle == false ? true : false;
		break;
	case 'q': case 'Q': glutLeaveMainLoop(); break; //--- 프로그램 종료			
	}
	
	glutPostRedisplay();
}

//--- 키보드 특수키 콜백 함수
GLvoid specialKeyboard(int key, int x, int y) {

	//std::cout << key << "가 눌림" << std::endl;
	glm::vec3 axis{ 1.0f };
	axis = camera.getDir() - camera.getPos();
	glm::normalize(axis);

	switch (key) {
	case GLUT_KEY_LEFT:
		for (Polygons& p : object) {
			p.addTranslation_x(-0.2f);
		}
		break;
	case GLUT_KEY_RIGHT:
		for (Polygons& p : object) {
			p.addTranslation_x(0.2f);
		}
		break;
	case GLUT_KEY_UP:
		for (Polygons& p : object) {
			p.addTranslation_y(0.2f);
		}
		break;
	case GLUT_KEY_DOWN:
		for (Polygons& p : object) {
			p.addTranslation_y(-0.2f);
		}
		break;
	}
	glutPostRedisplay();
}

//--- 마우스 콜백 함수
GLvoid Mouse(int button, int state, int x, int y) {
	GLfloat mx = (float)x / (window_row/2.0) - 1.0f;
	GLfloat my = -((float)y / (window_col / 2.0) - 1.0f);
	

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mousex = mx;
		mousey = my;
		movex = mx;
		movey = my;

		mouse.line_initBuffers({mousex, mousey, 0.0f},{movex, movey, 0.0f});

		leftdown = true;
	}
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		movex = mx;
		movey = my;

		//slide_polygon();
		if (!(mousex == movex and mousey == movey)) {
			slice_polygon();
		}


		leftdown = false;
	}

	glutPostRedisplay();
}

//--- 마우스 드래그 함수
GLvoid Motion(int x, int y) {
	//--- 마우스 모션 함수
	GLfloat mx = static_cast<float>(x) / (window_row / 2.0) - 1.0f;
	GLfloat my = -(static_cast<float>(y) / (window_col / 2.0) - 1.0f);
		
	
	if (leftdown) {
		float dx = mx - mousex;
		float dy = my - mousey;

		movex = mx;
		movey = my;

		mouse.line_initBuffers({ mousex, mousey, 0.0f }, { movex, movey, 0.0f });

		glutPostRedisplay();
		//버텍스 업데이트 필요
	}
}
//--- 마우스 휠 드래그 함수
GLvoid handleMouseWheel(int wheel, int direction, int x, int y) {
	// wheel: 마우스 휠의 상태를 나타내는 매개변수 (일반적으로 0)
	// direction: 마우스 휠의 방향 (1: 위로, -1: 아래로)
	// x, y: 마우스 커서의 현재 위치
	//std::cout << "wheel direction = " << direction << '\n';
	if (direction > 0) {
		//camera.addP_t_z(-0.25f);
		//camera.Pos_scale(glm::vec3{ 0.25f });
	}
	else if(direction < 0){
		//camera.addP_t_z(0.25f);
		//camera.Pos_scale(glm::vec3{ -0.25f });
	}
	//camera.show_state();
	glutPostRedisplay();
}


//--- 타이머 콜백 함수
GLvoid Timer(int value) { //--- 콜백 함수: 타이머 콜백 함수
	static int gen_time{ 0 };


	{// 바구니 이동 관련
		static int move_dir = 1;
		static float move_speed = 0.02f;
		float now_move{};

		basket.translation.x += move_dir * move_speed;
		now_move = move_dir * move_speed;

		if (glm::abs(basket.translation.x) + glm::abs(basket.scale.x) > 1.0f) {
			move_dir *= -1;
			basket.translation.x += move_dir * move_speed * 2;
			now_move += move_dir * move_speed * 2;
		}

		// 바구니 위의 조각들 움직임.
		for (Polygons& p : on_basket) {
			p.translation.x += now_move;
		}
	}

	{// 도형 삭제 관련 
		int index{ 0 };
		std::vector<int> erase_list;
		for (Polygons& o : object) {
			if (move(o)) {	//만약 도형이 화면 아래밖으로 떨어질경우 true
				erase_list.push_back(index);
				//object.erase(object.begin() + index);
			}
			index++;
		}

		// 삭제 시켜야 하는게 있을 시 도는 for loop 문
		for (auto it = erase_list.rbegin(); it != erase_list.rend(); ++it) {
			//해당 도형을 object 에서 삭제 시킨다.
			object.erase(object.begin() + *it);
			if (debug) {
				std::cout << *it << "삭제된 후 object에 현재 도형 갯수 :" << object.size() << '\n';
			}
		}
	}

	//도형 생성 관련
	if (gen_time == 59) {
		Polygons tmp;
		tmp.reset(random_number(MESH_TRIANGLE, MESH_OCTAGON));
		//object.at(object.size() - 1).reset(MESH_TRIANGLE);
		object.push_back(std::move(tmp));

		if (debug) {
			std::cout << "object에 현재 도형 갯수 :" << object.size() << '\n';
			std::cout << "object의 vao 넘버:" << object.at(object.size() - 1).mesh.vao << '\n';
		}
		
	}


	gen_time = (gen_time + 1) % 60;

	glutPostRedisplay();	
	glutTimerFunc(16, Timer, value); // 타이머함수 재 설정
}

//----------------------------------------
//실습용 함수 정의
//----------------------------------------

//--- GPU 버퍼에 있는 내용 출력
void DebugPrintVBOContents(GLuint vbo, int numVertices, int vertexSizeInBytes) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	GLfloat* data = new GLfloat[numVertices * vertexSizeInBytes / sizeof(GLfloat)];
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * vertexSizeInBytes, data);

	for (int i = 0; i < numVertices * vertexSizeInBytes / sizeof(GLfloat); ++i) {
		std::cout << data[i] << " ";
		if ((i + 1) % (vertexSizeInBytes / sizeof(GLfloat)) == 0) {
			std::cout << std::endl;
		}
	}

	delete[] data;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//--- 배경 색상을 랜덤하게 바꾸는 함수
void change_background() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> urd(0.0, 1.0);

	background_color = { urd(gen), urd(gen), urd(gen) };
}

void Timer_option(const int& type, const bool& option) {

}

void Change_switch(bool& variable) {
	variable = variable == true ? false : true;
}

bool move(Polygons& o) {
	if (o.move()) {
		on_basket.push_back(std::move(o));		

		int index = -1;
		for (Polygons& p : object) {
			++index;
			if (&p == &o) {
				break;
			}
		}

		if(index < object.size())
			object.erase(object.begin() + index);
	}

	//if (o.time >= 1.0f) {
	if (o.translation.y <= o.end_point.y) {
		return true;
	}

	return false;
}

float get_y(const GLfloat& m, const GLfloat& x, const GLfloat& c) {
	return m * x + c;
}

float get_x(const GLfloat& m, const GLfloat& y, const GLfloat& c) {
	return (1.0f / m) * (y - c);
}

bool check_inside(Polygons& polygon) {
	// 다각형
	float lp = polygon.translation.x - polygon.scale.x;
	float bp = polygon.translation.y - polygon.scale.y;
	float rp = polygon.translation.x + polygon.scale.x;
	float tp = polygon.translation.y + polygon.scale.y;

	// 마우스
	float lm = glm::min(mousex, movex);
	float bm = glm::min(mousey, movey);
	float rm = glm::max(mousex, movex);
	float tm = glm::max(mousey, movey);

	// 왼쪽 끝보다 우측끝이 더 왼쪽.
	if (lp > rm) {
		//std::cout << "폴리곤이 마우스보다 우측에 있음" << '\n';
		return false; 
	}
	// 우측 끝보다 좌측끝이 더 우측.
	if (rp < lm) {
		//std::cout << "폴리곤이 마우스보다 좌측에 있음" << '\n';
		return false;
	}
	// 마우스의 상단보다 폴리곤 하단이 더 위
	if (bp > tm) {
		//std::cout << "폴리곤이 마우스보다 위에 있음" << '\n';
		return false;
	}
	// 마우스의 하단보다 폴리곤 상단이 더 아래
	if (tp < bm) {
		//std::cout << "폴리곤이 마우스보다 아래에 있음" << '\n';
		return false;
	}

	return true;
}


void slice_polygon() {
	int cnt{};
	int size_o = object.size();

	// 마우스 관련 변수 미리 계산.
	float m_dx{ 0.0f };
	float m_m{ 0.0f };
	if ((m_dx = movex - mousex) != 0.0f) {
		m_m = (movey - mousey) / (movex - mousex);		// dy / dx
	}
	float m_c = mousey - m_m * mousex;					// c(y절편)

	//for (Polygons& p : object) {
	for (int i = 0; i < size_o; i++) {
		Polygons& p = object[cnt];
		//해당 위치의 AAB 판정에서 안쪽인지? - 미리 빼서 계산비용 절약
		if (!check_inside(p)) {	
			cnt++;//도형 1개의 잘림판단 완료.
			continue;
		}

		// 월드 변환을 가져옴.
		glm::mat4 matrix{ 1.0f };
		p.World_Transform(matrix);		

		// p의 정점위치를 전부 가져옴.
		std::vector<glm::vec3> p_vertex;
		//std::vector<glm::vec3> p_color;
		for (int i = 0; i < p.mesh.vertex.size(); i++) {
			p_vertex.push_back(p.mesh.vertex[i]);
			p_vertex[i] = matrix* glm::vec4 { p_vertex[i], 1.0f};
		}

		// 두개의 polygon 정점들을 저장한 vector 변수 생성
		std::vector<glm::vec3> first;
		std::vector<glm::vec3> second;

		// first와 second 중에 넣을 polygon 선택 변수
		bool select_first{ true };	//true 일때 first에 넣음.

		// 두번 만나게 된다면 새로운 polygon을 생성해야하니 flag를 생성
		int flag{ 0 };
		

		// 선분 하나하나를 가져와서 마우스 직선과의 비교
		if(debug)std::cout << "p_vertex 갯수:" << p_vertex.size() << '\n';
		for (int i = 0; i < p_vertex.size(); i++) {
			int next = (i + 1) % p_vertex.size();

			const glm::vec3& start = p_vertex[i];
			const glm::vec3& end = p_vertex[next];

			//먼저 시작위치를 현재 도형에 넣음.
			select_first ? first.push_back(start) : second.push_back(start);

			//두 정점간의 직선의 방정식 계산
			float v_dx = start.x - end.x;
			float v_m{ 0.0f };
			if (v_dx != 0.0f) {
				v_m = (start.y - end.y) / v_dx;
			}
			float v_c = start.y - v_m * start.x;

			// 만약 마우스 직선과 같은 기울기를 가진다면 만나지 않음.
			if ((m_m - v_m) != 0.0f) {
				// 같은 y값을 가질때의 x값을 구함.
				float meet_x = (v_c - m_c) / (m_m - v_m);
				float meet_y = (v_m * meet_x + v_c);
				// 해당 x값이 정점 두개 사이의 값인지 확인.
				bool v_inline_x = glm::min(start.x, end.x) <= meet_x and meet_x <= glm::max(start.x, end.x);
				bool m_inline_x = glm::min(mousex, movex) <= meet_x and meet_x <= glm::max(mousex, movex);
				bool v_inline_y = glm::min(start.y, end.y) <= meet_y and meet_y <= glm::max(start.y, end.y);
				bool m_inline_y = glm::min(mousey, movey) <= meet_y and meet_y <= glm::max(mousey, movey);

				if (v_inline_x and m_inline_x and v_inline_y and m_inline_y) {
					//선분과 직접 만남.
					flag += 1;
					// 만나는 지점을 두 polygon에 넣음.
					glm::vec3 meet{ meet_x, meet_y, 0.0f };
					first.push_back(meet);
					second.push_back(meet);
					if (select_first) {
						select_first = false;
					}
					else {
						select_first = true;
					}
				}
			}		
		}

		if (flag >= 2) {// 새로운 polygon과 현재 polygon의 mesh값 변경			
			p.is_sliced = true;
			if(debug) std::cout << "flag : TRUE, vao:" << p.getVao() << '\n';

			glm::vec3 now_translation = p.translation;

			// 역행렬도 미리 계산
			glm::mat4 reverse = glm::inverse(matrix);
			

			//새로 생길 도형의 중심점을 갱신
			glm::vec3 center_first{ 0.0f };

			for (const glm::vec3& v : first) {
				center_first += v;
			}
			center_first /= glm::vec3{ static_cast<float>(first.size()) };

			glm::vec3 now_to_new = center_first - p.translation;	// 현재 중앙점 -> 새 중앙점
			p.translation += now_to_new;	// 현재 위치를 새 중앙점 위치로 변경

			//first 정점들을 적용 - 현재 잘린 도형의 Mesh 변경

			p.mesh.vertex.clear();
			for (const glm::vec3 v : first) {
				glm::vec3 tmp = v;
				tmp -= now_to_new;
				tmp = reverse * glm::vec4{ tmp, 1.0f};
				p.mesh.vertex.push_back(tmp);
			}
			
			p.mesh.color.clear();
			for (int i = 1; i < first.size() + 1; i++) {
				float x = static_cast<float>(i % 2);
				float y = static_cast<float>(i / 2 % 2);
				float z = static_cast<float>(i / 4 % 2);
				p.mesh.color.push_back(glm::vec3{x, y, z});
			}

			p.mesh.index.clear();
			if (first.size() >= 2) {
				for (int i = 2; i < first.size(); i++) {
					p.mesh.index.push_back(0);
					p.mesh.index.push_back(i - 1);
					p.mesh.index.push_back(i);
				}
			}



			//적용한 vertex, color, index를 GPU에 보내기.
			p.mesh.push_GPU();
			
			p.mesh.indexnum = p.mesh.index.size();
			p.mesh.polygonnum = p.mesh.index.size() / 3;
			p.mesh.vertexnum = p.mesh.vertex.size();
			//TODO  잘린 도형에 새로운 route를 넣어주어야함.
			{
				//p.speed *= 2.0f;
				p.start_point = center_first;
				p.translation = center_first;

				float x_Value = center_first.x < now_translation.x ? -0.1f : 0.1f;				
				p.control_point = p.start_point + glm::vec3{ x_Value, 0.1f, 0.0f };

				p.end_point = p.control_point + glm::vec3{ x_Value,-0.0f, 0.0f };
				p.end_point.y = -1.0f - p.scale.y;

				p.time = 0.0f;				

				p.set_route();
			}

			glm::vec3 p_color = p.color;
			//second 정점들을 적용 - 새로운 polygons를 생성
			Polygons new_p;

			new_p.color = p_color;
			new_p.is_sliced = true;
			{
				new_p.reset(-1);
				//새로생긴 polygons의 값을 잘린 도형값으로.
				new_p.translation = p.translation;
				new_p.scale = p.scale;
				new_p.rotate = p.rotate;
				new_p.start_point = p.start_point;
				new_p.control_point = p.control_point;
				new_p.end_point = p.end_point;
				new_p.time = p.time;
			}
			//새로 생길 도형의 중심점을 갱신
			glm::vec3 center_second{ 0.0f };

			for (const glm::vec3& v : second) {
				center_second += v;
			}
			center_second /= glm::vec3{ static_cast<float>(second.size()) };

			glm::vec3 brand_new = center_second - new_p.translation;	// 현재 중앙점 -> 새 중앙점
			new_p.translation += brand_new;	// 현재 위치를 새 중앙점 위치로 변경


			new_p.mesh.vertex.clear();
			for (const glm::vec3 v : second) {
				glm::vec3 tmp = v;
				tmp -= brand_new;
				tmp = reverse * glm::vec4{ tmp, 1.0f };
				new_p.mesh.vertex.push_back(tmp);
			}

			new_p.mesh.color.clear();
			for (int i = 1; i < second.size() + 1; i++) {
				float x = static_cast<float>(i % 2);
				float y = static_cast<float>(i / 2 % 2);
				float z = static_cast<float>(i / 4 % 2);
				new_p.mesh.color.push_back(glm::vec3{ x, y, z });
			}

			new_p.mesh.index.clear();
			if (second.size() >= 2) {
				for (int i = 2; i < second.size(); i++) {
					new_p.mesh.index.push_back(0);
					new_p.mesh.index.push_back(i - 1);
					new_p.mesh.index.push_back(i);
				}
			}
			// 적용한 vertex, color, index를 GPU에 보내기.
			new_p.mesh.push_GPU();

			new_p.mesh.indexnum = new_p.mesh.index.size();
			new_p.mesh.polygonnum = new_p.mesh.index.size() / 3;
			new_p.mesh.vertexnum = new_p.mesh.vertex.size();

			//TODO 잘린 도형에 새로운 route를 넣어주어야함.
			{
				new_p.speed = p.speed;
				new_p.start_point = center_second;
				new_p.translation = center_second;

				float x_Value = center_second.x < now_translation.x ? -0.1f : 0.1f;
				new_p.control_point = new_p.start_point + glm::vec3{ x_Value, 0.1f, 0.0f };

				new_p.end_point = new_p.control_point + glm::vec3{ x_Value,-0.0f, 0.0f };
				new_p.end_point.y = -1.0f - new_p.scale.y;

				new_p.time = 0.0f;
				new_p.set_route();
			}

			object.push_back(std::move(new_p));

		}

		cnt++;//도형 1개의 잘림판단 완료.
	}//모든 도형이 끝날시 이 깊이를 빠져나옴.

}
