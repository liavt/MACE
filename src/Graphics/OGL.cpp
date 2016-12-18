/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/OGL.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <memory>
#include <string>

namespace mc {
	namespace gfx {
		namespace ogl {
			namespace {
				Shader createShader(const Enum& type, const char* source) {
					Shader s = Shader(type);
					s.init();
					s.setSource(source);
					s.compile();
					return s;
				}
			}

			void checkGLError() {
#ifdef MACE_ERROR_CHECK
				GLenum result = GL_NO_ERROR;
				while( (result = glGetError()) != GL_NO_ERROR ) {
					switch( result ) {
					case GL_INVALID_ENUM:
						throw GLError("GL_INVALID_ENUM! An unacceptable value is specified for an enumerated argument!");
						break;
					case GL_INVALID_VALUE:
						throw GLError("GL_INVALID_VALUE! A numeric argument is out of range!");
						break;
					case GL_INVALID_OPERATION:
						throw GLError("GL_INVALID_OPERATION! The specified operation is not allowed in the current state!");
						break;
					case GL_INVALID_FRAMEBUFFER_OPERATION:
						throw GLError("GL_INVALID_FRAMEBUFFER_OPERATION! The command is trying to render to or read from the framebuffer while the currently bound framebuffer is not framebuffer complete (i.e. the return value from glCheckFramebufferStatus is not GL_FRAMEBUFFER_COMPLETE!)");
						break;
					case GL_STACK_OVERFLOW:
						throw GLError("GL_STACK_OVERFLOW! A stack pushing operation cannot be done because it would overflow the limit of that stack's size!");
						break;
					case GL_STACK_UNDERFLOW:
						throw GLError("GL_STACK_UNDERFLOW! A stack popping operation cannot be done because the stack is already at its lowest point.");
						break;
					case GL_OUT_OF_MEMORY:
						throw GLError("GL_OUT_OF_MEMORY! There is not enough memory left to execute the command!");
						break;
					default:
						throw GLError("OpenGL has errored with an error code of " + std::to_string(result));
						break;
					}
				}
#endif
			}
			void throwShaderError(const Index& shaderId, const Enum& type, const std::string& message) {
				if( type == 0 || type == GL_PROGRAM ) {
					throw ShaderError("Error generating shader program with message \"" + message + "\"");
				} else {
					std::unique_ptr<GLchar[]> log_string = std::unique_ptr<GLchar[]>(new char[1024]);
					glGetShaderInfoLog(shaderId, 1024, 0, log_string.get());
					std::string friendlyType = std::to_string(type);//a more human friendly name for type, like VERTEX_SHADER instead of 335030
					if( type == GL_VERTEX_SHADER ) {
						friendlyType = "VERTEX";
					} else if( type == GL_FRAGMENT_SHADER ) {
						friendlyType = "FRAGMENT";
					} else if( type == GL_COMPUTE_SHADER ) {
						friendlyType = "COMPUTE";
					} else if( type == GL_GEOMETRY_SHADER ) {
						friendlyType = "GEOMETERY";
					} else if( type == GL_TESS_CONTROL_SHADER ) {
						friendlyType = "TESSELATION CONTROL";
					} else if( type == GL_TESS_EVALUATION_SHADER ) {
						friendlyType = "TESSELATION EVALUATION";
					}
					throw ShaderError("Error generating shader of type " + friendlyType + " with message \"" + message + "\" and GLSL error " + log_string.get());
				}
			}
			void throwShaderError(const Index & shaderId, const Enum & type) {
				throwShaderError(shaderId, type, "No message was specified");
			}

			void VertexArray::init() {
				glGenVertexArrays(1, &id);
			}

			void VertexArray::destroy() {
				glDeleteVertexArrays(1, &id);

				if( indices.isCreated() ) {
					indices.destroy();
				}

				for( Index i = 0; i < buffers.size(); ++i ) {
					if( buffers[i].isCreated() ) {
						buffers[i].destroy();
					}
				}
			}

			bool VertexArray::isCreated() const {
				return glIsVertexArray(id) == 1;
			}

			void VertexArray::draw(GLenum type) const {
				glDrawElements(type, indices.getIndiceNumber(), GL_UNSIGNED_INT, 0);
			}

			void VertexArray::loadVertices(const Size && verticeSize, const GLfloat vertices[], const Index && location, const Size && attributeSize) {
				vertexNumber = verticeSize;

				bind();
				storeDataInAttributeList(std::move(vertexNumber), vertices, std::move(location), std::move(attributeSize));
			}

			void VertexArray::storeDataInAttributeList(const Size&& dataSize, const GLvoid* data, const Index&& location, const Size&& attributeSize) {
				bind();

				VertexBuffer buffer = VertexBuffer();
				buffer.init();
				buffer.setLocation(location);
				// Give our data to opengl
				buffer.setData(sizeof(GLfloat)*(attributeSize * dataSize), data, GL_DYNAMIC_DRAW);
				buffer.setAttributePointer(attributeSize, GL_FLOAT, GL_FALSE, 0, 0);

				addBuffer(buffer);

				checkGLError();
			}

