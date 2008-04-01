/////////////////////////////////////////////////////////////////////////////
// Name:        filefn.h
// Purpose:     interface of wxPathList and file functions
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxPathList
    @wxheader{filefn.h}

    The path list is a convenient way of storing a number of directories, and
    when presented with a filename without a directory, searching for an
    existing file in those directories.

    Be sure to look also at wxStandardPaths if you only want to search files in
    some standard paths.

    @library{wxbase}
    @category{file}

    @see wxArrayString, wxStandardPaths, wxFileName
*/
class wxPathList : public wxArrayString
{
public:
    wxPathList();

    /**
        Constructs the object calling the Add() function.
    */
    wxPathList(const wxArrayString& arr);

    /**
        Adds the given directory to the path list, if the @a path is not already in the list.
        If the path cannot be normalized for some reason, it returns @false.

        The @a path is always considered to be a directory but no existence checks will be
        done on it (because if it doesn't exist, it could be created later and thus result a
        valid path when FindValidPath() is called).

        @note if the given path is relative, it won't be made absolute before adding it
              (this is why FindValidPath() may return relative paths).
    */
    bool Add(const wxString& path);

    /**
        Adds all elements of the given array as paths.
    */
    void Add(const wxArrayString& arr);

    /**
        Finds the value of the given environment variable, and adds all paths
        to the path list.

        Useful for finding files in the @c PATH variable, for example.
    */
    void AddEnvList(const wxString& env_variable);

    /**
        Given a full filename (with path), calls Add() with the path of the file.
    */
    bool EnsureFileAccessible(const wxString& filename);

    /**
        Like FindValidPath() but this function always returns an absolute path
        (eventually prepending the current working directory to the value returned
        wxPathList::FindValidPath()) or an empty string.
    */
    wxString FindAbsoluteValidPath(const wxString& file) const;

    /**
        Searches the given file in all paths stored in this class.

        The first path which concatenated to the given string points to an existing
        file (see wxFileExists()) is returned.

        If the file wasn't found in any of the stored paths, an empty string is returned.

        The given string must be a file name, eventually with a path prefix (if the path
        prefix is absolute, only its name will be searched); i.e. it must not end with
        a directory separator (see wxFileName::GetPathSeparator) otherwise an assertion
        will fail.

        The returned path may be relative to the current working directory.

        Note in fact that wxPathList can be used to store both relative and absolute
        paths so that if you added relative paths, then the current working directory
        (see wxGetCwd() and wxSetWorkingDirectory()) may affect the value returned
        by this function!
    */
    wxString FindValidPath(const wxString& file) const;
};



// ============================================================================
// Global functions/macros
// ============================================================================

/** @ingroup group_funcmacro_file */
//@{

/**
    Under Unix this macro changes the current process umask to the given value,
    unless it is equal to -1 in which case nothing is done, and restores it to
    the original value on scope exit. It works by declaring a variable which
    sets umask to @a mask in its constructor and restores it in its destructor.
    Under other platforms this macro expands to nothing.

    @header{wx/filefn.h}
*/
#define wxCHANGE_UMASK(int mask)

/**
    This function returns the total number of bytes and number of free bytes on
    the disk containing the directory @a path (it should exist). Both @a total
    and @a free parameters may be @NULL if the corresponding information is not
    needed.

    @wxsince{2.3.2}

    @note The generic Unix implementation depends on the system having the
          @c statfs() or @c statvfs() function.

    @returns @true on success, @false if an error occurred (for example, the
             directory doesn’t exist).

    @header{wx/filefn.h}
*/
bool wxGetDiskSpace(const wxString& path,
                    wxLongLong total = NULL,
                    wxLongLong free = NULL);

/**
    Returns the Windows directory under Windows; other platforms return an
    empty string.

    @header{wx/filefn.h}
*/
wxString wxGetOSDirectory();

/**
    Parses the @a wildCard, returning the number of filters. Returns 0 if none
    or if there's a problem.

    The arrays will contain an equal number of items found before the error. On
    platforms where native dialogs handle only one filter per entry, entries in
    arrays are automatically adjusted. @a wildCard is in the form:

    @code
    "All files (*)|*|Image Files (*.jpeg *.png)|*.jpg;*.png"
    @endcode

    @header{wx/filefn.h}
*/
int wxParseCommonDialogsFilter(const wxString& wildCard,
                               wxArrayString& descriptions,
                               wxArrayString& filters);

