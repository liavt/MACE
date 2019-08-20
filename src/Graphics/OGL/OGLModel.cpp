/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/OGL/OGLModel.h>

namespace mc {
	namespace internal {
		namespace ogl {
			namespace {
				const std::unordered_map<gfx::PrimitiveType, Enum> generatePrimitiveTypeLookup() {
					std::unordered_map<gfx::PrimitiveType, Enum> out{};
#define MACE__TABLE_ENTRY(name, value) out[gfx::PrimitiveType::name] = value;
					MACE__TABLE_ENTRY(POINTS, GL_POINTS);
					MACE__TABLE_ENTRY(LINES, GL_LINES);
					MACE__TABLE_ENTRY(LINES_ADJACENCY, GL_LINES_ADJACENCY);
					MACE__TABLE_ENTRY(LINES_STRIP, GL_LINE_STRIP);
					MACE__TABLE_ENTRY(LINES_STRIP_ADJACENCY, GL_LINE_STRIP_ADJACENCY);
					MACE__TABLE_ENTRY(TRIANGLES, GL_TRIANGLES);
					MACE__TABLE_ENTRY(TRIANGLES_ADJACENCY, GL_TRIANGLES_ADJACENCY);
					MACE__TABLE_ENTRY(TRIANGLES_STRIP, GL_TRIANGLE_STRIP);
					MACE__TABLE_ENTRY(TRIANGLES_STRIP_ADJACENCY, GL_TRIANGLE_STRIP_ADJACENCY);
#undef MACE__TABLE_ENTRY

					return out;
				}

				inline Enum lookupPrimitiveType(const gfx::PrimitiveType type) {
					static auto lookupTable = generatePrimitiveTypeLookup();

					return lookupTable[type];
				}
			}//anon namespace

			Model::Model(std::shared_ptr<Context> con) : Dispatchable(con) {
				dispatch([this]() {
					ogl::VertexArray::init();
					ogl::VertexArray::setName("OGLModel");
				});
			}

			Model::~Model() {
				dispatch([this]() {
					ogl::VertexArray::destroy();
				});
			}

			void Model::bind() const {
				dispatch([this]() {
					ogl::VertexArray::bind();
				});
				// not sure if we need to rebind all the VBO's in the VAO, or whether just binding the VAO will do

				//for (Index i = 0; i < buffers.size(); ++i) {
				//	buffers[i].bind();
				//}

				//if (indices.getIndiceNumber() > 0) {
				//	indices.bind();
				//}
			}

			void Model::draw() const {
				dispatch([this]() {
					MACE__BEGIN_OGL_FUNCTION;
					bind();

					if (indices.getIndiceNumber() > 0) {
						glDrawElements(lookupPrimitiveType(primitiveType), indices.getIndiceNumber(), GL_UNSIGNED_INT, nullptr);
					} else {
						glDrawArrays(lookupPrimitiveType(primitiveType), 0, getVertexNumber());
					}
				});
			}

			void Model::loadTextureCoordinates(const unsigned int dataSize, const float* data) {
				dispatch([this, dataSize, data]() {
					bind();

					ogl::VertexArray::storeDataInAttributeList(dataSize * sizeof(float), data, MACE__VAO_DEFAULT_TEXTURE_COORD_LOCATION, 2);
				});
			}

			void Model::loadVertices(const unsigned int verticeSize, const float* vertices) {
				dispatch([this, verticeSize, vertices]() {
					bind();

					ogl::VertexArray::loadVertices(verticeSize, vertices, MACE__VAO_DEFAULT_VERTICES_LOCATION, 3);
				});
			}

			void Model::loadIndices(const unsigned int indiceNum, const unsigned int* indiceData) {
				dispatch([this, indiceNum, indiceData]() {
					bind();

					ogl::VertexArray::loadIndices(indiceNum, indiceData);
				});
			}
		}//ogl
	}//internal
}//mc
