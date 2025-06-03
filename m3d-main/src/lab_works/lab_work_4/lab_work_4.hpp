#ifndef __LAB_WORK_4_HPP__
#define __LAB_WORK_4_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "define.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "common/camera.hpp"


namespace M3D_ISICG
{
	class LabWork4 : public BaseLabWork
	{
	  public:
		LabWork4() : BaseLabWork() {}
		~LabWork4();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;
		void _updateViewMatrix();
		void _updateProjectionMatrix();
		void _initCamera();
		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

	  private:
		// Gestion OpenGL
		GLuint _program		  = GL_INVALID_INDEX; // Programme shader
		//GLuint _uMVPMatrixLoc = GL_INVALID_INDEX; // Uniform de la matrice MVP

		// Matrices
		glm::mat4 _modelMatrix = glm::mat4( 1.0f );
		glm::mat4 _viewMatrix  = glm::mat4( 1.0f );
		glm::mat4 _projMatrix  = glm::mat4( 1.0f );
		GLuint	  _modelMatrixLoc = 0;
		GLuint	  _viewMatrixLoc  = 0;
		GLuint	  _projMatrixLoc  = 0;
		//GLuint	  _program				= GL_INVALID_INDEX;
		GLint	  _uMVPMatrixLoc		= GL_INVALID_INDEX;
		GLint	  _uModelMatrixLoc		= GL_INVALID_INDEX;
		GLint	  _uViewMatrixLoc		= GL_INVALID_INDEX;
		GLint	  _uProjectionMatrixLoc = GL_INVALID_INDEX;
		GLint	  _uNormalMatrixLoc		= GL_INVALID_INDEX;

		float	  lightPower	  = 3.f;
		glm::vec3 materialDff  = glm::vec3( 1.0f, 0.0f, 0.0f ); // Diffuse color
		glm::vec3 materialSpc = glm::vec3( 1.0f, 1.0f, 1.0f ); // Specular color
		float	  shininess		   = 12.0f;
		Vec3f	  ambientCl		   = Vec3f( 0.1f, 0.1f, 0.1f );
		Vec3f	  lightPosition	   = Vec3f( 2.0f, 2.0f, 2.0f );
		Vec3f	  lightColor	   = Vec3f( 1.0f, 1.0f, 1.0f ); 

		GLuint	  mvpLoc; 
		glm::mat4 mvp ;
		// Données de rendu
		TriangleMeshModel _model; 
		Camera _camera;
		float	 _fovy = 60.0f;
		int				  _blinnPhong = 2;
		bool			  _updateBlinn;
		GLuint			  _uBlinn;
		// Paramètres
		Vec4f					 _bgColor = Vec4f( 0.2f, 0.2f, 0.2f, 1.0f ); // Couleur de fond
		

		float _cameraSpeed		 =4 ;
		float _cameraSensitivity = 0.1f;

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_4_HPP__
