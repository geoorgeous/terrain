#include <stdio.h>
#include <time.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define TERRAIN_CHUNK_DISTANCE 3

#define CAMERA_SPEED 2
#define CAMERA_SPEED_BOOST_MULTIPLIER 100
#define CAMERA_TURN_SPEED 1
#define CAMERA_MOUSE_MULTIPLIER 0.01f
#define CAMERA_VFOV 60

#define TARGET_FPS 60

#include "gl.h"
#include "glutils.h"
#include "logging.h"
#include "mathutils.h"
#include "platform.h"
#include "terrain.h"
#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"

// todo
// perlin noise for terrain detail
// textures!
// sample values from neighbouring chunks
// stitch lod seams
// texture blending

typedef struct ShaderProgram {
	GLuint glHandle;
} ShaderProgram_t;

typedef struct ApplicationState {
	GLuint bIsRunning;
	ShaderProgram_t shaderProgram;
} ApplicationState_t;

int setup_state(struct ApplicationState* state)
{
	const char * vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 a_Position;"
		"layout (location = 1) in vec3 a_Normal;"
		"layout (location = 2) in vec2 a_TexCoords;"
		"uniform mat4 u_ProjectionMatrix;"
		"uniform mat4 u_ViewMatrix;"
		"out vec3 vertexNormal;"
		"out vec2 vertexTexCoords;"
		"void main()"
		"{"
			"vertexNormal = a_Normal;"
			"vertexTexCoords = a_TexCoords;"
			"gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(a_Position, 1.0);"
		"}";

	const char * fragmentShaderSource = "#version 330 core\n"
		"uniform vec3 u_LightDirection;"
		"in vec3 vertexNormal;"
		"in vec2 vertexTexCoords;"
		"flat in uint vertexTerrain;"
		"out vec4 o_color;"
		"void main()"
		"{"
			"vec3 lightDirection = normalize(-u_LightDirection);"
			"float ambient = 0.1f;"
			"float diffuse = max(dot(vertexNormal, lightDirection), 0.0);"
			"o_color = vec4(vec3(1, 0, 0) * (ambient + diffuse), 1.0f);"
		"}";

	return gut_create_shader_program(vertexShaderSource, fragmentShaderSource, &state->shaderProgram.glHandle);
}

