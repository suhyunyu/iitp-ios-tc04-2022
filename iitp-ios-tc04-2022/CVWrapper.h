//
//  CWrapper.h
//  iitp-ios-tc04-2022
//
//  Created by lfin on 2022/04/26.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface CVWrapper : NSObject
//+ (const char *)logTest;
//+ (const char *)CsvTest;
+ (int) initialize:(int32_t) taskCode;
+ (int) putImageModule:(UIImage *) image;
+ (int) processModule;
+ (char *) getPtrOfStringModule:(int) requestCode;
+ (int) putByteBlockModule: (NSData *) data;
+ (CGContextRef) newBitmapRGBA8ContextFromImage:(CGImageRef) image;
+ (int) start;
@end

NS_ASSUME_NONNULL_END
