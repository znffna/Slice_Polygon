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
"도형의 모드: LINE/FILL",
"경로 출력하기: on/off",
"날라오는 속도 변경하기: +/- (빨라지기/느려지기)",
"프로그램 종료: q",
//"Paste_here",
"-------------------"
};

//--------------------------------------------------------
//--- 클래스 정의
//--------------------------------------------------------


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
GLvoid Timer(int value);			// 타이머 콜백 함수

//--------------------------------------------------------
//--- 메인 변수 선언
//--------------------------------------------------------

//flag 및 베이스 변수들
glm::vec3 background_color{ 0.7f, 0.7f, 0.7f };	//--- 배경 색깔
bool leftdown{ false };								//--- 마우스 클릭상황 flag

//출력 옵션
bool drawstyle{ true };	//false : 와이어(line) 객체/ true : 솔리드(triangle) 객체
bool depthcheck{ true };	//은면제거 유무
bool perspective{ true };	//투영 방식(true : 원근 투영)

//마우스 좌표값
float mousex{ 0.0f };		//마우스의 x값
float mousey{ 0.0f };		//마우스의 y값

//세이더 클래스 생성
Shader shader;

//카메라 클래스 생성
Camera camera;

//--------------------------------------------------------
//--- 실습용 함수 선언
//--------------------------------------------------------
GLvoid setup();				//--- main에서 최초로 생성할때 부르는 초기화 함수
void change_background();	//--- 배경색을 변경
void DebugPrintVBOContents(GLuint vbo, int numVertices, int vertexSizeInBytes);	//--- GPU에 있는 버퍼에서 현재 바인드된 vao에 바인드된 vbo, ebo를 가져와 콘솔에 출력함.
void Draw_shape(const Object& obj);	//--- object가 가진 변환행렬과 mesh의 3D 객체의 내용을 출력하는 함수.
void Draw_Quadric(Object& obj);
void Timer_option(const int&, const bool&);
void Change_switch(bool&);
//--------------------------------------------------------
//--- 실습용 전역변수 선언
//-------------------------------------------------------
Axis obj_axis;	//--- x, y, z 축 출력용.

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

	//키보드 조작 명령어 출력
	for (std::string s : User_guide) {
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

	glutTimerFunc(20, Timer, 0);

	//--- 메인 루프 진행
	glutMainLoop();
}

