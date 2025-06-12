// This may be related:
// https://stackoverflow.com/questions/38362596/opengl-imagesize-is-always-zero

#include <fstream>
#include <iostream>
#include <vector>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

static void APIENTRY debugMessageCallback(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam) {
	std::cerr << message << std::endl;
}

int main() {
	/// Initialize GLFW and glad.

	if (glfwInit() != GLFW_TRUE) {
		std::cerr << "Failed to initialize GLFW." << std::endl;

		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Window", nullptr, nullptr);

	if (window == nullptr) {
		std::cerr << "Failed to create GLFW window." << std::endl;

		return 1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD." << std::endl;

		return 1;;
	}

	std::cout << "Using " << glGetString(GL_RENDERER) << std::endl;

	/// Enable debug output.

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(debugMessageCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	/// Load compute shader SPIR-V.

	std::fstream stream("Shader/ImageSize.comp.spv", std::ios::ate | std::ios::binary);

	if (!stream.is_open()) {
		std::cerr << "Failed to open stream." << std::endl;

		return 1;
	}

	const GLsizei length = (GLsizei) stream.tellg();

	std::vector<uint8_t> binary(length);

	stream.seekg(0, std::ios::beg);
	stream.read((char*) binary.data(), length);
	stream.close();

	/// Create compute shader and compile it.

	const GLuint shader = glCreateShader(GL_COMPUTE_SHADER);

	glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, binary.data(), length);
	glSpecializeShader(shader, "main", 0, nullptr, nullptr);

	GLint compileStatus;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == GL_FALSE) {
		GLint infoLogLength;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		std::vector<GLchar> infoLog(infoLogLength);

		glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog.data());
		glDeleteShader(shader);

		std::cerr << infoLog.data() << std::endl;

		return 1;
	}

	/// Create program and link it.

	const GLuint program = glCreateProgram();

	glAttachShader(program, shader);
	glLinkProgram(program);

	GLint linkStatus;

	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == GL_FALSE) {
		GLint infoLogLength;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		std::vector<GLchar> infoLog(infoLogLength);

		glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data());
		glDetachShader(program, shader);
		glDeleteShader(shader);
		glDeleteProgram(program);

		std::cerr << infoLog.data() << std::endl;

		return 1;
	}

	glDetachShader(program, shader);
	glDeleteShader(shader);
	glUseProgram(program);

	/// Create texture.

	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 64, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	/// Create buffer.

	GLuint buffer;

	glGenBuffers(1, &buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 2 * sizeof(uint32_t), nullptr, GL_STATIC_READ);

	/// Dispatch workgroups and delete the texture.

	glDispatchCompute(2, 2, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glDeleteTextures(1, &texture);

	/// Map the buffer and read it.

	const uint32_t* data = (uint32_t*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

	std::cout << "Width = " << data[0] << std::endl;
	std::cout << "Height = " << data[1] << std::endl;

	/// Cleanup.

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glDeleteBuffers(1, &buffer);
	glDeleteProgram(program);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
