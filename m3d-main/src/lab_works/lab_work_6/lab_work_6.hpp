#ifndef __LAB_WORK_6_HPP__
#define __LAB_WORK_6_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{

	class LabWork6 : public BaseLabWork
	{
	  public:
		LabWork6() : BaseLabWork() {}
		~LabWork6();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;


		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

		void none();

		void blinnphong();

		void phong();

	  private:
		bool _initGeometryPass();
		bool _initShadingPass();
		void _initCamera();
		bool _initShaders();
		bool _initGBuffer();
		void _updateShaders();

		void _geometryPass();
		void _shadingPass();

		// ================ Scene data.
		// ================

		// ================ GL data.
		GLuint _geometryPassProg;
		GLuint _shadingPassProg;

		GLuint _vbo;
		GLuint _vao;
		GLuint _ebo;

		GLuint _fbo;
		int	   _fboTaille[ 2 ];

		Camera _camera;
		float  _fovy = 60;
		bool   _updateFov;
		bool   _updateLightPosition;
		bool   _setLightPos;

		int	   _specularTy = 2;
		GLuint _uSpecularType;

		float _cameraSpeed		 = 1;
		float _cameraSens = 0.1f;

		GLuint _uMVP;
		GLuint _uMV;
		GLuint _uNormalMatrix;
		GLuint _uLightPosition;
		GLuint _uV;
		Mat4f  _MVP;
		Mat4f  _MV;
		Mat4f  _NormalMatrix;
		Vec3f  _lightPosition;
		Mat4f  _V;
		int	   _sBuffer = 0;

		TriangleMeshModel _model;

		float _time;

		std::vector<Vec2f> _points;
		std::vector<Vec3i> _triangle;

		bool w;
		bool a;bool s;
		bool d;
		bool sp;
		bool ls;bool lctrl;
		bool capslock;

		bool rotate_camera;

		
		
		bool		 _renderGBuff			= false;
		
		
		const char * _bufferTypeNom[ 6 ] = { "Position", "Normal", "Ambient", "Diffuse", "Spec", "Depth" };
		
		
		
		GLenum		 _bufferTypes[ 6 ]		= { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_DEPTH_ATTACHMENT };
		GLuint		 _gBufferTextures[ 6 ]; 


		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f );
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_6_HPP__
