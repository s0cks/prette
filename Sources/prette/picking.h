#ifndef PRT_PICKING_H
#define PRT_PICKING_H

namespace prt {
class Driver;
class Picking {
 private:
  static void InitImages(const Driver* driver);
  static void InitPipeline(const Driver* driver);
  static void InitRenderPass(const Driver* driver);
  static void InitFramebuffer(const Driver* driver);

  static void Destroy(const Driver* driver);

 public:
  static void Init();
};
}  // namespace prt

#endif  // PRT_PICKING_H
