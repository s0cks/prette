#include "prette/camera/camera_perspective.h"

#include <sstream>
#include "prette/thread_local.h"
#include "prette/window/window.h"

#include "prette/ubo/ubo_scope.h"
#include "prette/ubo/ubo_builder.h"

#include "prette/mouse/mouse.h"
#include "prette/keyboard/keyboard.h"

namespace prt::camera {
  PerspectiveCameraData::PerspectiveCameraData():
    viewport(0.0f),
    up(glm::vec3(0.0f, 1.0f, 0.0f)),
    pos(0.0f),
    front(glm::vec3(0.0f, 0.0f, -1.0f)),
    right(0.0f),
    projection(1.0f),
    view(1.0f),
    yaw(-90.0f),
    pitch(0.0f),
    speed(0.5f),
    sensitivity(0.1f),
    zoom(45.0f) {
    projection = CalculateProjection();
    view = CalculateView();
  }

  PerspectiveCameraData::PerspectiveCameraData(const glm::vec2& viewport_size,
                                               const glm::vec3& f,
                                               const glm::vec3& p,
                                               const glm::vec3& u):
    viewport(0, 0, viewport_size),
    up(u),
    pos(p),
    front(f),
    right(glm::vec3(0.0f)),
    projection(glm::mat4(1.0f)),
    view(glm::mat4(1.0f)),
    yaw(-90.0f),
    pitch(0.0f),
    speed(0.5f),
    sensitivity(0.1f),
    zoom(45.0f) {
    projection = CalculateProjection();
    view = CalculateView();
  }

  glm::mat4 PerspectiveCameraData::CalculateProjection() const {
    const auto v = viewport[2] / viewport[3];
    return glm::perspective(glm::radians(zoom), v, 0.1f, 100.0f);
  }

  glm::mat4 PerspectiveCameraData::CalculateView() const {
    return glm::lookAt(pos, pos + front, up);
  }

  PerspectiveCamera::PerspectiveCamera(const Metadata& meta, const PerspectiveCameraData& data):
    Camera(meta),
    data_(data),
    ubo_(nullptr) {
    ubo::UboBuilder<gpu::CameraData> builder;
    builder.Append( gpu::CameraData {
      .projection = GetProjection(),
      .view = GetView(), 
    });
    ubo_ = builder.Build();
    LOG_IF(ERROR, !ubo_) << "failed to create PerspectiveCameraData ubo.";
    on_mouse_moved_ = mouse::OnMouseMoveEvent()
      .subscribe([this](mouse::MouseMoveEvent* event) {
        return OnMouseMoved(event);
      });
  }

  PerspectiveCamera::~PerspectiveCamera() {
    if(on_mouse_moved_.is_subscribed())
      on_mouse_moved_.unsubscribe();
    if(on_key_pressed_.is_subscribed())
      on_key_pressed_.unsubscribe();
    delete ubo_; //TODO: remove
  }

  void PerspectiveCamera::UpdateUbo() {
    ubo::UboUpdateScope update(GetUbo());
    update.Update(gpu::CameraData {
      .projection = GetProjection(),
      .view = GetView(),
    });
  }

  void PerspectiveCamera::Update() {
    glm::vec3 front;
    front.x = cos(glm::radians(GetYaw()) * cos(glm::radians(GetPitch())));
    front.y = sin(glm::radians(GetPitch()));
    front.z = sin(glm::radians(GetYaw()) * cos(glm::radians(GetPitch())));
    data_.front = glm::normalize(front);
    data_.right = glm::normalize(glm::cross(data_.front, data_.up));
    data_.up = glm::normalize(glm::cross(data_.right, data_.front));
    data_.view = data_.CalculateView();
  }

  void PerspectiveCamera::OnKeyPressed(const keyboard::KeyCode code) {
    const auto engine = engine::GetEngine();
    PRT_ASSERT(engine);
    const auto delta = 0; //TODO: engine->GetState()->AsTickState()->GetTimeSinceLastTick();
    const auto velocity = data_.CalculateVelocity(delta);
    switch(code) {
      case GLFW_KEY_W:
        data_.pos += (data_.front * velocity);
        break;
      case GLFW_KEY_S:
        data_.pos -= (data_.front * velocity);
        break;
      case GLFW_KEY_A:
        data_.pos -= (data_.right * velocity);
        break;
      case GLFW_KEY_D:
        data_.pos += (data_.right * velocity);
        break; 
    }
  }

  void PerspectiveCamera::OnMouseMoved(const mouse::MouseMoveEvent* event) {
    const auto window = GetAppWindow();
    PRT_ASSERT(window);
    if(!window->IsFocused())
      return;
    const auto& delta = event->delta();
    data_.yaw += (delta[0] * data_.sensitivity);
    data_.pitch -= (delta[1] * data_.sensitivity);
    if(data_.pitch > 89.0f)
      data_.pitch = 89.0f;
    else if(data_.pitch < -89.0f)
      data_.pitch = -89.0f;
    Update();
  }

  std::string PerspectiveCamera::ToString() const {
    std::stringstream ss;
    ss << "PerspectiveCamera(";
    ss << "data=" << data_;
    ss << ")";
    return ss.str();
  }

  static ThreadLocal<PerspectiveCamera> camera_;

  static inline PerspectiveCamera*
  CreatePerspectiveCamera() {
    const auto window = GetAppWindow();
    PRT_ASSERT(window);
    const auto viewport_size = window->GetSize();
    const auto data = PerspectiveCameraData(viewport_size, PerspectiveCamera::kDefaultFront, PerspectiveCamera::kDefaultPos, PerspectiveCamera::kDefaultUp);
    return PerspectiveCamera::New(data);
  }

  PerspectiveCamera* GetPerspectiveCamera() {
    if(camera_)
      return camera_.Get();
    const auto camera = CreatePerspectiveCamera();
    camera_.Set(camera);
    return camera;
  }

  PerspectiveCamera* PerspectiveCamera::FromJson(const uri::Uri& uri) {
    NOT_IMPLEMENTED(FATAL); //TODO: implement
    return nullptr;
  }

  PerspectiveCamera* PerspectiveCamera::FromJson(const std::string& value) {
    if(IsCameraUri(value)) {
      auto path = value;
      if(!EndsWith(path, ".json"))
        path = fmt::format("{0:s}.json", path);
      return FromJson(uri::Uri(path));
    }
    NOT_IMPLEMENTED(FATAL); //TODO: implement
    return nullptr;
  }
}