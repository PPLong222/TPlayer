package indi.pplong.tplayer.ffmpeg

import android.os.Bundle
import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import indi.pplong.tplayer.R
import indi.pplong.tplayer.databinding.ActivityFfmpegBinding
import java.io.File

class FFmpegActivity : AppCompatActivity() {
    private val TAG = javaClass.name
    private lateinit var binding: ActivityFfmpegBinding
    private var player: Long = -1L
    private var path = ""
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        binding = ActivityFfmpegBinding.inflate(layoutInflater)
        setContentView(binding.root)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }
        path = filesDir.absolutePath + "/family.mp4"

        Log.d(TAG, path)
        initPlayer()
        binding.buttonPlay.setOnClickListener { play(player) }
        binding.buttonPause.setOnClickListener { pause(player) }
        binding.buttonContinue.setOnClickListener { resume(player) }
    }

    private fun initPlayer() {
        Log.d(TAG, "Init Player")

        if (File(path).exists()) {
            Log.d(TAG, "File exist, begin create player")
            binding.sfv.holder.addCallback(object : SurfaceHolder.Callback {
                override fun surfaceCreated(holder: SurfaceHolder) {
                    if (player == -1L) {
                        Log.d(TAG, "Player created begin")
                        player = createPlayer(path, holder.surface)
                        Log.d(TAG, "Player created done")
                    }
                }

                override fun surfaceChanged(
                    holder: SurfaceHolder,
                    format: Int,
                    width: Int,
                    height: Int
                ) {

                }

                override fun surfaceDestroyed(holder: SurfaceHolder) {

                }

            })
        } else {
            Toast.makeText(this, "File not exist", Toast.LENGTH_SHORT).show();
        }
    }

    private external fun createPlayer(path: String, surface: Surface): Long

    private external fun play(player: Long)

    private external fun pause(player: Long)

    private external fun resume(player: Long)

    private external fun test()

    companion object {
        init {
            System.loadLibrary("tplayer")
        }
    }
}