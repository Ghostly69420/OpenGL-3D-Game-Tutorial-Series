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
#import <Cocoa/Cocoa.h>
#include <glad/glad.h>
#include <assert.h>
#include <stdexcept>





//Cocoa Window Delegate - It allows to catch the events generated by a specific Window.
//A delegate is assigned to a specific Window during the Window construction.

@interface View : NSOpenGLView <NSWindowDelegate> {
      @public
      OWindow* window;
}
@end

@implementation View
- (id) initWithFrame: (NSRect) frame {
    return self;
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

-(void)windowWillClose:(NSNotification *)notification {
    [NSApp terminate:self];
}

- (void) dealloc {
    [super dealloc];
}
@end






OWindow::OWindow()
{
    NSUInteger windowStyle = NSTitledWindowMask  | NSClosableWindowMask ;

    NSRect screenRect = [[NSScreen mainScreen] frame];
    NSRect viewRect = NSMakeRect(0, 0, 1024, 768);
    NSRect windowRect = NSMakeRect(NSMidX(screenRect) - NSMidX(viewRect),
                                   NSMidY(screenRect) - NSMidY(viewRect),
                                   viewRect.size.width,
                                   viewRect.size.height);

    NSWindow * window = [[NSWindow alloc] initWithContentRect:windowRect
                                                              styleMask:windowStyle
                                                              backing:NSBackingStoreBuffered
                                                              defer:NO];
    assert(window);
    NSOpenGLPixelFormatAttribute windowedAttrs[] =
    {
        NSOpenGLPFAMultisample,
        NSOpenGLPFASampleBuffers, 0,
        NSOpenGLPFASamples, 0,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAColorSize, 32,
        NSOpenGLPFADepthSize, 24,
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
        0
    };

    // Try to choose a supported pixel format
    NSOpenGLPixelFormat* pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:windowedAttrs];
    assert(pf);
    NSOpenGLContext*  _context = [[NSOpenGLContext alloc] initWithFormat: pf shareContext: NULL];
    m_context = _context;
    assert(_context);

    makeCurrentContext();

    View* view = [[View alloc] initWithFrame:windowRect];
    view->window = this;
    [view setAutoresizingMask:  (NSViewWidthSizable | NSViewHeightSizable) ];
    [view setOpenGLContext:_context];
    [view setWantsBestResolutionOpenGLSurface: NO];


    [window setAcceptsMouseMovedEvents:YES];
    [window setContentView:view];
    [window setDelegate:view];
    [window setTitle:@"PardCode - OpenGL 3D Game"];
    [window orderFrontRegardless];

    m_handle = window;

    [pf release];
}



OWindow::~OWindow()
{
    NSOpenGLContext*  _context = (NSOpenGLContext*)m_context;
    [_context release];
    NSWindow* win= (NSWindow*) m_handle;
    [win release];
}


void OWindow::makeCurrentContext()
{
    NSOpenGLContext*  _context = (NSOpenGLContext*)m_context;
    [_context makeCurrentContext];
}

void OWindow::present(bool vsync)
{
    NSOpenGLContext*  _context = (NSOpenGLContext*)m_context;
    [_context flushBuffer];
}

