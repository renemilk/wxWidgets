/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/variant.cpp
// Purpose:     wxVariant class, container for any type
// Author:      Julian Smart
// Modified by:
// Created:     10/09/98
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/variant.h"

#if wxUSE_VARIANT

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/math.h"
    #include "wx/crt.h"
    #if wxUSE_STREAMS
        #include "wx/stream.h"
    #endif
#endif

#if wxUSE_STD_IOSTREAM
    #if wxUSE_IOSTREAMH
        #include <fstream.h>
    #else
        #include <fstream>
    #endif
#endif

#if defined(__MWERKS__) && __MSL__ >= 0x6000
namespace std {}
using namespace std ;
#endif

#if wxUSE_STREAMS
    #include "wx/txtstrm.h"
#endif

#include "wx/string.h"
#include "wx/tokenzr.h"

wxVariant WXDLLIMPEXP_BASE wxNullVariant;


#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxVariantList)

/*
 * wxVariant
 */

IMPLEMENT_DYNAMIC_CLASS(wxVariant, wxObject)

wxVariant::wxVariant()
{
    m_data = (wxVariantData*) NULL;
}

bool wxVariant::IsNull() const
{
     return (m_data == (wxVariantData*) NULL);
}

void wxVariant::MakeNull()
{
    UnRef();
}

void wxVariant::Clear()
{
    m_name = wxEmptyString;
}

wxVariant::wxVariant(const wxVariant& variant)
    : wxObject()
{
    m_data = (wxVariantData*) NULL;

    if (!variant.IsNull())
        Ref(variant);

    m_name = variant.m_name;
}

wxVariant::wxVariant(wxVariantData* data, const wxString& name) // User-defined data
{
    m_data = data;
    m_name = name;
}

wxVariant::~wxVariant()
{
    UnRef();
}

// Assignment
void wxVariant::operator= (const wxVariant& variant)
{
    Ref(variant);
    m_name = variant.m_name;
}

// myVariant = new wxStringVariantData("hello")
void wxVariant::operator= (wxVariantData* variantData)
{
    UnRef();
    m_data = variantData;
}

bool wxVariant::operator== (const wxVariant& variant) const
{
    if (IsNull() || variant.IsNull())
        return (IsNull() == variant.IsNull());

    return (GetData()->Eq(* variant.GetData()));
}

bool wxVariant::operator!= (const wxVariant& variant) const
{
    return (!(*this == variant));
}

wxString wxVariant::MakeString() const
{
    if (!IsNull())
    {
        wxString str;
        if (GetData()->Write(str))
            return str;
    }
    return wxEmptyString;
}

void wxVariant::SetData(wxVariantData* data)
{
    UnRef();
    m_data = data;
}

void wxVariant::Ref(const wxVariant& clone)
{
    // nothing to be done
    if (m_data == clone.m_data)
        return;

    // delete reference to old data
    UnRef();

    // reference new data
    if ( clone.m_data )
    {
        m_data = clone.m_data;
        m_data->m_count++;
    }
}


void wxVariant::UnRef()
{
    if ( m_data )
    {
        wxASSERT_MSG( m_data->m_count > 0, _T("invalid ref data count") );

        m_data->DecRef();
        m_data = NULL;
    }
}

bool wxVariant::Unshare()
{
    if ( m_data && m_data->GetRefCount() > 1 )
    {
        // note that ref is not going to be destroyed in this case...
        const wxVariantData* ref = m_data;
        UnRef();

        // ... so we can still access it
        m_data = ref->Clone();

        wxASSERT_MSG( (m_data && m_data->GetRefCount() == 1),
                  _T("wxVariant::AllocExclusive() failed.") );

        if (!m_data || m_data->GetRefCount() != 1)
            return false;
        else
            return true;
    }
    //else: data is null or ref count is 1, so we are exclusive owners of m_refData anyhow
    else
        return true;
}


// Returns a string representing the type of the variant,
// e.g. "string", "bool", "list", "double", "long"
wxString wxVariant::GetType() const
{
    if (IsNull())
        return wxString(wxT("null"));
    else
        return GetData()->GetType();
}


bool wxVariant::IsType(const wxString& type) const
{
    return (GetType() == type);
}

bool wxVariant::IsValueKindOf(const wxClassInfo* type) const
{
    wxClassInfo* info=GetData()->GetValueClassInfo();
    return info ? info->IsKindOf(type) : false ;
}


// -----------------------------------------------------------------
// wxVariantDataLong
// -----------------------------------------------------------------

class WXDLLIMPEXP_BASE wxVariantDataLong: public wxVariantData
{
public:
    wxVariantDataLong() { m_value = 0; }
    wxVariantDataLong(long value) { m_value = value; }

    inline long GetValue() const { return m_value; }
    inline void SetValue(long value) { m_value = value; }

    virtual bool Eq(wxVariantData& data) const;

    virtual bool Read(wxString& str);
    virtual bool Write(wxString& str) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Read(wxSTD istream& str);
    virtual bool Write(wxSTD ostream& str) const;
#endif
#if wxUSE_STREAMS
    virtual bool Read(wxInputStream& str);
    virtual bool Write(wxOutputStream &str) const;
#endif // wxUSE_STREAMS

    wxVariantData* Clone() const { return new wxVariantDataLong(m_value); }

    virtual wxString GetType() const { return wxT("long"); }

protected:
    long m_value;
};

bool wxVariantDataLong::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( (data.GetType() == wxT("long")), wxT("wxVariantDataLong::Eq: argument mismatch") );

    wxVariantDataLong& otherData = (wxVariantDataLong&) data;

    return (otherData.m_value == m_value);
}

