precision highp float;
uniform sampler2D Texture;
varying vec2 TextureCoordsVarying;
uniform float Time;

const float PI = 3.1415926;

void main(){
    // 每次闪白时长
    float duration = 0.5;
    // 时间周期[0.0,0.5]
    float time = mod(Time,duration);
    // 白色遮罩层
    vec4 whiteMask = vec4(1.0,1.0,1.0,1.0);
    // 振幅
    float amplitude = abs(sin(time * (PI / duration)));
    // 原图颜色
    vec4 mask = texture2D(Texture,TextureCoordsVarying);
    // 颜色混合 
    gl_FragColor = mask * (1.0 - amplitude) + whiteMask * amplitude;
}

