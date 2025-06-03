#include "triangle_mesh.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

namespace M3D_ISICG
{
	TriangleMesh::TriangleMesh( const std::string &				  p_name,
								const std::vector<Vertex> &		  p_vertices,
								const std::vector<unsigned int> & p_indices,
								const Material &				  p_material ) :
		_name( p_name ), _vertices( p_vertices ), _indices( p_indices ), _material( p_material )
	{
		_vertices.shrink_to_fit();
		_indices.shrink_to_fit();
		_setupGL();
	}
	void TriangleMesh::render1( const GLuint p_glProgram ) const
	{
		glUseProgram( p_glProgram ); // Activer le programme shader

		// Passez la couleur ambiante
		GLint ambientLoc = glGetUniformLocation( p_glProgram, "uAmbientColor" );
		glUniform3fv( ambientLoc, 1, glm::value_ptr( _ambientColor ) );

		// Passez la couleur diffuse
		GLint diffuseLoc = glGetUniformLocation( p_glProgram, "uMaterialDiffuse" );
		glUniform3fv( diffuseLoc, 1, glm::value_ptr( _material._diffuse ) );

		// **Ajoutez la couleur sp culaire**
		GLint specularLoc = glGetUniformLocation( p_glProgram, "uMaterialSpecular" );
		glUniform3fv( specularLoc, 1, glm::value_ptr( _material._specular ) );

		// **Passez l'exposant sp culaire (_shininess)**
		GLint shininessLoc = glGetUniformLocation( p_glProgram, "uShininess" );
		glUniform1f( shininessLoc, _material._shininess );

		// Dessinez le maillage
		glBindVertexArray( _vao ); // Activer le VAO
		glDrawElements( GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, nullptr );
		glBindVertexArray( 0 ); // D sactiver le VAO

		glUseProgram( 0 ); // D sactiver le programme shader
	}
	void TriangleMesh::render( const GLuint p_glProgram ) const
	{
		glUseProgram( p_glProgram );

		GLint ambientLoc = glGetUniformLocation( p_glProgram, "uAmbientColor" );
		glProgramUniform3fv( p_glProgram, ambientLoc, 1, glm::value_ptr( _material._ambient ) );

		GLint diffuseLoc = glGetUniformLocation( p_glProgram, "uDiffuseColor" );
		glProgramUniform3fv( p_glProgram, diffuseLoc, 1, glm::value_ptr( _material._diffuse ) );

		GLint specularLoc = glGetUniformLocation( p_glProgram, "uMaterialSpecular" );
		glUniform3fv( specularLoc, 1, glm::value_ptr( _material._specular ) );

		// **Passez l'exposant sp culaire (_shininess)**
		GLint shininessLoc = glGetUniformLocation( p_glProgram, "uShininess" );
		glUniform1f( shininessLoc, _material._shininess );

		glBindVertexArray( _vao );


		GLint uHasAmbientMap   = glGetUniformLocation( p_glProgram, "uHasAmbientMap" );
		GLint uHasDiffuseMap   = glGetUniformLocation( p_glProgram, "uHasDiffuseMap" );
		GLint uHasSpecularMap  = glGetUniformLocation( p_glProgram, "uHasSpecularMap" );
		GLint uHasShininessMap = glGetUniformLocation( p_glProgram, "uHasShininessMap" );
		GLint uHasNormalMap	   = glGetUniformLocation( p_glProgram, "uHasNormalMap" );
		if ( _material._hasAmbientMap == 1 )
		{
			glProgramUniform1i( p_glProgram, uHasAmbientMap, 0 );
			glBindTextureUnit( 0, _material._ambientMap._id );
		}
		else
		{
			glProgramUniform1i( p_glProgram, uHasAmbientMap, 0 );
		}
		if ( _material._hasDiffuseMap == 1 )
		{
			glProgramUniform1i( p_glProgram, uHasDiffuseMap, 1 );
			glBindTextureUnit( 1, _material._diffuseMap._id );
		}
		else
		{
			glProgramUniform1i( p_glProgram, uHasDiffuseMap, 0 );
		}
		if ( _material._hasSpecularMap == 1 )
		{
			glProgramUniform1i( p_glProgram, uHasSpecularMap, 1 );
			glBindTextureUnit( 2, _material._specularMap._id );
		}
		else
		{
			glProgramUniform1i( p_glProgram, uHasSpecularMap, 0 );
		}
		if ( _material._hasShininessMap == 1 )
		{
			glProgramUniform1i( p_glProgram, uHasShininessMap, 1 );
			glBindTextureUnit( 3, _material._shininessMap._id );
		}
		else
		{
			glProgramUniform1i( p_glProgram, uHasShininessMap, 0 );
		}
		if ( _material._hasNormalMap == 1 )
		{
			glProgramUniform1i( p_glProgram, uHasNormalMap, 1 );
			glBindTextureUnit( 4, _material._normalMap._id );
		}
		else
		{
			glProgramUniform1i( p_glProgram, uHasNormalMap, 0 );
		}

		//GLint uSpecularColor = glGetUniformLocation( p_glProgram, "uSpecularColor" );
		//glProgramUniform3fv( p_glProgram, uSpecularColor, 1, glm::value_ptr( _material._specular ) );

		//GLint uShininess = glGetUniformLocation( p_glProgram, "uShininess" );
		//glProgramUniform1f( p_glProgram, uShininess, _material._shininess );

		glDrawElements( GL_TRIANGLES, (GLsizei)( _indices.size() ), GL_UNSIGNED_INT, 0 );

		glBindTextureUnit( 0, 0 );
	}

