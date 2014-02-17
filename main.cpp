
#include <cstdio>
#include <cmath>
#include <iostream>
#include "terrain.hpp"
#include "soccerball.hpp"
#include "Player.hpp"

sf::Vector3f v3abs(const sf::Vector3f& v)
{
  return sf::Vector3f(abs(v.x),abs(v.y),abs(v.z));
}

sf::Vector3f fake_v3pow2(const sf::Vector3f& v)
{
  return sf::Vector3f(abs(v.x)*v.x,abs(v.y)*v.y,abs(v.z)*v.z);
}

float vec3power(const sf::Vector3f& v)
{
  return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

int main()
{
  srand(time(NULL));
  BasicTerrain terrain;
  SoccerBall ball;
  Player player;
  
  sf::View cam(sf::Vector2f(0,0),sf::Vector2f(terrain.width + 1,6.0*terrain.width/8.0));
  sf::RenderWindow window(sf::VideoMode(800, 600), "Jeu Foot");
  
  window.setView(cam);
  window.setFramerateLimit(60);
  
  
  
  float dt = 0.16;
  
  while (window.isOpen())
  {
  // Process events
    sf::Event event;
    
    ball.acceleration = sf::Vector3f(0,0,0);
    while (window.pollEvent(event))
    {
      
      switch (event.type)
      {
        case sf::Event::Closed:
          window.close(); // Close window : exit
          break;
        case sf::Event::Resized:
          cam.setSize(terrain.width + 1,terrain.width*event.size.height/event.size.width);
          window.setView(cam);
          break;
        case sf::Event::MouseButtonPressed:
          if (event.mouseButton.button == sf::Mouse::Left)
          {
            ball.acceleration += sf::Vector3f((25 - rand()%50) * 0.1, (25 - rand()%50) * 0.1, 40.0);
          }
          break;
        default:
          break;
      }
    }
    
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && cam.getCenter().y < terrain.height/2) {
      cam.move(0.0,0.4);
      window.setView(cam);
    }
    
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && cam.getCenter().y > -terrain.height/2) {
      cam.move(0.0,-0.4);
      window.setView(cam);
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
      player.move(0.0,-0.2);
    }
    
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
      player.move(0.0,0.2);
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
      player.move(-0.2,0.0);
    }
    
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
      player.move(0.2,0.0);
    }
    
    terrain.update();
    
    
    if (ball.z >= 0.1)
    {
      //frottements de l'air R=1/2 * Cx * p * S * v^2
      ball.acceleration -= fake_v3pow2(ball.velocity)*(float)(0.5*0.5*1.1845*0.6);
    }
    else
    {
      //std::cout << "frottement de terrain" << std::endl;
      float old_z=ball.acceleration.z;
      //frottements de terrain IREEL sinon la balle ne veut pas se stopper 0.5^2 trop faible
      ball.acceleration -= ball.velocity*0.1f;
      ball.acceleration.z=old_z;
    }
    //poids P = mg
      ball.acceleration.z -= 9.81*0.410;
 
    if (ball.z < 0.0) { 
      ball.z=0.0;
      ball.acceleration.z += ball.velocity.z*ball.velocity.z; 
      ball.velocity.z=0;
    }

    //Sortie de terrain
    if (ball.getPosition().x < -terrain.width/2) ball.setPosition(-terrain.width/2,ball.getPosition().y);
    if (ball.getPosition().x > terrain.width/2) ball.setPosition(terrain.width/2,ball.getPosition().y);
    if (ball.getPosition().y < -terrain.height/2) ball.setPosition(ball.getPosition().x,-terrain.height/2);
    if (ball.getPosition().y > terrain.height/2) ball.setPosition(ball.getPosition().x,terrain.height/2);
    
    ball.update(dt);
    
    // Clear screen
    window.clear();
    //std::cout << "ball z " << ball.z << " velocity^2 " << vec3power(ball.velocity) << std::endl;
   
    window.draw(terrain);
    window.draw(ball);
    window.draw(player);
    // Update the window
    window.display();
  }
  return 0xdead;
}