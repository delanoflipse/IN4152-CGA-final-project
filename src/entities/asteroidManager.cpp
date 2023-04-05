#pragma once

#include "../util/clock.cpp"
#include "../util/vector.cpp"
#include "../util/random.cpp"
#include "../gamestate.cpp"

namespace entities
{
  class AsteroidManager
  {
  public:
    std::vector<entities::Asteroid *> asteroids;
    int targetAsteroids = 8;
    float nextSpawn = 0;
    float minTime = 1;
    float maxTime = 5;

    void updateGenerate(glm::vec3 centerpos)
    {
      if (asteroids.size() == targetAsteroids)
      {
        return;
      }

      if (timing::time_s < nextSpawn)
      {
        return;
      }

      asteroids.push_back(new Asteroid(centerpos));
      nextSpawn = timing::time_s + random::randomRange(0.01f, 0.25f);
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

    glm::vec3 toAsteroidSpace(entities::Asteroid *asteroid, glm::vec3 realPoint)
    {
      glm::vec4 transformed = asteroid->inverseBaseTransform * glm::vec4(realPoint, 1.0f);
      return math::vec4toVec3(transformed);
    }

    void shootAt(glm::vec3 from, glm::vec3 dir)
    {

      auto iter = asteroids.begin();
      while (iter != asteroids.end())
      {
        entities::Asteroid *asteroid = *iter;
        
        bool hit = math::raySphereIntersect(
          toAsteroidSpace(asteroid, from),
          toAsteroidSpace(asteroid, dir),
          toAsteroidSpace(asteroid, asteroid->currentPosition),
          1.0f
        );
      
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

    bool hitBy(glm::vec3 shipLocation, float shipRadius)
    {
      for (auto asteroid : asteroids)
      {
        bool hit = math::sphereSphereIntersect(
          toAsteroidSpace(asteroid, shipLocation),
          shipRadius,
          toAsteroidSpace(asteroid, asteroid->currentPosition),
          1.0f
        );

        if (hit) {
          return true;
        }
      }
      return false;
    }

    void update(glm::vec3 centerpos)
    {
      updateGenerate(centerpos);
      updateLocations();
    };
  };
};