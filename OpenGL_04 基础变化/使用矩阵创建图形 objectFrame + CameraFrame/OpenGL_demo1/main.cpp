//
//  main.cpp
//  OpenGL_demo1
//
//  Created by baihuajun on 2020/3/27.
//  Copyright © 2020 baihuajun. All rights reserved.
//

#include "GLTools.h"
#include "GLBatch.h"
#include "GLFrame.h"
#include "GLMatrixStack.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"


#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLShaderManager shaderManager;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectViewMatrix;
// 观察者位置
GLFrame cameraFrame;
// 世界坐标位置
GLFrame objectFrame;
// 用来构造投影矩阵
GLFrustum viewFrustum;
// 三角形
GLTriangleBatch triangle;
// 球
GLTriangleBatch spereBatch;
// 环
GLTriangleBatch torusBatch;
// 圆柱
GLTriangleBatch cylinderBatch;
// 锥
GLTriangleBatch coneBatch;
// 磁盘
GLTriangleBatch diskBatch;

GLGeometryTransform transformPipeline;
M3DMatrix44f shadowMatrix;

GLfloat vGreen[] = {0.0f,1.0f,0.0f,1.0f};
GLfloat vBlack[] = {0.0f,0.0f,0.0f,1.0f};

int nStep = 0;

void SetupRC()
{
    glClearColor(0.7f, 0.7f, 0.7f, 1.0);
    shaderManager.InitializeStockShaders();
    
    glEnable(GL_DEPTH_TEST);
    
    cameraFrame.MoveForward(-15.0f);
    // 使用三角形批次类创建图形
    /*
     球体
     gltMakeSphere(GLTriangleBatch& sphereBatch, GLfloat fRadius, GLint iSlices, GLint iStacks);
     参数1: 三角形批次类
     参数2: 球体半径
     参数3: 从球体底部堆叠到顶部的三角形带的数量
     参数4: 围绕球体一圈排列的三角形对数
     一个对称性较好的球体，三角形对数是堆叠数量的2倍 iStacks = 2 * iSlices
     绘制球体都是围绕Z轴，+Z为顶部，-Z为底部
     */
    gltMakeSphere(spereBatch, 3.0f, 10, 20);
    /*
     环
     gltMakeTorus(GLTriangleBatch& torusBatch, GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor);
     参数1: 三角形批次类
     参数2: 圆环中心到外边缘的半径
     参数3: 圆环中心到内边缘的半径
     参数4: 沿着主半径的三角形数量
     参数5: 沿着内半径的三角形数量
     */
    gltMakeTorus(torusBatch, 3.0f, 0.75f, 15, 15);
    /*
     圆柱
     void gltMakeCylinder(GLTriangleBatch& cylinderBatch, GLfloat baseRadius, GLfloat topRadius, GLfloat fLength, GLint numSlices, GLint numStacks);
     参数1: 三角形批次类
     参数2: 圆柱底部半径
     参数3: 圆柱顶部半径
     参数4: 圆形长度
     参数5: 围绕Z轴的三角形数量
     参数6: 圆柱底部堆叠到顶部的三角形数量
     */
    gltMakeCylinder(cylinderBatch, 2.0f, 2.0f, 3.0f, 15, 2);
    /*
     锥体
     void gltMakeCylinder(GLTriangleBatch& cylinderBatch, GLfloat baseRadius, GLfloat topRadius, GLfloat fLength, GLint numSlices, GLint numStacks);
     参数1: 三角形批次类
     参数2: 底部半径
     参数3: 顶部半径
     参数4: 圆形长度
     参数5: 围绕Z轴的三角形数量
     参数6: 圆柱底部堆叠到顶部的三角形数量
     */
    gltMakeCylinder(coneBatch, 2.0f, 0.0f, 3.0f, 13, 2);
    
    /*
     磁盘
     void gltMakeDisk(GLTriangleBatch& diskBatch, GLfloat innerRadius, GLfloat outerRadius, GLint nSlices, GLint nStacks);
     参数1: 三角形批次类
     参数2: 内半径
     参数3: 外半径
     参数4: 围绕Z轴的三角形对的数量
     参数5: 圆盘外围到内围的三角形数量
     */
    gltMakeDisk(diskBatch, 1.5f, 3.0f, 13, 2);
    
}

