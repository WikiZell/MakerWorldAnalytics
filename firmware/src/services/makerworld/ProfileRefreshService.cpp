#include "mwa/ProfileRefreshService.h"

namespace mwa {

ProfileRefreshService::ProfileRefreshService() { results_ = xQueueCreate(1, sizeof(ProfileProbe*)); }

bool ProfileRefreshService::request(const String& username) {
  if (results_ == nullptr || inFlight_ || username.isEmpty()) return false;
  auto* work = new WorkItem{this, username};
  if (work == nullptr) return false;
  inFlight_ = true;
  const BaseType_t started = xTaskCreatePinnedToCore(task, "mwa-profile", 8192, work, 1, nullptr, 0);
  if (started != pdPASS) {
    inFlight_ = false;
    delete work;
    return false;
  }
  return true;
}

bool ProfileRefreshService::take(ProfileProbe& result) {
  if (results_ == nullptr) return false;
  ProfileProbe* pending = nullptr;
  if (xQueueReceive(results_, &pending, 0) != pdTRUE || pending == nullptr) return false;
  result = *pending;
  delete pending;
  return true;
}

bool ProfileRefreshService::inFlight() const { return inFlight_; }

void ProfileRefreshService::task(void* parameter) {
  auto* work = static_cast<WorkItem*>(parameter);
  auto* result = new ProfileProbe(work->owner->provider_.probePublicProfile(work->username));
  if (result != nullptr && xQueueSend(work->owner->results_, &result, 0) != pdTRUE) delete result;
  work->owner->inFlight_ = false;
  delete work;
  vTaskDelete(nullptr);
}

}  // namespace mwa
