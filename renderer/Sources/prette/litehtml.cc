#include "prette/litehtml.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "prette/common.h"
#include "prette/thread_local.h"
#include "prette/window/window.h"

namespace prt::html {
#define CONTAINER_WIDTH  (static_cast<int>(size_.width()))
#define CONTAINER_HEIGHT (static_cast<int>(size_.height()))

auto VulkanContainer::create_font(const char* faceName, int size, int weight, litehtml::font_style italic,
                                  unsigned int decoration, litehtml::font_metrics* fm) -> litehtml::uint_ptr {
  NOT_IMPLEMENTED(ERROR);
  return 0;
}
void VulkanContainer::delete_font(litehtml::uint_ptr hFont) {
  NOT_IMPLEMENTED(ERROR);
}

auto VulkanContainer::text_width(const char* text, litehtml::uint_ptr hFont) -> int {
  NOT_IMPLEMENTED(ERROR);
  return 0;
}

void VulkanContainer::draw_text(litehtml::uint_ptr hdc, const char* text, litehtml::uint_ptr hFont,
                                litehtml::web_color color, const litehtml::position& pos) {
  NOT_IMPLEMENTED(ERROR);
  DLOG(INFO) << "drawing text: " << text;
}

auto VulkanContainer::pt_to_px(int pt) const -> int {
  return pt;
}

auto VulkanContainer::get_default_font_size() const -> int {
  return default_font_size_;
}

auto VulkanContainer::get_default_font_name() const -> const char* {
  return default_font_name_.c_str();
}

void VulkanContainer::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) {
  NOT_IMPLEMENTED(ERROR);
}

void VulkanContainer::load_image(const char* src, const char* baseurl, bool redraw_on_ready) {
  NOT_IMPLEMENTED(ERROR);
}

void VulkanContainer::get_image_size(const char* src, const char* baseurl, litehtml::size& sz) {
  NOT_IMPLEMENTED(ERROR);
}

void VulkanContainer::draw_background(litehtml::uint_ptr hdc, const std::vector<litehtml::background_paint>& bg) {
  NOT_IMPLEMENTED(ERROR);
  DLOG(INFO) << "drawing background.";
}

void VulkanContainer::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders,
                                   const litehtml::position& draw_pos, bool root) {
  NOT_IMPLEMENTED(ERROR);
}

void VulkanContainer::set_caption(const char* caption) {
  NOT_IMPLEMENTED(ERROR);
}

void VulkanContainer::set_base_url(const char* base_url) {
  NOT_IMPLEMENTED(ERROR);
}

void VulkanContainer::link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) {
  NOT_IMPLEMENTED(ERROR);
}

void VulkanContainer::on_anchor_click(const char* url, const litehtml::element::ptr& el) {
  NOT_IMPLEMENTED(ERROR);
}

void VulkanContainer::set_cursor(const char* cursor) {
  NOT_IMPLEMENTED(ERROR);
}

void VulkanContainer::transform_text(litehtml::string& text, litehtml::text_transform tt) {
  NOT_IMPLEMENTED(ERROR);
}

void VulkanContainer::import_css(litehtml::string& text, const litehtml::string& url, litehtml::string& baseurl) {
  NOT_IMPLEMENTED(ERROR);
}

void VulkanContainer::set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius) {
  NOT_IMPLEMENTED(ERROR);
}

void VulkanContainer::del_clip() {
  NOT_IMPLEMENTED(ERROR);
}

void VulkanContainer::get_client_rect(litehtml::position& client) const {
  const auto window = GetAppWindow();
  const auto& size = window->GetFramebufferSize();
  client = litehtml::position(0, 0, CONTAINER_WIDTH, CONTAINER_HEIGHT);
}

auto VulkanContainer::create_element(const char* tag_name, const litehtml::string_map& attributes,
                                     const std::shared_ptr<litehtml::document>& doc) -> litehtml::element::ptr {
  return nullptr;
}

void VulkanContainer::get_media_features(litehtml::media_features& media) const {
  const auto window = GetAppWindow();
  const auto& size = window->GetFramebufferSize();
  media.width = CONTAINER_WIDTH;
  media.height = CONTAINER_HEIGHT;
}

void VulkanContainer::get_language(litehtml::string& language, litehtml::string& culture) const {
  NOT_IMPLEMENTED(ERROR);
}

auto VulkanContainer::resolve_color(const litehtml::string& color) const -> litehtml::string {
  return {};
}

void VulkanContainer::split_text(const char* text, const std::function<void(const char*)>& on_word,
                                 const std::function<void(const char*)>& on_space) {}

static ThreadLocal<VulkanContainer> container_{};

auto IsInitialized() -> bool {
  return container_.Get() != nullptr;
}

auto GetCurrentThreadContainer() -> VulkanContainer* {
  ASSERT(IsInitialized());
  return container_;
}

void Init() {
  DLOG(INFO) << "initializing litehtml....";
  ASSERT(!IsInitialized());
  const auto window = GetAppWindow();
  container_ = new VulkanContainer(window->GetFramebufferSize(), "Roboto");
  ASSERT(IsInitialized());
}

auto FromString(const std::string html) -> document::ptr {
  return document::createFromString(html.data(), GetCurrentThreadContainer());
}
}  // namespace prt::html