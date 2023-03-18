#pragma once

#include "VideoFrame/Common.h"

#include "SDL.h"

class CalibrationView
{
public:
	CalibrationView();
	~CalibrationView();

	void SetIRCalibrationPoints(float2 calibrationPoints[4]);
	void GetIRCalibrationPoints(float2 outCalibrationPoints[4]);

	bool GetIRCalibrationMatrix(matrix3f& outTransform, matrix3f& outInvTransform);

	void Show(bool& show, SDL_Renderer* renderer);
protected:
	void Render(SDL_Renderer* renderer);
	void LoadImage(SDL_Renderer* renderer, SDL_Texture*& textureToLoad, SDL_Vertex* vertices, 
		const char* filepath);
	
	void InitQuadVertices(SDL_Vertex* vertices);
	void TransformQuadVertices(SDL_Vertex* dstVertices, SDL_Vertex* srcVertices, int2 cameraOffset, float cameraScale);

	void SetIRImageAlpha(int alpha);
	int FindMouseOverIRVertex();

	matrix3f GetPerspectiveTransform(float2 src[4], float2 dst[4]);

protected:
	int _irImageAlpha = 127;
	int _irVertexRadius = 6;

	int _irVertexIdx = -1;
	bool _irVertexDrag = false;
	bool _irCalibrationPointsIsSet = false;

	bool _cameraDrag = false;
	int2 _cameraDragStartPosition;

	int2 _backbufferSize;
	int2 _cursorPos;
	int2 _cursorPosOffset = {5 ,5};

	float _cameraScale = 1.0f;
	float _cameraWheelVelocity = 0.0f;
	int2 _cameraOffset;

	SDL_Color _irCircleColor[4];

	SDL_Vertex _irQuadVertices[4];
	SDL_Vertex _eoQuadVertices[4];

	SDL_Vertex _transformedIRQuadVertices[4];
	SDL_Vertex _transformedEOQuadVertices[4];

	int _quadIndices[6] = { 0, 1, 2, 0, 2, 3 };

	SDL_Texture* _backBuffer = nullptr;
	SDL_Texture* _irImage = nullptr;
	SDL_Texture* _eoImage = nullptr;

	const static int _pathBufferSize = 256;
	char _irImageFilePath[_pathBufferSize];
	char _eoImageFilePath[_pathBufferSize];
};
