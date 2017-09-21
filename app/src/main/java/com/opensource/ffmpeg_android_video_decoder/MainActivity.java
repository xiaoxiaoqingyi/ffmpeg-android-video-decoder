package com.opensource.ffmpeg_android_video_decoder;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        init();
    }

    private void init(){
        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
    }


}
