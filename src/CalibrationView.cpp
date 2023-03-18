#include "CalibrationView.h"

#include "SDL.h"
#include "SDL_image.h"

#include "imgui.h"

#include "Eigen/Dense"

#include <math.h>
#include <algorithm>

CalibrationView::CalibrationView()
{
	if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0)
	{
		SDL_Log("SDL_image was not initalized");
	}

	memset(_irImageFilePath, 0, _pathBufferSize);
	memset(_eoImageFilePath, 0, _pathBufferSize);

	InitQuadVertices(_irQuadVertices);
	InitQuadVertices(_eoQuadVertices);

	SetIRImageAlpha(_irImageAlpha);

	const char* tmp_ir_image_path = "W:/sla/snapshots/snapshot19Sep2022_150245_315.jpeg";
	const char* tmp_eo_image_path = "W:/sla/snapshots/snapshot19Sep2022_150456_015.jpeg";

	SDL_strlcpy(_irImageFilePath, tmp_ir_image_path, _pathBufferSize);
	SDL_strlcpy(_eoImageFilePath, tmp_eo_image_path, _pathBufferSize);
}

CalibrationView::~CalibrationView()
{
	IMG_Quit();

	if (_irImage != nullptr)
	{
		SDL_DestroyTexture(_irImage);
	}

	if (_eoImage != nullptr)
	{
		SDL_DestroyTexture(_eoImage);
	}
}

int
SDL_RenderFillCircle1(SDL_Renderer* renderer, int x, int y, int radius)
{
	int offsetx, offsety, d;
	int status;

	offsetx = 0;
	offsety = radius;
	d = radius - 1;
	status = 0;

	while (offsety >= offsetx) {

		status += SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
			x + offsety, y + offsetx);
		status += SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
			x + offsetx, y + offsety);
		status += SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
			x + offsetx, y - offsety);
		status += SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
			x + offsety, y - offsetx);

		if (status < 0) {
			status = -1;
			break;
		}

		if (d >= 2 * offsetx) {
			d -= 2 * offsetx + 1;
			offsetx += 1;
		}
		else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}

	return status;
}

void CalibrationView::Render(SDL_Renderer* renderer)
{
	SDL_SetRenderTarget(renderer, _backBuffer);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	if (_eoImage != nullptr)
	{
		TransformQuadVertices(_transformedEOQuadVertices, _eoQuadVertices, _cameraOffset, _cameraScale);

		SDL_RenderGeometry(renderer, _eoImage, _transformedEOQuadVertices, 4, _quadIndices, 6);
	}

	if (_irImage != nullptr)
	{
		TransformQuadVertices(_transformedIRQuadVertices, _irQuadVertices, _cameraOffset, _cameraScale);

		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(_irImage, SDL_BLENDMODE_BLEND);

		SDL_RenderGeometry(renderer, _irImage, _transformedIRQuadVertices, 4, _quadIndices, 6);

		for (int i = 0; i < 4; i++)
		{
			SDL_SetRenderDrawColor(renderer, _irCircleColor[i].r, _irCircleColor[i].g,
				_irCircleColor[i].b, _irCircleColor[i].a);

			SDL_RenderFillCircle1(renderer, _transformedIRQuadVertices[i].position.x,
				_transformedIRQuadVertices[i].position.y, _irVertexRadius);
		}
	}

	SDL_SetRenderTarget(renderer, NULL);
}

void CalibrationView::LoadImage(SDL_Renderer* renderer, SDL_Texture*& textureToLoad, SDL_Vertex* vertices, 
	const char* filepath)
{
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	SDL_Texture* texture = IMG_LoadTexture(renderer, filepath);

	if (texture != nullptr)
	{
		if (textureToLoad != nullptr)
		{
			SDL_DestroyTexture(textureToLoad);
		}

		textureToLoad = texture;

		if (vertices != nullptr)
		{
			SDL_Point size;
			SDL_QueryTexture(textureToLoad, NULL, NULL, &size.x, &size.y);

			vertices[1].position.x = size.x;

			vertices[2].position.x = size.x;
			vertices[2].position.y = size.y;

			vertices[3].position.y = size.y;
		}
	}
}

void CalibrationView::InitQuadVertices(SDL_Vertex* vertices)
{
	vertices[0].position = { 0.0f, 0.0f };
	vertices[0].tex_coord = { 0.0f, 0.0f };
	vertices[0].color = { 255, 255, 255, 255 };

	vertices[1].position = { 1.0f, 0.0f };
	vertices[1].tex_coord = { 1.0f, 0.0f };
	vertices[1].color = { 255, 255, 255, 255 };

	vertices[2].position = { 1.0f, 1.0f };
	vertices[2].tex_coord = { 1.0f, 1.0f };
	vertices[2].color = { 255, 255, 255, 255 };

	vertices[3].position = { 0.0f, 1.0f };
	vertices[3].tex_coord = { 0.0f, 1.0f };
	vertices[3].color = { 255, 255, 255, 255 };
}

