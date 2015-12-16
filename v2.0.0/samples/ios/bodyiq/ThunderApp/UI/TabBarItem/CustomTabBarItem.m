//
//  CustomTabBarItem.m
//  Tecnoexpress
//
//  Created by Pablo Garcia-Morato on 28/04/14.
//  Copyright (c) 2015 Intel. All rights reserved.
//

#import "CustomTabBarItem.h"

@implementation CustomTabBarItem

#define imageScale 1.6

- (UIImage *)selectedImage
{
    return [self getImage:self.tag selected:YES scale:imageScale renderingMode:UIImageRenderingModeAlwaysOriginal];
}

- (UIImage *)unselectedImage
{
    return [self getImage:self.tag selected:NO scale:imageScale renderingMode:UIImageRenderingModeAlwaysOriginal];
}

- (NSString*)title
{
    NSString *title;
        
    [self setTitleTextAttributes:@{ NSForegroundColorAttributeName : [UIColor colorWithRed:236.0 green:245.0 blue:250.0 alpha:1.0] }
                        forState:UIControlStateNormal];
    [self setTitleTextAttributes:@{ NSForegroundColorAttributeName : [UIColor colorWithRed:236.0 green:245.0 blue:250.0 alpha:1.0] }
                        forState:UIControlStateSelected];
    
    NSAssert( self.tag == 1 || self.tag == 2, @"Review Tab bar items tags" );
    
    switch ( self.tag )
    {
        case 1:
            title = NSLocalizedString(@"all_activity", nil);
            break;
            
        case 2:
            title = NSLocalizedString(@"daily_tracker", nil);
            break;
            
        default:
            break;
    }
    
    return title;
}


#pragma mark - Utils

- (UIImage*)getImage:(NSInteger)tag selected:(BOOL)selected scale:(CGFloat)scale renderingMode:(UIImageRenderingMode)renderingMode
{
    UIImage *image;
    
    NSAssert( tag == 1 || tag == 2, @"Review Tab bar items tags" );
    
    // Set image
    switch ( tag )
    {
        case 1:
            image       = [UIImage imageNamed:[NSString stringWithFormat:@"all_activity%@.png", selected ? @"_on" : @"_off"]];
            break;
            
        case 2:
            image       = [UIImage imageNamed:[NSString stringWithFormat:@"daily_activity%@.png", selected ? @"_on" : @"_off"]];
            break;
            
        default:
            break;
    }
    
    NSAssert( image != nil, @"Check if image stored in local resources!" );
    
    // Scale image
    CGImageRef cgimage      = image.CGImage;
    image                   = [UIImage imageWithCGImage:cgimage scale:scale orientation:UIImageOrientationUp];
    
    // Specify rendering mode
    image                   = [image imageWithRenderingMode:renderingMode];
    
    return image;
}


#pragma mark - Dealloc

- (void)dealloc
{

}

@end