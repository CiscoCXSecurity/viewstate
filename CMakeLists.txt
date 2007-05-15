PROJECT (viewstate)
ADD_EXECUTABLE(viewstate viewstate.cpp base64.cpp type_convert.cpp)
INSTALL(TARGETS viewstate DESTINATION bin)
