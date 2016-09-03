#include <MACE/Graphics/Buffer.h>

namespace mc {
namespace gfx {

void UBO::setLocation(const Index location)
{
	this->location = location;
}

Index UBO::getLocation()
{
	return location;
}

const Index UBO::getLocation() const
{
	return location;
}

void UBO::bindForRender(const Index offset, const GLsizeiptr size) const
{
	if (size<0 && offset <= offset) {
		glBindBufferBase(GL_UNIFORM_BUFFER, this->location, id);
	}
	else {
		glBindBufferRange(GL_UNIFORM_BUFFER, this->location, id, offset, size);
	}
}

void UBO::bindToUniformBlock(const Index programID, const char* blockName) const
{
	glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, blockName), location);
}

}
}