precision highp float;
uniform sampler2D Texture;
varying vec2 TextureCoordsVarying;
uniform float Time;

void main(){
    // 每次灵魂出窍效果时长
    float duration = 0.7;
    // 透明度上限
    float maxAlpha = 0.4;
    // 图片放大的上限
    float maxScale = 1.8;
    // 进度值[0,1]
    float progress = mod(Time,duration) / duration;
    // 透明度[0,0.4]
    float alpha = maxAlpha * (1.0 - progress);
    // 缩放范围[1.0,1.8]
    float scale = 1.0 + (maxScale - 1.0) * progress;
    // 放大后的纹理坐标
    float weakX = 0.5 + (TextureCoordsVarying.x - 0.5) / scale;
    float weakY = 0.5 + (TextureCoordsVarying.y - 0.5) / scale;

    vec2 weakTextureCoord = vec2(weakX,weakY);
    // 放大后的颜色
    vec4 weakMask = texture2D(Texture,weakTextureCoord);
    // 原图颜色
    vec4 mask = texture2D(Texture,TextureCoordsVarying);
    // 颜色混合
    gl_FragColor = mask * (1.0 - alpha) + weakMask * alpha;
}

