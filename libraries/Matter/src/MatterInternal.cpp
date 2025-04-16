#include <sdkconfig.h>

#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
#include "MatterInternal.h"
#include <app/server/Server.h>

using namespace esp_matter;

namespace matter_internal {

constexpr auto k_timeout_seconds = 300;
static bool matter_stack_started = false;

// This callback is invoked for all Matter events
static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg) {
  switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
      log_i(
        "Interface %s Address changed", event->InterfaceIpAddressChanged.Type == chip::DeviceLayer::InterfaceIpChangeType::kIpV4_Assigned ? "IPv4" : "IPV6"
      );
      break;
    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:       log_i("Commissioning complete"); break;
    case chip::DeviceLayer::DeviceEventType::kFailSafeTimerExpired:        log_i("Commissioning failed, fail safe timer expired"); break;
    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted: log_i("Commissioning session started"); break;
    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStopped: log_i("Commissioning session stopped"); break;
    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowOpened:   log_i("Commissioning window opened"); break;
    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed:   log_i("Commissioning window closed"); break;
    case chip::DeviceLayer::DeviceEventType::kFabricRemoved:
    {
      log_i("Fabric removed successfully");
      if (chip::Server::GetInstance().GetFabricTable().FabricCount() == 0) {
        log_i("No fabric left, opening commissioning window");
        chip::CommissioningWindowManager &commissionMgr = chip::Server::GetInstance().GetCommissioningWindowManager();
        constexpr auto kTimeoutSeconds = chip::System::Clock::Seconds16(300);
        if (!commissionMgr.IsCommissioningWindowOpen()) {
          // After removing last fabric, it does not remove the Wi-Fi credentials and still has IP connectivity so, only advertising on DNS-SD.
          CHIP_ERROR err = commissionMgr.OpenBasicCommissioningWindow(kTimeoutSeconds, chip::CommissioningWindowAdvertisement::kDnssdOnly);
          if (err != CHIP_NO_ERROR) {
            log_e("Failed to open commissioning window, err:%" CHIP_ERROR_FORMAT, err.Format());
          }
        }
      }
      break;
    }
    case chip::DeviceLayer::DeviceEventType::kFabricWillBeRemoved: log_i("Fabric will be removed"); break;
    case chip::DeviceLayer::DeviceEventType::kFabricUpdated:       log_i("Fabric is updated"); break;
    case chip::DeviceLayer::DeviceEventType::kFabricCommitted:     log_i("Fabric is committed"); break;
    case chip::DeviceLayer::DeviceEventType::kBLEDeinitialized:    log_i("BLE deinitialized and memory reclaimed"); break;
    default:                                                       break;
  }
}

void start_matter_stack() {
  // Only start the Matter stack once
  if (!matter_stack_started) {
    // Start the Matter stack
    esp_err_t err = esp_matter::start(app_event_cb);
    if (err != ESP_OK) {
      log_e("Failed to start Matter, err:%d", err);
    } else {
      log_i("Matter stack started successfully");
      matter_stack_started = true;
    }
  } else {
    log_v("Matter stack already started");
  }
}

}  // namespace matter_internal

#endif /* CONFIG_ESP_MATTER_ENABLE_DATA_MODEL */
