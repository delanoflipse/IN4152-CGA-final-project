#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>

#include "../util/clock.cpp"

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