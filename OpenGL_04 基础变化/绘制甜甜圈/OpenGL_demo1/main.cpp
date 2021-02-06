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


#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif
GLShaderManager shaderManager;
// 设置角色帧 作为相机
GLFrame viewFrame;
GLTriangleBatch torusBatch;
GLMatrixStack   modleViewMatrix;
GLMatrixStack   projectMatrix;
GLGeometryTransform   transformPipeline;
GLFrustum viewFrustum;

GLFrame cameraFrame;

// 标记背面剔除、深度测试
int isCull = 0;
int isDepth = 0;


void SetupRC()
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    shaderManager.InitializeStockShaders();
    
//    cameraFrame.MoveForward(-10.0f);
    viewFrame.MoveForward(10.0f);
    
    gltMakeTorus(torusBatch, 1.0, 0.3, 52, 26);
    glPointSize(4.0f);
    
}


// 召唤场景
void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
//    modleViewMatrix.PushMatrix();
//    M3DMatrix44f mCamera;
//    cameraFrame.GetCameraMatrix(mCamera);
//    modleViewMatrix.MultMatrix(mCamera);
//
//    M3DMatrix44f mObjectFrame;
//    viewFrame.GetCameraMatrix(mObjectFrame);
//    modleViewMatrix.MultMatrix(mObjectFrame);
    
    modleViewMatrix.PushMatrix(viewFrame);
    
    
    GLfloat vRed[] = {1.0f,0.0f,0.0f,1.0f};
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vRed);
    torusBatch.Draw();
    modleViewMatrix.PopMatrix();
    glutSwapBuffers();
}

// 窗口已更改大小，或刚刚创建。无论哪种情况，我们都需要
// 使用窗口维度设置视口和投影矩阵.
void ChangeSize(int w, int h)
{
    glViewport(0, 0, w, h);
    
    viewFrustum.SetPerspective(35.0, float(w) / float(h), 1.0f, 100.0f);
    
    projectMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    transformPipeline.SetMatrixStacks(modleViewMatrix, projectMatrix);
}

void SpecialKeys(int key,int w,int h){
    
    if (key == GLUT_KEY_UP) {
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
    }
    
    if (key == GLUT_KEY_DOWN) {
        viewFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);
    }
    
    if (key == GLUT_KEY_LEFT) {
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 0.0f, 1.0f, 0.0f);
    }
    
    if (key == GLUT_KEY_RIGHT) {
        viewFrame.RotateWorld(m3dDegToRad(5.0), 0.0f, 1.0f, 0.0f);
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
    glutCreateWindow("GL_POINTS");
    //注册回调函数（改变尺寸）
    glutReshapeFunc(ChangeSize);
    //显示函数
    glutDisplayFunc(RenderScene);
    
    glutSpecialFunc(SpecialKeys);
    
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
