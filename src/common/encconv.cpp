/////////////////////////////////////////////////////////////////////////////
// Name:        encconv.cpp
// Purpose:     wxEncodingConverter class for converting between different
//              font encodings
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "encconv.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#include "wx/encconv.h"

#include <stdlib.h>

#include "unictabl.inc" 
// conversion tables, generated by scripts in $(WXWIN)/misc/unictabl


static wxUint16 *GetEncTable(wxFontEncoding enc)
{
    for (int i = 0; encodings_list[i].table != NULL; i++)
    {
        if (encodings_list[i].encoding == enc) 
            return encodings_list[i].table;
    }
    return NULL;
}

typedef struct {
    wxUint16 u;
    wxUint8  c;
} CharsetItem;



static int CompareCharsetItems(const void *i1, const void *i2)
{
    return ( ((CharsetItem*)i1) -> u - ((CharsetItem*)i2) -> u );
}


static CharsetItem* BuildReverseTable(wxUint16 *tbl)
{
    CharsetItem *rev = new CharsetItem[128];
    
    for (int i = 0; i < 128; i++)
        rev[i].c = 128 + i, rev[i].u = tbl[i];

    qsort(rev, 128, sizeof(CharsetItem), CompareCharsetItems);
    
    return rev;
}



wxEncodingConverter::wxEncodingConverter()
{
    m_Table = NULL;
    m_UnicodeInput = FALSE;
    m_JustCopy = FALSE;
}



bool wxEncodingConverter::Init(wxFontEncoding input_enc, wxFontEncoding output_enc, int method)
{
    unsigned i;
    wxUint16 *in_tbl = NULL, *out_tbl = NULL;

    if (m_Table) {delete[] m_Table; m_Table = NULL;}

#if !wxUSE_UNICODE
    if (input_enc == wxFONTENCODING_UNICODE || output_enc == wxFONTENCODING_UNICODE) return FALSE;
#endif

    if (input_enc == output_enc) {m_JustCopy = TRUE; return TRUE;}
    
    m_JustCopy = FALSE;
    
    if (input_enc == wxFONTENCODING_UNICODE)
    {
        if ((out_tbl = GetEncTable(output_enc)) == NULL) return FALSE;

        m_Table = new wxChar[65536];
        for (i = 0; i < 128; i++)  m_Table[i] = (wxChar)i; // 7bit ASCII
        for (i = 128; i < 65536; i++)  m_Table[i] = (wxChar)'?'; 
                // FIXME - this should be character that means `unicode to charset' impossible, not '?'

        if (method == wxCONVERT_SUBSTITUTE)
        {
            for (i = 0; i < encoding_unicode_fallback_count; i++)
                m_Table[encoding_unicode_fallback[i].c] = (wxChar) encoding_unicode_fallback[i].s;
        }

        for (i = 0; i < 128; i++)
            m_Table[out_tbl[i]] = (wxChar)(128 + i);

        m_UnicodeInput = TRUE;
        return TRUE;
    }
    
    else
    {
        if ((in_tbl = GetEncTable(input_enc)) == NULL) return FALSE;
        if (output_enc != wxFONTENCODING_UNICODE)
            if ((out_tbl = GetEncTable(output_enc)) == NULL) return FALSE;

        m_UnicodeInput = FALSE;
        
        m_Table = new wxChar[256];
        for (i = 0; i < 128; i++)  m_Table[i] = (wxChar)i; // 7bit ASCII
        
        if (output_enc == wxFONTENCODING_UNICODE)
        {
            for (i = 0; i < 128; i++)  m_Table[128 + i] = (wxChar)in_tbl[i]; // wxChar is 2byte now
            return TRUE;
        }
        else 
        {
            CharsetItem *rev = BuildReverseTable(out_tbl);
            CharsetItem *item, key;
            
            for (i = 0; i < 128; i++)  
            {
                key.u = in_tbl[i];
                item = (CharsetItem*) bsearch(&key, rev, 128, sizeof(CharsetItem), CompareCharsetItems);
                if (item == NULL && method == wxCONVERT_SUBSTITUTE)
                    item = (CharsetItem*) bsearch(&key, encoding_unicode_fallback, 
                                encoding_unicode_fallback_count, sizeof(CharsetItem), CompareCharsetItems);
                if (item)
                    m_Table[128 + i] = (wxChar)item -> c;
                else
                    m_Table[128 + i] = 128 + i; // don't know => don't touch
            }
            
            delete[] rev;
            return TRUE;
        }
    }
}


