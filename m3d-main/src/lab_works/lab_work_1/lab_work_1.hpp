#ifndef __LAB_WORK_1_HPP__
#define __LAB_WORK_1_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork1 : public BaseLabWork
	{
	  public:
		LabWork1() : BaseLabWork() {}
		~LabWork1();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

	  private:
		// ================ Scene data.
		std::vector<Vec2f> _triangleVer;
	

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		// ================

		// Identifiant du programme OpenGL
		GLuint _program = GL_INVALID_INDEX;


		// Identifiants pour le VBO et le VAO
		GLuint _vbo = GL_INVALID_INDEX; 
		GLuint _vao = GL_INVALID_INDEX; 

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_1_HPP__
