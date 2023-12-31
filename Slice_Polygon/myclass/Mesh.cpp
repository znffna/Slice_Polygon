#include "Mesh.hpp"
#include "Default.hpp"

//--- 생성자
Mesh::Mesh() {
	name = "None";

	vao = 0;
	for (GLuint& i : vbo) i = 0;
	ebo = 0;

	vertex.clear();
	color.clear();
	index.clear();

	vertexnum = 0;
	indexnum = 0;
	polygonnum = 0;

	origin_translation = { 0.0f, 0.0f, 0.0f };
	origin_scale = { 1.0f, 1.0f, 1.0f };
	polygon_center = nullptr;
}
//복사 생성자
Mesh::Mesh(const Mesh& other) {
	std::cout << "mesh복사 생성자 불림" << '\n';
	for (const glm::vec3& v : other.vertex) {
		vertex.push_back(v);
	}
	for (const glm::vec3& v : other.color) {
		color.push_back(v);
	}
	for (const unsigned int& i : other.index) {
		index.push_back(i);
	}
	push_GPU();

	name = other.name;
	vertexnum = other.vertex.size();
	indexnum = other.index.size();
	polygonnum = other.index.size();

	origin_translation = other.origin_translation;
	origin_scale = other.origin_scale;
}
//복사 할당 연산자
Mesh& Mesh::operator=(const Mesh& other) {
	if (this != &other) {
		//기존에 가지고 있던 GPU랑 vector 초기화
		delGPUbuffers();
		vertex.clear();
		color.clear();
		index.clear();

		// 깊은 복사 진행
		for (const glm::vec3& v : other.vertex) {
			vertex.push_back(v);
		}
		for (const glm::vec3& v : other.color) {
			color.push_back(v);
		}
		for (const unsigned int& i : other.index) {
			index.push_back(i);
		}
		push_GPU();

		name = other.name;
		vertexnum = other.vertex.size();
		indexnum = other.index.size();
		polygonnum = other.index.size();

		origin_translation = other.origin_translation;
		origin_scale = other.origin_scale;
	}
	return *this;
}
//이동 생성자
Mesh::Mesh(Mesh&& other) noexcept {
	for (const glm::vec3& v : other.vertex) {
		vertex.push_back(v);
	}
	for (const glm::vec3& v : other.color) {
		color.push_back(v);
	}
	for (const unsigned int& i : other.index) {
		index.push_back(i);
	}
	// 얇은 복사 진행
	vao = other.vao;
	vbo[0] = other.vbo[0];
	vbo[1] = other.vbo[1];
	ebo = other.ebo;

	name = other.name;
	vertexnum = other.vertex.size();
	indexnum = other.index.size();
	polygonnum = other.index.size();

	origin_translation = other.origin_translation;
	origin_scale = other.origin_scale;

	// 이제 기존에 있던것들을 제거
	other.vertex.clear();
	other.color.clear();
	other.index.clear();
	other.vertexnum = 0;
	other.indexnum = 0;
	other.polygonnum = 0;
	other.existVao = false;
	other.existVbo = false;
	other.existEbo = false;
	other.vao = -1;
	other.vbo[0] = -1;
	other.vbo[1] = -1;
	other.ebo = -1;
}
//이동 할당 생성자
Mesh& Mesh::operator=(Mesh&& other) noexcept {
	if (this != &other) {
		delGPUbuffers();
		vertex.clear();
		color.clear();
		index.clear();



		for (const glm::vec3& v : other.vertex) {
			vertex.push_back(v);
		}
		for (const glm::vec3& v : other.color) {
			color.push_back(v);
		}
		for (const unsigned int& i : other.index) {
			index.push_back(i);
		}
		// 얇은 복사 진행
		vao = other.vao;
		vbo[0] = other.vbo[0];
		vbo[1] = other.vbo[1];
		ebo = other.ebo;

		name = other.name;
		vertexnum = other.vertex.size();
		indexnum = other.index.size();
		polygonnum = other.index.size();

		origin_translation = other.origin_translation;
		origin_scale = other.origin_scale;

		// 이제 기존에 있던것들을 제거
		other.vertex.clear();
		other.color.clear();
		other.index.clear();
		other.vertexnum = 0;
		other.indexnum = 0;
		other.polygonnum = 0;
		other.existVao = false;
		other.existVbo = false;
		other.existEbo = false;
		other.vao = -1;
		other.vbo[0] = -1;
		other.vbo[1] = -1;
		other.ebo = -1;
	}
	return *this;
}
//소멸자
Mesh::~Mesh() {
	//일단 버그나니까 주석처리
	delGPUbuffers();
	vertex.clear();
	color.clear();
	index.clear();
}

