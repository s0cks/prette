#include "prette/mock_gl.h"
#include "prette/thread_local.h"

namespace prt {
  static ThreadLocal<MockGl> mock_;

  MockGl::MockGl() {
    mock_.Set(this);
  }

  MockGl::~MockGl() {
    mock_.Set(nullptr);
  }
}
using prt::MockGl;

static inline MockGl*
GetMock() {
  const auto mock = prt::mock_.Get();
  assert(mock != nullptr);
  return mock;
}

#define __ return GetMock()->

// vertex array objects
void glBindVertexArray(GLuint array) {
  __ glBindVertexArray(array);
}

void glGenVertexArrays(GLsizei n, GLuint *arrays) {
  __ glGenVertexArrays(n, arrays);
}

void glEnableVertexAttribArray(GLuint index) {
  __ glEnableVertexAttribArray(index);
}

GLint glGetAttribLocation(GLuint program, const GLchar* name) {
  __ glGetAttribLocation(program, name);
}

void glDeleteVertexArrays(GLsizei n, const GLuint *arrays) {
  __ glDeleteVertexArrays(n, arrays);
}

// textures
void glGenTextures(GLsizei n, GLuint* textures) {
  __ glGenTextures(n, textures);
}

void glGetTexParameteriv(GLenum target, GLenum pname, GLint* params) {
  __ glGetTexParameteriv(target, pname, params);
}

void glBindTexture(GLenum target, GLuint texture) {
  __ glBindTexture(target, texture);
}

void glActiveTexture(GLuint texture) {
  __ glActiveTexture(texture);
}

void glTexParameterIiv(GLenum target, GLenum pname, const GLint *params) {
  __ glTexParameterIiv(target, pname, params);
}

void glTexParameteri(GLenum target, GLenum pname, GLint param) {
  __ glTexParameteri(target, pname, param);
}

void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels) {
  __ glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}

void glDeleteTextures(GLsizei n, const GLuint* arrays) {
  __ glDeleteTextures(n, arrays);
}

// shaders
void glAttachShader(GLuint program, GLuint shader) {
  __ glAttachShader(program, shader);
}

void glDetachShader(GLuint program, GLuint shader) {
  __ glDetachShader(program, shader);
}

void glCompileShader(GLuint shader) {
  __ glCompileShader(shader);
}

GLuint glCreateProgram() {
  __ glCreateProgram();
}

GLuint glCreateShader(GLenum type) {
  __ glCreateShader(type);
}

void glDeleteShader(GLuint shader) {
  __ glDeleteShader(shader);
}

void glDeleteProgram(GLuint program) {
  __ glDeleteProgram(program);
}

void glGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
  __ glGetProgramInfoLog(program, bufSize, length, infoLog);
}

void glGetShaderiv(GLuint shader, GLenum pname, GLint *params) {
  __ glGetShaderiv(shader, pname, params);
}

void glGetProgramiv(GLuint program, GLenum pname, GLint *params) {
  __ glGetProgramiv(program, pname, params);
}

void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
  __ glGetShaderInfoLog(shader, bufSize, length, infoLog);
}

GLint glGetUniformLocation (GLuint program, const GLchar *name)  {
  __ glGetUniformLocation (program, name);
}

void glLinkProgram (GLuint program) {
  __ glLinkProgram(program);
}

void glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
  __ glUniformMatrix4fv(location, count, transpose, value);
}

void glUseProgram (GLuint program) {
  __ glUseProgram(program);
}

void glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) {
  __ glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void glShaderSource (GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length) {
  __ glShaderSource(shader, count, string, length);
}

void glUniform4fv (GLint location, GLsizei count, const GLfloat *value) {
  __ glUniform4fv(location, count, value);
}

void glUniform3fv (GLint location, GLsizei count, const GLfloat *value) {
  __ glUniform3fv(location, count, value);
}

void glUniform1i (GLint location, GLint v0) {
  __ glUniform1i(location, v0);
}

void glUniform1f (GLint location, GLfloat v0) {
  __ glUniform1f(location, v0);
}

void glGetActiveAttrib(	GLuint program,
                        GLuint index,
                        GLsizei bufSize,
                        GLsizei *length,
                        GLint *size,
                        GLenum *type,
                        GLchar *name) {
  __ glGetActiveAttrib(program, index, bufSize, length, size, type, name);
}

void glGetActiveUniform(	GLuint program,
                        GLuint index,
                        GLsizei bufSize,
                        GLsizei *length,
                        GLint *size,
                        GLenum *type,
                        GLchar *name) {
  __ glGetActiveUniform(program, index, bufSize, length, size, type, name);
}

// buffers
void glBindBuffer(GLenum target, GLuint buffer) {
  __ glBindBuffer(target, buffer);
}

void glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage) {
  __ glBufferData(target, size, data, usage);
}

