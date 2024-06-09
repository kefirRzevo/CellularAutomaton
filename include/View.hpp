#pragma once

#include <SFML/Graphics.hpp>

#include "Model.hpp"

namespace automaton {

class Polygon;

class View {
  Model& model_;

  sf::RenderWindow sfWindow_;
  sf::View sfView_;
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
    auto scaleX = sfView_.getSize().x / sfTexture_.getSize().x;
    auto scaleY = sfView_.getSize().y / sfTexture_.getSize().y;
    sfSprite.scale(scaleX, scaleY);
    sfWindow_.clear(sf::Color::Black);
    sfWindow_.draw(sfSprite);
  }

public:
  View(Model& model, unsigned int width = 800, unsigned int height = 600)
  : model_(model) {
    const auto& poly = model.getPolygon();
    sfWindow_.create(sf::VideoMode(width, height), "Automata");
    sfView_ = sfWindow_.getDefaultView();
    sfTexture_.create(poly.getWidth(), poly.getHeight());
    buf_.resize(poly.getWidth() * poly.getHeight());
    buf_.shrink_to_fit();
  }

  void run() {
    sf::Event sfEvent;
    model_.fill();
    while(sfWindow_.isOpen()) {
      while (sfWindow_.pollEvent(sfEvent)) {
        if (sfEvent.type == sf::Event::Closed) {
            sfWindow_.close();
        } else if (sfEvent.type == sf::Event::Resized) {
          sf::FloatRect sfRect{0., 0.,
            static_cast<float>(sfEvent.size.width),
            static_cast<float>(sfEvent.size.height)
          };
          sfView_ = sf::View{sfRect};
          sfWindow_.setView(sfView_);
        }
      }
      drawPolygon(model_.getPolygon());
      sfWindow_.display();
    }
  }
};

} // namespace automaton
