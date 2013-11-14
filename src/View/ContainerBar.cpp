/*
 Copyright (C) 2010-2013 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ContainerBar.h"

#include <wx/dcclient.h>

namespace TrenchBroom {
    namespace View {
        ContainerBar::ContainerBar(wxWindow* parent) :
        wxPanel(parent) {
#ifdef __APPLE__
            SetBackgroundStyle(wxBG_STYLE_PAINT);
            Bind(wxEVT_PAINT, &ContainerBar::OnPaint, this);
#endif
        }
        
        void ContainerBar::OnPaint(wxPaintEvent& event) {
            paintMac(event);
        }

        void ContainerBar::paintMac(wxPaintEvent& event) {
            wxPaintDC dc(this);
            wxRect rect = GetClientRect();
            rect.height -= 1;
            dc.GradientFillLinear(rect, wxColour(211, 211, 211), wxColour(174, 174, 174), wxDOWN);
            dc.SetPen(wxPen(wxColour(67, 67, 67)));
            dc.DrawLine(0, rect.height, rect.width, rect.height);
            dc.DrawLine(rect.width - 1, 0, rect.width - 1, rect.height);
        }
        
        void ContainerBar::paintOther(wxPaintEvent& event) {
            event.Skip();
        }
    }
}