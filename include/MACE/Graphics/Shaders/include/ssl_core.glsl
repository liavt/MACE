R"(

#ifndef _SSL_CORE_LIBRARY_
#define _SSL_CORE_LIBRARY_ 1

SSL_GL_VERSION_DECLARATION

#ifndef GL_VERSION_3_3
#extension GL_ARB_shader_bit_encoding : require
#extension GL_ARB_explicit_attrib_location : require
#endif //GL_VERSION_3_3

#ifndef GL_VERSION_3_2
#extension GL_ARB_fragment_coord_conventions : require
#extension GL_ARB_texture_multisample : require
#extension GL_ARB_geometry_shader4 : require
#endif //GL_VERSION_3_2

#ifndef GL_VERSION_3_1
#extension GL_ARB_uniform_buffer_object : require
#extension GL_ARB_draw_instanced : require
#extension GL_ARB_texture_rectangle : require
#endif //GL_VERSION_3_1

#ifndef GL_VERSION_3_0
#extension GL_EXT_texture_array : require
#endif //GL_VERSION_3_0

#define sslUniformBuffer layout(std140) uniform

#endif //_SSL_CORE_LIBRARY_

)"