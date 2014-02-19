
#include <cstdio>
#include <cmath>
#include <iostream>
#include <vector>
#include "terrain.hpp"
#include "soccerball.hpp"
#include "player.hpp"

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
  std::vector<Player*> players;
  players.push_back(new Player());
  players[0]->setPosition(-2.0,-4.0);
  players.push_back(new Player());
  players[1]->setPosition(-20.0,80.0);
  players.push_back(new Player());
  players[2]->setPosition(0.0,16.0);
  
  Player* control = players[0];
  
  
  sf::View cam(sf::Vector2f(0,0),sf::Vector2f(terrain.width + 1,6.0*terrain.width/8.0));
  sf::RenderWindow window(sf::VideoMode(800, 600), "Jeu Foot");
  
  window.setView(cam);
  window.setFramerateLimit(50);
  
  
  
  float dt = 0.02;
  float zoom = 0.5;
  
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
          break;
        case sf::Event::MouseButtonPressed:
          if (event.mouseButton.button == sf::Mouse::Left)
          {
            ball.acceleration += sf::Vector3f(0.0,0.0,100.0/0.410);
          }
          break;
        default:
          break;
      }
    }
    
    //Calcul de la camera 
    sf::Vector2f cam_delta(0,0);
    float d =sqrt((ball.getPosition().x-control->getPosition().x)*(ball.getPosition().x-control->getPosition().x)+
			(ball.getPosition().y-control->getPosition().y)*(ball.getPosition().y-control->getPosition().y));
    zoom = 0.3+0.02*d;
    zoom += ball.z*0.05;
    
    cam_delta.x = (ball.getPosition().x - (ball.getPosition().x - control->getPosition().x)*0.5 - cam.getCenter().x)*dt*3.0;
    cam_delta.y = (ball.getPosition().y - (ball.getPosition().y - control->getPosition().y)*0.5 - cam.getCenter().y)*dt*3.0;
    
    cam.setSize(terrain.width+1,terrain.width*(float)window.getSize().y/(float)window.getSize().x + 1);
    
    
      
    
    /*if (zoom*cam.getSize().x > terrain.width*0.51)
      zoom = (terrain.width*0.51)/cam.getSize().x;*/
    cam.zoom(zoom);
    cam.move(cam_delta);
    
    if (cam.getCenter().x + cam.getSize().x*0.5 > terrain.width*0.5 && 
	cam.getCenter().x-cam.getSize().x*0.5 < -terrain.width*0.5)
    {
      cam.setCenter(0.0,cam.getCenter().y);
      cam.setSize(terrain.width+1,terrain.width*(float)window.getSize().y/(float)window.getSize().x + 1);
    }
    
    else if (cam.getCenter().x + cam.getSize().x*0.5 > terrain.width*0.5 && cam.getCenter().x > 0.0)
    {
      cam.setCenter((terrain.width+1-cam.getSize().x)*0.5,cam.getCenter().y);
    }
    else if (cam.getCenter().x-cam.getSize().x*0.5 < -terrain.width*0.5 && cam.getCenter().x < -0.0)
    {
	cam.setCenter((-terrain.width-1+cam.getSize().x)*0.5,cam.getCenter().y);
    }
    /*
    if (cam.getCenter().y + cam.getSize().y*0.5 > terrain.height*0.5)
    {
      cam.setCenter(cam.getCenter().x,terrain.height*0.5-cam.getSize().y*0.5);
    }
    else if (cam.getCenter().y - cam.getSize().y*0.5 < -terrain.height*0.5)
    {
      cam.setCenter(cam.getCenter().x,-terrain.height*0.5+cam.getSize().y*0.5);
    }*/
      
    
    
   /* if (cam.getSize().x*0.5 + cam.getCenter().x > terrain.width*0.5)
    {
	cam.setSize(terrain.width-cam.getCenter().x + 1,
		   (terrain.width-cam.getCenter().x)*(float)window.getSize().y/(float)window.getSize().x + 1);
    }*/
    
    
    /*else if (cam.getSize().x/0.5 + cam.getCenter().x < terrain.width)
    {
	cam.setSize(terrain.width*0.5 - cam.getCenter().x,
		   (terrain.width*0.5 - cam.getCenter().x)*(float)window.getSize().y/(float)window.getSize().x + 1);
    }*/
    
    
    window.setView(cam);
    /*
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && cam.getCenter().y < terrain.height/2) {
      cam.move(0.0,0.4);
      window.setView(cam);
    }
    
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && cam.getCenter().y > -terrain.height/2) {
      cam.move(0.0,-0.4);
      window.setView(cam);
    }*/
    
    control->_acceleration.x = 0.0;
    control->_acceleration.y = 0.0;
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
      control->_acceleration.y -= 10.0;
    }
    
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
      control->_acceleration.y += 10.0;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
      control->_acceleration.x -= 10.0;
    }
    
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
      control->_acceleration.x += 10.0;
    }
    
    
    terrain.update();
    for (unsigned int i=0;i<players.size();i++)
    {
      if (players[i] != control)
      {
	players[i]->think(ball,players,players);
	
	/*float norm = sqrt((ball.getPosition().x-players[i]->getPosition().x)*(ball.getPosition().x-players[i]->getPosition().x)+
			  (ball.getPosition().y-players[i]->getPosition().y)*(ball.getPosition().y-players[i]->getPosition().y));

	if (norm > 0.5)
	players[i]->move(5.0*dt*(ball.getPosition().x-players[i]->getPosition().x)/norm,
		    5.0*dt*(ball.getPosition().y-players[i]->getPosition().y)/norm);*/
      
      }
      players[i]->update(dt);
      
    }
    
    //poids P = mg
    ball.acceleration.z -= 9.81;
    
    if (ball.z > 0.01) //Balle en l'air
    {
      //frottements de l'air R=1/2 * Cx * p * S * v^2
      ball.acceleration -= fake_v3pow2(ball.velocity)*(float)(0.5*0.5*1.1845*0.06);
    }
    else //Balle au sol
    {
      if (ball.z < 0.0)  ball.z=0.0; //balle dans la terre
    
      //std::cout << "frottement de terrain ";
      //frottements de terrain IREEL sinon la balle ne veut pas se stopper 0.5^2 trop faible
      ball.acceleration.x -= ball.velocity.x*0.7f;
      ball.acceleration.y -= ball.velocity.y*0.7f;
      
      //réaction du support = au poid
      ball.acceleration.z += 9.81;
      
      //réaction de vitesse
      if (ball.velocity.z < 0.0) 
      {
        
        //ball.acceleration.z += ball.velocity.z*ball.velocity.z*5.0/0.410;
        //Potable mais iréaliste
        ball.acceleration.z -= ball.velocity.z*90.0;
      }
    }
    
    for (int i=0;i<players.size();i++)
    {
      if (ball.intersect(players[i]->getPosition().x, players[i]->getPosition().y) && ball.z < 2.0)
      {
	ball.acceleration += sf::Vector3f(700.0*(ball.getPosition().x-players[i]->getPosition().x),
					  700.0*(ball.getPosition().y-players[i]->getPosition().y),
					  100.0 - ball.z*100.0);
      }
    }
    

    //Sortie de terrain
    if (ball.getPosition().x < -terrain.width/2) {
      ball.setPosition(-terrain.width/2,ball.getPosition().y);
      ball.z=0;
      ball.acceleration=sf::Vector3f(0,0,0);
      ball.velocity=sf::Vector3f(0,0,0);
    }
    if (ball.getPosition().x > terrain.width/2) {
      ball.setPosition(terrain.width/2,ball.getPosition().y);
      ball.z=0;
      ball.acceleration=sf::Vector3f(0,0,0);
      ball.velocity=sf::Vector3f(0,0,0);
    }
    if (ball.getPosition().y < -terrain.height/2) {
      ball.setPosition(ball.getPosition().x,-terrain.height/2);
      ball.z=0;
      ball.acceleration=sf::Vector3f(0,0,0);
      ball.velocity=sf::Vector3f(0,0,0);
    }
    if (ball.getPosition().y > terrain.height/2) {
      ball.setPosition(ball.getPosition().x,terrain.height/2);
      ball.z=0;
      ball.acceleration=sf::Vector3f(0,0,0);
      ball.velocity=sf::Vector3f(0,0,0);
    }
    
    ball.update(dt);
    
    // Clear screen
    window.clear();
    //std::cout << "ball z " << ball.z << " velocity^2 " << vec3power(ball.velocity) << std::endl;
    std::cout << "ball speed " << vec3power(ball.velocity)*3.6 << " km/h" << std::endl; 
    window.draw(terrain);
    window.draw(ball);
    for (int i=0;i<players.size();i++)
    {
	window.draw(*(players[i]));
    }
    
    // Update the window
    window.display();
  }
  return 0xdead;
}