			void VertexArray::loadIndices(const Size & indiceNum, const unsigned int * indiceData) {
				indices = ElementBuffer(indiceNum);
				indices.init();
				indices.setData(sizeof(unsigned int)*indiceNum, indiceData, GL_STATIC_DRAW);
			}

			void VertexArray::addBuffer(const VertexBuffer & newBuffer) {
				bind();
				newBuffer.bind();
				newBuffer.enable();

				buffers.push_back(newBuffer);
			}

			void VertexArray::setVertexNumber(const Size & vertexNum) {
				vertexNumber = vertexNum;
			}

			Size & VertexArray::getVertexNumber() {
				return vertexNumber;
			}

			const Size & VertexArray::getVertexNumber() const {
				return vertexNumber;
			}

			void VertexArray::setIndices(const ElementBuffer & buffer) {
				indices = buffer;
			}

			ElementBuffer & VertexArray::getIndices() {
				return indices;
			}

			const ElementBuffer & VertexArray::getIndices() const {
				return indices;
			}

			void VertexArray::setBuffers(const std::vector<VertexBuffer>& newBuffers) {
				buffers = newBuffers;
			}

			std::vector<VertexBuffer>& VertexArray::getBuffers() {
				return buffers;
			}

			const std::vector<VertexBuffer>& VertexArray::getBuffers() const {
				return buffers;
			}

			bool VertexArray::operator==(const VertexArray & other) const {
				return vertexNumber == other.vertexNumber&&indices == other.indices&&Object::operator==(other);
			}

			bool VertexArray::operator!=(const VertexArray & other) const {
				return !operator==(other);
			}

			void VertexArray::bindIndex(const Index & ID) const {
				glBindVertexArray(ID);
			}

			UniformBuffer::UniformBuffer() noexcept : Buffer(GL_UNIFORM_BUFFER) {}

			void UniformBuffer::setLocation(const Index loc) {
				this->location = loc;
			}

			Index& UniformBuffer::getLocation() {
				return location;
			}

			const Index& UniformBuffer::getLocation() const {
				return location;
			}

			void UniformBuffer::bindForRender(const Index offset, const GLsizeiptr size) const {
				if( size < 0 && offset == 0 ) {
					glBindBufferBase(GL_UNIFORM_BUFFER, this->location, id);
				} else {
					glBindBufferRange(GL_UNIFORM_BUFFER, this->location, id, offset, size);
				}
			}

			void UniformBuffer::bindToUniformBlock(const Index programID, const char* blockName) const {
				glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, blockName), location);
			}

			bool UniformBuffer::operator==(const UniformBuffer & other) const {
				return location == other.location&&Buffer::operator==(other);
			}

			bool UniformBuffer::operator!=(const UniformBuffer & other) const {
				return !(*this == other);
			}

			void FrameBuffer::init() {
				glGenFramebuffers(1, &id);
			}

			void FrameBuffer::destroy() {
				unbind();
				glDeleteFramebuffers(1, &id);
				id = 0;
			}

			void FrameBuffer::attachTexture(const GLenum & target, const GLenum & attachment, const GLuint & textureID, const GLint & level) {
				bind();
				glFramebufferTexture(target, attachment, textureID, level);
			}

			void FrameBuffer::attachTexture1D(const GLenum & target, const GLenum & attachment, const GLenum & texTarget, const GLuint & textureID, const GLint & level) {
				bind();
				glFramebufferTexture1D(target, attachment, texTarget, textureID, level);
			}

			void FrameBuffer::attachTexture2D(const GLenum & target, const GLenum & attachment, const GLenum & texTarget, const GLuint & textureID, const GLint & level) {
				bind();
				glFramebufferTexture2D(target, attachment, texTarget, textureID, level);
			}

			void FrameBuffer::attachTexture3D(const GLenum & target, const GLenum & attachment, const GLenum & texTarget, const GLuint & textureID, const GLint & level, const GLint& layer) {
				bind();
				glFramebufferTexture3D(target, attachment, texTarget, textureID, level, layer);
			}

			void FrameBuffer::attachTextureLayer(const GLenum & target, const GLenum & attachment, const GLuint& texture, const GLint & level, const GLint & layer) {
				bind();
				glFramebufferTextureLayer(target, attachment, level, texture, layer);
			}

			void FrameBuffer::attachRenderbuffer(const GLenum & target, const GLenum & attachment, const RenderBuffer & buffer) {
				bind();
				glFramebufferRenderbuffer(target, attachment, GL_RENDERBUFFER, buffer.getID());
			}

			void FrameBuffer::setDrawBuffers(const Size & arrSize, const GLenum * buffers) {
				glDrawBuffers(arrSize, buffers);
			}

