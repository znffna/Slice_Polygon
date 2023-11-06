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
	std::vector<glm::vec3> vertex;

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
	time = 0;
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

float speed{ 0.002f }; 
//float speed{ 0.016f }; 


void Polygons::move() {
	//현재 t를 증가시킴.
	time += speed;

	//t에 맞는 베지어 곡선 위치를 대입함.
	translation = CalculateBezierPoint(time, start_point, control_point, end_point);

	//이동하면서 자전하는 값을 키움.
	//float r = start_point.x < 0 ? -5.0f : 5.0f;
	//rotate.z += r;
	//degree_range_normalization(rotate.z);
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
//void slide_polygon();	//1차 시도(mat3, mat4 오류남)
void slice_polygon();	//2차 시도

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
	mouse.line_initBuffers({mousex, mousey, 0.0f},{movex, movey, 0.0f});

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
		camera.setPos({ 0.0f, 0.0f, 0.5f });
		camera.setDir({ 0.0f, 0.0f, 0.0f });
		camera.setUp({ 0.0f, 1.0f, 0.0f });
		perspective = false;	//직각투영 사용
	}

	{	//마우스 초기화
		mouse.clear();
		mouse.line_initBuffers({ mousex, mousey, 0.0f }, { movex, movey, 0.0f });

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
			glBindVertexArray(mouse.vao);

			// 단위행렬 월드변환
			glm::mat4 matrix{ 1.0f };
			glUniformMatrix4fv(shader.getUniformLocate("modelTransform"), 1, GL_FALSE, glm::value_ptr(matrix));

			glDrawArrays(GL_LINES, 0, 2);

		}
	}

	//--- 오브젝트 출력
	{				
		for (Polygons& o : object) {
			glBindVertexArray(o.getVao());
			shader.worldTransform(o);
			for (int i = 0; i < o.mesh.indexnum; i++) {
				//drawstyle? o.mesh.Fill_Draw(i): o.mesh.LINE_Draw(i);
				o.mesh.AUTO_Draw(drawstyle);			
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
	window_row = w;
	window_col = h;
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

		mouse.line_initBuffers({mousex, mousey, 0.0f},{movex, movey, 0.0f});

		leftdown = true;
	}
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		movex = mx;
		movey = my;

		//slide_polygon();
		slice_polygon();


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

	//도형 생성 관련
	static int gen_time{ 0 };
	if (gen_time == 59) {
		object.push_back(Polygons());
		object.at(object.size() - 1).reset(random_number(MESH_TRIANGLE, MESH_OCTAGON));

		if (debug) {
			std::cout << "object에 현재 도형 갯수 :" << object.size() << '\n';
		}
		std::cout << "object에 현재 도형 갯수 :" << object.size() << '\n';
		std::cout << "object의 vao 넘버:" << object.at(object.size() - 1).mesh.vao << '\n';
	}

	// 도형 삭제 관련 
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
	o.move();
	if (o.time >= 1.0f) {
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
	if (lp > rm) { std::cout << "폴리곤이 마우스보다 우측에 있음" << '\n'; return false; }
	// 우측 끝보다 좌측끝이 더 우측.
	if (rp < lm) { std::cout << "폴리곤이 마우스보다 좌측에 있음" << '\n'; return false; }
	// 마우스의 상단보다 폴리곤 하단이 더 위
	if (bp > tm) { std::cout << "폴리곤이 마우스보다 위에 있음" << '\n'; return false; }
	// 마우스의 하단보다 폴리곤 상단이 더 아래
	if (tp < bm) { std::cout << "폴리곤이 마우스보다 아래에 있음" << '\n'; return false; }

	return true;
}


void slice_polygon() {
	int cnt{};
	for (Polygons& p : object) {
		std::cout << "p.mesh.vertex.size() : " << p.mesh.vertex.size() << '\n';

		//해당 위치의 AAB 판정에서 안쪽인지? - 미리 빼서 계산비용 절약
		if (!check_inside(p)) {	
			cnt++;//도형 1개의 잘림판단 완료.
			continue;
		}
		// 이 아래는 일단 충돌 가능성 있음. - 계산비용 사용
		std::cout << "ABB에서 걸러지지 않음 vao: " << p.mesh.vao << '\n';

		// 월드 변환을 가져옴.
		glm::mat4 matrix{ 1.0f };
		p.World_Transform(matrix);

		// 역행렬도 미리 계산
		glm::mat4 reverse = glm::inverse(matrix);

		// 마우스 관련 변수 미리 계산.
		float m_dx{0.0f};
		float m_m{0.0f};
		if ((m_dx = movex - mousex)!= 0.0f) {
			m_m= (movey - mousey) / (movex - mousex);		// dy / dx
		}
		float m_c = mousey - m_m * mousex;					// c(y절편)

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

		// 한번이라도 만나게 된다면 새로운 polygon을 생성해야하니 flag를 생성
		bool flag{ false };


		// 선분 하나하나를 가져와서 마우스 직선과의 비교
		std::cout << "p_vertex 갯수:" << p_vertex.size() << '\n';
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

				// 해당 x값이 정점 두개 사이의 값인지 확인.
				if (glm::min(start.x, end.x) < meet_x and meet_x < glm::max(start.x, end.x)) {
					//선분과 직접 만남.
					flag = true;
					// 만나는 지점을 두 polygon에 넣음.
					glm::vec3 meet{ meet_x, (v_m * meet_x + v_c), 0.0f };
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

		if (flag) {
			// 새로운 polygon과 현재 polygon의 mesh값 변경
			std::cout << "flag : TRUE" << p.getVao() << '\n';
			//first 정점들을 적용 - 현재 잘린 도형의 Mesh 변경
			p.mesh.vertex.clear();
			for (const glm::vec3 v : first) {
				glm::vec3 tmp = v;
				tmp = reverse * glm::vec4{ tmp, 1.0f};
				p.mesh.vertex.push_back(tmp);
			}
			
			p.mesh.color.clear();
			for (int i = 0; i < first.size(); i++) {
				float x = static_cast<float>(i / 4 % 2);
				float y = static_cast<float>(i / 2 % 2);
				float z = static_cast<float>(i % 2);
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
			// 적용한 vertex, color, index를 GPU에 보내기.
			p.mesh.push_GPU();

			//second 정점들을 적용 - 새로운 polygons를 생성
			object.push_back(Polygons());
			Polygons& new_p = object[object.size() - 1];	//막 생성한 polygon을 가르킴.
			{	//새로생긴 polygons의 값을 잘린 도형값으로.
				new_p.translation = p.translation;
				new_p.scale = p.scale;
				new_p.rotate = p.rotate;
				new_p.start_point = p.start_point;
				new_p.control_point = p.control_point;
				new_p.end_point = p.end_point;
				new_p.time = p.time;
			}
			new_p.mesh.vertex.clear();
			for (const glm::vec3 v : second) {
				glm::vec3 tmp = v;
				tmp = reverse * glm::vec4{ tmp, 1.0f };
				new_p.mesh.vertex.push_back(tmp);
			}

			new_p.mesh.color.clear();
			for (int i = 0; i < second.size(); i++) {
				float x = static_cast<float>(i / 4 % 2);
				float y = static_cast<float>(i / 2 % 2);
				float z = static_cast<float>(i % 2);
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
		}

		cnt++;//도형 1개의 잘림판단 완료.
	}//모든 도형이 끝날시 이 깊이를 빠져나옴.

}


void slide_polygon() {
	//현재 잘린 도형의 정점 갯수(도형 종류)
	int cnt{};
	for (Polygons& p : object) {
		//std::cout << "slide_polygon 에서 각 도형마다의 호출 횟수 :" << cnt << '\n';

		bool flag{ false };	//한번이라도 마우스 직선이 도형의 변만 접할경우 true
		//현재 도형의 정점 갯수
		int vertexnum = p.mesh.vertexnum;
		//--- 계산시 사용될 정점위치 및 마우스 벡터
		std::vector<glm::vec3> vertex;

		glm::mat4 w_matrix{ 1.0f };
		p.World_Transform(w_matrix);

		glm::mat3 matrix = static_cast<glm::mat3> (w_matrix);
		glm::mat3 reverse = static_cast<glm::mat3> (glm::inverse(w_matrix));

		for (int i = 0; i < p.mesh.vertex.size(); i++) {
			vertex.push_back(matrix * p.mesh.vertex.at(i));
		}

		//--- 마우스 직선관련 값 계산
		glm::vec3 mouse_start = { mousex, mousey, 0.0f };
		float mouse_dx = movex - mousex;
		float mouse_dy = movey - mousey;
		float mouse_m = mouse_dy / mouse_dx;
		float mouse_c = mousey - mouse_m * mousex ;
		// y = mouse_m * x + mouse_c;	//마우스 직선의 방정식

		//--- 새로 생성할 도형들 정점위치 저장용
		std::vector<glm::vec3> first;
		std::vector<glm::vec3> second;

		bool select_obj{ true };	//true : fisrt, false: second

		for (int i = 0; i < vertex.size(); i++) {
			//정점 2개를 이은 벡터로 직선의 방정식 구함.
			glm::vec3& start = vertex.at(i);
			glm::vec3& end = vertex.at((i + 1) % vertex.size());

			float vertex_dx = end.x - start.x;
			float vertex_dy = end.y - start.y;
			float vertex_m = { vertex_dy / vertex_dx };
			float vertex_c = start.y - vertex_m * start.x;
			

			// 두개의 정점이 만나는 위치 계산
			// 두 직선이 만나는 지점 있다 => 만나는 지점에 vertex를 생성.
			// 두 직선이 만나는 지점 없다 => 현재 선택된 vertex list에 다음 vertex를 추가
			//y가 같을 경우로 계산
			{
				select_obj ? first.push_back(start) : second.push_back(start);

				//float meet_x = get_x(mouse_m, start.y, mouse_c);	//마우스가 vetex의 y값의 위치일때 x값
				if (mouse_m == vertex_m) {	//절대 안만남.
					continue;
				}

				float meet_x = (vertex_c  - mouse_c) / (mouse_m - vertex_m);	//마우스가 vetex의 y값의 위치일때 x값
				float meet_y = get_y(vertex_m, meet_x, vertex_c);
				bool check_x = glm::min(start.x, end.x) < meet_x and meet_x < glm::max(start.x, end.x) and glm::min(mousex, movex) < meet_x and meet_x < glm::max(mousex, movex);
				bool check_y = glm::min(start.y, end.y) < meet_y and meet_y < glm::max(start.y, end.y) and glm::min(mousey, movey) < meet_y and meet_y < glm::max(mousey, movey);
				if (check_x and check_y) {
					if (!flag) { //도형이 잘렸는지 확인하는 flag 바꿈.
						flag = true;
					}	

					first.push_back({ meet_x, get_y(mouse_m, meet_x, mouse_c), 0.0f });
					second.push_back({ meet_x, get_y(mouse_m, meet_x, mouse_c), 0.0f });

					select_obj = select_obj == true ? false : true;
				}

				//{
				//	std::cout << "first 정점 --" << '\n';
				//	for (glm::vec3& v : first) {
				//		print_vec3(v);
				//	}
				//	std::cout << " first 정점 갯수: " << first.size() << '\n';
				//	std::cout << "second 정점 --" << '\n';
				//	for (glm::vec3& v : second) {
				//		print_vec3(v);
				//	}
				//	std::cout << " second 정점 갯수: " << second.size() << '\n';
				//	std::cout << "-------------" << '\n';

				//}
			}

		}		

		// flag는 잘렸을 경우 true 반환
		if (flag) {
			if (debug){
				std::cout << "first 정점 --" << '\n';
				for (glm::vec3& v : first) {
					print_vec3(v);
				}
				std::cout << "second 정점 --" << '\n';
				for (glm::vec3& v : second) {
					print_vec3(v);
				}
				std::cout << "-------------" << '\n';

			}
			//해당 버텍스 컬러 사용
			std::vector<glm::vec3> first_color;
			std::vector<glm::vec3> second_color;

			//std::cout << "최종 변환한 first 좌표: object 좌표계로 전환한 좌표" << '\n';
			for (glm::vec3& v : first) {
				v = reverse * v;
			//	print_vec3(v);
				first_color.push_back(glm::vec3{ 0.5f } * v + 0.5f);
			}
			//std::cout << "최종 변환한 second 좌표: object 좌표계로 전환한 좌표" << '\n';
			for (glm::vec3& v : second) {
				v = reverse * v;
			//	print_vec3(v);
				second_color.push_back(glm::vec3{ 0.5f } *v + 0.5f);
			}
		
			{//first를 이용한 mesh 생성
				//현재 도형에 초기화
				p.mesh.clear();

				std::vector<unsigned int> index;
				if (first.size() >= 2) {
					for (int i = 2; i < first.size(); i++) {
						index.push_back(0);
						index.push_back(i - 1);
						index.push_back(i);
					}
				}
				//first 를 현재 도형 p 에 저장
				{
					Mesh& m = p.mesh;
					m.set_name("잘린 다각형");
					m.genGPUbuffers();
					glBindVertexArray(m.vao); //--- VAO를 바인드하기

					//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
					glBindBuffer(GL_ARRAY_BUFFER, m.vbo[0]);
					//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
					//--- triShape 배열의 사이즈: 9 * float		
					glBufferData(GL_ARRAY_BUFFER, first.size() * sizeof(glm::vec3), first.data(), GL_STATIC_DRAW);
					//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
					//--- attribute 인덱스 0번을 사용가능하게 함
					glEnableVertexAttribArray(0);

					//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
					glBindBuffer(GL_ARRAY_BUFFER, m.vbo[1]);
					//--- 변수 colors에서 버텍스 색상을 복사한다.
					//--- colors 배열의 사이즈: 9 *float
					glBufferData(GL_ARRAY_BUFFER, first_color.size() * sizeof(glm::vec3), first_color.data(), GL_STATIC_DRAW);
					//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
					glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
					//--- attribute 인덱스 1번을 사용 가능하게 함.
					glEnableVertexAttribArray(1);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(unsigned int), index.data(), GL_STATIC_DRAW);


					glBindVertexArray(0); //--- VAO를 바인드하기
					m.indexnum = index.size();
					m.vertexnum = first.size();
					m.polygonnum = first.size();
				}

				p.end_point.x = { 0.5f * (p.end_point.x + p.translation.x) };
				p.start_point = p.translation;

				p.control_point = glm::vec3{ 0.5f } *(p.start_point + p.end_point);
				p.time = 0.0f;				
			}

			{//second를 이용한 Polygon 생성

				//second에 있는 버텍스는 새로운 Polygons 생성해서 집어넣기.
				//auto spot = object.begin() + cnt;
				//object.insert(spot, p);
				object.push_back(p);	
				Polygons& new_p = object.at(object.size() - 1);
				new_p.mesh.clear();

				std::vector<unsigned int> index;
				if (second.size() >= 2) {
					for (int i = 2; i < second.size(); i++) {
						index.push_back(0);
						index.push_back(i - 1);
						index.push_back(i);
					}
				}
				//first 를 현재 도형 p 에 저장
				{
					Mesh& m = new_p.mesh;
					m.genGPUbuffers();
					m.set_name("잘린 다각형");
					glBindVertexArray(m.vao); //--- VAO를 바인드하기


					//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
					glBindBuffer(GL_ARRAY_BUFFER, m.vbo[0]);
					//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
					//--- triShape 배열의 사이즈: 9 * float		
					glBufferData(GL_ARRAY_BUFFER, second.size() * sizeof(glm::vec3), second.data(), GL_STATIC_DRAW);
					//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
					//--- attribute 인덱스 0번을 사용가능하게 함
					glEnableVertexAttribArray(0);

					//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
					glBindBuffer(GL_ARRAY_BUFFER, m.vbo[1]);
					//--- 변수 colors에서 버텍스 색상을 복사한다.
					//--- colors 배열의 사이즈: 9 *float
					glBufferData(GL_ARRAY_BUFFER, second_color.size() * sizeof(glm::vec3), second_color.data(), GL_STATIC_DRAW);
					//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
					glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
					//--- attribute 인덱스 1번을 사용 가능하게 함.
					glEnableVertexAttribArray(1);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(unsigned int), index.data(), GL_STATIC_DRAW);


					glBindVertexArray(0); //--- VAO를 바인드하기
					m.indexnum = index.size();
					m.vertexnum = second.size() * 3;
					m.polygonnum = second.size();
				}

				new_p.end_point.x = { 0.5f * (p.end_point.x + p.translation.x) };
				new_p.start_point = p.translation;

				new_p.control_point = glm::vec3{ 0.5f } *(p.start_point + p.end_point);
				new_p.time = 0.0f;
			}// second로 하나의 polygon mesh 작업 끝.		


		}//하나의 Polygons  를 2개의 Polygons로 나눔 작업 끝.(flags == true)일떄 작업 끝.

		cnt++;
	}// 하나의 polygon 작업이 끝남.

}// slide_polygon이 끝남.