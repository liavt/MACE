#include <MC-Graphics/GLBindings.h>
#include <MC-Graphics/GLUtil.h>

namespace mc {
	namespace gfx{
	Index ShaderProgram::createShader(const char code[], const  GLenum& type)
	{
		if (id == -1)createProgram();
		const Index shaderId = glCreateShader(type);

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
		unbind();
		if (id != 0) {
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
	void ShaderProgram::init()
	{
		if (id == -1)createProgram();

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
	void ShaderProgram::bind()
	{
		glUseProgram(id);
	}
	void ShaderProgram::unbind()
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

	void RawModel::bind()
	{
		glBindVertexArray(vaoID);
	}
	void RawModel::unbind()
	{
		glBindVertexArray(0);
	}
	void RawModel::destroy()
	{
	}
	void RawModel::load(const Size & verticeSize, const GLfloat vertices[])
	{
		vaoID = createVAO();
		vertexNumber = verticeSize;


		bind();
		storeDataInAttributeList(0, vertexNumber, vertices);
		unbind();
	}
	Index RawModel::createVAO()
	{
		Index id;
		glGenVertexArrays(1, &id);
		checkGLError();
		return id;
	}
	void RawModel::storeDataInAttributeList(const Index & attributeNumber, const Size & verticeSize, const GLfloat data[])
	{
		Index vboID;
		// Generate 1 buffer, put the resulting identifier in vertexbuffer
		glGenBuffers(1, &vboID);//
								// The following commands will talk about our 'vertexbuffer' buffer
		glBindBuffer(GL_ARRAY_BUFFER, vboID);//
											 // Give our vertices to OpenGL.
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(3 * verticeSize), data, GL_STATIC_DRAW);//
		gfx::checkGLError();//

		glVertexAttribPointer(attributeNumber, verticeSize, GL_FLOAT, false, 0, 0);

		glEnableVertexAttribArray(attributeNumber);


		glBindBuffer(GL_ARRAY_BUFFER, vboID);//
		checkGLError();
	}

}
}