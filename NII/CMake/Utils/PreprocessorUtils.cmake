#获取版本号
macro(get_preprocessor_entry CONTENTS KEYWORD VARIABLE)
  string(REGEX MATCH
    "# *define +${KEYWORD} +((\"([^\n]*)\")|([^ \n]*))"
    PREPROC_TEMP_VAR
    ${${CONTENTS}}
  )
  if (CMAKE_MATCH_3)
    set(${VARIABLE} ${CMAKE_MATCH_3})
  else ()
    set(${VARIABLE} ${CMAKE_MATCH_4})
  endif ()
endmacro()
#判断是否存在版本号标识
macro(has_preprocessor_entry CONTENTS KEYWORD VARIABLE)
  string(REGEX MATCH
    "\n *# *define +(${KEYWORD})"
    PREPROC_TEMP_VAR
    ${${CONTENTS}}
  )
  if (CMAKE_MATCH_1)
    set(${VARIABLE} TRUE)
  else ()
    set(${VARIABLE} FALSE)
  endif ()
endmacro()
#替换定义宏的值
macro(replace_preprocessor_entry VARIABLE KEYWORD NEW_VALUE)
  string(REGEX REPLACE 
    "(// *)?# *define +${KEYWORD} +[^ \n]*"
	"#define ${KEYWORD} ${NEW_VALUE}"
	${VARIABLE}_TEMP
	${${VARIABLE}}
  )
  set(${VARIABLE} ${${VARIABLE}_TEMP})  
endmacro()
#启用或禁用定义宏
macro(set_preprocessor_entry VARIABLE KEYWORD ENABLE)
  if (${ENABLE})
    set(TMP_REPLACE_STR "#define ${KEYWORD}")
  else ()
    set(TMP_REPLACE_STR "// #define ${KEYWORD}")
  endif ()
  string(REGEX REPLACE 
    "(// *)?# *define +${KEYWORD} *\n"
	${TMP_REPLACE_STR}
	${VARIABLE}_TEMP
	${${VARIABLE}}
  )
  set(${VARIABLE} ${${VARIABLE}_TEMP})  
endmacro()