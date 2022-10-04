/*
Raymarcher - Volumetrics
*/
#version 330 

#define MAX_STEPS 200
#define MAX_DIST 100.
#define SURF_DIST .001

in vec2 fragCoord;
out vec4 fragColor;

uniform vec2 iResolution;
uniform sampler3D volume_data;
uniform float time;

/*
Box raymarching shader
Test
*/
float sdBox(vec3 p, vec3 s) {
    p = abs(p)-s;
	return length(max(p, 0.))+min(max(p.x, max(p.y, p.z)) , 0.);
}
float GetDist(vec3 p) {
    float box = sdBox(p-vec3(0,0,0), vec3(1,1,1));
    
    
    float d = min(0.03, box);
    return d;
}

float RayMarch(vec3 ro, vec3 rd) {
	float dO=0.;
    
    for(int i=0; i<MAX_STEPS; i++) {
    	vec3 p = ro + rd*dO;
        float dS = GetDist(p);
        dO += dS;
        if(dO>MAX_DIST || dS<SURF_DIST) break;
    }
    
    return dO;
}
vec3 R(vec2 uv, vec3 p, vec3 l, float z) {
    vec3 f = normalize(l-p),
        r = normalize(cross(vec3(0,1,0), f)),
        u = cross(f,r),
        c = p+f*z,
        i = c + uv.x*r + uv.y*u,
        d = normalize(i-p);
    return d;
}

vec3 GetNormal(vec3 p) {
	float d = GetDist(p);
    vec2 e = vec2(.001, 0);
    
    vec3 n = d - vec3(
        GetDist(p-e.xyy),
        GetDist(p-e.yxy),
        GetDist(p-e.yyx));
    
    return normalize(n);
}

float GetLight(vec3 p) {
    vec3 lightPos = vec3(3, 5, 4);
    vec3 l = normalize(lightPos-p);
    vec3 n = GetNormal(p);
    
    float dif = clamp(dot(n, l)*.5+.5, 0., 1.);
    float d = RayMarch(p+n*SURF_DIST*2., l);
   // if(p.y<.01 && d<length(lightPos-p)) dif *= .5;
    
    return dif;
}
float SampleVolume(vec3 p, vec3 rd) {
float accumalate = 0.0;
    for(float i = 0; i < 0.5; i += 0.005)
    {
        vec4 sample = texture(volume_data,p + rd * i);
        accumalate += sample.x;
    }
    return accumalate;
}
void main()
{
    vec2 uv = (gl_FragCoord.xy-.5*iResolution.xy)/iResolution.y;
    vec3 ro = vec3(0.4,-0.5,-1.4);
    vec3 rd = R(uv, ro, vec3(0.,0,0), 1.);
    vec3 col = vec3(0.,0.,0.);
    float d = RayMarch(ro,rd);
    if(d<MAX_DIST) {
    	vec3 p = ro + rd * d;
        float sample = SampleVolume(p,rd) / 2;
    	col =  vec3(sample,sample,sample);
        
    }
    col = pow(col, vec3(.4));
    // Output to screen
    
 
   
    
   
    fragColor = vec4(col,1.);
}