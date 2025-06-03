#include "lab_work_4.hpp"
#include "common/camera.hpp"
#include "imgui.h"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork4::_shaderFolder = "src/lab_works/lab_work_4/shaders/";

	LabWork4::~LabWork4() {}
	bool LabWork4::init()
	{
		std::cout << "[LabWork4] Initialization started." << std::endl;

		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		try
		{
			std::cout << "Loading model 'Bunny'..." << std::endl;
			_model.load( "Bunny", "data/models/bunny.obj" );
			_model._transformation = glm::scale( _model._transformation, Vec3f( 0.003, 0.003, 0.003 ) );

			// triMshMode._setupGL(); // Initialisation des buffers
			std::cout << "Model loaded successfully!" << std::endl;
		}
		catch ( const std::runtime_error & e )
		{
			std::cerr << "Error loading model: " << e.what() << std::endl;
			return false;
		}
		const std::string vertexShaderStr = readFile( _shaderFolder + "mesh.vert" );
		if ( vertexShaderStr.empty() )
		{
			std::cerr << "Error: Failed to read vertex shader file." << std::endl;
			return false;
		}

		const std::string fragmentShaderStr = readFile( _shaderFolder + "mesh.frag" );
		if ( fragmentShaderStr.empty() )
		{
			std::cerr << "Error: Failed to read fragment shader file." << std::endl;
			return false;
		}
		// Create shader objects
		GLuint vertexShader	  = glCreateShader( GL_VERTEX_SHADER );
		GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		// Associate strings to shader obj
		const GLchar * vSrc = vertexShaderStr.c_str();
		const GLchar * fSrc = fragmentShaderStr.c_str();

		glShaderSource( vertexShader, 1, &vSrc, NULL );
		glShaderSource( fragmentShader, 1, &fSrc, NULL );

		glCompileShader( vertexShader );
		glCompileShader( fragmentShader );

		GLint compiled;
		glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &compiled );
		//verify compilation
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

		// Attache shaders
		glAttachShader( _program, vertexShader );
		glAttachShader( _program, fragmentShader );

		// Link program
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
		mvpLoc = glGetUniformLocation( _program, "uMVPMatrix" );
		_initCamera();
		_updateViewMatrix();
		_updateProjectionMatrix();

		glEnable( GL_DEPTH_TEST );

		return true;
	}

	void LabWork4::animate( const float p_deltaTime ) {}

	void LabWork4::_updateViewMatrix()
	{
		glUseProgram( _program );
		GLenum err;
		while ( ( err = glGetError() ) != GL_NO_ERROR )
		{
			std::cerr << "OpenGL Error: " << err << std::endl;
		}

		glProgramUniformMatrix4fv( _program,
								   glGetUniformLocation( _program, "uViewMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( _camera.getViewMatrix() ) );
	}

	void LabWork4::_updateProjectionMatrix()
	{
		glUseProgram( _program );
		GLenum err;
		while ( ( err = glGetError() ) != GL_NO_ERROR )
		{
			std::cerr << "OpenGL Error: " << err << std::endl;
		}

		glProgramUniformMatrix4fv( _program,
								   glGetUniformLocation( _program, "uProjectionMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( _camera.getProjectionMatrix() ) );
	}
	void LabWork4::_initCamera()
	{
		_camera.setPosition( Vec3f( 12, 12, 12 ) );
		_camera.setLookAt( Vec3f( 7, 4, 10 ) );

		_camera.setScreenSize( 1280, 720 );

		_camera.setLookAt( Vec3f( 0.f, 0.f, 0.f ) ); 

	}

	void LabWork4::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glUseProgram( _program );

		glm::mat4 modelMatrix = _modelMatrix;
		glm::mat4 viewMatrix  = _camera.getViewMatrix();
		glm::mat4 projMatrix  = _camera.getProjectionMatrix();
		
		// Passer la matrice MVP au shader
		mvp = projMatrix * viewMatrix * modelMatrix;
		glUniformMatrix4fv( mvpLoc, 1, GL_FALSE, glm::value_ptr( mvp ) );


		// GLuint mvpLoc = glGetUniformLocation( _program, "uMVPMatrix" );
		GLenum err;
		while ( ( err = glGetError() ) != GL_NO_ERROR )
		{
			std::cerr << "OpenGL Error: " << err << std::endl;
		}


		glm::mat3 normalMatrix = glm::transpose( glm::inverse( glm::mat3( modelMatrix ) ) );
		glUniformMatrix3fv(
			glGetUniformLocation( _program, "uNormalMatrix" ), 1, GL_FALSE, glm::value_ptr( normalMatrix ) );
		// Couleur ambiante
		glUniform3fv( glGetUniformLocation( _program, "uAmbientColor" ), 1, glm::value_ptr( ambientCl ) );

		// Position de la lumi re
		glUniform3fv( glGetUniformLocation( _program, "uLightPosition" ), 1, glm::value_ptr( lightPosition ) );
		glUniform3fv( glGetUniformLocation( _program, "uMaterialDiffuse" ), 1, glm::value_ptr( materialDff ) );
		glUniform3fv( glGetUniformLocation( _program, "uMaterialSpecular" ), 1, glm::value_ptr( materialSpc ) );
		glUniform1f( glGetUniformLocation( _program, "uShininess" ), shininess );
		glUniform3fv( glGetUniformLocation( _program, "uLightColor" ), 1, glm::value_ptr( lightColor ) );
		glUniform3fv( glGetUniformLocation( _program, "uCameraPosition" ), 1, glm::value_ptr( _camera.getPos() ) );

		// Rendu du mod le
		_model.render1( _program );
	}

	void LabWork4::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera.moveFront( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_S: // Back
				_camera.moveFront( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_A: // Left
				_camera.moveRight( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_D: // Right
				_camera.moveRight( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_R: // Up
				_camera.moveUp( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera.moveUp( -_cameraSpeed );
				_updateViewMatrix();
				break;
			default: break;
			}
		}

		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
			_updateViewMatrix();
		}
	}
	void LabWork4::displayUI()
	{
		ImGui::Begin( "Settings lab work 4 " );
		ImGui::Text( "No setting available!" );
		// set Field of View
		if ( ImGui::SliderFloat( "Field of View (FOVY)", &_fovy, 1.0f, 120.0f ) )
		{
			_camera.setFovy( _fovy );
		}
		// set Background color.
		if ( ImGui::ColorEdit3( "Background", glm::value_ptr( _bgColor ) ) )
		{
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		}
		//set speed
		if ( ImGui::SliderFloat( "Speed", &_cameraSpeed, 0.1f, 10.f, "%01.1f" ) ) {}

		ImGui::End();
	}

} // namespace M3D_ISICG
