
SET(yps_SOURCES yps.c)
ADD_EXECUTABLE(yps ${yps_SOURCES})
TARGET_LINK_LIBRARIES(yps satsolver)

SET(gimmick_SOURCES gimmick.c)
ADD_EXECUTABLE(gimmick ${gimmick_SOURCES})
TARGET_LINK_LIBRARIES(gimmick satsolver)


SET(deptestomatic_SOURCES deptestomatic.c)
ADD_EXECUTABLE(deptestomatic ${deptestomatic_SOURCES})
TARGET_LINK_LIBRARIES(deptestomatic satsolverext satsolver ${EXPAT_LIBRARY} ${ZLIB_LIBRARY})

install(TARGETS
	deptestomatic
   DESTINATION ${BIN_INSTALL_DIR} )

ENABLE_TESTING()
ADD_TEST(solver_testsuite ruby ${CMAKE_CURRENT_SOURCE_DIR}/runtest.rb -b ${CMAKE_BINARY_DIR} -s ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/data.libzypp/basic-exercises )

