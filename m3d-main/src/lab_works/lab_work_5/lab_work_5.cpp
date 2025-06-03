#include "lab_work_5.hpp"
#include "glm/gtx/string_cast.hpp"
#include "imgui.h"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace M3D_ISICG
{

	const std::string LabWork5::_shaderFolder = "src/lab_works/lab_work_5/shaders/";

	LabWork5::~LabWork5()
	{
		glDisableVertexArrayAttrib( _vao, 0 );
		glDeleteVertexArrays( 1, &_vao );
		glDeleteBuffers( 1, &_vbo );
		glDeleteProgram( _program );
	}

	bool LabWork5::init()
	{
		std::cout << "Initializing lab work 5..." << std::endl;
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );	
		_model.load( "Conference", "./data/models/sponza.obj" );
		_model._transformation = glm::scale( _model._transformation, Vec3f( 0.003, 0.003, 0.003 ) );

		const std::string vertexShaderStr = readFile( _shaderFolder + "mesh_texture.vert" );
		if ( vertexShaderStr.empty() )
		{
			std::cerr << "Error: Failed to read vertex shader file." << std::endl;
			return false;
		}

		const std::string fragmentShaderStr = readFile( _shaderFolder + "mesh_texture.frag" );
		if ( fragmentShaderStr.empty() )
		{
			std::cerr << "Error: Failed to read fragment shader file." << std::endl;
			return false;
		}
		// Create shaders objects
		GLuint vertexShader	  = glCreateShader( GL_VERTEX_SHADER );
		GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		// Associer les chaînes de caractères aux objets shaders
		const GLchar * vSrc = vertexShaderStr.c_str();
		const GLchar * fSrc = fragmentShaderStr.c_str();

		glShaderSource( vertexShader, 1, &vSrc, NULL );
		glShaderSource( fragmentShader, 1, &fSrc, NULL );

		glCompileShader( vertexShader );
		glCompileShader( fragmentShader );

		GLint compiled;
		glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertexShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

		glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertexShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std ::cerr << " Error compiling fragment shader : " << log << std ::endl;
			return false;
		}
		_program = glCreateProgram();
		if ( _program == 0 )
			throw std::runtime_error( "Failed to create shader program." );
		std::cout << "[LabWork4] Shader program created." << std::endl;

		// Attacher les shaders au programme
		glAttachShader( _program, vertexShader );
		glAttachShader( _program, fragmentShader );

		// Lier le programme
		glLinkProgram( _program );

		// Check if link is ok.
		GLint linked;
		glGetProgramiv( _program, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _program, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		glUseProgram( _program );
		GLenum err;
		while ( ( err = glGetError() ) != GL_NO_ERROR )
		{
			std::cerr << "OpenGL Error: " << err << std::endl;
		}

		// delete shaders after link
		glDeleteShader( vertexShader );
		glDeleteShader( fragmentShader );

		_uMVP		   = glGetUniformLocation( _program, "uMVPMatrix" );
		_uMV		   = glGetUniformLocation( _program, "uMVMatrix" );
		_uV			   = glGetUniformLocation( _program, "uVMatrix" );
		_uNormalMatrix = glGetUniformLocation( _program, "uNormalMatrix" );

		_uLightPosition = glGetUniformLocation( _program, "uLightPos" );
		_uSpecularType	= glGetUniformLocation( _program, "uSpecularType" );

		_lightPosition = Vec3f( 1, 1, 1 );
		_initCamera();

		glProgramUniform3fv( _program, _uLightPosition, 1, glm::value_ptr( _lightPosition ) );
		glProgramUniform1i( _program, _uSpecularType, _specularTy );
		glEnable( GL_DEPTH_TEST );

		std::cout << "Done!" << std::endl;
		return true;
	}


		void LabWork5::animate( const float p_deltaTime )
	{
		// Handle camera movements
		float multipl = capslock ? 3.f : 1.f;
		multipl		  = lctrl ? 0.2f : multipl;
		if ( w )
			_camera.moveFront( _cameraSpeed * multipl * p_deltaTime );
		if ( s )
			_camera.moveFront( -_cameraSpeed * multipl * p_deltaTime );
		if ( a )
			_camera.moveRight( -_cameraSpeed * multipl * p_deltaTime );
		if ( d )
			_camera.moveRight( _cameraSpeed * multipl * p_deltaTime );
		if ( s )
			_camera.moveUp( _cameraSpeed * multipl * p_deltaTime );
		if ( ls )
			_camera.moveUp( -_cameraSpeed * multipl * p_deltaTime );

		if ( _updateFov )
		{
			_camera.setFovy( _fovy );
		}
	}

	void LabWork5::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		_V			  = _camera.getViewMatrix();
		_MV			  = _camera.getViewMatrix() * _model._transformation;
		_MVP		  = _camera.getProjectionMatrix() * _MV;
		_NormalMatrix = glm::transpose( glm::inverse( _MV ) );

		glProgramUniformMatrix4fv( _program, _uV, 1, false, glm::value_ptr( _V ) );
		glProgramUniformMatrix4fv( _program, _uMV, 1, false, glm::value_ptr( _MV ) );
		glProgramUniformMatrix4fv( _program, _uMVP, 1, false, glm::value_ptr( _MVP ) );
		glProgramUniformMatrix4fv( _program, _uNormalMatrix, 1, false, glm::value_ptr( _NormalMatrix ) );

		_model.render( _program );

		glBindVertexArray( 0 );
	}

	void LabWork5::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN || p_event.type == SDL_KEYUP )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				w = p_event.type == SDL_KEYDOWN ? true : false;
				break;

			case SDL_SCANCODE_S: // Back
				s = p_event.type == SDL_KEYDOWN ? true : false;
				break;

			case SDL_SCANCODE_A: // Left
				a = p_event.type == SDL_KEYDOWN ? true : false;
				break;

			case SDL_SCANCODE_D: // Right
				d = p_event.type == SDL_KEYDOWN ? true : false;
				break;

			case SDL_SCANCODE_SPACE: // Up
				s = p_event.type == SDL_KEYDOWN ? true : false;
				break;

			case SDL_SCANCODE_LSHIFT: // Bottom
				ls = p_event.type == SDL_KEYDOWN ? true : false;
				break;

			case SDL_SCANCODE_LCTRL: // Bottom
				lctrl = p_event.type == SDL_KEYDOWN ? true : false;
				break;

			case SDL_SCANCODE_CAPSLOCK: capslock = p_event.type == SDL_KEYDOWN ? true : false; break;

			default: break;
			}
		}

		if ( p_event.type == SDL_MOUSEBUTTONDOWN )
		{
			switch ( p_event.button.button )
			{
			case SDL_BUTTON_LEFT:
				// Rotate the camera only when not on imgui
				if ( !ImGui::GetIO().WantCaptureMouse )
				{
					rotate_camera = true;
					SDL_SetRelativeMouseMode( SDL_TRUE );
				}
				break;

			case SDL_BUTTON_RIGHT:
				rotate_camera = false;
				SDL_SetRelativeMouseMode( SDL_FALSE );

				int w, h;
				SDL_GetWindowSize( SDL_GetMouseFocus(), &w, &h );
				SDL_WarpMouseInWindow( NULL, w / 2, h / 2 );
				break;

			default: break;
			}
		}

		// Rotate when left click + motion
		if ( rotate_camera && p_event.type == SDL_MOUSEMOTION )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSens, p_event.motion.yrel * _cameraSens );
		}
	}

	void LabWork5::displayUI()
	{
		ImGui::Begin( "Settings of lab work 5" );

		_updateFov = ImGui::SliderFloat( "FOV", &_fovy, 30, 120, "%.0f", 1 );

		if ( ImGui::DragFloat3( "Light position", glm::value_ptr( _lightPosition ), 0.05f, -10, 10, "%.2f", 1 ) )
		{
			glProgramUniform3fv( _program, _uLightPosition, 1, glm::value_ptr( _lightPosition ) );
		}
	


		NewFunction();
		ImGui::SameLine();
		phong();
		ImGui::SameLine();
		

	
		ImGui::End();
	}

	void LabWork5::phong()
	{
		if ( ImGui::RadioButton( "Phong method", _specularTy == 1 ) )
		{
			_specularTy = 1;
			glProgramUniform1i( _program, _uSpecularType, _specularTy );
		}
	}

	void LabWork5::NewFunction()
	{
		if ( ImGui::RadioButton( "Blinn-Phong method", _specularTy == 2 ) )
		{
			_specularTy = 2;
			glProgramUniform1i( _program, _uSpecularType, _specularTy );
		}
	}

	void LabWork5::_initCamera()
	{
		_camera.setPosition( Vec3f( -2.5, 0.75, 0 ) );
		_camera.setLookAt( Vec3f( 0, 0.75, 0 ) );
		_camera.setFovy( 60 );
		_camera.print();
	}
} // namespace M3D_ISICG