	void TriangleMesh::renderShadow( const GLuint p_glProgram ) const
	{
		glUseProgram( p_glProgram );
		glBindVertexArray( _vao );

		glDrawElements( GL_TRIANGLES, (GLsizei)( _indices.size() ), GL_UNSIGNED_INT, 0 );

		glBindTextureUnit( 0, 0 );
	}

	void TriangleMesh::cleanGL()
	{
		glDisableVertexArrayAttrib( _vao, 0 );
		glDisableVertexArrayAttrib( _vao, 1 );
		glDisableVertexArrayAttrib( _vao, 2 );
		glDisableVertexArrayAttrib( _vao, 3 );
		glDisableVertexArrayAttrib( _vao, 4 );

		glDeleteVertexArrays( 1, &_vao );

		glDeleteBuffers( 1, &_vbo );
		glDeleteBuffers( 1, &_ebo );
	}

	void TriangleMesh::_setupGL()
	{
		glGenVertexArrays( 1, &_vao );
		glBindVertexArray( _vao );

		glGenBuffers( 1, &_vbo );
		glBindBuffer( GL_ARRAY_BUFFER, _vbo );
		glBufferData( GL_ARRAY_BUFFER, _vertices.size() * sizeof( Vertex ), _vertices.data(), GL_STATIC_DRAW );

		glGenBuffers( 1, &_ebo );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ebo );
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof( unsigned int ), _indices.data(), GL_STATIC_DRAW );

		// Position
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void *)offsetof( Vertex, _position ) );
		glEnableVertexAttribArray( 0 );
		// Normal
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void *)offsetof( Vertex, _normal ) );
		glEnableVertexAttribArray( 1 );
		// Texture Coordinates
		glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void *)offsetof( Vertex, _texCoords ) );
		glEnableVertexAttribArray( 2 );
		// Tangent
		glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void *)offsetof( Vertex, _tangent ) );
		glEnableVertexAttribArray( 3 );
		// Bitangent
		glVertexAttribPointer( 4, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void *)offsetof( Vertex, _bitangent ) );
		glEnableVertexAttribArray( 4 );

		glBindVertexArray( 0 );
	}

} // namespace M3D_ISICG
