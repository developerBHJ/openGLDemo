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

GLShaderManager shaderManager;
GLMatrixStack modleViewMatrixStack;
GLMatrixStack projectMatrixStack;
GLFrame cameraFrame;
GLFrame objectFrame;
GLFrustum viewFrustum;
GLGeometryTransform transformPipeline;
M3DMatrix44f shadowMatrix;
GLBatch pyramidBatch;

// 纹理变量 一般用无符号整型
GLuint textureID;


/// 创建金字塔
/// @param pyramidBatch pyramidBatch description
void MakePyramid(GLBatch &pyramidBatch){
    
    // 1 通过pyramidBatch组件三角形批次
    /*
     参数1:类型
     参数2:顶点数
     参数3:这个批次中将会应用一个纹理，默认为0
     */
    pyramidBatch.Begin(GL_TRIANGLES, 18,1);
    /*
     1)设置法线
     // 添加一个表面法线 （法线坐标与Vertex顶点坐标中的Y轴一致）
     void Normal3f(GLfloat x, GLfloat y, GLfloat z);
     注：表面法线是一个向量，代表表面或者顶点面对的方向（相反的方向）；在多数的光照模式下是必须使用的
     pyramidBatch.Normal3f(X,Y,Z);
     2)设置纹理坐标
     // texture : 纹理层次
     //  s :对应顶点坐标中的X轴
     //  t :对应顶点坐标中的Y轴
     // (s,t,r,q) 对应顶点坐标的(x,y,z,w)
     void MultiTexCoord2f(GLuint texture, GLclampf s, GLclampf t);
     
     pyramidBatch.MultiTexCoord2f(0,s,t);
     3)设置顶点数据
     void Vertex3f(GLfloat x, GLfloat y, GLfloat z);
     void Vertex3fv(M3DVector3f vVertex);
     pyramidBatch.Vertex3f(-1.0f, -1.0f, -1.0f);
     4)获取法线
     void m3dFindNormal(result,point1, point2,point3);
     */
    
    // 塔顶
    M3DVector3f vApex = {0.0f,1.0f,0.0f};
    M3DVector3f vFrontLeft = {-1.0f,-1.0f,1.0f};
    M3DVector3f vFrontRight = {1.0f,-1.0f,1.0f};
    M3DVector3f vBackLeft = {-1.0f,-1.0f,-1.0f};
    M3DVector3f vBackRight = {1.0f,-1.0f,-1.0f};
    M3DVector3f n;
    
    // 底部 (三角形X + 三角形Y)
    // 找法线（三角形X）
    m3dFindNormal(n, vBackLeft, vBackRight, vFrontRight);
    
    // vBackLeft
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    // vBackRight
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    // vFrontRight
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    // 找法线（三角形Y）
    m3dFindNormal(n, vFrontLeft, vBackLeft, vFrontRight);
    
    // vFrontLeft
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);
    
    // vBackLeft
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    // vFrontRight
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    // 金字塔前面
    m3dFindNormal(n, vApex, vFrontLeft, vFrontRight);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    // 金字塔左边
    m3dFindNormal(n, vApex, vBackLeft, vFrontLeft);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);
    // 金字塔右边
    m3dFindNormal(n, vApex, vFrontRight, vBackRight);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    // 金字塔后边
    m3dFindNormal(n, vApex, vBackRight, vBackLeft);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    // 结束设置
    pyramidBatch.End();
    
}

/// 从TGA文件加载为2D纹理
/// @param fileName 纹理文件名
/// @param minFilter minFilter description
/// @param magFileter maxFileter description
/// @param wrapModel 环绕模式
bool LoadTGATexture(const char *fileName,GLenum minFilter,GLenum magFileter,GLenum wrapModel)
{
    GLbyte *pBytes;
    int nWidth,nHeight,nComponents;
    GLenum eformat;
    /*
     读取纹理
     1、纹理文件名称
     2、文件宽度地址
     3、文件高度地址
     4、文件组件地址
     5、文件格式地址
     pBytes ：指向图像数据的指针
     */
    pBytes = gltReadTGABits(fileName, &nWidth, &nHeight, &nComponents, &eformat);
    if (pBytes == NULL) {
        return false;
    }
    /// 2、设置纹理参数
    /*
     参数1: 纹理温度
     参数2: S/T坐标设置模式
     参数3: 环绕模式
     */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapModel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapModel);
    // 设置过滤方式
    /*
     参数1: 纹理维度
     参数2: 线性过滤
     */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFileter);
    /// 3、载入纹理
    /*
     参数1: 纹理维度
     参数2: mip贴图层次
     参数3: 纹理单元存储的颜色成分
     参数4: 加载纹理宽
     参数5: 加载纹理高
     参数6: 加载纹理的深度
     参数7: 像数数据的类型（GL_UNSIGNED_BYTE，每个颜色分量都是一个8位无符号整数）
     参数8: 指向纹理图像数据的指针
     */
    glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0, eformat, GL_UNSIGNED_BYTE, pBytes);
    /// 4、使用完释放
    free(pBytes);
    /// 5、加载mip
    glGenerateMipmap(GL_TEXTURE_2D);
    return true;
}

