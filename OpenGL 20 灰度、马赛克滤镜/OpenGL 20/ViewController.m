//
//  ViewController.m
//  OpenGL 20
//
//  Created by edz on 2021/1/25.
//  Copyright © 2021 bhj. All rights reserved.
//

#import "ViewController.h"
#import <GLKit/GLKit.h>
#import "HJFilterBar.h"

typedef struct {
    GLKVector3 position;
    GLKVector2 textureCoord;
} HJSenceVertex;

@interface ViewController ()<HJFilterBarDelegate>

@property (nonatomic, strong) EAGLContext *myContext;
@property (nonatomic, assign) HJSenceVertex *vertices;
@property (nonatomic, assign) NSTimeInterval startTime;
@property (nonatomic, strong) CADisplayLink *displayLink;
@property (nonatomic, assign) GLuint program; // 程序
@property (nonatomic, assign) GLuint textureID;// 纹理ID
@property (nonatomic, assign) GLuint vertexBuffer;// 顶点缓存区标识
@property (nonatomic, strong) HJFilterBar *toolBar;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self setupContext];
    
    [self setupLayer];
    
    [self filterInit];
    
    [self startFilterAnimation];
    
    [self.view addSubview:self.toolBar];
}

- (void)viewDidDisappear:(BOOL)animated{
    [super viewDidDisappear:animated];
    
    if (self.displayLink) {
        [self.displayLink invalidate];
        self.displayLink = nil;
    }
}

- (void)dealloc
{
    if ([EAGLContext currentContext] == self.myContext) {
        [EAGLContext setCurrentContext:nil];
    }
    
    if (_vertexBuffer) {
        glDeleteBuffers(1, &_vertexBuffer);
        _vertexBuffer = 0;
    }
    if (_vertices) {
        free(_vertices);
        _vertices = nil;
    }
}

//获取渲染缓存区的宽
- (GLint)drawableWidth {
    GLint backingWidth;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
    return backingWidth;
}
//获取渲染缓存区的高
- (GLint)drawableHeight {
    GLint backingHeight;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
    return backingHeight;
}

- (HJFilterBar *)toolBar{
    if (!_toolBar) {
        _toolBar = [[HJFilterBar alloc]initWithFrame:CGRectMake(0, self.view.frame.size.height - 100, self.view.frame.size.width, 90) items:@[@"无",@"灰度滤镜",@"颠倒滤镜",@"旋涡滤镜",@"马赛克",@"六边形马赛克",@"三角形马赛克"]];
        _toolBar.delegate = self;
    }
    return _toolBar;
}
#pragma mark - setupContext
-(void)setupContext{
    
    self.myContext = [[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES3];
    [EAGLContext setCurrentContext:self.myContext];
}

-(void)setupLayer{
    
    CAEAGLLayer *layer = [[CAEAGLLayer alloc]init];
    layer.frame = CGRectMake(0, 150, CGRectGetWidth(self.view.bounds), CGRectGetWidth(self.view.bounds));
    layer.contentsScale = [[UIScreen mainScreen] scale];
    [self.view.layer addSublayer:layer];
    [self bindRenderLayer:layer];
}

-(void)bindRenderLayer:(CALayer <EAGLDrawable> *)layer{
    
    GLuint renderBuffer;
    GLuint frameBuffer;
    
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    [self.myContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer];
    
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);
}

#pragma mark - filterInit
-(void)filterInit{
    
    self.vertices = malloc(sizeof(HJSenceVertex) * 4);
    self.vertices[0] = (HJSenceVertex){{-1.0f,-1.0f,0.0f},{0.0f,0.0f}};
    self.vertices[1] = (HJSenceVertex){{-1.0f,1.0f,0.0f},{0.0f,1.0f}};
    self.vertices[2] = (HJSenceVertex){{1.0f,-1.0f,0.0f},{1.0f,0.0f}};
    self.vertices[3] = (HJSenceVertex){{1.0f,1.0f,0.0f},{1.0f,1.0f}};

    [self setTextureWithImage:@"jugeng"];
    
    glViewport(0, 0, self.drawableWidth, self.drawableHeight);
    
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    GLsizeiptr bufferSizeBytes = sizeof(HJSenceVertex) * 4;
    glBufferData(GL_ARRAY_BUFFER, bufferSizeBytes, self.vertices, GL_STATIC_DRAW);
    
    self.vertexBuffer = vertexBuffer;
    
    [self setNormalShader];
}

