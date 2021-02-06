//
//  main.cpp
//  OpenGL_demo1
//
//  Created by baihuajun on 2020/3/27.
//  Copyright © 2020 baihuajun. All rights reserved.
//

#include "GLTools.h"
#include "GLBatch.h"
#include "GLShaderManager.h"
#include "GLFrustum.h"
#include "GLFrame.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"


#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif


GLShaderManager  shaderManager; // 着色器管理器
GLMatrixStack modelViewMatrix; // 模型视图矩阵
GLMatrixStack projectMatrix; // 投影矩阵
GLFrustum viewFrustum;// 视景体
GLGeometryTransform transformPepeline; // 几何变换管线


GLBatch floorBatch; // 地面
GLBatch leftBatch; // 左边墙体
GLBatch rightBatch; // 右边墙体
GLBatch ceilingBatch; // 天花板

//  深度
GLfloat viewZ = -200.f;

// 纹理标识
#define TEXTURE_BRICK   0 //墙面
#define TEXTURE_FLOOR   1 //地板
#define TEXTURE_CEILING 2 //纹理天花板
#define TEXTURE_COUNT   3 //纹理个数

GLuint  textures[TEXTURE_COUNT];//纹理标记数组
// 纹理名称数组
const char *szTextureFiles[TEXTURE_COUNT] = { "brick.tga", "floor.tga", "ceiling.tga" };

// 菜单选择
void ProcessMenu(int value){
    
    GLint iLoop;
    for (iLoop = 0; iLoop < TEXTURE_COUNT; iLoop ++) {
        // 绑定纹理
        glBindTexture(GL_TEXTURE_2D, textures[iLoop]);
        // 配置纹理参数
        switch (value) {
            case 0:
                // GL_TEXTURE_MIN_FILTER 缩小过滤。GL_NEAREST 临近过滤
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                break;
            case 1:
                // GL_TEXTURE_MIN_FILTER 缩小过滤。GL_LINEAR 线性过滤
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                break;
            case 2:
                // GL_TEXTURE_MAG_FILTER 放大过滤。GL_LINEAR 临近过滤
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            case 3:
                // GL_TEXTURE_MAG_FILTER 放大过滤。GL_LINEAR 线性过滤
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
            case 4:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST_MIPMAP_LINEAR（选择最邻近Mip层，并执行线性过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                break;
                
            case 5:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_LINEAR_MIPMAP_LINEAR（在Mip层之间执行线性插补，并执行线性过滤，又称为三线性过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                break;
                
            case 6:
                
                //设置各向异性过滤
                GLfloat fLargest;
                //获取各向异性过滤的最大数量
                glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
                //设置纹理参数(各向异性采样)
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
                break;
            case 7:
                //设置各向同性过滤，数量为1.0表示(各向同性采样)
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
                break;
            default:
                break;
        }
    }
    glutPostRedisplay();
}

