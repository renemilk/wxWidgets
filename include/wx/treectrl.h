#ifndef __TREECTRLH_BASE__
#define __TREECTRLH_BASE__

#if defined(__WXMSW__)
#include "wx/msw/treectrl.h"
#elif defined(__WXMOTIF__)
#include "wx/generic/treectrl.h"
#elif defined(__WXGTK__)
#include "wx/gtk/treectrl.h"
#endif

#endif
    // __TREECTRLH_BASE__