#if wxUSE_STD_IOSTREAM
bool wxVariantDataLong::Write(wxSTD ostream& str) const
{
    wxString s;
    Write(s);
    str << (const char*) s.mb_str();
    return true;
}
#endif

bool wxVariantDataLong::Write(wxString& str) const
{
    str.Printf(wxT("%ld"), m_value);
    return true;
}

#if wxUSE_STD_IOSTREAM
bool wxVariantDataLong::Read(wxSTD istream& str)
{
    str >> m_value;
    return true;
}
#endif

#if wxUSE_STREAMS
bool wxVariantDataLong::Write(wxOutputStream& str) const
{
    wxTextOutputStream s(str);

    s.Write32((size_t)m_value);
    return true;
}

bool wxVariantDataLong::Read(wxInputStream& str)
{
   wxTextInputStream s(str);
   m_value = s.Read32();
   return true;
}
#endif // wxUSE_STREAMS

bool wxVariantDataLong::Read(wxString& str)
{
    m_value = wxAtol(str);
    return true;
}

// wxVariant

wxVariant::wxVariant(long val, const wxString& name)
{
    m_data = new wxVariantDataLong(val);
    m_name = name;
}

wxVariant::wxVariant(int val, const wxString& name)
{
    m_data = new wxVariantDataLong((long)val);
    m_name = name;
}

wxVariant::wxVariant(short val, const wxString& name)
{
    m_data = new wxVariantDataLong((long)val);
    m_name = name;
}

bool wxVariant::operator== (long value) const
{
    long thisValue;
    if (!Convert(&thisValue))
        return false;
    else
        return (value == thisValue);
}

bool wxVariant::operator!= (long value) const
{
    return (!((*this) == value));
}

void wxVariant::operator= (long value)
{
    if (GetType() == wxT("long") &&
        m_data->GetRefCount() == 1)
    {
        ((wxVariantDataLong*)GetData())->SetValue(value);
    }
    else
    {
        UnRef();
        m_data = new wxVariantDataLong(value);
    }
}

long wxVariant::GetLong() const
{
    long value;
    if (Convert(& value))
        return value;
    else
    {
        wxFAIL_MSG(wxT("Could not convert to a long"));
        return 0;
    }
}

// -----------------------------------------------------------------
// wxVariantDoubleData
// -----------------------------------------------------------------

class WXDLLIMPEXP_BASE wxVariantDoubleData: public wxVariantData
{
public:
    wxVariantDoubleData() { m_value = 0.0; }
    wxVariantDoubleData(double value) { m_value = value; }

    inline double GetValue() const { return m_value; }
    inline void SetValue(double value) { m_value = value; }

    virtual bool Eq(wxVariantData& data) const;
    virtual bool Read(wxString& str);
#if wxUSE_STD_IOSTREAM
    virtual bool Write(wxSTD ostream& str) const;
#endif
    virtual bool Write(wxString& str) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Read(wxSTD istream& str);
#endif
#if wxUSE_STREAMS
    virtual bool Read(wxInputStream& str);
    virtual bool Write(wxOutputStream &str) const;
#endif // wxUSE_STREAMS
    virtual wxString GetType() const { return wxT("double"); }

    wxVariantData* Clone() const { return new wxVariantDoubleData(m_value); }
protected:
    double m_value;
};

bool wxVariantDoubleData::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( (data.GetType() == wxT("double")), wxT("wxVariantDoubleData::Eq: argument mismatch") );

    wxVariantDoubleData& otherData = (wxVariantDoubleData&) data;

    return wxIsSameDouble(otherData.m_value, m_value);
}

#if wxUSE_STD_IOSTREAM
bool wxVariantDoubleData::Write(wxSTD ostream& str) const
{
    wxString s;
    Write(s);
    str << (const char*) s.mb_str();
    return true;
}
#endif

bool wxVariantDoubleData::Write(wxString& str) const
{
    str.Printf(wxT("%.14g"), m_value);
    return true;
}

#if wxUSE_STD_IOSTREAM
bool wxVariantDoubleData::Read(wxSTD istream& str)
{
    str >> m_value;
    return true;
}
#endif

#if wxUSE_STREAMS
bool wxVariantDoubleData::Write(wxOutputStream& str) const
{
    wxTextOutputStream s(str);
    s.WriteDouble((double)m_value);
    return true;
}

bool wxVariantDoubleData::Read(wxInputStream& str)
{
    wxTextInputStream s(str);
    m_value = (float)s.ReadDouble();
    return true;
}
#endif // wxUSE_STREAMS

bool wxVariantDoubleData::Read(wxString& str)
{
    m_value = wxAtof(str);
    return true;
}

//  wxVariant double code

wxVariant::wxVariant(double val, const wxString& name)
{
    m_data = new wxVariantDoubleData(val);
    m_name = name;
}

bool wxVariant::operator== (double value) const
{
    double thisValue;
    if (!Convert(&thisValue))
        return false;

    return wxIsSameDouble(value, thisValue);
}

bool wxVariant::operator!= (double value) const
{
    return (!((*this) == value));
}

void wxVariant::operator= (double value)
{
    if (GetType() == wxT("double") &&
        m_data->GetRefCount() == 1)
    {
        ((wxVariantDoubleData*)GetData())->SetValue(value);
    }
    else
    {
        UnRef();
        m_data = new wxVariantDoubleData(value);
    }
}

double wxVariant::GetDouble() const
{
    double value;
    if (Convert(& value))
        return value;
    else
    {
        wxFAIL_MSG(wxT("Could not convert to a double number"));
        return 0.0;
    }
}

// -----------------------------------------------------------------
// wxVariantBoolData
// -----------------------------------------------------------------

