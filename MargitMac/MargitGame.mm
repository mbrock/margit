//
//  MargitGame.m
//  MargitMac
//
//  Created by Mikael Brockman on 11/7/10.
//  Copyright 2010 Göteborgs universitet. All rights reserved.
//

#import "MargitGame.h"

@implementation MargitGame

- (void) step
{
  game.step ();
}

- (void) render
{
  game.render ();
}

- (void) punt
{
  game.punt ();
}

- (void) snap { game.snap (); }
- (void) unsnap { game.unsnap (); }

@end
