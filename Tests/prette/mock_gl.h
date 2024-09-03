#ifndef PRT_MOCK_GLFW_H
#define PRT_MOCK_GLFW_H

#include <gmock/gmock.h>
#include "prette/gfx.h"

namespace prt {
  class MockGl {
  public:
    MockGl();
    ~MockGl();
    // draw
    MOCK_METHOD3(glDrawArrays, void(GLenum, GLint, GLsizei));
    MOCK_METHOD4(glDrawElements, void(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices));

    // vertex arrays
    MOCK_METHOD1(glBindVertexArray, void(GLuint));
    MOCK_METHOD2(glGenVertexArrays, void(GLsizei, GLuint*));
    MOCK_METHOD1(glEnableVertexAttribArray, void(GLuint index));
    MOCK_METHOD2(glGetAttribLocation, GLint(GLuint program, const GLchar* name));
    MOCK_METHOD2(glDeleteVertexArrays, void((GLsizei n, const GLuint* arrays)));

    // textures
    MOCK_METHOD1(glActiveTexture, void(GLenum target));
    MOCK_METHOD2(glBindTexture, void(GLenum target, GLuint texture));
    MOCK_METHOD2(glGenTextures, void(GLsizei n, GLuint* data));
    MOCK_METHOD3(glTexParameteri, void(GLenum target, GLenum pname, GLint param));
    MOCK_METHOD3(glTexParameterIiv, void(GLenum target, GLenum pname, const GLint *params));
    MOCK_METHOD3(glGetTexParameteriv, void(GLenum target, GLenum pname, GLint* param));
    MOCK_METHOD9(glTexImage2D, void(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels));
    MOCK_METHOD2(glDeleteTextures, void(GLsizei n, const GLuint* arrays));

    // shaders
    MOCK_METHOD2(glAttachShader, void(GLuint program, GLuint shader));
    MOCK_METHOD2(glDetachShader, void(GLuint program, GLuint shader));
    MOCK_METHOD7(glGetActiveUniform, void(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name));
    MOCK_METHOD7(glGetActiveAttrib, void(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name));
    MOCK_METHOD1(glCompileShader, void(GLuint shader));
    MOCK_METHOD0(glCreateProgram, GLuint());
    MOCK_METHOD1(glCreateShader, GLuint(GLenum type));
    MOCK_METHOD1(glDeleteShader, void(GLuint shader));
    MOCK_METHOD1(glDeleteProgram, void(GLuint program));
    MOCK_METHOD4(glGetProgramInfoLog, void(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog));
    MOCK_METHOD3(glGetShaderiv, void(GLuint shader, GLenum pname, GLint *params));
    MOCK_METHOD3(glGetProgramiv, void(GLuint program, GLenum pname, GLint *params));
    MOCK_METHOD4(glGetShaderInfoLog, void(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog));
    MOCK_METHOD2(glGetUniformLocation, GLint(GLuint program, const GLchar *name));
    MOCK_METHOD1(glLinkProgram, void(GLuint program));
    MOCK_METHOD4(glUniformMatrix4fv, void(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value));
    MOCK_METHOD1(glUseProgram, void(GLuint program));
    MOCK_METHOD6(glVertexAttribPointer, void(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer));
    MOCK_METHOD4(glShaderSource, void(GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length));
    MOCK_METHOD3(glUniform4fv, void(GLint location, GLsizei count, const GLfloat *value));
    MOCK_METHOD3(glUniform3fv, void(GLint location, GLsizei count, const GLfloat *value));
    MOCK_METHOD2(glUniform1i, void(GLint location, GLint v0));
    MOCK_METHOD2(glUniform1f, void(GLint location, GLfloat v0));

    // buffers
    MOCK_METHOD2(glGenBuffers, void(GLsizei n, GLuint* buffers));
    MOCK_METHOD2(glBindBuffer, void(GLenum target , GLuint buffer));
    MOCK_METHOD4(glBufferData, void(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage));
    MOCK_METHOD2(glDeleteBuffers, void(GLsizei n, const GLuint* buffers));
    MOCK_METHOD3(glBindBufferBase, void(GLenum target, GLuint index, GLuint buffer));
    
    // framebuffers
    MOCK_METHOD2(glBindFramebuffer, void(GLenum, GLuint));
    MOCK_METHOD2(glGenFramebuffers, void(GLsizei n, GLuint* framebuffers));
    MOCK_METHOD5(glFramebufferTexture2D, void(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level));

    // misc
    MOCK_METHOD2(glBlendFunc, void(GLenum sfactor, GLenum dfactor));
    MOCK_METHOD1(glBlendEquation, void(GLenum mode));
    MOCK_METHOD1(glClear, void(GLbitfield mask));
    MOCK_METHOD4(glClearColor, void(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha));
    MOCK_METHOD1(glDisable, void(GLenum cap));
    MOCK_METHOD1(glEnable, void(GLenum cap));
    MOCK_METHOD0(glGetError, GLenum(void));
    MOCK_METHOD1(glUnmapBuffer, GLboolean(GLenum target));
    MOCK_METHOD4(glViewport, void(GLint x, GLint y, GLsizei width, GLsizei height));
    MOCK_METHOD2(glPolygonMode, void(GLenum face, GLenum mode));
    MOCK_METHOD2(glPixelStorei, void(GLenum pname, GLint param));
    MOCK_METHOD2(glMapBuffer, GLvoid*(GLenum target, GLenum access));
    MOCK_METHOD4(glScissor, void(GLint x, GLint y, GLsizei width, GLsizei height));
    MOCK_METHOD1(glCullFace, void(GLenum mode));
    MOCK_METHOD1(glFrontFace, void(GLenum mode));
    MOCK_METHOD1(glDepthFunc, void(GLenum mode));
    MOCK_METHOD5(glBindBufferRange, void(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size));
    MOCK_METHOD4(glBufferSubData, void(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data));
    MOCK_METHOD2(glGetUniformBlockIndex, GLuint(GLuint program, const GLchar *uniformBlockName));
    MOCK_METHOD3(glUniformBlockBinding, void(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding));
    MOCK_METHOD1(glGenerateMipmap, void(GLenum target));
    MOCK_METHOD2(glBindRenderbuffer, void(GLenum target, GLuint renderbuffer));
    MOCK_METHOD2(glGenRenderbuffers, void(GLsizei n, GLuint* renderbuffers));
    MOCK_METHOD2(glDeleteRenderbuffers, void(GLsizei n, const GLuint* renderbuffers));
    MOCK_METHOD2(glGetIntegerv, void(GLenum pname, GLint *params));
    MOCK_METHOD4(glRenderbufferStorage, void(GLenum target, GLenum internalformat, GLsizei width, GLsizei height));
    MOCK_METHOD1(glCheckFramebufferStatus, GLenum(GLenum target));
    MOCK_METHOD4(glFramebufferRenderbuffer, void(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer));
    MOCK_METHOD2(glDrawBuffers, void(GLsizei n, const GLenum *bufs));
    MOCK_METHOD2(glDeleteFramebuffers, void(GLsizei n, const GLuint* buffers));
    MOCK_METHOD7(glReadPixels, void(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels));
    MOCK_METHOD1(glReadBuffer, void(GLenum mode));
    MOCK_METHOD4(glBlendFuncSeparate, void(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha));
    MOCK_METHOD2(glUniform1ui, void(GLint location, GLuint v0));
    MOCK_METHOD3(glUniform2fv, void(GLint location, GLsizei count, const GLfloat *value));
  };
}


#endif //PRT_MOCK_GLFW_H