//GPU option
void Mesh::genVao() {
	if (!existVao) {
		glGenVertexArrays(1, &vao);
		existVao = true;
	}
}
void Mesh::delVao() {
	if (existVao) {
		glDeleteVertexArrays(1, &vao);
		existVao = false;
	}
}

void Mesh::genVbo() {
	if (!existVbo) {
		glGenBuffers(2, vbo);
		existVbo = true;
	}
}
void Mesh::delVbo() {
	if (existVbo) {
		glDeleteBuffers(2, vbo);
		existVbo = false;
	}
}

void Mesh::genEbo() {
	if (!existEbo) {
		glGenBuffers(1, &ebo);
		existEbo = true;
	}
}
void Mesh::delEbo() {
	if (existEbo) {
		glDeleteBuffers(1, &ebo);
		existEbo = false;
	}
}

void Mesh::genGPUbuffers() {
	genVao();
	genVbo();
	genEbo();
}
void Mesh::delGPUbuffers() {
	delEbo();
	delVbo();
	delVao();
}
void Mesh::push_GPU() {
	{
		genGPUbuffers();
		glBindVertexArray(vao); //--- VAO를 바인드하기
		//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
		//--- triShape 배열의 사이즈: 9 * float		
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(glm::vec3), vertex.data(), GL_STATIC_DRAW);
		//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute 인덱스 0번을 사용가능하게 함
		glEnableVertexAttribArray(0);

		//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		//--- 변수 colors에서 버텍스 색상을 복사한다.
		//--- colors 배열의 사이즈: 9 *float
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec3), color.data(), GL_STATIC_DRAW);
		//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute 인덱스 1번을 사용 가능하게 함.
		glEnableVertexAttribArray(1);

		if(existEbo){
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(unsigned int), index.data(), GL_STATIC_DRAW);
		}
		glBindVertexArray(0); //--- VAO를 바인드하기
	}
}

//---면을 채워서 출력
void Mesh::Fill_Draw(const int& index) const {
	if(index <= indexnum /3 )
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(index * 3 * sizeof(unsigned int)));
}

//---선으로 출력
void Mesh::LINE_Draw(const int& index) const {
	if (index <= indexnum / 3)
		glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void*)(index * 3 * sizeof(unsigned int)));
}

//---정점 갯수에 따라 최대치로 출력
void Mesh::AUTO_Draw(const bool& TRIANGLE) const {
	if (vertexnum < 3) {
		if (vertexnum == 1) {
			glDrawArrays(GL_POINTS, 0, 1);
		}
		else {
			glDrawArrays(GL_LINES, 0, 2);
		}
	}
	else {
		if (TRIANGLE) {
			glDrawElements(GL_TRIANGLES, indexnum, GL_UNSIGNED_INT, 0);
		}
		else {
			for (int i = 0; i < indexnum / 3; i++) {
				glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(unsigned int)));
			}
		}
	}
}

//---mesh에 정보를 세팅
void Mesh::setMesh(const int& mesh, const float& radius) {
	//if (exist()) {
	//	clear();
	//}

	switch (mesh) {
	case MESH_AXIS:
		axis();
		name = "axis";
		break;
	case MESH_TETRAHEDRON:
		tetrahedron();
		name = "tetrahedron";
		break;
	case MESH_SPIRAL:
		spiral();
		name = "spiral";
		break;
	case MESH_CUBE:
		ReadObj("myclass\\resource\\cube.obj");
		name = "cube";
		break;
	case MESH_PYRAMID:
		ReadObj("myclass\\resource\\pyramid.obj");
		name = "pyramid";
		break;
	case MESH_CIRCLE:
		circle(radius);
		name = "circle";
		break;
	case MESH_TRIANGLE:	case MESH_SQUARE:	case MESH_PENTAGON:	case MESH_HEXAGON:	case MESH_HEPTAGON:	case MESH_OCTAGON:
		//std::cout << "생성되는 도형 : " << mesh - MESH_TRIANGLE + 3 << '\n';
		polygon(mesh - MESH_TRIANGLE + 3);
		break;
	}
	polygonnum = indexnum / 3;
}

