precision mediump float;
const float PI = 3.1415926;
uniform sampler2D Texture;
varying vec2 TextureCoordsVarying;

const float uD = 80.0;
const float uR = 0.4;

void main(){
    
    ivec2 ires = ivec2(512,512);
    float Res = float(ires.y);
    vec2 temp = TextureCoordsVarying;
    float Radius = Res * uR;
    vec2 xy = Res * temp;
    vec2 dxy = xy - vec2(Res / 2.0,Res / 2.0);
    float r = length(dxy);
    float attenValue = (1.0 - ((r / Radius) * (r / Radius)));
    float beta = atan(dxy.y,dxy.x) + radians(uD) * 2.0 * attenValue;
    if (r <= Radius) {
        xy = Res / 2.0 + r * vec2(cos(beta),sin(beta));
    }
    temp = xy / Res;
    vec3 irgb = texture2D(Texture,temp).rgb;
    gl_FragColor = vec4(irgb,1.0);
}

