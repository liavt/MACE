/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/OGL/OGL33.h>
#include <memory>
#include <string>

namespace mc {
	namespace gfx {
		namespace ogl33 {
			namespace {
				Shader createShader(const Enum type, const char* sources[], const GLsizei sourceSize) {
					Shader s = Shader(type);
					s.init();
					s.setSource(sourceSize, sources, nullptr);
					s.compile();
					return s;
				}

				void throwShaderError(const unsigned int shaderId, const Enum type, const std::string& message) {
#ifdef MACE_DEBUG_OPENGL
					std::unique_ptr<GLchar[]> log_string = std::unique_ptr<GLchar[]>(new char[1024]);
					glGetShaderInfoLog(shaderId, 1024, 0, log_string.get());
					std::string friendlyType = "UNKNOWN SHADER TYPE " + std::to_string(type);//a more human friendly name for type, like VERTEX_SHADER instead of 335030
					if (type == GL_VERTEX_SHADER) {
						friendlyType = "VERTEX SHADER";
					} else if (type == GL_FRAGMENT_SHADER) {
						friendlyType = "FRAGMENT SHADER";
					} else if (type == GL_GEOMETRY_SHADER) {
						friendlyType = "GEOMETERY SHADER";
					} else if (type == GL_CURRENT_PROGRAM) {
						friendlyType = "SHADER PROGRAM";
						glGetProgramInfoLog(shaderId, 1024, 0, log_string.get());
					}
					MACE__THROW(Shader, "Error generating " + friendlyType + ": " + message + ": " + log_string.get());
#else
					MACE__THROW(Shader, "Error generating shader of type " + std::to_string(type));
#endif//MACE_DEBUG_OPENGL
				}
			}//anon namespace

			void forceCheckGLError(const unsigned int line, const char* file, const char* message) {
				std::vector<Error> errors;

				Enum result = GL_NO_ERROR;
				//have to use glad_glGetError instead of glGetError to prevent stack overflow
				//see GLAD docs about the post- and pre- callbacks
				while ((result = glad_glGetError()) != GL_NO_ERROR) {
					switch (result) {
					case GL_INVALID_ENUM:
						errors.push_back(MACE__GET_ERROR_NAME(OpenGL) (std::string(message) + ": GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument", line, file));
						break;
					case GL_INVALID_VALUE:
						errors.push_back(MACE__GET_ERROR_NAME(OpenGL) (std::string(message) + ": GL_INVALID_VALUE: A numeric argument is out of range", line, file));
						break;
					case GL_INVALID_OPERATION:
						errors.push_back(MACE__GET_ERROR_NAME(OpenGL) (std::string(message) + ": GL_INVALID_OPERATION: The specified operation is not allowed in the current state", line, file));
						break;
					case GL_INVALID_FRAMEBUFFER_OPERATION:
						errors.push_back(MACE__GET_ERROR_NAME(OpenGL) (std::string(message) + ": GL_INVALID_FRAMEBUFFER_OPERATION: The command is trying to render to or read from the framebuffer while the currently bound framebuffer is not framebuffer complete (i.e. the return value from glCheckFramebufferStatus is not GL_FRAMEBUFFER_COMPLETE)", line, file));
						break;
					case GL_OUT_OF_MEMORY:
						errors.push_back(MACE__GET_ERROR_NAME(OutOfMemory) (std::string(message) + ": GL_OUT_OF_MEMORY: There is not enough memory left to execute the command", line, file));
						break;

					default:
						errors.push_back(MACE__GET_ERROR_NAME(OpenGL) (std::string(message) + ": OpenGL has errored with an error code of " + std::to_string(result), line, file));
						break;
					}
				}

				if (errors.size() == 1) {
					throw errors[0];
				} else if (!errors.empty()) {
					throw MultipleErrors(errors.data(), errors.size(), line, file);
				}
			}

			void VertexArray::destroy() {
				Object::destroy();

				if (indices.isCreated()) {
					indices.destroy();
				}

				for (Index i = 0; i < buffers.size(); ++i) {
					if (buffers[i].isCreated()) {
						buffers[i].destroy();
					}
				}
			}

			bool VertexArray::isCreated() const {
				return glIsVertexArray(id) == 1;
			}

			void VertexArray::loadVertices(const unsigned int verticeSize, const float* vertices, const GLuint location, const Byte attributeSize, const Enum type, const bool normalized) {
				vertexNumber = verticeSize;

				storeDataInAttributeList(vertexNumber * sizeof(float), vertices, location, attributeSize, type, normalized);
			}

			void VertexArray::storeDataInAttributeList(const unsigned int dataSize, const GLvoid* data, const GLuint location, const Byte attributeSize, const Enum type, const bool normalized) {
				bind();

				VertexBuffer buffer = VertexBuffer();
				buffer.init();
				buffer.bind();
				buffer.setLocation(location);
				// Give our data to opengl
				buffer.createStorage(static_cast<ptrdiff_t>(attributeSize) * static_cast<ptrdiff_t>(dataSize), data, GL_DYNAMIC_STORAGE_BIT);
				buffer.setAttributePointer(attributeSize, type, normalized, 0, 0);

				addBuffer(buffer);
			}

