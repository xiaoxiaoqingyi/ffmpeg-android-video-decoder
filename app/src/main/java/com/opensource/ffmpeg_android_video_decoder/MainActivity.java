package com.opensource.ffmpeg_android_video_decoder;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    private final static int REQUEST_CODE_FILE = 0x111;

    private EditText editText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        init();
    }

    private void init(){
        findViewById(R.id.button_file_path_input).setOnClickListener(this);
        findViewById(R.id.start).setOnClickListener(this);
        editText = (EditText)findViewById(R.id.file_path_input);
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
        EditText editTextOutputFolder = (EditText) findViewById(R.id.folder_created);
        String outputDir = Environment.getExternalStorageDirectory() + "/" + editTextOutputFolder.getText().toString();
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
            String curFileName = data.getData().getPath();
            editText.setText(curFileName);
        }
    }
}
