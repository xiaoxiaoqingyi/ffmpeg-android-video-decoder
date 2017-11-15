#include <jni.h>
#include <string>
#include <setjmp.h>
#include <android/log.h>

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libjpeg-turbo/jpeglib.h"
}

#define TAG "JNI_TAG"
//为了方便调用，将输出宏定义
//#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
//#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__);

// compatibility with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

#define true 1
#define false 0


extern "C"{

typedef uint8_t BYTE;

    char info[10000] = { 0 };
    AVFormatContext   *pFormatCtx = NULL;
    static int        i, j, k;
    int               videoStream;
    AVCodecContext    *pCodecCtxOrig = NULL;
    AVCodecContext    *pCodecCtx = NULL;
    AVCodec           *pCodec = NULL;
    AVFrame           *pFrame = NULL;
    AVFrame           *pFrameRGB = NULL;
    AVPacket          packet;
    int               frameFinished;
    int               numBytes;
    uint8_t           *buffer = NULL;
    struct SwsContext *sws_ctx = NULL;
    int               err;

    struct my_error_mgr {
        struct jpeg_error_mgr pub;
        jmp_buf setjmp_buffer;
    };

    typedef struct my_error_mgr * my_error_ptr;

    //int WriteJPEG (AVCodecContext *pCodecCtx, AVFrame *pFrame, int FrameNo);
    int generateJPEG(BYTE* data, int w, int h, int quality, const char* outfilename, jboolean optimize);


    void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame, char *absFilePath);

/*
 * Method:    OpenVideo
 */
JNIEXPORT jstring JNICALL Java_com_opensource_ffmpeg_1android_1video_1decoder_FFmpegUtils_openVideo
        (JNIEnv *env, jobject obj, jstring FilePath)
{
    // Register all formats and codecs
    av_register_all();

    // Open video file
    char *videoFileName;
    videoFileName = (char *)env->GetStringUTFChars(FilePath, NULL);
    err = avformat_open_input(&pFormatCtx, videoFileName, NULL, NULL);
    if(err != 0) {
        //av_strerror(err, info, 10000);
        sprintf(info, "Couldn't open file,Filename: %s,Error: %d\n", videoFileName, err);
        return env->NewStringUTF(info);
    }

    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, NULL)<0)
    {
        sprintf(info, "Couldn't find stream information\n");
        return env->NewStringUTF(info);
    }

    // Dump information about file onto standard error
    //av_dump_format(pFormatCtx, 0, argv[1], 0);
    av_dump_format(pFormatCtx, 0, videoFileName, 0);

    // Find the first video stream
    videoStream=-1;
    for(i=0; i<pFormatCtx->nb_streams; i++)
    {
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoStream=i;
            break;
        }
    }

    if(videoStream==-1)
    {
        sprintf(info, "Didn't find a video stream\n");
        return env->NewStringUTF(info);
    }

    // Get a pointer to the codec context for the video stream
    pCodecCtxOrig=pFormatCtx->streams[videoStream]->codec;
    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtxOrig->codec_id);
    if(pCodec==NULL) {
        sprintf(info, "Unsupported codec!\n");
        return env->NewStringUTF(info);
        //fprintf(stderr, "Unsupported codec!\n");
        //return -1; // Codec not found
    }
    // Copy context
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if(avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
        sprintf(info, "Couldn't copy codec context!\n");
        return env->NewStringUTF(info);
        //fprintf(stderr, "Couldn't copy codec context");
        //return -1; // Error copying codec context
    }

    // Open codec
    if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
    {
        sprintf(info, "Could not open codec!\n");
        return env->NewStringUTF(info);
    }

    // Allocate video frame
    pFrame=av_frame_alloc();

    // Allocate an AVFrame structure
    pFrameRGB=av_frame_alloc();
    if(pFrameRGB==NULL)
    {
        sprintf(info, "pFrameRGB==NULL!\n");
        return env->NewStringUTF(info);
    }

    // Determine required buffer size and allocate buffer
    numBytes=avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,
                                pCodecCtx->height);
    buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24,
                   pCodecCtx->width, pCodecCtx->height);

    // initialize SWS context for software scaling
    sws_ctx = sws_getContext(pCodecCtx->width,
                             pCodecCtx->height,
                             pCodecCtx->pix_fmt,
                             pCodecCtx->width,
                             pCodecCtx->height,
                             AV_PIX_FMT_RGB24,
                             SWS_BILINEAR,
                             NULL,
                             NULL,
                             NULL
    );

    i=0;
    j=0;
    k=0;

    char info[40000] = { 0 };
    return env->NewStringUTF(info);

}