			void VertexArray::loadIndices(const unsigned int indiceNum, const unsigned int* indiceData) {
				indices = ElementBuffer(indiceNum);
				indices.init();
				indices.bind();
				indices.createStorage(static_cast<ptrdiff_t>(sizeof(unsigned int) * indiceNum), indiceData, 0);
			}

			void VertexArray::addBuffer(const VertexBuffer& newBuffer) {
				bind();
				newBuffer.bind();
				newBuffer.enable();

				buffers.push_back(newBuffer);
			}

			void VertexArray::setVertexNumber(const GLsizei vertexNum) {
				vertexNumber = vertexNum;
			}

			GLsizei VertexArray::getVertexNumber() {
				return vertexNumber;
			}

			const GLsizei VertexArray::getVertexNumber() const {
				return vertexNumber;
			}

			void VertexArray::setIndices(const ElementBuffer& buffer) {
				indices = buffer;
			}

			ElementBuffer& VertexArray::getIndices() {
				return indices;
			}

			const ElementBuffer& VertexArray::getIndices() const {
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

			bool VertexArray::operator==(const VertexArray& other) const {
				return vertexNumber == other.vertexNumber && indices == other.indices && Object::operator==(other);
			}

			bool VertexArray::operator!=(const VertexArray& other) const {
				return !operator==(other);
			}

			void VertexArray::bindIndex(const GLuint ID) const {
				glBindVertexArray(ID);
			}

			void VertexArray::initIndices(GLuint ids[], const GLsizei length) const {
				if (GLAD_GL_ARB_direct_state_access) {
					glCreateVertexArrays(length, ids);
				} else {
					glGenVertexArrays(length, ids);
				}
			}

			void VertexArray::destroyIndices(const GLuint ids[], const GLsizei length) const {
				glDeleteVertexArrays(length, ids);
			}

			UniformBuffer::UniformBuffer() noexcept : UniformBuffer("") {}

			UniformBuffer::UniformBuffer(const char* n) noexcept : Buffer(GL_UNIFORM_BUFFER), name(n) {}

			const char* UniformBuffer::getName() const {
				return name;
			}

			void UniformBuffer::setName(const char* na) {
				this->name = na;
			}

			bool UniformBuffer::operator==(const UniformBuffer& other) const {
				return name == other.name && Buffer::operator==(other);
			}

			bool UniformBuffer::operator!=(const UniformBuffer& other) const {
				return !(*this == other);
			}

			void FrameBuffer::setClearColor(const float r, const float g, const float b, const float a) {
				glClearColor(r, g, b, a);
			}

			void FrameBuffer::attachTexture(const Enum target, const Enum attachment, const Texture2D& tex, const int level) {
				glFramebufferTexture(target, attachment, tex.getID(), level);
			}

			void FrameBuffer::attachTexture1D(const Enum target, const Enum attachment, const Texture2D& tex, const int level) {
				glFramebufferTexture1D(target, attachment, tex.getTarget(), tex.getID(), level);
			}

			void FrameBuffer::attachTexture2D(const Enum target, const Enum attachment, const Texture2D& tex, const int level) {
				glFramebufferTexture2D(target, attachment, tex.getTarget(), tex.getID(), level);
			}

			void FrameBuffer::attachTexture3D(const Enum target, const Enum attachment, const Texture2D& tex, const int level, const int layer) {
				glFramebufferTexture3D(target, attachment, tex.getTarget(), tex.getID(), level, layer);
			}

			void FrameBuffer::attachTextureLayer(const Enum target, const Enum attachment, const Texture2D& texture, const int level, const int layer) {
				glFramebufferTextureLayer(target, attachment, level, static_cast<GLint>(texture.getID()), layer);
			}

			void FrameBuffer::attachRenderbuffer(const Enum target, const Enum attachment, const RenderBuffer& buffer) {
				glFramebufferRenderbuffer(target, attachment, GL_RENDERBUFFER, buffer.getID());
			}

			void FrameBuffer::setDrawBuffers(const Size arrSize, const Enum* buffers) {
				glDrawBuffers(static_cast<GLsizei>(arrSize), buffers);
			}

			void FrameBuffer::setReadBuffer(const Enum mode) {
				glReadBuffer(mode);
			}

			void FrameBuffer::setDrawBuffer(const Enum mode) {
				glDrawBuffer(mode);
			}

			FrameBuffer FrameBuffer::getDefaultFramebuffer() {
				FrameBuffer def = FrameBuffer();
				def.id = 0;
				return def;
			}

			void FrameBuffer::clear(const unsigned int field) {
				glClear(field);
			}

			void FrameBuffer::readPixels(const int x, const int y, const Size width, const Size height, const Enum format, const Enum type, void* data) const {
				glReadPixels(x, y, static_cast<GLsizei>(width), static_cast<GLsizei>(height), format, type, data);
			}

			void FrameBuffer::setPixelStorage(const Enum name, const float param) {
				glPixelStoref(name, param);
			}

			void FrameBuffer::setPixelStorage(const Enum name, const int param) {
				glPixelStorei(name, param);
			}

			bool FrameBuffer::isCreated() const {
				return glIsFramebuffer(id) == 1;
			}

			Enum FrameBuffer::checkStatus(const Enum target) {
				return glCheckFramebufferStatus(target);
			}

			void FrameBuffer::bindIndex(const GLuint ID) const {
				glBindFramebuffer(GL_FRAMEBUFFER, ID);
			}

			void FrameBuffer::initIndices(GLuint ids[], const GLsizei length) const {
				if (GLAD_GL_ARB_direct_state_access) {
					glCreateFramebuffers(length, ids);
				} else {
					glGenFramebuffers(length, ids);
				}
			}

			void FrameBuffer::destroyIndices(const GLuint ids[], const GLsizei length) const {
				glDeleteFramebuffers(length, ids);
			}

			void RenderBuffer::setStorage(const Enum format, const GLsizei width, const GLsizei height) {
				if (GLAD_GL_ARB_direct_state_access) {
					glNamedRenderbufferStorage(id, format, width, height);
				} else if (GLAD_GL_EXT_direct_state_access) {
					glNamedRenderbufferStorageEXT(id, format, width, height);
				} else {
					bind();

					glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
				}
			}

			void RenderBuffer::setStorageMultisampled(const GLsizei samples, const Enum format, const GLsizei width, const GLsizei height) {
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
			}

			bool RenderBuffer::isCreated() const {
				return glIsRenderbuffer(id) == 1;
			}

			void RenderBuffer::bindIndex(const GLuint ID) const {
				glBindRenderbuffer(GL_RENDERBUFFER, ID);
			}

			void RenderBuffer::initIndices(GLuint ids[], const GLsizei length) const {
				if (GLAD_GL_ARB_direct_state_access) {
					glCreateRenderbuffers(length, ids);
				} else {
					glGenRenderbuffers(length, ids);
				}
			}

			void RenderBuffer::destroyIndices(const GLuint ids[], const GLsizei length) const {
				glDeleteRenderbuffers(length, ids);
			}

			void Object::init() {
				initIndices(&id, 1);
			}

			void Object::destroy() {
				destroyIndices(&id, 1);
				id = 0;
			}

			void Object::bind() const {
				bindIndex(id);
			}

			void Object::unbind() const {
				bindIndex(0);
			}

			GLuint Object::getID() const {
				return id;
			}

			void Object::init(Object* objects[], const GLsizei length) {
				if (length <= 0) {
					return;
				}

				std::vector<GLuint> ids = std::vector<GLuint>(length);
				for (Index i = 0; i < length; ++i) {
					ids.push_back(0);
				}
				objects[0]->initIndices(ids.data(), length);

				for (Index i = 0; i < length; ++i) {
					objects[i]->id = ids[i];
				}
			}

			void Object::destroy(Object* objects[], const GLsizei length) {
				if (length <= 0) {
					return;
				}

				std::vector<GLuint> ids = std::vector<GLuint>(length);
				for (Index i = 0; i < length; ++i) {
					ids.push_back(objects[i]->id);
				}
				objects[0]->destroyIndices(ids.data(), length);

				for (Index i = 0; i < length; ++i) {
					objects[i]->id = 0;
				}
			}

			bool Object::operator==(const Object& other) const {
				return this->id == other.id;
			}

			bool Object::operator!=(const Object& other) const {
				return !operator==(other);
			}

			Texture2D::Texture2D() noexcept {}

			void Texture2D::bind() const {
				Object::bind();
			}

			void Texture2D::bind(const unsigned int location) const {
				if (GLAD_GL_ARB_direct_state_access) {
					glBindTextureUnit(location, id);
				} else {
					glActiveTexture(GL_TEXTURE0 + location);
					Object::bind();
				}
			}

			void Texture2D::createStorage(GLsizei w, GLsizei h, Enum type, Enum format, Enum internalFormat, GLint mipmapLevels) {
				if (GLAD_GL_ARB_texture_storage) {
					if (GLAD_GL_ARB_direct_state_access) {
						glTextureStorage2D(id, mipmapLevels, internalFormat, w, h);
					} else if (GLAD_GL_EXT_direct_state_access) {
						glTextureStorage2DEXT(id, target, mipmapLevels, internalFormat, w, h);
					} else {
						bind();

						glTexStorage2D(target, mipmapLevels, internalFormat, w, h);
					}
				} else {
					bind();

					GLsizei width = w, height = h;
					for (GLint i = 0; i < mipmapLevels; ++i) {
						glTexImage2D(target, i, internalFormat, width, height, 0, format, type, nullptr);
						width = math::max(1, (width >> 1));
						height = math::max(1, (height >> 1));
					}
				}
			}

			void Texture2D::setSubdata(const void* data, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, Enum type, Enum format, GLint mipmapLevel) {
				if (GLAD_GL_ARB_direct_state_access) {
					glTextureSubImage2D(id, mipmapLevel, xoffset, yoffset, width, height, format, type, data);
				} else if (GLAD_GL_EXT_direct_state_access) {
					glTextureSubImage2DEXT(id, target, mipmapLevel, xoffset, yoffset, width, height, format, type, data);
				} else {
					bind();
					glTexSubImage2D(target, mipmapLevel, xoffset, yoffset, width, height, format, type, data);
				}
			}

			void Texture2D::setPixelStorage(const Enum alignment, const int number) const {
				glPixelStorei(alignment, number);
			}

			void Texture2D::setPixelStorage(const Enum alignment, const bool value) const {
				setPixelStorage(alignment, value ? 1 : 0);
			}

			void Texture2D::setPixelStorage(const Enum alignment, const float number) const {
				glPixelStoref(alignment, number);
			}

			void Texture2D::generateMipmap() {
				if (GLAD_GL_ARB_direct_state_access) {
					glGenerateTextureMipmap(id);
				} else if (GLAD_GL_EXT_direct_state_access) {
					glGenerateTextureMipmapEXT(id, target);
				} else {
					bind();
					glGenerateMipmap(target);
				}
			}

			void Texture2D::setTarget(Enum targ) {
				this->target = targ;
			}

			Enum& Texture2D::getTarget() {
				return target;
			}

			const Enum& Texture2D::getTarget() const {
				return target;
			}

			bool Texture2D::isCreated() const {
				return glIsTexture(id) == 1;
			}

			void Texture2D::setParameter(const Enum name, const int value) {
				glTexParameteri(target, name, value);
			}

			void Texture2D::getImage(const Enum format, const Enum type, void* data) const {
				bind();

				glGetTexImage(target, 0, format, type, data);
			}


			bool Texture2D::operator==(const Texture2D& other) const {
				return target == other.target && Object::operator==(other);
			}

			bool Texture2D::operator!=(const Texture2D& other) const {
				return !operator==(other);
			}

			void Texture2D::bindIndex(const GLuint ID) const {
				glBindTexture(target, ID);
			}

			void Texture2D::initIndices(GLuint ids[], const GLsizei length) const {
				if (GLAD_GL_ARB_direct_state_access) {
					glCreateTextures(target, length, ids);
				} else {
					glGenTextures(length, ids);
				}
			}

			void Texture2D::destroyIndices(const GLuint ids[], const GLsizei length) const {
				glDeleteTextures(length, ids);
			}

			Buffer::Buffer(const Enum type) noexcept : bufferType(type) {}

			bool Buffer::isCreated() const {
				return glIsBuffer(id) == 1;
			}

			void Buffer::createStorage(const GLsizeiptr size, const void* data, const GLbitfield flags) {
				if (GLAD_GL_ARB_buffer_storage) {
					if (GLAD_GL_ARB_direct_state_access) {
						glNamedBufferStorage(id, size, data, flags);
					} else if (GLAD_GL_EXT_direct_state_access) {
						glNamedBufferStorageEXT(id, size, data, flags);
					} else {
						bind();

						glBufferStorage(bufferType, size, data, flags);
					}
				} else {
					bind();

					glBufferData(bufferType, size, data, GL_DYNAMIC_DRAW);
				}
			}

			void Buffer::setDataRange(const Index offset, const ptrdiff_t& dataSize, const void* data) {
				if (GLAD_GL_ARB_direct_state_access) {
					glNamedBufferSubData(id, offset, dataSize, data);
				} else if (GLAD_GL_EXT_direct_state_access) {
					glNamedBufferSubDataEXT(id, offset, dataSize, data);
				} else {
					bind();

					glBufferSubData(bufferType, offset, dataSize, data);
				}
			}

			void Buffer::copyData(Buffer& other, const ptrdiff_t& size, const Index readOffset, const Index writeOffset) {
				glCopyBufferSubData(id, other.id, readOffset, writeOffset, size);
			}

			void* Buffer::map(const Enum access) {
				return glMapBuffer(bufferType, access);
			}

			void* Buffer::mapRange(const Index offset, const Size length, const unsigned int access) {
				return glMapBufferRange(bufferType, offset, length, access);
			}

			bool Buffer::unmap() {
				return glUnmapBuffer(bufferType) == 1;
			}

			void Buffer::flushRange(const Index offset, const Size length) {
				glFlushMappedBufferRange(bufferType, offset, length);
			}

			void Buffer::getPointer(void** param) {
				glGetBufferPointerv(bufferType, GL_BUFFER_MAP_POINTER, param);
			}

			void Buffer::getParameter(const Enum pname, int* data) const {
				glGetBufferParameteriv(bufferType, pname, data);
			}

			void Buffer::getParameter(const Enum pname, GLint64* data) const {
				glGetBufferParameteri64v(bufferType, pname, data);
			}

			bool Buffer::isMapped() const {
				int out;
				getParameter(GL_BUFFER_MAPPED, &out);
				return out != 0;
			}

			GLint64 Buffer::getMapLength() const {
				GLint64 out;
				getParameter(GL_BUFFER_MAP_LENGTH, &out);
				return out;
			}

			GLint64 Buffer::getMapOffset() const {
				GLint64 out;
				getParameter(GL_BUFFER_MAP_OFFSET, &out);
				return out;
			}

			const Enum Buffer::getBufferType() const {
				return bufferType;
			}

			int Buffer::getSize() const {
				int out;
				getParameter(GL_BUFFER_SIZE, &out);
				return out;
			}

			bool Buffer::operator==(const Buffer& other) const {
				return this->bufferType == other.bufferType && Object::operator==(other);
			}

			bool Buffer::operator!=(const Buffer& other) const {
				return !operator==(other);
			}

			void Buffer::bindIndex(const GLuint ID) const {
				glBindBuffer(bufferType, ID);
			}

			void Buffer::initIndices(GLuint ids[], const GLsizei length) const {
				if (GLAD_GL_ARB_direct_state_access) {
					glCreateBuffers(length, ids);
				} else {
					glGenBuffers(length, ids);
				}
			}

			void Buffer::destroyIndices(const GLuint ids[], const GLsizei length) const {
				glDeleteBuffers(length, ids);
			}

			VertexBuffer::VertexBuffer() noexcept : Buffer(GL_ARRAY_BUFFER) {}

			void VertexBuffer::setAttributePointer(const GLint attribSize, const Enum type, const bool normalized, const GLsizei stride, const void* pointer) {
				if (!normalized && (
					type == GL_BYTE ||
					type == GL_UNSIGNED_BYTE ||
					type == GL_SHORT ||
					type == GL_UNSIGNED_SHORT ||
					type == GL_INT ||
					type == GL_UNSIGNED_INT
					)) {

					glVertexAttribIPointer(location, attribSize, type, stride, pointer);
				} else {
					glVertexAttribPointer(location, attribSize, type, normalized, stride, pointer);
				}
			}

			void VertexBuffer::setDivisor(const GLuint divisor) {
				glVertexAttribDivisor(location, divisor);
			}

			void VertexBuffer::enable() const {
				glEnableVertexAttribArray(location);
			}

			void VertexBuffer::disable() const {
				glDisableVertexAttribArray(location);
			}

			GLuint VertexBuffer::getLocation() {
				return location;
			}

			const GLuint VertexBuffer::getLocation() const {
				return location;
			}

			void VertexBuffer::setLocation(const GLuint newLocation) {
				location = newLocation;
			}

			bool VertexBuffer::operator==(const VertexBuffer& other) const {
				return Buffer::operator==(other) && location == other.location;
			}

			bool VertexBuffer::operator!=(const VertexBuffer& other) const {
				return !operator==(other);
			}

			ElementBuffer::ElementBuffer() noexcept : ElementBuffer(0) {}

			ElementBuffer::ElementBuffer(const int indiceNum) noexcept : Buffer(GL_ELEMENT_ARRAY_BUFFER), indiceNumber(indiceNum) {}

			void ElementBuffer::setIndiceNumber(const int indices) {
				indiceNumber = indices;
			}

			int ElementBuffer::getIndiceNumber() {
				return indiceNumber;
			}

			const int ElementBuffer::getIndiceNumber() const {
				return indiceNumber;
			}

			bool ElementBuffer::operator==(const ElementBuffer& other) const {
				return Buffer::operator==(other) && indiceNumber == other.indiceNumber;
			}

			bool ElementBuffer::operator!=(const ElementBuffer& other) const {
				return !operator==(other);
			}

			Shader::Shader() noexcept : Shader(GL_FALSE) {}

			Shader::Shader(const Enum shaderType) noexcept : type(shaderType) {}

			void Shader::init() {
				if (type == GL_FALSE) {
					MACE__THROW(InitializationFailed, "Must assign a type to the shader before init() is called!");
				}
				id = glCreateShader(type);
			}

			void Shader::destroy() {
				glDeleteShader(id);
			}

			void Shader::setSource(const GLsizei count, const char* strings[], const int lengths[]) {
				if (type == GL_FALSE) {
					MACE__THROW(Shader, "Shader must have a type before compile() is called");
				}

				glShaderSource(id, count, strings, lengths);
			}

			void Shader::setSource(const char string[], const int length) {
				const int lengths[1] = {length};
				const char* strings[1] = {string};
				setSource(1, strings, lengths);
			}

			void Shader::setSource(const std::string& string) {
				//length() returns size_t which could be larger than unsigned in on some systems, causing problems. static_cast will fix it
				setSource(string.c_str(), static_cast<int>(string.length()));
			}

			char* Shader::getSource(const GLsizei length, char* characters, int amount) const {
				glGetShaderSource(id, length, &amount, characters);
				return characters;
			}

			int Shader::getParameter(const Enum param) const {
				int result;
				glGetShaderiv(id, param, &result);
				return result;
			}

			int Shader::getInfoLogLength() const {
				return getParameter(GL_INFO_LOG_LENGTH);
			}

			int Shader::getSourceLength() const {
				return getParameter(GL_SHADER_SOURCE_LENGTH);
			}

			bool Shader::isDeleted() const {
				return getParameter(GL_DELETE_STATUS) == GL_TRUE;
			}

			bool Shader::isCompiled() const {
				return getParameter(GL_COMPILE_STATUS) == GL_TRUE;
			}


			void Shader::compile() {
				if (type == GL_FALSE) {
					MACE__THROW(Shader, "Shader must have a type before compile() is called");
				}
				glCompileShader(id);

				if (!isCompiled()) {
					throwShaderError(id, type, "The shader failed to compile");
				}
			}

			bool Shader::isCreated() const {
				return glIsShader(id) == 1;
			}

			void Shader::setType(const Enum newType) {
				type = newType;
			}

			Enum Shader::getType() {
				return type;
			}

			const Enum Shader::getType() const {
				return type;
			}

			bool Shader::operator==(const Shader& other) const {
				return Object::operator==(other) && type == other.type;
			}

			bool Shader::operator!=(const Shader& other) const {
				return !operator==(other);
			}

			void Shader::bind() const {}
			void Shader::unbind() const {}
			void Shader::bindIndex(const GLuint) const {}

			void Shader::initIndices(GLuint[], const GLsizei) const {}

			void Shader::destroyIndices(const GLuint[], const GLsizei) const {}

			void ShaderProgram::bindIndex(const GLuint ID) const {
				glUseProgram(ID);
			}

			void ShaderProgram::initIndices(GLuint[], const GLsizei) const {}
			void ShaderProgram::destroyIndices(const GLuint[], const GLsizei) const {}

			void ShaderProgram::init() {
				id = glCreateProgram();

				if (id == 0) {
					throwShaderError(id, GL_CURRENT_PROGRAM, "Failed to create program ID");
				}
			}
			void ShaderProgram::destroy() {
				if (id > 0) {
					unbind();
					for (auto s : shaders) {
						if (s.second.isCreated()) {
							s.second.destroy();
						}
					}
					glDeleteProgram(id);
				}
			}
			void ShaderProgram::link() {
				glLinkProgram(id);

				if (!isLinked()) {
					throwShaderError(id, GL_CURRENT_PROGRAM, "The shader program was unable to link");
				}

				checkGLError(__LINE__, __FILE__, "Error linking shader program");

				validate();

				if (!isValidated()) {
					throwShaderError(id, GL_CURRENT_PROGRAM, "The shader program failed to validate");
				}

				checkGLError(__LINE__, __FILE__, "Error validating shader program");

				for (auto s : shaders) {
					if (s.second.isCreated()) {
						detachShader(s.second);
					}
				}

				checkGLError(__LINE__, __FILE__, "Error detaching shader program");
			}
			bool ShaderProgram::isCreated() const {
				return glIsProgram(id) == GL_TRUE;
			}
			int ShaderProgram::getParameter(const Enum param) const {
				int result;
				glGetProgramiv(id, param, &result);
				return result;
			}
			int ShaderProgram::getInfoLogLength() const {
				return getParameter(GL_INFO_LOG_LENGTH);
			}
			int ShaderProgram::getAttachedShaders() const {
				return getParameter(GL_ATTACHED_SHADERS);
			}
			bool ShaderProgram::isDeleted() const {
				//some compilers emit warnings saying conversion from int to bool, making it explicit silences those
				return getParameter(GL_DELETE_STATUS) == GL_TRUE;
			}
			bool ShaderProgram::isLinked() const {
				return getParameter(GL_LINK_STATUS) == GL_TRUE;
			}
			bool ShaderProgram::isValidated() const {
				return getParameter(GL_VALIDATE_STATUS) == GL_TRUE;
			}
			void ShaderProgram::detachShader(const GLuint shaderId) {
				glDetachShader(id, shaderId);
			}

			void ShaderProgram::detachShader(const Shader& sh) {
				detachShader(sh.getID());
			}

			void ShaderProgram::validate() {
				glValidateProgram(id);
			}

			void ShaderProgram::attachShader(const Shader shader) {
				glAttachShader(id, shader.getID());
				shaders.insert(std::pair<Enum, Shader>(shader.getType(), shader));
			}

			void ShaderProgram::createFragment(const char shader[]) {
				createFragment(1, &shader);
			}
			void ShaderProgram::createFragment(const GLsizei count, const char* strings[]) {
				attachShader(createShader(GL_FRAGMENT_SHADER, strings, count));
			}
			void ShaderProgram::createVertex(const char shader[]) {
				createVertex(1, &shader);
			}

			void ShaderProgram::createVertex(const GLsizei count, const char* strings[]) {
				attachShader(createShader(GL_VERTEX_SHADER, strings, count));
			}

			void ShaderProgram::createUniform(const std::string& name) {
				int location = glGetUniformLocation(id, name.data());
				if (location < 0) {
					MACE__THROW(Shader, "Error finding uniform with name " + std::string(name));
				}

				uniforms[name] = location;
			}

			void ShaderProgram::createUniform(const char* name) {
				createUniform(std::string(name));
			}

			void ShaderProgram::createUniformBuffer(const char* name, const GLint location) {
				UniformBufferData out = UniformBufferData();

				if (location >= 0) {
					glUniformBlockBinding(id, glGetUniformBlockIndex(id, name), location);
					out.index = location;
				} else {
					out.index = glGetUniformBlockIndex(id, name);
				}

				glGetActiveUniformBlockiv(id, out.index, GL_UNIFORM_BLOCK_DATA_SIZE, &out.size);

				GLint activeUniforms = 0;
				glGetActiveUniformBlockiv(id, out.index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &activeUniforms);

				std::vector<GLuint> indices(activeUniforms);
				{
					std::vector<GLint> tempIndices(activeUniforms);
					glGetActiveUniformBlockiv(id, out.index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, &tempIndices[0]);
#pragma omp parallel for
					for (Index i = 0; i < activeUniforms; ++i) {
						indices[i] = static_cast<GLuint>(tempIndices[i]);
					}
				}

				std::vector<GLint> nameLengths(activeUniforms);
				std::vector<GLint> sizes(activeUniforms);
				std::vector<GLint> offsets(activeUniforms);
				std::vector<GLint> types(activeUniforms);

				glGetActiveUniformsiv(id, activeUniforms, &indices[0], GL_UNIFORM_NAME_LENGTH, &nameLengths[0]);
				glGetActiveUniformsiv(id, activeUniforms, &indices[0], GL_UNIFORM_SIZE, &sizes[0]);
				glGetActiveUniformsiv(id, activeUniforms, &indices[0], GL_UNIFORM_OFFSET, &offsets[0]);
				glGetActiveUniformsiv(id, activeUniforms, &indices[0], GL_UNIFORM_TYPE, &types[0]);

				for (Index i = 0; i < activeUniforms; ++i) {
					std::string bufferName(static_cast<Size>(nameLengths[i]), '_');

					glGetActiveUniformName(id, out.index, nameLengths[i], nullptr, &bufferName[0]);

					UniformBufferData::Field field{};
					field.index = indices[i];
					field.size = sizes[i];
					field.offset = offsets[i];
					field.type = types[i];
					out.fields[bufferName] = field;
				}

				uniformBuffers[name] = out;
			}

			void ShaderProgram::createUniformBuffer(const UniformBuffer& buf, const GLint location) {
				createUniformBuffer(buf.getName(), location);
			}

			ShaderProgram::UniformBufferData& ShaderProgram::getUniformBuffer(const std::string name) {
				return uniformBuffers[name];
			}

			ShaderProgram::UniformBufferData& ShaderProgram::getUniformBuffer(const UniformBuffer& buf) {
				return getUniformBuffer(std::string(buf.getName()));
			}

			void ShaderProgram::setUniformBufferField(UniformBuffer& buf, const std::string name, const void* data, const ptrdiff_t size) {
				const UniformBufferData& bufferData = getUniformBuffer(buf);
				buf.setDataRange(bufferData.fields.at(name).offset, size, data);
			}

			void ShaderProgram::bindUniformBuffer(const UniformBuffer& buf) {
				const UniformBuffer* buffer[] = {
					&buf
				};

				bindUniformBuffers(buffer, 1);
			}

			void ShaderProgram::bindUniformBuffers(const UniformBuffer* bufs[], const Size size) {
				if (false && GLAD_GL_ARB_multi_bind) {
					std::vector<GLuint> ids = std::vector<GLuint>(size);
					for (Index i = 0; i < size; ++i) {
						ids.push_back(bufs[i]->getID());
					}

					glBindBuffersBase(GL_UNIFORM_BUFFER, 0, size, ids.data());
				} else {
					for (Index i = 0; i < size; ++i) {
						const UniformBufferData& bufferData = getUniformBuffer(*bufs[i]);
						glBindBufferBase(GL_UNIFORM_BUFFER, bufferData.index, bufs[i]->getID());
					}
				}
			}

			int ShaderProgram::getUniformLocation(const std::string& name) const {
				return uniforms.find(name)->second;
			}

			int ShaderProgram::getUniformLocation(const char* name) const {
				return uniforms.find(name)->second;
			}

			/*
			void ShaderProgram::getUniformIndices(const char * indices[], const Size count) {
				std::vector<unsigned int> counts = std::vector<unsigned int>();
				counts.reserve(count);
				glGetUniformIndices(id, count, indices, counts.data());

				for (Index i = 0; i < count; ++i) {
					if (counts[i] != GL_INVALID_INDEX) {
						uniforms[indices[i]] = counts[i];
					}
				}
			}
			*/

			bool ShaderProgram::operator==(const ShaderProgram& other) const {
				return Object::operator==(other) && uniforms == other.uniforms && shaders == other.shaders;
			}

			bool ShaderProgram::operator!=(const ShaderProgram& other) const {
				return !operator==(other);
			}

			//Autogenerated definitions.

			void ShaderProgram::setUniform(const char* name, const mc::Matrix<float, 4, 4> & m) {
				setUniform(name, true, m);
			}

			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<float, 4, 4> & m) {
				float flattenedData[4 * 4]; glUniformMatrix4fv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}

