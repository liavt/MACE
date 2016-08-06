#include <MC-Graphics/Render.h>
#include <MC-Graphics/GLUtil.h>
#include <MC-Graphics/GraphicsConstants.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace mc {
	namespace gfx{
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

	void ShaderProgram::createUniform(char name[])
	{
		int location = glGetUniformLocation(id, name);
		checkGLError();
		if (location < 0) {
			throw ShaderError("Shader of name "+std::string(name)+" not found!");
		}
		uniforms[name] = location;
	}

	Index ShaderProgram::getUniformLocation(char * name)
	{
		return uniforms[name];
	}

	void ShaderProgram::setUniform(char * name, const Color & c)
	{
		glUniform4f(uniforms[name],c.r,c.g,c.b,c.a);
	}

	void ShaderProgram::setUniform(char * name, const ByteField & b)
	{
		glUniform1i(uniforms[name],b.get());
	}

	void ShaderProgram::setUniform(char * name, const TransformMatrix & m)
	{
		setUniform(name,true,m.get());
	}

	RawModel::~RawModel()
	{
		destroy();
	}

	void RawModel::bind()
	{
		glBindVertexArray(vaoID);
	}
	void RawModel::unbind()
	{
		glBindVertexArray(0);
	}
	void RawModel::draw(GLenum type)
	{
		draw(type, 0);
	}
	void RawModel::draw(GLenum type, Index offset)
	{
		glDrawElements(type, indiceNumber,GL_UNSIGNED_INT,0);
	}
	void RawModel::destroy()
	{
	}
	void RawModel::loadVertices(const Size & verticeSize, const GLfloat vertices[])
	{
		vaoID = createVAO();
		vertexNumber = verticeSize;


		bind();
		storeDataInAttributeList(vertices,0, 3, vertexNumber);
		unbind();
	}

	void RawModel::loadTextureCoordinates(const Size & verticeSize, const GLfloat vertices[])
	{
		bind();
		storeDataInAttributeList(vertices, 1, 2, vertexNumber);
		unbind();
	}

	void RawModel::loadIndices(const Size & indiceNum, const GLuint indices[])
	{
		bind();
		Index indicesID;
		// Generate 1 buffer, put the resulting identifier in vertexbuffer
		glGenBuffers(1, &indicesID);//
								// The following commands will talk about our 'vertexbuffer' buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);//
										// Give our vertices to OpenGL.
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*(indiceNum), indices, GL_STATIC_DRAW);//

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);//

		gfx::checkGLError();//

		this->indiceNumber = indiceNum;

		unbind();
	}
	Index RawModel::createVAO()
	{
		Index id;
		glGenVertexArrays(1, &id);
		checkGLError();
		return id;
	}
	void RawModel::storeDataInAttributeList(const GLfloat data[],const Index & attributeNumber, const Size& attributeSize,const Size & verticeSize)
	{
		Index vboID;
		// Generate 1 buffer, put the resulting identifier in vertexbuffer
		glGenBuffers(1, &vboID);//
								// The following commands will talk about our 'vertexbuffer' buffer
		glBindBuffer(GL_ARRAY_BUFFER, vboID);//
											 // Give our data to opengl
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(attributeSize * verticeSize), data, GL_STATIC_DRAW);//
		gfx::checkGLError();//

		glVertexAttribPointer(attributeNumber, attributeSize, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(attributeNumber);


	//	glBindBuffer(GL_ARRAY_BUFFER, vboID);//
		checkGLError();
	}

	void Texture::generateTexture()
	{
		glGenTextures(1, &id);

		bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		unbind();

		checkGLError();
	}

	Texture::Texture()
	{
		id = 0;
	}

	Texture::Texture(Index id)
	{
		this->id = id;
	}

	Texture::Texture(const char * file)
	{
		generateTexture();
		bind();
		loadFile(file);
		unbind();
	}

	Texture::Texture(const GLvoid * data, Size width, Size height, GLenum type, GLenum format, GLenum internalFormat, Index mipmapLevel)
	{
		generateTexture();
		bind();
		setData(data, width, height, type, format, internalFormat, mipmapLevel);
		unbind();
	}

	void Texture::setData(const GLvoid * data, Size width, Size height, GLenum type, GLenum format, GLenum internalFormat, Index mipmapLevel)
	{
		glTexImage2D(target,mipmapLevel,internalFormat,width,height,0,format,type,data);

		checkGLError();
	}

	void Texture::loadFile(const char * file)
	{
		int width,height,componentSize;
		
		Byte* image = stbi_load(file,&width,&height,&componentSize,STBI_rgb_alpha);

		setData(image,width,height,GL_UNSIGNED_BYTE,GL_RGBA,GL_RGBA);

		stbi_image_free(image);
	}

	void Texture::setTarget(GLenum target)
	{
		this->target = target;
	}

	GLenum Texture::getTarget()
	{
		return target;
	}

	void Texture::bind()
	{
		glBindTexture(target, id);
	}

	void Texture::unbind()
	{
		glBindTexture(target, 0);
	}

}
}