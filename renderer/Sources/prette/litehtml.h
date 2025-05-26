#ifndef PRT_LITEHTML_H
#define PRT_LITEHTML_H

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// IWYU pragma: begin_exports
// clang-format off
#include <litehtml/litehtml.h>
#include <litehtml/os_types.h>
#include <litehtml/types.h>
#include <litehtml/web_color.h>
#include <litehtml/background.h>
#include <litehtml/document_container.h>
#include <litehtml/borders.h>
// clang-format on
// IWYU pragma: end_exports

#include "prette/dimension.h"

namespace prt::html {
using namespace ::litehtml;

class VulkanContainer : public document_container {
 private:
  Dimension size_;
  std::string default_font_name_;
  int default_font_size_;

 public:
  VulkanContainer(const Dimension& size, const std::string default_font_name, const int default_font_size = 12.0f) :
    document_container(),
    size_(size),
    default_font_name_(std::move(default_font_name)),
    default_font_size_(default_font_size) {}

  auto create_font(const char* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration,
                   litehtml::font_metrics* fm) -> litehtml::uint_ptr override;
  void delete_font(litehtml::uint_ptr hFont) override;
  auto text_width(const char* text, litehtml::uint_ptr hFont) -> int override;
  void draw_text(litehtml::uint_ptr hdc, const char* text, litehtml::uint_ptr hFont, litehtml::web_color color,
                 const litehtml::position& pos) override;
  auto pt_to_px(int pt) const -> int override;
  auto get_default_font_size() const -> int override;
  auto get_default_font_name() const -> const char* override;
  void draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) override;
  void load_image(const char* src, const char* baseurl, bool redraw_on_ready) override;
  void get_image_size(const char* src, const char* baseurl, litehtml::size& sz) override;
  void draw_background(litehtml::uint_ptr hdc, const std::vector<litehtml::background_paint>& bg) override;
  void draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos,
                    bool root) override;

  void set_caption(const char* caption) override;
  void set_base_url(const char* base_url) override;
  void link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) override;
  void on_anchor_click(const char* url, const litehtml::element::ptr& el) override;
  void set_cursor(const char* cursor) override;
  void transform_text(litehtml::string& text, litehtml::text_transform tt) override;
  void import_css(litehtml::string& text, const litehtml::string& url, litehtml::string& baseurl) override;
  void set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius) override;
  void del_clip() override;
  void get_client_rect(litehtml::position& client) const override;

  auto create_element(const char* tag_name, const litehtml::string_map& attributes,
                      const std::shared_ptr<litehtml::document>& doc) -> litehtml::element::ptr override;
  void get_media_features(litehtml::media_features& media) const override;
  void get_language(litehtml::string& language, litehtml::string& culture) const override;
  auto resolve_color(const litehtml::string& color) const -> litehtml::string override;
  void split_text(const char* text, const std::function<void(const char*)>& on_word,
                  const std::function<void(const char*)>& on_space) override;

 protected:
  ~VulkanContainer() = default;
};

void Init();
auto IsInitialized() -> bool;
auto GetCurrentThreadContainer() -> VulkanContainer*;

auto FromString(const std::string html) -> document::ptr;
}  // namespace prt::html

#endif  // PRT_LITEHTML_H