/**
    Converts a DOS to a Unix filename by replacing backslashes with forward
    slashes.

    @header{wx/filefn.h}
*/
void wxDos2UnixFilename(wxChar* s);

/**
    @warning This function is deprecated, use wxFileName instead.

    Converts a Unix to a DOS filename by replacing forward slashes with
    backslashes.

    @header{wx/filefn.h}
*/
void wxUnix2DosFilename(wxChar* s);

/**
    Returns @true if @a dirname exists and is a directory.

    @header{wx/filefn.h}
*/
bool wxDirExists(const wxString& dirname);

/**
    @warning This function is obsolete, please use wxFileName::SplitPath()
             instead.

    This function splits a full file name into components: the path (including
    possible disk/drive specification under Windows), the base name, and the
    extension. Any of the output parameters (@a path, @a name or @a ext) may be
    @NULL if you are not interested in the value of a particular component.

    wxSplitPath() will correctly handle filenames with both DOS and Unix path
    separators under Windows, however it will not consider backslashes as path
    separators under Unix (where backslash is a valid character in a filename).

    On entry, @a fullname should be non-@NULL (it may be empty though).

    On return, @a path contains the file path (without the trailing separator),
    @a name contains the file name and @c ext contains the file extension
    without leading dot. All three of them may be empty if the corresponding
    component is. The old contents of the strings pointed to by these
    parameters will be overwritten in any case (if the pointers are not @NULL).

    @header{wx/filefn.h}
*/
void wxSplitPath(const wxString& fullname,
                  wxString* path,
                  wxString* name,
                  wxString* ext);

/**
    Returns time of last modification of given file.

    The function returns <tt>(time_t)-1</tt> if an error occurred (e.g. file
    not found).

    @header{wx/filefn.h}
*/
time_t wxFileModificationTime(const wxString& filename);

/**
    Renames @a file1 to @e file2, returning @true if successful.

    If @a overwrite parameter is @true (default), the destination file is
    overwritten if it exists, but if @a overwrite is @false, the functions
    fails in this case.

    @header{wx/filefn.h}
*/
bool wxRenameFile(const wxString& file1,
                   const wxString& file2,
                   bool overwrite = true);

/**
    Copies @a file1 to @e file2, returning @true if successful. If @a overwrite
    parameter is @true (default), the destination file is overwritten if it
    exists, but if @a overwrite is @false, the functions fails in this case.

    This function supports resources forks under Mac OS.

    @header{wx/filefn.h}
*/
bool wxCopyFile(const wxString& file1,
                 const wxString& file2,
                 bool overwrite = true);

/**
    Returns @true if the file exists and is a plain file.

    @header{wx/filefn.h}
*/
bool wxFileExists(const wxString& filename);

/**
    Returns @true if the @a pattern matches the @e text; if @a dot_special is
    @true, filenames beginning with a dot are not matched with wildcard
    characters.

    @see wxIsWild()

    @header{wx/filefn.h}
*/
bool wxMatchWild(const wxString& pattern,
                  const wxString& text,
                  bool dot_special);

/**
    @warning This function is deprecated, use wxGetCwd() instead.

    Copies the current working directory into the buffer if supplied, or copies
    the working directory into new storage (which you must delete yourself) if
    the buffer is @NULL.

    @a sz is the size of the buffer if supplied.

    @header{wx/filefn.h}
*/
wxString wxGetWorkingDirectory(char* buf = NULL, int sz = 1000);

/**
    Returns the directory part of the filename.

    @header{wx/filefn.h}
*/
wxString wxPathOnly(const wxString& path);

/**
    Returns @true if the pattern contains wildcards.

    @see wxMatchWild()

    @header{wx/filefn.h}
*/
bool wxIsWild(const wxString& pattern);

