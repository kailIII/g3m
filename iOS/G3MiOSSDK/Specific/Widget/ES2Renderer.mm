//
//  ES2Renderer.m
//  Prueba Opengl iPad
//
//  Created by Agustín Trujillo Pino on 12/01/11.
//  Copyright 2011 Universidad de Las Palmas. All rights reserved.
//

#import "ES2Renderer.h"

#include "G3MWidget.hpp"
#include "GL.hpp"

GLuint fboHandle; 
GLuint fboTex;
GLuint defaultFramebuffer;
GLint backingWidth;
GLint backingHeight;



// uniform index
enum {
  UNIFORM_TRANSLATE,
  NUM_UNIFORMS
};
GLint uniforms[NUM_UNIFORMS];

// attribute index
enum {
  ATTRIB_VERTEX,
  ATTRIB_COLOR,
  NUM_ATTRIBUTES
};

@interface ES2Renderer (PrivateMethods)
- (BOOL)loadShaders;

- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file;

- (BOOL)linkProgram:(GLuint)prog;

- (BOOL)validateProgram:(GLuint)prog;
@end

@implementation ES2Renderer

// Create an OpenGL ES 2.0 context
- (id)init {
  self = [super init];
  
  if (self) {
    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    if (!context || ![EAGLContext setCurrentContext:context] || ![self loadShaders]) {
      return nil;
    }
    
    // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
    glGenFramebuffers(1, &defaultFramebuffer);
    glGenRenderbuffers(1, &colorRenderbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
    
    // create depth_buffer
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    
    // create buffer for render to texture
    {
      fbo_width = 256;
      fbo_height = 256;
      glGenFramebuffers(1, &fboHandle);
      glGenTextures(1, &fboTex);      
      glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
      glBindTexture(GL_TEXTURE_2D, fboTex);
      glTexImage2D( GL_TEXTURE_2D,
                   0,
                   GL_RGB,
                   fbo_width, fbo_height,
                   0,
                   GL_RGB,
                   GL_UNSIGNED_SHORT_5_6_5,
                   NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER_APPLE, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex, 0);
      
      // FBO status check
      GLenum status;
      status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      switch(status) {
        case GL_FRAMEBUFFER_COMPLETE:
          NSLog(@"fbo complete");
          break;
          
        case GL_FRAMEBUFFER_UNSUPPORTED:
          NSLog(@"fbo unsupported");
          break;
          
        default:
          /* programming error; will fail on all hardware */
          NSLog(@"Framebuffer Error");
          break;
      }
      
      glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);    
    }
  
  }
  
  return self;
}


- (int)render: (void*) widgetV
{
  if (widgetV == NULL) {
    return 0;
  }
  
  G3MWidget* widget = (G3MWidget*) widgetV;
  
  // This application only creates a single context which is already set current at this point.
  // This call is redundant, but needed if dealing with multiple contexts.
  [EAGLContext setCurrentContext:context];
  
  // This application only creates a single default framebuffer which is already bound at this point.
  // This call is redundant, but needed if dealing with multiple framebuffers.
  glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
  glViewport(0, 0, backingWidth, backingHeight);
  
  // Use shader program
  widget->getGL()->useProgram(program);
  
  int timeToRedraw = widget->render();
  
  // This application only creates a single color renderbuffer which is already bound at this point.
  // This call is redundant, but needed if dealing with multiple renderbuffers.
  glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
  [context presentRenderbuffer:GL_RENDERBUFFER];
  
  return timeToRedraw;
}

- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file {
  GLint status;
  const GLchar *source;
  
  source = (GLchar *) [[NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil] UTF8String];
  //NSLog(@"%s\n", source);
  if (!source) {
    NSLog(@"Failed to load vertex shader");
    return FALSE;
  }
  
  *shader = glCreateShader(type);
  glShaderSource(*shader, 1, &source, NULL);
  glCompileShader(*shader);
  
#if defined(DEBUG)
  GLint logLength;
  glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0) {
    GLchar *log = (GLchar *) malloc(logLength);
    glGetShaderInfoLog(*shader, logLength, &logLength, log);
    NSLog(@"Shader compile log:\n%s", log);
    free(log);
  }
