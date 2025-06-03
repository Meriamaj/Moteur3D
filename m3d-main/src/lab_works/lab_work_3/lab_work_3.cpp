#include "lab_work_3.hpp"
#include "common/camera.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork3::_shaderFolder = "src/lab_works/lab_work_3/shaders/";

	LabWork3::~LabWork3()
	{
		glDeleteBuffers( 1, &_cube.vbo );
		glDeleteBuffers( 1, &_cube.ebo );
		glDeleteBuffers( 1, &_cube.colorBuffer );

		glDeleteVertexArrays( 1, &_cube.vao );
	}

	bool LabWork3::init()
	{
		_createCube();
		_initBuffers();

		// Lire les shaders
		const std::string vertexShaderStr	= readFile( _shaderFolder + "lw3.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "lw3.frag" );

		if ( vertexShaderStr.empty() || fragmentShaderStr.empty() )
		{
			std::cerr << "Erreur: impossible de lire les shaders." << std::endl;
			return false;
		}

		// Créer les objets shaders
		GLuint vertexShader	  = glCreateShader( GL_VERTEX_SHADER );
		GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		// Associer les chaînes de caractères aux objets shaders
		const GLchar * vSrc = vertexShaderStr.c_str();
		const GLchar * fSrc = fragmentShaderStr.c_str();

		glShaderSource( vertexShader, 1, &vSrc, nullptr );
		glShaderSource( fragmentShader, 1, &fSrc, nullptr );

		// Compiler les shaders
		glCompileShader( vertexShader );
		glCompileShader( fragmentShader );

		// Vérification des erreurs de compilation
		GLint success;
		glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &success );
		if ( !success )
		{
			char log[ 512 ];
			glGetShaderInfoLog( vertexShader, 512, nullptr, log );
			std::cerr << "Erreur de compilation du vertex shader: " << log << std::endl;
			return false;
		}

		glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &success );
		if ( !success )
		{
			char log[ 512 ];
			glGetShaderInfoLog( fragmentShader, 512, nullptr, log );
			std::cerr << "Erreur de compilation du fragment shader: " << log << std::endl;
			return false;
		}

		// Créer le programme
		_program = glCreateProgram();
		glAttachShader( _program, vertexShader );
		glAttachShader( _program, fragmentShader );
		glLinkProgram( _program );

		// Vérification du linking
		glGetProgramiv( _program, GL_LINK_STATUS, &success );
		if ( !success )
		{
			char log[ 512 ];
			glGetProgramInfoLog( _program, 512, nullptr, log );
			std::cerr << "Erreur de linkage du programme: " << log << std::endl;
			return false;
		}

		// Supprimer les shaders après le linking
		glDeleteShader( vertexShader );
		glDeleteShader( fragmentShader );

		// Utiliser le programme et récupérer les uniform locations
		glUseProgram( _program );

		_modelMatrixLoc = glGetUniformLocation( _program, "uModelMatrix" );

		if ( _modelMatrixLoc == -1 )
		{
			std::cerr << "Erreur: impossible de trouver l'uniform 'uModelMatrix'." << std::endl;
			return false;
		}
		
		_viewMatrixLoc = glGetUniformLocation( _program, "uViewMatrix" );
		if ( _viewMatrixLoc == -1 )
		{
			std::cerr << "Erreur: impossible de trouver l'uniform 'uViewMatrix'." << std::endl;
			return false;
		}

		_projMatrixLoc = glGetUniformLocation( _program, "uProjMatrix" );

		if ( _projMatrixLoc == -1 )
		{
			std::cerr << "Erreur: impossible de trouver l'uniform 'uProjMatrix'." << std::endl;
			return false;
		}

		_cube.transform = glm::scale( Mat4f( 1.0f ), Vec3f( 0.8f ) );
		_initCamera();
		_updateViewMatrix();
		_updateProjectionMatrix();

		glEnable( GL_DEPTH_TEST );
		return true;
	}

	void LabWork3::_createCube()
	{
		// Positions des sommets
		_cube.positions = {
			{ -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f },
			{ -0.5f, -0.5f, 0.5f },	 { 0.5f, -0.5f, 0.5f },	 { 0.5f, 0.5f, 0.5f },	{ -0.5f, 0.5f, 0.5f },
		};
		for ( int i = 0; i < 8; ++i )
		{
			_cube.colors.push_back( Vec3f( getRandomFloat(), getRandomFloat(), getRandomFloat() ) );
		}

		_cube.indices = {
			0, 1, 2, 2, 3, 0, 
			4, 5, 6, 6, 7, 4, 
			0, 4, 7, 7, 3, 0, 
			1, 5, 6, 6, 2, 1, 
			3, 7, 6, 6, 2, 3, 
			0, 4, 5, 5, 1, 0, 
		};
		_cube.transform = glm::scale( Mat4f( 1.0f ), Vec3f( 0.8f ) );
	}
	void LabWork3::_initBuffers()
	{
		// Création des buffers
		glGenBuffers( 1, &_cube.vbo );
		glGenBuffers( 1, &_cube.ebo );
		glGenVertexArrays( 1, &_cube.vao );
		glBindVertexArray( _cube.vao );

		// Buffer des positions
		glBindBuffer( GL_ARRAY_BUFFER, _cube.vbo );
		glBufferData(
			GL_ARRAY_BUFFER, _cube.positions.size() * sizeof( Vec3f ), _cube.positions.data(), GL_STATIC_DRAW );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), nullptr );
		glEnableVertexAttribArray( 0 );
		// Buffer des couleurs
		GLuint colorBuffer;
		glGenBuffers( 1, &colorBuffer );
		glBindBuffer( GL_ARRAY_BUFFER, colorBuffer );
		glBufferData( GL_ARRAY_BUFFER, _cube.colors.size() * sizeof( Vec3f ), _cube.colors.data(), GL_STATIC_DRAW );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), nullptr );
		glEnableVertexAttribArray( 1 );

		// Buffer des indices
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _cube.ebo );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER,
					  _cube.indices.size() * sizeof( unsigned int ),
					  _cube.indices.data(),
					  GL_STATIC_DRAW );

		glBindVertexArray( 0 );
	}
	void LabWork3::animate( const float p_deltaTime )
	{
		float rotationspeed = 50.0f;
		Mat4f rotationMatrix
			= glm::rotate( Mat4f( 1.0f ), glm::radians( p_deltaTime * rotationspeed ), glm::vec3( 0.f, 1.f, 1.f ) );
		_cube.transform = rotationMatrix * _cube.transform;
		glUseProgram( _program );

		glProgramUniformMatrix4fv( _program, _modelMatrixLoc, 1, GL_FALSE, glm::value_ptr( _cube.transform ) );
	}

	void LabWork3::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glUseProgram( _program ); // Active le programme avant de définir les uniforms

		// Passe les matrices uniformes
		glUniformMatrix4fv( _modelMatrixLoc, 1, GL_FALSE, glm::value_ptr( _cube.transform ) );
		glUniformMatrix4fv( _viewMatrixLoc, 1, GL_FALSE, glm::value_ptr( _camera.getViewMatrix() ) );
		glUniformMatrix4fv( _projMatrixLoc, 1, GL_FALSE, glm::value_ptr( _camera.getProjectionMatrix() ) );

		// Rendu
		glBindVertexArray( _cube.vao );
		glDrawElements( GL_TRIANGLES, _cube.indices.size(), GL_UNSIGNED_INT, nullptr );
		glBindVertexArray( 0 );
	}

	void LabWork3::_updateViewMatrix()
	{
		glUseProgram( _program );

		glProgramUniformMatrix4fv( _program,
								   glGetUniformLocation( _program, "uViewMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( _camera.getViewMatrix() ) );
	}

	void LabWork3::_updateProjectionMatrix()
	{
		glUseProgram( _program );

		glProgramUniformMatrix4fv( _program,
								   glGetUniformLocation( _program, "uProjectionMatrix" ),
								   1,
								   GL_FALSE,
								   glm::value_ptr( _camera.getProjectionMatrix() ) );
	}
	void LabWork3::_initCamera()
	{
		// Placer la caméra en position (0, 1, 3)
		_camera.setPosition( Vec3f( 0.f, 1.f, 3.f ) );

		// Initialiser les dimensions de l'écran 
		_camera.setScreenSize( 1280, 720 );

		// (Optionnel) Si nécessaire, définir la cible de la caméra avec setLookAt
		 _camera.setLookAt(Vec3f(0.f, 0.f, 0.f)); // Regarde vers l'origine
	}

	
	void checkGLError( const std::string & location )
	{
		GLenum err;
		while ( ( err = glGetError() ) != GL_NO_ERROR )
		{
			std::cerr << "OpenGL Error at " << location << ": " << err << std::endl;
		}
	}

	void LabWork3::handleEvents( const SDL_Event & p_event )
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
			_camera.rotate( p_event.motion.xrel * _cameraSens, p_event.motion.yrel * _cameraSens );
			_updateViewMatrix();
		}
	}
	void LabWork3::displayUI()
	{
		ImGui::Begin( "Settings lab work 3" );
		// ImGui::Text( "No setting available!" );
		// 
		// Met à jour la caméra si l'utilisateur change la valeur de fovy.
		if ( ImGui::SliderFloat( "Field of View ", &_fovy, 1.0f, 120.0f ) )
		{
			_camera.setFovy( _fovy );
		}

		ImGui::End();
	}

} // namespace M3D_ISICG
