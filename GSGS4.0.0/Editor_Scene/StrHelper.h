/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |
                                            \___/


                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/
#ifndef _gsgs_STRHELPER_H_
#define _gsgs_STRHELPER_H_

#include "NiiString.h"
#include "NiiColour.h"
#include <wx/colour.h>
#include <wx/strconv.h>
#include <wx/arrstr.h>

namespace gsgs
{
    class StrHelper
    {
    public:
        /** Converts a wxWidgets string into a GUI string, Unicode proof.*/
        static NII::String ToNIIString( const wxString& str )
        {
            return NII::String(wxConvLibc.cMB2WX( str ));
        }
 
        static NII::Nui32 ToHash(const wxString& str)
        {
            return NII::FastHash(NII::String(wxConvLibc.cMB2WX(str)));
        }

        /** Converts a ansi string into a GUI string.*/
        static const NII::Nutf8 * ToNutf8( const char* cstr )
        {
            return reinterpret_cast<const NII::Nutf8*>( cstr );
        }

        /** Converts a GUI string into a wxWidgets string, Unicode proof.*/
        static wxString ToWXString( const NII::String& str )
        {
            return wxString( wxConvLibc.cWC2WX( str.c_str() ) );
        }

        /** Converts a wxWidgets string into a float.*/
        static float WXStringToFloat( const wxString& str )
        {
            double dVal = 0;
            str.ToDouble( &dVal );
            return static_cast<float>( dVal );
        }

        /** Converts a wxWidgets string into an int.*/
        static int WXStringToInt( const wxString& str )
        {
            long val = 0;
            str.ToLong( &val );
            return static_cast<int>( val );
        }

        /** Converts a wxWidgets colour to a cegui colour .*/
        static NII::Colour WXColourToNiiColour( const wxColour& colour )
        {
            NII::Colour col( colour.Red() / 255.0f, colour.Green() / 255.0f, colour.Blue() / 255.0f );
            return col;
        }

        /** Adds a item to the end of the supplied wxWidgets array string, provided
         * that it isn't a duplicated entry.
         * @access public static
         * @qualifier none
         * @param arrayStr Array string to process
         * @param value Item to add to the array string
         * @return void
         */
        static void WXPushBackUnique(wxArrayString &arrayStr, wxString value)
        {
            // Verify if the supplied item already exists in the array string
            bool exists = false;
            for (wxArrayString::iterator it = arrayStr.begin(); it != arrayStr.end(); it++)
            {
                // Is this the same as the supplied item?
                if (*it == value)
                {
                    // Yes, so nothing more to do
                    exists = true;
                    break;
                }
            }

            // Add item to array string, if it wasn't found before
            if (!exists)
                arrayStr.push_back(value);
        }

        /** Helper function which returns all array elements in a single comma-separated and
         * newline-terminated string. The entries can themselves be separated by newlines or not.
         * @access public static
         * @qualifier none
         * @param inputArrayString Array string to process
         * @param addNewLine Adds a new line after each entry in the array string (true) or appends
         * them all sequentially in the same line (false)
         * @return wxString String with the array string entries
         * @remarks Based on wxWidgets (version 2.8.6) function AllAsString() found in aboutdlg.cpp
         */
        static wxString ArrayStringToString(const wxArrayString& inputArrayString, bool addNewLine = false)
        {
            // Allocate space for the result
            wxString result;
            const size_t count = inputArrayString.size();
            result.reserve(20 * count);

            // Scan all array string entries
            for (size_t n = 0; n < count; n++)
            {
                // Add entry (with newline afterwards if necessary)
                if (addNewLine)
                    result << inputArrayString[n] << wxT("\n");
                else
                    result << inputArrayString[n] << (n == count - 1 ? wxT("\n") : wxT(", "));
            }

            // We're done
            return result;
        }
    };
}
#endif
