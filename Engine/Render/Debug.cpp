#include "Debug.hpp"
#include "GL/glew.h"
#include "Shader/Shader.hpp"
#include <queue>

namespace Debug {
	enum DebugShape {
		DEBUG_LINE,
		DEBUG_BOX,
		DEBUG_PLANE,
		DEBUG_SPHERE,
		NUM_SHAPES
	};

	struct DrawCmd {
		DebugShape shape;
		vec4 color;
		float lineWidth;
	};

	struct LineCmd : DrawCmd {
		vec3 start;
		vec3 end;
	};

	struct MeshCmd : DrawCmd {
		mat4 transform;
	};

	struct Shape {
		uint32 vao = 0;
		uint32 vbo = 0;
		uint32 ibo = 0;
		uint32 indices = 0;
	};

	static Shape shape[NUM_SHAPES];

	static std::queue<DrawCmd *> commands;

	static Shader lineProgram;

	static Shader meshProgram;

	void InitLine() {
		glGenVertexArrays(1, &shape[DEBUG_LINE].vao);
		glBindVertexArray(shape[DEBUG_LINE].vao);

		glGenBuffers(1, &shape[DEBUG_LINE].vbo);
		glBindBuffer(GL_ARRAY_BUFFER, shape[DEBUG_LINE].vbo);
		// buffer some dummy data. We use uniforms for setting the positions and colors of the lines.
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float), nullptr, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), nullptr);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void InitBox() {
		constexpr float mesh[] =
		{
			0.5, -0.5, -0.5,
			0.5, -0.5, 0.5,
			-0.5, -0.5, 0.5,
			-0.5, -0.5, -0.5,
			0.5, 0.5, -0.5,
			0.5, 0.5, 0.5,
			-0.5, 0.5, 0.5,
			-0.5, 0.5, -0.5
		};

		constexpr int indices[] =
		{
			// edges
			0, 1,
			1, 2,
			2, 3,
			3, 0,
			4, 5,
			5, 6,
			6, 7,
			7, 4,
			0, 4,
			1, 5,
			2, 6,
			3, 7
		};

		glGenVertexArrays(1, &shape[DEBUG_BOX].vao);
		glBindVertexArray(shape[DEBUG_BOX].vao);

		glGenBuffers(1, &shape[DEBUG_BOX].vbo);
		glBindBuffer(GL_ARRAY_BUFFER, shape[DEBUG_BOX].vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mesh), mesh, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

		glGenBuffers(1, &shape[DEBUG_BOX].ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape[DEBUG_BOX].ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		shape[DEBUG_BOX].indices = 24;

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void InitPlane() {
		constexpr float mesh[] =
		{
			0.5, 0.0, -0.5,
			0.5, 0.0, 0.5,
			-0.5, 0.0, 0.5,
			-0.5, 0.0, -0.5,
		};

		constexpr int indices[] =
		{
			// edges
			0, 1,
			1, 2,
			2, 3,
			3, 0
		};

		glGenVertexArrays(1, &shape[DEBUG_PLANE].vao);
		glBindVertexArray(shape[DEBUG_PLANE].vao);

		glGenBuffers(1, &shape[DEBUG_PLANE].vbo);
		glBindBuffer(GL_ARRAY_BUFFER, shape[DEBUG_PLANE].vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mesh), mesh, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

		glGenBuffers(1, &shape[DEBUG_PLANE].ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape[DEBUG_PLANE].ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		shape[DEBUG_PLANE].indices = 8;

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void InitSphere() {
		constexpr float vertices[] =
		{
			0, -1, 0,
			0.425323, -0.850654, 0.309011,
			-0.162456, -0.850654, 0.499995,
			0.723607, -0.44722, 0.525725,
			0.850648, -0.525736, 0,
			-0.52573, -0.850652, 0,
			-0.162456, -0.850654, -0.499995,
			0.425323, -0.850654, -0.309011,
			0.951058, 0, 0.309013,
			-0.276388, -0.44722, 0.850649,
			0.262869, -0.525738, 0.809012,
			0, 0, 1,
			-0.894426, -0.447216, 0,
			-0.688189, -0.525736, 0.499997,
			-0.951058, 0, 0.309013,
			-0.276388, -0.44722, -0.850649,
			-0.688189, -0.525736, -0.499997,
			-0.587786, -0, -0.809017,
			0.723607, -0.44722, -0.525725,
			0.262869, -0.525738, -0.809012,
			0.587786, -0, -0.809017,
			0.587786, 0, 0.809017,
			-0.587786, 0, 0.809017,
			-0.951058, -0, -0.309013,
			0, -0, -1,
			0.951058, -0, -0.309013,
			0.276388, 0.44722, 0.850649,
			0.688189, 0.525736, 0.499997,
			0.162456, 0.850654, 0.499995,
			-0.723607, 0.44722, 0.525725,
			-0.262869, 0.525738, 0.809012,
			-0.425323, 0.850654, 0.309011,
			-0.723607, 0.44722, -0.525725,
			-0.850648, 0.525736, -0,
			-0.425323, 0.850654, -0.309011,
			0.276388, 0.44722, -0.850649,
			-0.262869, 0.525738, -0.809012,
			0.162456, 0.850654, -0.499995,
			0.894426, 0.447216, -0,
			0.688189, 0.525736, -0.499997,
			0.52573, 0.850652, -0,
			0, 1, -0,
		};

		constexpr int indices[] =
		{
			0, 1, 2,
			3, 1, 4,
			0, 2, 5,
			0, 5, 6,
			0, 6, 7,
			3, 4, 8,
			9, 10, 11,
			12, 13, 14,
			15, 16, 17,
			18, 19, 20,
			3, 8, 21,
			9, 11, 22,
			12, 14, 23,
			15, 17, 24,
			18, 20, 25,
			26, 27, 28,
			29, 30, 31,
			32, 33, 34,
			35, 36, 37,
			38, 39, 40,
			40, 37, 41,
			40, 39, 37,
			39, 35, 37,
			37, 34, 41,
			37, 36, 34,
			36, 32, 34,
			34, 31, 41,
			34, 33, 31,
			33, 29, 31,
			31, 28, 41,
			31, 30, 28,
			30, 26, 28,
			28, 40, 41,
			28, 27, 40,
			27, 38, 40,
			25, 39, 38,
			25, 20, 39,
			20, 35, 39,
			24, 36, 35,
			24, 17, 36,
			17, 32, 36,
			23, 33, 32,
			23, 14, 33,
			14, 29, 33,
			22, 30, 29,
			22, 11, 30,
			11, 26, 30,
			21, 27, 26,
			21, 8, 27,
			8, 38, 27,
			20, 24, 35,
			20, 19, 24,
			19, 15, 24,
			17, 23, 32,
			17, 16, 23,
			16, 12, 23,
			14, 22, 29,
			14, 13, 22,
			13, 9, 22,
			11, 21, 26,
			11, 10, 21,
			10, 3, 21,
			8, 25, 38,
			8, 4, 25,
			4, 18, 25,
			7, 19, 18,
			7, 6, 19,
			6, 15, 19,
			6, 16, 15,
			6, 5, 16,
			5, 12, 16,
			5, 13, 12,
			5, 2, 13,
			2, 9, 13,
			4, 7, 18,
			4, 1, 7,
			1, 0, 7,
			2, 10, 9,
			2, 1, 10,
			1, 3, 10,
		};


		glGenVertexArrays(1, &shape[DEBUG_SPHERE].vao);
		glBindVertexArray(shape[DEBUG_SPHERE].vao);

		glGenBuffers(1, &shape[DEBUG_SPHERE].vbo);
		glBindBuffer(GL_ARRAY_BUFFER, shape[DEBUG_SPHERE].vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

		glGenBuffers(1, &shape[DEBUG_SPHERE].ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape[DEBUG_SPHERE].ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		shape[DEBUG_SPHERE].indices = std::size(indices);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void Init() {
		//lineProgram = ShaderResource::Instance()->CreateProgram(
		//	"../Shaders/LineDraw.vs",
		//	"../Shaders/LineDraw.fs");
		//meshProgram = ShaderResource::Instance()->CreateProgram(
		//	"../Shaders/BasicShader.vs",
		//	"../Shaders/BasicShader.fs");
		lineProgram = ShaderManager::Get().Load("shaders/LineDraw.vert", "shaders/LineDraw.frag");
		meshProgram = ShaderManager::Get().Load("shaders/rasterizer.vert", "shaders/wireframe.frag");
		InitLine();
		InitBox();
		InitPlane();
		InitSphere();
	}

	void DrawLine(vec3 start, vec3 end, vec4 color, float lineWidth) {
		LineCmd *cmd = new LineCmd();

		cmd->shape = DEBUG_LINE;
		cmd->color = color;

		cmd->start = start;
		cmd->end = end;

		cmd->lineWidth = lineWidth;

		commands.push(cmd);
	}

	void DrawBox(vec3 pos, vec3 rot, vec3 extents, vec4 color, float lineWidth) {
		MeshCmd *cmd = new MeshCmd();

		cmd->shape = DEBUG_BOX;
		cmd->color = color;
		cmd->lineWidth = lineWidth;

		cmd->transform = translate(identity<mat4>(), pos) * scale(identity<mat4>(), extents);

		commands.push(cmd);
	}

	void DrawBox(mat4 transform, vec4 color, float lineWidth) {
		MeshCmd *cmd = new MeshCmd();

		cmd->shape = DEBUG_BOX;
		cmd->color = color;
		cmd->lineWidth = lineWidth;

		cmd->transform = transform;

		commands.push(cmd);
	}

	void DrawSphere(vec3 pos, float radius, vec4 color, float lineWidth) {
		MeshCmd *cmd = new MeshCmd();

		cmd->shape = DEBUG_SPHERE;
		cmd->color = color;
		cmd->lineWidth = lineWidth;

		cmd->transform = translate(identity<mat4>(), pos) * scale(identity<mat4>(), vec3(radius));

		commands.push(cmd);
	}

	void DrawPlane(vec3 pos, vec3 rot, vec2 extents, vec4 color, float lineWidth) {
		MeshCmd *cmd = new MeshCmd();

		cmd->shape = DEBUG_PLANE;
		cmd->color = color;
		cmd->lineWidth = lineWidth;

		//cmd->transform = translate(pos) * rotationaxis(rot) * scale(vec3(extents.x, 0, extents.y));

		commands.push(cmd);
	}

	void ClearQueue() {
		// Probably not the most efficient way of doing it
		while (!commands.empty()) {
			DrawCmd *cmd = commands.front();
			commands.pop();
			delete cmd;
		}
	}

	void RenderLine(DrawCmd *cmd, const mat4 &projView) {
		LineCmd *lineCmd = (LineCmd *) cmd;

		lineProgram.Bind();
		glBindVertexArray(shape[DEBUG_LINE].vao);

		const vec4 v0 = vec4(lineCmd->start.x, lineCmd->start.y, lineCmd->start.z, 1);
		const vec4 v1 = vec4(lineCmd->end.x, lineCmd->end.y, lineCmd->end.z, 1);

		lineProgram.SetValue("projView", projView);
		lineProgram.SetValue("v0pos", v0);
		lineProgram.SetValue("v1pos", v1);
		lineProgram.SetValue("color", lineCmd->color);

		glPolygonMode(GL_FRONT, GL_LINE);
		glLineWidth(lineCmd->lineWidth);

		glDrawArrays(GL_LINES, 0, 2);

		glPolygonMode(GL_FRONT, GL_FILL);
	}

	void RenderMesh(DrawCmd *cmd, const mat4 &projView) {
		MeshCmd *boxCmd = (MeshCmd *) cmd;

		//glUseProgram(meshProgram);
		meshProgram.Bind();
		glBindVertexArray(shape[boxCmd->shape].vao);

		meshProgram.SetValue("projView", projView);
		meshProgram.SetValue("model", boxCmd->transform);
		meshProgram.SetValue("color", boxCmd->color);

		glPolygonMode(GL_FRONT, GL_LINE);
		glLineWidth(boxCmd->lineWidth);

		glDrawElements(GL_LINES, shape[boxCmd->shape].indices, GL_UNSIGNED_INT, NULL);

		glPolygonMode(GL_FRONT, GL_FILL);
	}

	void RenderDebug(const mat4 &projView) {
		while (!commands.empty()) {
			DrawCmd *cmd = commands.front();
			commands.pop();

			switch (cmd->shape) {
				case DEBUG_LINE:
					RenderLine(cmd, projView);
					break;
				case DEBUG_BOX:
					RenderMesh(cmd, projView);
					break;
				default: break;
			}

			delete cmd;
		}
	}
}
