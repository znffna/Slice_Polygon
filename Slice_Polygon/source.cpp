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
"도형의 모드 LINE/FILL : m",
"경로 출력하기 on/off : r",
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

	float time;		//시간 매개변수

	//2차 베지어 곡선 (이동 루트)
	glm::vec3 start_point;
	glm::vec3 control_point;
	glm::vec3 end_point;

	Polygons();
	~Polygons();

	void move();
	void reset(const float& speed);
};

Polygons::Polygons() : Object() {	
	scale = glm::vec3 { 0.1f };
	time = 0.0f;
}

Polygons::~Polygons() {

}

void Polygons::reset(const float& polygon) {
	Object::reset(polygon);
	//Polygons::Polygons();
	time = 0.0f;

	scale = glm::vec3{ 0.2f };

	time = 0.0f;
	start_point = { (random_number(0, 1) == 0 ? -1 : 1) * (1 + scale.x) , random_number(-0.7f, 0.5f), 0.0f };

	end_point = { start_point.x + (start_point.x < 0 ?  random_number(1.5f, 2.0f) : random_number(-2.0f, -1.5f)), -1.0f - scale.y, 0.0f };
	float cp = random_number(0.2f, 0.8f);
	control_point = { (1.0f - cp) * start_point.x + cp * end_point.x, 1.7f - start_point.y, 0.0f};

	translation = start_point;

	if (debug) {
		std::cout << "start_point : "; show_vec3(start_point);
		std::cout << "control_point : "; show_vec3(control_point);
		std::cout << "end_point : "; show_vec3(end_point);
	}
}

float speed{ 0.016f }; 
void Polygons::move() {
	//현재 t를 증가시킴.
	time += speed;

	//t에 맞는 베지어 곡선 위치를 대입함.
	translation = CalculateBezierPoint(time, start_point, control_point, end_point);

	//이동하면서 자전하는 값을 키움.
	float r = start_point.x < 0 ? -5.0f : 5.0f;
	rotate.z += r;
	degree_range_normalization(rotate.z);
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
float movex{ 0.0f };		//마우스 클릭중 x값
float movey{ 0.0f };		//마우스 클릭중 y값
Mesh mouse;

//세이더 클래스 생성
Shader shader;

//카메라 클래스 생성
Camera camera;

//오브젝트 클래스 생성
std::vector<Polygons> object;

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
GLvoid Gen_Timer(int value);


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

	glutTimerFunc(16, Timer, 0);
	glutTimerFunc(1000, Gen_Timer, 0);

	//--- 메인 루프 진행
	glutMainLoop();
}

//--- main에서만 호출하는 최초 초기화 함수
GLvoid setup() {
	//전역변수 초기화	
	leftdown = false;		//--- 마우스 클릭상황 flag
	depthcheck = true;		//은면제거 유무
	drawstyle = true;	//false : 와이어(line) 객체/ true : 솔리드(triangle) 객체

	//카메라 초기화
	{
		camera.setPos({ 0.0f, 0.0f, 2.0f });
		camera.setDir({ 0.0f, 0.0f, 0.0f });
		camera.setUp({ 0.0f, 1.0f, 0.0f });
	}

	{	//마우스 초기화
		mouse.clear();

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
	// 마우스 출력
	{
		if (leftdown) {
			//mouse.line_initBuffers({ mousex, mousey,0.0f }, { movex, movey, 0.0f });

		}
	}

	//--- 오브젝트 출력
	{				
		for (Polygons& o : object) {
			glBindVertexArray(o.getVao());
			shader.worldTransform(o);
			for (int i = 0; i < o.mesh.indexnum; i++) {
				drawstyle? o.mesh.Fill_Draw(i): o.mesh.LINE_Draw(i);
			}
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
	glViewport(0, 0, w, h);
}

//--- 키보드 콜백 함수
GLvoid Keyboard(unsigned char key, int x, int y) {
	//std::cout << key << "가 눌림" << std::endl;	
	switch (key) {
	

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

		movex = mx;
		movey = my;


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
	int index{0};
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

	glutPostRedisplay();	
	glutTimerFunc(16, Timer, value); // 타이머함수 재 설정
}

GLvoid Gen_Timer(int value) { //--- 콜백 함수: 타이머 콜백 함수

	Polygons tmp;	//생성 1
	tmp.reset(random_number(0x10, 0x16));
	object.push_back(tmp);	// 생성 2

	if (debug) {
		std::cout << "object에 현재 도형 갯수 :" << object.size() << '\n';
	}

	glutTimerFunc(1000, Gen_Timer, value); // 타이머함수 재 설정
}	// 삭제 1

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
	o.move();
	if (o.time >= 1.0f) {
		return true;
	}

	return false;
}

void slide_polygon(Polygons& p) {
	//현재 잘린 도형의 정점 갯수(도형 종류)
	int vertexnum = p.mesh.vertexnum;

	//현재 도형을 가르는 선
	mousex; mousey; movex; movey;

	float dx = movex - mousex;
	float dy = movey - mousey;
	float m = dy / dx;

	float c = mousey - m * mousex;

	//y = mx + c


}