int main(int argc, char** argv)
{
	Vec3 cameraPosition = { 1, 10, 1 };
	Vec3 cameraForward = { 1, 0, -1 };
	Vec3 cameraUp = { 0, 1, 0 };

	HINSTANCE hInstance = GetModuleHandle(NULL);

	HWND hWnd = create_window(hInstance, WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World");
	HDC hDeviceContext = GetDC(hWnd);
	HGLRC hRenderingContext = init_opengl(hDeviceContext, 3, 3);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	ApplicationState_t state;
	if (!setup_state(&state))
	{
		LOGFATAL("Failed to setup application state.");
	}

	TerrainChunk chunk;
	terrain_create_chunk_mesh(&chunk);

	clock_t lastTickStart = clock();
	float elapsedSinceLastFrame = 1.0f / TARGET_FPS;

	int bIsRunning = TRUE;
	while (bIsRunning)
	{
        clock_t now = clock();
		float deltaTime = ((float) now - lastTickStart) / CLOCKS_PER_SEC;
        lastTickStart = now;

		platform_window_flush_input(hWnd);

		if (!platform_process_events())
			bIsRunning = FALSE;

		const WindowInputState * input = platform_get_window_input_state(hWnd);

		Vec3 cameraPositionOffset;
		mut_vec3_init(&cameraPositionOffset);

		if (input->keys[KEY_W].bIsDown)
			cameraPositionOffset.z += CAMERA_SPEED;
		if (input->keys[KEY_S].bIsDown)
			cameraPositionOffset.z -= CAMERA_SPEED;
		if (input->keys[KEY_A].bIsDown)
			cameraPositionOffset.x += CAMERA_SPEED;
		if (input->keys[KEY_D].bIsDown)
			cameraPositionOffset.x -= CAMERA_SPEED;
		if (input->keys[KEY_SPACE].bIsDown)
			cameraPositionOffset.y += CAMERA_SPEED;
		if (input->keys[KEY_LEFT_CONTROL].bIsDown)
			cameraPositionOffset.y -= CAMERA_SPEED;

		Vec3 cameraRight;
		mut_vec3_cross(&cameraRight, &cameraUp, &cameraForward);

		if (mut_vec3_magsq(&cameraPositionOffset) > 0)
		{
			mut_vec3_normalise(&cameraPositionOffset);
			mut_vec3_multiplyf(&cameraPositionOffset, deltaTime);
			if (input->keys[KEY_LEFT_SHIFT].bIsDown)
				mut_vec3_multiplyf(&cameraPositionOffset, CAMERA_SPEED_BOOST_MULTIPLIER);

			Vec3 offsetF, offsetR, offsetU;
			mut_vec3_multiplyfc(&offsetF, &cameraForward, cameraPositionOffset.z);
			mut_vec3_multiplyfc(&offsetR, &cameraRight, cameraPositionOffset.x);
			mut_vec3_multiplyfc(&offsetU, &cameraUp, cameraPositionOffset.y);
			mut_vec3_add(&cameraPosition, &offsetF);
			mut_vec3_add(&cameraPosition, &offsetR);
			mut_vec3_add(&cameraPosition, &offsetU);
		}

		float cameraPitchOffset = 0;
		float cameraYawOffset = 0;

		if (input->keys[KEY_LEFT].bIsDown)
			cameraYawOffset += CAMERA_TURN_SPEED * deltaTime;
		if (input->keys[KEY_RIGHT].bIsDown)
			cameraYawOffset -= CAMERA_TURN_SPEED * deltaTime;
		if (input->keys[KEY_UP].bIsDown)
			cameraPitchOffset -= CAMERA_TURN_SPEED * deltaTime;
		if (input->keys[KEY_DOWN].bIsDown)
			cameraPitchOffset += CAMERA_TURN_SPEED * deltaTime;

		if (input->mouse.buttons[MOUSE_BUTTON_LEFT].bIsDown)
		{
			if (input->mouse.deltaX != 0)
				cameraYawOffset -= (float)input->mouse.deltaX * CAMERA_MOUSE_MULTIPLIER;
			if (input->mouse.deltaY != 0)
				cameraPitchOffset += (float)input->mouse.deltaY * CAMERA_MOUSE_MULTIPLIER;
		}

		if (cameraPitchOffset != 0 || cameraYawOffset != 0)
		{
			Quaternion pitchRotation;
			mut_quat_from_axis_angle(&pitchRotation, &cameraRight, cameraPitchOffset);

			Quaternion yawRotation;
			mut_quat_from_axis_angle(&yawRotation, &cameraUp, cameraYawOffset);

			Quaternion pitchYawRotation;
			mut_quat_multiply(&pitchYawRotation, &pitchRotation, &yawRotation);
			mut_quat_multiply_vec3(&cameraForward, &pitchYawRotation, &cameraForward);	
		}

		elapsedSinceLastFrame += deltaTime;
		if (elapsedSinceLastFrame < 1.0f / TARGET_FPS)
			continue;

		elapsedSinceLastFrame = 0;

		Mat4 view;
		Vec3 lookat;
		mut_vec3_addc(&lookat, &cameraPosition, &cameraForward);
		mut_mat4_lookat(&view, &cameraPosition, &lookat, &cameraUp);
		
		Mat4 projection;
		mut_mat4_perspective(&projection, mut_radians(CAMERA_VFOV), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 1000.0f);

		Vec3 lightDirection = { 1.0f, -1.0f, 1.0f };

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(state.shaderProgram.glHandle);

		gut_set_shader_uniform(state.shaderProgram.glHandle, GL_FLOAT_MAT4, "u_ProjectionMatrix", projection.data);
		gut_set_shader_uniform(state.shaderProgram.glHandle, GL_FLOAT_MAT4, "u_ViewMatrix", view.data);
		gut_set_shader_uniform(state.shaderProgram.glHandle, GL_FLOAT_VEC3, "u_LightDirection", lightDirection.data);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		mesh_draw_indexed(&chunk.mesh);
		
		SwapBuffers(hDeviceContext);
	}

	return 0;
}