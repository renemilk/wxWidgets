/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/graphics.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/graphics.h"

wxGraphicsRenderer* wxGraphicsRenderer::GetDefaultRenderer()
{
    return NULL;
}
