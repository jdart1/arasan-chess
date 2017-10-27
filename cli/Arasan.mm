//
//  Arasan.m
//  cli
//
//  Created by Vanuj Juneja on 10/25/17.
//  Copyright © 2017 Vanuj Juneja. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "arasanx.hpp"
#import "Arasan.h"

@implementation Arasan
-(int)mainwithargc:(int) argc argv:(char**)argv
{
    arasan *a = new arasan();
    return a->arasanmain(argc, argv);
}
@end