void glGenBuffers(GLsizei n, GLuint* buffers) {
  __ glGenBuffers(n, buffers);
}

void glDeleteBuffers(GLsizei n, const GLuint* buffers) {
  __ glDeleteBuffers(n, buffers);
}

// framebuffers
void glBindFramebuffer(GLenum target, GLuint framebuffer) {
  __ glBindFramebuffer(target, framebuffer);
}

void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {
  __ glFramebufferTexture2D(target, attachment, textarget, texture, level);
}

void glGenFramebuffers(GLsizei n, GLuint* framebuffers) {
  __ glGenFramebuffers(n, framebuffers);
}

// misc
void glBlendEquation(GLenum mode) {
  __ glBlendEquation(mode);
}

void glBlendFunc(GLenum sfactor, GLenum dfactor) {
  __ glBlendFunc(sfactor, dfactor);
}

void glClear(GLbitfield mask) {
  __ glClear(mask);
}

void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
  __ glClearColor(red, green, blue, alpha);
}

void glDisable(GLenum cap) {
  __ glDisable(cap);
}

void glEnable(GLenum cap) {
  __ glEnable(cap);
}

GLenum glGetError(void) {
  __ glGetError();
}

GLboolean glUnmapBuffer (GLenum target) {
  __ glUnmapBuffer(target);
}

void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
  __ glViewport(x, y, width, height);
}

void glPolygonMode(GLenum face, GLenum mode) {
  __ glPolygonMode(face, mode);
}

void glPixelStorei(GLenum pname, GLint param) {
  __ glPixelStorei(pname, param);
}

GLvoid* glMapBuffer(GLenum target, GLenum access) {
  __ glMapBuffer(target, access);
}

void glScissor(GLint x, GLint y, GLsizei width, GLsizei height) {
  __ glScissor(x, y, width, height);
}

void glFrontFace(GLenum mode) {
  __ glFrontFace(mode);
}

void glCullFace(GLenum mode) {
  __ glCullFace(mode);
}

void glDepthFunc(GLenum mode) {
  __ glDepthFunc(mode);
}

// draw
void glDrawArrays(GLenum mode, GLint first, GLsizei count) {
  __ glDrawArrays(mode, first, count);
}

void glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)  {
  __ glDrawElements(mode, count, type, indices);
}

void glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) {
  __ glBindBufferRange(target, index, buffer, offset, size);
}

void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data) {
  __ glBufferSubData(target, offset, size, data);
}

GLuint glGetUniformBlockIndex(GLuint program, const GLchar* uniformBlockName) {
  __ glGetUniformBlockIndex(program, uniformBlockName);
}

void glUniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding) {
  __ glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
}

void glBindBufferBase(GLenum target, GLuint index, GLuint buffer) {
  __ glBindBufferBase(target, index, buffer);

}

void glGenerateMipmap(GLenum target) {
  __ glGenerateMipmap(target);
}

void glBindRenderbuffer(GLenum target, GLuint renderbuffer) {
  __ glBindRenderbuffer(target, renderbuffer);
}

void glGenRenderbuffers(GLsizei n, GLuint *renderbuffers) {
  __ glGenRenderbuffers(n, renderbuffers);
}

void glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers) {
  __ glDeleteRenderbuffers(n, renderbuffers);
}

void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) {
  __ glRenderbufferStorage(target, internalformat, width, height);
}

void glGetIntegerv(GLenum pname, GLint *params) {
  __ glGetIntegerv(pname, params);
}

void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {
  __ glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

GLenum glCheckFramebufferStatus(GLenum target) {
  __ glCheckFramebufferStatus(target);
}

void glDrawBuffers(GLsizei n, const GLenum *bufs) {
  __ glDrawBuffers(n, bufs);
}

void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels) {
  __ glReadPixels(x, y, width, height, format, type, pixels);
}

void glReadBuffer(GLenum mode) {
  __ glReadBuffer(mode);
}

void glBlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha) {
  __ glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
}

void glUniform1ui(GLint location, GLuint v0) {
  __ glUniform1ui(location, v0);
}

void glUniform2fv(GLint location, GLsizei count, const GLfloat* value) {
  __ glUniform2fv(location, count, value);
}

void glDeleteFramebuffers(GLsizei n, const GLuint* buffers) {
  __ glDeleteFramebuffers(n, buffers);
}