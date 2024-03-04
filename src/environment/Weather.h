#ifndef FAITHFUL_SRC_ENVIRONMENT_WEATHER_H_
#define FAITHFUL_SRC_ENVIRONMENT_WEATHER_H_

/// This header currently not in use (and not implemented)

namespace faithful {

struct Weather {};

class WeatherHandler {
 public:
  void SetWeather(Weather weather) {}
};

// TODO; there is no class Weather, but rather types of weather with traits:
//  - wind
//  - precipitations: rain / snow
//  - mist / fog

} // namespace faithful

#endif  // FAITHFUL_SRC_ENVIRONMENT_WEATHER_H_