//---세팅된 정보를 삭제
void Mesh::clear() {
	if (exist()) {
		//polygon은 genbuffer를 존재시엔 하지 않음.(덮어쓰기 됨)
		//glDeleteBuffers(1, &ebo);
		//glDeleteBuffers(2, vbo);
		//glDeleteVertexArrays(1, &vao);
	}
	name = "None";
	vertex.clear();

	vao = 0;
	for (GLuint& i : vbo) i = 0;
	ebo = 0;
	vertexnum = 0;
	indexnum = 0;

	origin_translation = { 0.0f, 0.0f, 0.0f };
	origin_scale = { 1.0f, 1.0f, 1.0f };
	polygon_center = nullptr;
}

//.obj 파일 읽기 샘플 (버전에 따라 수정 필요)
void Mesh::ReadObj(const char* filename) {
	//std::cout << "filename : '" << filename << "'" << '\n';
	FILE* path = fopen(filename, "rt");

	if (!path) {
		std::cout << "##파일 오픈 실패 ##" << '\n';
		return;
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> urd(0.0, 1.0);

	char count[128];
	int vertexnum = 0;
	int facenum = 0;
	int uvnum = 0;
	int vertexnormal = 0;
	//--- 1. 전체 버텍스 개수 및 삼각형 개수 세기
	while (!feof(path)) {
		fscanf(path, "%s", count);
		if (count[0] == 'v' && count[1] == '\0')
			vertexnum++;
		else if (count[0] == 'f' && count[1] == '\0')
			facenum++;
		else if (count[0] == 'v' && count[1] == 't' && count[3] == '\0')
			uvnum++;
		else if (count[0] == 'v' && count[1] == 'n' && count[3] == '\0')
			vertexnormal++;

		memset(count, '\0', sizeof(count));
	}
	rewind(path);

	int vertIndex = 0;
	int vnIndex = 0;
	int faceIndex = 0;
	int uvIndex = 0;


	//--- 2. 메모리 할당
	glm::vec3* vertex = new glm::vec3[vertexnum]();
	glm::vec3* color = new glm::vec3[vertexnum]();
	glm::vec3* vn = new glm::vec3[vertexnormal]();
	glm::uvec3* face = new glm::uvec3[facenum]();
	glm::vec3* uvdata = new glm::vec3[facenum]();
	glm::vec3* normal = new glm::vec3[facenum]();
	glm::vec2* uv = new glm::vec2[uvnum]();
	glm::vec3* center = new glm::vec3[facenum]();

	//std::cout << "2. 메모리 할당 완료" << '\n';


	char bind[128];
	//--- 3. 할당된 메모리에 각 버텍스, 페이스, uv 정보 입력
	while (!feof(path)) {
		fscanf(path, "%s", bind);
		if (bind[0] == 'v' && bind[1] == '\0') {
			fscanf(path, "%f %f %f\n",
				&vertex[vertIndex].x, &vertex[vertIndex].y, &vertex[vertIndex].z);

			{
				//색상(랜덤)
				{/*
					std::random_device rd;
					std::mt19937 gen(rd());
					std::uniform_real_distribution<float> urd(0.0, 1.0);

					color[vertIndex].r = urd(gen);
					color[vertIndex].g = urd(gen);
					color[vertIndex].b = urd(gen);
				*/}
				//색상(정점 위치값 대입)
				{
					color[vertIndex].r = vertIndex % 2 == 0 ? 0.0f : 1.0f;
					color[vertIndex].g = (vertIndex/2) % 2 == 0 ? 0.0f : 1.0f;
					color[vertIndex].b = (vertIndex/4) % 2 == 0 ? 0.0f : 1.0f;
				}

			}
			vertIndex++;
		}
		else if (bind[0] == 'f' && bind[1] == '\0') {
			unsigned int temp_face[3], temp_uv[3], temp_normal[3];
			/*fscanf(path, "%d//%d//%d %d//%d//%d %d//%d//%d\n",
				&temp_face[0], &temp_uv[0], &temp_normal[0],
				&temp_face[1], &temp_uv[1], &temp_normal[1],
				&temp_face[2], &temp_uv[2], &temp_normal[2]);*/
			fscanf(path, "%d//%d %d//%d %d//%d\n",
				&temp_face[0], &temp_normal[0],
				&temp_face[1], &temp_normal[1],
				&temp_face[2], &temp_normal[2]);
			face[faceIndex].x = temp_face[0] - 1;
			face[faceIndex].y = temp_face[1] - 1;
			face[faceIndex].z = temp_face[2] - 1;
			uvdata[faceIndex].x = temp_uv[0];
			uvdata[faceIndex].y = temp_uv[1];
			uvdata[faceIndex].z = temp_uv[2];
			normal[faceIndex].x = temp_normal[0];
			normal[faceIndex].y = temp_normal[1];
			normal[faceIndex].z = temp_normal[2];
			faceIndex++;
		}
		else if (bind[0] == 'v' && bind[1] == 't' && bind[2] == '\0') {
			fscanf(path, "%f %f\n", &uv[uvIndex].x, &uv[uvIndex].y);
			uvIndex++;
		}
		else if (bind[0] == 'v' && bind[1] == 'n' && bind[2] == '\0') {
			fscanf(path, "%f %f %f\n",
				&vn[vnIndex].x, &vn[vnIndex].y, &vn[vnIndex].z);
			vnIndex++;
		}
	}
	fclose(path);

	//--- 필요한 경우 읽어온 값을 전역 변수 등에 저장
	vertexnum = vertexnum;
	indexnum = facenum * 3;
	for (int i = 0; i < facenum; i++) {
		for (int j = 0; j < 3; j++) {
			center[i][j] = (vertex[face[i][0]][j] + vertex[face[i][1]][j] + vertex[face[i][2]][j]) / 3;
		}
	}

	{
		genGPUbuffers();
		glBindVertexArray(vao); //--- VAO를 바인드하기


		//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
		//--- triShape 배열의 사이즈: 9 * float		
		glBufferData(GL_ARRAY_BUFFER, vertexnum * sizeof(glm::vec3), vertex, GL_STATIC_DRAW);
		//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute 인덱스 0번을 사용가능하게 함
		glEnableVertexAttribArray(0);

		//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		//--- 변수 colors에서 버텍스 색상을 복사한다.
		//--- colors 배열의 사이즈: 9 *float
		glBufferData(GL_ARRAY_BUFFER, vertexnum * sizeof(glm::vec3), color, GL_STATIC_DRAW);
		//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute 인덱스 1번을 사용 가능하게 함.
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, facenum * sizeof(glm::uvec3), face, GL_STATIC_DRAW);


		glBindVertexArray(0); //--- VAO를 바인드하기

		{
			glDepthMask(GL_TRUE);
		}

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			// OpenGL 오류 처리 코드
			std::cerr << "OpenGL 오류 발생: " << error << std::endl;
		}

		polygon_center = center;

		delete[] vertex;
		delete[] color;
		delete[] vn;
		delete[] face;	//--- 여기서 HEAP 에러 뜸
		delete[] uvdata;
		delete[] normal;
		delete[] uv;

	}
}

