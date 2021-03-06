#
# CMakeLists.txt for sat-solver/tools
#

ADD_LIBRARY(satsolvertools STATIC common_write.c)

SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )

IF ( NOT DEBIAN )
ADD_EXECUTABLE(rpmdb2solv rpmdb2solv.c)
TARGET_LINK_LIBRARIES(rpmdb2solv satsolvertools satsolverext satsolver ${RPMDB_LIBRARY} ${EXPAT_LIBRARY})

ADD_EXECUTABLE(rpms2solv rpms2solv.c)
TARGET_LINK_LIBRARIES(rpms2solv satsolvertools satsolverext satsolver ${RPMDB_LIBRARY})

ADD_EXECUTABLE(findfileconflicts findfileconflicts.c)
TARGET_LINK_LIBRARIES(findfileconflicts satsolverext satsolver ${RPMDB_LIBRARY})

ENDIF ( NOT DEBIAN )

ADD_EXECUTABLE(rpmmd2solv rpmmd2solv.c)
TARGET_LINK_LIBRARIES(rpmmd2solv satsolvertools satsolverext satsolver ${EXPAT_LIBRARY} ${ZLIB_LIBRARY})

ADD_EXECUTABLE(helix2solv helix2solv.c)
TARGET_LINK_LIBRARIES(helix2solv satsolvertools satsolverext satsolver ${EXPAT_LIBRARY})

ADD_EXECUTABLE(susetags2solv susetags2solv.c)
TARGET_LINK_LIBRARIES(susetags2solv satsolvertools satsolverext satsolver ${ZLIB_LIBRARY})

ADD_EXECUTABLE(updateinfoxml2solv updateinfoxml2solv.c)
TARGET_LINK_LIBRARIES(updateinfoxml2solv satsolvertools satsolverext satsolver ${EXPAT_LIBRARY})

ADD_EXECUTABLE(deltainfoxml2solv deltainfoxml2solv.c)
TARGET_LINK_LIBRARIES(deltainfoxml2solv satsolvertools satsolverext satsolver ${EXPAT_LIBRARY})

ADD_EXECUTABLE(repomdxml2solv repomdxml2solv.c)
TARGET_LINK_LIBRARIES(repomdxml2solv satsolvertools satsolverext satsolver ${EXPAT_LIBRARY})

ADD_EXECUTABLE(installcheck installcheck.c)
TARGET_LINK_LIBRARIES(installcheck satsolverext satsolver ${EXPAT_LIBRARY} ${ZLIB_LIBRARY})

ADD_EXECUTABLE(patchcheck patchcheck.c)
TARGET_LINK_LIBRARIES(patchcheck satsolverext satsolver ${EXPAT_LIBRARY} ${ZLIB_LIBRARY})

ADD_EXECUTABLE(dumpsolv dumpsolv.c )
TARGET_LINK_LIBRARIES(dumpsolv satsolver)

ADD_EXECUTABLE(mergesolv mergesolv.c )
TARGET_LINK_LIBRARIES(mergesolv satsolvertools satsolverext satsolver)

install(TARGETS
    mergesolv
    dumpsolv
    susetags2solv
    helix2solv
    rpmmd2solv
    updateinfoxml2solv
    deltainfoxml2solv
    repomdxml2solv
    installcheck
    DESTINATION ${BIN_INSTALL_DIR})

IF ( NOT DEBIAN )
install(TARGETS
    rpmdb2solv
    rpms2solv
    DESTINATION ${BIN_INSTALL_DIR})
ENDIF ( NOT DEBIAN )

install(PROGRAMS
   repo2solv.sh
   DESTINATION ${BIN_INSTALL_DIR})

SET(satsolvertools_HEADERS common_write.h)

INSTALL(FILES ${satsolvertools_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/satsolver")
INSTALL(TARGETS satsolvertools LIBRARY DESTINATION ${LIB_INSTALL_DIR} ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