#endif
  
  glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
  if (status == 0) {
    glDeleteShader(*shader);
    return FALSE;
  }
  
  return TRUE;
}

- (BOOL)linkProgram:(GLuint)prog {
  GLint status;
  
  glLinkProgram(prog);
  
#if defined(DEBUG)
  GLint logLength;
  glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0) {
    GLchar *log = (GLchar *) malloc(logLength);
    glGetProgramInfoLog(prog, logLength, &logLength, log);
    NSLog(@"Program link log:\n%s", log);
    free(log);
  }
#endif
  
  glGetProgramiv(prog, GL_LINK_STATUS, &status);
  if (status == 0)
    return FALSE;
  
  return TRUE;
}

- (BOOL)validateProgram:(GLuint)prog {
  GLint logLength, status;
  
  glValidateProgram(prog);
  glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0) {
    GLchar *log = (GLchar *) malloc(logLength);
    glGetProgramInfoLog(prog, logLength, &logLength, log);
    NSLog(@"Program validate log:\n%s", log);
    free(log);
  }
  
  glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
  if (status == 0)
    return FALSE;
  
  return TRUE;
}

- (BOOL)loadShaders {
  GLuint vertShader, fragShader;
  NSString *vertShaderPathname, *fragShaderPathname;
  
  // Create shader program
  program = glCreateProgram();
  
  // Create and compile vertex shader
  vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];
  if (![self compileShader:&vertShader type:GL_VERTEX_SHADER file:vertShaderPathname]) {
    NSLog(@"Failed to compile vertex shader");
    return FALSE;
  }
  
  // Create and compile fragment shader
  fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"fsh"];
  if (![self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:fragShaderPathname]) {
    NSLog(@"Failed to compile fragment shader");
    return FALSE;
  }
  
  // Attach vertex shader to program
  glAttachShader(program, vertShader);
  
  // Attach fragment shader to program
  glAttachShader(program, fragShader);
  
  // Bind attribute locations
  // this needs to be done prior to linking
  //glBindAttribLocation(program, ATTRIB_VERTEX, "position");
  //glBindAttribLocation(program, ATTRIB_COLOR, "color");
  
  // Link program
  if (![self linkProgram:program]) {
    NSLog(@"Failed to link program: %d", program);
    
    if (vertShader) {
      glDeleteShader(vertShader);
      vertShader = 0;
    }
    if (fragShader) {
      glDeleteShader(fragShader);
      fragShader = 0;
    }
    if (program) {
      glDeleteProgram(program);
      program = 0;
    }
    
    return FALSE;
  }
  
  // Get uniform locations
  //uniforms[UNIFORM_TRANSLATE] = glGetUniformLocation(program, "translate");
  
  // Release vertex and fragment shaders
  if (vertShader)
    glDeleteShader(vertShader);
  if (fragShader)
    glDeleteShader(fragShader);
  
  return TRUE;
}

- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer {
  // Allocate color buffer backing based on the current layer size
  glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
  [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer];
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
  
  // damos tamaño al buffer de profundidad
  glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
  
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    return NO;
  }
  
  return YES;
}

- (void)dealloc {
  // Tear down GL
  if (defaultFramebuffer) {
    glDeleteFramebuffers(1, &defaultFramebuffer);
    defaultFramebuffer = 0;
  }
  
  if (colorRenderbuffer) {
    glDeleteRenderbuffers(1, &colorRenderbuffer);
    colorRenderbuffer = 0;
  }
  
  if (depthRenderbuffer) {
    glDeleteRenderbuffers(1, &depthRenderbuffer);
    depthRenderbuffer = 0;
  }
  
  if (program) {
    glDeleteProgram(program);
    program = 0;
  }
  
  // Tear down context
  if ([EAGLContext currentContext] == context)
    [EAGLContext setCurrentContext:nil];
  
  context = nil;
  
}

@end
