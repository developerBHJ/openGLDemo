//
//  HJFilterCell.m
//  OpenGL 20
//
//  Created by edz on 2021/1/25.
//  Copyright © 2021 bhj. All rights reserved.
//

#import "HJFilterCell.h"

@interface HJFilterCell ()

@property (nonatomic, strong) UILabel *titleLable;

@end

@implementation HJFilterCell


- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self addSubview:self.titleLable];
        self.titleLable.frame = CGRectMake(0, 0, self.frame.size.width, self.frame.size.height);
    }
    return self;
}

- (void)setSelected:(BOOL)selected{
    
    if (selected) {
        self.titleLable.textColor = [UIColor whiteColor];
        self.titleLable.backgroundColor = [UIColor blackColor];
    }else{
        self.titleLable.textColor = [UIColor blackColor];
        self.titleLable.backgroundColor = [UIColor whiteColor];
    }
}

- (void)setTitle:(NSString *)title{
    
    if (title.length > 2) {
        NSString *firStr = [title substringToIndex:2];
        NSString *secondStr = [title substringFromIndex:2];
        _title = [NSString stringWithFormat:@"%@\n%@",firStr,secondStr];
    }else{
        _title = title;
    }
    self.titleLable.text = _title;
}

- (UILabel *)titleLable{
    
    if (!_titleLable) {
        _titleLable = [[UILabel alloc]init];
        _titleLable.textAlignment = NSTextAlignmentCenter;
        _titleLable.textColor = [UIColor blackColor];
        _titleLable.font = [UIFont systemFontOfSize:16];
        _titleLable.text = @"无";
        _titleLable.backgroundColor = [UIColor whiteColor];
        _titleLable.layer.borderColor = [UIColor whiteColor].CGColor;
        _titleLable.layer.borderWidth = 1.0;
        _titleLable.layer.cornerRadius = self.frame.size.height / 2;
        _titleLable.layer.masksToBounds = YES;
        _titleLable.numberOfLines = 0;
    }
    return _titleLable;
}

@end
