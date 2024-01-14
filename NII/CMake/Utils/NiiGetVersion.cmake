#-------------------------------------------------------------------
# This file is part of the CMake build system for NII
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http://www.ogre3d.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

include(PreprocessorUtils)

macro(nii_get_version SOURCEFILE)
  file(READ ${SOURCEFILE} TEMP_VAR_CONTENTS)
  get_preprocessor_entry(TEMP_VAR_CONTENTS NII_VERSION_MAJOR NII_VERSION_MAJOR)
  get_preprocessor_entry(TEMP_VAR_CONTENTS NII_VERSION_MINOR NII_VERSION_MINOR)
  get_preprocessor_entry(TEMP_VAR_CONTENTS NII_VERSION_PATCH NII_VERSION_PATCH)
  get_preprocessor_entry(TEMP_VAR_CONTENTS NII_VERSION_NAME NII_VERSION_NAME)
  get_preprocessor_entry(TEMP_VAR_CONTENTS NII_VERSION_SUFFIX NII_VERSION_SUFFIX)
  set(NII_VERSION "${NII_VERSION_NAME}-${NII_VERSION_MAJOR}.${NII_VERSION_MINOR}.${NII_VERSION_PATCH}")
  set(NII_SOVERSION "${NII_VERSION_MAJOR}.${NII_VERSION_MINOR}.${NII_VERSION_PATCH}")
  set(NII_VERSION_DASH_SEPARATED "${NII_VERSION_MAJOR}-${NII_VERSION_MINOR}-${NII_VERSION_PATCH}${NII_VERSION_SUFFIX}")

endmacro()
