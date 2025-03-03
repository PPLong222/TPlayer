package indi.pplong.tplayer.player

import android.content.Context
import android.os.Handler
import android.os.Looper
import android.util.AttributeSet
import android.util.Log
import android.view.LayoutInflater
import android.view.Surface
import android.view.SurfaceHolder
import android.widget.SeekBar
import android.widget.SeekBar.OnSeekBarChangeListener
import android.widget.Toast
import androidx.constraintlayout.widget.ConstraintLayout
import indi.pplong.tplayer.R
import indi.pplong.tplayer.databinding.TplayerBinding
import java.io.File
import java.util.Locale

/**
 * Description:
 * @author PPLong
 * @date 2/4/25 7:05 PM
 */
class TPlayer : ConstraintLayout {
    private val TAG = javaClass.name
    private lateinit var binding: TplayerBinding
    private var player: Long = -1L
    private var path = ""
    private val handler = Handler(Looper.getMainLooper())

    constructor(context: Context) : super(context)
    constructor(context: Context, attrs: AttributeSet?) : super(context, attrs) {
        init(context)
    }

    constructor(context: Context, attrs: AttributeSet?, defStyleAttr: Int) : super(
        context,
        attrs,
        defStyleAttr
    ) {
        init(context)
    }

    constructor(
        context: Context,
        attrs: AttributeSet?,
        defStyleAttr: Int,
        defStyleRes: Int
    ) : super(context, attrs, defStyleAttr, defStyleRes) {
        init(context)

    }

    private fun init(context: Context) {
        val view = LayoutInflater.from(context).inflate(R.layout.tplayer, this, true)
        binding = TplayerBinding.bind(view)
        path = context.filesDir.absolutePath + "/family.mp4"
        initView()
    }

    private fun initView() {
        initPlayer()
        setListener()
        initHandler()
    }

    private fun initPlayer() {
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
            Toast.makeText(context, "File not exist", Toast.LENGTH_SHORT).show();
        }
    }

    private fun setListener() {
        binding.buttonPlay.setOnClickListener { play(player) }
        binding.buttonPause.setOnClickListener { pause(player) }
        binding.buttonContinue.setOnClickListener { seek(player, 2000) }
        binding.seekbar.setOnSeekBarChangeListener(object : OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {

            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                val progress = seekBar?.progress ?: 0
                if (player != -1L) {
                    val targetTimeMs = getPlayTime(player) * progress / 100
                    seek(player, targetTimeMs)
                }
            }
        })
    }

    private fun initHandler() {
        val runnable = object : Runnable {
            override fun run() {
                if (player != -1L) {
                    val curPos = getCurPos(player) / 1000.0
                    binding.tvProgress.text = context.getString(
                        R.string.current_progress_time,
                        String.format(Locale.getDefault(), "%.2f", curPos)
                    )
                }
                handler.postDelayed(this, 100L)
            }
        }
        handler.post(runnable)

    }


    private external fun createPlayer(path: String, surface: Surface): Long

    private external fun play(player: Long)

    private external fun pause(player: Long)

    private external fun resume(player: Long)

    private external fun getPlayTime(player: Long): Long

    private external fun getCurPos(player: Long): Long

    private external fun seek(player: Long, targetTimeMs: Long)

    companion object {
        init {
            System.loadLibrary("tplayer")
        }
    }
}