/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__GRAPHICS_MODEL_H
#define MACE__GRAPHICS_MODEL_H

#include <MACE/Core/Constants.h>

#include <memory>

namespace mc {
	namespace gfx {
		class GraphicsContextComponent;

		enum class PrimitiveType: short int {
			//these draw modes were chosen because they exist in both OpenGL 3.3 and Direct3D
			POINTS = 0,
			LINES = 1,
			LINES_ADJACENCY = 2,
			LINES_STRIP = 3,
			LINES_STRIP_ADJACENCY = 4,
			TRIANGLES = 5,
			TRIANGLES_ADJACENCY = 6,
			TRIANGLES_STRIP = 7,
			TRIANGLES_STRIP_ADJACENCY = 8,
			//TRIANGLES_FAN does not exist in DirectX 10+ because of performance issues. It is not included in this list
		};

		class MACE_NOVTABLE ModelImpl {
			friend class Model;
		public:
			virtual MACE__DEFAULT_OPERATORS(ModelImpl);

			virtual void draw() const = 0;

			virtual void loadTextureCoordinates(const unsigned int dataSize, const float* data) = 0;
			virtual void loadVertices(const unsigned int verticeSize, const float* vertices) = 0;
			virtual void loadIndices(const unsigned int indiceNum, const unsigned int* indiceData) = 0;

			bool operator==(const ModelImpl& other) const;
			bool operator!=(const ModelImpl& other) const;
		protected:
			ModelImpl() noexcept = default;

			PrimitiveType primitiveType = PrimitiveType::TRIANGLES;
		};

		class Model {
		public:
			Model();
			Model(GraphicsContextComponent* context);
			Model(const Model& other);

			/**
			@rendercontext
			*/
			void init(GraphicsContextComponent* context);
			/**
			@rendercontext
			*/
			void destroy();

			/**
			@rendercontext
			*/
			void createTextureCoordinates(const unsigned int dataSize, const float* data);
			/**
			@rendercontext
			*/
			template<const unsigned int N>
			void createTextureCoordinates(const float(&data)[N]) {
				createTextureCoordinates(N, data);
			}

			/**
			@rendercontext
			*/
			void createVertices(const unsigned int verticeSize, const float* vertices, const PrimitiveType& prim);
			/**
			@rendercontext
			*/
			template<const unsigned int N>
			void createVertices(const float(&vertices)[N], const PrimitiveType& prim) {
				createVertices(N, vertices, prim);
			}

			/**
			@rendercontext
			*/
			void createIndices(const unsigned int indiceNum, const unsigned int* indiceData);
			/**
			@rendercontext
			*/
			template<const unsigned int N>
			void createIndices(const unsigned int(&indiceData)[N]) {
				createIndices(N, indiceData);
			}

			PrimitiveType getPrimitiveType();
			const PrimitiveType getPrimitiveType() const;

			/**
			@rendercontext
			*/
			void draw() const;

			bool isCreated() const;

			bool operator==(const Model& other) const;
			bool operator!=(const Model& other) const;
		private:
			std::shared_ptr<ModelImpl> model;

			Model(const std::shared_ptr<ModelImpl> mod);
		};
	}
}//mc

#endif//MACE__GRAPHICS_MODEL_H
