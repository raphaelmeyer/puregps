# Find libusb

find_path(libusb_INCLUDE_DIR
  NAMES "libusb.h"
  PATH_SUFFIXES "libusb"
)

find_library(libusb_LIBRARY
  NAMES "usb"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libusb
  FOUND_VAR libusb_FOUND
  REQUIRED_VARS
    libusb_LIBRARY
    libusb_INCLUDE_DIR
  VERSION_VAR libusb_VERSION
)

add_library(libusb::libusb UNKNOWN IMPORTED)
set_target_properties(libusb::libusb PROPERTIES
  IMPORTED_LOCATION "${libusb_LIBRARY}"
  INTERFACE_INCLUDE_DIRECTORIES "${libusb_LIBRARY}"
)

mark_as_advanced(
  libusb_LIBRARY
  libusb_INCLUDE_DIR
)

mark_as_advanced(SFML_INCLUDE_DIR)
