#ifndef __LAB_WORK_3_HPP__
#define __LAB_WORK_3_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork3 : public BaseLabWork
	{
	  public:
		LabWork3() : BaseLabWork() {}
		~LabWork3();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;
		void _createCube();
		void _initBuffers();
		void _updateViewMatrix();
		void _updateProjectionMatrix();
		void _initCamera();
		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

	  private:
		// structure Mesh
		struct Mesh
		{
			std::vector<Vec3f>		  positions;
			std::vector<Vec3f>		  colors;
			std::vector<unsigned int> indices;
			Mat4f					  transform;

			GLuint vbo = 0, vao = 0, ebo = 0, colorBuffer = 0;
		};

		Mesh   _cube;
		GLuint _modelMatrixLoc = 0;
		GLuint _viewMatrixLoc  = 0;
		GLuint _projMatrixLoc  = 0;
		// ================ Scene data.
		float _cameraSpeed		 = 0.1f;
		float _cameraSens  = 0.1f;

		// ================ GL data.
		// ================

		// ================ Settings.
		// ================

		GLuint _program = GL_INVALID_INDEX;
		Camera _camera;
		float  _fovy = 60.0f;

		// GLuint translationXLocation; // Pour stocker l'emplacement de la variable uniform

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_3_HPP__
