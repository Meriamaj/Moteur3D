#include "lab_work_2.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork2::_shaderFolder = "src/lab_works/lab_work_2/shaders/";

	LabWork2::~LabWork2()
	{
		if ( _program != GL_INVALID_INDEX )
		{
			glDeleteProgram( _program );
		}

		if ( _vao != GL_INVALID_INDEX )
		{
			glDisableVertexArrayAttrib( _vao, 0 );
			glDeleteVertexArrays( 1, &_vao );
		}

		if ( _vbo != GL_INVALID_INDEX )
		{
			glDeleteBuffers( 1, &_vbo );
		}

		if ( _ebo != GL_INVALID_INDEX ) // Ajout de la suppression de l'EBO
		{
			glDeleteBuffers( 1, &_ebo );
		}
	}

	bool LabWork2::init()
	{
		// Définir la couleur de fond
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// Lire les shaders
		const std::string vertexShaderStr	= readFile( _shaderFolder + "lw2.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "lw2.frag" );

		// Créer les objets shaders
		GLuint vertexShader	  = glCreateShader( GL_VERTEX_SHADER );
		GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		// Associer les chaînes de caractères aux objets shaders
		const GLchar * vSrc = vertexShaderStr.c_str();
		const GLchar * fSrc = fragmentShaderStr.c_str();

		glShaderSource( vertexShader, 1, &vSrc, NULL );
		glShaderSource( fragmentShader, 1, &fSrc, NULL );

		glCompileShader( vertexShader );
		glCompileShader( fragmentShader );
		
		// Initialisation des sommets
		_triangleVer = {
			Vec2f( -0.5f, 0.5f ), 
			Vec2f( 0.5f, 0.5f ),  
			Vec2f( 0.5f, -0.5f ), 
			Vec2f( -0.5f, -0.5f ) 
		};

		// Indices pour dessiner le quad (deux triangles)
		_indicesTri = {
			0, 1, 2, 
			0, 2, 3	 
		};

		// Initialisation des couleurs des sommets
		_triangleCol = {
			Vec3f( 1.0f, 0.0f, 0.0f ), // Rouge
			Vec3f( 0.0f, 1.0f, 0.0f ), // Vert
			Vec3f( 0.0f, 0.0f, 1.0f ), // Bleu
			Vec3f( 1.0f, 0.0f, 1.0f )  // Magenta
		};

		// Création du VBO pour les sommets
		glCreateBuffers( 1, &_vbo );
		glNamedBufferData( _vbo, _triangleVer.size() * sizeof( Vec2f ), _triangleVer.data(), GL_STATIC_DRAW );

		// Création du VBO pour les couleurs
		GLuint colorVBO; // VBO pour les couleurs
		glCreateBuffers( 1, &colorVBO );
		glNamedBufferData( colorVBO, _triangleCol.size() * sizeof( Vec3f ), _triangleCol.data(), GL_STATIC_DRAW );

		// Création du VAO
		glCreateVertexArrays( 1, &_vao );

		// Lier le VBO des sommets
		glEnableVertexArrayAttrib( _vao, 0 );
		glVertexArrayAttribFormat( _vao, 0, 2, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( Vec2f ) );
		glVertexArrayAttribBinding( _vao, 0, 0 );

		// Lier le VBO des couleurs (index 1)
		glEnableVertexArrayAttrib( _vao, 1 );
		glVertexArrayAttribFormat( _vao, 1, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( _vao, 1, colorVBO, 0, sizeof( Vec3f ) );
		glVertexArrayAttribBinding( _vao, 1, 1 );

		// Création de l'EBO
		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _ebo, _indicesTri.size() * sizeof( GLuint ), _indicesTri.data(), GL_STATIC_DRAW );

		// Lier l'EBO au VAO
		glVertexArrayElementBuffer( _vao, _ebo );

		GLint compiled;
		glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertexShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std::cerr << "Error compiling vertex shader: " << log << std::endl;
			return false;
		}
		_program = glCreateProgram();

		// Attacher les shaders au programme
		glAttachShader( _program, vertexShader );
		glAttachShader( _program, fragmentShader );
		// ====================================================================

		// Lier le programme
		glLinkProgram( _program );

		// Vérification du linking
		GLint linked;
		glGetProgramiv( _program, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _program, sizeof( log ), NULL, log );
			std::cerr << "Error linking program: " << log << std::endl;
			return false;
		}
		glUseProgram( _program );
		// Supprimer les shaders après le linking
		glDeleteShader( vertexShader );
		glDeleteShader( fragmentShader );

		// ====================================================================

		// Récupérer l'emplacement de la variable uniform
		translationXLoc = glGetUniformLocation( _program, "uTranslationX" );

		glProgramUniform1f( _program, translationXLoc, 0.5f );

		if ( translationXLoc == -1 )
		{
			std::cerr << "Erreur : Impossible de trouver la variable uniform 'uTranslationX'" << std::endl;
		}
		// ====================================================================
		//linking brightness
		GLint brightnessLocation = glGetUniformLocation( _program, "Ubright" );
		if ( brightnessLocation != -1 )
		{
			glProgramUniform1f( _program, brightnessLocation, _brightness );
		}
		else
		{
			std::cerr << "Error: Unable to find uniform 'Ubright' in the shader!" << std::endl;
		}

		return true;
	}

	void LabWork2::animate( const float p_deltaTime )
	{
		
		_time += p_deltaTime;
		float trans = 0.5f * glm::sin( _time );
		glProgramUniform1f( _program, translationXLoc, trans );
	}

	void LabWork2::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glUseProgram( _program );

		animate( 12.5 );

		glBindVertexArray( _vao );

		// Dessine le quad avec les indices
		glDrawElements( GL_TRIANGLES, _indicesTri.size(), GL_UNSIGNED_INT, 0 );

		glBindVertexArray( 0 );
	}

	void LabWork2::handleEvents( const SDL_Event & p_event ) {}

	void LabWork2::displayUI()
	{
		ImGui::Begin( "Settings lab work 2" );

		if ( ImGui::SliderFloat( "Brightness", &_brightness, 0.0f, 1.0f ) ) 
		{
			// Maj si elle a été modifiée
			glProgramUniform1f( _program, glGetUniformLocation( _program, "Ubright" ), _brightness );
		}
		if ( ImGui::ColorEdit3( "Background Color", glm::value_ptr( _bgColor ) ) ) 
		{
			// Met à jour la couleur de fond si modifiée
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, 1.0f ); 
		}


		ImGui::End();
	}

} // namespace M3D_ISICG