void CalibrationView::TransformQuadVertices(SDL_Vertex* dstVertices, SDL_Vertex* srcVertices, int2 cameraOffset, float cameraScale)
{
	SDL_memcpy(dstVertices, srcVertices, 4 * sizeof(SDL_Vertex));

	for (int i = 0; i < 4; i++)
	{
		dstVertices[i].position.x *= cameraScale;
		dstVertices[i].position.y *= cameraScale;

		dstVertices[i].position.x -= float(cameraOffset.x);
		dstVertices[i].position.y -= float(cameraOffset.y);
	}
}

void CalibrationView::SetIRImageAlpha(int alpha)
{
	_irQuadVertices[0].color.a = alpha;
	_irQuadVertices[1].color.a = alpha;
	_irQuadVertices[2].color.a = alpha;
	_irQuadVertices[3].color.a = alpha;
}

int CalibrationView::FindMouseOverIRVertex()
{
	auto distance = [](int2 v0, SDL_FPoint v1) 
	{ 
		return sqrtf(float((v0.x - v1.x) * (v0.x - v1.x) + (v0.y - v1.y) * (v0.y - v1.y))); 
	};

	for (int i = 0; i < 4; i++)
	{
		if (distance(_cursorPos - _cursorPosOffset, _transformedIRQuadVertices[i].position) < float(_irVertexRadius))
		{
			_irCircleColor[i] = { 0, 127, 0, 127 };
			return i;
		}
	}

	return -1;
}

void CalibrationView::SetIRCalibrationPoints(float2 calibrationPoints[4])
{
	for (int i = 0; i < 4; i++)
	{
		_irQuadVertices[i].position.x = calibrationPoints[i].x;
		_irQuadVertices[i].position.y = calibrationPoints[i].y;
	}

	_irCalibrationPointsIsSet = true;
}

void CalibrationView::GetIRCalibrationPoints(float2 outCalibrationPoints[4])
{
	for (int i = 0; i < 4; i++)
	{
		outCalibrationPoints[i].x = _irQuadVertices[i].position.x;
		outCalibrationPoints[i].y = _irQuadVertices[i].position.y;
	}
}

matrix3f CalibrationView::GetPerspectiveTransform(float2 src[4], float2 dst[4])
{
	Eigen::MatrixXf a(8, 8);
	Eigen::MatrixXf b(8, 1);

	for (uint32_t i = 0; i < 4; i++)
	{
		a(i, 0) = a(i + 4, 3) = src[i].x;
		a(i, 1) = a(i + 4, 4) = src[i].y;
		a(i, 2) = a(i + 4, 5) = 1.0f;
		a(i, 3) = a(i, 4) = a(i, 5) =
			a(i + 4, 0) = a(i + 4, 1) = a(i + 4, 2) = 0.0f;
		a(i, 6) = -src[i].x * dst[i].x;
		a(i, 7) = -src[i].y * dst[i].x;
		a(i + 4, 6) = -src[i].x * dst[i].y;
		a(i + 4, 7) = -src[i].y * dst[i].y;
		b(i) = dst[i].x;
		b(i + 4) = dst[i].y;
	}

	Eigen::MatrixXf x(8, 1);

	x = a.lu().solve(b);
	
	matrix3f m = matrix3f(x.data());
	m[8] = 1.0f;

	return m;
}

bool CalibrationView::GetIRCalibrationMatrix(matrix3f& outTransform, matrix3f& outInvTransform)
{
	if (_irImage == nullptr)
	{
		return false;
	}

	SDL_Point size;
	SDL_QueryTexture(_irImage, NULL, NULL, &size.x, &size.y);

	float2 inputPoints[4] =
	{
		float2(0, 0),
		float2(size.x, 0),
		float2(size.x, size.y),
		float2(0, size.y)
	};

	float2 outputPoints[4];
	GetIRCalibrationPoints(outputPoints);
	
	outTransform = GetPerspectiveTransform(inputPoints, outputPoints);
	outInvTransform = GetPerspectiveTransform(outputPoints, inputPoints);

	outTransform = outTransform.transpose();
	outInvTransform = outInvTransform.transpose();

	return true;
}

