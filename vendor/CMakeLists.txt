include(FetchContent)

FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/onqtam/doctest.git
  GIT_TAG        2.4.4
)

FetchContent_MakeAvailable(doctest)
