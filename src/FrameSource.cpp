#include "FrameSource.h"

#include <iostream>
#include <fstream>
#include <math.h>

VideoStream* VideoStreamFrameSource::InitVideoStream(const char* filename, uint32_t width, uint32_t height)
{
	std::fstream file;
	file.open(filename, std::ios::in | std::ios::binary);

	if (!file.is_open())
	{
		return nullptr;
	}

	file.seekg(0, std::ios::end);

	uint32_t fileSize = uint32_t(file.tellg());
	uint8_t* fileData = new uint8_t[fileSize];

	file.seekg(0, std::ios::beg);
	file.read((char*)fileData, fileSize);
	file.close();

	return new VideoStream(fileData, width, height, _numberOfVideoFrames);
}

bool VideoStreamFrameSource::Init(Options const& options)
{
	_frameWidth = options.width;
	_frameHeight = options.height;
	_videoFrameIdxOffset = options.frameOffset;
	_numberOfVideoFrames = options.numberOfVideoFrames;
	_playableNumberOfVideoFrames = _numberOfVideoFrames - _videoFrameIdxOffset;

	_videoStream = InitVideoStream(options.filename, options.width, options.height);

	if (_videoStream == nullptr)
	{
		std::cout << "Can't load video stream: " << options.filename << std::endl;
		return false;
	}

	return true;
}

void VideoStreamFrameSource::Update()
{
	_videoFrameCounter = (_videoFrameCounter + 1) % _playableNumberOfVideoFrames;
	_videoFrameIdx = _videoFrameIdxOffset + _videoFrameCounter;
}

void VideoStreamFrameSource::Render(SDL_Renderer* renderer, SDL_Texture* backbuffer, SDL_Texture* lockableBackbuffer)
{
	uint32_t* pixels = nullptr;
	
	int screenBufferPitch = 0;

	if (SDL_LockTexture(lockableBackbuffer, nullptr, (void**)(&pixels), &screenBufferPitch) == 0)
	{
		SDL_assert(screenBufferPitch == (_frameWidth * 4));

		VideoFrameRaw const& frame = _videoStream->GetVideoFrame(_videoFrameIdx);

		for (uint32_t x = 0; x < _frameWidth; x++)
		{
			for (uint32_t y = 0; y < _frameHeight; y++)
			{
				uint8_t pixelValue = frame.GetPixel(x, y);
				pixels[x + y * _frameWidth] = (pixelValue << 24) | (pixelValue << 16) | (pixelValue << 8);
			}
		}

		SDL_UnlockTexture(lockableBackbuffer);
	}

	SDL_SetRenderTarget(renderer, backbuffer);
	SDL_RenderCopy(renderer, lockableBackbuffer, NULL, NULL);
}

void VideoStreamFrameSource::Shutdown()
{
	delete _videoStream;
}

bool VideoStreamFrameSource::IsFirstFrame()
{
	return (_videoFrameIdx == _videoFrameIdxOffset);
}

bool RenderFrameSource::Init(Options const&)
{
	return true;
}

void RenderFrameSource::Update()
{
	_phase += 0.4f;
}

int
SDL_RenderFillCircle(SDL_Renderer* renderer, int x, int y, int radius)
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

void RenderFrameSource::Render(SDL_Renderer* renderer, SDL_Texture* backbuffer, SDL_Texture* lockableBackbuffer)
{
	SDL_SetRenderTarget(renderer, backbuffer);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);

	int radius = 50;

	// Test the bit and render in case of success
	//if (rfid.test(_currentFrame))
	{
		int circleX = 600 + int(2.0f * float(radius) * cos(_phase));
		int circleY = 300 + int(2.0f * float(radius) * sin(_phase));

		SDL_RenderFillCircle(renderer, circleX, circleY, radius);
	}

	// Render noise
	SDL_RenderFillCircle(renderer, 100 + int(2 * float(radius) * cos(_phase)), 100, 30);
	SDL_RenderFillCircle(renderer, 1300, 600 + int(4 * float(radius) * cos(_phase)), 20);

	if (_phase > _renderingDelay)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillCircle(renderer, 300, 200 + int(float(radius) * cos(_phase)), 10);
	}
}

void RenderFrameSource::Shutdown()
{

}

bool RenderFrameSource::IsFirstFrame()
{
	return false;
}
