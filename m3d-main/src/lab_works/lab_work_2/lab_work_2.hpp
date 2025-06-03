#ifndef __LAB_WORK_2_HPP__
#define __LAB_WORK_2_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork2 : public BaseLabWork
	{
	  public:
		LabWork2() : BaseLabWork() {}
		~LabWork2();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

	  private:
		// ================ Scene data.
		std::vector<Vec2f> _triangleVer;
		// ================
		std::vector<GLuint> _indicesTri;
		std::vector<Vec3f>	_triangleCol;


		// Background color
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); 
		// ================
		GLuint _program = GL_INVALID_INDEX;

		// Identifiants pour le VBO et le VAO
		GLuint _vbo = GL_INVALID_INDEX; 
		GLuint _vao = GL_INVALID_INDEX; // VAO
		GLuint _ebo = GL_INVALID_INDEX; // EBO

		float _time		  = 0.0f;
		float _brightness = 1.0f;

		//stocke location of translation
		GLuint translationXLoc = 0; 

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_2_HPP__