#ifdef HAVE_BOOL

class WXDLLIMPEXP_BASE wxVariantDataBool: public wxVariantData
{
public:
    wxVariantDataBool() { m_value = 0; }
    wxVariantDataBool(bool value) { m_value = value; }

    inline bool GetValue() const { return m_value; }
    inline void SetValue(bool value) { m_value = value; }

    virtual bool Eq(wxVariantData& data) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Write(wxSTD ostream& str) const;
#endif
    virtual bool Write(wxString& str) const;
    virtual bool Read(wxString& str);
#if wxUSE_STD_IOSTREAM
    virtual bool Read(wxSTD istream& str);
#endif
#if wxUSE_STREAMS
    virtual bool Read(wxInputStream& str);
    virtual bool Write(wxOutputStream& str) const;
#endif // wxUSE_STREAMS
    virtual wxString GetType() const { return wxT("bool"); }

    wxVariantData* Clone() const { return new wxVariantDataBool(m_value); }
protected:
    bool m_value;
};

bool wxVariantDataBool::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( (data.GetType() == wxT("bool")), wxT("wxVariantDataBool::Eq: argument mismatch") );

    wxVariantDataBool& otherData = (wxVariantDataBool&) data;

    return (otherData.m_value == m_value);
}

#if wxUSE_STD_IOSTREAM
bool wxVariantDataBool::Write(wxSTD ostream& str) const
{
    wxString s;
    Write(s);
    str << (const char*) s.mb_str();
    return true;
}
#endif

bool wxVariantDataBool::Write(wxString& str) const
{
    str.Printf(wxT("%d"), (int) m_value);
    return true;
}

#if wxUSE_STD_IOSTREAM
bool wxVariantDataBool::Read(wxSTD istream& WXUNUSED(str))
{
    wxFAIL_MSG(wxT("Unimplemented"));
//    str >> (long) m_value;
    return false;
}
#endif

#if wxUSE_STREAMS
bool wxVariantDataBool::Write(wxOutputStream& str) const
{
    wxTextOutputStream s(str);

    s.Write8(m_value);
    return true;
}

bool wxVariantDataBool::Read(wxInputStream& str)
{
    wxTextInputStream s(str);

    m_value = s.Read8() != 0;
    return true;
}
#endif // wxUSE_STREAMS

bool wxVariantDataBool::Read(wxString& str)
{
    m_value = (wxAtol(str) != 0);
    return true;
}

// wxVariant ****

wxVariant::wxVariant(bool val, const wxString& name)
{
    m_data = new wxVariantDataBool(val);
    m_name = name;
}

bool wxVariant::operator== (bool value) const
{
    bool thisValue;
    if (!Convert(&thisValue))
        return false;
    else
        return (value == thisValue);
}

bool wxVariant::operator!= (bool value) const
{
    return (!((*this) == value));
}

void wxVariant::operator= (bool value)
{
    if (GetType() == wxT("bool") &&
        m_data->GetRefCount() == 1)
    {
        ((wxVariantDataBool*)GetData())->SetValue(value);
    }
    else
    {
        UnRef();
        m_data = new wxVariantDataBool(value);
    }
}

bool wxVariant::GetBool() const
{
    bool value;
    if (Convert(& value))
        return value;
    else
    {
        wxFAIL_MSG(wxT("Could not convert to a bool"));
        return 0;
    }
}

#endif // HAVE_BOOL

// -----------------------------------------------------------------
// wxVariantDataChar
// -----------------------------------------------------------------

class WXDLLIMPEXP_BASE wxVariantDataChar: public wxVariantData
{
public:
    wxVariantDataChar() { m_value = 0; }
    wxVariantDataChar(const wxUniChar& value) { m_value = value; }

    inline wxUniChar GetValue() const { return m_value; }
    inline void SetValue(const wxUniChar& value) { m_value = value; }

    virtual bool Eq(wxVariantData& data) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Read(wxSTD istream& str);
    virtual bool Write(wxSTD ostream& str) const;
#endif
    virtual bool Read(wxString& str);
    virtual bool Write(wxString& str) const;
#if wxUSE_STREAMS
    virtual bool Read(wxInputStream& str);
    virtual bool Write(wxOutputStream& str) const;
#endif // wxUSE_STREAMS
    virtual wxString GetType() const { return wxT("char"); }
    wxVariantData* Clone() const { return new wxVariantDataChar(m_value); }

protected:
    wxUniChar m_value;
};

bool wxVariantDataChar::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( (data.GetType() == wxT("char")), wxT("wxVariantDataChar::Eq: argument mismatch") );

    wxVariantDataChar& otherData = (wxVariantDataChar&) data;

    return (otherData.m_value == m_value);
}

#if wxUSE_STD_IOSTREAM
bool wxVariantDataChar::Write(wxSTD ostream& str) const
{
    str << wxString(m_value);
    return true;
}
#endif

bool wxVariantDataChar::Write(wxString& str) const
{
    str = m_value;
    return true;
}

#if wxUSE_STD_IOSTREAM
bool wxVariantDataChar::Read(wxSTD istream& WXUNUSED(str))
{
    wxFAIL_MSG(wxT("Unimplemented"));

    return false;
}
#endif

#if wxUSE_STREAMS
bool wxVariantDataChar::Write(wxOutputStream& str) const
{
    wxTextOutputStream s(str);

    // FIXME-UTF8: this should be just "s << m_value;" after removal of
    //             ANSI build and addition of wxUniChar to wxTextOutputStream:
    s << (wxChar)m_value;

    return true;
}

