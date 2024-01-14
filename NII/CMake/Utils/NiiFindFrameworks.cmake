
# - helper module to find OSX frameworks
# Adapted from standard CMake version, but added dependencies
# Standard finder does not look in any variable locations such as 
# CMAKE_FRAMEWORK_PATH (not sure why not)

IF(NOT NII_FIND_FRAMEWORKS_INCLUDED)
  SET(NII_FIND_FRAMEWORKS_INCLUDED 1)
  MACRO(NII_FIND_FRAMEWORKS fwk)
    IF(APPLE)
      SET(${fwk}_FRAMEWORKS)
      SET(NII_FRAMEWORK_PATH
		${NII_DEPENDENCIES_DIR}
    	~/Library/Frameworks
    	/Library/Frameworks
    	/System/Library/Frameworks
    	/Network/Library/Frameworks
	  )
	  FOREACH(dir ${NII_FRAMEWORK_PATH})
	    SET(fwkpath ${dir}/${fwk}.framework)
	    IF(EXISTS ${fwkpath})
          SET(${fwk}_FRAMEWORKS ${${fwk}_FRAMEWORKS} ${fwkpath})
        ENDIF(EXISTS ${fwkpath})
      ENDFOREACH(dir)
    ENDIF(APPLE)
  ENDMACRO(NII_FIND_FRAMEWORKS)
ENDIF(NOT NII_FIND_FRAMEWORKS_INCLUDED)
