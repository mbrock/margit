#import "MargitMacAppDelegate.h"

@implementation MargitMacAppDelegate

@synthesize window;
@synthesize openGLView;

- (void) applicationDidFinishLaunching: (NSNotification *) notification
{
  srandomdev ();
}

- (IBAction) onPunt: (id) sender
{
  [[openGLView game] punt];
}

- (IBAction) onSnap: (id) sender { [[openGLView game] snap]; }
- (IBAction) onUnsnap: (id) sender { [[openGLView game] unsnap]; }

@end
