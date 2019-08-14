/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/Model.h>
#include <MACE/Core/Error.h>
#include <MACE/Graphics/Window.h>
#include <MACE/Graphics/Context.h>

namespace mc {
	namespace gfx {
#define MACE__VERIFY_MODEL_INIT() MACE_ASSERT(model != nullptr, "This Model has not had init() called yet")

		bool ModelImpl::operator==(const ModelImpl & other) const {
			return primitiveType == other.primitiveType;
		}

		bool ModelImpl::operator!=(const ModelImpl& other) const {
			return !operator==(other);
		}

		Model::Model() : model(nullptr) {}

		Model::Model(const std::shared_ptr<ModelImpl> mod) : model(mod) {}

		Model::Model(const Model& other) : model(other.model) {}

		void Model::init() {
			model = gfx::getCurrentWindow()->getComponent<GraphicsContextComponent>()->createModelImpl();
		}

		void Model::destroy() {
			model.reset();
		}

		void Model::createTextureCoordinates(const unsigned int dataSize, const float* data) {
			MACE__VERIFY_MODEL_INIT();

			model->loadTextureCoordinates(dataSize, data);
		}

		void Model::createVertices(const unsigned int verticeSize, const float* vertices, const PrimitiveType& prim) {
			MACE__VERIFY_MODEL_INIT();

			model->primitiveType = prim;
			model->loadVertices(verticeSize, vertices);

		}

		void Model::createIndices(const unsigned int indiceNum, const unsigned int* indiceData) {
			MACE__VERIFY_MODEL_INIT();

			model->loadIndices(indiceNum, indiceData);
		}

		PrimitiveType Model::getPrimitiveType() {
			return model->primitiveType;
		}

		const PrimitiveType Model::getPrimitiveType() const {
			return model->primitiveType;
		}

		void Model::draw() const {
			MACE__VERIFY_MODEL_INIT();

			model->draw();
		}

		bool Model::isCreated() const {
			return model != nullptr;
		}

		bool Model::operator==(const Model& other) const {
			return model == other.model;
		}

		bool Model::operator!=(const Model& other) const {
			return !operator==(other);
		}
	}//gfx
}//mc
