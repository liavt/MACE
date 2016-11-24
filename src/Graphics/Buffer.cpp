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

		void VertexArray::bindIndex(const Index & id) const {
			glBindVertexArray(id);
		}

		UniformBuffer::UniformBuffer() : Buffer(GL_UNIFORM_BUFFER) {
		}

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

		void FrameBuffer::attachTextureLayer(const GLenum & target, const GLenum & attachment, const GLuint& texture, const GLint & level, const GLint & layer) {
			bind();
			glFramebufferTextureLayer(target, attachment, level, texture, layer);
		}

		void FrameBuffer::attachRenderbuffer(const GLenum & target, const GLenum & attachment, const RenderBuffer & buffer) {
			bind();
			glFramebufferRenderbuffer(target, attachment, GL_RENDERBUFFER, buffer.getID());
		}

		void FrameBuffer::setParameter(const GLenum & paramName, const GLint & value) {
			glFramebufferParameteri(id, paramName, value);
		}

		void FrameBuffer::setDrawBuffers(const Size & arrSize, const GLenum * buffers) {
			glDrawBuffers(arrSize, buffers);
		}

		bool FrameBuffer::isCreated() const {
			return glIsFramebuffer(id) == 1;
		}

		GLenum FrameBuffer::checkStatus(const GLenum & target) {
			bind();
			return glCheckFramebufferStatus(target);
		}

		void FrameBuffer::bindIndex(const Index & id) const {
			glBindFramebuffer(GL_FRAMEBUFFER, id);
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

		void RenderBuffer::bindIndex(const Index & id) const {
			glBindRenderbuffer(GL_RENDERBUFFER, id);
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

		void Texture::bindIndex(const Index & id) const {
			glBindTexture(target, id);
		}

		Buffer::Buffer(const GLenum type) : bufferType(type) {}

		bool Buffer::isCreated() const {
			return glIsBuffer(id) == 1;
		}

		void Buffer::init() {
			glGenBuffers(1, &id);
		}

		void Buffer::destroy() {
			glDeleteBuffers(1, &id);
		};

		void Buffer::setImmutableData(const GLsizeiptr dataSize, const GLvoid* data, GLbitfield flags) {
			glBufferStorage(bufferType, dataSize, data, flags);
		};
		void Buffer::setData(const GLsizeiptr dataSize, const GLvoid* data, const GLenum drawType) const {
			glBufferData(bufferType, dataSize, data, drawType);
		};
		void Buffer::setDataRange(const Index offset, const GLsizeiptr dataSize, const GLvoid* data) const {
			glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, data);
		};

		void Buffer::copyData(Buffer other, GLsizeiptr size, Index readOffset, Index writeOffset) {
			glCopyBufferSubData(id, other.id, readOffset, writeOffset, size);
		};

		GLvoid* Buffer::map(const GLenum access) {
			return glMapBuffer(bufferType, access);
		};

		GLvoid* Buffer::mapRange(const Index offset, const Size length, const GLbitfield access) {
			return glMapBufferRange(bufferType, offset, length, access);
		};

		GLboolean Buffer::unmap() {
			return glUnmapBuffer(bufferType);
		}
		void Buffer::bindIndex(const Index & id) const {
			glBindBuffer(bufferType, id);
		}

}
}