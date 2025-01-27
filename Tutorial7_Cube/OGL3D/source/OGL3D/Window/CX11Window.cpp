/*MIT License

C++ OpenGL 3D Game Tutorial Series (https://github.com/PardCode/OpenGL-3D-Game-Tutorial-Series)

Copyright (c) 2021-2024, PardCode

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include <OGL3D/Window/OWindow.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <glad/glad.h>
#include <glad/glad_glx.h>
#include <assert.h>
#include <stdexcept>
#include "../Graphics/X11/CX11Globals.h"

Atom atomWmDeleteWindow;


void WndProc(OWindow* window, XEvent xev)
{
    if (xev.type == ClientMessage)
    {
        if (xev.xclient.data.l[0] == (long)atomWmDeleteWindow)
        {
            //Send Quit Message to MainLoop, Similar to PostQuitMessage(0)
            XClientMessageEvent quitEvent = {};
            quitEvent.type = ClientMessage;
            quitEvent.window = GlobalWindowRoot;
            quitEvent.format = 32;
            XSendEvent(GlobalDisplay, GlobalWindowRoot, 0, 0, (XEvent*)&quitEvent);
            XFlush(GlobalDisplay);
        }
    }
}


OWindow::OWindow()
{
    //Creating the Window with X11 API---------------------------
    //---------------------------------------------------------
    XSetWindowAttributes attributes;
    attributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask;
    attributes.colormap = GlobalColorMap;

    Window window =
        XCreateWindow(GlobalDisplay, GlobalWindowRoot, 0, 0, 1024, 768, 0,
                      DefaultDepth(GlobalDisplay, GlobalScreenId), InputOutput, GlobalVisual,
                      CWColormap | CWEventMask, &attributes);

    m_handle = (void*)window;
    XMapWindow(GlobalDisplay, window);
    XStoreName(GlobalDisplay, window, "PardCode | OpenGL 3D Game");

    assert (window);
    //---------------------------------------------------------



    //Creating OpenGL 4.6 RenderContext for this specific Window---------------
    //------------------------------------------------------
    GLint visual_attributes[] = {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DOUBLEBUFFER, 1,
        None
    };

    int num_fbc = 0;
    GLXFBConfig *fbc = glXChooseFBConfig(GlobalDisplay, GlobalScreenId, visual_attributes, &num_fbc);

    GLint context_attributes[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 6,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    GLXContext context =
        glXCreateContextAttribsARB(GlobalDisplay, fbc[0], NULL, 1, context_attributes);
    assert (context);

    glXMakeCurrent(GlobalDisplay, window, context);
    //------------------------------------------------------



    m_context = context;
    atomWmDeleteWindow = XInternAtom(GlobalDisplay, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(GlobalDisplay, window, &atomWmDeleteWindow, 1);


    //Set the Window not resizable
    XSizeHints hints={};
    hints.flags = PMinSize | PMaxSize;
    hints.min_width = hints.max_width = 1024;
    hints.min_height = hints.max_height = 768;

    XSetWMNormalHints(GlobalDisplay,window,&hints);
}


void X11CheckEvent(OWindow*window,void* event)
{
    XEvent xev =*(XEvent*)event;

    //Check the event
    if (xev.xclient.window == *(Window*)window)
       WndProc(window,xev);
}


OWindow::~OWindow()
{
    glXDestroyContext(GlobalDisplay,(GLXContext)m_context);
    XDestroyWindow(GlobalDisplay,(Window)m_handle );
}

ORect OWindow::getInnerSize()
{
    XWindowAttributes gwa;
    XGetWindowAttributes(GlobalDisplay, (Window)m_handle, &gwa);
    return ORect(gwa.width, gwa.height);
}

void OWindow::makeCurrentContext()
{
    glXMakeCurrent(GlobalDisplay,(Window)m_handle,(GLXContext)m_context);
}

void OWindow::present(bool vsync)
{
   glXSwapIntervalEXT(GlobalDisplay,(Window)m_handle,vsync);
   glXSwapBuffers(GlobalDisplay,(Window)m_handle);
}
