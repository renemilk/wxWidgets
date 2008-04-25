/////////////////////////////////////////////////////////////////////////////
// Name:        string.h
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_string wxString Overview

Classes: wxString, wxArrayString, wxStringTokenizer

@li @ref overview_string_intro
@li @ref overview_string_comparison
@li @ref overview_string_advice
@li @ref overview_string_related
@li @ref overview_string_refcount
@li @ref overview_string_tuning


<hr>


@section overview_string_intro Introduction

wxString is a class which represents a character string of arbitrary length and
containing arbitrary characters. The ASCII NUL character is allowed, but be
aware that in the current string implementation some methods might not work
correctly in this case.

Since wxWidgets 3.0 wxString internally uses UCS-2 (basically 2-byte per
character wchar_t) under Windows and UTF-8 under Unix, Linux and
OS X to store its content. Much work has been done to make
existing code using ANSI string literals work as before.

This class has all the standard operations you can expect to find in a string
class: dynamic memory management (string extends to accommodate new
characters), construction from other strings, C strings, wide character C strings 
and characters, assignment operators, access to individual characters, string
concatenation and comparison, substring extraction, case conversion, trimming and padding (with
spaces), searching and replacing and both C-like @c printf (wxString::Printf)
and stream-like insertion functions as well as much more - see wxString for a
list of all functions.


@section overview_string_comparison Comparison to Other String Classes

The advantages of using a special string class instead of working directly with
C strings are so obvious that there is a huge number of such classes available.
The most important advantage is the need to always remember to allocate/free
memory for C strings; working with fixed size buffers almost inevitably leads
to buffer overflows. At last, C++ has a standard string class (std::string). So
why the need for wxString? There are several advantages:

@li <b>Efficiency:</b> This class was made to be as efficient as possible: both in
    terms of size (each wxString objects takes exactly the same space as a
    <tt>char*</tt> pointer, see @ref overview_string_refcount
    "reference counting") and speed. It also provides performance
    @ref overview_string_tuning "statistics gathering code" which may be
    enabled to fine tune the memory allocation strategy for your particular
    application - and the gain might be quite big.
@li <b>Compatibility:</b> This class tries to combine almost full compatibility
    with the old wxWidgets 1.xx wxString class, some reminiscence to MFC
    CString class and 90% of the functionality of std::string class.
@li <b>Rich set of functions:</b> Some of the functions present in wxString are very
    useful but don't exist in most of other string classes: for example,
    wxString::AfterFirst, wxString::BeforeLast, wxString::operators or
    wxString::Printf. Of course, all the standard string operations are
    supported as well.
@li <b>Unicode wxString is Unicode friendly:</b> it allows to easily convert to
    and from ANSI and Unicode strings in any build mode (see the
    @ref overview_unicode "unicode overview" for more details) and maps to
    either @c string or @c wstring transparently depending on the current mode.
@li <b>Used by wxWidgets:</b> And, of course, this class is used everywhere
    inside wxWidgets so there is no performance loss which would result from
    conversions of objects of any other string class (including std::string) to
    wxString internally by wxWidgets.

However, there are several problems as well. The most important one is probably
that there are often several functions to do exactly the same thing: for
example, to get the length of the string either one of @c length(),
wxString::Len() or wxString::Length() may be used. The first function, as
almost all the other functions in lowercase, is std::string compatible. The
second one is the "native" wxString version and the last one is the wxWidgets
1.xx way.

So which is better to use? The usage of the std::string compatible functions is
strongly advised! It will both make your code more familiar to other C++
programmers (who are supposed to have knowledge of std::string but not of
wxString), let you reuse the same code in both wxWidgets and other programs (by
just typedefing wxString as std::string when used outside wxWidgets) and by
staying compatible with future versions of wxWidgets which will probably start
using std::string sooner or later too.

In the situations where there is no corresponding std::string function, please
try to use the new wxString methods and not the old wxWidgets 1.xx variants
which are deprecated and may disappear in future versions.


@section overview_string_advice Advice About Using wxString

Probably the main trap with using this class is the implicit conversion
operator to <tt>const char*</tt>. It is advised that you use wxString::c_str()
instead to clearly indicate when the conversion is done. Specifically, the
danger of this implicit conversion may be seen in the following code fragment:

@code
// this function converts the input string to uppercase,
// output it to the screen and returns the result
const char *SayHELLO(const wxString& input)
{
    wxString output = input.Upper();
    printf("Hello, %s!\n", output);
    return output;
}
@endcode

There are two nasty bugs in these three lines. The first is in the call to the
@c printf() function. Although the implicit conversion to C strings is applied
automatically by the compiler in the case of

@code
puts(output);
@endcode

because the argument of @c puts() is known to be of the type
<tt>const char*</tt>, this is @b not done for @c printf() which is a function
with variable number of arguments (and whose arguments are of unknown types).
So this call may do any number of things (including displaying the correct
string on screen), although the most likely result is a program crash. The
solution is to use wxString::c_str(). Just replace this line with this:

@code
printf("Hello, %s!\n", output.c_str());
@endcode

The second bug is that returning @c output doesn't work. The implicit cast is
used again, so the code compiles, but as it returns a pointer to a buffer
belonging to a local variable which is deleted as soon as the function exits,
its contents are completely arbitrary. The solution to this problem is also
easy, just make the function return wxString instead of a C string.

