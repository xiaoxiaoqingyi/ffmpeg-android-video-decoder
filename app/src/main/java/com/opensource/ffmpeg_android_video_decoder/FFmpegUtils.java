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
     *
     * @param sourcePath  MP4等视频文件路径
     * @param savePath    图片的保存路径
     * @param interval    间隔， 抽帧的时候间隔，当 interval=0 时，把所有的帧抽出来
     * @return
     */
    public native boolean captureFrameToJpeg(String sourcePath, String savePath, int interval);



}
