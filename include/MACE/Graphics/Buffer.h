/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_GRAPHICS_BUFFER_H
#define MACE_GRAPHICS_BUFFER_H

#include <MACE/System/Constants.h>
#include <MACE/Utility/Color.h>
#include <GL/glew.h>


namespace mc {
	namespace gfx {

		class Object {
		public:
			virtual ~Object() = default;

			virtual void init() = 0;
			virtual void destroy() = 0;

			void bind() const;
			void unbind() const;

			virtual bool isCreated() const = 0;

			Index getID() const;

		protected:
			Index id = 0;

			virtual void bindIndex(const Index& id) const = 0;
		};

		class VertexArray: public Object {
		public:
			Size vertexNumber, indiceNumber;

			~VertexArray();

			void init();
			void destroy();

			bool isCreated() const;

			void draw(const GLenum type = GL_TRIANGLES) const;

			void loadVertices(const Size&& verticeSize, const GLfloat vertices[], const Index&& location = 15, const Size&& attributeSize = 3);
			void loadIndices(const Size&& indiceNum, const GLuint indices[], const GLenum&& drawType = GL_DYNAMIC_DRAW);

			void storeDataInAttributeList(const Size&& dataSize, const GLvoid* data, const Index&& location = 0, const Size&& attributeSize = 3);

		private:
			void bindIndex(const Index& id) const;
		};//VertexArray

		template<GLenum bufferType>
		class Buffer: public Object {
		public:
			virtual ~Buffer() {};

			bool isCreated() const {
				return glIsBuffer(id) == 1;
			};

			void init() {
				glGenBuffers(1, &id);
			};
			void destroy() {
				glDeleteBuffers(1, &id);
			};

			void setImmutableData(const GLsizeiptr dataSize, const GLvoid* data, GLbitfield flags) {
				glBufferStorage(bufferType, dataSize, data, flags);
			};
			void setData(const GLsizeiptr dataSize, const GLvoid* data, const GLenum drawType = GL_DYNAMIC_DRAW) const {
				glBufferData(bufferType, dataSize, data, drawType);
			};
			void setDataRange(const Index offset, const GLsizeiptr dataSize, const GLvoid* data) const {
				glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, data);
			};

			template<GLenum otherType>
			void copyData(Buffer<otherType> other, GLsizeiptr size, Index readOffset = 0, Index writeOffset = 0) {
				glCopyBufferSubData(id, other.id, readOffset, writeOffset, size);
			};

			GLvoid* map(const GLenum access = GL_READ_WRITE) {
				return glMapBuffer(bufferType, access);
			};

			GLvoid* mapRange(const Index offset, const Size length, const GLbitfield access) {
				return glMapBufferRange(bufferType, offset, length, access);
			};

			GLboolean unmap() {
				return glUnmapBuffer(bufferType);
			};
		private:
			void bindIndex(const Index& id) const {
				glBindBuffer(bufferType, id);
			}
		};//Buffer

		class UniformBuffer: public Buffer<GL_UNIFORM_BUFFER> {
		public:
			void setLocation(const Index location);
			Index getLocation();
			const Index getLocation() const;

			void bindForRender(const Index offset = 0, const GLsizeiptr size = -1) const;

			void bindToUniformBlock(const Index programID, const char* blockName) const;

		private:
			Index location = 0;
		};//UniformBuffer

		class RenderBuffer: public Object {
		public:
			void init();
			void destroy();

			void setStorage(const GLenum& format, const GLsizei& width, const GLsizei& height);
			void setStorageMultisampled(const GLsizei& samples, const GLenum& format, const GLsizei& width, const GLsizei& height);

			bool isCreated() const;
		private:
			void bindIndex(const Index& id) const;
		};//RenderBuffer

		class FrameBuffer: public Object {
		public:
			void init();
			void destroy();

			void attachTexture(const GLenum& target, const GLenum& attachment, const GLuint& textureID, const GLint& level);
			void attachTexture1D(const GLenum& target, const GLenum& attachment, const GLenum& texTarget, const GLuint& textureID, const GLint& level);
			void attachTexture2D(const GLenum& target, const GLenum& attachment, const GLenum& texTarget, const GLuint& textureID, const GLint& level);
			void attachTextureLayer(const GLenum& target, const GLenum& attachment, const GLuint& texture, const GLint& level, const GLint& layer);

			void attachRenderbuffer(const GLenum& target, const GLenum& attachment, const RenderBuffer& buffer);

			void setParameter(const GLenum& paramName, const GLint& value);

			void setDrawBuffers(const Size& arrSize, const GLenum* buffers);

			bool isCreated() const;

			GLenum checkStatus(const GLenum& target);
		private:
			void bindIndex(const Index& id) const;
		};//FrameBuffer

		class Texture: public Object {
		public:
			void init();
			void destroy();

			void setData(const void * data, Size width, Size height, GLenum type = GL_FLOAT, GLenum format = GL_RGB, GLenum internalFormat = GL_RGB, Index mipmapLevel = 0);

			void loadFile(const char* file);

			void setTarget(const GLenum target);
			GLenum getTarget();

			Color& getPaint();
			const Color& getPaint() const;
			void setPaint(const Color& c);

			float getOpacity();
			const float getOpacity() const;
			void setOpacity(const float f);

			bool isCreated() const;

			void setParameter(const GLenum& name, const GLint& value);
		private:
			GLenum target = GL_TEXTURE_2D;

			Color paint = Color(1.0f, 1.0f, 1.0f, 1.0f);

			float opacity = 1.0f;

			void bindIndex(const Index& id) const;
		};//Texture

	}//gfx
}//mc

#endif