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

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif
GLShaderManager shaderManager;

GLBatch floorBatch;
GLGeometryTransform transformPipeLine;
GLFrustum viewFrustum;

GLMatrixStack projectMatrix;
GLMatrixStack modelViewMatrix;


void SetupRC()
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    shaderManager.InitializeStockShaders();
    
    glEnable(GL_DEPTH_TEST);
    
    floorBatch.Begin(GL_LINES, 324);
    
    for (GLfloat x = -20.0; x <= 20.0f; x += 0.5f) {
        floorBatch.Vertex3f(x, -0.5f, 20.0f);
        floorBatch.Vertex3f(x, -0.5f, -20.0f);
        
        floorBatch.Vertex3f(20.0f, -0.5f, x);
        floorBatch.Vertex3f(-20.0f, -0.5f, x);
    }
    
    floorBatch.End();
    
}

// 召唤场景
void RenderScene()
{
    static GLfloat floorColor[] = {0.0f,1.0f,0.0f,1.0f};
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeLine.GetModelViewProjectionMatrix(),floorColor);
    
    floorBatch.Draw();
    glutSwapBuffers();
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
