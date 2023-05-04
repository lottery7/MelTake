#version 460 core

const float PI = 3.14159265359;
const float AA = 2.;

out vec4 frag_color;

uniform float u_time;
uniform vec2 u_resolution;
uniform float u_spectrum[40];
uniform int u_spectrum_size;
uniform float u_max_magnitude;
uniform float u_audio_position;

mat2 rotate(float ang) {
    return mat2(cos(ang), -sin(ang), sin(ang), cos(ang));
}

float scale(float x) {
    return sqrt(x) * 5e-5;
}

// p - coords of the middle point on the left side
float rect(vec2 uv, vec2 p, float w, float h, float a) {
    uv = rotate(a) * (uv - p) + p;
    p -= vec2(0, h) * .5;
    float blur = 0.002;
    float s1 = smoothstep(p.x, p.x+blur, uv.x);
    float s2 = smoothstep(p.x+w, p.x+w-blur, uv.x);
    float s3 = smoothstep(p.y, p.y+blur, uv.y);
    float s4 = smoothstep(p.y+h, p.y+h-blur, uv.y);
    return s1 * s2 * s3 * s4;
}

float angle_to_index(float ang, float offset, float repeats) {
    if (ang < offset) {
        ang += 2. * PI;
    }
    return (ang - offset) * (u_spectrum_size * repeats) / (2. * PI);
}

float index_to_angle(float i, float offset, float repeats) {
    return 2. * PI * floor(i) / (u_spectrum_size * repeats) + offset;
}

float vis_progress(float phi) {
    if (phi > PI / 2.) {
        phi -= 2. * PI;
    }

    phi = (PI / 2. - phi) / (2. * PI);
    if (phi <= u_audio_position) {
        return max(smoothstep(.0, .2, scale(u_max_magnitude)), .2);
    }
    return .0;
}

float vis_circle(vec2 uv, float radius, float offset, float repeats) {
    // scale args accordingly to beat
    float scale_factor = smoothstep(.0, .6, scale(u_max_magnitude)) + 1.;
    uv /= scale_factor;

    // convert coords to polar
    float r = length(uv);
    float phi = atan(uv.y, uv.x);
    
    // draw audio process
    if (.095 < r && r < radius - 0.015) {
        return vis_progress(phi);
    }
    
    // draw square
    float i = angle_to_index(phi, offset, repeats);
    float w = scale(u_spectrum[int(mod(i, u_spectrum_size))]);
    float h = .6 / u_spectrum_size;
    float ang = index_to_angle(floor(i), offset, repeats);
    vec2 p = vec2(radius * cos(ang), radius * sin(ang));
    float c = rect(uv, p, w, h, ang);
    float mask = smoothstep(.0, .2, w) * .95 + .05;

    return c * mask;
}

void main() {
    vec2 uv = (gl_FragCoord.xy - .5 * u_resolution) / u_resolution.y;

    float radius = .12;
    float offset = PI / 2.;
    float repeats = 2.;

    vec3 color = vec3(0);
    for (float i = .0; i < AA; i += 1.) {
        for (float j = .0; j < AA; j += 1.) {
            color += vec3(vis_circle(uv + vec2(i, j) / (AA * u_resolution.y), radius, offset, repeats));
        }
    }
    color /= AA * AA;

    // vec3 c1 = vec3(.07, .02, .35);
    // vec3 c2 = vec3(.47, .02, .92);
    // vec3 c3 = vec3(1., .46, 1.);
    // vec3 c4 = vec3(1., .0, .63);
    // vec3 c5 = vec3(1., .07, .31);

    frag_color = vec4(color, 1.);
}
