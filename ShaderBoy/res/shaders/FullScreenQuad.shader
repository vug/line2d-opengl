#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

void main()
{
	gl_Position = position;
	v_TexCoord = texCoord;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform ivec2 u_Resolution;
uniform float u_Time;
uniform vec2 u_Mouse;

float capsule(float px, float py, float ax, float ay, float bx, float by, float r) {
	float pax = px - ax;
	float pay = py - ay;
	float bax = bx - ax;
	float bay = by - ay;
	float h = max(
		min(
			(pax * bax + pay * bay) / (bax * bax + bay * bay), 
			1.0f
		), 
		0.0f
	);
	float dx = pax - bax * h;
	float dy = pay - bay * h;
	return sqrt(dx * dx + dy * dy) - r;
}

float sample(float x, float y) {
	float W = u_Resolution.x;
	float H = u_Resolution.y;
	float PI = 3.14159265358f;
	float s = 0.0f;
	float cx = W * 0.5f;
	float cy = H * 0.5f;

	for (int j = 0; j < 5; j++) {
		float r1 = min(W, H) * (j + 0.5f) * 0.085f;
		float r2 = min(W, H) * (j + 1.5f) * 0.085f;
		float th = j * PI / 64.0f;
		float r = (j + 1) * 0.5f;
		float dir = j % 2 == 0 ? 1.0 : -1.0;
		for (int i = 0; i <= 64; i++, th += 2.0f * PI / 64.0f) {
			float t = sin(2.0 * u_Mouse.x / W * u_Time * dir / r) + 1.0;
			float ct = cos(th + t);
			float st = sin(th + t);
			s = max(
				s, 
				min(
					0.5f - capsule(x, y, cx + r1 * ct, cy - r1 * st, cx + r2 * ct, cy - r2 * st, r), 
					1.0f
				)
			);
		}
	}
	return s;
}

void main()
{
	vec2 uv = vec2(v_TexCoord);
	vec2 r = u_Resolution.x * uv;
	float val = 1.0 - sample(r.x, r.y);
	color = vec4(val, val, val, 1.0);
}
