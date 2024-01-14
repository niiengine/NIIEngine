#获取文件
macro(get_nii_include CONTENTS KEYWORD VARIABLE)
  string(REGEX MATCH
    "${KEYWORD} *\n+NIIINCLUDE[ *\n*]*(.*)NIISOURCE.*\n+${KEYWORD}_END"
    PREPROC_TEMP_VAR
    ${${CONTENTS}}
  )
  if (CMAKE_MATCH_1)
    set(${VARIABLE} ${CMAKE_MATCH_1})
  else ()
    set(${VARIABLE} "CMAKENIIERROR")
  endif ()
endmacro()

macro(get_nii_source CONTENTS KEYWORD VARIABLE)
  string(REGEX MATCH
    "${KEYWORD} *\n+NIIINCLUDE.*NIISOURCE[ *\n*]*(.*)\n+${KEYWORD}_END"
    PREPROC_TEMP_VAR
    ${${CONTENTS}}
  )
  if (CMAKE_MATCH_1)
    set(${VARIABLE} ${CMAKE_MATCH_1})
  else ()
    set(${VARIABLE} "CMAKENIIERROR")
  endif ()
endmacro()

macro(get_nii_spec CONTENTS KEYWORD VARIABLE)
  string(REGEX MATCH
    "${KEYWORD} *\n+(.*)${KEYWORD}_END"
    PREPROC_TEMP_VAR
    ${${CONTENTS}}
  )
  if (CMAKE_MATCH_1)
    set(${VARIABLE} ${CMAKE_MATCH_1})
  else ()
    set(${VARIABLE} "CMAKENIIERROR")
  endif ()
endmacro()