void SetupRC()
{
    glClearColor(0.7, 0.7, 0.7, 1);
    shaderManager.InitializeStockShaders();
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    // 1、分配纹理对象 参数1:纹理对象个数 参数2:纹理对象指针
    glGenTextures(1, &textureID);
    // 绑定纹理状态 参数1:纹理状态2D 参数2:纹理对象
    glBindTexture(GL_TEXTURE_2D, textureID);
    // 2、加载纹理
    LoadTGATexture("stone.tga", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR, GL_CLAMP_TO_EDGE);
    // 3、创建金字塔
    MakePyramid(pyramidBatch);
    // 4、设置观察者视角
    cameraFrame.MoveForward(-10);
}

// 清理…例如删除纹理对象
void ShutdownRC(void)
{
    glDeleteTextures(1, &textureID);
}

// 召唤场景
void RenderScene()
{
    // 光源位置
    static GLfloat vLightPos[] = {1.0f,1.0f,0.0f};
    static GLfloat vWhite[] = {1.0f,1.0f,1.0f,1.0f};
    // 1、清理缓存
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // 2、当前模型视频压栈
    modleViewMatrixStack.PushMatrix();
    // 3、添加相机矩阵
    M3DMatrix44f mCamera;
    // 从cameraFrame中获取一个4*4的矩阵
    cameraFrame.GetCameraMatrix(mCamera);
    // 矩阵乘以矩阵栈顶矩阵,相乘结果存到矩阵栈顶, 将相机矩阵和当前矩阵相乘压入栈顶
    modleViewMatrixStack.MultMatrix(mCamera);
    // 4、创建objectFrame矩阵
    M3DMatrix44f mObjectFrame;
    // 从objectFrame获取矩阵，objectFrame保存的是特殊键位的变换矩阵
    objectFrame.GetMatrix(mObjectFrame);
    // 矩阵相乘，结果存入栈顶
    modleViewMatrixStack.MultMatrix(mObjectFrame);
    
    // 5、绑定纹理
    glBindTexture(GL_TEXTURE_2D, textureID);
    /*
     1、GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF 着色器标签
     2、模型视图矩阵
     3、投影矩阵
     4、视点坐标系中的光源位置
     5、基本漫反色颜色
     6、图形颜色（使用纹理就不需要颜色，设为0）
     */
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vLightPos,vWhite,0);
    // 6、绘制
    pyramidBatch.Draw();
    // 7、模型视图出栈
    modleViewMatrixStack.PopMatrix();
    // 8、交换缓冲区
    glutSwapBuffers();
}

void SpecialKeys(int key, int x, int y)
{
    if(key == GLUT_KEY_UP)
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_DOWN)
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_LEFT)
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    
    if(key == GLUT_KEY_RIGHT)
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    
    glutPostRedisplay();
}



// 窗口已更改大小，或刚刚创建。无论哪种情况，我们都需要
// 使用窗口维度设置视口和投影矩阵.
void ChangeSize(int w, int h)
{
    glViewport(0, 0, w, h);
    
    viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 500.f);
    // 获取viewFrustum投影矩阵 并将其加载到投影矩阵堆栈上
    projectMatrixStack.LoadMatrix(viewFrustum.GetProjectionMatrix());
    // 设置变换管道以使用两个矩阵堆栈
    //初始化GLGeometryTransform 的实例transformPipeline.通过将它的内部指针设置为模型视图矩阵堆栈 和 投影矩阵堆栈实例，来完成初始化
    //当然这个操作也可以在SetupRC 函数中完成，但是在窗口大小改变时或者窗口创建时设置它们并没有坏处。而且这样可以一次性完成矩阵和管线的设置。
    transformPipeline.SetMatrixStacks(modleViewMatrixStack, projectMatrixStack);
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
    glutCreateWindow("金字塔（纹理）");
    //注册回调函数（改变尺寸）
    glutReshapeFunc(ChangeSize);
    
    glutSpecialFunc(SpecialKeys);
    //显示函数
    glutDisplayFunc(RenderScene);
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