//--- 정사면체 mesh값으로 초기화 시켜줌.
void Mesh::tetrahedron() {
	glm::vec3* vertex = new glm::vec3[4];

	vertex[0].x = 0.0f;
	vertex[0].y = glm::sin(glm::radians(60.0)) * 1.0f;
	vertex[0].z = 0.0f;

	vertex[1].x = cos(glm::radians(90.0)) * 1.0f;
	vertex[1].y = glm::sin(glm::radians(60.0)) * -1.0f * 1 / 2;
	vertex[1].z = sin(glm::radians(90.0)) * 1.0f;

	vertex[2].x = cos(glm::radians(210.0)) * 1.0f;
	vertex[2].y = glm::sin(glm::radians(60.0)) * -1.0f * 1 / 2;
	vertex[2].z = sin(glm::radians(210.0)) * 1.0f;

	vertex[3].x = cos(glm::radians(330.0)) * 1.0f;
	vertex[3].y = glm::sin(glm::radians(60.0)) * -1.0f * 1 / 2;
	vertex[3].z = sin(glm::radians(330.0)) * 1.0f;

	glm::vec3* color = new glm::vec3[4];



	//색상(랜덤)

	//{
	//	std::random_device rd;
	//	std::mt19937 gen(rd());
	//	std::uniform_real_distribution<float> urd(0.0, 1.0);
	//	for (int i = 0; i < 4; i++) {
	//		color[i].r = urd(gen);
	//		color[i].g = urd(gen);
	//		color[i].b = urd(gen);
	//	}
	//}

	//색상(정점 위치값 대입)
	{
		for (int i = 0; i < 4; i++) {
			color[i].r = vertex[i].x;
			color[i].g = vertex[i].y;
			color[i].b = vertex[i].z;
		}
	}

	glm::uvec3* face = new glm::uvec3[4];

	face[0].x = 0;
	face[0].y = 1;
	face[0].z = 2;

	face[1].x = 0;
	face[1].y = 2;
	face[1].z = 3;

	face[2].x = 0;
	face[2].y = 3;
	face[2].z = 1;

	face[3].x = 1;
	face[3].y = 2;
	face[3].z = 3;

	//갯수 설정
	vertexnum = 4;
	indexnum = 4 * 3;

	genGPUbuffers();
	glBindVertexArray(vao); //--- VAO를 바인드하기


	/*	std::cout << "sizeof(glm::vec3) : " << sizeof(glm::vec3) << '\n';
		std::cout << "sizeof(vertex) : " << sizeof(vertex) << '\n';
		std::cout << "vertexnum * sizeof(glm::vec3) :" << vertexnum * sizeof(glm::vec3) << '\n';*/

		//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
	//--- triShape 배열의 사이즈: 9 * float		
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), vertex, GL_STATIC_DRAW);
	//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 0번을 사용가능하게 함
	glEnableVertexAttribArray(0);

	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	//--- colors 배열의 사이즈: 9 *float
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), color, GL_STATIC_DRAW);
	//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(glm::uvec3), face, GL_STATIC_DRAW);

	glBindVertexArray(0); //--- VAO를 바인드하기

	origin_translation = { 0.0f, 0.0f, 0.0f };
	origin_scale = { 1.0f, 1.0f, 1.0f };

	delete[] vertex;
	delete[] color;
}
//--- x, y, z축 mesh값으로 초기화 시켜줌.
void Mesh::axis() {
	glm::vec3 vertex[6];
	glm::vec3 color[6];
	GLuint index[6];

	const float LEN{ 1.0f };
	for (int i = 0; i < 6; i++) {
		vertex[i].x = i / 2 == 0 ? 1.0f : 0.0f;
		vertex[i].x *= i % 2 == 0 ? -1 : 1;
		vertex[i].y = i / 2 == 1 ? 1.0f : 0.0f;
		vertex[i].y *= i % 2 == 0 ? -1 : 1;
		vertex[i].z = i / 2 == 2 ? 1.0f : 0.0f;
		vertex[i].z *= i % 2 == 0 ? -1 : 1;

		color[i].x = i / 2 == 0 ? 1.0f : 0.0f;
		color[i].y = i / 2 == 1 ? 1.0f : 0.0f;
		color[i].z = i / 2 == 2 ? 1.0f : 0.0f;

		index[i] = i;
	}


	{
		genGPUbuffers();
		glBindVertexArray(vao); //--- VAO를 바인드하기


		//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
		glBindBuffer(GL_ARRAY_BUFFER,vbo[0]);
		//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
		//--- triShape 배열의 사이즈: 9 * float		
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec3), vertex, GL_STATIC_DRAW);
		//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute 인덱스 0번을 사용가능하게 함
		glEnableVertexAttribArray(0);

		//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		//--- 변수 colors에서 버텍스 색상을 복사한다.
		//--- colors 배열의 사이즈: 9 *float
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec3), color, GL_STATIC_DRAW);
		//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute 인덱스 1번을 사용 가능하게 함.
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), index, GL_STATIC_DRAW);

		glBindVertexArray(0); //--- VAO를 바인드하기
	}

	name = "axis";
	vertexnum = 6;
	indexnum = 6;
}

