//
//  main.cpp
//  OpenGL_demo1
//
//  Created by baihuajun on 2020/3/27.
//  Copyright © 2020 baihuajun. All rights reserved.
//

/*
 OpenGL 学习第一天
 绘制一个完整的三角形
 */
#include "GLTools.h"
#include "GLBatch.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLShaderManager shaderManager;
/*
 三角形批次类
 可以传输顶点、光照、纹理、颜色数据等到存储着色器中
 */
// 容器类
GLBatch triangleBatch;// 三角形
GLBatch triangleBatch1;// 三角形
GLBatch triangleBatch2;// 三角形

GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat vBlue[] = { 0.0f, 0.0f, 1.0f, 1.0f };

void SetupRC()
{
    // 设置背景色
    glClearColor(0.8, 0.7, 0.5, 1.0);
    shaderManager.InitializeStockShaders();
    // 指定顶点
    GLfloat vVerts[] = { // 正三角
        -0.95f,0.0f,0.0f,
        -0.5f,0.45f,0.0f,
        -0.05f,0.0f,0.0f
    };
    triangleBatch.Begin(GL_TRIANGLES, 3);
    triangleBatch.CopyVertexData3f(vVerts);
    triangleBatch.End();
    
    // 指定顶点
    GLfloat vVerts1[] = { // 直角三角形
        0.1f,0.0f,0.0f,
        0.1f,0.6f,0.0f,
        0.9,0.0f,0.0f
    };
    triangleBatch1.Begin(GL_TRIANGLES, 3);
    triangleBatch1.CopyVertexData3f(vVerts1);
    triangleBatch1.End();
    
    // 指定顶点
    GLfloat vVerts2[] = { // 普通三角形
        -0.5f,-0.8f,0.0f,
        0.4f,-0.8f,0.0f,
        0.0f,-0.2f,0.0f
    };
    triangleBatch2.Begin(GL_TRIANGLES, 3);
    triangleBatch2.CopyVertexData3f(vVerts2);
    triangleBatch2.End();
}

// 召唤场景
void RenderScene()
{
    /*
     清楚一块或一组缓冲区
     缓冲区是一块存储图像信息的存储空间
     GL_COLOR_BUFFER_BIT 颜色缓冲区
     GL_DEPTH_BUFFER_BIT 深度缓冲区
     GL_STENCIL_BUFFER_BIT 模版缓冲区
     */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //  传递到存储着色器
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vRed);
    // 提交着色器
    triangleBatch.Draw();
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vGreen);
    triangleBatch1.Draw();
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vBlue);
    triangleBatch2.Draw();
    // 将后台缓冲区进行渲染，结束后交换缓冲区给前台
    glutSwapBuffers();
}

// 窗口已更改大小，或刚刚创建。无论哪种情况，我们都需要
// 使用窗口维度设置视口和投影矩阵.
void ChangeSize(int w, int h)
{
    glViewport(0, 0, w, h);
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
    glutCreateWindow("GL_TRIANGLES");
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