void SetupRC()
{
    // 1、
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    // 2、
    shaderManager.InitializeStockShaders();
    
    GLbyte *pBytes;
    GLint iWidth,iHeight,iComponents;
    GLenum eFormat;
    GLint iLoop;
    //3、 生成纹理标记
    glGenTextures(TEXTURE_COUNT, textures);
    // 4、循环设置纹理数组的纹理参数
    for (iLoop = 0; iLoop < TEXTURE_COUNT; iLoop ++) {
        // 绑定纹理
        /*
         glBindTexture
         参数1: 纹理模式 GL_TEXTURE_1D,GL_TEXTURE_2D,GL_TEXTURE_3D
         参数2: 纹理对象
         */
        glBindTexture(GL_TEXTURE_2D, textures[iLoop]);
        // 读取TGA文件
        /*
         gltReadTGABits:
         参数1:纹理文件
         参数2:文件宽度变量地址
         参数3:文件高度变量地址
         参数4:文件组件变量地址
         参数5：文件格式变量地址
         返回值：pBytes，指向图像数据的指针
         */
        pBytes = gltReadTGABits(szTextureFiles[iLoop], &iWidth, &iHeight, &iComponents, &eFormat);
        // 加载纹理  设置过滤器和包装模式
        //GL_TEXTURE_MAG_FILTER（放大过滤器,GL_NEAREST(最邻近过滤)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //GL_TEXTURE_MIN_FILTER(缩小过滤器),GL_NEAREST(最邻近过滤)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        // GL_TEXTURE_WRAP_S (s轴环绕)，GL_CLAMP_TO_EDGE（环绕模式，强制对范围之外的纹理坐标沿着合法的纹理单元的最后一行或一列进行采集）
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        // GL_TEXTURE_WRAP_S (t轴环绕)，GL_CLAMP_TO_EDGE（环绕模式，强制对范围之外的纹理坐标沿着合法的纹理单元的最后一行或一列进行采集）
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        // 载入纹理
        /*
         glTexImage2D:
         参数1:纹理纬度
         参数2:mip贴图层次
         参数3:纹理单元存储的颜色成分
         参数4:加载纹理宽度
         参数5:加载纹理高度
         参数6:加载纹理的深度
         参数7:像素数据的数据类型（GL_UNSIGNED_BYTE 无符号整型）
         参数8:指向纹理图像数据的指针
         */
        glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
        /*
         为纹理对象生成一组mip贴图
         */
        glGenerateMipmap(GL_TEXTURE_2D);
        
        // 释放原始纹理数据
        free(pBytes);
    }
    
    // 5、设置几何图形定点数据
    GLfloat z;
    /*
     GLTools库中的容器类，GBatch，
     void GLBatch::Begin(GLenum primitive,GLuint nVerts,GLuint nTextureUnits = 0);
     参数1:图元枚举值
     参数2:定点数
     参数3:1组或者2组纹理坐标
     */
    floorBatch.Begin(GL_TRIANGLE_STRIP, 28,1);
    
    for (z  =  200.0f; z >= 0; z -= 10.f) {
        // 纹理坐标
        floorBatch.MultiTexCoord2f(0.0f, 0.0f, 0.0f);
        // 顶点坐标
        floorBatch.Vertex3f(-10.0f, -10.0f, z);
        
        floorBatch.MultiTexCoord2f(0.0f, 1.0f, 0.0f);
        floorBatch.Vertex3f(10.0f, -10.0f, z);
        
        floorBatch.MultiTexCoord2f(0.0f, 0.0f, 1.0f);
        floorBatch.Vertex3f(-10.0f, -10.0f, z - 10.0f);
        
        floorBatch.MultiTexCoord2f(0.0f, 1.0f, 1.0f);
        floorBatch.Vertex3f(10.0f, -10.0f, z - 10.0f);
    }
    floorBatch.End();
    
    leftBatch.Begin(GL_TRIANGLE_STRIP, 28,1);
    for (z  =  200.0f; z >= 0; z -= 10.f) {
        // 纹理坐标
        leftBatch.MultiTexCoord2f(0.0f, 0.0f, 0.0f);
        // 顶点坐标
        leftBatch.Vertex3f(-10.0f, -10.0f, z);
        
        leftBatch.MultiTexCoord2f(0.0f, 0.0f, 1.0f);
        leftBatch.Vertex3f(-10.0f, 10.0f, z);
        
        leftBatch.MultiTexCoord2f(0.0f, 1.0f, 0.0f);
        leftBatch.Vertex3f(-10.0f, -10.0f, z - 10.0f);
        
        leftBatch.MultiTexCoord2f(0.0f, 1.0f, 1.0f);
        leftBatch.Vertex3f(-10.0f, 10.0f, z - 10.0f);
    }
    leftBatch.End();
    
    rightBatch.Begin(GL_TRIANGLE_STRIP, 28,1);
    for (z  =  200.0f; z >= 0; z -= 10.f) {
        // 纹理坐标
        rightBatch.MultiTexCoord2f(0.0f, 0.0f, 0.0f);
        // 顶点坐标
        rightBatch.Vertex3f(10.0f, -10.0f, z);
        
        rightBatch.MultiTexCoord2f(0.0f, 0.0f, 1.0f);
        rightBatch.Vertex3f(10.0f, 10.0f, z);
        
        rightBatch.MultiTexCoord2f(0.0f, 1.0f, 0.0f);
        rightBatch.Vertex3f(10.0f, -10.0f, z - 10.0f);
        
        rightBatch.MultiTexCoord2f(0.0f, 1.0f, 1.0f);
        rightBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);
    }
    rightBatch.End();
    
    ceilingBatch.Begin(GL_TRIANGLE_STRIP, 28,1);
    for (z  =  200.0f; z >= 0; z -= 10.f) {
        // 纹理坐标
        ceilingBatch.MultiTexCoord2f(0.0f, 0.0f, 0.0f);
        // 顶点坐标
        ceilingBatch.Vertex3f(-10.0f, 10.0f, z);
        
        ceilingBatch.MultiTexCoord2f(0.0f, 0.0f, 1.0f);
        ceilingBatch.Vertex3f(-10.0f, 10.0f, z - 10.f);
        
        ceilingBatch.MultiTexCoord2f(0.0f, 1.0f, 0.0f);
        ceilingBatch.Vertex3f(10.0f, 10.0f, z);
        
        ceilingBatch.MultiTexCoord2f(0.0f, 1.0f, 1.0f);
        ceilingBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);
    }
    ceilingBatch.End();
}

