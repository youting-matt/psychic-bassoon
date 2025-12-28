// app/src/main/java/com/example/myapplication/MainActivity.kt
package com.example.myapplication

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.location.Location
import android.location.LocationManager
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.google.gson.Gson
import com.google.gson.reflect.TypeToken
import okhttp3.*
import java.io.IOException
import java.lang.reflect.Type

class MainActivity : AppCompatActivity() {
    private val REQUEST_LOCATION_PERMISSION = 1001
    private lateinit var temperatureText: TextView
    private lateinit var locationText: TextView
    private lateinit var weatherText: TextView
    private lateinit var refreshButton: Button
    private lateinit var locationManager: LocationManager
    private val client = OkHttpClient()
    private val gson = Gson()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // 初始化UI组件
        temperatureText = findViewById(R.id.temperatureText)
        locationText = findViewById(R.id.locationText)
        weatherText = findViewById(R.id.weatherText)
        refreshButton = findViewById(R.id.refreshButton)
        locationManager = getSystemService(Context.LOCATION_SERVICE) as LocationManager

        // 检查位置权限
        if (checkLocationPermission()) {
            getCurrentLocationAndWeather()
        } else {
            requestLocationPermission()
        }

        // 刷新按钮点击事件
        refreshButton.setOnClickListener {
            if (checkLocationPermission()) {
                getCurrentLocationAndWeather()
            } else {
                requestLocationPermission()
            }
        }
    }

    private fun checkLocationPermission(): Boolean {
        return ContextCompat.checkSelfPermission(
            this,
            Manifest.permission.ACCESS_FINE_LOCATION
        ) == PackageManager.PERMISSION_GRANTED
    }

    private fun requestLocationPermission() {
        ActivityCompat.requestPermissions(
            this,
            arrayOf(
                Manifest.permission.ACCESS_FINE_LOCATION,
                Manifest.permission.ACCESS_COARSE_LOCATION
            ),
            REQUEST_LOCATION_PERMISSION
        )
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == REQUEST_LOCATION_PERMISSION) {
            if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                getCurrentLocationAndWeather()
            } else {
                Toast.makeText(this, "位置权限被拒绝，无法获取天气信息", Toast.LENGTH_LONG).show()
            }
        }
    }

    private fun getCurrentLocationAndWeather() {
        if (!checkLocationPermission()) return

        val location = getLastKnownLocation()
        if (location != null) {
            val latitude = location.latitude
            val longitude = location.longitude
            fetchWeatherData(latitude, longitude)
        } else {
            Toast.makeText(this, "无法获取位置信息", Toast.LENGTH_SHORT).show()
        }
    }

    private fun getLastKnownLocation(): Location? {
        val providers = locationManager.getProviders(true)
        var bestLocation: Location? = null
        for (provider in providers) {
            if (ActivityCompat.checkSelfPermission(
                    this,
                    Manifest.permission.ACCESS_FINE_LOCATION
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                continue
            }
            val l = locationManager.getLastKnownLocation(provider) ?: continue
            if (bestLocation == null || l.accuracy < bestLocation.accuracy) {
                bestLocation = l
            }
        }
        return bestLocation
    }

    private fun fetchWeatherData(lat: Double, lon: Double) {
        // 使用OpenWeatherMap API获取天气数据
        val apiKey = "YOUR_API_KEY" // 需要在OpenWeatherMap注册获取API Key
        val url = "https://api.openweathermap.org/data/2.5/weather?lat=$lat&lon=$lon&appid=$apiKey&units=metric&lang=zh_cn"

        val request = Request.Builder()
            .url(url)
            .build()

        client.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                runOnUiThread {
                    Toast.makeText(this@MainActivity, "网络请求失败", Toast.LENGTH_SHORT).show()
                }
            }

            override fun onResponse(call: Call, response: Response) {
                if (!response.isSuccessful) {
                    runOnUiThread {
                        Toast.makeText(this@MainActivity, "获取天气数据失败", Toast.LENGTH_SHORT).show()
                    }
                    return
                }

                val responseData = response.body?.string()
                Log.d("WeatherApp", "Response: $responseData")

                val weatherData = parseWeatherData(responseData)

                runOnUiThread {
                    if (weatherData != null) {
                        temperatureText.text = "${weatherData.main?.temp?.toInt()}°C"
                        locationText.text = "${weatherData.name}, ${weatherData.sys?.country}"
                        weatherText.text = weatherData.weather?.get(0)?.description ?: "未知"

                        // 保存到本地存储
                        saveWeatherData(weatherData)
                    } else {
                        Toast.makeText(this@MainActivity, "解析天气数据失败", Toast.LENGTH_SHORT).show()
                    }
                }
            }
        })
    }

    private fun parseWeatherData(json: String?): WeatherResponse? {
        if (json.isNullOrEmpty()) return null
        try {
            val responseType: Type = object : TypeToken<WeatherResponse>() {}.type
            return gson.fromJson(json, responseType)
        } catch (e: Exception) {
            Log.e("WeatherApp", "解析JSON失败: ${e.message}")
            return null
        }
    }

    private fun saveWeatherData(data: WeatherResponse) {
        val sharedPreferences = getSharedPreferences("weather_data", Context.MODE_PRIVATE)
        val editor = sharedPreferences.edit()
        editor.putString("last_weather_data", gson.toJson(data))
        editor.putLong("last_update_time", System.currentTimeMillis())
        editor.apply()
    }

    private fun loadSavedWeatherData() {
        val sharedPreferences = getSharedPreferences("weather_data", Context.MODE_PRIVATE)
        val jsonData = sharedPreferences.getString("last_weather_data", null)
        val lastUpdateTime = sharedPreferences.getLong("last_update_time", 0)

        if (jsonData != null && System.currentTimeMillis() - lastUpdateTime < 300000) { // 5分钟内有效
            val weatherData = gson.fromJson(jsonData, WeatherResponse::class.java)
            if (weatherData != null) {
                runOnUiThread {
                    temperatureText.text = "${weatherData.main?.temp?.toInt()}°C"
                    locationText.text = "${weatherData.name}, ${weatherData.sys?.country}"
                    weatherText.text = weatherData.weather?.get(0)?.description ?: "未知"
                }
            }
        }
    }

    // 数据模型类
    data class WeatherResponse(
        val name: String,
        val main: Main?,
        val weather: List<Weather>?,
        val sys: Sys?
    )

    data class Main(
        val temp: Double,
        val feels_like: Double,
        val temp_min: Double,
        val temp_max: Double,
        val pressure: Int,
        val humidity: Int
    )

    data class Weather(
        val id: Int,
        val main: String,
        val description: String,
        val icon: String
    )

    data class Sys(
        val country: String,
        val sunrise: Long,
        val sunset: Long
    )
}