			void ShaderProgram::setUniform(const char* name, const mc::Matrix<float, 3, 3> & m) {
				setUniform(name, true, m);
			}

			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<float, 3, 3> & m) {
				float flattenedData[3 * 3]; glUniformMatrix3fv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}

			void ShaderProgram::setUniform(const char* name, const mc::Matrix<float, 2, 2> & m) {
				setUniform(name, true, m);
			}

			//setUniform with float matrices

			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<float, 2, 2> & m) {
				float flattenedData[2 * 2]; glUniformMatrix2fv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<float, 2, 3> & m) {
				float flattenedData[2 * 3]; glUniformMatrix2x3fv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<float, 2, 3> & m) {
				setUniform(name, true, m);
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<float, 3, 2> & m) {
				float flattenedData[3 * 2]; glUniformMatrix3x2fv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<float, 3, 2> & m) {
				setUniform(name, true, m);
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<float, 2, 4> & m) {
				float flattenedData[2 * 4]; glUniformMatrix2x4fv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<float, 2, 4> & m) {
				setUniform(name, true, m);
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<float, 3, 4> & m) {
				float flattenedData[3 * 4]; glUniformMatrix3x4fv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<float, 3, 4> & m) {
				setUniform(name, true, m);
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<float, 4, 3> & m) {
				float flattenedData[4 * 3]; glUniformMatrix4x3fv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<float, 4, 3> & m) {
				setUniform(name, true, m);
			}

