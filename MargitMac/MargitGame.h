//
//  MargitGame.h
//  MargitMac
//
//  Created by Mikael Brockman on 11/7/10.
//  Copyright 2010 Göteborgs universitet. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include <margit/game.hpp>

@interface MargitGame : NSObject {
  margit::Game game;
}

- (void) render;
- (void) step;
- (void) punt;
- (void) snap;
- (void) unsnap;

@end
