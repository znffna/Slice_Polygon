#include "Mesh.hpp"
#include "Default.hpp"

//--- ������
Mesh::Mesh() {
	name = "None";
	vao = 0;
	for (GLuint& i : vbo) i = 0;
	ebo = 0;

	vertexnum = 0;
	indexnum = 0;
	polygonnum = 0;

	origin_translation = { 0.0f, 0.0f, 0.0f };
	origin_scale = { 1.0f, 1.0f, 1.0f };
	polygon_center = nullptr;
}

//---���� ä���� ���
void Mesh::Fill_Draw(const int& index) const {
	if(index <= indexnum /3 )
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(index * 3 * sizeof(unsigned int)));
}

//---������ ���
void Mesh::LINE_Draw(const int& index) const {
	if (index <= indexnum / 3)
		glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void*)(index * 3 * sizeof(unsigned int)));
}

//---mesh�� ������ ����
void Mesh::setMesh(const int& mesh, const float& radius) {
	if (exist()) {
		clear();
	}

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
	}
	polygonnum = indexnum / 3;
}

//---���õ� ������ ����
void Mesh::clear() {
	if (exist()) {
		glDeleteBuffers(1, &ebo);
		glDeleteBuffers(2, vbo);
		glDeleteVertexArrays(1, &vao);
	}
	name = "None";
	vao = 0;
	for (GLuint& i : vbo) i = 0;
	ebo = 0;
	vertexnum = 0;
	indexnum = 0;
	origin_translation = { 0.0f, 0.0f, 0.0f };
	origin_scale = { 1.0f, 1.0f, 1.0f };
	polygon_center = nullptr;
}

