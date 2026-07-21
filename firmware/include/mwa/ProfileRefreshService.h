#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "mwa/MakerWorldProvider.h"

namespace mwa {

class ProfileRefreshService {
 public:
  ProfileRefreshService();
  bool request(const String& username);
  bool take(ProfileProbe& result);
  bool inFlight() const;

 private:
  struct WorkItem {
    ProfileRefreshService* owner;
    String username;
  };

  static void task(void* parameter);

  QueueHandle_t results_{nullptr};
  volatile bool inFlight_{false};
  MakerWorldProvider provider_;
};

}  // namespace mwa
