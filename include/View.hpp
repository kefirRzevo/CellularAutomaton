#pragma once

#include <SFML/Graphics.hpp>

#include "Model.hpp"

namespace automaton {

class Polygon;

class View final {
  using size_type = size_t;

  Model model_;

  sf::RenderWindow sfWindow_;
  sf::View sfView_;
  sf::Texture sfTexture_;

  size_type windowWidth_;
  size_type windowHeight_;
  size_type polyWidth_;
  size_type polyHeight_;
  std::vector<sf::Color> polyBuf_;

  sf::Rect<size_type> visiblePart_;
  std::vector<sf::Color> visiblePartBuf_;

  void fillPolyBuf() {
    polyBuf_.resize(polyWidth_ * polyHeight_);
    polyBuf_.shrink_to_fit();
    for (size_type i = 0; i != polyHeight_; ++i) {
      for (size_type j = 0; j != polyWidth_; ++j) {
        if (model_.getPolygon()[i][j])
          polyBuf_[i * polyWidth_ + j] = sf::Color::Black;
        else
          polyBuf_[i * polyWidth_ + j] = sf::Color::White;
      }
    }
  }

  void fillVisiblePartBuf() {
    auto x = visiblePart_.left;
    auto y = visiblePart_.top;
    auto width = visiblePart_.width;
    auto height = visiblePart_.height;
    if (visiblePartBuf_.size() != width * height) {
      visiblePartBuf_.resize(width * height);
      visiblePartBuf_.shrink_to_fit();
    }
    for (size_type i = 0; i != height; ++i)
      for (size_type j = 0; j != width; ++j)
        visiblePartBuf_.at(i * width + j) =
            polyBuf_.at((i + y) * polyWidth_ + (j + x));
  }

  void drawVisiblePart() {
    auto width = visiblePart_.width;
    auto height = visiblePart_.height;
    assert(visiblePart_.left + width <= polyWidth_);
    assert(visiblePart_.top + height <= polyHeight_);
    fillVisiblePartBuf();
    sf::Uint8 *data = reinterpret_cast<sf::Uint8 *>(visiblePartBuf_.data());
    if (sfTexture_.getSize().x != width || sfTexture_.getSize().y != height)
      sfTexture_.create(width, height);
    sfTexture_.update(data);
    sf::Sprite sfSprite{sfTexture_};
    auto scaleX = static_cast<float>(windowWidth_) / width;
    auto scaleY = static_cast<float>(windowHeight_) / height;
    if (windowWidth_ != width || windowHeight_ != height)
      sfSprite.scale(scaleX, scaleY);
    sfWindow_.draw(sfSprite);
  }

  enum class Moving {
    DOWN,
    RIGHT,
    LEFT,
    UP,
    UNKNOWN,
  };

  static Moving getMovingSize(sf::Keyboard::Key code) noexcept {
    switch (code) {
    case sf::Keyboard::Down:
      return Moving::DOWN;
    case sf::Keyboard::Right:
      return Moving::RIGHT;
    case sf::Keyboard::Left:
      return Moving::LEFT;
    case sf::Keyboard::Up:
      return Moving::UP;
    default:
      return Moving::UNKNOWN;
    }
  }

  void moveVisiblePart(Moving side) noexcept {
    const size_type shift = 100;
    auto &top = visiblePart_.top;
    auto &left = visiblePart_.left;
    auto width = visiblePart_.width;
    auto height = visiblePart_.height;
    switch (side) {
    case Moving::DOWN:
      if (top + shift + height <= polyHeight_)
        top += shift;
      break;
    case Moving::RIGHT:
      if (left + shift + width <= polyWidth_)
        left += shift;
      break;
    case Moving::LEFT:
      if (left >= shift)
        left -= shift;
      break;
    case Moving::UP:
      if (top >= shift)
        top -= shift;
      break;
    default:
      break;
    }
  }

public:
  View(Model &&model, size_type width = 1248, size_type height = 868)
      : model_(model), windowWidth_(width), windowHeight_(height),
        polyWidth_(model_.getPolygon().width()),
        polyHeight_(model_.getPolygon().height()) {
    fillPolyBuf();
    visiblePart_ = sf::Rect<size_type>{0, 0, std::min(windowWidth_, polyWidth_),
                                       std::min(windowHeight_, polyHeight_)};
    sfWindow_.create(sf::VideoMode(windowWidth_, windowHeight_), "Automaton",
                     sf::Style::Titlebar | sf::Style::Close);
    sfTexture_.create(windowWidth_, windowHeight_);
  }

  void run() {
    sf::Event sfEvent;
    while (sfWindow_.isOpen()) {
      while (sfWindow_.pollEvent(sfEvent)) {
        switch (sfEvent.type) {
        case sf::Event::Closed: {
          sfWindow_.close();
          break;
        }
        case sf::Event::KeyPressed: {
          auto side = getMovingSize(sfEvent.key.code);
          moveVisiblePart(side);
          break;
        }
        default:
          break;
        }
      }
      drawVisiblePart();
      sfWindow_.display();
    }
  }
};

} // namespace automaton
