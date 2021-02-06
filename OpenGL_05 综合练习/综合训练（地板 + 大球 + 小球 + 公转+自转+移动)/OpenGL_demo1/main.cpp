//
//  main.cpp
//  OpenGL_demo1
//
//  Created by baihuajun on 2020/3/27.
//  Copyright © 2020 baihuajun. All rights reserved.
//

#include "GLTools.h"
#include "GLBatch.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"
#include "GLFrustum.h"
#include "StopWatch.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif
GLShaderManager shaderManager;

GLBatch floorBatch; // 地板
GLGeometryTransform transformPipeLine;
GLFrustum viewFrustum;

GLMatrixStack projectMatrix; // 投影矩阵
GLMatrixStack modelViewMatrix;// 模型视图矩阵

GLTriangleBatch torusBatch;// 大球
GLTriangleBatch sphereBatch;// 小球
// 小球随机值
#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];
// 照相机 观察者
GLFrame cameraFrame;

void SetupRC()
{
    // 1. 初始化
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    shaderManager.InitializeStockShaders();
    // 2. 开启深度测试
    glEnable(GL_DEPTH_TEST);
    // 3. 设置地板顶点数据
    floorBatch.Begin(GL_LINES, 324);
    for (GLfloat x = -20.0; x <= 20.0f; x += 0.5f) {
        floorBatch.Vertex3f(x, -0.5f, 20.0f);
        floorBatch.Vertex3f(x, -0.5f, -20.0f);
        
        floorBatch.Vertex3f(20.0f, -0.5f, x);
        floorBatch.Vertex3f(-20.0f, -0.5f, x);
    }
    
    floorBatch.End();
    
    // 4.设置大球数据
    gltMakeSphere(torusBatch, 0.4f, 40, 80);
    // 5.设置小球数据
    gltMakeSphere(sphereBatch, 0.1f, 26, 13);
    // 小球位置随机
    // y轴不变，X，Z轴随机位置
    for (int i = 0; i < NUM_SPHERES; i ++) {
        
        GLfloat x = GLfloat((rand() % 400) - 200) * 0.1f;
        GLfloat z = GLfloat((rand() % 400) - 200) * 0.1f;
        // 对spheres数组中每一个球体设置顶点数据
        // 在y轴设置为0，使球体看起来像漂浮在眼睛高度
        spheres[i].SetOrigin(x, 0.0f, z);
    }
}

// 召唤场景
void RenderScene()
{
    static GLfloat floorColor[] = {0.0f,1.0f,0.0f,1.0f};
    static GLfloat vTorusColor[] = {1.0f,0.0f,0.0f,1.0f};
    static GLfloat vSphereColor[] = {0.0f,0.0f,1.0f,1.0f};
    // 基于时间的动画（自转/公转）
    static CStopWatch rotTimer;
    float yRot = rotTimer.GetElapsedSeconds() * 60.0f;
    
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 加入观察者
    M3DMatrix44f mCamara;
    cameraFrame.GetCameraMatrix(mCamara);
    modelViewMatrix.PushMatrix(mCamara);
    // 绘制地面
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeLine.GetModelViewProjectionMatrix(),floorColor);
    floorBatch.Draw();
    
    // 获取光源位置
    M3DVector4f vLightPosition = {0.0f,20.0f,10.0f,1.0f};
    // 使大球位置平移
    modelViewMatrix.Translate(0.0f, 0.0f, -3.0f);
    // 压栈
    modelViewMatrix.PushMatrix();
    // 大球自转
    modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
    shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,transformPipeLine.GetModelViewMatrix(),transformPipeLine.GetProjectionMatrix(),vLightPosition,vTorusColor);
    torusBatch.Draw();
    // 绘制完成 pop
    modelViewMatrix.PopMatrix();
    // 绘制小球
    for (int i = 0; i < NUM_SPHERES; i ++) {
        modelViewMatrix.PushMatrix();
        modelViewMatrix.MultMatrix(spheres[i]);
        shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,transformPipeLine.GetModelViewMatrix(),transformPipeLine.GetProjectionMatrix(),vLightPosition,vSphereColor);
        sphereBatch.Draw();
        modelViewMatrix.PopMatrix();
    }
    // 绘制一个小球围绕大球公转
    modelViewMatrix.Rotate(yRot * -2.0f, 0.0f, 1.0f, 0.0f);
    modelViewMatrix.Translate(0.8f, 0.0f, 0.0f);
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeLine.GetModelViewProjectionMatrix(),vSphereColor);
    sphereBatch.Draw();
    modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
    
    glutPostRedisplay();
}

// 窗口已更改大小，或刚刚创建。无论哪种情况，我们都需要
// 使用窗口维度设置视口和投影矩阵.
void ChangeSize(int w, int h)
{
    glViewport(0, 0, w, h);
    
    viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0, 100.f);
    
    projectMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    transformPipeLine.SetMatrixStacks(modelViewMatrix, projectMatrix);
}

void SpeacialKeys(int key,int x,int y){
    
    // 移动步长
    float step = 0.1f;
    //
    float angle = float(m3dDegToRad(5.0f));
    if (key == GLUT_KEY_UP) {
        cameraFrame.MoveForward(step);
    }
    if (key == GLUT_KEY_DOWN) {
        cameraFrame.MoveForward(-step);
    }
    if (key == GLUT_KEY_LEFT) {
        cameraFrame.RotateWorld(angle, 0.0f, 1.0f, 0.0f);
    }
    if (key == GLUT_KEY_RIGHT) {
        cameraFrame.RotateWorld(-angle, 0.0f, 1.0f, 0.0f);
    }
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
    glutCreateWindow("综合训练");
    //注册回调函数（改变尺寸）
    glutReshapeFunc(ChangeSize);
    //显示函数
    glutDisplayFunc(RenderScene);
    // 移动控制
    glutSpecialFunc(SpeacialKeys);
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
