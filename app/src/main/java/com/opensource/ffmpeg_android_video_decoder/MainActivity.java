package com.opensource.ffmpeg_android_video_decoder;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    private final static int REQUEST_CODE_FILE = 0x111;

    private EditText input_editText;
    private FFmpegUtils ffmpegUtils;
    private TextView txt;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        init();
    }

    private void init(){
        findViewById(R.id.button_file_path_input).setOnClickListener(this);
        findViewById(R.id.start).setOnClickListener(this);
        input_editText = (EditText)findViewById(R.id.file_path_input);
        txt = (TextView)findViewById(R.id.txt);
        ffmpegUtils = new FFmpegUtils();
    }


    @Override
    public void onClick(View v) {
          switch (v.getId()){
              case R.id.start:
                  startDecoder();
                  break;
              case R.id.button_file_path_input:
                  chooceFile();
                  break;
          }
    }

    private void startDecoder(){
        EditText folder_created = (EditText) findViewById(R.id.folder_created);
        String outputDir = Environment.getExternalStorageDirectory() + "/" + folder_created.getText().toString();
        File saveDir = new File(outputDir);
        if(!saveDir.exists()){
            saveDir.mkdir();
        }

        String filePath = input_editText.getText().toString();
        ffmpegUtils.openVideo(filePath);

        //把前1秒的删了
        ffmpegUtils.setBeginning(0,30);

        for(int i=1; i<= 300; i++){
            String fileName = i + ".jpg";
            String absFilePath = outputDir + "/" + fileName;
            ffmpegUtils.saveAFrame(absFilePath, 1);
            txt.setText("已处理:"+i);
        }
        ffmpegUtils.closeVideo();
        txt.setText("已处理:"+300+",已完成!");

    }

    private void chooceFile(){
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.setType("*/*");
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        if (intent.resolveActivity(getPackageManager()) != null) {
            startActivityForResult(Intent.createChooser(intent, "Select a File"), REQUEST_CODE_FILE);
        } else {
            new AlertDialog.Builder(this).setTitle("未找到文件管理器")
                    .setMessage("请安装文件管理器以选择文件")
                    .setPositiveButton("确定", null)
                    .show();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if(resultCode == Activity.RESULT_OK && requestCode == REQUEST_CODE_FILE){
            String curFileName = Utils.getPath(this, data.getData());
            input_editText.setText(curFileName);
        }
    }


}
