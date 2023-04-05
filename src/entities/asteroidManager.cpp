#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>

#include "../util/clock.cpp"
#include "../gamestate.cpp"

namespace entities
{
  class AsteroidManager
  {
  public:
    std::vector<entities::Asteroid *> asteroids;
    int targetAsteroids = 5;
    float nextSpawn = INFINITY;
    float minTime = 1;
    float maxTime = 5;

    void updateGenerate()
    {
      if (asteroids.size() == targetAsteroids)
      {
        return;
      }

      asteroids.push_back(new Asteroid());
    }

    void updateLocations()
    {
      auto iter = asteroids.begin();
      while (iter != asteroids.end())
      {
        entities::Asteroid *asteroid = *iter;
        asteroid->progress += asteroid->speed * timing::delta_s;
        asteroid->update();

        if (asteroid->progress >= 1)
        {
          gamestate::missed++;
          delete *iter;
          iter = asteroids.erase(iter);
        }
        else
        {
          ++iter;
        }
      }
    }

    void shootAt(glm::vec3 from, glm::vec3 dir)
    {

      auto iter = asteroids.begin();
      while (iter != asteroids.end())
      {
        entities::Asteroid *asteroid = *iter;
        bool hit = math::raySphereIntersect(from, dir, asteroid->currentPosition, 1.0f);
      
        if (hit)
        {
          gamestate::score++;
          delete *iter;
          iter = asteroids.erase(iter);
        }
        else
        {
          ++iter;
        }
      }
    }

    void update()
    {
      updateGenerate();
      updateLocations();
    };
  };
};