bool wxVariantDataChar::Read(wxInputStream& str)
{
    wxTextInputStream s(str);

    // FIXME-UTF8: this should be just "s >> m_value;" after removal of
    //             ANSI build and addition of wxUniChar to wxTextInputStream:
    wxChar ch;
    s >> ch;
    m_value = ch;

    return true;
}
#endif // wxUSE_STREAMS

bool wxVariantDataChar::Read(wxString& str)
{
    m_value = str[0u];
    return true;
}

wxVariant::wxVariant(const wxUniChar& val, const wxString& name)
{
    m_data = new wxVariantDataChar(val);
    m_name = name;
}

wxVariant::wxVariant(char val, const wxString& name)
{
    m_data = new wxVariantDataChar(val);
    m_name = name;
}

wxVariant::wxVariant(wchar_t val, const wxString& name)
{
    m_data = new wxVariantDataChar(val);
    m_name = name;
}

bool wxVariant::operator==(const wxUniChar& value) const
{
    wxUniChar thisValue;
    if (!Convert(&thisValue))
        return false;
    else
        return (value == thisValue);
}

wxVariant& wxVariant::operator=(const wxUniChar& value)
{
    if (GetType() == wxT("char") &&
        m_data->GetRefCount() == 1)
    {
        ((wxVariantDataChar*)GetData())->SetValue(value);
    }
    else
    {
        UnRef();
        m_data = new wxVariantDataChar(value);
    }

    return *this;
}

wxUniChar wxVariant::GetChar() const
{
    wxUniChar value;
    if (Convert(& value))
        return value;
    else
    {
        wxFAIL_MSG(wxT("Could not convert to a char"));
        return wxUniChar(0);
    }
}

// ----------------------------------------------------------------------------
// wxVariantDataString
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxVariantDataString: public wxVariantData
{
public:
    wxVariantDataString() { }
    wxVariantDataString(const wxString& value) { m_value = value; }

    inline wxString GetValue() const { return m_value; }
    inline void SetValue(const wxString& value) { m_value = value; }

    virtual bool Eq(wxVariantData& data) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Write(wxSTD ostream& str) const;
#endif
    virtual bool Read(wxString& str);
    virtual bool Write(wxString& str) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Read(wxSTD istream& WXUNUSED(str)) { return false; }
#endif
#if wxUSE_STREAMS
    virtual bool Read(wxInputStream& str);
    virtual bool Write(wxOutputStream& str) const;
#endif // wxUSE_STREAMS
    virtual wxString GetType() const { return wxT("string"); }
    wxVariantData* Clone() const { return new wxVariantDataString(m_value); }

protected:
    wxString m_value;
};

bool wxVariantDataString::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( (data.GetType() == wxT("string")), wxT("wxVariantDataString::Eq: argument mismatch") );

    wxVariantDataString& otherData = (wxVariantDataString&) data;

    return (otherData.m_value == m_value);
}

#if wxUSE_STD_IOSTREAM
bool wxVariantDataString::Write(wxSTD ostream& str) const
{
    str << (const char*) m_value.mb_str();
    return true;
}
#endif

bool wxVariantDataString::Write(wxString& str) const
{
    str = m_value;
    return true;
}

#if wxUSE_STREAMS
bool wxVariantDataString::Write(wxOutputStream& str) const
{
  // why doesn't wxOutputStream::operator<< take "const wxString&"
    wxTextOutputStream s(str);
    s.WriteString(m_value);
    return true;
}

bool wxVariantDataString::Read(wxInputStream& str)
{
    wxTextInputStream s(str);

    m_value = s.ReadLine();
    return true;
}
#endif // wxUSE_STREAMS

bool wxVariantDataString::Read(wxString& str)
{
    m_value = str;
    return true;
}

// wxVariant ****

wxVariant::wxVariant(const wxString& val, const wxString& name)
{
    m_data = new wxVariantDataString(val);
    m_name = name;
}

wxVariant::wxVariant(const char* val, const wxString& name)
{
    m_data = new wxVariantDataString(wxString(val));
    m_name = name;
}

wxVariant::wxVariant(const wchar_t* val, const wxString& name)
{
    m_data = new wxVariantDataString(wxString(val));
    m_name = name;
}

wxVariant::wxVariant(const wxCStrData& val, const wxString& name)
{
    m_data = new wxVariantDataString(val.AsString());
    m_name = name;
}

wxVariant::wxVariant(const wxCharBuffer& val, const wxString& name)
{
    m_data = new wxVariantDataString(wxString(val));
    m_name = name;
}

wxVariant::wxVariant(const wxWCharBuffer& val, const wxString& name)
{
    m_data = new wxVariantDataString(wxString(val));
    m_name = name;
}

bool wxVariant::operator== (const wxString& value) const
{
    wxString thisValue;
    if (!Convert(&thisValue))
        return false;

    return value == thisValue;
}

bool wxVariant::operator!= (const wxString& value) const
{
    return (!((*this) == value));
}

wxVariant& wxVariant::operator= (const wxString& value)
{
    if (GetType() == wxT("string") &&
        m_data->GetRefCount() == 1)
    {
        ((wxVariantDataString*)GetData())->SetValue(value);
    }
    else
    {
        UnRef();
        m_data = new wxVariantDataString(value);
    }
    return *this;
}

wxString wxVariant::GetString() const
{
    wxString value;
    if (!Convert(& value))
    {
        wxFAIL_MSG(wxT("Could not convert to a string"));
    }

    return value;
}

// ----------------------------------------------------------------------------
// wxVariantDataWxObjectPtr
// ----------------------------------------------------------------------------

class wxVariantDataWxObjectPtr: public wxVariantData
{
public:
    wxVariantDataWxObjectPtr() { }
    wxVariantDataWxObjectPtr(wxObject* value) { m_value = value; }