			//setUniform with float

			void ShaderProgram::setUniform(const char* name, const float a) {
				glUniform1f(uniforms[name], a);
			}
			void ShaderProgram::setUniform(const char* name, const float a, const float b) {
				glUniform2f(uniforms[name], a, b);
			}
			void ShaderProgram::setUniform(const char* name, const float a, const float b, const float c) {
				glUniform3f(uniforms[name], a, b, c);
			}
			void ShaderProgram::setUniform(const char* name, const float a, const float b, const float c, const float d) {
				glUniform4f(uniforms[name], a, b, c, d);
			}
			void ShaderProgram::setUniform(const char* name, const GLsizei arraySize, const float* a) {
				glUniform1fv(uniforms[name], arraySize, a);
			}
			void ShaderProgram::setUniform(const char* name, const GLsizei componentSize, const GLsizei arraySize, const float* a) {
				if (componentSize == 1) glUniform1fv(uniforms[name], arraySize, a); else if (componentSize == 2) glUniform2fv(uniforms[name], arraySize, a); else if (componentSize == 3) glUniform3fv(uniforms[name], arraySize, a); else if (componentSize == 4) glUniform4fv(uniforms[name], arraySize, a);
			}

			//setUniform with int

			void ShaderProgram::setUniform(const char* name, const int a) {
				glUniform1i(uniforms[name], a);
			}
			void ShaderProgram::setUniform(const char* name, const int a, const int b) {
				glUniform2i(uniforms[name], a, b);
			}
			void ShaderProgram::setUniform(const char* name, const int a, const int b, const int c) {
				glUniform3i(uniforms[name], a, b, c);
			}
			void ShaderProgram::setUniform(const char* name, const int a, const int b, const int c, const int d) {
				glUniform4i(uniforms[name], a, b, c, d);
			}
			void ShaderProgram::setUniform(const char* name, const GLsizei arraySize, const int* a) {
				glUniform1iv(uniforms[name], arraySize, a);
			}
			void ShaderProgram::setUniform(const char* name, const GLsizei componentSize, const GLsizei arraySize, const int* a) {
				if (componentSize == 1) glUniform1iv(uniforms[name], arraySize, a); else if (componentSize == 2) glUniform2iv(uniforms[name], arraySize, a); else if (componentSize == 3) glUniform3iv(uniforms[name], arraySize, a); else if (componentSize == 4) glUniform4iv(uniforms[name], arraySize, a);
			}

