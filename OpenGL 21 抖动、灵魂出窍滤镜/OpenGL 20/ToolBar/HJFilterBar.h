//
//  HJFilterBar.h
//  OpenGL 20
//
//  Created by edz on 2021/1/25.
//  Copyright © 2021 bhj. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@protocol HJFilterBarDelegate <NSObject>

-(void)toolBarSelected:(NSInteger)index;

@end

@interface HJFilterBar : UIView

- (instancetype)initWithFrame:(CGRect)frame items:(NSArray *)items;

@property (nonatomic, assign) id <HJFilterBarDelegate> delegate;

@end

NS_ASSUME_NONNULL_END
