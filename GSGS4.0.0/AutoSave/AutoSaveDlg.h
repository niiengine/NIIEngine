//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2016 The CodeLite Team
// File name            : AutoSaveDlg.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef AUTOSAVEDLG_H
#define AUTOSAVEDLG_H
#include "AutoSaveUI.h"

#include "gsgsConfig.h"

class AutoSaveSettings : public gsgs::ObjectData
{
public:
    enum {
        kEnabled = (1 << 0),
    };

public:
    AutoSaveSettings();
    ~AutoSaveSettings();

    virtual void FromJSON(const gsgs::JSONObject& json);
    virtual gsgs::JSONObject ToJSON() const;

    static AutoSaveSettings Load();
    static void Save(const AutoSaveSettings& settings);

    AutoSaveSettings& EnableFlag(int flag, bool b = true)
    {
        b ? (m_flags |= flag) : (m_flags &= ~flag);
        return *this;
    }

    bool HasFlag(int flag) const { return m_flags & flag; }
    AutoSaveSettings& SetFlags(size_t flags)
    {
        this->m_flags = flags;
        return *this;
    }
    size_t GetFlags() const { return m_flags; }
    AutoSaveSettings& SetCheckInterval(size_t checkInterval)
    {
        this->m_checkInterval = checkInterval;
        return *this;
    }
    size_t GetCheckInterval() const { return m_checkInterval; }
protected:
    size_t m_flags;
    size_t m_checkInterval;
};

class AutoSaveDlg : public AutoSaveDlgBase
{
public:
    AutoSaveDlg(wxWindow* parent);
    virtual ~AutoSaveDlg();
protected:
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnEnabledUI(wxUpdateUIEvent& event);
};
#endif // AUTOSAVEDLG_H
