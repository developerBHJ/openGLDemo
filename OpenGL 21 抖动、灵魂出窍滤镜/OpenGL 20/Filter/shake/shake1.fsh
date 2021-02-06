precision highp float;
uniform sampler2D Texture;
varying vec2 TextureCoordsVarying;
uniform float Time;

const float PI = 3.1415926;

void main(){
    // 每次抖动时长
    float duration = 0.7;
    // 最大缩放
    float maxScale = 1.2;
    // 颜色偏移量
    float offset = 0.02;
    // 进度(0,1)
    float progres = mod(Time,duration) / duration;
    // 颜色偏移值范围[0.0,0.02]
    vec2 offsetCoords = vec2(offset,offset) * progres;
    // 缩放范围[1.0,1.2]
    float scale = 1.0 + (maxScale - 1.0) * progres;
    // 缩放后的纹理坐标
    vec2 scaleTextureCoords = vec2(0.5,0.5) + (TextureCoordsVarying - vec2(0.5,0.5)) / scale;
    // 获取放大的颜色
    vec4 maskR = texture2D(Texture,scaleTextureCoords + offsetCoords);
    // 获取缩小的颜色
    vec4 maskB = texture2D(Texture,scaleTextureCoords - offsetCoords);
    // 原图颜色
    vec4 mask = texture2D(Texture,TextureCoordsVarying);
    // 颜色混合
    gl_FragColor = vec4(maskR.r,mask.g,maskB.b,mask.a);
}