void Mesh::spiral() {	
	std::vector<float> vertex;
	std::vector<float> color;

	float LEN{ 0.0f };
	float degree{ 0.0f };
	int count{ 0 };
	while (LEN < 1.0f) {
		vertex.push_back(cos(glm::radians(degree)) * LEN);	//x
		vertex.push_back(0.0f);	//y
		vertex.push_back(sin(glm::radians(degree)) * LEN);	//z


		color.push_back(0.4f);
		color.push_back(0.64f);
		color.push_back(1.0f);
		/*color.push_back(random_number(0.0f, 1.0f));
		color.push_back(random_number(0.0f, 1.0f));
		color.push_back(random_number(0.0f, 1.0f));*/
		degree += 1.0f;
		if (degree >= 360.0f) {
			count++;
			degree -= 360.0f;
		}
		LEN += 0.0003f;
	}


	{
		genGPUbuffers();
		glBindVertexArray(vao); //--- VAO를 바인드하기


		//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
		//--- triShape 배열의 사이즈: 9 * float		
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), vertex.data(), GL_STATIC_DRAW);
		//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute 인덱스 0번을 사용가능하게 함
		glEnableVertexAttribArray(0);

		//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		//--- 변수 colors에서 버텍스 색상을 복사한다.
		//--- colors 배열의 사이즈: 9 *float
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(float), color.data(), GL_STATIC_DRAW);
		//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute 인덱스 1번을 사용 가능하게 함.
		glEnableVertexAttribArray(1);

		glBindVertexArray(0); //--- VAO를 바인드하기
	}

	name = "spiral";
	vertexnum = vertex.size() / 3;
	indexnum = 0;
}

