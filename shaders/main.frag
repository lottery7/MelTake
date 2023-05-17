#version 330 core
out vec4 frag_color;


/********** GLOBALS **********/

uniform float u_time;
uniform vec2 u_resolution;
uniform float u_spectrum[40];
uniform int u_spectrum_size;
uniform float u_max_magnitude;
uniform float u_audio_position;


const float PI = 3.14159265359;
const vec3 COLOR_PALETTE[] = {
    vec3(.07, .07, .23),
    vec3(.02, .85, .91),
    vec3(1., .16, .43)
};
const float BAR_WIDTH = .2;
const float BAR_HEIGHT = .5;
const float FADE_STRENGTH = .05;
float BLUR = .001;


/********** UTILITY FUNCTIONS **********/

// TRANSFORMATIONS

mat2 get_rotation_matrix(float ang) {
    return mat2(cos(ang), -sin(ang), sin(ang), cos(ang));
}

vec2 get_normalized_coords() {
    return (gl_FragCoord.xy - .5 * u_resolution) / min(u_resolution.x, u_resolution.y);
}

float get_scaled_magnitude(float x) {
    return smoothstep(.0, 4e2, pow(x, .3));
}

vec2 get_scaled_coords(vec2 uv) {
    uv -= vec2(.0, -.12 + .02 * sin(u_time));
    uv /= smoothstep(.0, .65, get_scaled_magnitude(u_max_magnitude)) + 1.;
    return uv;
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

// CONVERSIONS

vec3 magnitude_to_color(float magnitude) {
    return get_color(smoothstep(.02, .4, get_scaled_magnitude(magnitude)));
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


/********** RENDERING **********/

// VISUALIZATION

vec4 vis_sparkle(vec2 uv, vec2 p, float r) {
    float d = length(uv - p);
    return vec4(get_circle_color(), r / d);
}

vec4 vis_inner_circle(vec2 uv, float max_r, float w) {
    float r = length(uv);
    float s1 = smoothstep(max_r - w - BLUR, max_r - w, r);
    float s2 = smoothstep(max_r + BLUR, max_r - BLUR, r);
    float c = s1 * s2;
    return vec4(get_circle_color(), c);
}

// p - coords of the middle point on the left side
float vis_bar(vec2 uv, vec2 p, float w, float h, float a) {
    uv = get_rotation_matrix(a) * (uv - p) + p;
    p -= vec2(0, h / 2.);
    float fade = max(BLUR, FADE_STRENGTH * pow(u_max_magnitude * 3.2e-8, 4.));
    float s1 = smoothstep(p.x - BLUR, p.x + BLUR, uv.x);
    float s2 = smoothstep(p.x + w + fade, p.x + w - fade, uv.x);
    float s3 = smoothstep(p.y - BLUR, p.y + BLUR, uv.y);
    float s4 = smoothstep(p.y + h + BLUR, p.y + h - BLUR, uv.y);
    return s1 * s2 * s3 *s4;
}

vec4 vis_progress(vec2 uv, float max_r, float w) {
    // convert coords to polar
    float r = length(uv);
    float phi = atan(uv.y, uv.x);

    if (phi > PI / 2.) {
        phi -= 2. * PI;
    }
    phi = (PI / 2. - phi) / (2. * PI);
    float c = float(phi <= u_audio_position);
    float s1 = smoothstep(max_r - w - BLUR, max_r - w + BLUR, r);
    float s2 = smoothstep(max_r + BLUR, max_r, r);
    c *= s1 * s2;
    return vec4(get_circle_color(), c);
}

float vis_bar_from_index(vec2 uv, float radius, float offset, float index, out float magnitude) {
    magnitude = u_spectrum[
        int(u_spectrum_size - abs(index - u_spectrum_size))
    ];

    float w = get_scaled_magnitude(magnitude) * BAR_HEIGHT;
    float h = BAR_WIDTH / u_spectrum_size;

    float ang = index_to_angle(floor(index), offset);
    vec2 p = vec2(radius * cos(ang), radius * sin(ang));
    return vis_bar(uv, p, w, h, ang);
}

vec4 vis_bars(vec2 uv, float radius, float offset) {
    // Rotate to move first bar from the middle
    uv *= get_rotation_matrix(-.5 * PI / u_spectrum_size);

    // Convert coordinates to polar
    float r = length(uv);
    float phi = atan(uv.y, uv.x);

    // Draw bar
    // Current bar can overlap the next one, so we need to check both and return max.
    float index1 = angle_to_index(phi, offset);
    float magnitude1;
    float bar_mask1 = vis_bar_from_index(uv, radius, offset, index1, magnitude1);

    float index2 = mod(index1 + 1., 2. * u_spectrum_size);;
    float magnitude2;
    float bar_mask2 = vis_bar_from_index(uv, radius, offset, index2, magnitude2);


    if (bar_mask1 > bar_mask2) {
        return vec4(magnitude_to_color(magnitude1), bar_mask1);
    }

    return vec4(magnitude_to_color(magnitude2), bar_mask2);
}

// MAIN

vec3 render(vec2 uv) {
    // bars
    float radius = .1;
    vec4 bars_color = vis_bars(get_scaled_coords(uv), radius, PI / 2.);

    // circle
    float inner_circle_radius = radius - .01;
    float inner_circle_width = .006;
    vec4 inner_circle_color = vis_inner_circle(get_scaled_coords(uv), inner_circle_radius, inner_circle_width);

    // progress bar
    float progress_radius = inner_circle_radius - inner_circle_width;
    float progress_width = .004;
    vec4 progress_color = vis_progress(get_scaled_coords(uv), progress_radius, progress_width);
    
    // sparkle
    // vec2 sparkle_pos = vec2(.0, -1.);
    // vec4 sparkle_color = vis_sparkle(uv, sparkle_pos, .2* get_scaled_magnitude(u_max_magnitude));

    vec4 color = vec4(0);
    if (bars_color.a > color.a) color = bars_color;
    if (inner_circle_color.a > color.a) color = inner_circle_color;
    if (progress_color.a > color.a) color = progress_color;
    // if (sparkle_color.a > color.a) color = sparkle_color;

    // vec3 bg_color = mix(vec3(0), sparkle_color.rgb, sparkle_color.a);
    vec3 bg_color = vec3(0);
    color.rgb = mix(bg_color, color.rgb, color.a);
    return color.rgb;
}

void main() {
    vec2 uv = get_normalized_coords();
    frag_color = vec4(render(uv), 1.);
}
