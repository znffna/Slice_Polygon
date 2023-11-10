#include "Shader.hpp"

Shader::Shader() {	
	//--- 소스코드 저장 변수
	vertexSource = nullptr;
	fragmentSource = nullptr;
	//--- 세이더 객체
	vertexShader = 0;
	fragmentShader = 0;
	//--- 셰이더 프로그램
	shaderProgramID = 0;

}

//--- 파일을 읽어오는 함수
char* Shader::filetobuf(const char* file) {
	std::ifstream in(file, std::ios_base::binary);

	if (!in)
	{
		std::cout << file << "파일 못찾음" << std::endl;
		exit(1);
	}

	in.seekg(0, std::ios_base::end);
	long len = in.tellg();
	char* buf = new char[len + 1];
	in.seekg(0, std::ios_base::beg);	//orignal
	//in.seekg(3, std::ios_base::beg);

	int cnt = -1;
	while (in >> std::noskipws >> buf[++cnt]) {}
	buf[len] = 0;
	//std::cout << "buf = " << buf << '\n';	//읽어온 파일 콘솔에 출력
	return buf;
}


//--- 세이더 프로그램 생성하기
void Shader::make_shaderProgram(const char* vertex, const char* fragment) {
	make_vertexShaders(vertex); //--- 버텍스 세이더 만들기
	make_fragmentShaders(fragment); //--- 프래그먼트 세이더 만들기
	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	//--- 세이더 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program 사용하기
	glUseProgram(shaderProgramID);
}
//void Shader::make_shaderProgram() {
//	make_vertexShaders("vertex.glsl"); //--- 버텍스 세이더 만들기
//	make_fragmentShaders("fragment.glsl"); //--- 프래그먼트 세이더 만들기
//	//-- shader Program
//	shaderProgramID = glCreateProgram();
//	glAttachShader(shaderProgramID, vertexShader);
//	glAttachShader(shaderProgramID, fragmentShader);
//	glLinkProgram(shaderProgramID);
//	//--- 세이더 삭제하기
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//	//--- Shader Program 사용하기
//	glUseProgram(shaderProgramID);
//}

//--- 버텍스 세이더 객체 만들기
void Shader::make_vertexShaders(const char* vertex) {
	vertexSource = filetobuf(vertex);
	//--- 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);
	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

//--- 프래그먼트 세이더 객체 만들기
void Shader::make_fragmentShaders(const char* fragment) {
	fragmentSource = filetobuf(fragment);
	//--- 프래그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);
	//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void Shader::use() const {
	glUseProgram(shaderProgramID);
}

unsigned int Shader::getUniformLocate(const char* uName) const {
	GLint uniformLocation = glGetUniformLocation(shaderProgramID, uName);
	if (uniformLocation == -1) {
		std::cerr << uName << "- Failed to get uniform variable location!" << std::endl;
	}
	else {
		//std::cout << uName <<"- Uniform variable location: " << uniformLocation << std::endl;
	}
	return uniformLocation;
	//return glGetUniformLocation(shaderProgramID, uName);
}

void Shader::select_color(const int& true_to_vertexcolor) const {
	setInt(true_to_vertexcolor, "selectVertexColor");
}

void Shader::set_color(const glm::vec3& vector) const {
	setVec3(vector, "uniform_color");
}


void Shader::setVec3(const glm::vec3& vector, const char* uniform) const {
	glUniform3fv(getUniformLocate(uniform), 1, glm::value_ptr(vector));
}

void Shader::setInt(const int& value, const char* uniform) const {
	glUniform1i(getUniformLocate(uniform), value);
}


void Shader::worldTransform(const Object& obj) {
	//--- 출력 옵션
	glm::mat4 World_transformMatrix(1.0f);

	obj.World_Transform(World_transformMatrix);
	obj.mesh.Object_Space_Transform(World_transformMatrix);	//오브젝트 파일에서 크기 정규화/ 중점 원점화

	//--- 세이더 프로그램에서 modelTransform 변수 위치 가져오기
	//unsigned int transformLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	//--- modelTransform 변수에 변환 값 적용하기
	glUniformMatrix4fv(getUniformLocate("modelTransform"), 1, GL_FALSE, glm::value_ptr(World_transformMatrix));
	
}

void Shader::worldTransform(const Camera& camera) const {
	//--- 출력 옵션
	glm::mat4 World_transformMatrix(1.0f);

	camera.World_transform(World_transformMatrix);

	//--- 세이더 프로그램에서 modelTransform 변수 위치 가져오기
	//unsigned int transformLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	//--- modelTransform 변수에 변환 값 적용하기
	glUniformMatrix4fv(getUniformLocate("modelTransform"), 1, GL_FALSE, glm::value_ptr(World_transformMatrix));

}

void Shader::worldTransform() const {
	//--- 출력 옵션
	glm::mat4 World_transformMatrix(1.0f);
	//--- modelTransform 변수에 변환 값 적용하기
	glUniformMatrix4fv(getUniformLocate("modelTransform"), 1, GL_FALSE, glm::value_ptr(World_transformMatrix));

}

//뷰 변환
void Shader::viewTransform(const Camera& camera) {
	glm::mat4 view = glm::mat4(1.0f);
	view = camera.View_transform();
	//unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform"); //--- 뷰잉 변환 설정
	glUniformMatrix4fv(getUniformLocate("viewTransform"), 1, GL_FALSE, &view[0][0]);
}

//원근 투영 변환
void Shader::perspectiveTransform(const Camera& camera) {
	glm::mat4 projection = camera.perspective_transform();
	//unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform"); //--- 투영 변환 값 설정
	glUniformMatrix4fv(getUniformLocate("projectionTransform"), 1, GL_FALSE, &projection[0][0]);
}

//직각 투영 변환
void Shader::orthoTransform(const Camera& camera) {
	glm::mat4 projection = camera.ortho_transform();
	//unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform"); //--- 투영 변환 값 설정
	glUniformMatrix4fv(getUniformLocate("projectionTransform"), 1, GL_FALSE, &projection[0][0]);
}
