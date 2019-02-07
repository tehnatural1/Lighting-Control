package com.example.hmclights2;

import static com.example.hmclights2.Constants.*;

import android.content.Context;
import android.graphics.Color;
import android.support.v4.view.ViewCompat;
import android.support.v4.view.ViewPropertyAnimatorCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AccelerateInterpolator;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.AnimationSet;
import android.view.animation.DecelerateInterpolator;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.Space;
import android.widget.TextView;
import android.widget.Toast;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.List;
import java.util.Locale;


public class MainActivity extends AppCompatActivity implements AsyncResponse
{
    IPScanner ipscanner = new IPScanner();
    Butter butter = new Butter();

    // Must be set before executing any async task
    public String server_ip = null;
    public int server_port = 0;

    public boolean layout_was_set = false;

    public static final int ITEM_DELAY = 50;


    @Override
    protected void onCreate(Bundle savedInstanceState)
    {

        setTheme(R.style.AppTheme);
        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
        );

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ipscanner.delegate = this;
        butter.delegate = this;

        Get_Server_Address();

        TextView status = findViewById(R.id.txtStatus);

        if ((server_ip == null) || server_port == 0)
        {
            status.setText(MSG_SCANNING_FOR_CONNECTION);
            ipscanner.execute();
        }
        else
        {
            status.setText(MSG_OBTAINING_BUTTON_LAYOUT);
            butter.execute(GET_BUTTON_LAYOUT_COMMAND);
        }
    }

    @Override
    public boolean get_layout_set() { return this.layout_was_set; }

    @Override
    public void set_layout_set(boolean b) { this.layout_was_set = b; }

    @Override
    public String get_server_ip(){ return server_ip; }

    @Override
    public int get_server_port(){ return server_port; }

    @Override
    public Context get_context() { return this; }

    @Override
    public void ipscan_complete(String result)
    {
        TextView status = findViewById(R.id.txtStatus);

        if ((result == null) || result.equals("null"))
        {
            status.setTextColor(DEFAULT_FAILURE_COLOR);
            status.setText(ERR_UNABLE_TO_CONNECT);
        }
        else
        {
            status.setTextColor(DEFAULT_SUCCESS_COLOR);
            status.setText(String.format(
                    Locale.CANADA,
                    "%s\nIP: %s PORT:%d",
                    MSG_ESTABLISHED_CONNECTION,
                    result,
                    DEFAULT_PORT)
            );

            Set_Server_Address(result, DEFAULT_PORT);
            new Butter().execute(GET_BUTTON_LAYOUT_COMMAND);
        }
    }

    @Override
    public void butter_complete(List<Button> btns, JSONObject layout_options)
    {
        TextView status = findViewById(R.id.txtStatus);
        ProgressBar spinner = findViewById(R.id.progressBar1);

        spinner.setVisibility(View.GONE);

        if (layout_options == null)
        {
            status.setText(ERR_UNABLE_TO_LOAD_LAYOUT);
            status.setTextColor(DEFAULT_FAILURE_COLOR);
            return;
        }

        try
        {
            Space s1 = findViewById(R.id.space1);
            s1.setLayoutParams(get_params(
                    layout_options.getInt(LAYOUT_TOP_SPACER_WEIGHT_IDENTIFIER))
            );

            Space s2 = findViewById(R.id.space2);
            s2.setLayoutParams(get_params(
                    layout_options.getInt(LAYOUT_BOT_SPACER_WEIGHT_IDENTIFIER))
            );

            LinearLayout c = findViewById(R.id.container);
            c.setBackgroundColor(Color.parseColor(
                    layout_options.getString(LAYOUT_BACK_COLOR_IDENTIFIER))
            );

            String[] layout_padding = layout_options.getString(
                    LAYOUT_PADDING_IDENTIFIER
            ).split(";");

            c.setPadding(
                    Integer.parseInt(layout_padding[0]),
                    Integer.parseInt(layout_padding[1]),
                    Integer.parseInt(layout_padding[2]),
                    Integer.parseInt(layout_padding[3])
            );

            for (Button btn : btns)
            {
                Space sp = new Space(getBaseContext());
                sp.setMinimumHeight(
                        layout_options.getInt(LAYOUT_SPACE_HEIGHT_IDENTIFIER) + 20
                );

                final String text = (String) btn.getText();
                final String desc = (String) btn.getContentDescription();
                final boolean toast = (boolean) btn.getTag();
                final AsyncResponse a = this;

                btn.setOnClickListener(new View.OnClickListener() {
                    public void onClick(View view) {
                        Butter b = new Butter();
                        b.delegate = a;
                        b.execute(text);

                        if (toast) {
                            Toast.makeText(
                                    view.getContext(),
                                    desc,
                                    Toast.LENGTH_SHORT
                            ).show();
                        }
                    }
                });

                c.addView(btn);
                c.addView(sp);
            }

            c.removeView(s2);
            c.addView(s2);


            status.setText(MSG_LAYOUT_LOAD_SUCCESS);
            status.setTextColor(Color.parseColor(
                    layout_options.getString(LAYOUT_SUCCESS_COLOR_IDENTIFIER))
            );

            animate_hide(status);
            animate();
        }
        catch (JSONException e)
        {
            e.printStackTrace();
            System.err.println("JSON Layout is invalid");
        }
    }

    private void animate() {
        ViewGroup container = findViewById(R.id.container);

        for (int i = 0; i < container.getChildCount(); i++) {
            View v = container.getChildAt(i);
            ViewPropertyAnimatorCompat viewAnimator;

            if (!(v instanceof Button)) {
                viewAnimator = ViewCompat.animate(v)
                        .translationY(v.getHeight()).alpha(1)
                        .setStartDelay((ITEM_DELAY * i) + 500)
                        .setDuration(1000);
            } else {
                viewAnimator = ViewCompat.animate(v)
                        .scaleY(1).scaleX(1)
                        .setStartDelay((ITEM_DELAY * i) + 500)
                        .setDuration(500);
            }

            viewAnimator.setInterpolator(new DecelerateInterpolator()).start();
        }
    }

    private void animate_hide(View v)
    {
        Animation fadeOut = new AlphaAnimation(1, 0);
        fadeOut.setInterpolator(new AccelerateInterpolator());
        fadeOut.setStartOffset(ANIMATION_FADE_DURATION);
        fadeOut.setDuration(ANIMATION_FADE_DURATION);

        AnimationSet animation = new AnimationSet(false);
        animation.addAnimation(fadeOut);
        v.setAnimation(animation);
        v.setVisibility(View.GONE);
    }

    private void Get_Server_Address()
    {
        StringBuilder sb = new StringBuilder();
        FileInputStream is;
        InputStreamReader ir;
        BufferedReader br;
        JSONObject data;

        try
        {
            is = openFileInput(DEVICE_STORAGE_FILE);
            ir = new InputStreamReader(is);
            br = new BufferedReader(ir);

            String line;
            while ((line = br.readLine()) != null)
            {
                sb.append(line);
            }

            br.close();
            ir.close();
            is.close();

            data = new JSONObject(sb.toString());

            server_ip = data.getString(DEVICE_STORAGE_IP_IDENTIFIER);
            server_port = data.getInt(DEVICE_STORAGE_PORT_IDENTIFIER);
        }
        catch (FileNotFoundException e)
        {
            e.printStackTrace();
            System.err.println("Unable to locate file.");
        }
        catch (IOException e)
        {
            e.printStackTrace();
            System.err.println("Unable to access stdin/stdout PIPE");
        }
        catch (JSONException e)
        {
            e.printStackTrace();
            System.err.println("File contents were in an invalid JSON format.");
        }
    }

    private void Set_Server_Address(String ip_address, int port)
    {
        FileOutputStream os;
        JSONObject obj = new JSONObject();

        try
        {
            obj.put(DEVICE_STORAGE_IP_IDENTIFIER, ip_address);
            obj.put(DEVICE_STORAGE_PORT_IDENTIFIER, port);
        }
        catch (JSONException e)
        {
            e.printStackTrace();
            return;
        }

        try
        {
            os = openFileOutput(DEVICE_STORAGE_FILE, Context.MODE_PRIVATE);
            os.write(obj.toString().getBytes());
            os.close();
        }
        catch (FileNotFoundException e)
        {
            e.printStackTrace();
            System.err.println("Unable to create file.");
        }
        catch (IOException e)
        {
            e.printStackTrace();
            System.err.println("Unable to write file contents.");
        }
    }

    private LinearLayout.LayoutParams get_params(int weight) {
        return new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.WRAP_CONTENT,
                LinearLayout.LayoutParams.WRAP_CONTENT,
                (1.0f * weight)
        );
    }
}