// 召唤场景
void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT);
    modelViewMatrix.PushMatrix();
    // Z轴平移viewZ距离
    modelViewMatrix.Translate(0.0f, 0.0f, viewZ);
    /*
     纹理替换矩阵着色器
     GLT_SHADER_TEXTURE_REPLACE（着色器标签）
     参数2:模型视图投影矩阵
     参数3:纹理层
     */
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE,transformPepeline.GetModelViewProjectionMatrix(),0);
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_FLOOR]);
    floorBatch.Draw();
    
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_BRICK]);
    leftBatch.Draw();
    
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_BRICK]);
    rightBatch.Draw();
    
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_CEILING]);
    ceilingBatch.Draw();
    
    // 5、pop出栈
    modelViewMatrix.PopMatrix();
    // 6、交换缓冲区
    glutSwapBuffers();
}

void SpecialKeys(int key,int x,int y){
    if (key == GLUT_KEY_UP) {
        viewZ += 0.5f;
    }
    if (key == GLUT_KEY_DOWN) {
        viewZ -= 0.5f;
    }
    glutPostRedisplay();
}

// 窗口已更改大小，或刚刚创建。无论哪种情况，我们都需要
// 使用窗口维度设置视口和投影矩阵.
void ChangeSize(int w, int h)
{
    glViewport(0, 0, w, h);
    
    GLfloat fAspect = GLfloat(w) / GLfloat(h);
    // 生成透视投影
    viewFrustum.SetPerspective(80.f, fAspect, 1.0, 120.0);
    projectMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    transformPepeline.SetMatrixStacks(modelViewMatrix, projectMatrix);
}

void ShutdownRC(){
    // 删除纹理
    glDeleteTextures(TEXTURE_COUNT, textures);
}

int main(int argc, char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    //申请一个颜色缓存区、深度缓存区、双缓存区、模板缓存区
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    //设置window 的尺寸
    glutInitWindowSize(800, 600);
    //创建window的名称
    glutCreateWindow("纹理-隧道");
    //注册回调函数（改变尺寸）
    glutReshapeFunc(ChangeSize);
    //显示函数
    glutDisplayFunc(RenderScene);
    
    glutSpecialFunc(SpecialKeys);
    
    // 添加菜单入口，改变过滤器
    glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("GL_NEAREST",0);
    glutAddMenuEntry("GL_LINEAR",1);
    glutAddMenuEntry("GL_NEAREST_MIPMAP_NEAREST",2);
    glutAddMenuEntry("GL_NEAREST_MIPMAP_LINEAR", 3);
    glutAddMenuEntry("GL_LINEAR_MIPMAP_NEAREST", 4);
    glutAddMenuEntry("GL_LINEAR_MIPMAP_LINEAR", 5);
    glutAddMenuEntry("Anisotropic Filter", 6);
    glutAddMenuEntry("Anisotropic Off", 7);
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    //判断一下是否能初始化glew库，确保项目能正常使用OpenGL 框架
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    //绘制
    SetupRC();
    //runloop运行循环
    glutMainLoop();
    ShutdownRC();
    
    return 0;
}
