#include <MACE/Graphics/Shaders.h>
#include <MACE/Graphics/GLUtil.h>
#include <MACE/Graphics/GraphicsConstants.h>
#include <iostream>

namespace mc {
namespace gfx {
	int ShaderProgram::createShader(const char code[], const  GLenum& type)
	{
		if (id == -1)createProgram();
		const int shaderId = glCreateShader(type);

		checkGLError();

		if (shaderId == 0) {
			throwShaderError(shaderId, type, "Failed to retrieve shader ID");
		}

		glShaderSource(shaderId, 1, &code, 0);
		glCompileShader(shaderId);

		GLint result = -1;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
		if (result == 0) {
			throwShaderError(shaderId, type, "Shader failed to compile");
		}

		glAttachShader(id, shaderId);

		return shaderId;
	}
	void ShaderProgram::createProgram()
	{
		id = glCreateProgram();
		checkGLError();
		if (id == 0) {
			throwShaderError(id, GL_PROGRAM, "Failed to retrieve program ID");
		}
	}
	ShaderProgram::ShaderProgram()
	{
	}
	ShaderProgram::~ShaderProgram()
	{
	}
	void ShaderProgram::init()
	{
		if (id == 0)createProgram();

		glLinkProgram(id);

		GLint result = -1;
		glGetProgramiv(id, GL_LINK_STATUS, &result);
		if (result == 0) {
			throwShaderError(id, GL_PROGRAM, "The shader program was unable to link with result " + std::to_string(result));
		}

		checkGLError();

		glValidateProgram(id);

		glGetProgramiv(id, GL_VALIDATE_STATUS, &result);
		if (result == 0) {
			throwShaderError(id, GL_PROGRAM, "The shader program failed validation." + std::to_string(result));
		}
	}
	void ShaderProgram::destroy()
	{
		if (id > 0) {
			unbind();
			if (vertId != 0) {
				glDetachShader(id, vertId);
			}
			if (fragId != 0) {
				glDetachShader(id, fragId);
			}
			glDeleteProgram(id);
		}
		checkGLError();
	}
	void ShaderProgram::bind() const
	{
		glUseProgram(id);
	}
	void ShaderProgram::unbind() const
	{
		glUseProgram(0);
	}
	void ShaderProgram::createFragment(const char shader[])
	{
		fragId = createShader(shader, GL_FRAGMENT_SHADER);
	}
	void ShaderProgram::createVertex(const char shader[])
	{
		vertId = createShader(shader, GL_VERTEX_SHADER);
	}

	void ShaderProgram::createUniform(const std::string& name)
	{
		int location = glGetUniformLocation(id,name.data());
		if (location < 0) {
			throw ShaderError("Error finding uniform with name "+std::string(name));
		}
		uniforms[name] = location;
	}

	void ShaderProgram::createUniform(const char * name)
	{
		createUniform(std::string(name));
	}

	int ShaderProgram::getUniformLocation(const std::string& name)
	{
		return uniforms[name];
	}

	int ShaderProgram::getUniformLocation(const char * name)
	{
		return uniforms[name];
	}

	int ShaderProgram::getProgramID() const
	{
		return id;
	}

	int ShaderProgram::getVertexID() const
	{
		return vertId;
	}

	int ShaderProgram::getFragmentID() const
	{
		return fragId;
	}
}
}