    inline wxObject* GetValue() const { return m_value; }
    inline void SetValue(wxObject* value) { m_value = value; }

    virtual bool Eq(wxVariantData& data) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Write(wxSTD ostream& str) const;
#endif
    virtual bool Write(wxString& str) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Read(wxSTD istream& str);
#endif
    virtual bool Read(wxString& str);
    virtual wxString GetType() const ;
    virtual wxVariantData* Clone() const { return new wxVariantDataWxObjectPtr(m_value); }

    virtual wxClassInfo* GetValueClassInfo();

protected:
    wxObject* m_value;
};

bool wxVariantDataWxObjectPtr::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( data.GetType() == GetType(), wxT("wxVariantDataWxObjectPtr::Eq: argument mismatch") );

    wxVariantDataWxObjectPtr& otherData = (wxVariantDataWxObjectPtr&) data;

    return (otherData.m_value == m_value);
}

wxString wxVariantDataWxObjectPtr::GetType() const
{
    wxString returnVal(wxT("wxObject*"));

    if (m_value)
    {
        returnVal = m_value->GetClassInfo()->GetClassName();
        returnVal += wxT("*");
    }

    return returnVal;
}

wxClassInfo* wxVariantDataWxObjectPtr::GetValueClassInfo()
{
    wxClassInfo* returnVal=NULL;

    if (m_value) returnVal = m_value->GetClassInfo();

    return returnVal;
}

#if wxUSE_STD_IOSTREAM
bool wxVariantDataWxObjectPtr::Write(wxSTD ostream& str) const
{
    wxString s;
    Write(s);
    str << (const char*) s.mb_str();
    return true;
}
#endif

bool wxVariantDataWxObjectPtr::Write(wxString& str) const
{
    str.Printf(wxT("%s(%p)"), GetType().c_str(), wx_static_cast(void*, m_value));
    return true;
}

#if wxUSE_STD_IOSTREAM
bool wxVariantDataWxObjectPtr::Read(wxSTD istream& WXUNUSED(str))
{
    // Not implemented
    return false;
}
#endif

bool wxVariantDataWxObjectPtr::Read(wxString& WXUNUSED(str))
{
    // Not implemented
    return false;
}

// wxVariant

wxVariant::wxVariant( wxObject* val, const wxString& name)
{
    m_data = new wxVariantDataWxObjectPtr(val);
    m_name = name;
}

bool wxVariant::operator== (wxObject* value) const
{
    return (value == ((wxVariantDataWxObjectPtr*)GetData())->GetValue());
}

bool wxVariant::operator!= (wxObject* value) const
{
    return (!((*this) == (wxObject*) value));
}

void wxVariant::operator= (wxObject* value)
{
    UnRef();
    m_data = new wxVariantDataWxObjectPtr(value);
}

wxObject* wxVariant::GetWxObjectPtr() const
{
    return (wxObject*) ((wxVariantDataWxObjectPtr*) m_data)->GetValue();
}

// ----------------------------------------------------------------------------
// wxVariantDataVoidPtr
// ----------------------------------------------------------------------------

class wxVariantDataVoidPtr: public wxVariantData
{
public:
    wxVariantDataVoidPtr() { }
    wxVariantDataVoidPtr(void* value) { m_value = value; }

    inline void* GetValue() const { return m_value; }
    inline void SetValue(void* value) { m_value = value; }

    virtual bool Eq(wxVariantData& data) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Write(wxSTD ostream& str) const;
#endif
    virtual bool Write(wxString& str) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Read(wxSTD istream& str);
#endif
    virtual bool Read(wxString& str);
    virtual wxString GetType() const { return wxT("void*"); }
    virtual wxVariantData* Clone() const { return new wxVariantDataVoidPtr(m_value); }

protected:
    void* m_value;
};

bool wxVariantDataVoidPtr::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( data.GetType() == wxT("void*"), wxT("wxVariantDataVoidPtr::Eq: argument mismatch") );

    wxVariantDataVoidPtr& otherData = (wxVariantDataVoidPtr&) data;

    return (otherData.m_value == m_value);
}

#if wxUSE_STD_IOSTREAM
bool wxVariantDataVoidPtr::Write(wxSTD ostream& str) const
{
    wxString s;
    Write(s);
    str << (const char*) s.mb_str();
    return true;
}
#endif

bool wxVariantDataVoidPtr::Write(wxString& str) const
{
    str.Printf(wxT("%p"), m_value);
    return true;
}

#if wxUSE_STD_IOSTREAM
bool wxVariantDataVoidPtr::Read(wxSTD istream& WXUNUSED(str))
{
    // Not implemented
    return false;
}
#endif

bool wxVariantDataVoidPtr::Read(wxString& WXUNUSED(str))
{
    // Not implemented
    return false;
}

// wxVariant

wxVariant::wxVariant( void* val, const wxString& name)
{
    m_data = new wxVariantDataVoidPtr(val);
    m_name = name;
}

bool wxVariant::operator== (void* value) const
{
    return (value == ((wxVariantDataVoidPtr*)GetData())->GetValue());
}

bool wxVariant::operator!= (void* value) const
{
    return (!((*this) == (void*) value));
}

void wxVariant::operator= (void* value)
{
    if (GetType() == wxT("void*") && (m_data->GetRefCount() == 1))
    {
        ((wxVariantDataVoidPtr*)GetData())->SetValue(value);
    }
    else
    {
        UnRef();
        m_data = new wxVariantDataVoidPtr(value);
    }
}

void* wxVariant::GetVoidPtr() const
{
    wxASSERT( (GetType() == wxT("void*")) );

    return (void*) ((wxVariantDataVoidPtr*) m_data)->GetValue();
}

