﻿/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__GRAPHICS_OGL_OGL_H
#define MACE__GRAPHICS_OGL_OGL_H

//The api docs shouldn't include a bunch of internal classes, since any end user wouldn't care about them
#ifndef MACE__DOXYGEN_PASS

#include <MACE/Core/Constants.h>
#include <MACE/Core/Interfaces.h>
#include <MACE/Utility/Vector.h>
#include <MACE/Utility/Matrix.h>
#include <MACE/Utility/Color.h>
#include <MACE/Graphics/RenderTarget.h>

#include <glad/gl.h>

#include <unordered_map>
#include <vector>

#define MACE__VAO_DEFAULT_VERTICES_LOCATION 0
#define MACE__VAO_DEFAULT_TEXTURE_COORD_LOCATION 1

			//how many floats in the uniform buffer
#define MACE__ENTITY_DATA_BUFFER_SIZE sizeof(float) * 28
			//which binding location the uniform buffer goes to
#define MACE__ENTITY_DATA_LOCATION 0
#define MACE__ENTITY_DATA_STORAGE_FLAGS GL_DYNAMIC_STORAGE_BIT
			//the definition is later stringified. cant be a string because this gets added to the shader via a macro (see createShader)
#define MACE__ENTITY_DATA_NAME _mc_EntityData

#define MACE__PAINTER_DATA_BUFFER_SIZE sizeof(float) * 44
#define MACE__PAINTER_DATA_LOCATION 1
#define MACE__PAINTER_DATA_STORAGE_FLAGS GL_DYNAMIC_STORAGE_BIT
#define MACE__PAINTER_DATA_NAME _mc_PainterData

#define MACE__SCENE_ATTACHMENT_INDEX 0
#define MACE__ID_ATTACHMENT_INDEX 1
#define MACE__DATA_ATTACHMENT_INDEX 2

#define MACE__HAS_RENDER_FEATURE(features, feature) (features & gfx::Painter::RenderFeatures::feature) != gfx::Painter::RenderFeatures::NONE

namespace mc {
	namespace internal {
		/**
		Contains various abstractions for OpenGL. Assume that any class in this namespace requires an OpenGL context.
		<p>
		OpenGL abstractions will only implement functionality from core OpenGL 3.3, but may use features from extensions
		using the GLAD_GL_<extension> runtime check.
		<p>
		Most of the abstractions have very minimal extra work. Most of the time they are direct bindings to the
		actual OpenGL function. However, some classes like `ShaderProgram` do a lot of bookkeeping to make it easier
		to use.
		<p>
		The documentation for classes in the `ogl` namespace assumes that you understand the underlying OpenGL concepts.
		A link to a page about the OpenGL concept is usually present.
		<p>
		Each abstraction follows a very similar syntax and style to make it easier to understand.
		@see Object
		@see Buffer
		@internal
		*/
		namespace ogl {

			/**
			Thrown when OpenGL fails or errors
			@see ShaderError
			*/
			MACE__DECLARE_ERROR(OpenGL);

			/**
			Thrown when a Shader throws an exception, such as a failed compilation.
			@see OpenGLError
			*/
			MACE__DECLARE_ERROR(Shader);

			/**
			Thrown when a Framebuffer fails to be created, or throws an error
			@see gfx::ogl::Framebuffer
			@see OpenGLError
			*/
			MACE__DECLARE_ERROR(Framebuffer);

			void forceCheckGLError(const unsigned int line, const char* MACE_RESTRICT file, const char* MACE_RESTRICT message);

			/**
			@rendercontext
			*/
#ifdef MACE_DEBUG
			inline void checkGLError(const unsigned int line = 0, const char* MACE_RESTRICT file = "Unknown file", const char* MACE_RESTRICT message = "No message specified") {
				forceCheckGLError(line, file, message);
			}
			/**
			@copydoc ogl::checkGLError(const Index, const char*, const char*)
			*/
			inline void checkGLError(const unsigned int line, const char* file, const std::string& message) {
				checkGLError(line, file, message.c_str());
			}
#else
			inline void checkGLError(const unsigned int, const char* MACE_RESTRICT, const char* MACE_RESTRICT) {}
			/**
			@copydoc ogl::checkGLError(const Index, const char*, const char*)
			*/
			inline void checkGLError(const unsigned int, const char*, const std::string&) {}
#endif
			/**
			@rendercontext
			*/
			inline void enable(const Enum param) {
				glEnable(param);
			}
			/**
			@rendercontext
			*/
			inline void disable(const Enum param) {
				glDisable(param);
			}

			/**
			@rendercontext
			*/
			inline void setBlending(const Enum sfactor, const Enum dfactor) {
				glBlendFunc(sfactor, dfactor);
			}