			void FrameBuffer::setReadBuffer(const Enum & mode) {
				bind();
				glReadBuffer(mode);
			}

			void FrameBuffer::readPixels(const int & x, const int & y, const Size & width, const Size & height, const Enum & format, const Enum & type, void * data) const {
				bind();
				glReadPixels(x, y, width, height, format, type, data);
			}

			void FrameBuffer::setPixelStore(const Enum & name, const float & param) {
				bind();
				glPixelStoref(name, param);
			}

			void FrameBuffer::setPixelStore(const Enum & name, const int & param) {
				bind();
				glPixelStorei(name, param);
			}

			bool FrameBuffer::isCreated() const {
				return glIsFramebuffer(id) == 1;
			}

			GLenum FrameBuffer::checkStatus(const GLenum & target) {
				bind();
				return glCheckFramebufferStatus(target);
			}

			void FrameBuffer::bindIndex(const Index & ID) const {
				glBindFramebuffer(GL_FRAMEBUFFER, ID);
			}

			void RenderBuffer::init() {
				glGenRenderbuffers(1, &id);
			}

			void RenderBuffer::destroy() {
				glDeleteRenderbuffers(1, &id);
			}

			void RenderBuffer::setStorage(const GLenum & format, const GLsizei & width, const GLsizei & height) {
				bind();
				glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
			}

			void RenderBuffer::setStorageMultisampled(const GLsizei & samples, const GLenum & format, const GLsizei & width, const GLsizei & height) {
				bind();
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
			}

			bool RenderBuffer::isCreated() const {
				return glIsRenderbuffer(id) == 1;
			}

			void RenderBuffer::bindIndex(const Index & ID) const {
				glBindRenderbuffer(GL_RENDERBUFFER, ID);
			}

			void Object::bind() const {
				bindIndex(id);
			}

			void Object::unbind() const {
				bindIndex(0);
			}

			Index Object::getID() const {
				return id;
			}

			bool Object::operator==(const Object & other) const {
				return this->id == other.id;
			}

			bool Object::operator!=(const Object & other) const {
				return !operator==(other);
			}

			void Texture::init() {
				glGenTextures(1, &id);
			}

			void Texture::destroy() {
				glDeleteTextures(1, &id);
			}

			void Texture::setData(const void * data, Size width, Size height, GLenum type, GLenum format, GLenum internalFormat, Index mipmapLevel) {
				bind();
				glTexImage2D(target, mipmapLevel, internalFormat, width, height, 0, format, type, data);

				checkGLError();
			}

			void Texture::loadFile(const char * file) {
				int width, height, componentSize;

				Byte* image = stbi_load(file, &width, &height, &componentSize, STBI_rgb_alpha);

				setData(image, width, height, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA);

				stbi_image_free(image);
			}

			void Texture::setTarget(GLenum targ) {
				this->target = targ;
			}

			GLenum Texture::getTarget() {
				return target;
			}

			Color & Texture::getPaint() {
				return paint;
			}

			const Color & Texture::getPaint() const {
				return paint;
			}

			void Texture::setPaint(const Color & c) {
				paint = c;
			}

			float Texture::getOpacity() {
				return opacity;
			}

			const float Texture::getOpacity() const {
				return opacity;
			}

			void Texture::setOpacity(const float f) {
				opacity = f;
			}

			bool Texture::isCreated() const {
				return glIsTexture(id) == 1;
			}

			void Texture::setParameter(const GLenum & name, const GLint & value) {
				bind();
				glTexParameteri(GL_TEXTURE_2D, name, value);
			}

			bool Texture::operator==(const Texture & other) const {
				return target == other.target&&paint == other.paint&&opacity == other.opacity&&Object::operator==(other);
			}

			bool Texture::operator!=(const Texture & other) const {
				return !operator==(other);
			}

			void Texture::bindIndex(const Index & ID) const {
				glBindTexture(target, ID);
			}

			Buffer::Buffer(const Enum& type) noexcept : bufferType(type) {}

			bool Buffer::isCreated() const {
				return glIsBuffer(id) == 1;
			}

			void Buffer::init() {
				glGenBuffers(1, &id);
			}

