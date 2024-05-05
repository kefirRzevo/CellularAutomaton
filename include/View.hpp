#pragma once

#include <SFML/Graphics.hpp>

#include "Model.hpp"

namespace automaton {

class Polygon;

class View {
  Model& model_;

  sf::RenderWindow sfWindow_;
  sf::Time period_;
  sf::Texture sfTexture_;
  std::vector<sf::Color> buf_;

  void drawPolygon(const Polygon& poly) {
    auto width = poly.getWidth(), height = poly.getHeight();
    for (size_t i = 0; i != height; ++i) {
      for (size_t j = 0; j != width; ++j) {
        buf_[i * width + j] = poly[i].get(j) ? sf::Color::White : sf::Color::Black;
      }
    }
    sfTexture_.update(reinterpret_cast<sf::Uint8*>(buf_.data()));
    sf::Sprite sfSprite{sfTexture_};
    auto scaleX = sfWindow_.getSize().x / sfTexture_.getSize().x;
    auto scaleY = sfWindow_.getSize().y / sfTexture_.getSize().y;
    sfSprite.scale(scaleX, scaleY);
    sfWindow_.clear(sf::Color::Black);
    sfWindow_.draw(sfSprite);
  }

public:
  View(Model& model, size_t freq = 60, unsigned int width = 800, unsigned int height = 600)
  : model_(model), period_(sf::microseconds(1000000 / freq)) {
    const auto& poly = model.getPolygon();
    sfWindow_.create(sf::VideoMode(width, height), "Automaton");
    sfTexture_.create(poly.getWidth(), poly.getHeight());
    buf_.resize(poly.getWidth() * poly.getHeight());
    buf_.shrink_to_fit();
  }

  void run() {
    sf::Event sfEvent;
    sf::Clock sfClock;
    auto elapsedSinceUpdate = sf::Time::Zero;
    auto startSinceUpdate = sfClock.getElapsedTime();
    while(sfWindow_.isOpen()) {
      while (sfWindow_.pollEvent(sfEvent)) {
        if (sfEvent.type == sf::Event::Closed) {
            sfWindow_.close();
        }
      }
      drawPolygon(model_.getPolygon());
      auto end = sfClock.getElapsedTime();
      if (end - startSinceUpdate > period_) {
        model_.update();
        elapsedSinceUpdate = (end-startSinceUpdate);
        elapsedSinceUpdate -= period_;
        startSinceUpdate = sfClock.getElapsedTime();
      }
      sfWindow_.display();
    }
  }
};

} // namespace automaton
