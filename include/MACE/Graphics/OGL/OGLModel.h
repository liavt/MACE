/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__GRAPHICS_OGL_OGLMODEL_H
#define MACE__GRAPHICS_OGL_OGLMODEL_H

//The api docs shouldn't include a bunch of internal classes, since any end user wouldn't care about them
#ifndef MACE__DOXYGEN_PASS

#include <MACE/Graphics/Model.h>
#include <MACE/Graphics/OGL/OGL.h>
#include <MACE/Graphics/OGL/Dispatchable.h>

namespace mc {
	namespace internal {
		namespace ogl {
			class Model: public gfx::ModelImpl, private ogl::VertexArray, private Dispatchable {
			public:
				Model(std::shared_ptr<Context> context);
				~Model();

				void bind() const;

				void draw() const override;

				/*
				@bug these functions will create a NEW buffer each time you call them, instead of reusing them
				*/
				void loadTextureCoordinates(const unsigned int dataSize, const float* data) override;
				void loadVertices(const unsigned int verticeSize, const float* vertices) override;
				void loadIndices(const unsigned int indiceNum, const unsigned int* indiceData) override;
			};
		}//ogl
	}//internal
}//mc

#endif//MACE__DOXYGEN_PASS

#endif//MACE__GRAPHICS_OGL_OGLMODEL_H