void Mesh::circle(const float& radius) {
	std::vector<float> vertex;
	std::vector<float> color;

	float LEN{ 1.0f };
	float degree{ 0.0f };
	int count{ 0 };
	while (degree < 360.0f) {
		vertex.push_back(cos(glm::radians(degree)) * LEN);	//x
		vertex.push_back(sin(glm::radians(degree)) * LEN);	//y
		vertex.push_back(0.0f);	//z


		color.push_back(0.4f);
		color.push_back(0.64f);
		color.push_back(1.0f);
		/*color.push_back(random_number(0.0f, 1.0f));
		color.push_back(random_number(0.0f, 1.0f));
		color.push_back(random_number(0.0f, 1.0f));*/
		degree += 1.0f;
		
	}


	{
		genGPUbuffers();
		glBindVertexArray(vao); //--- VAO를 바인드하기


		//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
		//--- triShape 배열의 사이즈: 9 * float		
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), vertex.data(), GL_STATIC_DRAW);
		//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute 인덱스 0번을 사용가능하게 함
		glEnableVertexAttribArray(0);

		//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		//--- 변수 colors에서 버텍스 색상을 복사한다.
		//--- colors 배열의 사이즈: 9 *float
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(float), color.data(), GL_STATIC_DRAW);
		//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute 인덱스 1번을 사용 가능하게 함.
		glEnableVertexAttribArray(1);

		glBindVertexArray(0); //--- VAO를 바인드하기
	}

	name = "circle";
	vertexnum = vertex.size() / 3;
	indexnum = 0;
}