			//setUniform with unsigned int

			void ShaderProgram::setUniform(const char* name, const unsigned int a) {
				glUniform1ui(uniforms[name], a);
			}
			void ShaderProgram::setUniform(const char* name, const unsigned int a, const unsigned int b) {
				glUniform2ui(uniforms[name], a, b);
			}
			void ShaderProgram::setUniform(const char* name, const unsigned int a, const unsigned int b, const unsigned int c) {
				glUniform3ui(uniforms[name], a, b, c);
			}
			void ShaderProgram::setUniform(const char* name, const unsigned int a, const unsigned int b, const unsigned int c, const unsigned int d) {
				glUniform4ui(uniforms[name], a, b, c, d);
			}
			void ShaderProgram::setUniform(const char* name, const GLsizei arraySize, const unsigned int* a) {
				glUniform1uiv(uniforms[name], arraySize, a);
			}
			void ShaderProgram::setUniform(const char* name, const GLsizei componentSize, const GLsizei arraySize, const unsigned int* a) {
				if (componentSize == 1) glUniform1uiv(uniforms[name], arraySize, a); else if (componentSize == 2) glUniform2uiv(uniforms[name], arraySize, a); else if (componentSize == 3) glUniform3uiv(uniforms[name], arraySize, a); else if (componentSize == 4) glUniform4uiv(uniforms[name], arraySize, a);
			}
			void ShaderProgram::setUniform(const char* name, const mc::Vector<unsigned int, 4> v) {
				glUniform4ui(uniforms[name], v[0], v[1], v[2], v[3]);
			}