void DrawWireFramedBatch(GLTriangleBatch *batch)
{
    //  绘制图形
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vGreen);
    batch -> Draw();
    // 绘制黑色边框
    // 开启多边形偏移
    glEnable(GL_POLYGON_OFFSET_LINE);
    // 将多边形背面设置为线框模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // 偏移
    glPolygonOffset(-1.0f, -1.0f);
    // 边框宽度
    glLineWidth(2.0f);
    // 开启混合功能
    glEnable(GL_BLEND);
    // 开启抗锯齿功能
    glEnable(GL_LINE_SMOOTH);
    // 设置颜色因子
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 绘制边框
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vBlack);
    batch -> Draw();
    
    // 恢复
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glDisable(GL_BLEND);
    glLineWidth(1.0f);
    glDisable(GL_LINE_SMOOTH);
}

// 召唤场景
void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // 模型视图压栈
    modelViewMatrix.PushMatrix();
    // 获取摄像头矩阵
    M3DMatrix44f mCamera;
    // 从cameraFrame中获取矩阵到mCamera
    cameraFrame.GetCameraMatrix(mCamera);
    // 矩阵相乘。模型视图矩阵堆栈的矩阵与mCamera矩阵相乘后存储到modelViewMatrix矩阵堆栈中
    modelViewMatrix.MultMatrix(mCamera);
    // 创建矩阵mObjectFrame
    M3DMatrix44f mObjectFrame;
    // 从ObjectFrame中获取矩阵到mObjectFrame
    objectFrame.GetMatrix(mObjectFrame);
    // 将modelViewMatrix中的矩阵与mObjectFrame相乘，存储到modelViewMatrix中
    modelViewMatrix.MultMatrix(mObjectFrame);
    
    //
    switch (nStep) {
        case 0:
            DrawWireFramedBatch(&spereBatch);
            break;
        case 1:
            DrawWireFramedBatch(&torusBatch);
            break;
        case 2:
            DrawWireFramedBatch(&cylinderBatch);
            break;
        case 3:
            DrawWireFramedBatch(&coneBatch);
            break;
        case 4:
            DrawWireFramedBatch(&diskBatch);
            break;
        default:
            break;
    }
    
    modelViewMatrix.PopMatrix();
    glutSwapBuffers();
}

// 窗口已更改大小，或刚刚创建。无论哪种情况，我们都需要
// 使用窗口维度设置视口和投影矩阵.
void ChangeSize(int w, int h)
{
    glViewport(0, 0, w, h);
    // 透视投影
    viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 500.0f);
    // 加载透视投影矩阵
    projectViewMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    // modelViewMatrix矩阵堆栈加载单元矩阵
    modelViewMatrix.LoadIdentity();
    // 通过GLGeometryTransform管理矩阵堆栈
    // 使用transformPipeline管道管理模型视图矩阵堆栈和投影矩阵堆栈
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectViewMatrix);
}

void SpecialKeys(int key, int w,int h)
{
    switch (key) {
        case GLUT_KEY_UP:
            objectFrame.RotateWorld(m3dDegToRad(-5.0), 1.0, 0.0, 0.0);
            break;
        case GLUT_KEY_DOWN:
            objectFrame.RotateWorld(m3dDegToRad(5.0), 1.0, 0.0, 0.0);
            break;
        case GLUT_KEY_LEFT:
            objectFrame.RotateWorld(m3dDegToRad(-5.0), 0.0, 1.0, 0.0);
            break;
        case GLUT_KEY_RIGHT:
            objectFrame.RotateWorld(m3dDegToRad(-5.0), 0.0, 1.0, 0.0);
            break;
        default:
            break;
    }
    glutPostRedisplay();
}


/// 点击空格切换绘制图形
/// @param key key description
/// @param x x description
/// @param y y description
void KeyPressFunc(unsigned char key, int x, int y)
{
    if (key == 32) {
        nStep ++;
        if (nStep > 4) {
            nStep = 0;
        }
    }
    switch (nStep) {
        case 0:
            glutSetWindowTitle("Sphere");
            break;
        case 1:
            glutSetWindowTitle("Torus");
            break;
        case 2:
            glutSetWindowTitle("Cylinder");
            break;
        case 3:
            glutSetWindowTitle("Cone");
            break;
        case 4:
            glutSetWindowTitle("Disk");
            break;
        default:
            break;
    }
    glutPostRedisplay();
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
    glutCreateWindow("矩阵创建图形");
    //注册回调函数（改变尺寸）
    glutReshapeFunc(ChangeSize);
    
    glutKeyboardFunc(KeyPressFunc);
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
    return 0;
}