-(void)setupShaderProgramWithShaderName:(NSString *)shaderName{
    
    GLuint program = [self programWithShader:shaderName];
    
    glUseProgram(program);
    // 获取着色器的属性值的索引位置
    GLuint positionSlot = glGetAttribLocation(program, "Position");
    GLuint textureSlot = glGetUniformLocation(program, "Texture");
    GLuint textureCoordsSlot = glGetAttribLocation(program, "TextureCoords");
    // 激活纹理、绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE0, self.textureID);
    
    // 纹理sample
    glUniform1i(textureSlot, 0);
    
    // 打开positionSlot并将数据传递到positionSlot
    glEnableVertexAttribArray(positionSlot);
    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, sizeof(HJSenceVertex), NULL + offsetof(HJSenceVertex, position));
    
    glEnableVertexAttribArray(textureCoordsSlot);
    glVertexAttribPointer(textureCoordsSlot, 2, GL_FLOAT, GL_FALSE, sizeof(HJSenceVertex), NULL + offsetof(HJSenceVertex, textureCoord));
    
    self.program = program;
}


-(void)setTextureWithImage:(NSString *)name{
    
    UIImage *image = [UIImage imageNamed:name];
    self.textureID = [self createTextureWithImage:image];
}

-(GLuint)createTextureWithImage:(UIImage *)image{
    // 将图片转成 CGImageRef
    CGImageRef cgImageRef = [image CGImage];
    if (!cgImageRef) {
        NSAssert(NO, @"Failed to load image");
        return 0;
    }
    // 获取图片的宽、高
    GLuint width,height;
    width = (GLuint)CGImageGetWidth(cgImageRef);
    height = (GLuint)CGImageGetHeight(cgImageRef);
    // 图片的rect
    CGRect imageRect = CGRectMake(0, 0, width, height);
    // 获取图片的颜色空间
    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    // 获取图片的字节数
    void *imageData = malloc(width * height * 4);
    /*
     创建上下文
     
     参数1：data,指向要渲染的绘制图像的内存地址
     参数2：width,bitmap的宽度，单位为像素
     参数3：height,bitmap的高度，单位为像素
     参数4：bitPerComponent,内存中像素的每个组件的位数，比如32位RGBA，就设置为8
     参数5：bytesPerRow,bitmap的没一行的内存所占的比特数
     参数6：colorSpace,bitmap上使用的颜色空间  kCGImageAlphaPremultipliedLast：RGBA
     */
    CGContextRef imageContext = CGBitmapContextCreate(imageData, width, height, 8, width * 4, colorSpaceRef, kCGImageAlphaPremultipliedLast | kCGImageByteOrder32Big);
    
    // 图片翻转
    CGContextTranslateCTM(imageContext, 0, height);
    CGContextScaleCTM(imageContext, 1.0f, -1.0f);
    CGColorSpaceRelease(colorSpaceRef);
    CGContextClearRect(imageContext, imageRect);
    // 重绘
    CGContextDrawImage(imageContext, imageRect, cgImageRef);
    
    // 获取纹理
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D,textureID);
    
    // 载入纹理
    /*
     参数1：纹理模式，GL_TEXTURE_1D、GL_TEXTURE_2D、GL_TEXTURE_3D
     参数2：加载的层次，一般设置为0
     参数3：纹理的颜色值GL_RGBA
     参数4：宽
     参数5：高
     参数6：border，边界宽度
     参数7：format
     参数8：type
     参数9：纹理数据
     */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    
    // 设置纹理属性
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    CGContextRelease(imageContext);
    free(imageData);
    
    return textureID;
}

/// 开启定时器
-(void)startFilterAnimation{
    
    if (self.displayLink) {
        [self.displayLink invalidate];
        self.displayLink = nil;
    }
    self.startTime = 0;
    self.displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(timeAction)];
    
    [self.displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
}