//.obj ���� �б� ���� (������ ���� ���� �ʿ�)
void Mesh::ReadObj(const char* filename) {
	//std::cout << "filename : '" << filename << "'" << '\n';
	FILE* path = fopen(filename, "rt");

	if (!path) {
		std::cout << "##���� ���� ���� ##" << '\n';
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
	//--- 1. ��ü ���ؽ� ���� �� �ﰢ�� ���� ����
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


	//--- 2. �޸� �Ҵ�
	glm::vec3* vertex = new glm::vec3[vertexnum]();
	glm::vec3* color = new glm::vec3[vertexnum]();
	glm::vec3* vn = new glm::vec3[vertexnormal]();
	glm::uvec3* face = new glm::uvec3[facenum]();
	glm::vec3* uvdata = new glm::vec3[facenum]();
	glm::vec3* normal = new glm::vec3[facenum]();
	glm::vec2* uv = new glm::vec2[uvnum]();
	glm::vec3* center = new glm::vec3[facenum]();

	//std::cout << "2. �޸� �Ҵ� �Ϸ�" << '\n';


	char bind[128];
	//--- 3. �Ҵ�� �޸𸮿� �� ���ؽ�, ���̽�, uv ���� �Է�
	while (!feof(path)) {
		fscanf(path, "%s", bind);
		if (bind[0] == 'v' && bind[1] == '\0') {
			fscanf(path, "%f %f %f\n",
				&vertex[vertIndex].x, &vertex[vertIndex].y, &vertex[vertIndex].z);

			{
				//����(����)
				{/*
					std::random_device rd;
					std::mt19937 gen(rd());
					std::uniform_real_distribution<float> urd(0.0, 1.0);

					color[vertIndex].r = urd(gen);
					color[vertIndex].g = urd(gen);
					color[vertIndex].b = urd(gen);
				*/}
				//����(���� ��ġ�� ����)
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

	//--- �ʿ��� ��� �о�� ���� ���� ���� � ����
	vertexnum = vertexnum;
	indexnum = facenum * 3;
	for (int i = 0; i < facenum; i++) {
		for (int j = 0; j < 3; j++) {
			center[i][j] = (vertex[face[i][0]][j] + vertex[face[i][1]][j] + vertex[face[i][2]][j]) / 3;
		}
	}

	{
		glGenVertexArrays(1, &vao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
		glBindVertexArray(vao); //--- VAO�� ���ε��ϱ�

		glGenBuffers(2, vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

		//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		//--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
		//--- triShape �迭�� ������: 9 * float		
		glBufferData(GL_ARRAY_BUFFER, vertexnum * sizeof(glm::vec3), vertex, GL_STATIC_DRAW);
		//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute �ε��� 0���� ��밡���ϰ� ��
		glEnableVertexAttribArray(0);

		//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
		//--- colors �迭�� ������: 9 *float
		glBufferData(GL_ARRAY_BUFFER, vertexnum * sizeof(glm::vec3), color, GL_STATIC_DRAW);
		//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute �ε��� 1���� ��� �����ϰ� ��.
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &ebo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, facenum * sizeof(glm::uvec3), face, GL_STATIC_DRAW);


		glBindVertexArray(0); //--- VAO�� ���ε��ϱ�

		{
			glDepthMask(GL_TRUE);
		}

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			// OpenGL ���� ó�� �ڵ�
			std::cerr << "OpenGL ���� �߻�: " << error << std::endl;
		}

		polygon_center = center;

		delete[] vertex;
		delete[] color;
		delete[] vn;
		delete[] face;	//--- ���⼭ HEAP ���� ��
		delete[] uvdata;
		delete[] normal;
		delete[] uv;


		/*
		{	//�� �ҷ��Դ��� Ȯ�ο� �ܼ� ���
			std::cout << "vertexnum: " << vertexnum << "\n";
			for (int i = 0; i < vertexnum; i++) {
				std::cout << "vertex[" << i << "] : (" << vertex[i].x << ", " << vertex[i].y << ", " << vertex[i].z << ") \n";
			}
			for (int i = 0; i < vertexnum; i++) {
				std::cout << "color[" << i << "] : (" << color[i].r << ", " << color[i].g << ", " << color[i].b << ") \n";
			}
			std::cout << "facenum: " << facenum << "\n";
			for (int i = 0; i < facenum; i++) {
				std::cout << "face[" << i << "] : (" << face[i].x << ", " << face[i].y << ", " << face[i].z << ") \n";
			}
			for (int i = 0; i < facenum; i++) {
				std::cout << "uvdata[" << i << "] : (" << uvdata[i].x << ", " << uvdata[i].y << ", " << uvdata[i].z << ") \n";
			}
			for (int i = 0; i < facenum; i++) {
				std::cout << "normal[" << i << "] : (" << normal[i].x << ", " << normal[i].y << ", " << normal[i].z << ") \n";
			}
			std::cout << "uvnum: " << uvnum <<  "\n";
			for (int i = 0; i < uvnum; i++) {
				std::cout << "uv[" << i << "] : (" << uv[i].x << ", " << uv[i].y << ") \n";
			}
		}
		*/
	}
}

//--- �����ü mesh������ �ʱ�ȭ ������.
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



	//����(����)

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

	//����(���� ��ġ�� ����)
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

	//���� ����
	vertexnum = 4;
	indexnum = 4 * 3;

	glGenVertexArrays(1, &vao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(vao); //--- VAO�� ���ε��ϱ�

	glGenBuffers(2, vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

	/*	std::cout << "sizeof(glm::vec3) : " << sizeof(glm::vec3) << '\n';
		std::cout << "sizeof(vertex) : " << sizeof(vertex) << '\n';
		std::cout << "vertexnum * sizeof(glm::vec3) :" << vertexnum * sizeof(glm::vec3) << '\n';*/

		//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
	//--- triShape �迭�� ������: 9 * float		
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), vertex, GL_STATIC_DRAW);
	//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute �ε��� 0���� ��밡���ϰ� ��
	glEnableVertexAttribArray(0);

	//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
	//--- colors �迭�� ������: 9 *float
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), color, GL_STATIC_DRAW);
	//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//--- attribute �ε��� 1���� ��� �����ϰ� ��.
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &ebo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(glm::uvec3), face, GL_STATIC_DRAW);

	glBindVertexArray(0); //--- VAO�� ���ε��ϱ�

	origin_translation = { 0.0f, 0.0f, 0.0f };
	origin_scale = { 1.0f, 1.0f, 1.0f };

	delete[] vertex;
	delete[] color;
}
//--- x, y, z�� mesh������ �ʱ�ȭ ������.
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
		glGenVertexArrays(1, &vao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
		glBindVertexArray(vao); //--- VAO�� ���ε��ϱ�

		glGenBuffers(2, vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

		//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
		glBindBuffer(GL_ARRAY_BUFFER,vbo[0]);
		//--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
		//--- triShape �迭�� ������: 9 * float		
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec3), vertex, GL_STATIC_DRAW);
		//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute �ε��� 0���� ��밡���ϰ� ��
		glEnableVertexAttribArray(0);

		//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
		//--- colors �迭�� ������: 9 *float
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec3), color, GL_STATIC_DRAW);
		//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute �ε��� 1���� ��� �����ϰ� ��.
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), index, GL_STATIC_DRAW);

		glBindVertexArray(0); //--- VAO�� ���ε��ϱ�
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
		glGenVertexArrays(1, &vao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
		glBindVertexArray(vao); //--- VAO�� ���ε��ϱ�

		glGenBuffers(2, vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

		//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		//--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
		//--- triShape �迭�� ������: 9 * float		
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), vertex.data(), GL_STATIC_DRAW);
		//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute �ε��� 0���� ��밡���ϰ� ��
		glEnableVertexAttribArray(0);

		//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
		//--- colors �迭�� ������: 9 *float
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(float), color.data(), GL_STATIC_DRAW);
		//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute �ε��� 1���� ��� �����ϰ� ��.
		glEnableVertexAttribArray(1);

		glBindVertexArray(0); //--- VAO�� ���ε��ϱ�
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
		vertex.push_back(0.0f);	//y
		vertex.push_back(sin(glm::radians(degree)) * LEN);	//z


		color.push_back(0.4f);
		color.push_back(0.64f);
		color.push_back(1.0f);
		/*color.push_back(random_number(0.0f, 1.0f));
		color.push_back(random_number(0.0f, 1.0f));
		color.push_back(random_number(0.0f, 1.0f));*/
		degree += 1.0f;
		
	}


	{
		glGenVertexArrays(1, &vao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
		glBindVertexArray(vao); //--- VAO�� ���ε��ϱ�

		glGenBuffers(2, vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�

		//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		//--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
		//--- triShape �迭�� ������: 9 * float		
		glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), vertex.data(), GL_STATIC_DRAW);
		//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute �ε��� 0���� ��밡���ϰ� ��
		glEnableVertexAttribArray(0);

		//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
		//--- colors �迭�� ������: 9 *float
		glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(float), color.data(), GL_STATIC_DRAW);
		//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//--- attribute �ε��� 1���� ��� �����ϰ� ��.
		glEnableVertexAttribArray(1);

		glBindVertexArray(0); //--- VAO�� ���ε��ϱ�
	}

	name = "circle";
	vertexnum = vertex.size() / 3;
	indexnum = 0;
}

bool Mesh::exist() const {
	if (!name.compare("None")) {
		//std::cout << "���� mesh�� ����� �� ����." << '\n';
		return false;
	}
	//std::cout << "���� mesh�� ����� �� :" << name << '\n';
	return true;
}

std::string Mesh::getName() const {
	return name;
}

void Mesh::Object_Space_Transform(glm::mat4& transformMatrix) const {
	transformMatrix = glm::scale(transformMatrix, origin_scale);		//--- Ȯ����� ��ȯ(Scaling)
	transformMatrix = glm::translate(transformMatrix, origin_translation); //--- �̵�(Translation)	
}
