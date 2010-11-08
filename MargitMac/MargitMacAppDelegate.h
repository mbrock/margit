#import <Cocoa/Cocoa.h>

#import "MargitOpenGLView.h"

@interface MargitMacAppDelegate : NSObject <NSApplicationDelegate> {
  NSWindow* window;
  MargitOpenGLView* openGLView;
}

@property (assign) IBOutlet NSWindow* window;
@property (assign) IBOutlet MargitOpenGLView* openGLView;

- (IBAction) onPunt: (id) sender;
- (IBAction) onSnap: (id) sender;
- (IBAction) onUnsnap: (id) sender;

@end
