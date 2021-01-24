#include <libusb.h>

#include <csignal>
#include <iostream>
#include <stdexcept>

namespace {
int LIBUSB_CALL on_attach([[maybe_unused]] libusb_context *ctx,
                          [[maybe_unused]] libusb_device *dev,
                          [[maybe_unused]] libusb_hotplug_event event,
                          [[maybe_unused]] void *user_data) {

  libusb_device_descriptor desc{};
  auto rc = libusb_get_device_descriptor(dev, &desc);
  if (LIBUSB_SUCCESS != rc) {
    throw std::runtime_error{"libusb_get_device_descriptor"};
  }

  std::cout << "attach " << std::hex << desc.idVendor << ":" << desc.idProduct
            << "\n";

  // libusb_device_handle *handle = nullptr;
  // rc = libusb_open(dev, &handle);
  // if (LIBUSB_SUCCESS != rc) {
  //   handle = nullptr;
  // }
  // if (handle) {
  //   libusb_close(handle);
  // }

  return 0;
}

int LIBUSB_CALL on_detach([[maybe_unused]] libusb_context *ctx,
                          [[maybe_unused]] libusb_device *dev,
                          [[maybe_unused]] libusb_hotplug_event event,
                          [[maybe_unused]] void *user_data) {

  std::cout << "detach"
            << "\n";

  return 0;
}

libusb_hotplug_callback_handle hp[2]{};
bool listening = true;

void signal_handler([[maybe_unused]] int signal) {
  listening = false;
  std::cout << "shutting down..."
            << "\n";
  libusb_hotplug_deregister_callback(nullptr, hp[0]);
  libusb_hotplug_deregister_callback(nullptr, hp[1]);
}

} // namespace

int main() {
  int product_id = 0x1d9d;
  int vendor_id = 0x1052;
  int class_id = LIBUSB_HOTPLUG_MATCH_ANY;

  std::signal(SIGINT, signal_handler);

  auto rc = libusb_init(NULL);
  if (rc < 0) {
    throw std::runtime_error{"libusb_init"};
  }

  if (!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
    throw std::runtime_error{"libusb_has_capability"};
  }

  rc = libusb_hotplug_register_callback(
      NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, 0, vendor_id, product_id,
      class_id, on_attach, NULL, &hp[0]);
  if (LIBUSB_SUCCESS != rc) {
    throw std::runtime_error{"libusb_hotplug_register_callback"};
  }

  rc = libusb_hotplug_register_callback(NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
                                        0, vendor_id, product_id, class_id,
                                        on_detach, NULL, &hp[1]);
  if (LIBUSB_SUCCESS != rc) {
    throw std::runtime_error{"libusb_hotplug_register_callback"};
  }

  while (listening) {
    rc = libusb_handle_events(NULL);
    std::cout << "event..."
              << "\n";
    if (rc < 0) {
      // throw std::runtime_error{"libusb_handle_events"};
      listening = false;
    }
  }

  libusb_exit(NULL);
}
