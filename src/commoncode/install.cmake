
GET_TARGET_PROPERTY(TARGET_TYPE ${PROGNAME} TYPE)

if (NOT ${TARGET_TYPE} STREQUAL STATIC_LIBRARY)
  INSTALL(TARGETS ${PROGNAME}
      RUNTIME DESTINATION bin
      LIBRARY DESTINATION lib
  )

ENDIF()

if (${TARGET_TYPE} STREQUAL SHARED_LIBRARY)
  set_property(TARGET ${PROGNAME} PROPERTY SOVERSION ${TESTCOCOON_VERSION_DOT})
ENDIF()
