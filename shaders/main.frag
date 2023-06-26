#version 330 core
out vec4 frag_color;


/********** GLOBALS **********/

uniform float u_time;
uniform vec2 u_resolution;
uniform float u_spectrum[64];
uniform int u_spectrum_size;
uniform float u_max_magnitude;
uniform float u_audio_position;


const float PI = 3.14159265359;
const float BRIGHTNESS = .7;
const vec3 COLOR_PALETTE[] = {
    vec3(.07, .07, .23) * (1. + BRIGHTNESS),
    vec3(.02, .85, .91) * (1. + BRIGHTNESS),
    vec3(1., .16, .43) * (1. + BRIGHTNESS)
};
const float BAR_WIDTH = .15;
const float BAR_HEIGHT = .2;
const float FADE_STRENGTH = .0;
float BLUR = .001;

struct {
    vec2 uv;
    vec2 scaled_uv;
    vec2 st;
    vec2 scaled_st;
    vec4 color;
} context;

/********** UTILITY FUNCTIONS **********/

// TRANSFORMATIONS

mat2 get_rotation_matrix(float ang) {
    return mat2(cos(ang), -sin(ang), sin(ang), cos(ang));
}

vec2 get_normalized_coords() {
    return (gl_FragCoord.xy - .5 * u_resolution) / min(u_resolution.x, u_resolution.y);
}

float scale_magnitude(float x) {
    return smoothstep(1., .26e4, pow(x, .4));
}

vec2 scale_coords(vec2 uv) {
    uv -= vec2(.0, -.12 + .02 * sin(u_time));
    uv /= smoothstep(.1, .2, scale_magnitude(u_max_magnitude)) + 1.8;
    return uv;
}

float get_scaled_blur() {
    return max(BLUR, FADE_STRENGTH * scale_magnitude(u_max_magnitude));
}

float get_magnitude(float index) {
    return scale_magnitude(u_spectrum[
        int(u_spectrum_size - abs(index - u_spectrum_size))
    ]);
}

// COLORS

vec3 get_color(float x  /* x in [0;1] */) {
    int n = COLOR_PALETTE.length() - 1;
    int index = int(x * n);
    return mix(COLOR_PALETTE[index], COLOR_PALETTE[index + 1], fract(x * n));
}

vec3 get_circle_color() {
    return get_color(.5 * sin(.3 * u_time) + .5);
}

// #define TEST

void context_add_color(vec3 color, float mask) {
#ifdef TEST
    context.color.rgb += color * mask;
    context.color.a = 1.;
#else
    if (context.color.a < mask) {
        // context.color.rgb += color * mask;
        // context.color.a = mask;
        context.color = vec4(color, mask);
    }
#endif
}
// CONVERSIONS

vec3 magnitude_to_color(float magnitude) {
    return get_color(smoothstep(.06, .48, magnitude));
}

float angle_to_index(float ang, float offset) {
    if (ang < offset) {
        ang += 2. * PI;
    }

    return (ang - offset) * u_spectrum_size / PI;
}

float index_to_angle(float i, float offset) {
    return PI * i / (u_spectrum_size) + offset;
}

vec2 to_polar(vec2 uv) {
    return vec2(length(uv), atan(uv.y, uv.x));
}

vec2 to_cartesian(vec2 st) {
    return st.s * vec2(cos(st.t), sin(st.t));
}


/********** RENDERING **********/

// VISUALIZATION

void draw_sparkle(vec2 p, float r, vec3 color) {
    float d = length(context.scaled_uv - p);
    float mask = clamp(r / d, .0, 1.);

    context_add_color(color, mask);
}

void draw_circle(float max_r, float w) {
    float r = context.scaled_st.s;
    float s1 = smoothstep(max_r - w - BLUR, max_r - w, r);
    float s2 = smoothstep(max_r + BLUR, max_r - BLUR, r);
    float mask = s1 * s2;

    context_add_color(get_circle_color(), mask);
}

void draw_progress_bar(float radius, float w) {
    float phi = context.scaled_st.t;

    if (phi > PI / 2.) {
        phi -= 2. * PI;
    }

    phi = (PI / 2. - phi) / (2. * PI);

    if (phi > u_audio_position) {
        return;
    }

    float s1 = smoothstep(radius - w - BLUR, radius - w + BLUR, context.scaled_st.s);
    float s2 = smoothstep(radius + BLUR, radius, context.scaled_st.s);
    float mask = s1 * s2;

    context_add_color(get_circle_color(), mask);
}

// st = (r, phi) - polar coordinates of the middle point on the left side
void draw_bar(vec2 st, float w, float h, vec3 color) {
    vec2 p = to_cartesian(st);
    vec2 uv = get_rotation_matrix(st.t) * (context.scaled_uv - p) + p;

    draw_sparkle(p, .001, color);

    p -= vec2(0, h / 2.);
    BLUR = get_scaled_blur();

    float s1 = smoothstep(p.x - BLUR, p.x + BLUR, uv.x);
    // float s2 = smoothstep(p.x + w + fade, p.x + w - fade, uv.x);
    float s2 = smoothstep(p.x + w + BLUR, p.x + w - BLUR, uv.x);
    float s3 = smoothstep(p.y - BLUR, p.y + BLUR, uv.y);
    float s4 = smoothstep(p.y + h + BLUR, p.y + h - BLUR, uv.y);
    float mask = s1 * s2 * s3 * s4;
    
    context_add_color(color, mask);
}

void draw_bar_from_angle(float radius, float offset, float angle) {
    float index = angle_to_index(angle, offset);
    float magnitude = get_magnitude(index);

    float w = magnitude * BAR_HEIGHT;
    float h = BAR_WIDTH / u_spectrum_size;
    float bar_angle = index_to_angle(floor(index), offset);

    draw_bar(vec2(radius, bar_angle), w, h, magnitude_to_color(magnitude));
}

void draw_bars(float radius, float offset) {
    // Current bar may overlap the next one, so we need to call both draw functions
    draw_bar_from_angle(radius, offset, context.scaled_st.t);
    draw_bar_from_angle(radius, offset, context.scaled_st.t + PI / u_spectrum_size);
}

// MAIN

void render() {
    // float eps = .0007;
    // if (abs(context.uv.x) < eps || abs(context.uv.y) < eps) {
    //     context.color = vec4(1, 0, 0, 1);
    //     return;
    // }

    float bars_radius = .14;
    draw_bars(bars_radius, PI / 2. * (1. + 1. / u_spectrum_size));

    float circle_radius = bars_radius - .01;
    float circle_width = .006;
    draw_circle(circle_radius, circle_width);

    float progress_radius = circle_radius - circle_width;
    float progress_width = .004;
    draw_progress_bar(progress_radius, progress_width);
    
    // sparkle
    // vec2 sparkle_pos = vec2(.0, -1.);
    // vec4 sparkle_color = vis_sparkle(uv, sparkle_pos, .2* scale_magnitude(u_max_magnitude));
    // if (sparkle_color.a > color.a) color = sparkle_color;

    // vec3 bg_color = mix(vec3(0), sparkle_color.rgb, sparkle_color.a);
    // vec3 bg_color = vec3(0);
    // color.rgb = mix(bg_color, color.rgb, color.a);
    // return color.rgb;
}

void construct_context() {
    context.uv = get_normalized_coords();
    context.scaled_uv = scale_coords(context.uv);
    context.st = to_polar(context.uv);
    context.scaled_st = to_polar(context.scaled_uv);
    context.color = vec4(0);
}

void main() {
    construct_context();
    render();
    frag_color = clamp(context.color, .0, 1.);
}
