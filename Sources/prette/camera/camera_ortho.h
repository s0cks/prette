#ifndef PRT_CAMERA_ORTHO_H
#define PRT_CAMERA_ORTHO_H

#include <utility>

#include "prette/uri.h"
#include "prette/uuid.h"
#include "prette/keyboard/key.h"
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
    public:
      struct IdComparator {
        bool operator()(OrthoCamera* lhs, OrthoCamera* rhs) const {
          return lhs->GetId() < rhs->GetId();
        }
      };
    protected:
      OrthoCameraData data_;

      explicit OrthoCamera(const OrthoCameraData& data);
    public:
      ~OrthoCamera() override;

      const glm::mat4& GetProjection() const override {
        return data_.p;
      }

      const glm::mat4& GetView() const override {
        return data_.v;
      }

      const glm::mat4& GetViewProjection() const {
        return data_.vp;
      }

      const glm::vec3& GetPos() const {
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

      std::string ToString() const override {
        return "OrthoCamera()";
      }
    public:
      static OrthoCamera* New(const OrthoCameraData& data) {
        return new OrthoCamera(data);
      }

      static inline OrthoCamera*
      New(const float left,
          const float right,
          const float bottom,
          const float top,
          const float zNear = OrthoCameraData::kDefaultZNear,
          const float zFar = OrthoCameraData::kDefaultZFar) {
        return New(OrthoCameraData(left, right, bottom, top, zNear, zFar));
      }
    };

    typedef std::set<OrthoCamera*, OrthoCamera::IdComparator> OrthoCameraSet;

    const OrthoCameraSet& GetAllOrthoCameras();
    uword GetTotalNumberOfOrthoCameras();
  }
  using camera::OrthoCamera;
}

#endif //PRT_CAMERA_ORTHO_H