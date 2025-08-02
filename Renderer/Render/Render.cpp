#include "Render.h"
#include "Mesh.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <SDL3/SDL_opengl.h>
#undef near
#undef far
#include "../def.h"

#pragma region printing
inline std::ostream& operator<<(std::ostream& stream, glm::vec3 vec) {
	return stream << '(' << vec.x << ',' << vec.y << ',' << vec.z << ')';
}
inline std::ostream& operator<<(std::ostream& stream, glm::vec4 vec) {
	return stream << '(' << vec.x << ',' << vec.y << ',' << vec.z << ',' << vec.w << ')';
}
inline std::ostream& operator<<(std::ostream& stream, glm::mat4 mat) {
	int bufSize=snprintf(0, 0, "%8.3f,%8.3f,%8.3f,%8.3f,\n%8.3f,%8.3f,%8.3f,%8.3f,\n%8.3f,%8.3f,%8.3f,%8.3f,\n%8.3f,%8.3f,%8.3f,%8.3f,\n",
		mat[0][0], mat[1][0], mat[2][0], mat[3][0],
		mat[0][1], mat[1][1], mat[2][1], mat[3][1],
		mat[0][2], mat[1][2], mat[2][2], mat[3][2],
		mat[0][3], mat[1][3], mat[2][3], mat[3][3]
		)+1;
	char* buf = new char[bufSize];
	snprintf(buf, bufSize, "%8.3f,%8.3f,%8.3f,%8.3f,\n%8.3f,%8.3f,%8.3f,%8.3f,\n%8.3f,%8.3f,%8.3f,%8.3f,\n%8.3f,%8.3f,%8.3f,%8.3f,\n",
		mat[0][0], mat[1][0], mat[2][0], mat[3][0],
		mat[0][1], mat[1][1], mat[2][1], mat[3][1],
		mat[0][2], mat[1][2], mat[2][2], mat[3][2],
		mat[0][3], mat[1][3], mat[2][3], mat[3][3]
		);
	stream << buf;
	delete[] buf;
	return stream;
}
#pragma endregion

void Camera::Init() {
	fov = 90;
	near = 0.1;
	far = 1000;
	position = glm::vec3(0, 0, -5);
	rotation = glm::vec3(0, 0, 0);
}

void Renderer::Init(float aspectRatio)
{
	this->aspectRatio = aspectRatio;
	shader = Shader::GetDefaultShader();
	camera.Init();
}

void Renderer::Render() {
	Render_VP();
}

void Renderer::RenderMesh(const Mesh& mesh) {
	glm::mat4 model = mesh.transform.GetMatrix();

	// ��ȡ�� shader �е� uniform ����λ��
	GLuint modelLoc = glGetUniformLocation(shader, "model");
	GLuint viewLoc = glGetUniformLocation(shader, "view");
	GLuint projectionLoc = glGetUniformLocation(shader, "projection");

	// ����Ⱦѭ���У����þ���
	glUseProgram(shader);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//=========Buffer=========
	GLuint VBO, VAO, EBO;

	// ���� VAO��VBO �� EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// �� VAO
	glBindVertexArray(VAO);

	// �� VBO���ϴ���������
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);

	// �� EBO���ϴ���������
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*mesh.tris.size(), mesh.tris.data(), GL_STATIC_DRAW);

	// ���ö�������ָ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0); // ����λ��
	glEnableVertexAttribArray(0);

	//========Render========
	// ����Ⱦѭ����ʹ�� VAO ����ɫ����Ⱦ������
	glDrawElements(GL_TRIANGLES, mesh.tris.size(), GL_UNSIGNED_INT, 0);  // ��Ⱦ������

	//disable
	glDisableVertexAttribArray(0);
	// ��� VAO
	glBindVertexArray(0);
}

void Renderer::Render_VP() {
	// ��ͼ���������λ�ã�
	view = glm::lookAt(camera.position,   // �����λ��
		glm::vec3(0.0f, 0.0f, 0.0f),   // Ŀ���
		glm::vec3(0.0f, 1.0f, 0.0f));  // ������

	// ͶӰ����͸��ͶӰ��
	projection = glm::perspective(glm::radians(camera.fov),   // ��Ұ�Ƕ�
		aspectRatio, // ���ڿ�߱�
		camera.near, camera.far); // ��ƽ����Զƽ��
}