void CalibrationView::Show(bool& show, SDL_Renderer* renderer)
{
	// Init Circle colors
	for (int i = 0; i < 4; i++)
	{
		_irCircleColor[i] = {127, 0, 0, 127};
	}

	if (!_irVertexDrag && (_cursorPos.x >= 0 && _cursorPos.y >= 0))
	{
		_irVertexIdx = FindMouseOverIRVertex();
	}

	if (ImGui::IsMouseReleased(0))
	{
		_irVertexDrag = false;
		_irVertexIdx = -1;
	}

	if (ImGui::IsMouseDragging(2))
	{
		if (!_cameraDrag)
		{
			_cameraDrag = true;
			_cameraDragStartPosition = _cursorPos + _cameraOffset;
		}

		_cameraOffset = _cameraDragStartPosition - _cursorPos;
	}
	else
	{
		_cameraDrag = false;
	}

	ImGuiIO& io = ImGui::GetIO();

	if (io.MouseWheel != 0.0f)
	{
		_cameraWheelVelocity = io.MouseWheel * 0.3f;
	}

	if (_cameraWheelVelocity != 0.0f)
	{
		_cameraScale += _cameraWheelVelocity;

		_cameraScale = std::max(0.3f, _cameraScale);

		_cameraOffset.x += _cursorPos.x * _cameraWheelVelocity;
		_cameraOffset.y += _cursorPos.y * _cameraWheelVelocity;

		_cameraWheelVelocity *= 0.5f;

		if (abs(_cameraWheelVelocity) < 0.001f)
		{
			_cameraWheelVelocity = 0.0f;
		}
	}

	// Drag vertex
	if ((ImGui::IsMouseDown(0) && (_irVertexIdx != -1)) || _irVertexDrag)
	{
		_irVertexDrag = true;

		_irQuadVertices[_irVertexIdx].position.x = (_cursorPos.x + _cameraOffset.x - _cursorPosOffset.x) / _cameraScale;
		_irQuadVertices[_irVertexIdx].position.y = (_cursorPos.y + _cameraOffset.y - _cursorPosOffset.y) / _cameraScale;
	}

	ImVec2 viewOffset;

	if (ImGui::Begin("Camera calibration", &show))
	{
		if (ImGui::BeginTable("table", 2, ImGuiTableFlags_Borders))
		{
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(500.0f);
			if (ImGui::InputText("IR Camera Image File Path", _irImageFilePath, _pathBufferSize))
			{
				SDL_Vertex* vertices = _irCalibrationPointsIsSet ? nullptr : _irQuadVertices;
				LoadImage(renderer, _irImage, vertices, _irImageFilePath);
			}

			if (ImGui::InputText("EO Camera Image File Path", _eoImageFilePath, _pathBufferSize))
			{
				LoadImage(renderer, _eoImage, _eoQuadVertices, _eoImageFilePath);
			}

			if (ImGui::SliderInt("IR Image Alpha", &_irImageAlpha, 0, 255))
			{
				SetIRImageAlpha(_irImageAlpha);
			}

			ImGui::PushItemWidth(50.0f);

			ImGui::TableNextColumn();

			ImGui::InputFloat2("P0", &_irQuadVertices[0].position.x);
			ImGui::InputFloat2("P1", &_irQuadVertices[1].position.x);
			ImGui::InputFloat2("P2", &_irQuadVertices[2].position.x);
			ImGui::InputFloat2("P3", &_irQuadVertices[3].position.x);

			ImGui::PopItemWidth();
			ImGui::PopItemWidth();

			ImGui::EndTable();
		}

		Render(renderer);

		// Recreate backbuffer
		ImVec2 imWindowSize = ImGui::GetContentRegionAvail();

		imWindowSize.x = std::max(0.0f, imWindowSize.x - 10.0f);
		imWindowSize.y = std::max(0.0f, imWindowSize.y - 60.0f);

		if ((_backbufferSize.x != int(imWindowSize.x)) ||
			(_backbufferSize.y != int(imWindowSize.y)) ||
			(_backBuffer == nullptr))
		{
			if (_backBuffer != nullptr)
			{
				SDL_DestroyTexture(_backBuffer);
			}

			_backbufferSize.x = int(imWindowSize.x);
			_backbufferSize.y = int(imWindowSize.y);

			_backBuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
				SDL_TEXTUREACCESS_TARGET, _backbufferSize.x, _backbufferSize.y);
		}

		viewOffset = ImGui::GetCursorPos();

		ImGui::ImageButton(static_cast<void*>(_backBuffer), imWindowSize);
	}

	ImVec2 windowPos = ImGui::GetWindowPos();

	ImGui::End();

	ImVec2 cursorPos = ImGui::GetMousePos();

	cursorPos.x = cursorPos.x - windowPos.x - viewOffset.x;
	cursorPos.y = cursorPos.y - windowPos.y - viewOffset.y;

	_cursorPos.x = int(cursorPos.x);
	_cursorPos.y = int(cursorPos.y);
}