void Mesh::polygon(const int& polygon) {
	//name = polygon + "각형";
	//std::cout << "생성할 polygon :" << polygon << "\n";
	vertex.clear();
	color.clear();
	index.clear();
	std::vector<float> vertex;
	std::vector<float> color;
	std::vector<unsigned int> index;

	float LEN{ 1.0f };
	float degree{ 360.0f / polygon };
	unsigned int count{ 0 };

	while (count < polygon) {
		//vertex.push_back({cos(glm::radians(degree * count)) * LEN, sin(glm::radians(degree * count)) * LEN, 0.0f});	//x
		vertex.push_back(cos(glm::radians(degree * count)) * LEN);	//y
		vertex.push_back(sin(glm::radians(degree * count)) * LEN);	//y
		vertex.push_back(0.0f);	//z
		this-> vertex.push_back({ cos(glm::radians(degree * count)) * LEN, sin(glm::radians(degree * count)) * LEN, 0.0f });	//x
		

		color.push_back(rainbow[count % 8].x);
		color.push_back(rainbow[count % 8].y);
		color.push_back(rainbow[count % 8].z);
		this->color.push_back({ rainbow[count % 8].x, rainbow[count % 8].y, rainbow[count % 8].z });	//x
		/*color.push_back(random_number(0.0f, 1.0f));
		color.push_back(random_number(0.0f, 1.0f));
		color.push_back(random_number(0.0f, 1.0f));*/
		if (count >= 2) {
			index.push_back(0);
			index.push_back(count - 1);
			index.push_back(count);
			this->index.push_back(0);
			this->index.push_back(count - 1);
			this->index.push_back(count);
		}
		count++;
	}
	{
		genGPUbuffers();
		glBindVertexArray(vao); //--- VAO를 바인드하기


		//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
		//--- triShape 배열의 사이즈: 9 * float		

		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), vertex.data(), GL_STATIC_DRAW);

		//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute 인덱스 0번을 사용가능하게 함
		glEnableVertexAttribArray(0);

		//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		//--- 변수 colors에서 버텍스 색상을 복사한다.
		//--- colors 배열의 사이즈: 9 *float
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(float), color.data(), GL_STATIC_DRAW);
		//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute 인덱스 1번을 사용 가능하게 함.
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(unsigned int), index.data(), GL_STATIC_DRAW);


		glBindVertexArray(0); //--- VAO를 바인드하기
	}

	vertexnum = vertex.size() / 3;
	indexnum = index.size();

	switch (polygon) {
	case 3:
		name = "삼각형";
		break;
	case 4:
		name = "사각형";
		break;
	case 5:
		name = "오각형";
		break;
	case 6:
		name = "육각형";
		break;
	case 7:
		name = "칠각형";
		break;
	case 8:
		name = "팔각형";
		break;
	}
}

bool Mesh::exist() const {
	if (!name.compare("None")) {
		//std::cout << "현재 mesh에 저장된 값 없음." << '\n';
		return false;
	}
	//std::cout << "현재 mesh에 저장된 값 :" << name << '\n';
	return true;
}

std::string Mesh::getName() const {
	return name;
}

void Mesh::Object_Space_Transform(glm::mat4& transformMatrix) const {
	transformMatrix = glm::scale(transformMatrix, origin_scale);		//--- 확대축소 변환(Scaling)
	transformMatrix = glm::translate(transformMatrix, origin_translation); //--- 이동(Translation)	
}


void Mesh::line_initBuffers(const glm::vec3& start, const glm::vec3& end) {

	vertex.clear();

	std::vector<float> color;
	std::vector<unsigned int> index;

	{	//값을 집어넣음.
		//--정점
		vertex.push_back({ start.x, start.y, 0.0f });
		//vertex.push_back(start.x);
		//vertex.push_back(start.y);
		//vertex.push_back(0.0f);

		vertex.push_back({ end.x, end.y, 0.0f });
		//vertex.push_back(end.x);
		//vertex.push_back(end.y);
		//vertex.push_back(0.0f);
		//--색상
		color.push_back(0.0f);
		color.push_back(0.0f);
		color.push_back(0.0f);

		color.push_back(0.0f);
		color.push_back(0.0f);
		color.push_back(0.0f);

		index.push_back(0);
		index.push_back(1);
	}

	{
		genGPUbuffers();
		glBindVertexArray(vao); //--- VAO를 바인드하기

		//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
		//--- triShape 배열의 사이즈: 9 * float		
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(glm::vec3), vertex.data(), GL_STATIC_DRAW);
		//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute 인덱스 0번을 사용가능하게 함
		glEnableVertexAttribArray(0);

		//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		//--- 변수 colors에서 버텍스 색상을 복사한다.
		//--- colors 배열의 사이즈: 9 *float
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(float), color.data(), GL_STATIC_DRAW);
		//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute 인덱스 1번을 사용 가능하게 함.
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(unsigned int), index.data(), GL_STATIC_DRAW);


		glBindVertexArray(0); //--- VAO를 바인드하기
	}
	name = "Line";
}
