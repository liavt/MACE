/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/OGL/OGL.h>
#include <memory>
#include <string>

namespace mc {
	namespace gfx {
		namespace ogl {
			namespace {
				Shader createShader(const Enum type, const char* sources[], const Size sourceSize) {
					Shader s = Shader(type);
					s.init();
					s.setSource(sourceSize, sources, nullptr);
					s.compile();
					return s;
				}

				void throwShaderError(const Index shaderId, const Enum type, const std::string& message) {
#ifdef MACE_DEBUG_OPENGL
					std::unique_ptr<GLchar[]> log_string = std::unique_ptr<GLchar[]>(new char[1024]);
					glGetShaderInfoLog(shaderId, 1024, 0, log_string.get());
					std::string friendlyType = "UNKNOWN SHADER TYPE " + std::to_string(type);//a more human friendly name for type, like VERTEX_SHADER instead of 335030
					if (type == GL_VERTEX_SHADER) {
						friendlyType = "VERTEX SHADER";
					} else if (type == GL_FRAGMENT_SHADER) {
						friendlyType = "FRAGMENT SHADER";
					} else if (type == GL_COMPUTE_SHADER) {
						friendlyType = "COMPUTE SHADER";
					} else if (type == GL_GEOMETRY_SHADER) {
						friendlyType = "GEOMETERY SHADER";
					} else if (type == GL_TESS_CONTROL_SHADER) {
						friendlyType = "TESSELLATION CONTROL SHADER";
					} else if (type == GL_TESS_EVALUATION_SHADER) {
						friendlyType = "TESSELLATION EVALUATION SHADER";
					} else if (type == GL_PROGRAM) {
						friendlyType = "SHADER PROGRAM";
						glGetProgramInfoLog(shaderId, 1024, 0, log_string.get());
					}
					MACE__THROW(Shader, "Error generating " + friendlyType + ": " + message + ": " + log_string.get());
#else
					MACE__THROW(Shader, "Error generating shader of type " + std::to_string(type));
#endif
				}
			}//anon namespace

