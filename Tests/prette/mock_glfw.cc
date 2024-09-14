#include "prette/mock_glfw.h"
#include "prette/thread_local.h"

namespace prt {
  static ThreadLocal<MockGlfw> mock_; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

  MockGlfw::MockGlfw() {
    mock_.Set(this);
  }

  MockGlfw::~MockGlfw() {
    mock_.Set(nullptr);
  }
}
using prt::MockGlfw;

static inline auto
GetMock() -> MockGlfw* {
  const auto mock = prt::mock_.Get();
  assert(mock != nullptr);
  return mock;
}

#define __ return GetMock()->

void glfwSwapInterval(int interval) {
  __ glfwSwapInterval(interval);
}

auto glfwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share) -> GLFWwindow* {
  __ glfwCreateWindow(width, height, title, monitor, share);
}

void glfwGetCursorPos(GLFWwindow* window, double* xpos, double* ypos) {
  __ glfwGetCursorPos(window, xpos, ypos);
}

void glfwGetFramebufferSize(GLFWwindow* window, int* width, int* height) {
  __ glfwGetFramebufferSize(window, width, height);
}

auto glfwCreateStandardCursor(const int shape) -> GLFWcursor* {
  __ glfwCreateStandardCursor(shape);
}

void glfwTerminate(void) {
  __ glfwTerminate();
}

auto glfwGetKey(GLFWwindow* window, int key) -> int {
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

auto glfwGetMouseButton(GLFWwindow* window, int button) -> int {
  __ glfwGetMouseButton(window, button);
}

auto glfwSetWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun callback) -> GLFWwindowclosefun {
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

auto glfwGetPrimaryMonitor() -> GLFWmonitor* {
  __ glfwGetPrimaryMonitor();
}

auto glfwGetWindowUserPointer(GLFWwindow* window) -> void* {
  __ glfwGetWindowUserPointer(window);
}

void glfwSetWindowUserPointer(GLFWwindow* window, void* data) {
  __ glfwSetWindowUserPointer(window, data);
}

void glfwGetWindowPos(GLFWwindow* window, int* xPos, int* yPos) {
  __ glfwGetWindowPos(window, xPos, yPos);
}

auto glfwSetWindowContentScaleCallback(GLFWwindow* window, GLFWwindowcontentscalefun callback) -> GLFWwindowcontentscalefun {
  __ glfwSetWindowContentScaleCallback(window, callback);
}

auto glfwSetWindowFocusCallback(GLFWwindow* window, GLFWwindowfocusfun callback) -> GLFWwindowfocusfun {
  __ glfwSetWindowFocusCallback(window, callback);
}

auto glfwSetWindowIconifyCallback(GLFWwindow* window, GLFWwindowiconifyfun callback) -> GLFWwindowiconifyfun {
  __ glfwSetWindowIconifyCallback(window, callback);
}

auto glfwSetWindowMaximizeCallback(GLFWwindow* window, GLFWwindowmaximizefun callback) -> GLFWwindowmaximizefun {
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

auto glfwSetWindowPosCallback(GLFWwindow* window, GLFWwindowposfun callback) -> GLFWwindowposfun {
  __ glfwSetWindowPosCallback(window, callback);
}

auto glfwSetWindowRefreshCallback(GLFWwindow* window, GLFWwindowrefreshfun callback) -> GLFWwindowrefreshfun {
  __ glfwSetWindowRefreshCallback(window, callback);
}

auto glfwSetWindowSizeCallback(GLFWwindow* window, GLFWwindowsizefun callback) -> GLFWwindowsizefun  {
  __ glfwSetWindowSizeCallback(window, callback);
}

auto glfwGetVideoMode(GLFWmonitor* monitor) -> const GLFWvidmode* {
  __ glfwGetVideoMode(monitor);
}

auto glfwGetKeyName(const int key, const int scancode) -> const char* {
  __ glfwGetKeyName(key, scancode);
}

auto glfwGetWindowAttrib(GLFWwindow* window, int attrib) -> int {
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

auto glfwGetWindowTitle(GLFWwindow*	window) -> const char* {
  __ glfwGetWindowTitle(window);
}


void glfwSetWindowSizeLimits(GLFWwindow* window, int minwidth, int minheight, int maxwidth, int maxheight) {
  __ glfwSetWindowSizeLimits(window, minwidth, minheight, maxwidth, maxheight);
}

void glfwGetWindowContentScale(GLFWwindow* window, float* xScale, float* yScale) {
  __ glfwGetWindowContentScale(window, xScale, yScale);
}