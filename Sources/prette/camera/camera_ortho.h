#ifndef PRT_CAMERA_ORTHO_H
#define PRT_CAMERA_ORTHO_H

#include <set>
#include <utility>
#include <unordered_set>
#include "prette/uuid.h"
#include "prette/camera/camera.h"

namespace prt {
  namespace camera {
    struct OrthoCameraData {
      static constexpr const float kDefaultZNear = -1.0f;
      static constexpr const float kDefaultZFar = 1.0f;
      static constexpr const auto kDefaultPos = glm::vec3(0.0f);
      static constexpr const auto kDefaultRotation = 0.0f;

      static constexpr const auto kUp = glm::vec3(0.0f, 1.0f, 0.0f);
      static constexpr const auto kDown = glm::vec3(0.0f, -1.0f, 0.0f);
      static constexpr const auto kLeft = glm::vec3(-1.0f, 0.0f, 0.0f);
      static constexpr const auto kRight = glm::vec3(1.0f, 0.0f, 0.0f);

      glm::mat4 p;
      glm::mat4 v;
      glm::mat4 vp;
      glm::vec3 pos;
      float rotation;

      OrthoCameraData(const float left,
                      const float right,
                      const float bottom,
                      const float top,
                      const float zNear = kDefaultZNear,
                      const float zFar = kDefaultZFar):
        p(1.0f),
        v(1.0f),
        vp(1.0f),
        pos(kDefaultPos),
        rotation(kDefaultRotation) {
        SetProjection(left, right, bottom, top, zNear, zFar);
      }

      void SetProjection(const float left,
                         const float right,
                         const float bottom,
                         const float top,
                         const float zNear = kDefaultZNear,
                         const float zFar = kDefaultZFar) {
        p = glm::ortho(left, right, bottom, top, zNear, zFar);
        vp = p * v;
      }

      void SetPosition(const glm::vec3& new_pos) {
        pos = new_pos;
        DLOG(INFO) << "new_pos: " << glm::to_string(new_pos);
        Update();
      }

      void SetRotation(const float rot) {
        rotation = rot;
        Update();
      }

      inline void MoveUp() {
        return SetPosition(pos + kUp);
      }

      inline void MoveDown() {
        return SetPosition(pos + kDown);
      }

      inline void MoveLeft() {
        return SetPosition(pos + kLeft);
      }

      inline void MoveRight() {
        return SetPosition(pos + kRight);
      }

      friend class OrthoCamera;
    private:
      void Update() {
        auto view = glm::mat4(1.0f);
        view = glm::translate(view, pos);
        view = glm::rotate(view, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        v = glm::inverse(view);
        vp = p * v;
      }
    };

    class OrthoCamera : public Camera {
      DEFINE_NON_COPYABLE_TYPE(OrthoCamera);
    public:
      struct IdComparator {
        auto operator()(OrthoCamera* lhs, OrthoCamera* rhs) const -> bool {
          return lhs->GetId() < rhs->GetId();
        }
      };
    private:
      OrthoCameraData data_;
      explicit OrthoCamera(const OrthoCameraData& data);
    public:
      ~OrthoCamera() override;

      auto GetProjection() const -> const glm::mat4& override {
        return data_.p;
      }

      auto GetView() const -> const glm::mat4& override {
        return data_.v;
      }

      auto GetViewProjection() const -> const glm::mat4& {
        return data_.vp;
      }

      auto GetPos() const -> const glm::vec3& {
        return data_.pos;
      }

      void SetProjection(const float left,
                         const float right,
                         const float bottom,
                         const float top,
                         const float zNear = OrthoCameraData::kDefaultZNear,
                         const float zFar = OrthoCameraData::kDefaultZFar) {
        return data_.SetProjection(left, right, bottom, top, zNear, zFar);
      }

      void SetRotation(const float rotation) {
        return data_.SetRotation(rotation);
      }

      void SetPosition(const glm::vec3& pos) {
        return data_.SetPosition(pos);
      }

      auto ToString() const -> std::string override {
        return "OrthoCamera()";
      }
    public:
      static auto New(const OrthoCameraData& data) -> OrthoCamera* {
        return new OrthoCamera(data);
      }

      static inline auto
      New(const float left,
          const float right,
          const float bottom,
          const float top,
          const float zNear = OrthoCameraData::kDefaultZNear,
          const float zFar = OrthoCameraData::kDefaultZFar) -> OrthoCamera* {
        return New(OrthoCameraData(left, right, bottom, top, zNear, zFar));
      }
    };

    using OrthoCameraSet = std::set<OrthoCamera *, OrthoCamera::IdComparator>; //TODO: convert to std::unordered_set

    auto GetAllOrthoCameras() -> const OrthoCameraSet&;
    auto GetTotalNumberOfOrthoCameras() -> uword;
  }
  using camera::OrthoCamera;
}

#endif //PRT_CAMERA_ORTHO_H