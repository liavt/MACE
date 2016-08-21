#pragma once
#include <GL/glew.h>
#include <MC-System/Constants.h>
#include <iostream>

namespace mc {
namespace gfx{

template<GLenum bufferType>
class BufferObject {
public:
	virtual ~BufferObject() {
		if (isBuffer())destroy();
	};

	void bind() const{
		glBindBuffer(bufferType, id);
	};
	void unbind() const {
		glBindBuffer(bufferType, 0);
	};

	Index getID() const {
		return id;
	};

	GLboolean isBuffer() const {
		return glIsBuffer(id);
	};

	void init() {
		glGenBuffers(1, &id);
	};
	void destroy() {
		glDeleteBuffers(1,&id);
	};

	void setImmutableData(const GLsizeiptr dataSize, const GLvoid* data, GLbitfield flags) {
		glBufferStorage(bufferType,dataSize,data,flags);
	};
	void setData(const GLsizeiptr dataSize, const GLvoid* data, const GLenum drawType = GL_DYNAMIC_DRAW) const {
		glBufferData(bufferType, dataSize, data, drawType);
	};
	void setDataRange(const Index offset, const GLsizeiptr dataSize, const GLvoid* data) const {
		glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, data);
	};

	template<GLenum otherType>
	void copyData(BufferObject<otherType> other, GLsizeiptr size, Index readOffset = 0, Index writeOffset = 0) {
		glCopyBufferSubData(id,other.id,readOffset,writeOffset,size);
	};

	GLvoid* map(const GLenum access = GL_READ_WRITE) {
		return glMapBuffer(bufferType,access);
	};

	GLvoid* mapRange(const Index offset, const Size length, const GLbitfield access) {
		return glMapBufferRange(bufferType,offset,length,access);
	};

	GLboolean unmap() {
		return glUnmapBuffer(bufferType);
	};
protected:
	Index id=0;
};//BufferObject

class UBO : public BufferObject<GL_UNIFORM_BUFFER> {
public:
	void setLocation(const Index location);
	Index getLocation();
	const Index getLocation() const;

	void bindForRender(const Index offset = 0, const GLsizeiptr size = -1) const;

	void bindToUniformBlock(const Index programID, const char* blockName) const;

private:
	Index location = 0;
};//UBO

}//gfx
}//mc