package com.example.hmclights2;

import static com.example.hmclights2.Constants.*;

import android.content.DialogInterface;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Handler;
import android.support.v4.view.ViewCompat;
import android.support.v4.view.ViewPropertyAnimatorCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.InputType;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.style.ForegroundColorSpan;
import android.text.style.RelativeSizeSpan;
import android.view.ContextThemeWrapper;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AccelerateInterpolator;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.AnimationSet;
import android.view.animation.DecelerateInterpolator;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Space;
import android.widget.TextView;
import android.widget.Toast;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.text.DateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.Locale;


public class MainActivity extends AppCompatActivity implements AsyncResponse
{

    private static final String OPEN_WEATHER_MAP_API = "GET_AN_API_KEY";



    IPScanner ipscanner;
    Butter butter;

    TextView city_name;
    TextView weather_details;
    TextView weather_icon;

    Handler handler;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        setTheme(R.style.AppTheme);

        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
        );

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        handler = new Handler();

        city_name = findViewById(R.id.city);
        weather_details = findViewById(R.id.details_field);
        weather_icon = findViewById(R.id.weather_icon);

        weather_icon.setTypeface(
                Typeface.createFromAsset(
                        this.getAssets(),
                        "fonts/weather.ttf"
                )
        );
    }

    @Override
    public void onResume()
    {
        super.onResume();

        updateWeatherData(new CityPreference(this).getCity());

        update_buttons();

        animate();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu_options, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        switch (item.getItemId())
        {
            case R.id.change_city:
                showInputDialog();
                return true;

            case R.id.Item1:
                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private void showInputDialog()
    {
        AlertDialog.Builder ab = new AlertDialog.Builder(this);
        ab.setTitle("Change city");
        final EditText et = new EditText(this);
        et.setInputType(InputType.TYPE_CLASS_TEXT);
        ab.setView(et);

        ab.setPositiveButton("Go", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which)
            {
                changeCity(et.getText().toString());
            }
        });
        ab.show();
    }

    @Override
    public String get_server_ip(){
        return new ServerAddress(this).getIP();
    }

    @Override
    public int get_server_port(){
        return new ServerAddress(this).getPort();
    }

    @Override
    public void ipscan_complete(String result)
    {
        TextView status = findViewById(R.id.txtStatus);

        if ((result == null) || result.equals("null"))
        {
            status.setTextColor(Color.parseColor(DEFAULT_FAILURE_COLOR));
            status.setText(ERR_UNABLE_TO_CONNECT);
        }
        else
        {
            status.setTextColor(Color.parseColor(DEFAULT_SUCCESS_COLOR));
            status.setText(String.format(
                    Locale.CANADA,
                    "%s\nIP: %s PORT:%d",
                    MSG_ESTABLISHED_CONNECTION,
                    result,
                    DEFAULT_PORT)
            );

            new ServerAddress(this).setIP(result, DEFAULT_PORT);
            butter.execute(GET_BUTTON_LAYOUT_COMMAND);
        }
    }

    @Override
    public void butter_complete(String s)
    {
        try
        {
            if (s.equals("done"))
            {
                Toast.makeText(
                        getBaseContext(),
                        "Success",
                        Toast.LENGTH_SHORT
                ).show();
            }
            else if (s.equals("butter"))
            {
                System.out.println("System Alive check response");
            }
            else
            {
                JSONObject data = new JSONObject(s);
                JSONArray btns = data.getJSONArray(LAYOUT_BUTTONS_ID);
                JSONObject layout = data.getJSONObject(LAYOUT_OBJECT_ID);

                update_btn_layout(btns, layout);
            }
        }
        catch (JSONException e)
        {
            e.printStackTrace();
        }
    }


    public void update_btn_layout(JSONArray btns, JSONObject layout)
    {
        TextView status = findViewById(R.id.txtStatus);

        if (layout == null)
        {
            status.setText(ERR_UNABLE_TO_LOAD_LAYOUT);
            status.setTextColor(Color.parseColor(DEFAULT_FAILURE_COLOR));
            return;
        }

        try
        {
            LinearLayout c = findViewById(R.id.container);
            c.setBackgroundColor(Color.parseColor(
                    layout.getString(LAYOUT_BACK_COLOR_ID))
            );

            String[] layout_padding = layout.getString(
                    LAYOUT_PADDING_ID
            ).split(";");

            c.setPadding(
                    Integer.parseInt(layout_padding[0]),
                    Integer.parseInt(layout_padding[1]),
                    Integer.parseInt(layout_padding[2]),
                    Integer.parseInt(layout_padding[3])
            );


            for (int i = 0; i < btns.length(); i++)
            {
                JSONObject btn = btns.getJSONObject(i);

                if (btn != null) {
                    Button b = create_button(btn);

                    Space sp = new Space(getBaseContext());
                    sp.setMinimumHeight(
                            layout.getInt(LAYOUT_SPACE_HEIGHT_ID)
                    );

                    if (b != null) {
                        c.addView(b);
                        c.addView(sp);
                    }
                }
            }

            status.setText(MSG_LAYOUT_LOAD_SUCCESS);
            status.setTextColor(Color.parseColor(
                    layout.getString(LAYOUT_SUCCESS_COLOR_ID))
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


    private Button create_button(JSONObject cmd)
    {
        final String[] padding = cmd.optString(
                BTN_PADDING_ID,
                DEFAULT_BUTTON_PADDING
        ).split(";");
        final String text = cmd.optString(
                BTN_TEXT_ID,
                DEFAULT_BUTTON_ERR_TEXT
        ) + '\0';
        final String desc = cmd.optString(
                BTN_DESCRIPTION_ID,
                DEFAULT_BUTTON_ERR_TEXT
        ) + '\0';
        final String btn_id = cmd.optString(
                BTN_GPIO_ID,
                ""
        ) + '\0';
        final String f_color = cmd.optString(
                BTN_TEXT_COLOR_ID,
                DEFAULT_BUTTON_BACKGROUND_COLOR
        );
        final String b_color = cmd.optString(
                BTN_COLOR_ID,
                DEFAULT_BUTTON_FOREGROUND_COLOR
        );
        final int text_size = cmd.optInt(
                BTN_TEXT_SIZE_ID,
                DEFAULT_BUTTON_TEXT_SIZE
        );
        final int height = cmd.optInt(
                BTN_HEIGHT_ID,
                DEFAULT_BUTTON_HEIGHT
        );
        final Boolean toast = cmd.optBoolean(
                BTN_TOAST_ID,
                DEFAULT_TOAST_DISPLAY
        );
        final boolean broadcast = cmd.optBoolean(
                BTN_BROADCAST_ID,
                DEFAULT_BUTTON_OPTION_STATE
        );
        final boolean enabled = cmd.optBoolean(
                BTN_ENABLE_ID,
                DEFAULT_BUTTON_OPTION_STATE
        );
        final AsyncResponse a = this;


        if (!enabled) return null;

        Button btn = new Button(
                new ContextThemeWrapper(
                        getBaseContext(),
                        R.style.ButtonAppTheme
                ),
                null,
                R.style.ButtonAppTheme
        );

        btn.setMinHeight(height);
        btn.setText(text);
        btn.setTextSize(text_size);
        btn.setScaleX(0f);
        btn.setScaleY(0f);

        if (f_color != DEFAULT_BUTTON_BACKGROUND_COLOR)
        {
            //btn.setTextColor(Color.parseColor(f_color));
        }

        if (b_color != DEFAULT_BUTTON_FOREGROUND_COLOR)
        {
            //btn.setBackgroundTintList(ColorStateList.valueOf(Color.parseColor(b_color)));
        }

        if (!Arrays.stream(padding).allMatch("0"::equals))
        {
            btn.setPadding(
                    Integer.parseInt(padding[0]),
                    Integer.parseInt(padding[1]),
                    Integer.parseInt(padding[2]),
                    Integer.parseInt(padding[3])
            );
        }

        btn.setOnClickListener((View v) -> {
            Butter b = new Butter();
            b.delegate = a;
            b.execute(btn_id);
        });

        return btn;
    }


    private void updateWeatherData(final String city)
    {
        new Thread(() -> {
            final JSONObject data = RemoteFetch.getJSON(
                    getApplicationContext(),
                    city,
                    OPEN_WEATHER_MAP_API
            );

            if (data != null) {
                Runnable r = () -> renderWeather(data);
                handler.post(r);
            }
        }).start();
    }


    private void update_buttons()
    {
        butter = new Butter();
        butter.delegate = this;

        LinearLayout l = findViewById(R.id.container);
        l.removeAllViews();

        new Thread(() -> butter.execute(GET_BUTTON_LAYOUT_COMMAND)).start();
    }


    private void renderWeather(JSONObject json)
    {
        try
        {
            JSONObject details = json.getJSONArray("weather")
                    .getJSONObject(0);

            JSONObject main = json.getJSONObject("main");
            JSONObject wind = json.getJSONObject("wind");

            double temp = main.getDouble("temp");
            double wind_speed = wind.getDouble("speed");
            int humidity = main.getInt("humidity");

            String updatedOn = DateFormat.getDateTimeInstance().format(
                    new Date(json.getLong("dt")*1000)
            );

            String city = String.format(
                    "%s, %s",
                    json.getString("name"),
                            //.toUpperCase(Locale.CANADA),
                    json.getJSONObject("sys")
                            .getString("country")
            );


            String temperature = String.format(
                    Locale.CANADA,
                    "%.0f°",
                    temp
            );

            String detail = String.format(
                    Locale.CANADA,
                    "Low: %.0f° | High: %.0f°",
                    main.getDouble("temp_min"),
                    main.getDouble("temp_max")
            );

/*
            String weather_low = getString(R.string.weather_low);
            String weather_high = getString(R.string.weather_high);

            String detail = String.format(
                    Locale.CANADA,
                    "%s: %.0f° | %s: %.0f°",
                    weather_low,
                    main.getDouble("temp_min"),
                    weather_high,
                    main.getDouble("temp_max")
            );
*/
            setWeatherIcon(
                    details.getInt("id"),
                    json.getJSONObject("sys")
                            .getLong("sunrise") * 1000,
                    json.getJSONObject("sys")
                            .getLong("sunset") * 1000
            );


            /**
             * Environment Canada wind chill formula:
             *     Ta = air temperature
             *     v = wind velocity
             * T = 13.12 + 0.6215 Ta - 11.37v^0.16 + 0.3965 Ta v^0.16
             *
             * Environment Canada humidex formula:
             *     h = (0.5555)*(e - 10.0)
             *     e = 6.11 * exp(5417.7530 * ((1/273.16) - (1/dewpoint)))
             * Humidex = (air temperature) + h
             *
             */
            /*double feels_like = 13.12
                    + (0.6215 * temp)
                    - (11.37 * Math.pow(wind_speed, 0.16))
                    + (0.3965 * temp * Math.pow(wind_speed, 0.16));*/

            long feels_like = Math.round(
                    13.12
                    + (0.6215 * temp)
                    - (11.37 * Math.pow(wind_speed, 0.16))
                    + (0.3965 * temp * Math.pow(wind_speed, 0.16))
            );

/*
            double humidex = temp
                    + (5/9)
                    * 6.112 * Math.pow(10, (
                            7.5
                            * (temp / (237.7 + temp))
                            * (humidity / 100)));
*/

            String s = String.format(Locale.CANADA,"%d°", feels_like);

            //String wind_icon = getString(R.string.weather_wind);

            //String s_wind = wind_icon + String.format(Locale.CANADA," %.1f", wind_speed);

            //Typeface weather_font = Typeface.createFromAsset(this.getAssets(), "fonts/weather.ttf");


            int temp_color;
            if (feels_like < Math.round(temp)){
                temp_color = Color.parseColor("#458588");
            }
            else if(feels_like > Math.round(temp))
            {
                temp_color = Color.parseColor("#fb4934");
            }
            else
            {
                temp_color = Color.BLACK;
                s = "";
            }


            SpannableString ss = new SpannableString(
                    String.format(
                            Locale.CANADA,
                            "%s  %s\n%s",
                            temperature, s, detail
                    )
            );

            ss.setSpan(
                    new RelativeSizeSpan(2f),
                    0,
                    temperature.length() + 2, // + s.length(),
                    0
            );

            /*
            ss.setSpan(
                    new CustomTypefaceSpan("", weather_font),
                    temperature.length() + 2 + s.length() + 1,
                    temperature.length() + 2 + s.length() + 1 + wind_icon.length() + 1 + detail.length(),
                    Spanned.SPAN_EXCLUSIVE_INCLUSIVE
            );
            */

            ss.setSpan(
                    new ForegroundColorSpan(temp_color),
                    temperature.length() + 2,
                    temperature.length() + 2 + s.length(),
                    0
            );

            city_name.setText(city);
            weather_details.setText(ss);
        }
        catch(Exception e)
        {
            e.printStackTrace();
            System.err.println("JSON data is incomplete.");
        }
    }


    private void setWeatherIcon(int actualId, long sunrise, long sunset){

        int id = actualId / 100;
        String icon = "";

        if(actualId == 800)
        {
            long currentTime = new Date().getTime();

            if(currentTime>=sunrise && currentTime<sunset)
            {
                icon = getString(R.string.weather_sunny);
            }
            else
            {
                icon = getString(R.string.weather_clear_night);
            }
        }
        else
        {
            switch(id)
            {
                case 2 : icon = getString(R.string.weather_thunder);
                    break;

                case 3 : icon = getString(R.string.weather_drizzle);
                    break;

                case 7 : icon = getString(R.string.weather_foggy);
                    break;

                case 8 : icon = getString(R.string.weather_cloudy);
                    break;

                case 6 : icon = getString(R.string.weather_snowy);
                    break;

                case 5 : icon = getString(R.string.weather_rainy);
                    break;
            }
        }

        weather_icon.setText(icon);
    }

    public void changeCity (String city)
    {
        updateWeatherData(city);
        new CityPreference(this).setCity(city);
    }


    private void animate_container(ViewGroup c, int duration)
    {
        for (int i = 0; i < c.getChildCount(); i++)
        {
            View v = c.getChildAt(i);
            ViewPropertyAnimatorCompat a;

            a = ViewCompat.animate(v)
                    .scaleX(1)
                    .scaleY(1)
                    .translationY(50)
                    .alpha(1)
                    .setStartDelay(0)
                    .setDuration(duration);

            a.setInterpolator(new DecelerateInterpolator()).start();
        }
    }


    private void animate()
    {
        animate_container(findViewById(R.id.container), 1000);
        animate_container(findViewById(R.id.weather), 3000);
        animate_container(findViewById(R.id.base_container), 3000);
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

}
