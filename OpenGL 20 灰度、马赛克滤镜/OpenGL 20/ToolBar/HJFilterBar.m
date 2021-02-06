//
//  HJFilterBar.m
//  OpenGL 20
//
//  Created by edz on 2021/1/25.
//  Copyright © 2021 bhj. All rights reserved.
//

#import "HJFilterBar.h"
#import "HJFilterCell.h"


@interface HJFilterBar ()<UICollectionViewDelegate,UICollectionViewDataSource,UICollectionViewDelegateFlowLayout>

@property (nonatomic, strong) UICollectionView *collectionView;
@property (nonatomic, strong) NSMutableArray *dataSource;

@end

@implementation HJFilterBar

- (instancetype)initWithFrame:(CGRect)frame items:(NSArray *)items
{
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor colorWithRed:0.3 green:0.35 blue:0.35 alpha:1.0];
        self.dataSource = [NSMutableArray arrayWithArray:items];
        [self addSubview:self.collectionView];
        [self.collectionView reloadData];
        [self collectionView:self.collectionView didSelectItemAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]];
        [self.collectionView selectItemAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0] animated:NO scrollPosition:UICollectionViewScrollPositionNone];
    }
    return self;
}

- (UICollectionView *)collectionView{
    
    if (!_collectionView) {
        UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc]init];
        layout.scrollDirection = UICollectionViewScrollDirectionHorizontal;
        _collectionView = [[UICollectionView alloc]initWithFrame:CGRectMake(15, 10, self.frame.size.width - 30, self.frame.size.height - 20) collectionViewLayout:layout];
        _collectionView.backgroundColor = [UIColor clearColor];
        _collectionView.delegate = self;
        _collectionView.dataSource = self;
        [_collectionView registerClass:[HJFilterCell class] forCellWithReuseIdentifier:NSStringFromClass([HJFilterCell class])];
    }
    return _collectionView;
}

- (NSMutableArray *)dataSource{
    
    if (!_dataSource) {
        _dataSource = [[NSMutableArray alloc]init];
    }
    return _dataSource;
}
#pragma mark - UICollectionViewDelegate
- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section{
    
    return self.dataSource.count;
}

- (__kindof UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath{
    
    HJFilterCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:NSStringFromClass([HJFilterCell class]) forIndexPath:indexPath];
    cell.title = self.dataSource[indexPath.row];
    return cell;
}

-(CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath{
    
    return CGSizeMake(self.frame.size.height - 20, self.frame.size.height - 20);
}

- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath{
    
    if (self.delegate && [self.delegate respondsToSelector:@selector(toolBarSelected:)]) {
        [self.delegate toolBarSelected:indexPath.row];
    }
}


@end
