package indi.pplong.tplayer

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import android.widget.Toast
import indi.pplong.tplayer.databinding.ActivityMainBinding
import indi.pplong.tplayer.ffmpeg.FFmpegActivity

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method
        binding.sampleText.text = stringFromJNI()

        Toast.makeText(this, stringFFmpegVersion(), Toast.LENGTH_SHORT).show();

        binding.btnMain.setOnClickListener {
            startActivity(Intent(this, FFmpegActivity::class.java))
        }
    }

    /**
     * A native method that is implemented by the 'tplayer' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    external fun stringFFmpegVersion(): String

    companion object {
        // Used to load the 'tplayer' library on application startup.
        init {
            System.loadLibrary("tplayer")
        }
    }
}