/*
 * Method:    SaveAFrame
 */
JNIEXPORT jboolean JNICALL Java_com_opensource_ffmpeg_1android_1video_1decoder_FFmpegUtils_saveAFrame
        (JNIEnv *env, jobject obj, jstring filePath, jint interval)
{
    j=1;

    char *absFilePath;
    absFilePath = (char *)env->GetStringUTFChars(filePath, NULL);
    while(av_read_frame(pFormatCtx, &packet)>=0) {
        // Is this a packet from the video stream?
        if(packet.stream_index==videoStream) {
            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
            //LOGI("decode_video2");

            // Did we get a video frame?
            if(frameFinished) {

                if( j%interval==0 )
                {
                    k = j/interval;
                    sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
                              pFrame->linesize, 0, pCodecCtx->height,
                              pFrameRGB->data, pFrameRGB->linesize);
                    SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, k+1, absFilePath);
                    LOGI("have saved frames, j is %d, k is %d\n", j, k);

                    //LOGI("frameFinished, free packet\n");
                    av_free_packet(&packet);
                    break;
                }
                ++j;
            }
        }

        // Free the packet that was allocated by av_read_frame
        //LOGI("free packet\n");
        av_free_packet(&packet);
    }

    return true;
}

/*
 * Method:    CloseVideo
 */
JNIEXPORT jboolean JNICALL Java_com_opensource_ffmpeg_1android_1video_1decoder_FFmpegUtils_closeVideo
        (JNIEnv *env, jobject obj)
{
    // Free the RGB image
    av_free(buffer);
    //av_frame_free(&pFrameRGB);
    av_free(pFrameRGB);

    // Free the YUV frame
    //av_frame_free(&pFrame);
    av_free(pFrame);

    // Close the codecs
    avcodec_close(pCodecCtx);
    avcodec_close(pCodecCtxOrig);

    // Close the video file
    avformat_close_input(&pFormatCtx);

    return true;
}

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame, char *absFilePath) {

    generateJPEG(pFrame->data[0], width, height, 80, absFilePath, true);
}

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    (*cinfo->err->output_message) (cinfo);
//    error=myerr->pub.jpeg_message_table[myerr->pub.msg_code];
    LOGI("jpeg_message_table[%d]:%s", myerr->pub.msg_code,myerr->pub.jpeg_message_table[myerr->pub.msg_code]);
    // LOGE("addon_message_table:%s", myerr->pub.addon_message_table);
//  LOGE("SIZEOF:%d",myerr->pub.msg_parm.i[0]);
//  LOGE("sizeof:%d",myerr->pub.msg_parm.i[1]);
    longjmp(myerr->setjmp_buffer, 1);
}

int generateJPEG(BYTE* data, int w, int h, int quality, const char* outfilename, jboolean optimize)
{
    int nComponent = 3;

    struct jpeg_compress_struct jcs;

    struct my_error_mgr jem;

    jcs.err = jpeg_std_error(&jem.pub);
    jem.pub.error_exit = my_error_exit;
    if (setjmp(jem.setjmp_buffer)) {
        return 0;
    }
    jpeg_create_compress(&jcs);
    FILE* f = fopen(outfilename, "wb");
    if (f == NULL) {
        return 0;
    }
    jpeg_stdio_dest(&jcs, f);

    jcs.image_width = w;
    jcs.image_height = h;

    jcs.arith_code = false;
    jcs.input_components = nComponent;
    jcs.in_color_space = JCS_RGB;

    jpeg_set_defaults(&jcs);
    jcs.optimize_coding = optimize;
    jpeg_set_quality(&jcs, quality, true);

    jpeg_start_compress(&jcs, TRUE);
    JSAMPROW row_pointer[1];
    int row_stride;
    row_stride = w * nComponent;
    while (jcs.next_scanline < jcs.image_height) {
        row_pointer[0] = &data[jcs.next_scanline * row_stride];
        jpeg_write_scanlines(&jcs, row_pointer, 1);
    }

    jpeg_finish_compress(&jcs);
    jpeg_destroy_compress(&jcs);
    fclose(f);

    return 1;
}

}

