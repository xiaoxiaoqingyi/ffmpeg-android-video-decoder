package com.opensource.ffmpeg_android_video_decoder;

/**
 * Created by Administrator on 2017/9/15.
 */

public class FFmpegUtils {

    //载入动态库
    static {
        System.loadLibrary("videoHandler");
    }

    /**
     * 打开视频
     * @param absFileName
     * @return
     */
    public native String  openVideo(String absFileName);

    /**
     *
     * @param time_sec 开始的时间
     * @param fps  帧数的间隔
     * @return
     */
    public native boolean setBeginning(int time_sec, int fps);

    /**
     *
     * @param filePath 文件保存地址
     * @param interval 1秒钟抽的帧数
     * @return
     */
    public native boolean saveAFrame(String filePath, int interval);
    public native boolean closeVideo();



}
