/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Buffer.h>
#include <MACE/Graphics/GraphicsConstants.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace mc {
	namespace gfx {

		void VertexArray::init() {
			glGenVertexArrays(1, &id);
		}

		void VertexArray::destroy() {
			glDeleteVertexArrays(1, &id);
		}

		bool VertexArray::isCreated() const {
			return glIsVertexArray(id) == 1;
		}

		void VertexArray::draw(GLenum type) const {
			glDrawElements(type, indiceNumber, GL_UNSIGNED_INT, 0);
		}

		void VertexArray::loadVertices(const Size && verticeSize, const GLfloat vertices[], const Index && location, const Size && attributeSize) {
			vertexNumber = verticeSize;

			bind();
			storeDataInAttributeList(std::move(vertexNumber), vertices, std::move(location), std::move(attributeSize));
		}

		void VertexArray::loadIndices(const Size&& indiceNum, const GLuint indices[], const GLenum&& drawType) {
			bind();
			Index indicesID;
			// Generate 1 buffer, put the resulting identifier in vertexbuffer
			glGenBuffers(1, &indicesID);//
										// The following commands will talk about our 'vertexbuffer' buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);//
																// Give our vertices to OpenGL.
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*(indiceNum), indices, drawType);//

																									//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);//

			gfx::checkGLError();//

			this->indiceNumber = indiceNum;

			unbind();
		}
		void VertexArray::storeDataInAttributeList(const Size&& dataSize, const GLvoid* data, const Index&& location, const Size&& attributeSize) {
			bind();

			Index vboID;
			// Generate 1 buffer, put the resulting identifier in vertexbuffer
			glGenBuffers(1, &vboID);//
									// The following commands will talk about our 'vertexbuffer' buffer
			glBindBuffer(GL_ARRAY_BUFFER, vboID);//
													// Give our data to opengl
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(attributeSize * dataSize), data, GL_DYNAMIC_DRAW);//
			gfx::checkGLError();//

			glVertexAttribPointer(location, attributeSize, GL_FLOAT, GL_FALSE, 0, 0);

			glEnableVertexAttribArray(location);


			//	glBindBuffer(GL_ARRAY_BUFFER, vboID);//
			checkGLError();
		}

		bool VertexArray::operator==(const VertexArray & other) const {
			return vertexNumber == other.vertexNumber&&indiceNumber == other.indiceNumber&&Object::operator==(other);
		}

		bool VertexArray::operator!=(const VertexArray & other) const {
			return !operator==(other);
		}

		void VertexArray::bindIndex(const Index & ID) const {
			glBindVertexArray(ID);
		}

		UniformBuffer::UniformBuffer() : Buffer(GL_UNIFORM_BUFFER) {}

		void UniformBuffer::setLocation(const Index loc) {
			this->location = loc;
		}

		Index UniformBuffer::getLocation() {
			return location;
		}

		const Index UniformBuffer::getLocation() const {
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

		Buffer::Buffer(const Enum& type) : bufferType(type) {}

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
		bool Buffer::operator==(const Buffer & other) const {
			return this->bufferType == other.bufferType&&Object::operator==(other);
		}
		bool Buffer::operator!=(const Buffer & other) const {
			return !operator==(other);
		}
		void Buffer::bindIndex(const Index & ID) const {
			glBindBuffer(bufferType, ID);
		}

		VertexBuffer::VertexBuffer() : Buffer(GL_ARRAY_BUFFER) {}

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

		Index VertexBuffer::getLocation() {
			return location;
		}

		const Index VertexBuffer::getLocation() const {
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

		ElementBuffer::ElementBuffer() : Buffer(GL_ELEMENT_ARRAY_BUFFER) {}

		CopyReadBuffer::CopyReadBuffer() : Buffer(GL_COPY_READ_BUFFER) {}

		CopyWriteBuffer::CopyWriteBuffer() : Buffer(GL_COPY_WRITE_BUFFER) {}

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

		PixelUnpackBuffer::PixelUnpackBuffer() : Buffer(GL_PIXEL_UNPACK_BUFFER) {}

		PixelPackBuffer::PixelPackBuffer() : Buffer(GL_PIXEL_PACK_BUFFER) {}

	}
}