			void ShaderProgram::setUniform(const char* name, const mc::Vector<unsigned int, 3> v) {
				glUniform3ui(uniforms[name], v[0], v[1], v[2]);
			}
			void ShaderProgram::setUniform(const char* name, const mc::Vector<unsigned int, 2> v) {
				glUniform2ui(uniforms[name], v[0], v[1]);
			}
			void ShaderProgram::setUniform(const char* name, const mc::Vector<unsigned int, 1> v) {
				glUniform1ui(uniforms[name], v[0]);
			}
			void ShaderProgram::setUniform(const char* name, const mc::Vector<int, 4> v) {
				glUniform4i(uniforms[name], v[0], v[1], v[2], v[3]);
			}
			void ShaderProgram::setUniform(const char* name, const mc::Vector<int, 3> v) {
				glUniform3i(uniforms[name], v[0], v[1], v[2]);
			}
			void ShaderProgram::setUniform(const char* name, const mc::Vector<int, 2> v) {
				glUniform2i(uniforms[name], v[0], v[1]);
			}
			void ShaderProgram::setUniform(const char* name, const mc::Vector<int, 1> v) {
				glUniform1i(uniforms[name], v[0]);
			}
			void ShaderProgram::setUniform(const char* name, const mc::Vector<float, 4> v) {
				glUniform4f(uniforms[name], v[0], v[1], v[2], v[3]);
			}
			void ShaderProgram::setUniform(const char* name, const mc::Vector<float, 3> v) {
				glUniform3f(uniforms[name], v[0], v[1], v[2]);
			}
			void ShaderProgram::setUniform(const char* name, const mc::Vector<float, 2> v) {
				glUniform2f(uniforms[name], v[0], v[1]);
			}
			void ShaderProgram::setUniform(const char* name, const mc::Vector<float, 1> v) {
				glUniform1f(uniforms[name], v[0]);
			}

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
		}//ogl33
	}//gfx
}//mc