//--- main에서만 호출하는 최초 초기화 함수
GLvoid setup() {
	//전역변수 초기화	
	leftdown = false;		//--- 마우스 클릭상황 flag
	depthcheck = true;		//은면제거 유무
	drawstyle = true;	//false : 와이어(line) 객체/ true : 솔리드(triangle) 객체

	{	//중심축 초기화
		obj_axis.reset();
		obj_axis.changemesh(MESH_AXIS);
		obj_axis.setScale(glm::vec3(10.0f, 10.0f, 10.0f));
	}

	{	//바닥 초기화
		
	}
	
	{	//Object 초기화

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

	}
	

	//--- 오브젝트 출력
	{				
		
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
	glViewport(0, 0, w, h);
}

//--- 키보드 콜백 함수
GLvoid Keyboard(unsigned char key, int x, int y) {

	/*std::random_device rd;
	std::mt19937 gen(rd());	
	std::uniform_int_distribution<int> uid(0, 5);*/

	//std::cout << key << "가 눌림" << std::endl;	
	switch (key) {
	//크레인 조작
	case 'b': case 'B':
		key == 'b'? Timer_option(0, false): Timer_option(0, true);
		break;
	case 'm': case 'M':
		key == 'm' ? Timer_option(1, false) : Timer_option(1, true);
		break;
	case 'f': case 'F':
		if (timers[3]) { timers[3] = false;	}
		key == 'f' ? Timer_option(2, false) : Timer_option(2, true);
		break;
	case 'e': case 'E':
		if (timers[2]) { timers[2] = false; }
		key == 'e' ? Timer_option(3, false) : Timer_option(3, true);
		break;
	case 't': case 'T':
		key == 't' ? Timer_option(4, false) : Timer_option(4, true);
		break;
	//카메라 변환
	case 'z': case 'Z':
		key == 'z' ? Timer_option(5, false) : Timer_option(5, true);
		break;
	case 'x': case 'X':
		key == 'x' ? Timer_option(6, false) : Timer_option(6, true);
		break;
	case 'y': case 'Y':
		key == 'y' ? Timer_option(7, false) : Timer_option(7, true);
		break;
	case 'r': case 'R':
		key == 'r' ? Timer_option(8, false) : Timer_option(8, true);
		break;
	case 'a': case 'A':
		key == 'a' ? Timer_option(9, false) : Timer_option(9, true);
		break;
	//애니메이션 전체 옵션
	case 's': case 'S':
		for (bool& t : timers) { t = false; }
		timer_stop = 0;
		break;
	case 'c': case 'C':
		crane.reset();
		for (bool& t : timers) { t = false; }
		timer_stop = 0;
		break;
	// 은면 제거
	case 'h': case 'H':	
		depthcheck = depthcheck == true ? false : true;
		break;
	//투영 선택(직각/원근)
	case 'p': case 'P':	//  옆면 1개씩 번갈아 애니메이션 시작/정지
		perspective = perspective == false ? true : false;
		break;
	//default option
	case 'd': case 'D':
		drawstyle = drawstyle == false ? true : false;
		break;
	case '1':
		std::cout << "- flat_floor -" << '\n';
		flat_floor.show_state();

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
		camera.trans_Pos_x(axis.x * -0.5f);
		camera.trans_Pos_x(axis.y * -0.5f);
		camera.trans_Pos_x(axis.z * -0.5f);
		break;
	case GLUT_KEY_RIGHT:
		camera.trans_Pos_x(axis.x * 0.5f);
		camera.trans_Pos_x(axis.y * 0.5f);
		camera.trans_Pos_x(axis.z * 0.5f);
		break;
	case GLUT_KEY_UP:
		axis = glm::cross(axis, camera.getUp());
		camera.trans_Pos_x(axis.x * 0.5f);
		camera.trans_Pos_x(axis.y * 0.5f);
		camera.trans_Pos_x(axis.z * 0.5f);
		break;
	case GLUT_KEY_DOWN:
		axis = glm::cross(axis, camera.getUp());
		camera.trans_Pos_x(axis.x * -0.5f);
		camera.trans_Pos_x(axis.y * -0.5f);
		camera.trans_Pos_x(axis.z * -0.5f);
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
		leftdown = true;
	}
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
	
		leftdown = false;
	}

	glutPostRedisplay();
}

//--- 마우스 드래그 함수
GLvoid Motion(int x, int y) {
	//--- 마우스 모션 함수
	GLfloat mx = (float)x / (window_row / 2.0) - 1.0f;
	GLfloat my = -((float)y / (window_col / 2.0) - 1.0f);
		
	
	if (leftdown) {
		float dx = mx - mousex;
		float dy = my - mousey;
	
		camera.Pos_rotate(glm::vec3(camera.getSensitivity(), camera.getSensitivity(), camera.getSensitivity()) * glm::vec3(dy, -dx, 0.0f));
		//camera.show_state();

		mousex = mx;
		mousey = my;

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
		camera.addP_t_z(-0.25f);
		//camera.Pos_scale(glm::vec3{ 0.25f });
	}
	else if(direction < 0){
		camera.addP_t_z(0.25f);
		//camera.Pos_scale(glm::vec3{ -0.25f });
	}
	camera.show_state();
	//camera.show_state();
	glutPostRedisplay();
}

//타이머 변수 선언
//int timer_stop{ 0 };	//0일때 timer 꺼짐.
//bool timers[10]{ false };	//--- 해당 타이머 스위치
//bool reverse[10]{ false };//--- 해당 타이머의 역방향 여부