void wxEncodingConverter::Convert(const wxChar* input, wxChar* output)
{
    if (m_JustCopy)
    {
        wxStrcpy(output, input);
        return;
    }
    
    wxASSERT_MSG(m_Table != NULL, wxT("You must call wxEncodingConverter::Init() before actually converting!"));
    
    const wxChar *i;
    wxChar *o;
    
    if (m_UnicodeInput)
        for (i = input, o = output; *i != 0; i++, o++)
            *o = (wxChar)(m_Table[(wxUint16)*i]);
    else
        for (i = input, o = output; *i != 0; i++, o++)
            *o = (wxChar)(m_Table[(wxUint8)*i]);
    *o = 0;
}



wxString wxEncodingConverter::Convert(const wxString& input)
{
    if (m_JustCopy) return input;
    
    wxString s;
    const wxChar *i;
    
    wxASSERT_MSG(m_Table != NULL, wxT("You must call wxEncodingConverter::Init() before actually converting!"));
    
    if (m_UnicodeInput)
        for (i = input.c_str(); *i != 0; i++)
            s << (wxChar)(m_Table[(wxUint16)*i]);
    else
        for (i = input.c_str(); *i != 0; i++)
            s << (wxChar)(m_Table[(wxUint8)*i]);
    return s;
}




// Following tables describe classes of encoding equivalence.
// 

#define STOP wxFONTENCODING_SYSTEM

#define NUM_OF_PLATFORMS  4 /*must conform to enum wxPLATFORM_XXXX !!!*/
#define ENC_PER_PLATFORM  3
           // max no. of encodings for one language used on one platform
           // Anybody thinks 3 is not enough? ;-)

static wxFontEncoding 
    EquivalentEncodings[][NUM_OF_PLATFORMS][ENC_PER_PLATFORM+1] = {

    // West European (Latin1)
    {
        /* unix    */ {wxFONTENCODING_ISO8859_1, wxFONTENCODING_ISO8859_15, STOP},
        /* windows */ {wxFONTENCODING_CP1252, STOP},
        /* os2     */ {STOP},
        /* mac     */ {STOP}   
    },

    // Central European (Latin2)
    {
        /* unix    */ {wxFONTENCODING_ISO8859_2, STOP},
        /* windows */ {wxFONTENCODING_CP1250, STOP},
        /* os2     */ {STOP},
        /* mac     */ {STOP}   
    },

    {{STOP},{STOP},{STOP},{STOP}} /* Terminator */
    /* no, _not_ Arnold! */
};




wxFontEncodingArray wxEncodingConverter::GetPlatformEquivalents(wxFontEncoding enc, int platform)
{
    if (platform == wxPLATFORM_CURRENT)
    {
#if defined(__WXMSW__)
        platform = wxPLATFORM_WINDOWS;
#elif defined(__WXGTK__) || defined(__WXMOTIF__)
        platform = wxPLATFORM_UNIX;
#elif defined(__WXOS2__)
        platform = wxPLATFORM_OS2;
#elif defined(__WXMAC__)
        platform = wxPLATFORM_MAC;
#endif
    }
    
    int i, clas, e ;
    wxFontEncoding *f;
    wxFontEncodingArray arr;

    clas = 0;
    while (EquivalentEncodings[clas][0][0] != STOP)
    {
        for (i = 0; i < NUM_OF_PLATFORMS; i++)
            for (e = 0; EquivalentEncodings[clas][i][e] != STOP; e++)
                if (EquivalentEncodings[clas][i][e] == enc)
                {
                    for (f = EquivalentEncodings[clas][platform]; *f != STOP; f++) 
                        if (arr.Index(*f) == wxNOT_FOUND) arr.Add(*f);
                    i = NUM_OF_PLATFORMS/*hack*/; break; 
                }
        clas++;
    }
    
    return arr;
}



wxFontEncodingArray wxEncodingConverter::GetAllEquivalents(wxFontEncoding enc)
{
    int i, clas, e, j ;
    wxFontEncoding *f;
    wxFontEncodingArray arr;

    clas = 0;
    while (EquivalentEncodings[clas][0][0] != STOP)
    {
        for (i = 0; i < NUM_OF_PLATFORMS; i++)
            for (e = 0; EquivalentEncodings[clas][i][e] != STOP; e++)
                if (EquivalentEncodings[clas][i][e] == enc)
                {
                    for (j = 0; j < NUM_OF_PLATFORMS; j++)
                        for (f = EquivalentEncodings[clas][j]; *f != STOP; f++) 
                            if (arr.Index(*f) == wxNOT_FOUND) arr.Add(*f);                            
                    i = NUM_OF_PLATFORMS/*hack*/; break; 
                }
        clas++;
    }
    
    return arr;
}