// ----------------------------------------------------------------------------
// wxVariantDataDateTime
// ----------------------------------------------------------------------------

#if wxUSE_DATETIME

class wxVariantDataDateTime: public wxVariantData
{
public:
    wxVariantDataDateTime() { }
    wxVariantDataDateTime(const wxDateTime& value) { m_value = value; }

    inline wxDateTime GetValue() const { return m_value; }
    inline void SetValue(const wxDateTime& value) { m_value = value; }

    virtual bool Eq(wxVariantData& data) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Write(wxSTD ostream& str) const;
#endif
    virtual bool Write(wxString& str) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Read(wxSTD istream& str);
#endif
    virtual bool Read(wxString& str);
    virtual wxString GetType() const { return wxT("datetime"); }
    virtual wxVariantData* Clone() const { return new wxVariantDataDateTime(m_value); }

protected:
    wxDateTime m_value;
};


bool wxVariantDataDateTime::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( (data.GetType() == wxT("datetime")), wxT("wxVariantDataDateTime::Eq: argument mismatch") );

    wxVariantDataDateTime& otherData = (wxVariantDataDateTime&) data;

    return (otherData.m_value == m_value);
}


#if wxUSE_STD_IOSTREAM
bool wxVariantDataDateTime::Write(wxSTD ostream& str) const
{
    wxString value;
    Write( value );
    str << value.c_str();
    return true;
}
#endif


bool wxVariantDataDateTime::Write(wxString& str) const
{
    str = m_value.Format();
    return true;
}


#if wxUSE_STD_IOSTREAM
bool wxVariantDataDateTime::Read(wxSTD istream& WXUNUSED(str))
{
    // Not implemented
    return false;
}
#endif


bool wxVariantDataDateTime::Read(wxString& str)
{
    if(! m_value.ParseDateTime(str.c_str()/*FIXME-UTF8*/))
        return false;
    return true;
}

// wxVariant

wxVariant::wxVariant(const wxDateTime& val, const wxString& name) // Date
{
    m_data = new wxVariantDataDateTime(val);
    m_name = name;
}

bool wxVariant::operator== (const wxDateTime& value) const
{
    wxDateTime thisValue;
    if (!Convert(&thisValue))
        return false;

    return value.IsEqualTo(thisValue);
}

bool wxVariant::operator!= (const wxDateTime& value) const
{
    return (!((*this) == value));
}

void wxVariant::operator= (const wxDateTime& value)
{
    if (GetType() == wxT("datetime") &&
        m_data->GetRefCount() == 1)
    {
        ((wxVariantDataDateTime*)GetData())->SetValue(value);
    }
    else
    {
        UnRef();
        m_data = new wxVariantDataDateTime(value);
    }
}

wxDateTime wxVariant::GetDateTime() const
{
    wxDateTime value;
    if (!Convert(& value))
    {
        wxFAIL_MSG(wxT("Could not convert to a datetime"));
    }

    return value;
}

#endif // wxUSE_DATETIME

// ----------------------------------------------------------------------------
// wxVariantDataArrayString
// ----------------------------------------------------------------------------

class wxVariantDataArrayString: public wxVariantData
{
public:
    wxVariantDataArrayString() { }
    wxVariantDataArrayString(const wxArrayString& value) { m_value = value; }

    wxArrayString GetValue() const { return m_value; }
    void SetValue(const wxArrayString& value) { m_value = value; }

    virtual bool Eq(wxVariantData& data) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Write(wxSTD ostream& str) const;
#endif
    virtual bool Write(wxString& str) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Read(wxSTD istream& str);
#endif
    virtual bool Read(wxString& str);
    virtual wxString GetType() const { return wxT("arrstring"); }
    virtual wxVariantData* Clone() const { return new wxVariantDataArrayString(m_value); }

protected:
    wxArrayString m_value;
};

bool wxVariantDataArrayString::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( data.GetType() == GetType(), wxT("wxVariantDataArrayString::Eq: argument mismatch") );

    wxVariantDataArrayString& otherData = (wxVariantDataArrayString&) data;

    return otherData.m_value == m_value;
}

#if wxUSE_STD_IOSTREAM
bool wxVariantDataArrayString::Write(wxSTD ostream& WXUNUSED(str)) const
{
    // Not implemented
    return false;
}
#endif

bool wxVariantDataArrayString::Write(wxString& str) const
{
    size_t count = m_value.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        if ( n )
            str += _T(';');

        str += m_value[n];
    }

    return true;
}


#if wxUSE_STD_IOSTREAM
bool wxVariantDataArrayString::Read(wxSTD istream& WXUNUSED(str))
{
    // Not implemented
    return false;
}
#endif


bool wxVariantDataArrayString::Read(wxString& str)
{
    wxStringTokenizer tk(str, _T(";"));
    while ( tk.HasMoreTokens() )
    {
        m_value.Add(tk.GetNextToken());
    }

    return true;
}

// wxVariant

wxVariant::wxVariant(const wxArrayString& val, const wxString& name) // Strings
{
    m_data = new wxVariantDataArrayString(val);
    m_name = name;
}

bool wxVariant::operator==(const wxArrayString& WXUNUSED(value)) const
{
    wxFAIL_MSG( _T("TODO") );

    return false;
}

bool wxVariant::operator!=(const wxArrayString& value) const
{
    return !(*this == value);
}

void wxVariant::operator=(const wxArrayString& value)
{
    if (GetType() == wxT("arrstring") &&
        m_data->GetRefCount() == 1)
    {
        ((wxVariantDataArrayString *)GetData())->SetValue(value);
    }
    else
    {
        UnRef();
        m_data = new wxVariantDataArrayString(value);
    }
}