/// 定时动画
-(void)timeAction{
    
    if (self.startTime == 0) {
        self.startTime = self.displayLink.timestamp;
    }
    glUseProgram(self.program);
    glBindBuffer(GL_ARRAY_BUFFER, self.vertexBuffer);
    
    CGFloat currentTime = self.displayLink.timestamp - self.startTime;
    GLuint time = glGetUniformLocation(self.program, "Time");
    glUniform1f(time, currentTime);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    [self.myContext presentRenderbuffer:GL_RENDERBUFFER];
}
#pragma mark - Shader
-(GLuint)programWithShader:(NSString *)shaderName{
    
    GLuint vertexShader = [self compileShander:shaderName type:GL_VERTEX_SHADER];
    GLuint fragmentShader = [self compileShander:shaderName type:GL_FRAGMENT_SHADER];
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    
    glLinkProgram(program);
    
    GLint linkSucess;
    glGetProgramiv(program, GL_LINK_STATUS, &linkSucess);
    if (linkSucess == GL_FALSE) {
        GLchar messages[256];
        glGetProgramInfoLog(program, sizeof(messages), 0, &messages[0]);
        NSString *error = [NSString stringWithUTF8String:messages];
        NSAssert(NO, @"program Link error \n %@",error);
        return 0;
    }
    return program;
}


/// 编译着色器
/// @param name 着色器名
/// @param type 着色器类型
-(GLuint)compileShander:(NSString *)name type:(GLuint)type{
    
    NSString *shaderPath = [[NSBundle mainBundle]pathForResource:name ofType:type == GL_VERTEX_SHADER ? @"vsh":@"fsh"];
    NSError *error;
    
    NSString *shaderStr = [NSString stringWithContentsOfFile:shaderPath encoding:NSUTF8StringEncoding error:&error];
    if (!shaderStr) {
        NSAssert(NO, @"读取shader失败");
        return 0;
    }
    // 创建shader
    GLuint shader = glCreateShader(type);
    
    const GLchar *shaderChar = (GLchar *)[shaderStr UTF8String];
    int shaderLength = (int)[shaderStr length];
    // 获取shader source
    glShaderSource(shader, 1, &shaderChar, &shaderLength);
    // 编译shader
    glCompileShader(shader);
    
    GLint compileSuccess;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE) {
        GLchar message[512];
        glGetShaderInfoLog(shader, sizeof(message), 0, &message[0]);
        NSString *messageString = [NSString stringWithUTF8String:message];
        NSAssert(NO, @"shader编译失败！\n%@",messageString);
        return 0;
    }
    return shader;
}

#pragma mark - Filter
/// 正常着色器
-(void)setNormalShader{
    
    [self setupShaderProgramWithShaderName:@"normal"];
}

/// 灰度着色器
-(void)setGrayShader{
    
    [self setupShaderProgramWithShaderName:@"gray"];
}

/// 颠倒着色器
-(void)setReversalShader{
    
    [self setupShaderProgramWithShaderName:@"reversal"];
}

/// 旋涡着色器
-(void)setVortexShader{
    
    [self setupShaderProgramWithShaderName:@"vortex"];
}

/// 马赛克
-(void)setMosaicShader{
    
    [self setupShaderProgramWithShaderName:@"mosaic"];
}

/// 六边形马赛克
-(void)setHexagonMosaicShader{
    
    [self setupShaderProgramWithShaderName:@"hexagonMosaic"];
}

/// 三角形马赛克
-(void)setTriangularMosaicShader{
    
    [self setupShaderProgramWithShaderName:@"triangularMosaic"];
}
#pragma mark - HJFilterBarDelegate
- (void)toolBarSelected:(NSInteger)index{
    
    switch (index) {
        case 0:
            [self setNormalShader];
            break;
        case 1:
            [self setGrayShader];
            break;
        case 2:
            [self setReversalShader];
            break;
        case 3:
            [self setVortexShader];
            break;
        case 4:
            [self setMosaicShader];
            break;
        case 5:
            [self setHexagonMosaicShader];
            break;
        case 6:
            [self setTriangularMosaicShader];
            break;
            
        default:
            break;
    }
}

@end
