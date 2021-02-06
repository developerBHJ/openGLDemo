precision highp float;
uniform sampler2D Texture;
varying vec2 TextureCoordsVarying;
uniform float Time;

const float PI = 3.1415926;

// 随机数
float rand(float n){
    // fract(x) 返回x的小数部分
    return fract(sin(n) * 56578.88);
}

void main(){
    // 每次毛刺的时长
    float duration = 0.3;
    // 最大抖动
    float maxJitter = 0.05;
    // 红色颜色偏移量
    float colorOffset = 0.01;
    // 蓝色颜色偏移量
    float blueOffset = -0.025;
    // 时间周期（0.0，0.6）
    float time = mod(Time,duration * 2.0);
    // 振幅[0,1]
    float amplitude = max(sin(time * (PI / duration)),0.0);
    // 像素随机偏移(-1,1)
    float jitter = rand(TextureCoordsVarying.y) * 2.0 - 1.0;
    // 判断是否超出最大振幅 超出就给默认值 jitter * amplitude * 0.06
    bool needOffset = abs(jitter) < maxJitter * amplitude;
    // 计算x轴偏移量
    float textureX = TextureCoordsVarying.x + (needOffset ? jitter : (jitter * amplitude * 0.06));
    // 偏移后的纹理坐标
    vec2 textureCoord = vec2(textureX,TextureCoordsVarying.y);
    // 原图
    vec4 mask = texture2D(Texture,textureCoord);
    // 偏移后的颜色
    vec4 maskR = texture2D(Texture,textureCoord + vec2(colorOffset * amplitude,0.0));
    // 偏移后的颜色
    vec4 maskB = texture2D(Texture,textureCoord + vec2(blueOffset * amplitude,0.0));
    // 颜色混合
    gl_FragColor = vec4(maskR.r,mask.g,maskB.b,mask.a);
}
