//
//  CWrapper.m
//  iitp-ios-tc04-2022
//
//  Created by lfin on 2022/04/26.
//

#import "CVWrapper.h"
#include "common.hpp"
#import <opencv2/opencv.hpp>
#import <opencv2/imgcodecs/ios.h>

@implementation CVWrapper

// test 함수
//+ (const char *)logTest {
//    //sleep
//    [NSThread sleepForTimeInterval:1.000];
//    const char *testResult = lpin::opencv::LogTest();
//    return testResult;
//}
//
//+ (const char *)CsvTest {
//    //sleep
//    [NSThread sleepForTimeInterval:1.000];
//    //const char *testResult = lpin::opencv::CsvTest();
//    return testResult;
//}

+ (int) initialize:(int)taskCode{
    int result = lpin::opencv::Initialize(taskCode);
    return result;
}

+ (int) processModule{
//    NSLog(@"processModule()");
    //[NSThread sleepForTimeInterval:1.00];
    int result = lpin::opencv::Process();
    return result;
}

+ (const char *) getPtrOfStringModule:(int) requestCode{
//    NSLog(@"getPtrOfStringModule()");
    //[NSThread sleepForTimeInterval:0.10];
    const char *result = lpin::opencv::GetPtrOfString(requestCode);
       
    
    return result;
}


+ (int) putByteBlockModule: (NSData *) data{//:(char *) data (int) length{
    //printf(data);
//    NSLog(@"putByteBlockModule()");
    
    char *bytePtr = (char *)[data bytes];
    int result = lpin::opencv::PutByteBlock(bytePtr);
//    NSUInteger len = [data length];
//    Byte *byteData = (Byte*)malloc(len);
//    memcpy(byteData, [data bytes], len);
//    std::cout<<"byteData: "<<byteData<<"\n";
    
    return 0;
}

// UIImage를 unsigned char*, width, height를 C++ 모듈로 전달
+ (int) putImageModule:(UIImage *)image{
//    NSLog(@"putImageModule()");
    CGImageRef imageRef = image.CGImage;
    // Create a bitmap context to draw the uiimage into
    CGContextRef context = [self newBitmapRGBA8ContextFromImage:imageRef];

    if(!context) {
        return NULL;
    }

    size_t width = CGImageGetWidth(imageRef);
    size_t height = CGImageGetHeight(imageRef);

    CGRect rect = CGRectMake(0, 0, width, height);

     //Draw image into the context to get the raw image data
    CGContextDrawImage(context, rect, imageRef);

    // Get a pointer to the data
    unsigned char *bitmapData = (unsigned char *)CGBitmapContextGetData(context);

    // Copy the data and release the memory (return memory allocated with new)
    size_t bytesPerRow = CGBitmapContextGetBytesPerRow(context);
    size_t bufferLength = bytesPerRow * height;

    char *newBitmap = NULL;

    if(bitmapData) {
        newBitmap = (char *)malloc(sizeof(unsigned char) * bytesPerRow * height);

        if(newBitmap) {    // Copy the data
            for(int i = 0; i < bufferLength; ++i) {
                newBitmap[i] = bitmapData[i];
            }
        }

        free(bitmapData);

    } else {
        NSLog(@"Error getting bitmap pixel data\n");
    }
    
    int putImageResult = lpin::opencv::PutImage(newBitmap);

//    int putImageResult = 0;
    
    CGContextRelease(context);
    image = NULL;
    
    imageRef = NULL;
    free(newBitmap);//, width, height);
    
    return putImageResult;
    
}

+ (CGContextRef) newBitmapRGBA8ContextFromImage:(CGImageRef) image {
    CGContextRef context = NULL;
    CGColorSpaceRef colorSpace;
    uint32_t *bitmapData;
    
    size_t bitsPerPixel = 32;
    size_t bitsPerComponent = 8;
    size_t bytesPerPixel = bitsPerPixel / bitsPerComponent;
    
    size_t width = CGImageGetWidth(image);
    size_t height = CGImageGetHeight(image);
    
    size_t bytesPerRow = width * bytesPerPixel;
    size_t bufferLength = bytesPerRow * height;
    
    colorSpace = CGColorSpaceCreateDeviceRGB();
    
    if(!colorSpace) {
        NSLog(@"Error allocating color space RGB\n");
        return NULL;
    }
    
    // Allocate memory for image data
    bitmapData = (uint32_t *)malloc(bufferLength);
    
    if(!bitmapData) {
        NSLog(@"Error allocating memory for bitmap\n");
        CGColorSpaceRelease(colorSpace);
        return NULL;
    }
    
    //Create bitmap context
    
    context = CGBitmapContextCreate(bitmapData,
            width,
            height,
            bitsPerComponent,
            bytesPerRow,
            colorSpace,
            kCGImageAlphaPremultipliedLast);    // RGBA
    if(!context) {
        free(bitmapData);
        NSLog(@"Bitmap context not created");
    }
    
    CGColorSpaceRelease(colorSpace);
    
    return context;
}

+ (int) start{
    int result = lpin::opencv::Start();
    return result;
}

//// UIImage를 unsigned char*, width, height를 C++ 모듈로 전달
//+ (unsigned char *) convertUIImageToBitmapRGBA8:(UIImage *) image {
//    CGImageRef imageRef = image.CGImage;
////
//    // Create a bitmap context to draw the uiimage into
//    CGContextRef context = [self newBitmapRGBA8ContextFromImage:imageRef];
//
//    if(!context) {
//        return NULL;
//    }
//
//    size_t width = CGImageGetWidth(imageRef);
//    size_t height = CGImageGetHeight(imageRef);
//
//    CGRect rect = CGRectMake(0, 0, width, height);
//
//    // Draw image into the context to get the raw image data
//    CGContextDrawImage(context, rect, imageRef);
//
//    // Get a pointer to the data
//    unsigned char *bitmapData = (unsigned char *)CGBitmapContextGetData(context);
//
//    // Copy the data and release the memory (return memory allocated with new)
//    size_t bytesPerRow = CGBitmapContextGetBytesPerRow(context);
//    size_t bufferLength = bytesPerRow * height;
//
//    unsigned char *newBitmap = NULL;
//
//    if(bitmapData) {
//        newBitmap = (unsigned char *)malloc(sizeof(unsigned char) * bytesPerRow * height);
//
//        if(newBitmap) {    // Copy the data
//            for(int i = 0; i < bufferLength; ++i) {
//                newBitmap[i] = bitmapData[i];
//            }
//        }
//
//        free(bitmapData);
//
//    } else {
//        NSLog(@"Error getting bitmap pixel data\n");
//    }
//
//    CGContextRelease(context);
//
//
//    return newBitmap;
//}


@end
