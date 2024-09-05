#include "prette/mock_glfw.h"
#include "prette/thread_local.h"

namespace prt {
  static ThreadLocal<MockGlfw> mock_;

  MockGlfw::MockGlfw() {
    mock_.Set(this);
  }

  MockGlfw::~MockGlfw() {
    mock_.Set(nullptr);
  }
}
using prt::MockGlfw;

static inline MockGlfw*
GetMock() {
  const auto mock = prt::mock_.Get();
  assert(mock != nullptr);
  return mock;
}

#define __ return GetMock()->

void glfwSwapInterval(int interval) {
  __ glfwSwapInterval(interval);
}

GLFWwindow* glfwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share) {
  __ glfwCreateWindow(width, height, title, monitor, share);
}

void glfwGetCursorPos(GLFWwindow* window, double* xpos, double* ypos) {
  __ glfwGetCursorPos(window, xpos, ypos);
}

void glfwGetFramebufferSize(GLFWwindow* window, int* width, int* height) {
  __ glfwGetFramebufferSize(window, width, height);
}

GLFWcursor* glfwCreateStandardCursor(const int shape) {
  __ glfwCreateStandardCursor(shape);
}

void glfwTerminate(void) {
  __ glfwTerminate();
}

int glfwGetKey(GLFWwindow* window, int key) {
  __ glfwGetKey(window, key);
}

void glfwSwapBuffers(GLFWwindow* window) {
  __ glfwSwapBuffers(window);
}

void glfwPollEvents(void) {
  __ glfwPollEvents();
}

void glfwSetWindowShouldClose(GLFWwindow* window, int value) {
  __ glfwSetWindowShouldClose(window, value);
}

int glfwGetMouseButton(GLFWwindow* window, int button) {
  __ glfwGetMouseButton(window, button);
}

GLFWwindowclosefun glfwSetWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun callback) {
  __ glfwSetWindowCloseCallback(window, callback);
}

void glfwMakeContextCurrent(GLFWwindow* window) {
  __ glfwMakeContextCurrent(window);
}

void glfwGetWindowSize(GLFWwindow* window, int* width, int* height) {
  __ glfwGetWindowSize(window, width, height);
}

void glfwSetInputMode(GLFWwindow* window, int mode, int value) {
  __ glfwSetInputMode(window, mode, value);
}

GLFWmonitor* glfwGetPrimaryMonitor() {
  __ glfwGetPrimaryMonitor();
}

void* glfwGetWindowUserPointer(GLFWwindow* window) {
  __ glfwGetWindowUserPointer(window);
}

void glfwSetWindowUserPointer(GLFWwindow* window, void* data) {
  __ glfwSetWindowUserPointer(window, data);
}

void glfwGetWindowPos(GLFWwindow* window, int* xPos, int* yPos) {
  __ glfwGetWindowPos(window, xPos, yPos);
}

GLFWwindowcontentscalefun glfwSetWindowContentScaleCallback(GLFWwindow* window, GLFWwindowcontentscalefun callback) {
  __ glfwSetWindowContentScaleCallback(window, callback);
}

GLFWwindowfocusfun glfwSetWindowFocusCallback(GLFWwindow* window, GLFWwindowfocusfun callback) {
  __ glfwSetWindowFocusCallback(window, callback);
}

GLFWwindowiconifyfun glfwSetWindowIconifyCallback(GLFWwindow* window, GLFWwindowiconifyfun callback) {
  __ glfwSetWindowIconifyCallback(window, callback);
}

GLFWwindowmaximizefun glfwSetWindowMaximizeCallback(GLFWwindow* window, GLFWwindowmaximizefun callback) {
  __ glfwSetWindowMaximizeCallback(window, callback);
}

void glfwSetWindowPos(GLFWwindow* window, int xPos, int yPos) {
  __ glfwSetWindowPos(window, xPos, yPos);
}

void glfwSetWindowSize(GLFWwindow* window, int xPos, int yPos) {
  __ glfwSetWindowSize(window, xPos, yPos);
}

void glfwSetWindowTitle(GLFWwindow* window, const char* title) {
  __ glfwSetWindowTitle(window, title);
}

GLFWwindowposfun glfwSetWindowPosCallback(GLFWwindow* window, GLFWwindowposfun callback) {
  __ glfwSetWindowPosCallback(window, callback);
}

GLFWwindowrefreshfun glfwSetWindowRefreshCallback(GLFWwindow* window, GLFWwindowrefreshfun callback) {
  __ glfwSetWindowRefreshCallback(window, callback);
}

GLFWwindowsizefun glfwSetWindowSizeCallback(GLFWwindow* window, GLFWwindowsizefun callback) {
  __ glfwSetWindowSizeCallback(window, callback);
}

const GLFWvidmode* glfwGetVideoMode(GLFWmonitor* monitor) {
  __ glfwGetVideoMode(monitor);
}

const char* glfwGetKeyName(const int key, const int scancode) {
  __ glfwGetKeyName(key, scancode);
}

int glfwGetWindowAttrib(GLFWwindow* window, int attrib) {
  __ glfwGetWindowAttrib(window, attrib);
}

void glfwSetWindowAttrib(GLFWwindow* window, int attrib, int value) {
  __ glfwSetWindowAttrib(window, attrib, value);
}

void glfwHideWindow(GLFWwindow* window) {
  __ glfwHideWindow(window);
}

void glfwShowWindow(GLFWwindow* window) {
  __ glfwShowWindow(window);
}

void glfwWindowHint(int hint, int value) {
  __ glfwWindowHint(hint, value);
}

void glfwWindowHintString(int hint, const char* value) {
  __ glfwWindowHintString(hint, value);
}

const char* glfwGetWindowTitle(GLFWwindow*	window) {
  __ glfwGetWindowTitle(window);
}


void glfwSetWindowSizeLimits(GLFWwindow* window, int minwidth, int minheight, int maxwidth, int maxheight) {
  __ glfwSetWindowSizeLimits(window, minwidth, minheight, maxwidth, maxheight);
}

void glfwGetWindowContentScale(GLFWwindow* window, float* xScale, float* yScale) {
  __ glfwGetWindowContentScale(window, xScale, yScale);
}