//
//  main.cpp
//  OpenGL_demo1
//
//  Created by baihuajun on 2020/3/27.
//  Copyright © 2020 baihuajun. All rights reserved.
//

#include "GLTools.h"
#include "GLBatch.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif
/*
 矩阵变换
 利用矩阵实现图形的平移、旋转、综合变化等
 */

GLShaderManager shaderManager;
GLBatch squareBatch;

GLfloat blockSize = 0.1f;
GLfloat vVerts[] = {
    -blockSize,-blockSize,0.0f,
    blockSize,-blockSize,0.0f,
    blockSize,blockSize,0.0f,
    -blockSize,blockSize,0.0f
};

GLfloat xPos = 0.0f;
GLfloat yPos = 0.0f;

void SetupRC()
{
    // 初始化
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    shaderManager.InitializeStockShaders();
    // 加载图形
    squareBatch.Begin(GL_TRIANGLE_FAN, 4);
    squareBatch.CopyVertexData3f(vVerts);
    squareBatch.End();
}

// 召唤场景
void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLfloat vGreen[] = {1.0f,0.0f,0.0f,1.0f};
    // 定义三个变量，用于保存变换后的
    M3DMatrix44f mFinalTransform,mTranslationMatrix,mRotationMatrix;
    // 平移
    m3dTranslationMatrix44(mTranslationMatrix, xPos, yPos, 0.0f);
    // 每次重新绘制时旋转5度
    static float yRotation = 0.0f;
    yRotation += 5.0f;
    m3dRotationMatrix44(mRotationMatrix, m3dDegToRad(yRotation), xPos, yPos, 1.0f);
    // 将平移和旋转的结果合并到mFinalTransform中
    m3dMatrixMultiply44(mFinalTransform, mTranslationMatrix, mRotationMatrix);
    // 将矩阵结果提交到固定着色器
    shaderManager.UseStockShader(GLT_SHADER_FLAT,mFinalTransform,vGreen);
    squareBatch.Draw();
    // 执行缓冲区交换
    glutSwapBuffers();
}

/*
 键盘控制
 移动
 计算x,y
 边界检测
 */
void SpecialKeys(int key, int w, int h){
    // 步长
    GLfloat stepSize = 0.025f;
    if (key == GLUT_KEY_UP) {
        yPos += stepSize;
    }
    if (key == GLUT_KEY_DOWN) {
        yPos -= stepSize;
    }
    if (key == GLUT_KEY_LEFT) {
        xPos -= stepSize;
    }
    if (key == GLUT_KEY_RIGHT) {
        xPos += stepSize;
    }
    // 边界检测
    if (xPos < (-1.0f + blockSize)) {
        xPos = -1.0f + blockSize;
    }
    if (xPos > (1.0f - blockSize)) {
        xPos = 1.0f - blockSize;
    }
    if (yPos < (-1.0f + blockSize)) {
        yPos = -1.0f + blockSize;
    }
    
    if (yPos > (1.0f - blockSize)) {
        yPos = 1.0f - blockSize;
    }
    glutPostRedisplay();
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
    glutCreateWindow("GL_POINTS");
    //注册回调函数（改变尺寸）
    glutReshapeFunc(ChangeSize);
    //显示函数
    glutDisplayFunc(RenderScene);
    // 特殊按钮事件
    glutSpecialFunc(SpecialKeys);
    
    //判断一下是否能初始化glew库，确保项目能正常使用OpenGL 框架
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    // 绘制
    SetupRC();
    //runloop运行循环
    glutMainLoop();
    return 0;
}
