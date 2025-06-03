#include "lab_work_6.hpp"
#include "glm/gtx/string_cast.hpp"
#include "imgui.h"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace M3D_ISICG
{

	const std::string LabWork6::_shaderFolder = "src/lab_works/lab_work_6/shaders/";

	LabWork6::~LabWork6()
	{
		glDisableVertexArrayAttrib( _vao, 0 );
		glDeleteVertexArrays( 1, &_vao );
		glDeleteBuffers( 1, &_vbo );
		glDeleteProgram( _shadingPassProg );
		glDeleteProgram( _geometryPassProg );
	}

	bool LabWork6::init()
	{
		_shadingPassProg	 = glCreateProgram();
		_geometryPassProg = glCreateProgram();

		std::cout << "Initializing lab work 6..." << std::endl;

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		_model.load( "Sponza", "./data/models/sponza.obj" );
		_model._transformation = glm::scale( _model._transformation, Vec3f( 0.003, 0.003, 0.003 ) );

	
		const std::string fragShadingPassFile = readFile( _shaderFolder + "shading_pass.frag" );

		const std::string vertGeometryPassFile = readFile( _shaderFolder + "geometry_pass.vert" );
		const std::string fragGeometryPassFile = readFile( _shaderFolder + "geometry_pass.frag" );

		const GLchar * fShadingPassSrc = fragShadingPassFile.c_str();

		const GLchar * vGeometryPassSrc = vertGeometryPassFile.c_str();
		const GLchar * fGeometryPassSrc = fragGeometryPassFile.c_str();

		GLuint fragShadingPassShader = glCreateShader( GL_FRAGMENT_SHADER );

		GLuint vertGeometryPassShader = glCreateShader( GL_VERTEX_SHADER );
		GLuint fragGeometryPassShader = glCreateShader( GL_FRAGMENT_SHADER );

		glShaderSource( fragShadingPassShader, 1, &fShadingPassSrc, NULL );

		glShaderSource( vertGeometryPassShader, 1, &vGeometryPassSrc, NULL );
		glShaderSource( fragGeometryPassShader, 1, &fGeometryPassSrc, NULL );

		glCompileShader( fragShadingPassShader );

		glCompileShader( vertGeometryPassShader );
		glCompileShader( fragGeometryPassShader );

		// Check for errors in compilation
		GLint compiled;
		glGetShaderiv( fragShadingPassShader, GL_COMPILE_STATUS, &compiled );
		if ( compiled == GL_FALSE )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( fragShadingPassShader, sizeof( log ), NULL, log );
			glDeleteShader( fragShadingPassShader );
			std ::cerr << " Error compiling fragment shader : " << log << std ::endl;
			return false;
		}

		glGetShaderiv( fragGeometryPassShader, GL_COMPILE_STATUS, &compiled );
		if ( compiled == GL_FALSE )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( fragGeometryPassShader, sizeof( log ), NULL, log );
			glDeleteShader( fragGeometryPassShader );
			glDeleteShader( vertGeometryPassShader );
			std ::cerr << " Error compiling fragment shader : " << log << std ::endl;
			return false;
		}
		glGetShaderiv( vertGeometryPassShader, GL_COMPILE_STATUS, &compiled );
		if ( compiled == GL_FALSE )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertGeometryPassShader, sizeof( log ), NULL, log );
			glDeleteShader( fragGeometryPassShader );
			glDeleteShader( vertGeometryPassShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

		glAttachShader( _shadingPassProg, fragShadingPassShader );

		glAttachShader( _geometryPassProg, vertGeometryPassShader );
		glAttachShader( _geometryPassProg, fragGeometryPassShader );

		glLinkProgram( _shadingPassProg );
		// Check if link is ok
		GLint linked;
		glGetProgramiv( _shadingPassProg, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _shadingPassProg, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		glDeleteShader( fragShadingPassShader );

		glLinkProgram( _geometryPassProg );
		// Check if link is ok
		glGetProgramiv( _geometryPassProg, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _geometryPassProg, sizeof( log ), NULL, log );
			std ::cerr << " Error linking geometry pass program : " << log << std ::endl;
			return false;
		}

		glDeleteShader( vertGeometryPassShader );
		glDeleteShader( fragGeometryPassShader );

		glEnable( GL_DEPTH_TEST );

		_initCamera();

		if ( !_initGeometryPass() )
			return false;

		if ( !_initShadingPass() )
			return false;

		if ( !_initGBuffer() )
			return false;

		std::cout << "Done!" << std::endl;
		return true;
	}

	bool LabWork6::_initGeometryPass()
	{
		_uMVP		   = glGetUniformLocation( _geometryPassProg, "uMVPMatrix" );
		_uMV		   = glGetUniformLocation( _geometryPassProg, "uMVMatrix" );
		_uV			   = glGetUniformLocation( _geometryPassProg, "uVMatrix" );
		_uNormalMatrix = glGetUniformLocation( _geometryPassProg, "uNormalMatrix" );

		return true;
	}

	bool LabWork6::_initGBuffer()
	{
		_fboTaille[ 0 ] = this->getWindowWidth();
		_fboTaille[ 1 ] = this->getWindowHeight();

		glCreateFramebuffers( 1, &_fbo );

		// create textures
		glCreateTextures( GL_TEXTURE_2D, 6, _gBufferTextures );

		glTextureStorage2D( _gBufferTextures[ 0 ], 1, GL_RGB32F, this->getWindowWidth(), this->getWindowHeight() );
		glTextureStorage2D( _gBufferTextures[ 1 ], 1, GL_RGB32F, this->getWindowWidth(), this->getWindowHeight() );
		glTextureStorage2D( _gBufferTextures[ 2 ], 1, GL_RGB32F, this->getWindowWidth(), this->getWindowHeight() );
		glTextureStorage2D( _gBufferTextures[ 3 ], 1, GL_RGB32F, this->getWindowWidth(), this->getWindowHeight() );
		glTextureStorage2D( _gBufferTextures[ 4 ], 1, GL_RGBA32F, this->getWindowWidth(), this->getWindowHeight() );
		glTextureStorage2D(
			_gBufferTextures[ 5 ], 1, GL_DEPTH_COMPONENT32F, this->getWindowWidth(), this->getWindowHeight() );

		// bind textures to fbo
		glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _gBufferTextures[ 0 ], 0 );
		glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT1, _gBufferTextures[ 1 ], 0 );
		glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT2, _gBufferTextures[ 2 ], 0 );
		glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT3, _gBufferTextures[ 3 ], 0 );
		glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT4, _gBufferTextures[ 4 ], 0 );
		glNamedFramebufferTexture( _fbo, GL_DEPTH_ATTACHMENT, _gBufferTextures[ 5 ], 0 );

		GLenum buffers[] = {
			GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4
		};

		glNamedFramebufferDrawBuffers( _fbo, 5, buffers );

		

		return true;
	}

	bool LabWork6::_initShadingPass()
	{
		_uLightPosition = glGetUniformLocation( _shadingPassProg, "uLight" );
		_uSpecularType	= glGetUniformLocation( _shadingPassProg, "uSpecularType" );

		_lightPosition = Vec3f( 1, 1, 1 );
		glProgramUniform3fv( _shadingPassProg,
							 _uLightPosition,
							 1,
							 glm::value_ptr( Vec3f( _camera.getViewMatrix() * Vec4f( _lightPosition, 1. ) ) ) );

		glProgramUniform1i( _shadingPassProg, _uSpecularType, _specularTy );

		// create the quad
		_points.push_back( Vec2f( -1, -1 ) );
		_points.push_back( Vec2f( -1, 1 ) );
		_points.push_back( Vec2f( 1, 1 ) );
		_points.push_back( Vec2f( 1, -1 ) );

		_triangle.push_back( Vec3i( 0, 1, 2 ) );
		_triangle.push_back( Vec3i( 0, 2, 3 ) );

		glCreateBuffers( 1, &_vbo );
		glNamedBufferData( _vbo, _points.size() * sizeof( Vec2f ), _points.data(), GL_STATIC_DRAW );

		// Create VAO
		glCreateVertexArrays( 1, &_vao );
		glEnableVertexArrayAttrib( _vao, 0 ); // Enable attribute at index 0

		glVertexArrayAttribFormat(
			_vao, 0, 2, GL_FLOAT, GL_FALSE, 0 ); // Set attrib 0 as floats with 2 space between each object

		glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( Vec2f ) );
		glVertexArrayAttribBinding( _vao, 0, 0 ); // Set to the attr 0 of the vert Shader the attr 0 of the vao

		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _ebo, _triangle.size() * sizeof( Vec3i ), _triangle.data(), GL_STATIC_DRAW );

		glVertexArrayElementBuffer( _vao, _ebo );

		return true;
	}

	bool LabWork6::_initShaders()
	{
		const std::string fragShadingPassFile = readFile( _shaderFolder + "shading_pass.frag" );

		const std::string vertGeometryPassFile = readFile( _shaderFolder + "geometry_pass.vert" );
		const std::string fragGeometryPassFile = readFile( _shaderFolder + "geometry_pass.frag" );

		const GLchar * fShadingPassSrc = fragShadingPassFile.c_str();

		const GLchar * vGeometryPassSrc = vertGeometryPassFile.c_str();
		const GLchar * fGeometryPassSrc = fragGeometryPassFile.c_str();

		GLuint fragShadingPassShader = glCreateShader( GL_FRAGMENT_SHADER );

		GLuint vertGeometryPassShader = glCreateShader( GL_VERTEX_SHADER );
		GLuint fragGeometryPassShader = glCreateShader( GL_FRAGMENT_SHADER );

		glShaderSource( fragShadingPassShader, 1, &fShadingPassSrc, NULL );

		glShaderSource( vertGeometryPassShader, 1, &vGeometryPassSrc, NULL );
		glShaderSource( fragGeometryPassShader, 1, &fGeometryPassSrc, NULL );

		glCompileShader( fragShadingPassShader );

		glCompileShader( vertGeometryPassShader );
		glCompileShader( fragGeometryPassShader );

		// Check for errors in compilation
		GLint compiled;
		glGetShaderiv( fragShadingPassShader, GL_COMPILE_STATUS, &compiled );
		if ( compiled == GL_FALSE )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( fragShadingPassShader, sizeof( log ), NULL, log );
			glDeleteShader( fragShadingPassShader );
			std ::cerr << " Error compiling fragment shader : " << log << std ::endl;
			return false;
		}

		glGetShaderiv( fragGeometryPassShader, GL_COMPILE_STATUS, &compiled );
		if ( compiled == GL_FALSE )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( fragGeometryPassShader, sizeof( log ), NULL, log );
			glDeleteShader( fragGeometryPassShader );
			glDeleteShader( vertGeometryPassShader );
			std ::cerr << " Error compiling fragment shader : " << log << std ::endl;
			return false;
		}
		glGetShaderiv( vertGeometryPassShader, GL_COMPILE_STATUS, &compiled );
		if ( compiled == GL_FALSE )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertGeometryPassShader, sizeof( log ), NULL, log );
			glDeleteShader( fragGeometryPassShader );
			glDeleteShader( vertGeometryPassShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

		glAttachShader( _shadingPassProg, fragShadingPassShader );

		glAttachShader( _geometryPassProg, vertGeometryPassShader );
		glAttachShader( _geometryPassProg, fragGeometryPassShader );

		glLinkProgram( _shadingPassProg );
		// Check if link is ok
		GLint linked;
		glGetProgramiv( _shadingPassProg, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _shadingPassProg, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		glDeleteShader( fragShadingPassShader );

		glLinkProgram( _geometryPassProg );
		// Check if link is ok
		glGetProgramiv( _geometryPassProg, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _geometryPassProg, sizeof( log ), NULL, log );
			std ::cerr << " Error linking geometry pass program : " << log << std ::endl;
			return false;
		}
		glProgramUniform3fv( _shadingPassProg, _uLightPosition, 1, glm::value_ptr( _lightPosition ) );

		glDeleteShader( vertGeometryPassShader );
		glDeleteShader( fragGeometryPassShader );

		return true;
	}

	void LabWork6::_updateShaders()
	{
		// now delete programs
		glDeleteProgram( _shadingPassProg );
		glDeleteProgram( _geometryPassProg );

		_shadingPassProg	 = glCreateProgram();
		_geometryPassProg = glCreateProgram();

		_initShaders();

		glProgramUniform3fv( _geometryPassProg, _uLightPosition, 1, glm::value_ptr( _lightPosition ) );
		glProgramUniform1i( _geometryPassProg, _uSpecularType, _specularTy );
	}

	void LabWork6::animate( const float p_deltaTime )
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
		if ( s)
			_camera.moveUp( _cameraSpeed * multipl * p_deltaTime );
		if ( ls )
			_camera.moveUp( -_cameraSpeed * multipl * p_deltaTime );
	}

	void LabWork6::render()
	{
		// do the geometry pass, then the shading pass
		glEnable( GL_DEPTH_TEST ); 

		// bind fbo
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, _fbo );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		_V			  = _camera.getViewMatrix();
		_MV			  = _camera.getViewMatrix() * _model._transformation;
		_MVP		  = _camera.getProjectionMatrix() * _MV;
		_NormalMatrix = glm::transpose( glm::inverse( _MV ) );

		glProgramUniformMatrix4fv( _geometryPassProg, _uV, 1, false, glm::value_ptr( _V ) );
		glProgramUniformMatrix4fv( _geometryPassProg, _uMV, 1, false, glm::value_ptr( _MV ) );
		glProgramUniformMatrix4fv( _geometryPassProg, _uMVP, 1, false, glm::value_ptr( _MVP ) );
		glProgramUniformMatrix4fv( _geometryPassProg, _uNormalMatrix, 1, false, glm::value_ptr( _NormalMatrix ) );

		_model.render( _geometryPassProg );

		// fbo no need
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

		_lightPosition = _camera.getPos();
		glProgramUniform3fv( _shadingPassProg, _uLightPosition, 1, glm::value_ptr( _lightPosition ) );
		
		//shading pass
		if ( _renderGBuff )
		{
			glNamedFramebufferReadBuffer( _fbo, _bufferTypes[ _sBuffer ] );
			glBlitNamedFramebuffer( _fbo,
									0,
									0,
									0,
									getWindowWidth(),
									getWindowHeight(),
									0,
									0,
									getWindowWidth(),
									getWindowHeight(),
									GL_COLOR_BUFFER_BIT,
									GL_NEAREST );
		}
		else
		{
			glUseProgram( _shadingPassProg );

			// bind default fbo
			glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
			glBindVertexArray( _vao );
			//disabling depth
			glDisable( GL_DEPTH_TEST );
			glClear( GL_COLOR_BUFFER_BIT );
			//bindingtextures
			glBindTextureUnit( 0, _gBufferTextures[ 0 ] );
			glBindTextureUnit( 1, _gBufferTextures[ 1 ] );
			glBindTextureUnit( 2, _gBufferTextures[ 2 ] );
			glBindTextureUnit( 3, _gBufferTextures[ 3 ] );
			glBindTextureUnit( 4, _gBufferTextures[ 4 ] );

			Vec3f lightPs = Vec3f( _camera.getViewMatrix() * Vec4f( _lightPosition, 1. ) );

			glProgramUniform3fv( _shadingPassProg, _uLightPosition, 1, glm::value_ptr( lightPs ) );

			// then draw the quad
			glDrawElements( GL_TRIANGLES, (GLsizei)( 3 * _triangle.size() ), GL_UNSIGNED_INT, 0 );
		}
	}




	void LabWork6::handleEvents( const SDL_Event & p_event )
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
				sp = p_event.type == SDL_KEYDOWN ? true : false;
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

	void LabWork6::displayUI()
	{
		ImGui::Begin( "Settings lab work 6" );

	if ( ImGui::SliderFloat( "Field of View (FOVY)", &_fovy, 1.0f, 120.0f ) )
		{
			_camera.setFovy( _fovy );
		}
	if ( ImGui::DragFloat3( "Light position", glm::value_ptr( _lightPosition ), 0.05f, -10, 10, "%.2f", 1 ) )
		{
			glProgramUniform3fv( _shadingPassProg, _uLightPosition, 1, glm::value_ptr( _lightPosition ) );
		}

		Vec3f camPos = _camera.getPos();
		if ( ImGui::DragFloat3( "Camera position", glm::value_ptr( camPos ), 0.05f, -10.f, 10.f, "%.2f", 1 ) )
		{
			_camera.setPosition( camPos );
		}
		//choosing lighting method
		blinnphong();
		phong();
		none();
		//choosing between6 textures
		if ( ImGui::RadioButton( "choose only one texture ", _renderGBuff ) )
		{
			_renderGBuff = !_renderGBuff;
		}
		if ( _renderGBuff )
		{
			std::string text = "Rendered buffer \n( ";
			text.append( _bufferTypeNom[ _sBuffer ] );
			text.append( " )" );

			ImGui::ListBox( text.c_str(), &_sBuffer, _bufferTypeNom, 5 );
		}


		ImGui::End();
	}

	void LabWork6::none()
	{
		if ( ImGui::RadioButton( "None", _specularTy == 0 ) )
		{
			_specularTy = 0;
			glProgramUniform1i( _shadingPassProg, _uSpecularType, _specularTy );
		}
	}

	void LabWork6::blinnphong()
	{
		if ( ImGui::RadioButton( "Blinn-Phong", _specularTy == 2 ) )
		{
			_specularTy = 2;
			glProgramUniform1i( _shadingPassProg, _uSpecularType, _specularTy );
		}
	}

	void LabWork6::phong()
	{
		if ( ImGui::RadioButton( "Phong", _specularTy == 1 ) )
		{
			_specularTy = 1;
			glProgramUniform1i( _shadingPassProg, _uSpecularType, _specularTy );
		}
	}

	void LabWork6::_initCamera()
	{
		_camera.setPosition( Vec3f( -2.5, 0.75, 0 ) );
		_camera.setLookAt( Vec3f( 0, 0.75, 0 ) );
		_camera.setFovy( 60 );
		_camera.print();
	}

	

} // namespace M3D_ISICG
