//
//  MargitOpenGLView.h
//
//  Created by Mikael Brockman on 11/7/10.
//  Copyright 2010 Göteborgs universitet. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

#import "MargitGame.h"

@interface MargitOpenGLView : NSOpenGLView {
  MargitGame* game;
  
  CVDisplayLinkRef displayLink;
}

- (void) prepareOpenGL;
- (CVReturn) getFrameForTime: (const CVTimeStamp*) outputTime;
- (void) dealloc;

- (MargitGame*) game;

@end
