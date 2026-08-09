package com.detectnotes.chord;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity {
    private TextView titleText;
    private TextView statusText;
    private AudioAnalyzer analyzer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        titleText = findViewById(R.id.titleText);
        statusText = findViewById(R.id.statusText);
        Button analyzeButton = findViewById(R.id.analyzeButton);

        analyzer = new AudioAnalyzer();

        analyzeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                titleText.setText("Analyse en cours...");
                statusText.setText(analyzer.analyze());
            }
        });
    }
}
