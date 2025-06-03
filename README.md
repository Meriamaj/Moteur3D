3D Engine Project

Overview
This project is a 3D rendering engine developed as part of the Master in Computer Science, Image Synthesis, and Graphic Design program at the Université de Limoges. The project focuses on building a 3D engine 
using OpenGL, covering fundamental concepts such as vertex buffers, shaders, 3D transformations, lighting models, texture mapping, and deferred rendering. 

Project Structure
The project is divided into six practical works(TPs), each focusing on a specific aspect of 3D rendering with OpenGL:

TP1 - Getting Started with OpenGL : Objective: Familiarize with OpenGL by rendering a red triangle using vertex and fragment shaders, VBOs (Vertex Buffer Objects), VAOs (Vertex Array Objects), and glDrawArrays.
TP2 - Advancing in 2D : Objective: Optimize a 2D quad mesh using EBOs (Element Buffer Objects) to reduce vertex redundancy and implement horizontal translation and brightness adjustments via ImGui.
Outcome: Rendered an animated 2D quad with user-controlled brightness.
TP3 - 3D Transformations and Projection Objective: Implement 3D transformations (scaling, rotation) and configure the MVP (Model-View-Projection) matrix for a rotating cube with adjustable field of view (FOV) via ImGui.
Outcome: Rendered a rotating 3D cube with dynamic FOV control.
TP4 - Lighting Models : Objective: Implement Phong, Blinn-Phong, and Gaussian lighting models, applied to a 3D bunny model, with toggleable lighting modes via ImGui.
Outcome: Rendered a bunny model with dynamic lighting effects.
TP5 - Discrete Textures : Objective: Apply diffuse and specular textures to the bunny model, implement magnification/minification filters (bilinear interpolation and mipmapping), use normal mapping for surface relief, and manage transparency with depth sorting.
Outcome: Enhanced visual realism with textures, normal maps, and transparency.
TP6 - Deferred Shading and Post Processing : Objective: Implement a deferred rendering pipeline with a Geometry Pass (storing data in a G-Buffer) and a Shading Pass (computing lighting from G-Buffer textures), with an interface to select displayed textures.
Outcome: Efficient rendering pipeline with post-processing capabilities.

This project provided hands-on experience with OpenGL, covering 2D and 3D rendering, transformations, lighting, texture mapping, and deferred shading. It serves as a foundation for understanding real-time graphics
and can be extended with additional features like advanced post-processing.
