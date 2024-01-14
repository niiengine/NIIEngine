
#ifndef ICONFIGTOOL_H
#define ICONFIGTOOL_H

#include "gsgsPreInclude.h"
#include "gsgsSerializer.h"

namespace gsgs
{
    /**
     * @class ConfigTool
     * @author Eran
     * @date 05/07/08
     * @file iconfigtool.h
     * @brief a configuration tool that allow plugin easy access to CodeLite's configuration file. Plugin should only read/write its own
     * values
     * @sa ObjectData
     */
    class _gsgsAPI ConfigTool {
    public:
        ConfigTool() {}
        virtual ~ConfigTool();

        /**
         * @brief read an object from the configuration file and place it into obj
         * @param name the name of the object that is stored in the configuration file
         * @param obj a pointer previsouly allocated ObjectData
         * @return true if the object exist and was read successfully from the configuration file
         */
        virtual bool ReadObject(const wxString &name, ObjectData *obj) = 0;

        /**
         * @brief write an object to the configuration file
         * @param name the name that identifies the object
         * @param obj object to store.
         * @return true on success, false otherwise
         */
        virtual bool WriteObject(const wxString &name, ObjectData *obj, bool event = true) = 0;
    };
}
#endif
