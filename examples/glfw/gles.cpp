#include <emscripten.h>
#include <emscripten/html5.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>
#include <GLES3/gl32.h>
#define __gl2_h_
#include <GLES2/gl2ext.h>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <cstdarg>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <tuple>

#include "GlobeViewer.h"

std::tuple<EGLSurface*, int, int> setupWindow( const std::string& title );

std::unique_ptr<gv::GlobeViewer> globeViewer;
bool drag = false;
bool shift = false;
bool fullscreen = false;
bool firstClick = true;
double lastX;
double lastY;
int lastPosX;
int lastPosY;
int lastWidth;
int lastHeight;
gv::TileServer tileServer = gv::TileServer::OSM;
bool drawWireFrameView = true;
bool drawMapTilesView = true;

using namespace std;
using namespace std::chrono;

double Ttime,Dtime;
EGLDisplay display;
EGLSurface surface;
EGLContext contextegl;
GLsizei nsources,i;
GLsizei s4=4;
EGLint config_size,major,minor;
EGLConfig eglconfig=NULL;
EmscriptenWebGLContextAttributes attr;
EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx;
EGLint v0=0,v1=1,v2=2,v3=3,v4=4,v6=6,v8=8,v32=32,a,b;
struct timespec rem;
struct timespec req={0,8500000};
GLfloat F=1.0f;
GLfloat F0=0.0f;
GLfloat Fm1=-1.0f;
GLfloat fps;
GLfloat x;
GLfloat y;
GLfloat mouseX;
GLfloat mouseY;
GLfloat mouseLPressed=0.0f;
static EMSCRIPTEN_RESULT ret;

static EM_BOOL mouse_callback(int eventType,const EmscriptenMouseEvent *e,void *userData){
if(e->screenX!=0&&e->screenY!=0&&e->clientX!=0&&e->clientY!=0&&e->targetX!=0&&e->targetY!=0){
if(eventType==EMSCRIPTEN_EVENT_MOUSEDOWN&&e->buttons!=0){
mouseLPressed=1.0f;
}
if(eventType==EMSCRIPTEN_EVENT_MOUSEUP){
mouseLPressed=0.0f;
}
if(eventType==EMSCRIPTEN_EVENT_MOUSEMOVE&&(e->movementX!=0||e->movementY!=0)){
x=e->clientX;
y=e->clientY;
}}
return 0;
}
GLsizei S;

void renderFrame(){
eglSwapBuffers(display,surface);
glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
ret=emscripten_set_click_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,0,1,mouse_callback);
ret=emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,0,1,mouse_callback);
ret=emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,0,1,mouse_callback);
ret=emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,0,1,mouse_callback);
mouseX=(float)x/S;
mouseY=(float)y/S;
if(mouseLPressed==1.0f){
// EM_ASM({console.log("S = "+$0);},S);
// EM_ASM({console.log("x = "+$0);},x);
EM_ASM({console.log("mouseX = "+$0);},mouseX);
const float cMouseX=mouseX;
const float cMouseY=mouseY;
EM_ASM({console.log("cMouseX = "+$0);},cMouseX);
}
        globeViewer->render();
}

void strt(){
std::tuple<EGLSurface*, int, int> tup = setupWindow( "GlobeViewer" );
auto window = std::get<0>( tup );
S=EM_ASM_INT({return parseInt(document.getElementById('pmhig').innerHTML,10);});
eglBindAPI(EGL_OPENGL_ES_API);
const EGLint attribut_list[]={ 
EGL_GL_COLORSPACE_KHR,EGL_GL_COLORSPACE_SRGB_KHR,
EGL_NONE};
const EGLint anEglCtxAttribs2[]={
EGL_CONTEXT_CLIENT_VERSION,3,
EGL_CONTEXT_MINOR_VERSION_KHR,0,
// EGL_COLOR_COMPONENT_TYPE_EXT,EGL_COLOR_COMPONENT_TYPE_FLOAT_EXT,
EGL_CONTEXT_PRIORITY_LEVEL_IMG,EGL_CONTEXT_PRIORITY_REALTIME_NV,
EGL_CONTEXT_FLAGS_KHR,EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR,
EGL_CONTEXT_FLAGS_KHR,EGL_CONTEXT_OPENGL_ROBUST_ACCESS_BIT_KHR,
EGL_NONE};
const EGLint attribute_list[]={
// EGL_COLOR_COMPONENT_TYPE_EXT,EGL_COLOR_COMPONENT_TYPE_FLOAT_EXT,
EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR,EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR,
EGL_RENDERABLE_TYPE,EGL_OPENGL_ES3_BIT,
EGL_CONTEXT_OPENGL_ROBUST_ACCESS_EXT,EGL_TRUE,
EGL_DEPTH_ENCODING_NV,EGL_DEPTH_ENCODING_NONLINEAR_NV,
EGL_RENDER_BUFFER,EGL_QUADRUPLE_BUFFER_NV,
EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE,EGL_TRUE,
EGL_RED_SIZE,8,
EGL_GREEN_SIZE,8,
EGL_BLUE_SIZE,8,
EGL_ALPHA_SIZE,8,
EGL_DEPTH_SIZE,24,
EGL_STENCIL_SIZE,8,
EGL_BUFFER_SIZE,32,
EGL_NONE
};
emscripten_webgl_init_context_attributes(&attr);
attr.alpha=EM_TRUE;
attr.stencil=EM_TRUE;
attr.depth=EM_TRUE;
attr.antialias=EM_FALSE;
attr.premultipliedAlpha=EM_FALSE;
attr.preserveDrawingBuffer=EM_FALSE;
attr.enableExtensionsByDefault=EM_FALSE;
attr.renderViaOffscreenBackBuffer=EM_FALSE;
attr.powerPreference=EM_WEBGL_POWER_PREFERENCE_DEFAULT;
attr.failIfMajorPerformanceCaveat=EM_FALSE;
attr.majorVersion=2;
attr.minorVersion=0;
ctx=emscripten_webgl_create_context("#canvas",&attr);
display=eglGetDisplay(EGL_DEFAULT_DISPLAY);
eglInitialize(display,&v3,&v0);
eglChooseConfig(display,attribute_list,&eglconfig,1,&config_size);
contextegl=eglCreateContext(display,eglconfig,EGL_NO_CONTEXT,anEglCtxAttribs2);
surface=eglCreateWindowSurface(display,eglconfig,0,attribut_list);
eglMakeCurrent(display,surface,surface,contextegl);
emscripten_webgl_make_context_current(ctx);
glEnable(GL_CULL_FACE);
glCullFace(GL_FRONT);
glEnable(GL_DITHER);
glEnable(GL_BLEND);
glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_LESS);
glClearColor(F0,F0,F0,F);
glViewport(0,0,S,S);
glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
emscripten_set_main_loop((void(*)())renderFrame,0,0);
}

int main(){
strt();
return 1;
}
