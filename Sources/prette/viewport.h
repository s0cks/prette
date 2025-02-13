#ifndef PRT_VIEWPORT_H
#define PRT_VIEWPORT_H

namespace prt {
class Driver;
class Viewport {
 private:
  static void InitRenderPass(const Driver* driver);
  static void InitPipeline(const Driver* driver);
  static void InitFramebuffers(const Driver* driver);
  static void InitImage(const Driver* driver);
  static void InitImageViews(const Driver* driver);

 public:
  static void Init();
};
}  // namespace prt

#endif  // PRT_VIEWPORT_H