//--- 타이머 콜백 함수
GLvoid Timer(int value) { //--- 콜백 함수: 타이머 콜백 함수
	if (timer_stop == 0) {
		return;
	}
	//----------------크레인 조작-------------------------
	//크레인의 아래 몸체가 x축 방향으로 양/음 방향으로 이동
	if (timers[0]) {
		int sign = reverse[0] ? 1 : -1;
		crane.move(sign * 0.1f);
	}
	//크레인의 중앙 몸체가 y축에 대하여 양/음 방향으로 회전한다
	if (timers[1]) {
		int sign = reverse[1] ? 1 : -1;
		crane.upper_twist(sign * 5.0f);
	}
	//포신이 y축에 대하여 양/음 방향으로 회전하는데, 두 포신이 서로 반대방향으로 회전한다. 다시 누르면 멈춘다.
	if (timers[2]) { 		
		if (crane.get_drag_distance() < 4.0f) {
			crane.drag_cannon(-0.05f);
			if (crane.get_drag_distance() > crane.rcannon.translation.x) {
				crane.set_swing_angle(0.0f);
			}
		}
		else {
			int sign = reverse[2] ? 1 : -1;
			crane.swing_cannon(sign * 0.5f);
			if (crane.get_swing_angle() < 0.0f) {
				crane.set_swing_angle(0.0f);
				Change_switch(reverse[2]);
			}
			else if (crane.get_swing_angle() > 120.0f) {
				crane.set_swing_angle(120.0f);
				Change_switch(reverse[2]);
			}
		}		
	}
	//2개 포신이 조금씩 이동해서 한 개가 된다 / 다시 제자리로 이동해서 2개가 된다.
	if (timers[3]) {
		int sign = reverse[3] ? 1 : -1;
		//만약 회전되어있다면 정면을 향할때까진 회전만 입력.
		if (crane.get_swing_angle() > 0.0f) {
			crane.swing_cannon(0.5f);
			if (crane.get_swing_angle() < 0.0f) {
				crane.set_swing_angle(0.0f);
			}
		}
		else {	//--- 회전이 안되어있기에 이동 진행
			crane.drag_cannon(sign * 0.05f);
			std::cout << "crane.get_drag_distance() : " << crane.get_drag_distance() << '\n';
			if (crane.get_drag_distance() < 0.0f) {
				//crane.set_drag_distance(0.0f);
				Change_switch(reverse[3]);
			}
			else if (crane.get_drag_distance() > crane.rcannon.translation.x) {
				//crane.set_drag_distance(5.0f);
				Change_switch(reverse[3]);
			}
		}		
	}
	//t/T: 크레인의 맨 위 2개의 팔이 z축에 대하여 양/음 방향으로 서로 반대방향으로 회전한다. 다시 누르면 멈춘다.
	if (timers[4]) {
		int sign = reverse[4] ? 1 : -1;
		crane.swing_arm(sign * 5.0f);
		if (crane.get_swing_arm() < -90.0f) {
			crane.set_swing_arm(-90.0f);
			Change_switch(reverse[4]);
		}
		else if (crane.get_swing_arm() > 90.0f) {
			crane.set_swing_arm(90.0f);
			Change_switch(reverse[4]);
		}		
	}
	//----------------카메라 변환------------------------
	//z/Z: 카메라가 z축 양/음 방향으로 이동
	if (timers[5]) {
		int sign = reverse[5] ? 1 : -1;
		camera.trans_Pos_x(sign * 0.1f);
		camera.trans_Dir_x(sign * 0.1f);
	}
	//x/X: 카메라가 x축 양/음 방향으로 이동
	if (timers[6]) {
		int sign = reverse[6] ? 1 : -1;
		camera.trans_Pos_z(sign * 0.1f);
		camera.trans_Dir_z(sign * 0.1f);
	}
	//y/Y: 카메라 기준 y축에 대하여 회전  **************
	if (timers[7]) {
		int sign = reverse[7] ? 1 : -1;
		camera.rotate_Dir_y(sign * 5.0f);

	}
	//r/R: 화면의 중심의 y축에 대하여 카메라가 회전 (중점에 대하여 공전)
	if (timers[8]) {
		int sign = reverse[8] ? 1 : -1;
		camera.rotate_Pos_y(sign * 5.0f);
	}
	//a/A: r 명령어와 같이 화면의 중심의 축에 대하여 카메라가 회전하는 애니메이션을 진행한다/멈춘다.
	if (timers[9]) {
		int sign = reverse[9] ? 1 : -1;
		camera.rotate_Up(sign * 5.0f);
		camera.show_state();
	}
	
	glutPostRedisplay();	
	glutTimerFunc(20, Timer, value); // 타이머함수 재 설정
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
	/*if (timers[type] and reverse[type] == option) {
		timers[type] = false;
		timer_stop--;
	}
	else if (timers[type]) {
		reverse[type] = option;
	}*/
	if (timers[type]) {
		timers[type] = false;
		timer_stop--;
	}	
	else {
		timers[type] = true;
		reverse[type] = option;
		if (timer_stop == 0) {
			glutTimerFunc(20, Timer, 0); // 타이머함수 호출
		}
		timer_stop++;
	}
}

void Change_switch(bool& variable) {
	variable = variable == true ? false : true;
}