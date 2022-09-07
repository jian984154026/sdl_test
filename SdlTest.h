#ifndef CSDL_TEST_H
#define CSDL_TEST_H

#include "SDL.h"
#include <string>

//自定义消息类型
#define REFRESH_EVENT   (SDL_USEREVENT + 1)     // 请求画面刷新事件
#define QUIT_EVENT      (SDL_USEREVENT + 2)     // 退出事件

class CSdlTest
{
public:
	CSdlTest();
	~CSdlTest();

	int LoadConfig(std::string cfgFile);
	int YuvTest();
	int PcmTest();
	void RefreshVideoThread();
	void Destroy();
private:
	// SDL
	SDL_Event event;                            // 事件
	SDL_Rect rect;                              // 矩形
	SDL_Window *window = NULL;                  // 窗口
	SDL_Renderer *renderer = NULL;              // 渲染
	SDL_Texture *texture = NULL;                // 纹理
	SDL_Thread *timer_thread = NULL;            // 请求刷新线程
	uint32_t pixformat = SDL_PIXELFORMAT_IYUV;  // YUV420P，即是SDL_PIXELFORMAT_IYUV

	// 分辨率
	// 1. YUV的分辨率
	int video_width = 320;
	int video_height = 240;

	// 2.显示窗口的分辨率
	int win_width = 320;
	int win_height = 240;

	// YUV文件句柄
	FILE *video_fd = NULL;
	std::string yuv_path = "yuv420p_320x240.yuv";
	size_t video_buff_len = 0;
	uint8_t *video_buf = NULL; //读取数据后先把放到buffer里面

	int s_thread_exit = 0;  // 退出标志 = 1则退出
};

#endif 

