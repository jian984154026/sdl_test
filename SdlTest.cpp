#include "SdlTest.h"
#include "SimpleIni.h"
#include <stdlib.h>

using namespace std;

CSdlTest::CSdlTest()
{

}

CSdlTest::~CSdlTest()
{

}

int CSdlTest::LoadConfig(string cfgFile)
{
	CSimpleIniA cfgini;
	SI_Error rc;
	rc = cfgini.LoadFile(cfgFile.c_str());
	if (rc != 0)
	{
		printf("LoadFile test.ini failed\n");
		return -1;
	}

	yuv_path = cfgini.GetValue("Yuv", "yuv_path", "");

	video_width = atoi(cfgini.GetValue("Yuv", "video_width", ""));
	video_height = atoi(cfgini.GetValue("Yuv", "video_height", ""));

	win_width = atoi(cfgini.GetValue("Yuv", "win_width", ""));
	win_height = atoi(cfgini.GetValue("Yuv", "win_height", ""));
}

int refresh_video_timer(void *data)
{
	CSdlTest *ptr = (CSdlTest *)data;
	ptr->RefreshVideoThread();
	return 0;
}

int CSdlTest::YuvTest()
{
	//初始化 SDL
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}

	uint32_t y_frame_len = video_width * video_height;
	uint32_t u_frame_len = video_width * video_height / 4;
	uint32_t v_frame_len = video_width * video_height / 4;
	uint32_t yuv_frame_len = y_frame_len + u_frame_len + v_frame_len;

	//创建窗口
	window = SDL_CreateWindow("Simplest YUV Player",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		video_width, video_height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!window)
	{
		printf("SDL: could not create window, err:%s\n", SDL_GetError());
	}

	// 基于窗口创建渲染器
	renderer = SDL_CreateRenderer(window, -1, 0);

	// 基于渲染器创建纹理
	texture = SDL_CreateTexture(renderer,
		pixformat,
		SDL_TEXTUREACCESS_STREAMING,
		video_width,
		video_height);

	// 分配空间
	video_buf = new uint8_t[yuv_frame_len];
	if (!video_buf)
	{
		printf("Failed to alloce yuv frame space!\n");
		Destroy();
	}

	// 打开YUV文件
	video_fd = fopen(yuv_path.c_str(), "rb");
	if (!video_fd)
	{
		printf("Failed to open yuv file\n");
		Destroy();
	}

	// 创建请求刷新线程
	timer_thread = SDL_CreateThread(refresh_video_timer, "video_thread", this);

	while (1)
	{
		// 收取SDL系统里面的事件
		SDL_WaitEvent(&event);

		if (event.type == REFRESH_EVENT) // 画面刷新事件
		{
			video_buff_len = fread(video_buf, 1, yuv_frame_len, video_fd);
			if (video_buff_len <= 0)
			{
				fprintf(stderr, "Failed to read data from yuv file!\n");
				Destroy();
			}
			// 设置纹理的数据 video_width = 320， plane
			SDL_UpdateTexture(texture, NULL, video_buf, video_width);

			// 显示区域，可以通过修改w和h进行缩放
			rect.x = 0;
			rect.y = 0;
			float w_ratio = win_width * 1.0 / video_width;
			float h_ratio = win_height * 1.0 / video_height;
			// 320x240 怎么保持原视频的宽高比例
			rect.w = video_width * w_ratio;
			rect.h = video_height * h_ratio;
			//            rect.w = video_width * 0.5;
			//            rect.h = video_height * 0.5;

			// 清除当前显示
			SDL_RenderClear(renderer);
			// 将纹理的数据拷贝给渲染器
			SDL_RenderCopy(renderer, texture, NULL, &rect);
			// 显示
			SDL_RenderPresent(renderer);
		}
		else if (event.type == SDL_WINDOWEVENT)
		{
			//If Resize
			SDL_GetWindowSize(window, &win_width, &win_height);
			printf("SDL_WINDOWEVENT win_width:%d, win_height:%d\n", win_width,
				win_height);
		}
		else if (event.type == SDL_QUIT) //退出事件
		{
			s_thread_exit = 1;
		}
		else if (event.type == QUIT_EVENT)
		{
			break;
		}
	}
}

int CSdlTest::PcmTest()
{
	return 0;
}

void CSdlTest::RefreshVideoThread()
{
	while (!s_thread_exit)
	{
		SDL_Event event;
		event.type = REFRESH_EVENT;
		SDL_PushEvent(&event);
		SDL_Delay(40);
	}

	s_thread_exit = 0;

	//push quit event
	SDL_Event event;
	event.type = QUIT_EVENT;
	SDL_PushEvent(&event);
}

void CSdlTest::Destroy()
{
	s_thread_exit = 1;      // 保证线程能够退出

	// 释放资源
	if (timer_thread)
		SDL_WaitThread(timer_thread, NULL); // 等待线程退出
	if (video_buf)
		free(video_buf);
	if (video_fd)
		fclose(video_fd);
	if (texture)
		SDL_DestroyTexture(texture);
	if (renderer)
		SDL_DestroyRenderer(renderer);
	if (window)
		SDL_DestroyWindow(window);

	SDL_Quit();
}