/**
    Returns @true if the argument is an absolute filename, i.e. with a slash
    or drive name at the beginning.

    @header{wx/filefn.h}
*/
bool wxIsAbsolutePath(const wxString& filename);

/**
    Returns a string containing the current (or working) directory.

    @header{wx/filefn.h}
*/
wxString wxGetCwd();

/**
    Sets the current working directory, returning @true if the operation
    succeeded. Under MS Windows, the current drive is also changed if @a dir
    contains a drive specification.

    @header{wx/filefn.h}
*/
bool wxSetWorkingDirectory(const wxString& dir);

/**
    Concatenates @a file1 and @a file2 to @e file3, returning @true if
    successful.

    @header{wx/filefn.h}
*/
bool wxConcatFiles(const wxString& file1,
                    const wxString& file2,
                    const wxString& file3);

/**
    Removes @e file, returning @true if successful.

    @header{wx/filefn.h}
*/
bool wxRemoveFile(const wxString& file);

/**
    Makes the directory @a dir, returning @true if successful.

    @a perm is the access mask for the directory for the systems on which it is
    supported (Unix) and doesn't have any effect on the other ones.

    @header{wx/filefn.h}
*/
bool wxMkdir(const wxString& dir, int perm = 0777);

/**
    Removes the directory @a dir, returning @true if successful. Does not work
    under VMS.

    The @a flags parameter is reserved for future use.

    @note There is also a wxRmDir() function which simply wraps the standard
          POSIX @c rmdir() function and so return an integer error code instead
          of a boolean value (but otherwise is currently identical to
          wxRmdir()), don't confuse these two functions.

    @header{wx/filefn.h}
*/
bool wxRmdir(const wxString& dir, int flags = 0);

/**
    Returns the next file that matches the path passed to wxFindFirstFile().

    See wxFindFirstFile() for an example.

    @header{wx/filefn.h}
*/
wxString wxFindNextFile();

/**
    This function does directory searching; returns the first file that matches
    the path @a spec, or the empty string. Use wxFindNextFile() to get the next
    matching file. Neither will report the current directory "." or the parent
    directory "..".

    @warning As of 2.5.2, these functions are not thread-safe! (they use static
             variables). You probably want to use wxDir::GetFirst() or
             wxDirTraverser instead.

    @a spec may contain wildcards.

    @a flags may be wxDIR for restricting the query to directories, wxFILE for
    files or zero for either.

    For example:

    @code
    wxString f = wxFindFirstFile("/home/project/*.*");
    while ( !f.empty() )
    {
        ...
        f = wxFindNextFile();
    }
    @endcode

    @header{wx/filefn.h}
*/
wxString wxFindFirstFile(const wxString& spec, int flags = 0);

/**
    File kind enumerations returned from wxGetFileKind().

    @header{wx/filefn.h}
*/
enum wxFileKind
{
    wxFILE_KIND_UNKNOWN,  ///< Unknown file kind, or unable to determine
    wxFILE_KIND_DISK,     ///< A file supporting seeking to arbitrary offsets
    wxFILE_KIND_TERMINAL, ///< A tty
    wxFILE_KIND_PIPE      ///< A pipe
};

//@}

/** @ingroup group_funcmacro_file */
//@{
/**
    Returns the type of an open file. Possible return values are enumerations
    of ::wxFileKind.

    @header{wx/filefn.h}
*/
wxFileKind wxGetFileKind(int fd);
wxFileKind wxGetFileKind(FILE* fp);
//@}

/** @ingroup group_funcmacro_file */
//@{
/**
    @warning This function is obsolete, please use wxFileName::SplitPath()
             instead.

    Returns the filename for a full path. The second form returns a pointer to
    temporary storage that should not be deallocated.

    @header{wx/filefn.h}
*/
wxString wxFileNameFromPath(const wxString& path);
char* wxFileNameFromPath(char* path);
//@}

/** @ingroup group_funcmacro_file */
//@{
/**
    @warning This function is obsolete, please use
             wxFileName::CreateTempFileName() instead.

    @header{wx/filefn.h}
*/
char* wxGetTempFileName(const wxString& prefix, char* buf = NULL);
bool wxGetTempFileName(const wxString& prefix, wxString& buf);
//@}