			/**
			Reset the blending values to default values
			@rendercontext
			*/
			inline void resetBlending() {
				setBlending(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}

			/**
			@rendercontext
			*/
			inline void setViewport(const int x, const int y, const int width, const int height) {
				glViewport(x, y, width, height);
			}

			/**
			Represents a OpenGL object in memory. All abstractions for OpenGL objects override this.
			<p>
			Due to how the OpenGL model works, using an OpenGL function outside of a thread with a context will throw an error. Thus,
			this class does not follow the RAII model.
			<p>
			A subclass of `Object` will observe the following rules:
			- The constructor and destructor will do nothing, as there is no guarantee that the `Object` will be constructed in a OpenGL thread.
			- The init() and destroy() functions must be called in an OpenGL thread and will call the coressponding glCreate* and glDestroy* functions
			- The bind() and unbind() functions must be called in an OpenGL thread and will call the coressponding glBind* functions.
			- It will have an ID generated by glCreate\* and be deleted in glDelete\*.
			- bind() will use the ID created by glCreate\* and no other
			- isCreated() has to be called in an OpenGL thread
			- It will override == and !=

			@see Buffer
			@see https://www.opengl.org/wiki/OpenGL_Object
			*/
			class MACE_NOVTABLE Object: public Initializable {
			public:
				Object() noexcept = default;
				virtual MACE__DEFAULT_OPERATORS(Object);


				/**
				Calls the corresponding glCreate\* function and assigns an ID.
				@rendercontext
				@see Object::destroy()
				@see Object::bind() const
				@see Object::unbind const
				@see Object::isCreated() const
				*/
				virtual void init() override;
				/**
				Calls the corresponding glDestroy\* function and deletes the ID.
				@rendercontext
				@see Object::init()
				@see Object::bind() const
				@see Object::unbind const
				@see Object::isCreated() const
				@throws GL_INVALID_OPERATION If this `Object` has not been created yet (Object::init() has not been called)
				*/
				virtual void destroy() override;

				/**
				Binds this `Object` to be used in an OpenGL function.
				@rendercontext
				@see Object::unbind() const
				@throws GL_INVALID_OPERATION If this `Object` has not been created yet
				*/
				virtual void bind() const;
				/**
				Unbinds this `Object` which is equivalent to binding ID 0.
				@rendercontext
				@see Object::bind() const
				*/
				virtual void unbind() const;

				/**
				Queries OpenGL whether this Object's ID is a valid object.
				@return Whether this `Object` represents memory
				@see Object::bind() const
				@see Object::init()
				@rendercontext
				*/
				virtual bool isCreated() const = 0;

				/**
				Retrieves the current ID that this `Object` represents.
				<p>
				The ID is an unsigned number that acts like a pointer to OpenGL memory. It is assigned when Object::init() is called.
				<p>
				If it is 0, the `Object` is considered uncreated.
				<p>
				When using Object::bind() const it will bind to this ID. Object::unbind() const will bind to ID 0, which is the equivelant of a null pointer.
				@return The ID represented by this `Object`
				*/
				GLuint getID() const;

				static void init(Object* objects[], const GLsizei length);
				static void destroy(Object* objects[], const GLsizei length);

				/**
				Compares if 2 `Objects` are equal.
				@see Object::getID() const
				@see Object::operator!=(const Object&) const
				@return Whether `this` and `other` are the same
				@param other What to compare with
				*/
				bool operator==(const Object& other) const;
				/**
				Compares if 2 `Objects` are not equal.
				@see Object::getID() const
				@see Object::operator==(const Object&) const
				@return Whether `this` and `other` are different
				@param other What to compare with
				*/
				bool operator!=(const Object& other) const;
			protected:
				/**
				The ID of this `Object.` Should be set in Object::init() and become 0 in Object::destroy()
				<p>
				Object::getID() const returns this.
				*/
				GLuint id = 0;

				/**
				Intended to be overloaded to bind to a certain index. Object::bind() const and Object::unbind() const both use this function.
				@param id Index to bind to. If it is 0, it needs to be unbinded.
				@rendercontext
				@internal
				*/
				virtual void bindIndex(const GLuint id) const = 0;

				virtual void initIndices(GLuint id[], const GLsizei length) const = 0;
				template<Size N>
				void initIndices(GLuint id[N]) const {
					initIndices(id, N);
				}

				virtual void destroyIndices(const GLuint id[], const GLsizei length) const = 0;
				template<Size N>
				void destroyIndices(const GLuint id[N]) const {
					destroyIndices(id, N);
				}

			private:
				bool isInit() const noexcept override;

			};

			/**
			Represents a render buffer for use with a `FrameBuffer.` Instead of using a `Texture2D` target, you can attach a `RenderBuffer` instead
			and accomplish a similar effect. A `RenderBuffer` supports anti-aliasing natively but you can not access or modify it's data. It also
			represents a single image, similar to a texture.
			@see FrameBuffer
			@see https://www.opengl.org/wiki/Renderbuffer_Object
			*/
			class RenderBuffer: public Object {
			public:
				/**
				Sets the flags for the storage of this `RenderBuffer.`
				@param format A valied OpenGL image format such as GL_RGBA8
				@param width The width of the buffer
				@param height The height of the buffer
				@see RenderBuffer::setStorageMultisampled(const Size, const Enum, const Size, const Size)
				@see https://www.opengl.org/wiki/GLAPI/glRenderbufferStorage
				@rendercontext
				*/
				void setStorage(const Enum format, const GLsizei width, const GLsizei height);
				/**
				Sets the flags for the storage of this `RenderBuffer.` This version can have anti-aliasing.
				@param samples How many samples to use for MSAA
				@param format A valied OpenGL image format such as GL_RGBA8
				@param width The width of the buffer
				@param height The height of the buffer
				@see RenderBuffer::setStorage(const Enum, const Size, const Size)
				@see https://www.opengl.org/wiki/GLAPI/glRenderbufferStorageMultisample
				@rendercontext
				*/
				void setStorageMultisampled(const GLsizei samples, const Enum format, const GLsizei width, const GLsizei height);

				bool isCreated() const override;

				using Object::operator==;
				using Object::operator!=;
			private:
				void bindIndex(const GLuint id) const override;

				void initIndices(GLuint id[], const GLsizei length) const override;
				void destroyIndices(const GLuint id[], const GLsizei length) const override;
			};//RenderBuffer

			/**
			@see https://www.opengl.org/wiki/Texture
			@note Must be used as immutable storage (even if the extension isn't supported!)
			*/
			class Texture2D: public Object {
			public:
				Texture2D() noexcept;

				void bind() const override;
				void bind(const unsigned int location) const;

				bool isCreated() const override;

				/**
				@rendercontext
				@see https://www.opengl.org/wiki/GLAPI/glTexStorage2D
				@see https://www.opengl.org/wiki/GLAPI/glTexImage2D
				*/
				void createStorage(GLsizei width, GLsizei height, Enum type, Enum format, Enum internalFormat, GLint mipmapLevels);

				/**
				@rendercontext
				@see https://www.opengl.org/wiki/GLAPI/glTexSubImage2D
				*/
				void setSubdata(const void* data, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, Enum type, Enum format, GLint mipmapLevel);

				/**
				@rendercontext
				@see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glPixelStore.xhtml
				*/
				void setPixelStorage(const Enum alignment, const int number) const;

				/**
				@copydoc Texture2D::setPixelStorage(const Enum, const int)
				*/
				void setPixelStorage(const Enum alignment, const float number) const;


				/**
				@copydoc Texture2D::setPixelStorage(const Enum, const int)
				*/
				void setPixelStorage(const Enum alignment, const bool value) const;

				/**
				@rendercontext
				@see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGenerateMipmap.xhtml
				*/
				void generateMipmap();

				void setTarget(const Enum target);
				Enum& getTarget();
				const Enum& getTarget() const;

				void getImage(const Enum format, const Enum type, void* data) const;

				/**
				@rendercontext
				@see https://www.opengl.org/wiki/GLAPI/glTexParameter
				*/
				void setParameter(const Enum name, const int value);

				/**
				@copydoc Object::operator==(const Object&) const
				*/
				bool operator==(const Texture2D& other) const;
				/**
				@copydoc Object::operator!=(const Object&) const
				*/
				bool operator!=(const Texture2D& other) const;

			private:
				Enum target = GL_TEXTURE_2D;

				void bindIndex(const GLuint id) const override;

				void initIndices(GLuint id[], const GLsizei length) const override;
				void destroyIndices(const GLuint id[], const GLsizei length) const override;
			};//Texture2D

			/**
			Represents an OpenGL Framebuffer Object which allows rendering to a non-screen location. This also allows
			rendering to a `Texture2D` or `RenderBuffer` object.
			<p>
			This is not a subclass of the `Buffer` object as this is technically not an OpenGL Buffer. The name is
			misleading.

			@see https://www.opengl.org/wiki/Framebuffer_Object
			@see RenderBuffer
			@see Texture2D
			@todo target variable which is GL_FRAMEBUFFER by default. and update documentation accordingly
			*/
			class FrameBuffer: public Object {
			public:
				/**
				Select which color Buffer to use for reading via
				FrameBuffer::readPixels(const int, const int, const Size, const Size, const Enum, const Enum, void*) const

				@param mode Which attachment to use
				@see FrameBuffer::setPixelStorage(const Enum, const float)
				@see https://www.opengl.org/sdk/docs/man/html/glReadBuffer.xhtml
				@see FrameBuffer::attachTexture(const Enum, const Enum, const Enum, const unsigned int)
				@see FrameBuffer::attachTexture1D(const Enum, const Enum, const Enum, const unsigned int, const int)
				@see FrameBuffer::attachTexture2D(const Enum, const Enum, const Enum, const unsigned int, const int)
				@see FrameBuffer::attachTexture2D(const Enum, const Enum, const Enum, const unsigned int, const int, const int)
				@see FrameBuffer::attachTextureLayer(const Enum, const Enum, const unsigned int, const int)
				@see FrameBuffer::attachRenderbuffer(const Enum, const Enum, const RenderBuffer&)
				@rendercontext
				*/
				static void setReadBuffer(const Enum mode);

				/**
				@rendercontext
				*/
				static void setDrawBuffer(const Enum mode);

				static FrameBuffer getDefaultFramebuffer();

				/**
				@rendercontext
				*/
				static void clear(const unsigned int field);

				/**
				@rendercontext
				*/
				static void setClearColor(const float r, const float g, const float b, const float a);

				/**
				Attaches a texture to this `FrameBuffer`
				@param target The framebuffer target. Must be `GL_DRAW_FRAMEBUFFER`, `GL_READ_FRAMEBUFFER` or `GL_FRAMEBUFFER`
				@param attachment Which attachment port to use.
				@param tex What to attach
				@param level Mipmap level to attach. Is 0 by default.
				@see https://www.opengl.org/wiki/GLAPI/glFramebufferTexture
				@see setDrawBuffers(const Size, const Enum*)
				@rendercontext
				*/
				void attachTexture(const Enum target, const Enum attachment, const Texture2D& tex, const int level = 0);
				/**
				@copydoc FrameBuffer::attachTexture(const Enum, const Enum, const Texture2D&, const int)
				*/
				void attachTexture1D(const Enum target, const Enum attachment, const Texture2D& tex, const int level = 0);
				/**
				@copydoc FrameBuffer::attachTexture1D(const Enum, const Enum, const Texture2D&, const int)
				*/
				void attachTexture2D(const Enum target, const Enum attachment, const Texture2D& tex, const int level = 0);
				/**
				@copydoc FrameBuffer::attachTexture2D(const Enum, const Enum, const Texture2D&, const int)
				@param layer Which layer of the 3-dimensional image to use. It is 0 by default.
				*/
				void attachTexture3D(const Enum target, const Enum attachment, const Texture2D& tex, const int level = 0, const int layer = 0);

				/**
				Attaches a single layer from a texture to this `FrameBuffer`
				@param target The framebuffer target. Must be `GL_DRAW_FRAMEBUFFER`, `GL_READ_FRAMEBUFFER` or `GL_FRAMEBUFFER`
				@param attachment Which attachment port to use.
				@param tex What to attach
				@param level Mipmap level to attach. Is 0 by default.
				@param layer Which layer of the texture to use. It is 0 by default.
				@see https://www.opengl.org/wiki/GLAPI/glFramebufferTextureLayer
				@rendercontext
				*/
				void attachTextureLayer(const Enum target, const Enum attachment, const Texture2D& tex, const int level = 0, const int layer = 0);

				/**
				Attaches a `RenderBuffer` to this `FrameBuffer` for rendering.
				@param target The framebuffer target. Must be `GL_DRAW_FRAMEBUFFER`, `GL_READ_FRAMEBUFFER` or `GL_FRAMEBUFFER`
				@param attachment Which attachment port to use
				@param buffer The `RenderBuffer` to use.
				@see https://www.opengl.org/wiki/GLAPI/glFramebufferRenderbuffer
				@rendercontext
				*/
				void attachRenderbuffer(const Enum target, const Enum attachment, const RenderBuffer& buffer);


				/**
				Specifies what buffers to render to. Takes in an array of attachments. A `RenderBuffer` or `Texture2D` can be attached to act
				as a draw Buffer.
				@param arrSize Size of the array of render buffers
				@param buffers Pointer to the first element of the array
				@see https://www.opengl.org/wiki/GLAPI/glDrawBuffers
				@see FrameBuffer::attachTexture(const Enum, const Enum, const Enum, const unsigned int)
				@see FrameBuffer::attachTexture1D(const Enum, const Enum, const Enum, const unsigned int, const int)
				@see FrameBuffer::attachTexture2D(const Enum, const Enum, const Enum, const unsigned int, const int)
				@see FrameBuffer::attachTexture2D(const Enum, const Enum, const Enum, const unsigned int, const int, const int)
				@see FrameBuffer::attachTextureLayer(const Enum, const Enum, const unsigned int, const int)
				@see FrameBuffer::attachRenderbuffer(const Enum, const Enum, const RenderBuffer&)
				@rendercontext
				*/
				static void setDrawBuffers(const Size arrSize, const Enum* buffers);

				/**
				Read pixels from this `FrameBuffer`

				@param x The x-coordinate of the lower-left pixel block you want to read. The origin is the lower left corner of the framebuffer
				@param y The y-coordinate of the lower-left pixel block you want to read. The origin is the lower left corner of the framebuffer
				@param width The width of the pixel block you want to read. A width and height of 1 means a single pixel
				@param height The height of the pixel block you want to read. A width and height of 1 means a single pixel
				@param format The format of the pixel data. Must be one of GL_STENCIL_INDEX, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL, GL_RED, GL_GREEN, GL_BLUE, GL_RGB, GL_BGR, GL_RGBA, and GL_BGRA.
				@param type The type of the pixel data
				@param data Pointer to where you want the data to be written to.
				@see https://www.opengl.org/sdk/docs/man/html/glReadPixels.xhtml
				@see FrameBuffer::setPixelStorage(const Enum, const float)
				@see FrameBuffer::setReadBuffer(const Enum);
				@rendercontext
				*/
				static void readPixels(const int x, const int y, const Size width, const Size height, const Enum format, const Enum type, void* data);

				/**
				Set the pixel storage mode for this `FrameBuffer` for use in
				FrameBuffer::readPixels(const int, const int, const Size, const Size, const Enum, const Enum, void*) const

				@param name The parameter to change
				@param param What to change it to
				@see https://www.opengl.org/sdk/docs/man/html/glPixelStore.xhtml
				@see
				@rendercontext
				*/
				void setPixelStorage(const Enum name, const float param);
				/**
				@copydoc setPixelStorage(const Enum, const float)
				*/
				void setPixelStorage(const Enum name, const int param);

				bool isCreated() const override;

				/**
				Queries OpenGL for the status of this `FrameBuffer.` Can be used to detect whether this `FrameBuffer` encountered an error
				during creation.
				@param target The framebuffer target. Must be `GL_DRAW_FRAMEBUFFER`, `GL_READ_FRAMEBUFFER` or `GL_FRAMEBUFFER`
				@return The `FrameBuffer` status. The full list of enums is described in the OpenGL specification.
				@see https://www.opengl.org/wiki/GLAPI/glCheckFramebufferStatus
				@rendercontext
				*/
				Enum checkStatus(const Enum target);

				using Object::operator==;
				using Object::operator!=;
			private:
				void bindIndex(const GLuint id) const override;

				void initIndices(GLuint id[], const GLsizei length) const override;
				void destroyIndices(const GLuint id[], const GLsizei length) const override;
			};//FrameBuffer

			/**
			Represents a Buffer of memory in the GPU. This class should not be used directly. Instead,
			use special subclasses which provide additional functionality for the specific Buffer type.

			@see RenderBuffer
			@see FrameBuffer
			@see VertexBuffer
			@see UniformBuffer
			@see ElementBuffer
			*/
			class MACE_NOVTABLE Buffer: public Object {
			public:
				/**
				Creates the Buffer with the specified Buffer type. You should not usually use this directly.
				Instead, use a subclass which has extra functions for the Buffer type.
				<p>
				For example, instead of creating a `Buffer` with the type of `GL_ARRAY_BUFFER`, use the
				`VertexBuffer` class.

				@internal
				@see RenderBuffer
				@see FrameBuffer
				@see Texture2D
				@see VertexBuffer
				@see UniformBuffer
				@param bufferType OpenGL symbolic constant with the desired type.
				*/
				Buffer(const Enum bufferType) noexcept;

				bool isCreated() const override;

				void createStorage(const GLsizeiptr size, const void* data, const GLbitfield flags);
				/**
				Sets data in a range of the Buffer.
				<p>
				Does not initialize data. Buffer:setData(const ptrdiff_t&, const void*, const Enum) must be called first.
				@param offset Offset into the Buffer to set the data, measured in bytes
				@param dataSize How large the region being inserted is, measured in bytes
				@param data Pointer to the data being inserted
				@see https://www.opengl.org/wiki/GLAPI/glBufferSubData
				@see Buffer::copyData(Buffer&, const ptrdiff_t&, const Index, const Index)
				@rendercontext
				*/
				void setDataRange(const Index offset, const ptrdiff_t& dataSize, const void* data);

				/**
				Copy part of the data store of this `Buffer` into another `Buffer`

				@param other Target `Buffer` to copy into
				@param size How much data to copy
				@param readOffset How much data to offset by in this `Buffer`
				@param writeOffset Where to write the data in the target `Buffer`
				@see Buffer::setData(const ptrdiff_t&, const void*, const Enum)
				@see https://www.opengl.org/wiki/GLAPI/glCopyBufferSubData
				@see CopyReadBuffer
				@see CopyWriteBuffer
				@rendercontext
				*/
				void copyData(Buffer& other, const ptrdiff_t& size, const Index readOffset = 0, const Index writeOffset = 0);

				/**
				Maps the data in this `Buffer` to a pointer on the CPU side. May be slow.

				@param access Which access to use. Must be GL_READ_ONLY, GL_WRITE_ONLY, or GL_READ_WRITE.
				@see https://www.opengl.org/wiki/GLAPI/glMapBuffer
				@see Buffer::mapRange(const Index, const Size, const unsigned int)
				@warning DO NOT DELETE THE POINTER RETURNED. IT IS NOT DYNAMICALLY ALLOCATED.
				@warning After Buffer::unmap() is called, the pointer returned may be unsafe. Using it will be undefined.
				@attention When using this function don't forget to use Buffer::unmap() when you are done using the pointer
				@rendercontext
				*/
				void* map(const Enum access = GL_READ_WRITE);

				/**
				Maps a range of data in this `Buffer` to a pointer on the CPU side.

				@param offset The starting offset to map
				@param length How much data to map
				@param access A combination of access flags to determine how the data will be used. These flags determine how fast or slow the mapping will take.
				@see https://www.opengl.org/wiki/GLAPI/glMapBufferRange
				@see Buffer::map(const Enum)
				@warning DO NOT DELETE THE POINTER RETURNED. IT IS NOT DYNAMICALLY ALLOCATED.
				@warning After Buffer::unmap() is called, the pointer returned may be unsafe. Using it will be undefined.
				@attention When using this function don't forget to use Buffer::unmap() when you are done using the pointer
				@rendercontext
				*/
				void* mapRange(const Index offset, const Size length, const unsigned int access);

				/**
				Unmaps the Buffer. Any mapped points will be deleted and using them will be undefined.

				@return Whether the data was corrupted during reading. This will be very rare but when it does happen you must reinitialize the data.
				@see Buffer::map(const Enum)
				@see Buffer::mapRange(const Index, const Size, const unsigned int)
				@see https://www.opengl.org/wiki/GLAPI/glMapBuffer
				@warning Using a pointer after it was unmapped will be undefined
				@rendercontext
				*/
				bool unmap();

				void flushRange(const Index offset, const Size length);

				void getPointer(void** param);

				void getParameter(const Enum pname, int* data) const;
				void getParameter(const Enum pname, GLint64* data) const;

				bool isMapped() const;

				GLint64 getMapLength() const;
				GLint64 getMapOffset() const;

				/**
				Retrieves the buffer type for this Buffer. This is based on the class.
				@return The buffer type
				*/
				const Enum getBufferType() const;

				int getSize() const;

				/**
				@copydoc Object::operator==(const Object&) const
				*/
				bool operator==(const Buffer& other) const;
				/**
				@copydoc Object::operator!=(const Object&) const
				*/
				bool operator!=(const Buffer& other) const;
			private:
				Enum bufferType;

				void bindIndex(const GLuint id) const override;

				void initIndices(GLuint id[], const GLsizei length) const override;
				void destroyIndices(const GLuint id[], const GLsizei length) const override;
			};//Buffer

			/**
			Stores vertex data for a `VertexArray.` This is absolutely crucial for any rendering in `OpenGL.`
			<p>
			A `VertexArray` is simply an array of `VertexBuffer` behind the scenes. It provides an easy way
			to have multiple sets of data for one rendering pass, such as colors, texture coordinates, and
			normals.
			@see https://www.opengl.org/wiki/Vertex_Specification::Vertex_Buffer_Object
			*/
			class VertexBuffer: public Buffer {
			public:
				VertexBuffer() noexcept;

				/**
				Tells OpenGL how to interpret array data. Must be called in order for rendering to work. Must be used in
				conjuction with Buffer::setData(const ptrdiff_t, const GLvoid*, const Enum)
				<p>
				Behind the scenes, it will call either `glVertexAttribPointer` or `glVertexAttribIPointer` based on the
				arguments provided.
				@param attribSize The size of the component. Must be 1,2,3,4 or GL_BGRA (which is 4.)
				@param type The data type of the component, like GL_FLOAT or GL_UNSIGNED_INT
				@param normalized Whether the data should be normalized to a float or keep the initial value. False by default.
				@param stride The offset between components. 0 means every component is tightly packed. 0 by default.
				@param pointer An offset of the first component. 0 by default.
				@see https://www.opengl.org/wiki/GLAPI/glVertexAttribPointer
				@see VertexArray
				@rendercontext
				*/
				void setAttributePointer(const GLint attribSize, const Enum type, const bool normalized = 0, const GLsizei stride = 0, const void* pointer = nullptr);

				/**
				Modify the rate at which vertex attribute advance during instancing. If instanced rendering is not being used,
				this function does nothing.
				@param divisor The rate at which it moves through the data in rendering
				@see https://www.opengl.org/wiki/GLAPI/glVertexAttribDivisor
				@rendercontext
				@*/
				void setDivisor(const GLuint divisor);

				/**
				Enables this `VertexBuffer` for use in rendering. Not to be confused with Object::bind() const which binds
				data for modification or reading.
				@see https://www.opengl.org/wiki/GLAPI/glEnableVertexAttribArray
				@see VertexBuffer::disable() const
				@rendercontext
				*/
				void enable() const;
				/**
				Disable this `VertexBuffer` for use in rendering. Not to be confused with Object::unbind() const which unbinds
				data for modification or reading.
				@see https://www.opengl.org/wiki/GLAPI/glEnableVertexAttribArray
				@see VertexBuffer::enable() const
				@rendercontext
				*/
				void disable() const;

				/**
				Gets the current attribute location of this `VertexBuffer`
				@return The attribute index
				*/
				GLuint getLocation();
				/**
				@copydoc VertexBuffer::getLocation()
				*/
				const GLuint getLocation() const;
				/**
				Sets the attribute location of this `VertexBuffer.` 0 by default.
				@param newLocation What to set it to
				*/
				void setLocation(const GLuint newLocation);

				/**
				@copydoc Object::operator==(const Object&) const
				*/
				bool operator==(const VertexBuffer& other) const;
				/**
				@copydoc Object::operator!=(const Object&) const
				*/
				bool operator!=(const VertexBuffer& other) const;
			private:
				GLuint location = 0;
			};//VertexBuffer

			/**
			Stores element indices to save vertex data.

			@see https://www.opengl.org/wiki/Vertex_Specification::Index_buffers
			@see https://www.opengl.org/wiki/GLAPI/glDrawElements
			@see VertexArray
			@see VertexBuffer
			@note You must use the ElementBuffer::setIndiceNumber(const Size) function for this class to work correctly.
			*/
			class ElementBuffer: public Buffer {
			public:
				ElementBuffer() noexcept;

				/**
				Constructs an `ElementBuffer` with a specified amount of indices. Analagous to calling ElementBuffer::setIndiceNumber(const Size)
				immediately after construction.
				@param indiceNum The amount of indices this `ElementBuffer` will have
				@see https://www.opengl.org/wiki/GLAPI/glDrawElements
				@see https://www.opengl.org/wiki/Vertex_Rendering#Basic_Drawing
				@see ElementBuffer::getIndiceNumber()
				@see VertexArray
				*/
				ElementBuffer(const int indiceNum) noexcept;

				/**
				Tell this `ElementBuffer` how many indices there are. This is required for proper usage in the `VertexArray` class.
				@see https://www.opengl.org/wiki/GLAPI/glDrawElements
				@see https://www.opengl.org/wiki/Vertex_Rendering#Basic_Drawing
				@see Buffer::setData(const ptrdiff_t&, const void*, const Enum)
				@see VertexArray
				@param indices The amount of indices loaded
				*/
				void setIndiceNumber(const int indices);
				/**
				Retrieves how many indices this `ElementBuffer` has.
				@see ElementBuffer::setIndiceNumber(const Size)
				@see VertexArray
				@see https://www.opengl.org/wiki/GLAPI/glDrawElements
				@see https://www.opengl.org/wiki/Vertex_Rendering#Basic_Drawing
				@return The amount of indices
				*/
				int getIndiceNumber();
				/**
				@copydoc ElementBuffer::getIndiceNumber()
				*/
				const int getIndiceNumber() const;

				/**
				@copydoc Object::operator==(const Object&) const
				*/
				bool operator==(const ElementBuffer& other) const;
				/**
				@copydoc Object::operator!=(const Object&) const
				*/
				bool operator!=(const ElementBuffer& other) const;
			private:
				int indiceNumber;
			};//ElementBuffer

			/**
			@see https://www.opengl.org/wiki/Vertex_Specification#Vertex_Array_Object
			@see VertexBuffer
			*/
			class VertexArray: public Object {
			public:
				void destroy() override;

				bool isCreated() const override;

				/**
				@rendercontext
				@see VertexBuffer
				@param verticeSize amount of vertices
				*/
				void loadVertices(const unsigned int verticeSize, const float* vertices, const GLuint location = 15, const Byte attributeSize = 3, const Enum type = GL_FLOAT, const bool normalized = false);
				/**
				@rendercontext
				@see VertexBuffer
				*/
				void loadIndices(const unsigned int indiceNum, const unsigned int* indiceData);

				/**
				@rendercontext
				@see VertexBuffer
				@param dataSize amount of elements * sizeof() each individual element
				*/
				void storeDataInAttributeList(const unsigned int dataSize, const GLvoid* data, const GLuint location = 0, const Byte attributeSize = 3, const Enum type = GL_FLOAT, const bool normalized = false);

				/**
				@rendercontext
				*/
				void addBuffer(const VertexBuffer& newBuffer);

				void setVertexNumber(const GLsizei vertexNum);
				GLsizei getVertexNumber();
				const GLsizei getVertexNumber() const;

				void setIndices(const ElementBuffer& buffer);
				ElementBuffer& getIndices();
				const ElementBuffer& getIndices() const;

				void setBuffers(const std::vector<VertexBuffer>& newBuffers);
				std::vector<VertexBuffer>& getBuffers();
				const std::vector<VertexBuffer>& getBuffers() const;

				/**
				@copydoc Object::operator==(const Object&) const
				*/
				bool operator==(const VertexArray& other) const;
				/**
				@copydoc Object::operator!=(const Object&) const
				*/
				bool operator!=(const VertexArray& other) const;
			protected:
				ElementBuffer indices;
				std::vector<VertexBuffer> buffers = std::vector<VertexBuffer>();
			private:
				GLsizei vertexNumber = 0;

				void bindIndex(const GLuint id) const override;

				void initIndices(GLuint id[], const GLsizei length) const override;
				void destroyIndices(const GLuint id[], const GLsizei length) const override;
			};//VertexArray


			/**
			Stores uniform data for a shader in the form of a Buffer. Can be used to share data between multiple shaders or quickly change between
			sets of uniforms in one program.
			@see https://www.opengl.org/wiki/Uniform_Buffer_Object
			@see VertexBuffer
			@see ShaderProgram
			*/
			class UniformBuffer: public Buffer {
			public:
				UniformBuffer() noexcept;
				UniformBuffer(const char* n) noexcept;

				const char* getName() const;
				void setName(const char* name);

				/**
				@copydoc Object::operator==(const Object&) const
				*/
				bool operator==(const UniformBuffer& other) const;
				/**
				@copydoc Object::operator!=(const Object&) const
				*/
				bool operator!=(const UniformBuffer& other) const;
			private:
				const char* name;
			};//UniformBuffer

			/**
			Represents a shader object in OpenGL. A shader is a special program that gets executed during different parts of rendering.
			<p>
			The `Shader` class must be used with the `ShaderProgram` class.
			<p>
			In order to work correctly, `Shader::setType(const Enum)` or `Shader::Shader(const Enum) noexcept` must be called and a type must be
			set.
			<p>
			Unlike other `Objects`, the default constructor of `Shader` should not be used. It is only implemented for compatibility reasons.
			The `type` of the shader must be set before most functions are called. If `type` is equal to `GL_FALSE` an error is thrown.
			<p>
			Even though it extends `Object` it does not implement the `Object::bind()` const or `unbind()` functions. Instead, add the
			shader to a `ShaderProgram` and it will link from there.
			@see https://www.opengl.org/wiki/Shader
			@see https://www.opengl.org/wiki/GLSL_Object#Program_objects
			*/
			class Shader: public Object {
			public:
				/**
				Default constructor which sets `type` to `GL_FALSE`. If a function that requires `type` to be set is called, an error
				is thrown.
				<p>
				If this constructor is used, the Shader::setType(const Enum) function should be used later to set a type.
				@see Shader::Shader(const Enum) noexcept
				@see Shader
				*/
				Shader() noexcept;
				/**
				Constructor which sets the type of the `Shader`. Should be used instead of `Shader::Shader() noexcept`
				<p>
				The `type` must be a valid OpenGL constant representing a type of shader. If it is incorrect, an
				`OpenGLError` is thrown.
				@param type The type of `Shader`. Must be `GL_VERTEX_SHADER`, `GL_FRAGMENT_SHADER`, `GL_GEOMETRY_SHADER`, or the following if available: `GL_TESS_CONTROL_SHADER`, `GL_TESS_EVALUATION_SHADER`, and `GL_COMPUTE_SHADER`
				@see Shader::setType(const Enum)
				@see Shader
				*/
				Shader(const Enum type) noexcept;

				void init() override;
				void destroy() override;

				/**
				@see https://www.opengl.org/wiki/GLAPI/glShaderSource
				@rendercontext
				*/
				void setSource(const GLsizei count, const char* strings[], const int lengths[]);
				void setSource(const char string[], const int length);
				void setSource(const std::string& string);

				/**
				@rendercontext
				*/
				char* getSource(const GLsizei length, char* characters, int amount = 0) const;

				/**
				@rendercontext
				*/
				int getParameter(const Enum param) const;
				/**
				@rendercontext
				*/
				int getInfoLogLength() const;
				/**
				@rendercontext
				*/
				int getSourceLength() const;
				/**
				@rendercontext
				*/
				bool isDeleted() const;
				/**
				@rendercontext
				*/
				bool isCompiled() const;

				/**
				@see https://www.opengl.org/wiki/GLAPI/glCompileShader
				*/
				void compile();

				bool isCreated() const override;

				void setType(const Enum newType);
				Enum getType();
				const Enum getType() const;

				bool operator==(const Shader& other) const;
				bool operator!=(const Shader& other) const;
			private:
				Enum type;

				//shaders cant be bound or unbound according to the opengl spec
				void bind() const override final;
				void unbind() const override final;

				void bindIndex(const GLuint id) const override final;

				void initIndices(GLuint id[], const GLsizei length) const override;
				void destroyIndices(const GLuint id[], const GLsizei length) const override;
			};//Shader

			/**
			@see https://www.opengl.org/wiki/Shader
			@see https://www.opengl.org/wiki/GLSL_Object#Program_objects
			@see Shader
			*/
			class ShaderProgram: public Object {
			public:
				struct UniformBufferData {
					struct Field {
						GLint index;
						GLint size, offset;
						Enum type;
					};

					GLint size = 0;
					GLuint index = 0;

					std::unordered_map<std::string, Field> fields{};
				};

				void init() override;
				void destroy() override;

				/**
				@rendercontext
				*/
				void link();

				bool isCreated() const override;

				/**
				@rendercontext
				*/
				int getParameter(const Enum param) const;

				/**
				@rendercontext
				*/
				int getInfoLogLength() const;
				/**
				@rendercontext
				*/
				int getAttachedShaders() const;

				/**
				@rendercontext
				*/
				bool isDeleted() const;
				/**
				@rendercontext
				*/
				bool isLinked() const;
				/**
				@rendercontext
				*/
				bool isValidated() const;

				/**
				@rendercontext
				*/
				void detachShader(const GLuint id);
				/**
				@copydoc ShaderProgram::detachShader(const Index)
				*/
				void detachShader(const Shader& sh);

				/**
				@rendercontext
				*/
				void validate();

				/**
				@rendercontext
				*/
				void attachShader(const Shader shader);

				/**
				@rendercontext
				*/
				void createFragment(const char shader[]);
				/**
				@rendercontext
				*/
				void createFragment(const GLsizei count, const char* strings[]);
				/**
				@rendercontext
				*/
				void createVertex(const char shader[]);
				/**
				@rendercontext
				*/
				void createVertex(const GLsizei count, const char* strings[]);

				/**
				@rendercontext
				*/
				void createUniform(const std::string& name);
				/**
				@rendercontext
				*/
				void createUniform(const char* name);

				/**
				@rendercontext
				*/
				void createUniformBuffer(const char* name, const GLint location = -1);
				/**
				@copydoc createUniformBuffer(const char*, const GLint)
				*/
				void createUniformBuffer(const UniformBuffer& buf, const GLint location = -1);

				UniformBufferData& getUniformBuffer(const std::string name);

				UniformBufferData& getUniformBuffer(const UniformBuffer& buf);

				/**
				@rendercontext
				*/
				void setUniformBufferField(UniformBuffer& buf, const std::string name, const void* data, const ptrdiff_t size);

				/**
				@rendercontext
				*/
				void bindUniformBuffers(const UniformBuffer* buf[], const Size size);

				int getUniformLocation(const std::string& name) const;
				int getUniformLocation(const char* name) const;

				//Up next are a bunch of autogenerated overloaded functions
				//setUniform with float matrices
				/**
				@rendercontext
				*/
				void setUniform(CString name, const bool transpose, const mc::Matrix<float, 2, 2> & m);
				void setUniform(CString name, const mc::Matrix<float, 2, 2> & m);
				void setUniform(CString name, const bool transpose, const mc::Matrix<float, 3, 3> & m);
				void setUniform(CString name, const mc::Matrix<float, 3, 3> & m);
				void setUniform(CString name, const bool transpose, const mc::Matrix<float, 4, 4> & m);
				void setUniform(CString name, const mc::Matrix<float, 4, 4> & m);
				void setUniform(CString name, const bool transpose, const mc::Matrix<float, 2, 3> & m);
				void setUniform(CString name, const mc::Matrix<float, 2, 3> & m);
				void setUniform(CString name, const bool transpose, const mc::Matrix<float, 3, 2> & m);
				void setUniform(CString name, const mc::Matrix<float, 3, 2> & m);
				void setUniform(CString name, const bool transpose, const mc::Matrix<float, 2, 4> & m);
				void setUniform(CString name, const mc::Matrix<float, 2, 4> & m);
				void setUniform(CString name, const bool transpose, const mc::Matrix<float, 3, 4> & m);
				void setUniform(CString name, const mc::Matrix<float, 3, 4> & m);
				void setUniform(CString name, const bool transpose, const mc::Matrix<float, 4, 3> & m);
				void setUniform(CString name, const mc::Matrix<float, 4, 3> & m);

				//setUniform with float
				void setUniform(CString name, const float a);
				void setUniform(CString name, const float a, const float b);
				void setUniform(CString name, const float a, const float b, const float c);
				void setUniform(CString name, const float a, const float b, const float c, const float d);
				void setUniform(CString name, const GLsizei arraySize, const float* a);
				void setUniform(CString name, const GLsizei componentSize, const GLsizei arraySize, const float* a);
				void setUniform(CString name, const mc::Vector<float, 1> v);
				void setUniform(CString name, const mc::Vector<float, 2> v);
				void setUniform(CString name, const mc::Vector<float, 3> v);
				void setUniform(CString name, const mc::Vector<float, 4> v);
				//setUniform with int
				void setUniform(CString name, const int a);
				void setUniform(CString name, const int a, const int b);
				void setUniform(CString name, const int a, const int b, const int c);
				void setUniform(CString name, const int a, const int b, const int c, const int d);
				void setUniform(CString name, const GLsizei arraySize, const int* a);
				void setUniform(CString name, const GLsizei componentSize, const GLsizei arraySize, const int* a);
				void setUniform(CString name, const mc::Vector<int, 1> v);
				void setUniform(CString name, const mc::Vector<int, 2> v);
				void setUniform(CString name, const mc::Vector<int, 3> v);
				void setUniform(CString name, const mc::Vector<int, 4> v);
				//setUniform with unsigned int
				void setUniform(CString name, const unsigned int a);
				void setUniform(CString name, const unsigned int a, const unsigned int b);
				void setUniform(CString name, const unsigned int a, const unsigned int b, const unsigned int c);
				void setUniform(CString name, const unsigned int a, const unsigned int b, const unsigned int c, const unsigned int d);
				void setUniform(CString name, const GLsizei arraySize, const unsigned int* a);
				void setUniform(CString name, const GLsizei componentSize, const GLsizei arraySize, const unsigned int* a);
				void setUniform(CString name, const mc::Vector<unsigned int, 1> v);
				void setUniform(CString name, const mc::Vector<unsigned int, 2> v);
				void setUniform(CString name, const mc::Vector<unsigned int, 3> v);
				void setUniform(CString name, const mc::Vector<unsigned int, 4> v);

				void setShaders(const std::unordered_map<Enum, Shader>& newShaders);
				const std::unordered_map<Enum, Shader>& getShaders() const;
				std::unordered_map<Enum, Shader>& getShaders();

				void setUniforms(const std::unordered_map<std::string, int>& newUniforms);
				const std::unordered_map<std::string, int>& getUniforms() const;
				std::unordered_map<std::string, int>& getUniforms();

				bool operator==(const ShaderProgram& other) const;
				bool operator!=(const ShaderProgram& other) const;

			private:
				std::unordered_map<Enum, Shader> shaders;
				std::unordered_map<std::string, int> uniforms;
				std::unordered_map<std::string, UniformBufferData> uniformBuffers;

				void bindIndex(const GLuint id) const override;

				void initIndices(GLuint id[], const GLsizei length) const override;
				void destroyIndices(const GLuint id[], const GLsizei length) const override;
			};//ShaderProgram
		}//ogl
	}//internal
}//mc

#endif//MACE__DOXYGEN_PASS

#endif//MACE__GRAPHICS_OGL_OGL_H
