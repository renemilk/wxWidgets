///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/datetimefmt.cpp
// Purpose:     wxDateTime formatting & parsing code
// Author:      Vadim Zeitlin
// Modified by:
// Created:     11.05.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
//              parts of code taken from sndcal library by Scott E. Lee:
//
//               Copyright 1993-1995, Scott E. Lee, all rights reserved.
//               Permission granted to use, copy, modify, distribute and sell
//               so long as the above copyright and this permission statement
//               are retained in all copies.
//
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if !defined(wxUSE_DATETIME) || wxUSE_DATETIME

#ifndef WX_PRECOMP
    #ifdef __WXMSW__
        #include "wx/msw/wrapwin.h"
    #endif
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/stopwatch.h"           // for wxGetLocalTimeMillis()
    #include "wx/module.h"
    #include "wx/crt.h"
#endif // WX_PRECOMP

#include "wx/thread.h"
#include "wx/tokenzr.h"

#include <ctype.h>

#ifdef __WINDOWS__
    #include <winnls.h>
    #ifndef __WXWINCE__
        #include <locale.h>
    #endif
#endif

#include "wx/datetime.h"

// ============================================================================
// implementation of wxDateTime
// ============================================================================

// ----------------------------------------------------------------------------
// helpers shared between datetime.cpp and datetimefmt.cpp
// ----------------------------------------------------------------------------

extern void InitTm(struct tm& tm);

extern int GetTimeZone();

extern wxString CallStrftime(const wxString& format, const tm* tm);

// ----------------------------------------------------------------------------
// constants (see also datetime.cpp)
// ----------------------------------------------------------------------------

static const int DAYS_PER_WEEK = 7;

static const int HOURS_PER_DAY = 24;

static const int SEC_PER_MIN = 60;

static const int MIN_PER_HOUR = 60;

// ----------------------------------------------------------------------------
// parsing helpers
// ----------------------------------------------------------------------------