wxArrayString wxVariant::GetArrayString() const
{
    if ( GetType() == wxT("arrstring") )
        return ((wxVariantDataArrayString *)GetData())->GetValue();

    return wxArrayString();
}

// ----------------------------------------------------------------------------
// wxVariantDataList
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxVariantDataList: public wxVariantData
{
public:
    wxVariantDataList() {}
    wxVariantDataList(const wxVariantList& list);
    virtual ~wxVariantDataList();

    wxVariantList& GetValue() { return m_value; }
    void SetValue(const wxVariantList& value) ;

    virtual bool Eq(wxVariantData& data) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Write(wxSTD ostream& str) const;
#endif
    virtual bool Write(wxString& str) const;
#if wxUSE_STD_IOSTREAM
    virtual bool Read(wxSTD istream& str);
#endif
    virtual bool Read(wxString& str);
    virtual wxString GetType() const { return wxT("list"); }

    void Clear();

    wxVariantData* Clone() const { return new wxVariantDataList(m_value); }
protected:
    wxVariantList  m_value;
};

wxVariantDataList::wxVariantDataList(const wxVariantList& list)
{
    SetValue(list);
}

wxVariantDataList::~wxVariantDataList()
{
    Clear();
}

void wxVariantDataList::SetValue(const wxVariantList& value)
{
    Clear();
    wxVariantList::compatibility_iterator node = value.GetFirst();
    while (node)
    {
        wxVariant* var = node->GetData();
        m_value.Append(new wxVariant(*var));
        node = node->GetNext();
    }
}

void wxVariantDataList::Clear()
{
    wxVariantList::compatibility_iterator node = m_value.GetFirst();
    while (node)
    {
        wxVariant* var = node->GetData();
        delete var;
        node = node->GetNext();
    }
    m_value.Clear();
}

bool wxVariantDataList::Eq(wxVariantData& data) const
{
    wxASSERT_MSG( (data.GetType() == wxT("list")), wxT("wxVariantDataList::Eq: argument mismatch") );

    wxVariantDataList& listData = (wxVariantDataList&) data;
    wxVariantList::compatibility_iterator node1 = m_value.GetFirst();
    wxVariantList::compatibility_iterator node2 = listData.GetValue().GetFirst();
    while (node1 && node2)
    {
        wxVariant* var1 = node1->GetData();
        wxVariant* var2 = node2->GetData();
        if ((*var1) != (*var2))
            return false;
        node1 = node1->GetNext();
        node2 = node2->GetNext();
    }
    if (node1 || node2) return false;
    return true;
}

#if wxUSE_STD_IOSTREAM
bool wxVariantDataList::Write(wxSTD ostream& str) const
{
    wxString s;
    Write(s);
    str << (const char*) s.mb_str();
    return true;
}
#endif

bool wxVariantDataList::Write(wxString& str) const
{
    str = wxEmptyString;
    wxVariantList::compatibility_iterator node = m_value.GetFirst();
    while (node)
    {
        wxVariant* var = node->GetData();
        if (node != m_value.GetFirst())
          str += wxT(" ");
        wxString str1;
        str += var->MakeString();
        node = node->GetNext();
    }

    return true;
}

#if wxUSE_STD_IOSTREAM
bool wxVariantDataList::Read(wxSTD istream& WXUNUSED(str))
{
    wxFAIL_MSG(wxT("Unimplemented"));
    // TODO
    return false;
}
#endif

bool wxVariantDataList::Read(wxString& WXUNUSED(str))
{
    wxFAIL_MSG(wxT("Unimplemented"));
    // TODO
    return false;
}

// wxVariant

wxVariant::wxVariant(const wxVariantList& val, const wxString& name) // List of variants
{
    m_data = new wxVariantDataList(val);
    m_name = name;
}

bool wxVariant::operator== (const wxVariantList& value) const
{
    wxASSERT_MSG( (GetType() == wxT("list")), wxT("Invalid type for == operator") );

    wxVariantDataList other(value);
    return (GetData()->Eq(other));
}

bool wxVariant::operator!= (const wxVariantList& value) const
{
    return (!((*this) == value));
}

void wxVariant::operator= (const wxVariantList& value)
{
    if (GetType() == wxT("list") &&
        m_data->GetRefCount() == 1)
    {
        ((wxVariantDataList*)GetData())->SetValue(value);
    }
    else
    {
        UnRef();
        m_data = new wxVariantDataList(value);
    }
}

wxVariantList& wxVariant::GetList() const
{
    wxASSERT( (GetType() == wxT("list")) );

    return (wxVariantList&) ((wxVariantDataList*) m_data)->GetValue();
}

// Make empty list
void wxVariant::NullList()
{
    SetData(new wxVariantDataList());
}

// Append to list
void wxVariant::Append(const wxVariant& value)
{
    wxVariantList& list = GetList();

    list.Append(new wxVariant(value));
}

// Insert at front of list
void wxVariant::Insert(const wxVariant& value)
{
    wxVariantList& list = GetList();

    list.Insert(new wxVariant(value));
}

// Returns true if the variant is a member of the list
bool wxVariant::Member(const wxVariant& value) const
{
    wxVariantList& list = GetList();

    wxVariantList::compatibility_iterator node = list.GetFirst();
    while (node)
    {
        wxVariant* other = node->GetData();
        if (value == *other)
            return true;
        node = node->GetNext();
    }
    return false;
}

// Deletes the nth element of the list
bool wxVariant::Delete(size_t item)
{
    wxVariantList& list = GetList();

    wxASSERT_MSG( (item < list.GetCount()), wxT("Invalid index to Delete") );
    wxVariantList::compatibility_iterator node = list.Item(item);
    wxVariant* variant = node->GetData();
    delete variant;
    list.Erase(node);
    return true;
}

