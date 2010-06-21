/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/window_qt.h
// Author:      Javier Torres
// Id:          $Id$
// Copyright:   (c) Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_WIDGET_QT_H_
#define _WX_QT_WIDGET_QT_H_

#include "wx/window.h"
#include <QtGui/QWidget>
#include "wx/qt/winevent_qt.h"

class WXDLLIMPEXP_CORE wxQtWidget : public WindowEventForwarder< QWidget >
{
    Q_OBJECT

    public:
        wxQtWidget( wxWindow *window, QWidget *parent );
        
    protected:
        virtual wxWindow *GetEventReceiver();

    private:
        wxWindow *m_wxWindow;

};

#endif