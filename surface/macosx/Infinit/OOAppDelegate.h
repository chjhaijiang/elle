//
//  OOAppDelegate.h
//  Infinit
//
//  Created by Charles Guillot - Infinit on 3/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface OOAppDelegate : NSObject <NSApplicationDelegate>
{
    NSWindow *window;
    IBOutlet NSMenu *statusMenu;
    NSStatusItem * statusItem;
}

@property (assign) IBOutlet NSWindow *window;
- (void)launch8infinit;
@end
