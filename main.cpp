#include <SFML\Graphics.hpp>
#include <iostream>
#include <cmath>

#define DEG_TO_RAD 0.017453292519943295769236907684886

uint16_t mapW = 10000, mapH = 10000;

class Eat {
  sf::CircleShape circle;
  float_t size;
public:
  Eat() {
    circle.setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
    circle.setPosition(sf::Vector2f(rand() % mapW, rand() % mapH));
    size = float_t(rand() % 10);
    circle.setRadius(size );
    circle.setPointCount(circle.getRadius() * 2.0F);
    circle.setOrigin(circle.getRadius(), circle.getRadius());
    size /= 10;
  }

  sf::Vector2f getPoint() {
    return circle.getPosition();
  }

  sf::FloatRect getRect() {
    return circle.getGlobalBounds();
  }

  float_t getSize() {
    return size;
  }

  void draw(sf::RenderWindow &window) {
    window.draw(circle);
  }
};
std::vector<Eat> eat(10000);

class Player {
  std::vector<sf::CircleShape> body;
  sf::View view;
  int16_t degree_ = 0;
  float_t remain_ = 0.0F;
  float_t bodyWidth_ = 10.0F;
public:
  bool doPause = false;
  bool doSpeedUp = false;

  bool chShrink = false;
  bool chGrow = false;
  bool chRestart = false;

  Player() {
    for(uint8_t i = 0; i < 10; i++) {
      addBodyPart();
    }
    body.front().setPosition(mapW / 2, mapH / 2);
  }

  sf::FloatRect getRect() {
    return body.front().getGlobalBounds();
  }

  void addBodyPart() {
    body.emplace_back();
    body.back().setRadius(bodyWidth_);
    body.back().setPointCount(20);
    body.back().setPosition(-100.0F, -100.0F);
    body.back().setFillColor(sf::Color(0, 0, body.size()));
    body.back().setOrigin(bodyWidth_, bodyWidth_);
  }

  void remBodyPart() {
    if(body.size() > 10) {
      body.pop_back();
    }
  }

  void setBodyWidth(float_t bodyWidth) {
    bodyWidth_ = bodyWidth;
    for(uint32_t i = 0; i < body.size(); i++) {
      body[i].setRadius(bodyWidth_);
      body[i].setOrigin(bodyWidth_, bodyWidth_);
    }
  }

  void addBodyWidth() {
    bodyWidth_ += 0.1F;
    for(uint32_t i = 0; i < body.size(); i++) {
      body[i].setRadius(bodyWidth_);
      body[i].setOrigin(bodyWidth_, bodyWidth_);
    }
  }

  void remBodyWidth() {
    bodyWidth_ -= 0.1F;
    for(uint32_t i = 0; i < body.size(); i++) {
      body[i].setRadius(bodyWidth_);
      body[i].setOrigin(bodyWidth_, bodyWidth_);
    }
  }

  void eat(float_t eatSize) {
    uint8_t bodyCount = static_cast<uint8_t>(eatSize);
    remain_ += eatSize - static_cast<float_t>(bodyCount);
    while(remain_ > 1) {
      remain_--;
      bodyCount++;
    }

    for(uint8_t i = 0; i < bodyCount; i++) {
      addBodyPart();
    }

    setBodyWidth(body.size() / 200.0F + 10.0F);
  }

  void addDegree(int16_t degree) {
    if(!doPause) {
      degree_ += degree;
      if(degree_ > 360) {
        degree_ = 0;
      }
      if(degree_ < 0) {
        degree_ = 360;
      }
    }
  }

  void update() {
    if(!doPause) {
      if(chRestart) {
        chRestart = false;
        for(uint32_t i = 0; i < body.size(); i++) {
          body[i].setPosition(mapW / 2, mapH / 2);
        }
        return;
      }
      sf::Vector2f prevPos = body.front().getPosition();
      sf::Vector2f nextPos;
      if(doSpeedUp) {
        body.front().move(std::sinf(degree_ * DEG_TO_RAD) * 4.0F, std::cosf(degree_ * DEG_TO_RAD) * 4.0F);
      }
      else {
        body.front().move(std::sinf(degree_ * DEG_TO_RAD) * 2.5F, std::cosf(degree_ * DEG_TO_RAD) * 2.5F);
      }
      for(uint32_t i = 1; i < body.size(); i++) {
        nextPos = body[i].getPosition();
        body[i].setPosition(prevPos);
        prevPos = nextPos;
      }
    }
  }

  void draw(sf::RenderWindow &window, sf::FloatRect viewRect) {
    for(uint32_t i = body.size() - 1; i != 0; i--) {
      if(viewRect.intersects(body[i].getGlobalBounds())) {
        window.draw(body[i]);
      }
    }
  }
};
Player player;

int main() {
  sf::ContextSettings contextSettings;
  contextSettings.antialiasingLevel = 8;

  sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Circle", sf::Style::Fullscreen, contextSettings);
  window.setFramerateLimit(60);
  window.setKeyRepeatEnabled(false);
  window.setMouseCursorVisible(false);

  sf::Event event;

  sf::View view;
  view.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));

  sf::FloatRect viewRect, plRect;
  while(window.isOpen()) {
    while(window.pollEvent(event)) {
      if(event.type == sf::Event::Closed) {
        window.close();
      }

      if(event.type == sf::Event::KeyPressed) {
        switch(event.key.code) {
          case sf::Keyboard::P:
          {
            player.doPause = !player.doPause;
            break;
          }
          case sf::Keyboard::L:
          {
            player.chRestart = true;
            break;
          }

          case sf::Keyboard::W:
          {
            player.doSpeedUp = true;
            break;
          }
        }
      }
      if(event.type == sf::Event::KeyReleased) {
        switch(event.key.code) {
          case sf::Keyboard::W:
          {
            player.doSpeedUp = false;
            break;
          }
        }
      }
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
      player.addDegree(3);
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
      player.addDegree(-3);
    }

    player.update();
    plRect = player.getRect();
    viewRect = sf::FloatRect(plRect.left + plRect.width / 2 - view.getSize().x / 2 - 25, plRect.top + plRect.height / 2 - view.getSize().y / 2 - 25, view.getSize().x + 50, view.getSize().y + 50);

    view.setCenter(plRect.left + plRect.width / 2, plRect.top + plRect.height / 2);
    window.clear(sf::Color(64, 64, 64));
    window.setView(view);
    for(uint32_t i = 0; i < eat.size(); i++) {
      if(viewRect.contains(eat[i].getPoint())) {
        eat[i].draw(window);
        uint16_t distance = std::sqrt(std::pow(((plRect.left + plRect.width / 2) - eat[i].getPoint().x), 2) + std::pow(plRect.top + plRect.height / 2 - eat[i].getPoint().y, 2));
        if(distance < 25) {
          player.eat(eat[i].getSize());
          eat.erase(eat.begin() + i);
        }
      }
    }
    player.draw(window, viewRect);
    window.display();
  }
}