			void forceCheckGLError(const Index line, const char* file, const char* message) {
				std::vector<Error> errors;

				Enum result = GL_NO_ERROR;
				while ((result = glGetError()) != GL_NO_ERROR) {
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
						case GL_STACK_OVERFLOW:
							errors.push_back(MACE__GET_ERROR_NAME(OpenGL) (std::string(message) + ": GL_STACK_OVERFLOW: A stack pushing operation cannot be done because it would overflow the limit of that stack's size", line, file));
							break;
						case GL_STACK_UNDERFLOW:
							errors.push_back(MACE__GET_ERROR_NAME(OpenGL) (std::string(message) + ": GL_STACK_UNDERFLOW: A stack popping operation cannot be done because the stack is already at its lowest point", line, file));
							break;
						case GL_OUT_OF_MEMORY:
							errors.push_back(MACE__GET_ERROR_NAME(OutOfMemory) (std::string(message) + ": GL_OUT_OF_MEMORY: There is not enough memory left to execute the command", line, file));
							break;
#ifdef GL_CONTEXT_LOST
						case GL_CONTEXT_LOST:
							errors.push_back(MACE__GET_ERROR_NAME(OpenGL) (std::string(message) + ": GL_CONTEXT_LOST: The GL Context has been lost due to a graphics card reset", line, file));
							break;
#endif
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

#ifdef MACE_DEBUG_OPENGL
			void checkGLError(const Index line, const char* file, const char* message) {
				forceCheckGLError(line, file, message);
			}

			void checkGLError(const Index line, const char* file, const std::string message) {
				checkGLError(line, file, message.c_str());
			}
#else
			void checkGLError(const Index, const char*, const char*) {}

			void checkGLError(const Index, const char*, const std::string&) {}
#endif

			void VertexArray::init() {
				glGenVertexArrays(1, &id);
			}

			void VertexArray::destroy() {
				glDeleteVertexArrays(1, &id);

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

			void VertexArray::loadVertices(const Size verticeSize, const float* vertices, const Index location, const Byte attributeSize, const Enum type, const bool normalized) {
				vertexNumber = verticeSize;

				storeDataInAttributeList(vertexNumber * sizeof(float), vertices, location, attributeSize, type, normalized);
			}

			void VertexArray::storeDataInAttributeList(const Size dataSize, const GLvoid* data, const Index location, const Byte attributeSize, const Enum type, const bool normalized) {
				bind();

				VertexBuffer buffer = VertexBuffer();
				buffer.init();
				buffer.bind();
				buffer.setLocation(location);
				// Give our data to opengl
				buffer.setData(static_cast<ptrdiff_t>(attributeSize * dataSize), data, GL_DYNAMIC_DRAW);
				buffer.setAttributePointer(attributeSize, type, normalized, 0, 0);

				addBuffer(buffer);
			}

			void VertexArray::loadIndices(const Size indiceNum, const unsigned int * indiceData) {
				indices = ElementBuffer(indiceNum);
				indices.init();
				indices.bind();
				indices.setData(static_cast<ptrdiff_t>(sizeof(unsigned int)*indiceNum), indiceData, GL_STATIC_DRAW);
			}

			void VertexArray::addBuffer(const VertexBuffer & newBuffer) {
				bind();
				newBuffer.bind();
				newBuffer.enable();

				buffers.push_back(newBuffer);
			}

			void VertexArray::setVertexNumber(const Size vertexNum) {
				vertexNumber = vertexNum;
			}

			Size VertexArray::getVertexNumber() {
				return vertexNumber;
			}

			const Size VertexArray::getVertexNumber() const {
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

			void VertexArray::bindIndex(const GLuint ID) const {
				glBindVertexArray(ID);
			}

			UniformBuffer::UniformBuffer() noexcept : Buffer(GL_UNIFORM_BUFFER) {}

			void UniformBuffer::setLocation(const Index loc) {
				this->location = loc;
			}

			Index UniformBuffer::getLocation() {
				return location;
			}

			const Index UniformBuffer::getLocation() const {
				return location;
			}

			void UniformBuffer::bindForRender(const Index offset, const ptrdiff_t size) const {
				if (size < 0 && offset == 0) {
					glBindBufferBase(GL_UNIFORM_BUFFER, this->location, id);
				} else {
					glBindBufferRange(GL_UNIFORM_BUFFER, this->location, id, offset, size);
				}
			}

			void UniformBuffer::bindToUniformBlock(const Index programID, const char* blockName) const {
				glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, blockName), location);
			}

			void UniformBuffer::bindToUniformBlock(const ShaderProgram & program, const char * blockname) const {
				bindToUniformBlock(program.getID(), blockname);
			}

			bool UniformBuffer::operator==(const UniformBuffer & other) const {
				return location == other.location&&Buffer::operator==(other);
			}

			bool UniformBuffer::operator!=(const UniformBuffer & other) const {
				return !(*this == other);
			}

			void FrameBuffer::setClearColor(const float r, const float g, const float b, const float a) {
				glClearColor(r, g, b, a);
			}

			void FrameBuffer::init() {
				glGenFramebuffers(1, &id);
			}

			void FrameBuffer::destroy() {
				glDeleteFramebuffers(1, &id);
				id = 0;
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

			void FrameBuffer::attachRenderbuffer(const Enum target, const Enum attachment, const RenderBuffer & buffer) {
				glFramebufferRenderbuffer(target, attachment, GL_RENDERBUFFER, buffer.getID());
			}

			void FrameBuffer::setDrawBuffers(const Size arrSize, const Enum * buffers) {
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

			void FrameBuffer::readPixels(const int x, const int y, const Size width, const Size height, const Enum format, const Enum type, void * data) const {
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

			void RenderBuffer::init() {
				glGenRenderbuffers(1, &id);
			}

			void RenderBuffer::destroy() {
				glDeleteRenderbuffers(1, &id);
			}

			void RenderBuffer::setStorage(const Enum format, const Size width, const Size height) {
				glRenderbufferStorage(GL_RENDERBUFFER, format, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
			}

			void RenderBuffer::setStorageMultisampled(const Size samples, const Enum format, const Size width, const Size height) {
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
			}

			bool RenderBuffer::isCreated() const {
				return glIsRenderbuffer(id) == 1;
			}

			void RenderBuffer::bindIndex(const GLuint ID) const {
				glBindRenderbuffer(GL_RENDERBUFFER, ID);
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

			bool Object::operator==(const Object & other) const {
				return this->id == other.id;
			}

			bool Object::operator!=(const Object & other) const {
				return !operator==(other);
			}

			Texture2D::Texture2D() noexcept {}

			void Texture2D::init() {
				glGenTextures(1, &id);

			}

			void Texture2D::destroy() {
				glDeleteTextures(1, &id);
			}

			void Texture2D::bind() const {
				Object::bind();
			}

			void Texture2D::bind(const unsigned int location) const {
				glActiveTexture(GL_TEXTURE0 + location);
				Object::bind();
			}

			void Texture2D::setData(const void * data, Size width, Size height, Enum type, Enum format, Enum internalFormat, Index mipmapLevel) {
				glTexImage2D(target, mipmapLevel, internalFormat, width, height, 0, format, type, data);
			}

			void Texture2D::setMultisampledData(const Size samples, const Size width, const Size height, const Enum internalFormat, const bool fixedSamples) {
				glTexImage2DMultisample(target, samples, internalFormat, width, height, fixedSamples);
			}

			void Texture2D::setPixelStorage(const Enum alignment, const int number) {
				glPixelStorei(alignment, number);
			}

			void Texture2D::setPixelStorage(const Enum alignment, const bool value) {
				setPixelStorage(alignment, value ? 1 : 0);
			}

			void Texture2D::setPixelStorage(const Enum alignment, const float number) {
				glPixelStoref(alignment, number);
			}

			void Texture2D::generateMipmap() {
				glGenerateMipmap(target);
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

			void Texture2D::getImage(const Enum format, const Enum type, void * data) const {
				glGetTexImage(target, 0, format, type, data);
			}


			bool Texture2D::operator==(const Texture2D & other) const {
				return target == other.target&&Object::operator==(other);
			}

			bool Texture2D::operator!=(const Texture2D & other) const {
				return !operator==(other);
			}

			void Texture2D::bindIndex(const GLuint ID) const {
				glBindTexture(target, ID);
			}

			Buffer::Buffer(const Enum type) noexcept : bufferType(type) {}

			bool Buffer::isCreated() const {
				return glIsBuffer(id) == 1;
			}

			void Buffer::init() {
				glGenBuffers(1, &id);
			}

			void Buffer::destroy() {
				glDeleteBuffers(1, &id);
			}

			void Buffer::setData(const ptrdiff_t& dataSize, const void* data, const Enum drawType) {
				glBufferData(bufferType, dataSize, data, drawType);
			}

			void Buffer::setDataRange(const Index offset, const ptrdiff_t& dataSize, const void* data) {
				glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, data);
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

			void Buffer::getPointer(void ** param) {
				glGetBufferPointerv(bufferType, GL_BUFFER_MAP_POINTER, param);
			}

			void Buffer::getParameter(const Enum pname, int * data) const {
				glGetBufferParameteriv(bufferType, pname, data);
			}

			void Buffer::getParameter(const Enum pname, GLint64 * data) const {
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

			bool Buffer::isImmutable() const {
				int out;
				getParameter(GL_BUFFER_IMMUTABLE_STORAGE, &out);
				return out != 0;
			}

			Enum Buffer::getAccess() const {
				int out;
				getParameter(GL_BUFFER_ACCESS, &out);
				return static_cast<Enum>(out);
			}

			Enum Buffer::getAccessFlags() const {
				int out;
				getParameter(GL_BUFFER_ACCESS_FLAGS, &out);
				return static_cast<Enum>(out);
			}

			Enum Buffer::getStorageFlags() const {
				int out;
				getParameter(GL_BUFFER_STORAGE_FLAGS, &out);
				return static_cast<Enum>(out);
			}

			Enum Buffer::getUsage() const {
				int out;
				getParameter(GL_BUFFER_USAGE, &out);
				return static_cast<Enum>(out);
			}

			const Enum Buffer::getBufferType() const {
				return bufferType;
			}

			int Buffer::getSize() const {
				int out;
				getParameter(GL_BUFFER_SIZE, &out);
				return out;
			}

			bool Buffer::operator==(const Buffer & other) const {
				return this->bufferType == other.bufferType&&Object::operator==(other);
			}

			bool Buffer::operator!=(const Buffer & other) const {
				return !operator==(other);
			}

			void Buffer::bindIndex(const GLuint ID) const {
				glBindBuffer(bufferType, ID);
			}

			VertexBuffer::VertexBuffer() noexcept : Buffer(GL_ARRAY_BUFFER) {}

			void VertexBuffer::setAttributePointer(const Byte attribSize, const Enum type, const bool normalized, const Index stride, const void * pointer) {
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

			void VertexBuffer::setDivisor(const unsigned int divisor) {
				glVertexAttribDivisor(location, divisor);
			}

			void VertexBuffer::enable() const {
				glEnableVertexAttribArray(location);
			}

			void VertexBuffer::disable() const {
				glDisableVertexAttribArray(location);
			}

			Index VertexBuffer::getLocation() {
				return location;
			}

			const Index VertexBuffer::getLocation() const {
				return location;
			}

			void VertexBuffer::setLocation(const Index newLocation) {
				location = newLocation;
			}

			bool VertexBuffer::operator==(const VertexBuffer & other) const {
				return Buffer::operator==(other) && location == other.location;
			}

			bool VertexBuffer::operator!=(const VertexBuffer & other) const {
				return !operator==(other);
			}

			ElementBuffer::ElementBuffer() noexcept : Buffer(GL_ELEMENT_ARRAY_BUFFER) {}

			ElementBuffer::ElementBuffer(const Size indiceNum) noexcept : ElementBuffer() {
				indiceNumber = indiceNum;
			}

			void ElementBuffer::setIndiceNumber(const Size indices) {
				indiceNumber = indices;
			}

			Size ElementBuffer::getIndiceNumber() {
				return indiceNumber;
			}

			const Size ElementBuffer::getIndiceNumber() const {
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

			void QueryObject::begin(const Enum target) {
				glBeginQuery(target, id);
			}

			void QueryObject::end(const Enum target) {
				glEndQuery(target);
			}

			void QueryObject::get(const Enum name, int * data) const {
				glGetQueryObjectiv(id, name, data);
			}

			void QueryObject::get(const Enum name, unsigned int * data) const {
				glGetQueryObjectuiv(id, name, data);
			}

			void QueryObject::get(const Enum name, int64_t * data) const {
				glGetQueryObjecti64v(id, name, data);
			}

			void QueryObject::get(const Enum name, uint64_t * data) const {
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

			void QueryObject::bindIndex(const GLuint) const {}

			PixelUnpackBuffer::PixelUnpackBuffer() noexcept : Buffer(GL_PIXEL_UNPACK_BUFFER) {}

			PixelPackBuffer::PixelPackBuffer() noexcept : Buffer(GL_PIXEL_PACK_BUFFER) {}

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

			void Shader::setSource(const Size count, const char * strings[], const int lengths[]) {
				if (type == GL_FALSE) {
					MACE__THROW(Shader, "Shader must have a type before compile() is called");
				}

				glShaderSource(id, count, strings, lengths);
			}

			void Shader::setSource(const char string[], const int length) {
				const int lengths[1] = { length };
				const char* strings[1] = { string };
				setSource(1, strings, lengths);
			}

			void Shader::setSource(const std::string & string) {
				//length() returns size_t which could be larger than unsigned in on some systems, causing problems. static_cast will fix it
				setSource(string.c_str(), static_cast<int>(string.length()));
			}

			char * Shader::getSource(const Size length, char * characters, int amount) const {
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

			bool Shader::operator==(const Shader & other) const {
				return Object::operator==(other) && type == other.type;
			}

			bool Shader::operator!=(const Shader & other) const {
				return !operator==(other);
			}

			void Shader::bind() const {}
			void Shader::unbind() const {}
			void Shader::bindIndex(const GLuint) const {}

			void ShaderProgram::bindIndex(const GLuint ID) const {
				glUseProgram(ID);
			}

			void ShaderProgram::init() {
				id = glCreateProgram();

				if (id == 0) {
					throwShaderError(id, GL_PROGRAM, "Failed to create program ID");
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
					throwShaderError(id, GL_PROGRAM, "The shader program was unable to link");
				}

				validate();

				if (!isValidated()) {
					throwShaderError(id, GL_PROGRAM, "The shader program failed to validate");
				}

				for (auto s : shaders) {
					if (s.second.isCreated()) {
						detachShader(s.second);
					}
				}

				checkGLError(__LINE__, __FILE__, "Error linking shader program");
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
			void ShaderProgram::detachShader(const Index shaderId) {
				glDetachShader(id, shaderId);
			}

			void ShaderProgram::detachShader(const Shader & sh) {
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
			void ShaderProgram::createFragment(const Size count, const char* strings[]) {
				attachShader(createShader(GL_FRAGMENT_SHADER, strings, count));
			}
			void ShaderProgram::createVertex(const char shader[]) {
				createVertex(1, &shader);
			}

			void ShaderProgram::createVertex(const Size count, const char* strings[]) {
				attachShader(createShader(GL_VERTEX_SHADER, strings, count));
			}

			void ShaderProgram::createUniform(const std::string& name) {
				int location = glGetUniformLocation(id, name.data());
				if (location < 0) {
					MACE__THROW(Shader, "Error finding uniform with name " + std::string(name));
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

			bool ShaderProgram::operator==(const ShaderProgram & other) const {
				return Object::operator==(other) && uniforms == other.uniforms&&shaders == other.shaders;
			}

			bool ShaderProgram::operator!=(const ShaderProgram & other) const {
				return !operator==(other);
			}

			//Autogenerated definitions.

			void ShaderProgram::setUniform(const char* name, const mc::Matrix<float, 4, 4>& m) {
				setUniform(name, true, m);
			}

			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<float, 4, 4>& m) {
				float flattenedData[4 * 4]; glUniformMatrix4fv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}

			void ShaderProgram::setUniform(const char* name, const mc::Matrix<float, 3, 3>& m) {
				setUniform(name, true, m);
			}

			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<float, 3, 3>& m) {
				float flattenedData[3 * 3]; glUniformMatrix3fv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}

			void ShaderProgram::setUniform(const char* name, const mc::Matrix<float, 2, 2>& m) {
				setUniform(name, true, m);
			}

			//setUniform with float matrices

			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<float, 2, 2>& m) {
				float flattenedData[2 * 2]; glUniformMatrix2fv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<float, 2, 3>& m) {
				float flattenedData[2 * 3]; glUniformMatrix2x3fv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<float, 2, 3>& m) {
				setUniform(name, true, m);
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<float, 3, 2>& m) {
				float flattenedData[3 * 2]; glUniformMatrix3x2fv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<float, 3, 2>& m) {
				setUniform(name, true, m);
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<float, 2, 4>& m) {
				float flattenedData[2 * 4]; glUniformMatrix2x4fv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<float, 2, 4>& m) {
				setUniform(name, true, m);
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<float, 3, 4>& m) {
				float flattenedData[3 * 4]; glUniformMatrix3x4fv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<float, 3, 4>& m) {
				setUniform(name, true, m);
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<float, 4, 3>& m) {
				float flattenedData[4 * 3]; glUniformMatrix4x3fv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<float, 4, 3>& m) {
				setUniform(name, true, m);
			}

			//setUniform with double matrices

			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<double, 2, 2>& m) {
				double flattenedData[2 * 2]; glUniformMatrix2dv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<double, 2, 2>& m) {
				setUniform(name, true, m);
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<double, 3, 3>& m) {
				double flattenedData[3 * 3]; glUniformMatrix3dv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<double, 3, 3>& m) {
				setUniform(name, true, m);
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<double, 4, 4>& m) {
				double flattenedData[4 * 4]; glUniformMatrix4dv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<double, 4, 4>& m) {
				setUniform(name, true, m);
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<double, 2, 3>& m) {
				double flattenedData[2 * 3]; glUniformMatrix2x3dv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<double, 2, 3>& m) {
				setUniform(name, true, m);
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<double, 3, 2>& m) {
				double flattenedData[3 * 2]; glUniformMatrix3x2dv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<double, 3, 2>& m) {
				setUniform(name, true, m);
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<double, 2, 4>& m) {
				double flattenedData[2 * 4]; glUniformMatrix2x4dv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<double, 2, 4>& m) {
				setUniform(name, true, m);
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<double, 3, 4>& m) {
				double flattenedData[3 * 4]; glUniformMatrix3x4dv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<double, 3, 4>& m) {
				setUniform(name, true, m);
			}
			void ShaderProgram::setUniform(const char* name, const bool transpose, const mc::Matrix<double, 4, 3>& m) {
				double flattenedData[4 * 3]; glUniformMatrix4x3dv(uniforms[name], 1, transpose, m.flatten(flattenedData));
			}
			void ShaderProgram::setUniform(const char* name, const mc::Matrix<double, 4, 3>& m) {
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
			void ShaderProgram::setUniform(const char* name, const Size arraySize, const float * a) {
				glUniform1fv(uniforms[name], arraySize, a);
			}
			void ShaderProgram::setUniform(const char* name, const Size componentSize, const Size arraySize, const float * a) {
				if (componentSize == 1) glUniform1fv(uniforms[name], arraySize, a); else if (componentSize == 2) glUniform2fv(uniforms[name], arraySize, a); else if (componentSize == 3) glUniform3fv(uniforms[name], arraySize, a); else if (componentSize == 4) glUniform4fv(uniforms[name], arraySize, a);
			}

			//setUniform with double

			void ShaderProgram::setUniform(const char* name, const double a) {
				glUniform1d(uniforms[name], a);
			}
			void ShaderProgram::setUniform(const char* name, const double a, const double b) {
				glUniform2d(uniforms[name], a, b);
			}
			void ShaderProgram::setUniform(const char* name, const double a, const double b, const double c) {
				glUniform3d(uniforms[name], a, b, c);
			}
			void ShaderProgram::setUniform(const char* name, const double a, const double b, const double c, const double d) {
				glUniform4d(uniforms[name], a, b, c, d);
			}
			void ShaderProgram::setUniform(const char* name, const Size arraySize, const double * a) {
				glUniform1dv(uniforms[name], arraySize, a);
			}
			void ShaderProgram::setUniform(const char* name, const Size componentSize, const Size arraySize, const double * a) {
				if (componentSize == 1) glUniform1dv(uniforms[name], arraySize, a); else if (componentSize == 2) glUniform2dv(uniforms[name], arraySize, a); else if (componentSize == 3) glUniform3dv(uniforms[name], arraySize, a); else if (componentSize == 4) glUniform4dv(uniforms[name], arraySize, a);
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
			void ShaderProgram::setUniform(const char* name, const Size arraySize, const int * a) {
				glUniform1iv(uniforms[name], arraySize, a);
			}
			void ShaderProgram::setUniform(const char* name, const Size componentSize, const Size arraySize, const int * a) {
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
			void ShaderProgram::setUniform(const char* name, const Size arraySize, const unsigned int * a) {
				glUniform1uiv(uniforms[name], arraySize, a);
			}
			void ShaderProgram::setUniform(const char* name, const Size componentSize, const Size arraySize, const unsigned int * a) {
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
			void ShaderProgram::setUniform(const char* name, const mc::Vector<double, 4> v) {
				glUniform4d(uniforms[name], v[0], v[1], v[2], v[3]);
			}
			void ShaderProgram::setUniform(const char* name, const mc::Vector<double, 3> v) {
				glUniform3d(uniforms[name], v[0], v[1], v[2]);
			}
			void ShaderProgram::setUniform(const char* name, const mc::Vector<double, 2> v) {
				glUniform2d(uniforms[name], v[0], v[1]);
			}
			void ShaderProgram::setUniform(const char* name, const mc::Vector<double, 1> v) {
				glUniform1d(uniforms[name], v[0]);
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

			void enable(const Enum param) {
				glEnable(param);
			}

			void disable(const Enum param) {
				glDisable(param);
			}

			void setBlending(const Enum sfactor, const Enum dfactor) {
				glBlendFunc(sfactor, dfactor);
			}

			void setViewport(const Index x, const Index y, const Size width, const Size height) {
				glViewport(x, y, width, height);
			}
		}//ogl
	}//gfx
}//mc
