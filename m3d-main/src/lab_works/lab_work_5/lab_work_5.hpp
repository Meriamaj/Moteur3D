#ifndef __LAB_WORK_5_HPP__
#define __LAB_WORK_5_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{

	class LabWork5 : public BaseLabWork
	{

	  public:
		LabWork5() : BaseLabWork() {}
		~LabWork5();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void _updateViewMatrix();
		void _updateProjectionMatrix();
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

		void phong();

		void NewFunction();

	  private:

		void _initCamera();
		
		// ================ Scene data.
		// ================

		// ================ GL data.
		GLuint _program;
		GLuint _vbo;
		GLuint _vboColor;
		GLuint _vao;
		GLuint _ebo;

		GLuint _vertShader;
		GLuint _fragShader;

		Camera _camera;
		float  _fovy = 60;
		bool   _updateFov;
		bool   _updateLightPosition;
		bool   _setLightPos;

		int	   _specularTy = 2;
		GLuint _uSpecularType;
		
		float  _cameraSpeed = 1;
		float _cameraSens  = 0.1f;
		
		GLuint _uMVP; GLuint _uMV; GLuint _uNormalMatrix; GLuint _uLightPosition; GLuint _uV;
		Mat4f  _MVP;  Mat4f  _MV;  Mat4f  _NormalMatrix;  Vec3f  _lightPosition;  Mat4f  _V;
		

		TriangleMeshModel _model;

		float _time;

		std::vector<Vec2f> _points;
		std::vector<Vec3f> _pointColors;
		std::vector<Vec3i> _triangle;

		bool w;
		bool a;
		bool s;
		bool d;
		bool sp;
		bool ls;
		bool lctrl;
		bool capslock;
		bool rotate_camera;

		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_5_HPP__
