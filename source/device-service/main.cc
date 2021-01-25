#include <libudev.h>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <chrono>
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

namespace {

bool listening = true;

void signal_handler([[maybe_unused]] int signal) {
  listening = false;
  std::cout << "shutting down..."
            << "\n";
  // ...
}

void probe(std::string device_name) {
  std::cout << "probe " << device_name << "\n";

  auto fd = ::open(device_name.c_str(), O_RDWR | O_NOCTTY);
  if (fd < 0) {
    throw std::runtime_error{"::open"};
  }

  termios options{};
  tcgetattr(fd, &options);
  options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
  options.c_oflag &= ~(ONLCR | OCRNL);
  options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  tcsetattr(fd, TCSANOW, &options);

  try {
    // TODO: computer says NO.

    std::uint8_t command{};
    std::uint8_t response[32]{};

    command = 0xfe;
    if (::write(fd, &command, 1) < 0) {
      throw std::runtime_error{"write 0xf4"};
    }
    if (::read(fd, response, 11) < 11) {
      throw std::runtime_error{"read 0xf4"};
    }
    std::cout << static_cast<uint16_t>(response[0]) << " "
              << static_cast<uint16_t>(response[1]) << "\n";
  } catch (std::runtime_error const &error) {
    std::cout << "error : " << error.what() << "\n";
  }

  ::close(fd);
}

} // namespace

int main() {
  std::signal(SIGINT, signal_handler);

  auto udev = udev_new();
  if (not udev) {
    throw std::runtime_error{"udev_new"};
  }

  auto mon = udev_monitor_new_from_netlink(udev, "udev");
  udev_monitor_filter_add_match_subsystem_devtype(mon, "tty", NULL);
  udev_monitor_enable_receiving(mon);
  auto fd = udev_monitor_get_fd(mon);

  while (listening) {
    fd_set fds{};
    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    timeval tv{};
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    auto const ret = select(fd + 1, &fds, NULL, NULL, &tv);
    if (ret > 0 && FD_ISSET(fd, &fds)) {
      auto dev = udev_monitor_receive_device(mon);
      if (dev) {
        std::string const action = udev_device_get_action(dev);
        std::cout << "action = " << action << "\n";
        // std::cout << "name = " << udev_device_get_sysname(dev) << "\n";
        // std::cout << "path = " << udev_device_get_devpath(dev) << "\n";

        std::string devname{};
        int32_t product_id{0};
        int32_t vendor_id{0};

        udev_list_entry *list_entry;
        udev_list_entry_foreach(list_entry,
                                udev_device_get_properties_list_entry(dev)) {
          std::string const name = udev_list_entry_get_name(list_entry);
          std::string const value = udev_list_entry_get_value(list_entry);

          if (name == "DEVNAME") {
            devname = value;
          }
          if (name == "ID_MODEL_ID") {
            product_id = std::stoi(value, 0, 16);
          }
          if (name == "ID_VENDOR_ID") {
            vendor_id = std::stoi(value, 0, 16);
          }

          // DEVNAME -> /dev/ttyACM0

          // ID_MODEL_ID -> 1052
          // ID_MODEL -> PURE_GPS
          // ID_MODEL_ENC -> PURE\x20GPS

          // ID_VENDOR_ID -> 1d9d
          // ID_VENDOR -> SIGMA_Elektro_GmbH
          // ID_VENDOR_ENC -> SIGMA\x20Elektro\x20GmbH
        }

        if (action == "add" && vendor_id == 0x1d9d && product_id == 0x1052) {
          probe(devname);
        }

        udev_device_unref(dev);
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }

  udev_unref(udev);
}