This leads us to the following general advice: all functions taking string
arguments should take <tt>const wxString</tt> (this makes assignment to the
strings inside the function faster because of
@ref overview_string_refcount "reference counting") and all functions returning
strings should return wxString - this makes it safe to return local variables.


@section overview_string_related String Related Functions and Classes

As most programs use character strings, the standard C library provides quite
a few functions to work with them. Unfortunately, some of them have rather
counter-intuitive behaviour (like @c strncpy() which doesn't always terminate
the resulting string with a @NULL) and are in general not very safe (passing
@NULL to them will probably lead to program crash). Moreover, some very useful
functions are not standard at all. This is why in addition to all wxString
functions, there are also a few global string functions which try to correct
these problems: wxIsEmpty() verifies whether the string is empty (returning
@true for @NULL pointers), wxStrlen() also handles @NULL correctly and returns
0 for them and wxStricmp() is just a platform-independent version of
case-insensitive string comparison function known either as @c stricmp() or
@c strcasecmp() on different platforms.

The <tt>@<wx/string.h@></tt> header also defines wxSnprintf and wxVsnprintf
functions which should be used instead of the inherently dangerous standard
@c sprintf() and which use @c snprintf() instead which does buffer size checks
whenever possible. Of course, you may also use wxString::Printf which is also
safe.

There is another class which might be useful when working with wxString:
wxStringTokenizer. It is helpful when a string must be broken into tokens and
replaces the standard C library @c strtok() function.

And the very last string-related class is wxArrayString: it is just a version
of the "template" dynamic array class which is specialized to work with
strings. Please note that this class is specially optimized (using its
knowledge of the internal structure of wxString) for storing strings and so it
is vastly better from a performance point of view than a wxObjectArray of
wxStrings.


@section overview_string_refcount Reference Counting and Why You Shouldn't Care

All considerations for wxObject-derived
@ref overview_refcount "reference counted" objects are valid also for wxString,
even if it does not derive from wxObject.

Probably the unique case when you might want to think about reference counting
is when a string character is taken from a string which is not a constant (or
a constant reference). In this case, due to C++ rules, the "read-only"
@c operator[] (which is the same as wxString::GetChar()) cannot be chosen and
the "read/write" @c operator[] (the same as wxString::GetWritableChar()) is
used instead. As the call to this operator may modify the string, its data is
unshared (COW is done) and so if the string was really shared there is some
performance loss (both in terms of speed and memory consumption). In the rare
cases when this may be important, you might prefer using wxString::GetChar()
instead of the array subscript operator for this reasons. Please note that
wxString::at() method has the same problem as the subscript operator in this
situation and so using it is not really better. Also note that if all string
arguments to your functions are passed as <tt>const wxString</tt> (see the
@ref overview_string_advice section) this situation will almost never arise
because for constant references the correct operator is called automatically.


@section overview_string_tuning Tuning wxString for Your Application

@note This section is strictly about performance issues and is absolutely not
necessary to read for using wxString class. Please skip it unless you feel
familiar with profilers and relative tools. If you do read it, please also
read the preceding section about
@ref overview_string_refcount "reference counting".

For the performance reasons wxString doesn't allocate exactly the amount of
memory needed for each string. Instead, it adds a small amount of space to each
allocated block which allows it to not reallocate memory (a relatively
expensive operation) too often as when, for example, a string is constructed by
subsequently adding one character at a time to it, as for example in:

@code
// delete all vowels from the string
wxString DeleteAllVowels(const wxString& original)
{
    wxString result;

    size_t len = original.length();
    for ( size_t n = 0; n < len; n++ )
    {
        if ( strchr("aeuio", tolower(original[n])) == NULL )
            result += original[n];
    }

    return result;
}
@endcode

This is quite a common situation and not allocating extra memory at all would
lead to very bad performance in this case because there would be as many memory
(re)allocations as there are consonants in the original string. Allocating too
much extra memory would help to improve the speed in this situation, but due to
a great number of wxString objects typically used in a program would also
increase the memory consumption too much.

The very best solution in precisely this case would be to use wxString::Alloc()
function to preallocate, for example, len bytes from the beginning - this will
lead to exactly one memory allocation being performed (because the result is at
most as long as the original string).

However, using wxString::Alloc() is tedious and so wxString tries to do its
best. The default algorithm assumes that memory allocation is done in
granularity of at least 16 bytes (which is the case on almost all of
wide-spread platforms) and so nothing is lost if the amount of memory to
allocate is rounded up to the next multiple of 16. Like this, no memory is lost
and 15 iterations from 16 in the example above won't allocate memory but use
the already allocated pool.

The default approach is quite conservative. Allocating more memory may bring
important performance benefits for programs using (relatively) few very long
strings. The amount of memory allocated is configured by the setting of
@c EXTRA_ALLOC in the file string.cpp during compilation (be sure to understand
why its default value is what it is before modifying it!). You may try setting
it to greater amount (say twice nLen) or to 0 (to see performance degradation
which will follow) and analyse the impact of it on your program. If you do it,
you will probably find it helpful to also define @c WXSTRING_STATISTICS symbol
which tells the wxString class to collect performance statistics and to show
them on stderr on program termination. This will show you the average length of
strings your program manipulates, their average initial length and also the
percent of times when memory wasn't reallocated when string concatenation was
done but the already preallocated memory was used (this value should be about
98% for the default allocation policy, if it is less than 90% you should
really consider fine tuning wxString for your application).

It goes without saying that a profiler should be used to measure the precise
difference the change to @c EXTRA_ALLOC makes to your program.

*/