namespace
{

#ifdef HAVE_STRPTIME

#if wxUSE_UNIX && !defined(HAVE_STRPTIME_DECL)
    // configure detected that we had strptime() but not its declaration,
    // provide it ourselves
    extern "C" char *strptime(const char *, const char *, struct tm *);
#endif

// strptime() wrapper: call strptime() for the string starting at the given
// iterator and fill output tm struct with the results and modify input to
// point to the end of the string consumed by strptime() if successful,
// otherwise return false and don't modify anything
bool
CallStrptime(const wxString& str,
             wxString::const_iterator& p,
             const char *fmt,
             tm *tm)
{
    // convert from iterator to char pointer: this is simple as wxCStrData
    // already supports this
    const char * const start = str.c_str() + (p - str.begin());

    const char * const end = strptime(start, fmt, tm);
    if ( !end )
        return false;

    // convert back from char pointer to iterator: unfortunately we have no way
    // to do it efficiently currently so create a temporary string just to
    // compute the number of characters between start and end
    p += wxString(start, end - start).length();

    return true;
}

#endif // HAVE_STRPTIME

enum
{
    DateLang_English = 1,
    DateLang_Local   = 2
};

// return the month if the string is a month name or Inv_Month otherwise
//
// flags can contain wxDateTime::Name_Abbr/Name_Full or both of them and lang
// can be either DateLang_Local (default) to interpret string as a localized
// month name or DateLang_English to parse it as a standard English name or
// their combination to interpret it in any way
wxDateTime::Month
GetMonthFromName(const wxString& name, int flags, int lang)
{
    wxDateTime::Month mon;
    for ( mon = wxDateTime::Jan; mon < wxDateTime::Inv_Month; wxNextMonth(mon) )
    {
        // case-insensitive comparison either one of or with both abbreviated
        // and not versions
        if ( flags & wxDateTime::Name_Full )
        {
            if ( lang & DateLang_English )
            {
                if ( name.CmpNoCase(wxDateTime::GetEnglishMonthName(mon,
                        wxDateTime::Name_Full)) == 0 )
                    break;
            }

            if ( lang & DateLang_Local )
            {
                if ( name.CmpNoCase(wxDateTime::GetMonthName(mon,
                        wxDateTime::Name_Full)) == 0 )
                    break;
            }
        }

        if ( flags & wxDateTime::Name_Abbr )
        {
            if ( lang & DateLang_English )
            {
                if ( name.CmpNoCase(wxDateTime::GetEnglishMonthName(mon,
                        wxDateTime::Name_Abbr)) == 0 )
                    break;
            }

            if ( lang & DateLang_Local )
            {
                if ( name.CmpNoCase(wxDateTime::GetMonthName(mon,
                        wxDateTime::Name_Abbr)) == 0 )
                    break;
            }
        }
    }

    return mon;
}

// return the weekday if the string is a weekday name or Inv_WeekDay otherwise
//
// flags and lang parameters have the same meaning as for GetMonthFromName()
// above
wxDateTime::WeekDay
GetWeekDayFromName(const wxString& name, int flags, int lang)
{
    wxDateTime::WeekDay wd;
    for ( wd = wxDateTime::Sun; wd < wxDateTime::Inv_WeekDay; wxNextWDay(wd) )
    {
        if ( flags & wxDateTime::Name_Full )
        {
            if ( lang & DateLang_English )
            {
                if ( name.CmpNoCase(wxDateTime::GetEnglishWeekDayName(wd,
                        wxDateTime::Name_Full)) == 0 )
                    break;
            }

            if ( lang & DateLang_Local )
            {
                if ( name.CmpNoCase(wxDateTime::GetWeekDayName(wd,
                        wxDateTime::Name_Full)) == 0 )
                    break;
            }
        }

        if ( flags & wxDateTime::Name_Abbr )
        {
            if ( lang & DateLang_English )
            {
                if ( name.CmpNoCase(wxDateTime::GetEnglishWeekDayName(wd,
                        wxDateTime::Name_Abbr)) == 0 )
                    break;
            }

            if ( lang & DateLang_Local )
            {
                if ( name.CmpNoCase(wxDateTime::GetWeekDayName(wd,
                        wxDateTime::Name_Abbr)) == 0 )
                    break;
            }
        }
    }

    return wd;
}

// scans all digits (but no more than len) and returns the resulting number
bool GetNumericToken(size_t len,
                     wxString::const_iterator& p,
                     const wxString::const_iterator& end,
                     unsigned long *number)
{
    size_t n = 1;
    wxString s;
    while ( p != end && wxIsdigit(*p) )
    {
        s += *p++;

        if ( len && ++n > len )
            break;
    }

    return !s.empty() && s.ToULong(number);
}

// scans all alphabetic characters and returns the resulting string
wxString
GetAlphaToken(wxString::const_iterator& p,
              const wxString::const_iterator& end)
{
    wxString s;
    while ( p != end && wxIsalpha(*p) )
    {
        s += *p++;
    }

    return s;
}

// parses string starting at given iterator using the specified format and,
// optionally, a fall back format (and optionally another one... but it stops
// there, really)
//
// if unsuccessful, returns invalid wxDateTime without changing p; otherwise
// advance p to the end of the match and returns wxDateTime containing the
// results of the parsing
wxDateTime
ParseFormatAt(wxString::const_iterator& p,
              const wxString::const_iterator& end,
              const wxString& fmt,
              // FIXME-VC6: using wxString() instead of wxEmptyString in the
              //            line below results in error C2062: type 'class
              //            wxString (__cdecl *)(void)' unexpected
              const wxString& fmtAlt = wxEmptyString,
              const wxString& fmtAlt2 = wxString())
{
    const wxString str(p, end);
    wxString::const_iterator endParse;
    wxDateTime dt;
    if ( dt.ParseFormat(str, fmt, &endParse) ||
            (!fmtAlt.empty() && dt.ParseFormat(str, fmtAlt, &endParse)) ||
                (!fmtAlt2.empty() && dt.ParseFormat(str, fmtAlt2, &endParse)) )
    {
        p += endParse - str.begin();
    }
    //else: all formats failed

    return dt;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxDateTime to/from text representations
// ----------------------------------------------------------------------------

wxString wxDateTime::Format(const wxString& format, const TimeZone& tz) const
{
    wxCHECK_MSG( !format.empty(), wxEmptyString,
                 _T("NULL format in wxDateTime::Format") );

    // we have to use our own implementation if the date is out of range of
    // strftime() or if we use non standard specificators
#ifdef HAVE_STRFTIME
    time_t time = GetTicks();

    if ( (time != (time_t)-1) && !wxStrstr(format, _T("%l")) )
    {
        // use strftime()
        struct tm tmstruct;
        struct tm *tm;
        if ( tz.GetOffset() == -GetTimeZone() )
        {
            // we are working with local time
            tm = wxLocaltime_r(&time, &tmstruct);

            // should never happen
            wxCHECK_MSG( tm, wxEmptyString, _T("wxLocaltime_r() failed") );
        }
        else
        {
            time += (int)tz.GetOffset();

#if defined(__VMS__) || defined(__WATCOMC__) // time is unsigned so avoid warning
            int time2 = (int) time;
            if ( time2 >= 0 )
#else
            if ( time >= 0 )
#endif
            {
                tm = wxGmtime_r(&time, &tmstruct);

                // should never happen
                wxCHECK_MSG( tm, wxEmptyString, _T("wxGmtime_r() failed") );
            }
            else
            {
                tm = (struct tm *)NULL;
            }
        }

        if ( tm )
        {
            return CallStrftime(format, tm);
        }
    }
    //else: use generic code below
#endif // HAVE_STRFTIME

    // we only parse ANSI C format specifications here, no POSIX 2
    // complications, no GNU extensions but we do add support for a "%l" format
    // specifier allowing to get the number of milliseconds
    Tm tm = GetTm(tz);

    // used for calls to strftime() when we only deal with time
    struct tm tmTimeOnly;
    tmTimeOnly.tm_hour = tm.hour;
    tmTimeOnly.tm_min = tm.min;
    tmTimeOnly.tm_sec = tm.sec;
    tmTimeOnly.tm_wday = 0;
    tmTimeOnly.tm_yday = 0;
    tmTimeOnly.tm_mday = 1;         // any date will do
    tmTimeOnly.tm_mon = 0;
    tmTimeOnly.tm_year = 76;
    tmTimeOnly.tm_isdst = 0;        // no DST, we adjust for tz ourselves

    wxString tmp, res, fmt;
    for ( wxString::const_iterator p = format.begin(); p != format.end(); ++p )
    {
        if ( *p != _T('%') )
        {
            // copy as is
            res += *p;

            continue;
        }

        // set the default format
        switch ( (*++p).GetValue() )
        {
            case _T('Y'):               // year has 4 digits
                fmt = _T("%04d");
                break;

            case _T('j'):               // day of year has 3 digits
            case _T('l'):               // milliseconds have 3 digits
                fmt = _T("%03d");
                break;

            case _T('w'):               // week day as number has only one
                fmt = _T("%d");
                break;

            default:
                // it's either another valid format specifier in which case
                // the format is "%02d" (for all the rest) or we have the
                // field width preceding the format in which case it will
                // override the default format anyhow
                fmt = _T("%02d");
        }

        bool restart = true;
        while ( restart )
        {
            restart = false;

            // start of the format specification
            switch ( (*p).GetValue() )
            {
                case _T('a'):       // a weekday name
                case _T('A'):
                    // second parameter should be true for abbreviated names
                    res += GetWeekDayName(tm.GetWeekDay(),
                                          *p == _T('a') ? Name_Abbr : Name_Full);
                    break;

                case _T('b'):       // a month name
                case _T('B'):
                    res += GetMonthName(tm.mon,
                                        *p == _T('b') ? Name_Abbr : Name_Full);
                    break;

                case _T('c'):       // locale default date and time  representation
                case _T('x'):       // locale default date representation
#ifdef HAVE_STRFTIME
                    //
                    // the problem: there is no way to know what do these format
                    // specifications correspond to for the current locale.
                    //
                    // the solution: use a hack and still use strftime(): first
                    // find the YEAR which is a year in the strftime() range (1970
                    // - 2038) whose Jan 1 falls on the same week day as the Jan 1
                    // of the real year. Then make a copy of the format and
                    // replace all occurrences of YEAR in it with some unique
                    // string not appearing anywhere else in it, then use
                    // strftime() to format the date in year YEAR and then replace
                    // YEAR back by the real year and the unique replacement
                    // string back with YEAR. Notice that "all occurrences of YEAR"
                    // means all occurrences of 4 digit as well as 2 digit form!
                    //
                    // the bugs: we assume that neither of %c nor %x contains any
                    // fields which may change between the YEAR and real year. For
                    // example, the week number (%U, %W) and the day number (%j)
                    // will change if one of these years is leap and the other one
                    // is not!
                    {
                        // find the YEAR: normally, for any year X, Jan 1 of the
                        // year X + 28 is the same weekday as Jan 1 of X (because
                        // the weekday advances by 1 for each normal X and by 2
                        // for each leap X, hence by 5 every 4 years or by 35
                        // which is 0 mod 7 every 28 years) but this rule breaks
                        // down if there are years between X and Y which are
                        // divisible by 4 but not leap (i.e. divisible by 100 but
                        // not 400), hence the correction.

                        int yearReal = GetYear(tz);
                        int mod28 = yearReal % 28;

                        // be careful to not go too far - we risk to leave the
                        // supported range
                        int year;
                        if ( mod28 < 10 )
                        {
                            year = 1988 + mod28;      // 1988 == 0 (mod 28)
                        }
                        else
                        {
                            year = 1970 + mod28 - 10; // 1970 == 10 (mod 28)
                        }

                        int nCentury = year / 100,
                            nCenturyReal = yearReal / 100;

                        // need to adjust for the years divisble by 400 which are
                        // not leap but are counted like leap ones if we just take
                        // the number of centuries in between for nLostWeekDays
                        int nLostWeekDays = (nCentury - nCenturyReal) -
                                            (nCentury / 4 - nCenturyReal / 4);

                        // we have to gain back the "lost" weekdays: note that the
                        // effect of this loop is to not do anything to
                        // nLostWeekDays (which we won't use any more), but to
                        // (indirectly) set the year correctly
                        while ( (nLostWeekDays % 7) != 0 )
                        {
                            nLostWeekDays += year++ % 4 ? 1 : 2;
                        }

                        // finally move the year below 2000 so that the 2-digit
                        // year number can never match the month or day of the
                        // month when we do the replacements below
                        if ( year >= 2000 )
                            year -= 28;

                        wxASSERT_MSG( year >= 1970 && year < 2000,
                                      _T("logic error in wxDateTime::Format") );


                        // use strftime() to format the same date but in supported
                        // year
                        //
                        // NB: we assume that strftime() doesn't check for the
                        //     date validity and will happily format the date
                        //     corresponding to Feb 29 of a non leap year (which
                        //     may happen if yearReal was leap and year is not)
                        struct tm tmAdjusted;
                        InitTm(tmAdjusted);
                        tmAdjusted.tm_hour = tm.hour;
                        tmAdjusted.tm_min = tm.min;
                        tmAdjusted.tm_sec = tm.sec;
                        tmAdjusted.tm_wday = tm.GetWeekDay();
                        tmAdjusted.tm_yday = GetDayOfYear();
                        tmAdjusted.tm_mday = tm.mday;
                        tmAdjusted.tm_mon = tm.mon;
                        tmAdjusted.tm_year = year - 1900;
                        tmAdjusted.tm_isdst = 0; // no DST, already adjusted
                        wxString str = CallStrftime(*p == _T('c') ? _T("%c")
                                                                  : _T("%x"),
                                                    &tmAdjusted);

                        // now replace the replacement year with the real year:
                        // notice that we have to replace the 4 digit year with
                        // a unique string not appearing in strftime() output
                        // first to prevent the 2 digit year from matching any
                        // substring of the 4 digit year (but any day, month,
                        // hours or minutes components should be safe because
                        // they are never in 70-99 range)
                        wxString replacement("|");
                        while ( str.find(replacement) != wxString::npos )
                            replacement += '|';

                        str.Replace(wxString::Format("%d", year),
                                    replacement);
                        str.Replace(wxString::Format("%d", year % 100),
                                    wxString::Format("%d", yearReal % 100));
                        str.Replace(replacement,
                                    wxString::Format("%d", yearReal));

                        res += str;
                    }
#else // !HAVE_STRFTIME
                    // Use "%m/%d/%y %H:%M:%S" format instead
                    res += wxString::Format(wxT("%02d/%02d/%04d %02d:%02d:%02d"),
                            tm.mon+1,tm.mday, tm.year, tm.hour, tm.min, tm.sec);
#endif // HAVE_STRFTIME/!HAVE_STRFTIME
                    break;

                case _T('d'):       // day of a month (01-31)
                    res += wxString::Format(fmt, tm.mday);
                    break;

                case _T('H'):       // hour in 24h format (00-23)
                    res += wxString::Format(fmt, tm.hour);
                    break;

                case _T('I'):       // hour in 12h format (01-12)
                    {
                        // 24h -> 12h, 0h -> 12h too
                        int hour12 = tm.hour > 12 ? tm.hour - 12
                                                  : tm.hour ? tm.hour : 12;
                        res += wxString::Format(fmt, hour12);
                    }
                    break;

                case _T('j'):       // day of the year
                    res += wxString::Format(fmt, GetDayOfYear(tz));
                    break;

                case _T('l'):       // milliseconds (NOT STANDARD)
                    res += wxString::Format(fmt, GetMillisecond(tz));
                    break;

                case _T('m'):       // month as a number (01-12)
                    res += wxString::Format(fmt, tm.mon + 1);
                    break;

                case _T('M'):       // minute as a decimal number (00-59)
                    res += wxString::Format(fmt, tm.min);
                    break;

                case _T('p'):       // AM or PM string
#ifdef HAVE_STRFTIME
                    res += CallStrftime(_T("%p"), &tmTimeOnly);
#else // !HAVE_STRFTIME
                    res += (tmTimeOnly.tm_hour > 12) ? wxT("pm") : wxT("am");
#endif // HAVE_STRFTIME/!HAVE_STRFTIME
                    break;

                case _T('S'):       // second as a decimal number (00-61)
                    res += wxString::Format(fmt, tm.sec);
                    break;

                case _T('U'):       // week number in the year (Sunday 1st week day)
                    res += wxString::Format(fmt, GetWeekOfYear(Sunday_First, tz));
                    break;

                case _T('W'):       // week number in the year (Monday 1st week day)
                    res += wxString::Format(fmt, GetWeekOfYear(Monday_First, tz));
                    break;

                case _T('w'):       // weekday as a number (0-6), Sunday = 0
                    res += wxString::Format(fmt, tm.GetWeekDay());
                    break;

                // case _T('x'): -- handled with "%c"

                case _T('X'):       // locale default time representation
                    // just use strftime() to format the time for us
#ifdef HAVE_STRFTIME
                    res += CallStrftime(_T("%X"), &tmTimeOnly);
#else // !HAVE_STRFTIME
                    res += wxString::Format(wxT("%02d:%02d:%02d"),tm.hour, tm.min, tm.sec);
#endif // HAVE_STRFTIME/!HAVE_STRFTIME
                    break;

                case _T('y'):       // year without century (00-99)
                    res += wxString::Format(fmt, tm.year % 100);
                    break;

                case _T('Y'):       // year with century
                    res += wxString::Format(fmt, tm.year);
                    break;

                case _T('Z'):       // timezone name
#ifdef HAVE_STRFTIME
                    res += CallStrftime(_T("%Z"), &tmTimeOnly);
#endif
                    break;

                default:
                    // is it the format width?
                    fmt.Empty();
                    while ( *p == _T('-') || *p == _T('+') ||
                            *p == _T(' ') || wxIsdigit(*p) )
                    {
                        fmt += *p;
                    }

                    if ( !fmt.empty() )
                    {
                        // we've only got the flags and width so far in fmt
                        fmt.Prepend(_T('%'));
                        fmt.Append(_T('d'));

                        restart = true;

                        break;
                    }

                    // no, it wasn't the width
                    wxFAIL_MSG(_T("unknown format specificator"));

                    // fall through and just copy it nevertheless

                case _T('%'):       // a percent sign
                    res += *p;
                    break;

                case 0:             // the end of string
                    wxFAIL_MSG(_T("missing format at the end of string"));

                    // just put the '%' which was the last char in format
                    res += _T('%');
                    break;
            }
        }
    }

    return res;
}

// this function parses a string in (strict) RFC 822 format: see the section 5
// of the RFC for the detailed description, but briefly it's something of the
// form "Sat, 18 Dec 1999 00:48:30 +0100"
//
// this function is "strict" by design - it must reject anything except true
// RFC822 time specs.
bool
wxDateTime::ParseRfc822Date(const wxString& date, wxString::const_iterator *end)
{
    wxString::const_iterator p = date.begin();

    // 1. week day
    static const int WDAY_LEN = 3;
    const wxString::const_iterator endWday = p + WDAY_LEN;
    const wxString wday(p, endWday);
    if ( GetWeekDayFromName(wday, Name_Abbr, DateLang_English) == Inv_WeekDay )
        return false;
    //else: ignore week day for now, we could also check that it really
    //      corresponds to the specified date

    p = endWday;

    // 2. separating comma
    if ( *p++ != ',' || *p++ != ' ' )
        return false;

    // 3. day number
    if ( !wxIsdigit(*p) )
        return false;

    wxDateTime_t day = (wxDateTime_t)(*p++ - '0');
    if ( wxIsdigit(*p) )
    {
        day *= 10;
        day = (wxDateTime_t)(day + (*p++ - '0'));
    }

    if ( *p++ != ' ' )
        return false;

    // 4. month name
    static const int MONTH_LEN = 3;
    const wxString::const_iterator endMonth = p + MONTH_LEN;
    const wxString monName(p, endMonth);
    Month mon = GetMonthFromName(monName, Name_Abbr, DateLang_English);
    if ( mon == Inv_Month )
        return false;

    p = endMonth;

    if ( *p++ != ' ' )
        return false;

    // 5. year
    if ( !wxIsdigit(*p) )
        return false;

    int year = *p++ - '0';
    if ( !wxIsdigit(*p) ) // should have at least 2 digits in the year
        return false;

    year *= 10;
    year += *p++ - '0';

    // is it a 2 digit year (as per original RFC 822) or a 4 digit one?
    if ( wxIsdigit(*p) )
    {
        year *= 10;
        year += *p++ - '0';

        if ( !wxIsdigit(*p) )
        {
            // no 3 digit years please
            return false;
        }

        year *= 10;
        year += *p++ - '0';
    }

    if ( *p++ != ' ' )
        return false;

    // 6. time in hh:mm:ss format with seconds being optional
    if ( !wxIsdigit(*p) )
        return false;

    wxDateTime_t hour = (wxDateTime_t)(*p++ - '0');

    if ( !wxIsdigit(*p) )
        return false;

    hour *= 10;
    hour = (wxDateTime_t)(hour + (*p++ - '0'));

    if ( *p++ != ':' )
        return false;

    if ( !wxIsdigit(*p) )
        return false;

    wxDateTime_t min = (wxDateTime_t)(*p++ - '0');

    if ( !wxIsdigit(*p) )
        return false;

    min *= 10;
    min += (wxDateTime_t)(*p++ - '0');

    wxDateTime_t sec = 0;
    if ( *p == ':' )
    {
        p++;
        if ( !wxIsdigit(*p) )
            return false;

        sec = (wxDateTime_t)(*p++ - '0');

        if ( !wxIsdigit(*p) )
            return false;

        sec *= 10;
        sec += (wxDateTime_t)(*p++ - '0');
    }

    if ( *p++ != ' ' )
        return false;

    // 7. now the interesting part: the timezone
    int offset wxDUMMY_INITIALIZE(0);
    if ( *p == '-' || *p == '+' )
    {
        // the explicit offset given: it has the form of hhmm
        bool plus = *p++ == '+';

        if ( !wxIsdigit(*p) || !wxIsdigit(*(p + 1)) )
            return false;


        // hours
        offset = MIN_PER_HOUR*(10*(*p - '0') + (*(p + 1) - '0'));

        p += 2;

        if ( !wxIsdigit(*p) || !wxIsdigit(*(p + 1)) )
            return false;

        // minutes
        offset += 10*(*p - '0') + (*(p + 1) - '0');

        if ( !plus )
            offset = -offset;

        p += 2;
    }
    else // not numeric
    {
        // the symbolic timezone given: may be either military timezone or one
        // of standard abbreviations
        if ( !*(p + 1) )
        {
            // military: Z = UTC, J unused, A = -1, ..., Y = +12
            static const int offsets[26] =
            {
                //A  B   C   D   E   F   G   H   I    J    K    L    M
                -1, -2, -3, -4, -5, -6, -7, -8, -9,   0, -10, -11, -12,
                //N  O   P   R   Q   S   T   U   V    W    Z    Y    Z
                +1, +2, +3, +4, +5, +6, +7, +8, +9, +10, +11, +12, 0
            };

            if ( *p < _T('A') || *p > _T('Z') || *p == _T('J') )
                return false;

            offset = offsets[*p++ - 'A'];
        }
        else
        {
            // abbreviation
            const wxString tz(p, date.end());
            if ( tz == _T("UT") || tz == _T("UTC") || tz == _T("GMT") )
                offset = 0;
            else if ( tz == _T("AST") )
                offset = AST - GMT0;
            else if ( tz == _T("ADT") )
                offset = ADT - GMT0;
            else if ( tz == _T("EST") )
                offset = EST - GMT0;
            else if ( tz == _T("EDT") )
                offset = EDT - GMT0;
            else if ( tz == _T("CST") )
                offset = CST - GMT0;
            else if ( tz == _T("CDT") )
                offset = CDT - GMT0;
            else if ( tz == _T("MST") )
                offset = MST - GMT0;
            else if ( tz == _T("MDT") )
                offset = MDT - GMT0;
            else if ( tz == _T("PST") )
                offset = PST - GMT0;
            else if ( tz == _T("PDT") )
                offset = PDT - GMT0;
            else
                return false;

            p += tz.length();
        }

        // make it minutes
        offset *= MIN_PER_HOUR;
    }


    // the spec was correct, construct the date from the values we found
    Set(day, mon, year, hour, min, sec);
    MakeFromTimezone(TimeZone::Make(offset*SEC_PER_MIN));

    if ( end )
        *end = p;

    return true;
}

#ifdef __WINDOWS__

// returns the string containing strftime() format used for short dates in the
// current locale or an empty string
static wxString GetLocaleDateFormat()
{
    wxString fmtWX;

    // there is no setlocale() under Windows CE, so just always query the
    // system there
#ifndef __WXWINCE__
    if ( strcmp(setlocale(LC_ALL, NULL), "C") != 0 )
#endif
    {
        // The locale was programatically set to non-C. We assume that this was
        // done using wxLocale, in which case thread's current locale is also
        // set to correct LCID value and we can use GetLocaleInfo to determine
        // the correct formatting string:
#ifdef __WXWINCE__
        LCID lcid = LOCALE_USER_DEFAULT;
#else
        LCID lcid = GetThreadLocale();
#endif
        // according to MSDN 80 chars is max allowed for short date format
        wxChar fmt[81];
        if ( ::GetLocaleInfo(lcid, LOCALE_SSHORTDATE, fmt, WXSIZEOF(fmt)) )
        {
            wxChar chLast = _T('\0');
            size_t lastCount = 0;
            for ( const wxChar *p = fmt; /* NUL handled inside */; p++ )
            {
                if ( *p == chLast )
                {
                    lastCount++;
                    continue;
                }

                switch ( *p )
                {
                    // these characters come in groups, start counting them
                    case _T('d'):
                    case _T('M'):
                    case _T('y'):
                    case _T('g'):
                        chLast = *p;
                        lastCount = 1;
                        break;

                    default:
                        // first deal with any special characters we have had
                        if ( lastCount )
                        {
                            switch ( chLast )
                            {
                                case _T('d'):
                                    switch ( lastCount )
                                    {
                                        case 1: // d
                                        case 2: // dd
                                            // these two are the same as we
                                            // don't distinguish between 1 and
                                            // 2 digits for days
                                            fmtWX += _T("%d");
                                            break;

                                        case 3: // ddd
                                            fmtWX += _T("%a");
                                            break;

                                        case 4: // dddd
                                            fmtWX += _T("%A");
                                            break;

                                        default:
                                            wxFAIL_MSG( _T("too many 'd's") );
                                    }
                                    break;

                                case _T('M'):
                                    switch ( lastCount )
                                    {
                                        case 1: // M
                                        case 2: // MM
                                            // as for 'd' and 'dd' above
                                            fmtWX += _T("%m");
                                            break;

                                        case 3:
                                            fmtWX += _T("%b");
                                            break;

                                        case 4:
                                            fmtWX += _T("%B");
                                            break;

                                        default:
                                            wxFAIL_MSG( _T("too many 'M's") );
                                    }
                                    break;

                                case _T('y'):
                                    switch ( lastCount )
                                    {
                                        case 1: // y
                                        case 2: // yy
                                            fmtWX += _T("%y");
                                            break;

                                        case 4: // yyyy
                                            fmtWX += _T("%Y");
                                            break;

                                        default:
                                            wxFAIL_MSG( _T("wrong number of 'y's") );
                                    }
                                    break;

                                case _T('g'):
                                    // strftime() doesn't have era string,
                                    // ignore this format
                                    wxASSERT_MSG( lastCount <= 2,
                                                  _T("too many 'g's") );
                                    break;

                                default:
                                    wxFAIL_MSG( _T("unreachable") );
                            }

                            chLast = _T('\0');
                            lastCount = 0;
                        }

                        // not a special character so must be just a separator,
                        // treat as is
                        if ( *p != _T('\0') )
                        {
                            if ( *p == _T('%') )
                            {
                                // this one needs to be escaped
                                fmtWX += _T('%');
                            }

                            fmtWX += *p;
                        }
                }

                if ( *p == _T('\0') )
                    break;
            }
        }
        //else: GetLocaleInfo() failed, leave fmtDate value unchanged and
        //      try our luck with the default formats
    }
    //else: default C locale, default formats should work

    return fmtWX;
}

#endif // __WINDOWS__

bool
wxDateTime::ParseFormat(const wxString& date,
                        const wxString& format,
                        const wxDateTime& dateDef,
                        wxString::const_iterator *endParse)
{
    wxCHECK_MSG( !format.empty(), false, "format can't be empty" );
    wxCHECK_MSG( endParse, false, "end iterator pointer must be specified" );

    wxString str;
    unsigned long num;

    // what fields have we found?
    bool haveWDay = false,
         haveYDay = false,
         haveDay = false,
         haveMon = false,
         haveYear = false,
         haveHour = false,
         haveMin = false,
         haveSec = false,
         haveMsec = false;

    bool hourIsIn12hFormat = false, // or in 24h one?
         isPM = false;              // AM by default

    // and the value of the items we have (init them to get rid of warnings)
    wxDateTime_t msec = 0,
                 sec = 0,
                 min = 0,
                 hour = 0;
    WeekDay wday = Inv_WeekDay;
    wxDateTime_t yday = 0,
                 mday = 0;
    wxDateTime::Month mon = Inv_Month;
    int year = 0;

    wxString::const_iterator input = date.begin();
    const wxString::const_iterator end = date.end();
    for ( wxString::const_iterator fmt = format.begin(); fmt != format.end(); ++fmt )
    {
        if ( *fmt != _T('%') )
        {
            if ( wxIsspace(*fmt) )
            {
                // a white space in the format string matches 0 or more white
                // spaces in the input
                while ( wxIsspace(*input) )
                {
                    input++;
                }
            }
            else // !space
            {
                // any other character (not whitespace, not '%') must be
                // matched by itself in the input
                if ( *input++ != *fmt )
                {
                    // no match
                    return false;
                }
            }

            // done with this format char
            continue;
        }

        // start of a format specification

        // parse the optional width
        size_t width = 0;
        while ( wxIsdigit(*++fmt) )
        {
            width *= 10;
            width += *fmt - '0';
        }

        // the default widths for the various fields
        if ( !width )
        {
            switch ( (*fmt).GetValue() )
            {
                case _T('Y'):               // year has 4 digits
                    width = 4;
                    break;

                case _T('j'):               // day of year has 3 digits
                case _T('l'):               // milliseconds have 3 digits
                    width = 3;
                    break;

                case _T('w'):               // week day as number has only one
                    width = 1;
                    break;

                default:
                    // default for all other fields
                    width = 2;
            }
        }

        // then the format itself
        switch ( (*fmt).GetValue() )
        {
            case _T('a'):       // a weekday name
            case _T('A'):
                {
                    wday = GetWeekDayFromName
                           (
                            GetAlphaToken(input, end),
                            *fmt == 'a' ? Name_Abbr : Name_Full,
                            DateLang_Local
                           );
                    if ( wday == Inv_WeekDay )
                    {
                        // no match
                        return false;
                    }
                }
                haveWDay = true;
                break;

            case _T('b'):       // a month name
            case _T('B'):
                {
                    mon = GetMonthFromName
                          (
                            GetAlphaToken(input, end),
                            *fmt == 'b' ? Name_Abbr : Name_Full,
                            DateLang_Local
                          );
                    if ( mon == Inv_Month )
                    {
                        // no match
                        return false;
                    }
                }
                haveMon = true;
                break;

            case _T('c'):       // locale default date and time  representation
                {
#ifdef HAVE_STRPTIME
                    struct tm tm;

                    // try using strptime() -- it may fail even if the input is
                    // correct but the date is out of range, so we will fall back
                    // to our generic code anyhow
                    if ( CallStrptime(date, input, "%c", &tm) )
                    {
                        hour = tm.tm_hour;
                        min = tm.tm_min;
                        sec = tm.tm_sec;

                        year = 1900 + tm.tm_year;
                        mon = (Month)tm.tm_mon;
                        mday = tm.tm_mday;
                    }
                    else // strptime() failed; try generic heuristic code
#endif // HAVE_STRPTIME
                    {

                        // try the format which corresponds to ctime() output
                        // first, then the generic date/time formats
                        const wxDateTime dt = ParseFormatAt
                                              (
                                                input,
                                                end,
                                                wxS("%a %b %d %H:%M:%S %Y"),
                                                wxS("%x %X"),
                                                wxS("%X %x")
                                              );
                        if ( !dt.IsValid() )
                            return false;

                        Tm tm = dt.GetTm();

                        hour = tm.hour;
                        min = tm.min;
                        sec = tm.sec;

                        year = tm.year;
                        mon = tm.mon;
                        mday = tm.mday;
                    }

                    haveDay = haveMon = haveYear =
                    haveHour = haveMin = haveSec = true;
                }
                break;

            case _T('d'):       // day of a month (01-31)
                if ( !GetNumericToken(width, input, end, &num) ||
                        (num > 31) || (num < 1) )
                {
                    // no match
                    return false;
                }

                // we can't check whether the day range is correct yet, will
                // do it later - assume ok for now
                haveDay = true;
                mday = (wxDateTime_t)num;
                break;

            case _T('H'):       // hour in 24h format (00-23)
                if ( !GetNumericToken(width, input, end, &num) || (num > 23) )
                {
                    // no match
                    return false;
                }

                haveHour = true;
                hour = (wxDateTime_t)num;
                break;

            case _T('I'):       // hour in 12h format (01-12)
                if ( !GetNumericToken(width, input, end, &num) ||
                        !num || (num > 12) )
                {
                    // no match
                    return false;
                }

                haveHour = true;
                hourIsIn12hFormat = true;
                hour = (wxDateTime_t)(num % 12);        // 12 should be 0
                break;

            case _T('j'):       // day of the year
                if ( !GetNumericToken(width, input, end, &num) ||
                        !num || (num > 366) )
                {
                    // no match
                    return false;
                }

                haveYDay = true;
                yday = (wxDateTime_t)num;
                break;

            case _T('l'):       // milliseconds (0-999)
                if ( !GetNumericToken(width, input, end, &num) )
                    return false;

                haveMsec = true;
                msec = (wxDateTime_t)num;
                break;

            case _T('m'):       // month as a number (01-12)
                if ( !GetNumericToken(width, input, end, &num) ||
                        !num || (num > 12) )
                {
                    // no match
                    return false;
                }

                haveMon = true;
                mon = (Month)(num - 1);
                break;

            case _T('M'):       // minute as a decimal number (00-59)
                if ( !GetNumericToken(width, input, end, &num) ||
                        (num > 59) )
                {
                    // no match
                    return false;
                }

                haveMin = true;
                min = (wxDateTime_t)num;
                break;

            case _T('p'):       // AM or PM string
                {
                    wxString am, pm, token = GetAlphaToken(input, end);

                    // some locales have empty AM/PM tokens and thus when formatting
                    // dates with the %p specifier nothing is generated; when trying to
                    // parse them back, we get an empty token here... but that's not
                    // an error.
                    if (token.empty())
                        break;

                    GetAmPmStrings(&am, &pm);
                    if (am.empty() && pm.empty())
                        return false;  // no am/pm strings defined
                    if ( token.CmpNoCase(pm) == 0 )
                    {
                        isPM = true;
                    }
                    else if ( token.CmpNoCase(am) != 0 )
                    {
                        // no match
                        return false;
                    }
                }
                break;

            case _T('r'):       // time as %I:%M:%S %p
                {
                    wxDateTime dt;
                    if ( !dt.ParseFormat(wxString(input, end),
                                         wxS("%I:%M:%S %p"), &input) )
                        return false;

                    haveHour = haveMin = haveSec = true;

                    Tm tm = dt.GetTm();
                    hour = tm.hour;
                    min = tm.min;
                    sec = tm.sec;
                }
                break;

            case _T('R'):       // time as %H:%M
                {
                    const wxDateTime
                        dt = ParseFormatAt(input, end, wxS("%H:%M"));
                    if ( !dt.IsValid() )
                        return false;

                    haveHour =
                    haveMin = true;

                    Tm tm = dt.GetTm();
                    hour = tm.hour;
                    min = tm.min;
                }
                break;

            case _T('S'):       // second as a decimal number (00-61)
                if ( !GetNumericToken(width, input, end, &num) ||
                        (num > 61) )
                {
                    // no match
                    return false;
                }

                haveSec = true;
                sec = (wxDateTime_t)num;
                break;

            case _T('T'):       // time as %H:%M:%S
                {
                    const wxDateTime
                        dt = ParseFormatAt(input, end, wxS("%H:%M:%S"));
                    if ( !dt.IsValid() )
                        return false;

                    haveHour =
                    haveMin =
                    haveSec = true;

                    Tm tm = dt.GetTm();
                    hour = tm.hour;
                    min = tm.min;
                    sec = tm.sec;
                }
                break;

            case _T('w'):       // weekday as a number (0-6), Sunday = 0
                if ( !GetNumericToken(width, input, end, &num) ||
                        (wday > 6) )
                {
                    // no match
                    return false;
                }

                haveWDay = true;
                wday = (WeekDay)num;
                break;

            case _T('x'):       // locale default date representation
#ifdef HAVE_STRPTIME
                // try using strptime() -- it may fail even if the input is
                // correct but the date is out of range, so we will fall back
                // to our generic code anyhow
                {
                    struct tm tm;

                    if ( CallStrptime(date, input, "%x", &tm) )
                    {
                        haveDay = haveMon = haveYear = true;

                        year = 1900 + tm.tm_year;
                        mon = (Month)tm.tm_mon;
                        mday = tm.tm_mday;

                        break;
                    }
                }
#endif // HAVE_STRPTIME

                {
                    wxString fmtDate,
                             fmtDateAlt;

#ifdef __WINDOWS__
                    // The above doesn't work for all locales, try to query
                    // Windows for the right way of formatting the date:
                    fmtDate = GetLocaleDateFormat();
                    if ( fmtDate.empty() )
#endif // __WINDOWS__
                    {
                        if ( IsWestEuropeanCountry(GetCountry()) ||
                             GetCountry() == Russia )
                        {
                            fmtDate = _T("%d/%m/%y");
                            fmtDateAlt = _T("%m/%d/%y");
                        }
                        else // assume USA
                        {
                            fmtDate = _T("%m/%d/%y");
                            fmtDateAlt = _T("%d/%m/%y");
                        }
                    }

                    const wxDateTime
                        dt = ParseFormatAt(input, end,
                                           fmtDate, fmtDateAlt);
                    if ( !dt.IsValid() )
                        return false;

                    Tm tm = dt.GetTm();

                    haveDay =
                    haveMon =
                    haveYear = true;

                    year = tm.year;
                    mon = tm.mon;
                    mday = tm.mday;
                }

                break;

            case _T('X'):       // locale default time representation
#ifdef HAVE_STRPTIME
                {
                    // use strptime() to do it for us (FIXME !Unicode friendly)
                    struct tm tm;
                    if ( !CallStrptime(date, input, "%X", &tm) )
                        return false;

                    haveHour = haveMin = haveSec = true;

                    hour = tm.tm_hour;
                    min = tm.tm_min;
                    sec = tm.tm_sec;
                }
#else // !HAVE_STRPTIME
                // TODO under Win32 we can query the LOCALE_ITIME system
                //      setting which says whether the default time format is
                //      24 or 12 hour
                {
                    // try to parse what follows as "%H:%M:%S" and, if this
                    // fails, as "%I:%M:%S %p" - this should catch the most
                    // common cases
                    const wxDateTime
                        dt = ParseFormatAt(input, end, "%T", "%r");
                    if ( !dt.IsValid() )
                        return false;

                    haveHour =
                    haveMin =
                    haveSec = true;

                    Tm tm = dt.GetTm();
                    hour = tm.hour;
                    min = tm.min;
                    sec = tm.sec;
                }
#endif // HAVE_STRPTIME/!HAVE_STRPTIME
                break;

            case _T('y'):       // year without century (00-99)
                if ( !GetNumericToken(width, input, end, &num) ||
                        (num > 99) )
                {
                    // no match
                    return false;
                }

                haveYear = true;

                // TODO should have an option for roll over date instead of
                //      hard coding it here
                year = (num > 30 ? 1900 : 2000) + (wxDateTime_t)num;
                break;

            case _T('Y'):       // year with century
                if ( !GetNumericToken(width, input, end, &num) )
                {
                    // no match
                    return false;
                }

                haveYear = true;
                year = (wxDateTime_t)num;
                break;

            case _T('Z'):       // timezone name
                wxFAIL_MSG(_T("TODO"));
                break;

            case _T('%'):       // a percent sign
                if ( *input++ != _T('%') )
                {
                    // no match
                    return false;
                }
                break;

            case 0:             // the end of string
                wxFAIL_MSG(_T("unexpected format end"));

                // fall through

            default:            // not a known format spec
                return false;
        }
    }

    // format matched, try to construct a date from what we have now
    Tm tmDef;
    if ( dateDef.IsValid() )
    {
        // take this date as default
        tmDef = dateDef.GetTm();
    }
    else if ( IsValid() )
    {
        // if this date is valid, don't change it
        tmDef = GetTm();
    }
    else
    {
        // no default and this date is invalid - fall back to Today()
        tmDef = Today().GetTm();
    }

    Tm tm = tmDef;

    // set the date
    if ( haveMon )
    {
        tm.mon = mon;
    }

    if ( haveYear )
    {
        tm.year = year;
    }

    // TODO we don't check here that the values are consistent, if both year
    //      day and month/day were found, we just ignore the year day and we
    //      also always ignore the week day
    if ( haveDay )
    {
        if ( mday > GetNumberOfDays(tm.mon, tm.year) )
            return false;

        tm.mday = mday;
    }
    else if ( haveYDay )
    {
        if ( yday > GetNumberOfDays(tm.year) )
            return false;

        Tm tm2 = wxDateTime(1, Jan, tm.year).SetToYearDay(yday).GetTm();

        tm.mon = tm2.mon;
        tm.mday = tm2.mday;
    }

    // deal with AM/PM
    if ( haveHour && hourIsIn12hFormat && isPM )
    {
        // translate to 24hour format
        hour += 12;
    }
    //else: either already in 24h format or no translation needed

    // set the time
    if ( haveHour )
    {
        tm.hour = hour;
    }

    if ( haveMin )
    {
        tm.min = min;
    }

    if ( haveSec )
    {
        tm.sec = sec;
    }

    if ( haveMsec )
        tm.msec = msec;

    Set(tm);

    // finally check that the week day is consistent -- if we had it
    if ( haveWDay && GetWeekDay() != wday )
        return false;

    *endParse = input;

    return true;
}

bool
wxDateTime::ParseDateTime(const wxString& date, wxString::const_iterator *end)
{
    wxCHECK_MSG( end, false, "end iterator pointer must be specified" );

    // Set to current day and hour, so strings like '14:00' becomes today at
    // 14, not some other random date
    wxDateTime dtDate = wxDateTime::Today();
    wxDateTime dtTime = wxDateTime::Today();

    wxString::const_iterator
        endTime,
        endDate,
        endBoth;

    // If we got a date in the beginning, see if there is a time specified
    // after the date
    if ( dtDate.ParseDate(date, &endDate) )
    {
        // Skip spaces, as the ParseTime() function fails on spaces
        while ( endDate != date.end() && wxIsspace(*endDate) )
            ++endDate;

        const wxString timestr(endDate, date.end());
        if ( !dtTime.ParseTime(timestr, &endTime) )
            return false;

        endBoth = endDate + (endTime - timestr.begin());
    }
    else // no date in the beginning
    {
        // check if we have a time followed by a date
        if ( !dtTime.ParseTime(date, &endTime) )
            return false;

        while ( endTime != date.end() && wxIsspace(*endTime) )
            ++endTime;

        const wxString datestr(endTime, date.end());
        if ( !dtDate.ParseDate(datestr, &endDate) )
            return false;

        endBoth = endTime + (endDate - datestr.begin());
    }

    Set(dtDate.GetDay(), dtDate.GetMonth(), dtDate.GetYear(),
        dtTime.GetHour(), dtTime.GetMinute(), dtTime.GetSecond(),
        dtTime.GetMillisecond());

    *end = endBoth;

    return true;
}

bool
wxDateTime::ParseDate(const wxString& date, wxString::const_iterator *end)
{
    wxCHECK_MSG( end, false, "end iterator pointer must be specified" );

    // this is a simplified version of ParseDateTime() which understands only
    // "today" (for wxDate compatibility) and digits only otherwise (and not
    // all esoteric constructions ParseDateTime() knows about)

    const wxString::const_iterator pBegin = date.begin();

    wxString::const_iterator p = pBegin;
    while ( wxIsspace(*p) )
        p++;

    // some special cases
    static struct
    {
        const char *str;
        int dayDiffFromToday;
    } literalDates[] =
    {
        { wxTRANSLATE("today"),             0 },
        { wxTRANSLATE("yesterday"),        -1 },
        { wxTRANSLATE("tomorrow"),          1 },
    };

    const size_t lenRest = date.end() - p;
    for ( size_t n = 0; n < WXSIZEOF(literalDates); n++ )
    {
        const wxString dateStr = wxGetTranslation(literalDates[n].str);
        size_t len = dateStr.length();

        if ( len > lenRest )
            continue;

        const wxString::const_iterator pEnd = p + len;
        if ( wxString(p, pEnd).CmpNoCase(dateStr) == 0 )
        {
            // nothing can follow this, so stop here

            p = pEnd;

            int dayDiffFromToday = literalDates[n].dayDiffFromToday;
            *this = Today();
            if ( dayDiffFromToday )
            {
                *this += wxDateSpan::Days(dayDiffFromToday);
            }

            *end = pEnd;

            return true;
        }
    }

    // We try to guess what we have here: for each new (numeric) token, we
    // determine if it can be a month, day or a year. Of course, there is an
    // ambiguity as some numbers may be days as well as months, so we also
    // have the ability to back track.

    // what do we have?
    bool haveDay = false,       // the months day?
         haveWDay = false,      // the day of week?
         haveMon = false,       // the month?
         haveYear = false;      // the year?

    // and the value of the items we have (init them to get rid of warnings)
    WeekDay wday = Inv_WeekDay;
    wxDateTime_t day = 0;
    wxDateTime::Month mon = Inv_Month;
    int year = 0;

    // tokenize the string
    size_t nPosCur = 0;
    static const wxStringCharType *dateDelimiters = wxS(".,/-\t\r\n ");
    wxStringTokenizer tok(wxString(p, date.end()), dateDelimiters);
    while ( tok.HasMoreTokens() )
    {
        wxString token = tok.GetNextToken();
        if ( !token )
            continue;

        // is it a number?
        unsigned long val;
        if ( token.ToULong(&val) )
        {
            // guess what this number is

            bool isDay = false,
                 isMonth = false,
                 isYear = false;

            if ( !haveMon && val > 0 && val <= 12 )
            {
                // assume it is month
                isMonth = true;
            }
            else // not the month
            {
                if ( haveDay )
                {
                    // this can only be the year
                    isYear = true;
                }
                else // may be either day or year
                {
                    // use a leap year if we don't have the year yet to allow
                    // dates like 2/29/1976 which would be rejected otherwise
                    wxDateTime_t max_days = (wxDateTime_t)(
                        haveMon
                        ? GetNumberOfDays(mon, haveYear ? year : 1976)
                        : 31
                    );

                    // can it be day?
                    if ( (val == 0) || (val > (unsigned long)max_days) )
                    {
                        // no
                        isYear = true;
                    }
                    else // yes, suppose it's the day
                    {
                        isDay = true;
                    }
                }
            }

            if ( isYear )
            {
                if ( haveYear )
                    break;

                haveYear = true;

                year = (wxDateTime_t)val;
            }
            else if ( isDay )
            {
                if ( haveDay )
                    break;

                haveDay = true;

                day = (wxDateTime_t)val;
            }
            else if ( isMonth )
            {
                haveMon = true;

                mon = (Month)(val - 1);
            }
        }
        else // not a number
        {
            // be careful not to overwrite the current mon value
            Month mon2 = GetMonthFromName
                         (
                            token,
                            Name_Full | Name_Abbr,
                            DateLang_Local | DateLang_English
                         );
            if ( mon2 != Inv_Month )
            {
                // it's a month
                if ( haveMon )
                {
                    // but we already have a month - maybe we guessed wrong?
                    if ( !haveDay )
                    {
                        // no need to check in month range as always < 12, but
                        // the days are counted from 1 unlike the months
                        day = (wxDateTime_t)(mon + 1);
                        haveDay = true;
                    }
                    else
                    {
                        // could possible be the year (doesn't the year come
                        // before the month in the japanese format?) (FIXME)
                        break;
                    }
                }

                mon = mon2;

                haveMon = true;
            }
            else // not a valid month name
            {
                WeekDay wday2 = GetWeekDayFromName
                                (
                                    token,
                                    Name_Full | Name_Abbr,
                                    DateLang_Local | DateLang_English
                                );
                if ( wday2 != Inv_WeekDay )
                {
                    // a week day
                    if ( haveWDay )
                    {
                        break;
                    }

                    wday = wday2;

                    haveWDay = true;
                }
                else // not a valid weekday name
                {
                    // try the ordinals
                    static const char *ordinals[] =
                    {
                        wxTRANSLATE("first"),
                        wxTRANSLATE("second"),
                        wxTRANSLATE("third"),
                        wxTRANSLATE("fourth"),
                        wxTRANSLATE("fifth"),
                        wxTRANSLATE("sixth"),
                        wxTRANSLATE("seventh"),
                        wxTRANSLATE("eighth"),
                        wxTRANSLATE("ninth"),
                        wxTRANSLATE("tenth"),
                        wxTRANSLATE("eleventh"),
                        wxTRANSLATE("twelfth"),
                        wxTRANSLATE("thirteenth"),
                        wxTRANSLATE("fourteenth"),
                        wxTRANSLATE("fifteenth"),
                        wxTRANSLATE("sixteenth"),
                        wxTRANSLATE("seventeenth"),
                        wxTRANSLATE("eighteenth"),
                        wxTRANSLATE("nineteenth"),
                        wxTRANSLATE("twentieth"),
                        // that's enough - otherwise we'd have problems with
                        // composite (or not) ordinals
                    };

                    size_t n;
                    for ( n = 0; n < WXSIZEOF(ordinals); n++ )
                    {
                        if ( token.CmpNoCase(ordinals[n]) == 0 )
                        {
                            break;
                        }
                    }

                    if ( n == WXSIZEOF(ordinals) )
                    {
                        // stop here - something unknown
                        break;
                    }

                    // it's a day
                    if ( haveDay )
                    {
                        // don't try anything here (as in case of numeric day
                        // above) - the symbolic day spec should always
                        // precede the month/year
                        break;
                    }

                    haveDay = true;

                    day = (wxDateTime_t)(n + 1);
                }
            }
        }

        nPosCur = tok.GetPosition();
    }

    // either no more tokens or the scan was stopped by something we couldn't
    // parse - in any case, see if we can construct a date from what we have
    if ( !haveDay && !haveWDay )
        return false;

    if ( haveWDay && (haveMon || haveYear || haveDay) &&
         !(haveDay && haveMon && haveYear) )
    {
        // without adjectives (which we don't support here) the week day only
        // makes sense completely separately or with the full date
        // specification (what would "Wed 1999" mean?)
        return false;
    }

    if ( !haveWDay && haveYear && !(haveDay && haveMon) )
    {
        // may be we have month and day instead of day and year?
        if ( haveDay && !haveMon )
        {
            if ( day <= 12  )
            {
                // exchange day and month
                mon = (wxDateTime::Month)(day - 1);

                // we're in the current year then
                if ( (year > 0) && (year <= (int)GetNumberOfDays(mon, Inv_Year)) )
                {
                    day = (wxDateTime_t)year;

                    haveMon = true;
                    haveYear = false;
                }
                //else: no, can't exchange, leave haveMon == false
            }
        }

        if ( !haveMon )
            return false;
    }

    if ( !haveMon )
    {
        mon = GetCurrentMonth();
    }

    if ( !haveYear )
    {
        year = GetCurrentYear();
    }

    if ( haveDay )
    {
        // normally we check the day above but the check is optimistic in case
        // we find the day before its month/year so we have to redo it now
        if ( day > GetNumberOfDays(mon, year) )
            return false;

        Set(day, mon, year);

        if ( haveWDay )
        {
            // check that it is really the same
            if ( GetWeekDay() != wday )
                return false;
        }
    }
    else // haveWDay
    {
        *this = Today();

        SetToWeekDayInSameWeek(wday);
    }

    // return the pointer to the first unparsed char
    p += nPosCur;
    if ( nPosCur && wxStrchr(dateDelimiters, *(p - 1)) )
    {
        // if we couldn't parse the token after the delimiter, put back the
        // delimiter as well
        p--;
    }

    *end = p;

    return true;
}

bool
wxDateTime::ParseTime(const wxString& time, wxString::const_iterator *end)
{
    wxCHECK_MSG( end, false, "end iterator pointer must be specified" );

    // first try some extra things
    static const struct
    {
        const char *name;
        wxDateTime_t hour;
    } stdTimes[] =
    {
        { wxTRANSLATE("noon"),      12 },
        { wxTRANSLATE("midnight"),  00 },
        // anything else?
    };

    for ( size_t n = 0; n < WXSIZEOF(stdTimes); n++ )
    {
        const wxString timeString = wxGetTranslation(stdTimes[n].name);
        const wxString::const_iterator p = time.begin() + timeString.length();
        if ( timeString.CmpNoCase(wxString(time.begin(), p)) == 0 )
        {
            // casts required by DigitalMars
            Set(stdTimes[n].hour, wxDateTime_t(0), wxDateTime_t(0));

            if ( end )
                *end = p;

            return true;
        }
    }

    // try all time formats we may think about in the order from longest to
    // shortest
    static const char *timeFormats[] =
    {
        "%I:%M:%S %p",  // 12hour with AM/PM
        "%H:%M:%S",     // could be the same or 24 hour one so try it too
        "%I:%M %p",     // 12hour with AM/PM but without seconds
        "%H:%M:%S",     // and a possibly 24 hour version without seconds
        "%X",           // possibly something from above or maybe something
                        // completely different -- try it last

        // TODO: parse timezones
    };

    for ( size_t nFmt = 0; nFmt < WXSIZEOF(timeFormats); nFmt++ )
    {
        if ( ParseFormat(time, timeFormats[nFmt], end) )
            return true;
    }

    return false;
}

// ----------------------------------------------------------------------------
// Workdays and holidays support
// ----------------------------------------------------------------------------

bool wxDateTime::IsWorkDay(Country WXUNUSED(country)) const
{
    return !wxDateTimeHolidayAuthority::IsHoliday(*this);
}

// ============================================================================
// wxDateSpan
// ============================================================================

wxDateSpan WXDLLIMPEXP_BASE operator*(int n, const wxDateSpan& ds)
{
    wxDateSpan ds1(ds);
    return ds1.Multiply(n);
}

// ============================================================================
// wxTimeSpan
// ============================================================================

wxTimeSpan WXDLLIMPEXP_BASE operator*(int n, const wxTimeSpan& ts)
{
    return wxTimeSpan(ts).Multiply(n);
}

// this enum is only used in wxTimeSpan::Format() below but we can't declare
// it locally to the method as it provokes an internal compiler error in egcs
// 2.91.60 when building with -O2
enum TimeSpanPart
{
    Part_Week,
    Part_Day,
    Part_Hour,
    Part_Min,
    Part_Sec,
    Part_MSec
};

// not all strftime(3) format specifiers make sense here because, for example,
// a time span doesn't have a year nor a timezone
//
// Here are the ones which are supported (all of them are supported by strftime
// as well):
//  %H          hour in 24 hour format
//  %M          minute (00 - 59)
//  %S          second (00 - 59)
//  %%          percent sign
//
// Also, for MFC CTimeSpan compatibility, we support
//  %D          number of days
//
// And, to be better than MFC :-), we also have
//  %E          number of wEeks
//  %l          milliseconds (000 - 999)
wxString wxTimeSpan::Format(const wxString& format) const
{
    // we deal with only positive time spans here and just add the sign in
    // front for the negative ones
    if ( IsNegative() )
    {
        wxString str(Negate().Format(format));
        return "-" + str;
    }

    wxCHECK_MSG( !format.empty(), wxEmptyString,
                 _T("NULL format in wxTimeSpan::Format") );

    wxString str;
    str.Alloc(format.length());

    // Suppose we have wxTimeSpan ts(1 /* hour */, 2 /* min */, 3 /* sec */)
    //
    // Then, of course, ts.Format("%H:%M:%S") must return "01:02:03", but the
    // question is what should ts.Format("%S") do? The code here returns "3273"
    // in this case (i.e. the total number of seconds, not just seconds % 60)
    // because, for me, this call means "give me entire time interval in
    // seconds" and not "give me the seconds part of the time interval"
    //
    // If we agree that it should behave like this, it is clear that the
    // interpretation of each format specifier depends on the presence of the
    // other format specs in the string: if there was "%H" before "%M", we
    // should use GetMinutes() % 60, otherwise just GetMinutes() &c

    // we remember the most important unit found so far
    TimeSpanPart partBiggest = Part_MSec;

    for ( wxString::const_iterator pch = format.begin(); pch != format.end(); ++pch )
    {
        wxChar ch = *pch;

        if ( ch == _T('%') )
        {
            // the start of the format specification of the printf() below
            wxString fmtPrefix(_T('%'));

            // the number
            long n;

            // the number of digits for the format string, 0 if unused
            unsigned digits = 0;

            ch = *++pch;    // get the format spec char
            switch ( ch )
            {
                default:
                    wxFAIL_MSG( _T("invalid format character") );
                    // fall through

                case _T('%'):
                    str += ch;

                    // skip the part below switch
                    continue;

                case _T('D'):
                    n = GetDays();
                    if ( partBiggest < Part_Day )
                    {
                        n %= DAYS_PER_WEEK;
                    }
                    else
                    {
                        partBiggest = Part_Day;
                    }
                    break;

                case _T('E'):
                    partBiggest = Part_Week;
                    n = GetWeeks();
                    break;

                case _T('H'):
                    n = GetHours();
                    if ( partBiggest < Part_Hour )
                    {
                        n %= HOURS_PER_DAY;
                    }
                    else
                    {
                        partBiggest = Part_Hour;
                    }

                    digits = 2;
                    break;

                case _T('l'):
                    n = GetMilliseconds().ToLong();
                    if ( partBiggest < Part_MSec )
                    {
                        n %= 1000;
                    }
                    //else: no need to reset partBiggest to Part_MSec, it is
                    //      the least significant one anyhow

                    digits = 3;
                    break;

                case _T('M'):
                    n = GetMinutes();
                    if ( partBiggest < Part_Min )
                    {
                        n %= MIN_PER_HOUR;
                    }
                    else
                    {
                        partBiggest = Part_Min;
                    }

                    digits = 2;
                    break;

                case _T('S'):
                    n = GetSeconds().ToLong();
                    if ( partBiggest < Part_Sec )
                    {
                        n %= SEC_PER_MIN;
                    }
                    else
                    {
                        partBiggest = Part_Sec;
                    }

                    digits = 2;
                    break;
            }

            if ( digits )
            {
                fmtPrefix << _T("0") << digits;
            }

            str += wxString::Format(fmtPrefix + _T("ld"), n);
        }
        else
        {
            // normal character, just copy
            str += ch;
        }
    }

    return str;
}

#endif // wxUSE_DATETIME