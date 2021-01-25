# Find libudev

find_path(udev_INCLUDE_DIR
  NAMES "libudev.h"
)

find_library(udev_LIBRARY
  NAMES "udev"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(udev
  FOUND_VAR udev_FOUND
  REQUIRED_VARS
    udev_LIBRARY
    udev_INCLUDE_DIR
)

if(udev_FOUND)
  add_library(udev::udev UNKNOWN IMPORTED)
  set_target_properties(udev::udev PROPERTIES
    IMPORTED_LOCATION "${udev_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${udev_INCLUDE_DIR}"
  )

  mark_as_advanced(
    udev_LIBRARY
    udev_INCLUDE_DIR
  )
endif()