// Clear list
void wxVariant::ClearList()
{
    if (!IsNull() && (GetType() == wxT("list")))
    {
        ((wxVariantDataList*) m_data)->Clear();
    }
    else
    {
        if (!GetType().IsSameAs(wxT("list")))
            UnRef();

        m_data = new wxVariantDataList;
    }
}

// Treat a list variant as an array
wxVariant wxVariant::operator[] (size_t idx) const
{
    wxASSERT_MSG( GetType() == wxT("list"), wxT("Invalid type for array operator") );

    if (GetType() == wxT("list"))
    {
        wxVariantDataList* data = (wxVariantDataList*) m_data;
        wxASSERT_MSG( (idx < data->GetValue().GetCount()), wxT("Invalid index for array") );
        return *(data->GetValue().Item(idx)->GetData());
    }
    return wxNullVariant;
}

wxVariant& wxVariant::operator[] (size_t idx)
{
    // We can't return a reference to a variant for a string list, since the string
    // is actually stored as a char*, not a variant.

    wxASSERT_MSG( (GetType() == wxT("list")), wxT("Invalid type for array operator") );

    wxVariantDataList* data = (wxVariantDataList*) m_data;
    wxASSERT_MSG( (idx < data->GetValue().GetCount()), wxT("Invalid index for array") );

    return * (data->GetValue().Item(idx)->GetData());
}

// Return the number of elements in a list
size_t wxVariant::GetCount() const
{
    wxASSERT_MSG( GetType() == wxT("list"), wxT("Invalid type for GetCount()") );

    if (GetType() == wxT("list"))
    {
        wxVariantDataList* data = (wxVariantDataList*) m_data;
        return data->GetValue().GetCount();
    }
    return 0;
}

// ----------------------------------------------------------------------------
// Type conversion
// ----------------------------------------------------------------------------

bool wxVariant::Convert(long* value) const
{
    wxString type(GetType());
    if (type == wxT("double"))
        *value = (long) (((wxVariantDoubleData*)GetData())->GetValue());
    else if (type == wxT("long"))
        *value = ((wxVariantDataLong*)GetData())->GetValue();
#ifdef HAVE_BOOL
    else if (type == wxT("bool"))
        *value = (long) (((wxVariantDataBool*)GetData())->GetValue());
#endif
    else if (type == wxT("string"))
        *value = wxAtol(((wxVariantDataString*)GetData())->GetValue());
    else
        return false;

    return true;
}

bool wxVariant::Convert(bool* value) const
{
    wxString type(GetType());
    if (type == wxT("double"))
        *value = ((int) (((wxVariantDoubleData*)GetData())->GetValue()) != 0);
    else if (type == wxT("long"))
        *value = (((wxVariantDataLong*)GetData())->GetValue() != 0);
#ifdef HAVE_BOOL
    else if (type == wxT("bool"))
        *value = ((wxVariantDataBool*)GetData())->GetValue();
#endif
    else if (type == wxT("string"))
    {
        wxString val(((wxVariantDataString*)GetData())->GetValue());
        val.MakeLower();
        if (val == wxT("true") || val == wxT("yes") || val == wxT('1') )
            *value = true;
        else if (val == wxT("false") || val == wxT("no") || val == wxT('0') )
            *value = false;
        else
            return false;
    }
    else
        return false;

    return true;
}

bool wxVariant::Convert(double* value) const
{
    wxString type(GetType());
    if (type == wxT("double"))
        *value = ((wxVariantDoubleData*)GetData())->GetValue();
    else if (type == wxT("long"))
        *value = (double) (((wxVariantDataLong*)GetData())->GetValue());
#ifdef HAVE_BOOL
    else if (type == wxT("bool"))
        *value = (double) (((wxVariantDataBool*)GetData())->GetValue());
#endif
    else if (type == wxT("string"))
        *value = (double) wxAtof(((wxVariantDataString*)GetData())->GetValue());
    else
        return false;

    return true;
}

bool wxVariant::Convert(wxUniChar* value) const
{
    wxString type(GetType());
    if (type == wxT("char"))
        *value = ((wxVariantDataChar*)GetData())->GetValue();
    else if (type == wxT("long"))
        *value = (char) (((wxVariantDataLong*)GetData())->GetValue());
#ifdef HAVE_BOOL
    else if (type == wxT("bool"))
        *value = (char) (((wxVariantDataBool*)GetData())->GetValue());
#endif
    else
        return false;

    return true;
}

bool wxVariant::Convert(char* value) const
{
    wxUniChar ch;
    if ( !Convert(&ch) )
        return false;
    *value = ch;
    return true;
}

bool wxVariant::Convert(wchar_t* value) const
{
    wxUniChar ch;
    if ( !Convert(&ch) )
        return false;
    *value = ch;
    return true;
}

bool wxVariant::Convert(wxString* value) const
{
    *value = MakeString();
    return true;
}

#if wxUSE_DATETIME
bool wxVariant::Convert(wxDateTime* value) const
{
    wxString type(GetType());
    if (type == wxT("datetime"))
    {
        *value = ((wxVariantDataDateTime*)GetData())->GetValue();
        return true;
    }
    // Fallback to string conversion
    wxString val;
    return Convert(&val) &&
                (value->ParseDateTime(val.c_str()/*FIXME-UTF8*/) ||
                 value->ParseDate(val.c_str()/*FIXME-UTF8*/) ||
                 value->ParseTime(val.c_str()/*FIXME-UTF8*/));
}
#endif // wxUSE_DATETIME

#endif // wxUSE_VARIANT
