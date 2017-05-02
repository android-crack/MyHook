package com.lmd.anticheater;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

public class AntiCheater extends AppCompatActivity {

    static {
        System.loadLibrary("LMDLoader");
        System.loadLibrary("LMDAnti");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }
}