			void Buffer::destroy() {
				glDeleteBuffers(1, &id);
			};
			void Buffer::setData(const GLsizeiptr& dataSize, const void* data, const Enum& drawType) {
				bind();
				glBufferData(bufferType, dataSize, data, drawType);
			};
			void Buffer::setDataRange(const Index& offset, const GLsizeiptr& dataSize, const void* data) {
				bind();
				glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, data);
			};

			void Buffer::copyData(Buffer& other, const GLsizeiptr& size, const Index& readOffset, const Index& writeOffset) {
				glCopyBufferSubData(id, other.id, readOffset, writeOffset, size);
			};

			void* Buffer::map(const Enum& access) {
				bind();
				return glMapBuffer(bufferType, access);
			};

			void* Buffer::mapRange(const Index& offset, const Size& length, const unsigned int& access) {
				bind();
				return glMapBufferRange(bufferType, offset, length, access);
			};

			bool Buffer::unmap() {
				bind();
				return glUnmapBuffer(bufferType) == 1;
			}
			const Enum & Buffer::getBufferType() const {
				return bufferType;
			}
			bool Buffer::operator==(const Buffer & other) const {
				return this->bufferType == other.bufferType&&Object::operator==(other);
			}
			bool Buffer::operator!=(const Buffer & other) const {
				return !operator==(other);
			}
			void Buffer::bindIndex(const Index & ID) const {
				glBindBuffer(bufferType, ID);
			}

			VertexBuffer::VertexBuffer() noexcept : Buffer(GL_ARRAY_BUFFER) {}

			void VertexBuffer::setAttributePointer(const Byte & attribSize, const Enum & type, const bool & normalized, const Index & stride, const void * pointer) {
				bind();
				if( !normalized && (
					type == GL_BYTE ||
					type == GL_UNSIGNED_BYTE ||
					type == GL_SHORT ||
					type == GL_UNSIGNED_SHORT ||
					type == GL_INT ||
					type == GL_UNSIGNED_INT
					) ) {

					glVertexAttribIPointer(location, attribSize, type, stride, pointer);
				} else {
					glVertexAttribPointer(location, attribSize, type, normalized, stride, pointer);
				}
			}

			void VertexBuffer::setDivisor(const unsigned int & divisor) {
				bind();
				glVertexAttribDivisor(location, divisor);
			}

			void VertexBuffer::enable() const {
				glEnableVertexAttribArray(location);
			}

			void VertexBuffer::disable() const {
				glDisableVertexAttribArray(location);
			}

			Index& VertexBuffer::getLocation() {
				return location;
			}

			const Index& VertexBuffer::getLocation() const {
				return location;
			}

			void VertexBuffer::setLocation(const Index & newLocation) {
				location = newLocation;
			}

			bool VertexBuffer::operator==(const VertexBuffer & other) const {
				return Buffer::operator==(other) && location == other.location;
			}

			bool VertexBuffer::operator!=(const VertexBuffer & other) const {
				return !operator==(other);
			}

			ElementBuffer::ElementBuffer() noexcept : Buffer(GL_ELEMENT_ARRAY_BUFFER) {}

			ElementBuffer::ElementBuffer(const Size & indiceNum) noexcept : ElementBuffer() {
				indiceNumber = indiceNum;
			}

			void ElementBuffer::setIndiceNumber(const Size & indices) {
				indiceNumber = indices;
			}

			Size & ElementBuffer::getIndiceNumber() {
				return indiceNumber;
			}

			const Size & ElementBuffer::getIndiceNumber() const {
				return indiceNumber;
			}

			bool ElementBuffer::operator==(const ElementBuffer & other) const {
				return Buffer::operator==(other) && indiceNumber == other.indiceNumber;
			}

			bool ElementBuffer::operator!=(const ElementBuffer & other) const {
				return !operator==(other);
			}

			CopyReadBuffer::CopyReadBuffer() noexcept : Buffer(GL_COPY_READ_BUFFER) {}

			CopyWriteBuffer::CopyWriteBuffer() noexcept : Buffer(GL_COPY_WRITE_BUFFER) {}

			void QueryObject::begin(const Enum & target) {
				glBeginQuery(target, id);
			}

			void QueryObject::end(const Enum & target) {
				glEndQuery(target);
			}

			void QueryObject::get(const Enum & name, int * data) const {
				glGetQueryObjectiv(id, name, data);
			}

			void QueryObject::get(const Enum & name, unsigned int * data) const {
				glGetQueryObjectuiv(id, name, data);
			}

			void QueryObject::get(const Enum & name, int64_t * data) const {
				glGetQueryObjecti64v(id, name, data);
			}

			void QueryObject::get(const Enum & name, uint64_t * data) const {
				glGetQueryObjectui64v(id, name, data);
			}

			void QueryObject::counter() {
				glQueryCounter(id, GL_TIMESTAMP);
			}

			void QueryObject::init() {
				glGenQueries(1, &id);
			}

			void QueryObject::destroy() {
				glDeleteQueries(1, &id);
			}

			bool QueryObject::isCreated() const {
				return glIsQuery(id) == 1;
			}

			void QueryObject::bind() const {}

			void QueryObject::unbind() const {}

			void QueryObject::bindIndex(const Index & id) const {}

			PixelUnpackBuffer::PixelUnpackBuffer() noexcept : Buffer(GL_PIXEL_UNPACK_BUFFER) {}

			PixelPackBuffer::PixelPackBuffer() noexcept : Buffer(GL_PIXEL_PACK_BUFFER) {}

			Shader::Shader() noexcept : Shader(GL_FALSE) {}

			Shader::Shader(const Enum & shaderType) noexcept : type(shaderType) {}

			void Shader::init() {
				if( type == GL_FALSE ) {
					throw InitializationError("Must assign a type to the shader before init() is called!");
				}
				id = glCreateShader(type);
			}

			void Shader::destroy() {
				glDeleteShader(id);
			}

			void Shader::setSource(const Size & count, const char * strings[], const int lengths[]) {
				if( type == GL_FALSE ) {
					throw ShaderError("Shader must have a type before compile() is called");
				}

				glShaderSource(id, count, strings, lengths);
			}

			void Shader::setSource(const char string[], const int& length) {
				const int lengths[1] = { length };
				const char* strings[1] = { string };
				setSource(1, strings, lengths);
			}

			void Shader::setSource(const std::string & string) {
				setSource(string.c_str(), string.length());
			}

			void Shader::compile() {
				if( type == GL_FALSE ) {
					throw ShaderError("Shader must have a type before compile() is called");
				}
				glCompileShader(id);

				int success = 0;
				glGetShaderiv(id, GL_COMPILE_STATUS, &success);
				if( success == GL_FALSE ) {
					throwShaderError(id, type, "The shader failed to compile with error: " + std::to_string(success));
				}

				checkGLError();
			}

			bool Shader::isCreated() const {
				return glIsShader(id) == 1;
			}

			void Shader::setType(const Enum & newType) {
				type = newType;
			}

			Enum & Shader::getType() {
				return type;
			}

			const Enum & Shader::getType() const {
				return type;
			}

			bool Shader::operator==(const Shader & other) const {
				return Object::operator==(other) && type == other.type;
			}

			bool Shader::operator!=(const Shader & other) const {
				return !operator==(other);
			}

			void Shader::bind() const {}
			void Shader::unbind() const {}
			void Shader::bindIndex(const Index & id) const {}

			void ShaderProgram::bindIndex(const Index & ID) const {
				glUseProgram(ID);
			}

			void ShaderProgram::init() {
				id = glCreateProgram();
				checkGLError();
				if( id == 0 ) {
					throwShaderError(id, GL_PROGRAM, "Failed to retrieve program ID");
				}
			}
			void ShaderProgram::destroy() {
				if( id > 0 ) {
					unbind();
					for( auto s : shaders ) {
						if( s.second.isCreated() ) {
							s.second.destroy();
						}
					}
					glDeleteProgram(id);
				}
				checkGLError();
			}
			void ShaderProgram::link() {
				glLinkProgram(id);

				GLint result = -1;
				glGetProgramiv(id, GL_LINK_STATUS, &result);
				if( result == 0 ) {
					throwShaderError(id, GL_PROGRAM, "The shader program was unable to link with result " + std::to_string(result));
				}

				glValidateProgram(id);

				glGetProgramiv(id, GL_VALIDATE_STATUS, &result);
				if( result == 0 ) {
					throwShaderError(id, GL_PROGRAM, "The shader program failed validation." + std::to_string(result));
				}

				for( auto s : shaders ) {
					if( s.second.isCreated() ) {
						glDetachShader(id, s.second.getID());
					}
				}

				checkGLError();
			}
			bool ShaderProgram::isCreated() const {
				return glIsProgram(id) == 1;
			}
			void ShaderProgram::attachShader(const Shader shader) {
				glAttachShader(id, shader.getID());
				shaders[shader.getType()] = shader;
			}

			void ShaderProgram::createFragment(const char shader[]) {
				attachShader(createShader(GL_FRAGMENT_SHADER, shader));
			}
			void ShaderProgram::createFragment(const std::string & shader) {
				createFragment(shader.c_str());
			}
			void ShaderProgram::createVertex(const char shader[]) {
				attachShader(createShader(GL_VERTEX_SHADER, shader));
			}

			void ShaderProgram::createVertex(const std::string & shader) {
				createVertex(shader.c_str());
			}

			void ShaderProgram::createGeometry(const char shader[]) {
				attachShader(createShader(GL_GEOMETRY_SHADER, shader));
			}

			void ShaderProgram::createGeometry(const std::string & shader) {
				createGeometry(shader.c_str());
			}

			void ShaderProgram::createUniform(const std::string& name) {
				int location = glGetUniformLocation(id, name.data());
				if( location < 0 ) {
					throw ShaderError("Error finding uniform with name " + std::string(name));
				}
				uniforms[name] = location;
			}

			void ShaderProgram::createUniform(const char * name) {
				createUniform(std::string(name));
			}

			int ShaderProgram::getUniformLocation(const std::string& name) const {
				return uniforms.find(name)->second;
			}

			int ShaderProgram::getUniformLocation(const char * name) const {
				return uniforms.find(name)->second;
			}

			bool ShaderProgram::operator==(const ShaderProgram & other) const {
				return Object::operator==(other) && uniforms == other.uniforms&&shaders == other.shaders;
			}

			bool ShaderProgram::operator!=(const ShaderProgram & other) const {
				return !operator==(other);
			}

			//Autogenerated definitions.

			inline void ShaderProgram::setUniform(char * name, const mc::Matrix<float, 4, 4>& m) { setUniform(name, true, m); }

			inline void ShaderProgram::setUniform(char * name, const bool transpose, const mc::Matrix<float, 4, 4>& m) { float flattenedData[4 * 4]; glUniformMatrix4fv(uniforms[name], 1, transpose, m.flatten(flattenedData)); }

			inline void ShaderProgram::setUniform(char * name, const mc::Matrix<float, 3, 3>& m) { setUniform(name, true, m); }

			inline void ShaderProgram::setUniform(char * name, const bool transpose, const mc::Matrix<float, 3, 3>& m) { float flattenedData[3 * 3]; glUniformMatrix3fv(uniforms[name], 1, transpose, m.flatten(flattenedData)); }

			inline void ShaderProgram::setUniform(char * name, const mc::Matrix<float, 2, 2>& m) { setUniform(name, true, m); }

			//setUniform with float matrices

			inline void ShaderProgram::setUniform(char * name, const bool transpose, const mc::Matrix<float, 2, 2>& m) { float flattenedData[2 * 2]; glUniformMatrix2fv(uniforms[name], 1, transpose, m.flatten(flattenedData)); }
			inline void ShaderProgram::setUniform(char * name, const bool transpose, const mc::Matrix<float, 2, 3>& m) { float flattenedData[2 * 3]; glUniformMatrix2x3fv(uniforms[name], 1, transpose, m.flatten(flattenedData)); }
			inline void ShaderProgram::setUniform(char * name, const mc::Matrix<float, 2, 3>& m) { setUniform(name, true, m); }
			inline void ShaderProgram::setUniform(char * name, const bool transpose, const mc::Matrix<float, 3, 2>& m) { float flattenedData[3 * 2]; glUniformMatrix3x2fv(uniforms[name], 1, transpose, m.flatten(flattenedData)); }
			inline void ShaderProgram::setUniform(char * name, const mc::Matrix<float, 3, 2>& m) { setUniform(name, true, m); }
			inline void ShaderProgram::setUniform(char * name, const bool transpose, const mc::Matrix<float, 2, 4>& m) { float flattenedData[2 * 4]; glUniformMatrix2x4fv(uniforms[name], 1, transpose, m.flatten(flattenedData)); }
			inline void ShaderProgram::setUniform(char * name, const mc::Matrix<float, 2, 4>& m) { setUniform(name, true, m); }
			inline void ShaderProgram::setUniform(char * name, const bool transpose, const mc::Matrix<float, 3, 4>& m) { float flattenedData[3 * 4]; glUniformMatrix3x4fv(uniforms[name], 1, transpose, m.flatten(flattenedData)); }
			inline void ShaderProgram::setUniform(char * name, const mc::Matrix<float, 3, 4>& m) { setUniform(name, true, m); }
			inline void ShaderProgram::setUniform(char * name, const bool transpose, const mc::Matrix<float, 4, 3>& m) { float flattenedData[4 * 3]; glUniformMatrix4x3fv(uniforms[name], 1, transpose, m.flatten(flattenedData)); }
			inline void ShaderProgram::setUniform(char * name, const mc::Matrix<float, 4, 3>& m) { setUniform(name, true, m); }

			//setUniform with double matrices

			inline void ShaderProgram::setUniform(char * name, const bool transpose, const mc::Matrix<double, 2, 2>& m) { double flattenedData[2 * 2]; glUniformMatrix2dv(uniforms[name], 1, transpose, m.flatten(flattenedData)); }
			inline void ShaderProgram::setUniform(char * name, const mc::Matrix<double, 2, 2>& m) { setUniform(name, true, m); }
			inline void ShaderProgram::setUniform(char * name, const bool transpose, const mc::Matrix<double, 3, 3>& m) { double flattenedData[3 * 3]; glUniformMatrix3dv(uniforms[name], 1, transpose, m.flatten(flattenedData)); }
			inline void ShaderProgram::setUniform(char * name, const mc::Matrix<double, 3, 3>& m) { setUniform(name, true, m); }
			inline void ShaderProgram::setUniform(char * name, const bool transpose, const mc::Matrix<double, 4, 4>& m) { double flattenedData[4 * 4]; glUniformMatrix4dv(uniforms[name], 1, transpose, m.flatten(flattenedData)); }
			inline void ShaderProgram::setUniform(char * name, const mc::Matrix<double, 4, 4>& m) { setUniform(name, true, m); }
			inline void ShaderProgram::setUniform(char * name, const bool transpose, const mc::Matrix<double, 2, 3>& m) { double flattenedData[2 * 3]; glUniformMatrix2x3dv(uniforms[name], 1, transpose, m.flatten(flattenedData)); }
			inline void ShaderProgram::setUniform(char * name, const mc::Matrix<double, 2, 3>& m) { setUniform(name, true, m); }
			inline void ShaderProgram::setUniform(char * name, const bool transpose, const mc::Matrix<double, 3, 2>& m) { double flattenedData[3 * 2]; glUniformMatrix3x2dv(uniforms[name], 1, transpose, m.flatten(flattenedData)); }
			inline void ShaderProgram::setUniform(char * name, const mc::Matrix<double, 3, 2>& m) { setUniform(name, true, m); }
			inline void ShaderProgram::setUniform(char * name, const bool transpose, const mc::Matrix<double, 2, 4>& m) { double flattenedData[2 * 4]; glUniformMatrix2x4dv(uniforms[name], 1, transpose, m.flatten(flattenedData)); }
			inline void ShaderProgram::setUniform(char * name, const mc::Matrix<double, 2, 4>& m) { setUniform(name, true, m); }
			inline void ShaderProgram::setUniform(char * name, const bool transpose, const mc::Matrix<double, 3, 4>& m) { double flattenedData[3 * 4]; glUniformMatrix3x4dv(uniforms[name], 1, transpose, m.flatten(flattenedData)); }
			inline void ShaderProgram::setUniform(char * name, const mc::Matrix<double, 3, 4>& m) { setUniform(name, true, m); }
			inline void ShaderProgram::setUniform(char * name, const bool transpose, const mc::Matrix<double, 4, 3>& m) { double flattenedData[4 * 3]; glUniformMatrix4x3dv(uniforms[name], 1, transpose, m.flatten(flattenedData)); }
			inline void ShaderProgram::setUniform(char * name, const mc::Matrix<double, 4, 3>& m) { setUniform(name, true, m); }

			//setUniform with float

			inline void ShaderProgram::setUniform(char * name, const float a) { glUniform1f(uniforms[name], a); }
			inline void ShaderProgram::setUniform(char * name, const float a, const float b) { glUniform2f(uniforms[name], a, b); }
			inline void ShaderProgram::setUniform(char * name, const float a, const float b, const float c) { glUniform3f(uniforms[name], a, b, c); }
			inline void ShaderProgram::setUniform(char * name, const float a, const float b, const float c, const float d) { glUniform4f(uniforms[name], a, b, c, d); }
			inline void ShaderProgram::setUniform(char * name, const Size arraySize, const float * a) { glUniform1fv(uniforms[name], arraySize, a); }
			inline void ShaderProgram::setUniform(char * name, const Size componentSize, const Size arraySize, const float * a) { if( componentSize == 1 ) glUniform1fv(uniforms[name], arraySize, a); else if( componentSize == 2 ) glUniform2fv(uniforms[name], arraySize, a); else if( componentSize == 3 ) glUniform3fv(uniforms[name], arraySize, a); else if( componentSize == 4 ) glUniform4fv(uniforms[name], arraySize, a); }

			//setUniform with double

			inline void ShaderProgram::setUniform(char * name, const double a) { glUniform1d(uniforms[name], a); }
			inline void ShaderProgram::setUniform(char * name, const double a, const double b) { glUniform2d(uniforms[name], a, b); }
			inline void ShaderProgram::setUniform(char * name, const double a, const double b, const double c) { glUniform3d(uniforms[name], a, b, c); }
			inline void ShaderProgram::setUniform(char * name, const double a, const double b, const double c, const double d) { glUniform4d(uniforms[name], a, b, c, d); }
			inline void ShaderProgram::setUniform(char * name, const Size arraySize, const double * a) { glUniform1dv(uniforms[name], arraySize, a); }
			inline void ShaderProgram::setUniform(char * name, const Size componentSize, const Size arraySize, const double * a) { if( componentSize == 1 ) glUniform1dv(uniforms[name], arraySize, a); else if( componentSize == 2 ) glUniform2dv(uniforms[name], arraySize, a); else if( componentSize == 3 ) glUniform3dv(uniforms[name], arraySize, a); else if( componentSize == 4 ) glUniform4dv(uniforms[name], arraySize, a); }

			//setUniform with int

			inline void ShaderProgram::setUniform(char * name, const int a) { glUniform1i(uniforms[name], a); }
			inline void ShaderProgram::setUniform(char * name, const int a, const int b) { glUniform2i(uniforms[name], a, b); }
			inline void ShaderProgram::setUniform(char * name, const int a, const int b, const int c) { glUniform3i(uniforms[name], a, b, c); }
			inline void ShaderProgram::setUniform(char * name, const int a, const int b, const int c, const int d) { glUniform4i(uniforms[name], a, b, c, d); }
			inline void ShaderProgram::setUniform(char * name, const Size arraySize, const int * a) { glUniform1iv(uniforms[name], arraySize, a); }
			inline void ShaderProgram::setUniform(char * name, const Size componentSize, const Size arraySize, const int * a) { if( componentSize == 1 ) glUniform1iv(uniforms[name], arraySize, a); else if( componentSize == 2 ) glUniform2iv(uniforms[name], arraySize, a); else if( componentSize == 3 ) glUniform3iv(uniforms[name], arraySize, a); else if( componentSize == 4 ) glUniform4iv(uniforms[name], arraySize, a); }

			//setUniform with unsigned int

			inline void ShaderProgram::setUniform(char * name, const unsigned int a) { glUniform1ui(uniforms[name], a); }
			inline void ShaderProgram::setUniform(char * name, const unsigned int a, const unsigned int b) { glUniform2ui(uniforms[name], a, b); }
			inline void ShaderProgram::setUniform(char * name, const unsigned int a, const unsigned int b, const unsigned int c) { glUniform3ui(uniforms[name], a, b, c); }
			inline void ShaderProgram::setUniform(char * name, const unsigned int a, const unsigned int b, const unsigned int c, const unsigned int d) { glUniform4ui(uniforms[name], a, b, c, d); }
			inline void ShaderProgram::setUniform(char * name, const Size arraySize, const unsigned int * a) { glUniform1uiv(uniforms[name], arraySize, a); }
			inline void ShaderProgram::setUniform(char * name, const Size componentSize, const Size arraySize, const unsigned int * a) { if( componentSize == 1 ) glUniform1uiv(uniforms[name], arraySize, a); else if( componentSize == 2 ) glUniform2uiv(uniforms[name], arraySize, a); else if( componentSize == 3 ) glUniform3uiv(uniforms[name], arraySize, a); else if( componentSize == 4 ) glUniform4uiv(uniforms[name], arraySize, a); }
			inline void ShaderProgram::setUniform(char * name, const mc::Vector<unsigned int, 4> v) { glUniform4ui(uniforms[name], v[0], v[1], v[2], v[3]); }

			inline void ShaderProgram::setUniform(char * name, const mc::Vector<unsigned int, 3> v) { glUniform3ui(uniforms[name], v[0], v[1], v[2]); }
			inline void ShaderProgram::setUniform(char * name, const mc::Vector<unsigned int, 2> v) { glUniform2ui(uniforms[name], v[0], v[1]); }
			inline void ShaderProgram::setUniform(char * name, const mc::Vector<unsigned int, 1> v) { glUniform1ui(uniforms[name], v[0]); }
			inline void ShaderProgram::setUniform(char * name, const mc::Vector<int, 4> v) { glUniform4i(uniforms[name], v[0], v[1], v[2], v[3]); }
			inline void ShaderProgram::setUniform(char * name, const mc::Vector<int, 3> v) { glUniform3i(uniforms[name], v[0], v[1], v[2]); }
			inline void ShaderProgram::setUniform(char * name, const mc::Vector<int, 2> v) { glUniform2i(uniforms[name], v[0], v[1]); }
			inline void ShaderProgram::setUniform(char * name, const mc::Vector<int, 1> v) { glUniform1i(uniforms[name], v[0]); }
			inline void ShaderProgram::setUniform(char * name, const mc::Vector<double, 4> v) { glUniform4d(uniforms[name], v[0], v[1], v[2], v[3]); }
			inline void ShaderProgram::setUniform(char * name, const mc::Vector<double, 3> v) { glUniform3d(uniforms[name], v[0], v[1], v[2]); }
			inline void ShaderProgram::setUniform(char * name, const mc::Vector<double, 2> v) { glUniform2d(uniforms[name], v[0], v[1]); }
			inline void ShaderProgram::setUniform(char * name, const mc::Vector<double, 1> v) { glUniform1d(uniforms[name], v[0]); }
			inline void ShaderProgram::setUniform(char * name, const mc::Vector<float, 4> v) { glUniform4f(uniforms[name], v[0], v[1], v[2], v[3]); }
			inline void ShaderProgram::setUniform(char * name, const mc::Vector<float, 3> v) { glUniform3f(uniforms[name], v[0], v[1], v[2]); }
			inline void ShaderProgram::setUniform(char * name, const mc::Vector<float, 2> v) { glUniform2f(uniforms[name], v[0], v[1]); }
			inline void ShaderProgram::setUniform(char * name, const mc::Vector<float, 1> v) { glUniform1f(uniforms[name], v[0]); }

			void ShaderProgram::setShaders(const std::unordered_map<Enum, Shader>& newShaders) {
				shaders = newShaders;
			}

			const std::unordered_map<Enum, Shader>& ShaderProgram::getShaders() const {
				return shaders;
			}

			std::unordered_map<Enum, Shader>& ShaderProgram::getShaders() {
				return shaders;
			}

			void ShaderProgram::setUniforms(const std::unordered_map<std::string, int>& newUniforms) {
				uniforms = newUniforms;
			}

			const std::unordered_map<std::string, int>& ShaderProgram::getUniforms() const {
				return uniforms;
			}

			std::unordered_map<std::string, int>& ShaderProgram::getUniforms() {
				return uniforms;
			}
		}//ogl
	}//gfx
}//mc