package me.jiahuan.android.audiovideosample.encoder

import android.content.Context
import android.opengl.GLES20
import me.jiahuan.android.audiovideosample.R
import me.jiahuan.android.audiovideosample.egl.EGLRenderer
import me.jiahuan.android.audiovideosample.util.ShaderUtils
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer

class SurfaceRenderer(private val context: Context, private val textureId: Int) : EGLRenderer {

    // 顶点坐标，三角形带绘制，左下角开始
    private val vertexData = floatArrayOf(
        -1f, -1f,
        1f, -1f,
        -1f, 1f,
        1f, 1f
    )

    // 顶点坐标Buffer
    private lateinit var vertexBuffer: FloatBuffer

    // 纹理坐标，与顶点坐标对应
    private val fragmentData = floatArrayOf(
        0f, 0f,
        1f, 0f,
        0f, 1f,
        1f, 1f
    )

    // 纹理坐标Buffer
    private lateinit var fragmentBuffer: FloatBuffer

    private var programId = 0
    // 顶点
    private var vPositionId = 0
    // 纹理
    private var fPositionId = 0

    private var vboId = 0

    init {
        initialize()
    }

    private fun initialize() {
        // 顶点坐标Buffer
        // 底层开辟内存空间
        vertexBuffer = ByteBuffer.allocateDirect(vertexData.size * 4)
            .order(ByteOrder.nativeOrder())
            .asFloatBuffer()
            .put(vertexData)

        // 指针指向内存起始位置
        vertexBuffer.position(0)


        // 纹理坐标Buffer
        // 底层开辟内存空间
        fragmentBuffer = ByteBuffer.allocateDirect(fragmentData.size * 4)
            .order(ByteOrder.nativeOrder())
            .asFloatBuffer()
            .put(fragmentData)

        // 指针指向内存起始位置
        fragmentBuffer.position(0)
    }

    override fun onSurfaceCreated(width: Int, height: Int) {
        // 顶点Shader源码
        val vertexSource = ShaderUtils.getRawResourceContent(context, R.raw.vertex_shader)
        val fragmentSource =
            ShaderUtils.getRawResourceContent(context, R.raw.fragment_shader)
        // 获取program
        programId = ShaderUtils.createProgram(vertexSource, fragmentSource)


        vPositionId = GLES20.glGetAttribLocation(programId, "v_Position")
        fPositionId = GLES20.glGetAttribLocation(programId, "f_Position")


        // VBO
        val vboIds = IntArray(1)
        GLES20.glGenBuffers(1, vboIds, 0)
        vboId = vboIds[0]
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboId)
        // 开辟内存空间
        GLES20.glBufferData(
            GLES20.GL_ARRAY_BUFFER,
            vertexData.size * 4 + fragmentData.size * 4,
            null,
            GLES20.GL_STATIC_DRAW
        )
        // 顶点数据
        GLES20.glBufferSubData(GLES20.GL_ARRAY_BUFFER, 0, vertexData.size * 4, vertexBuffer)
        // 纹理数据
        GLES20.glBufferSubData(
            GLES20.GL_ARRAY_BUFFER,
            vertexData.size * 4,
            fragmentData.size * 4,
            fragmentBuffer
        )
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0)
    }

    override fun onSurfaceChanged(width: Int, height: Int) {
        GLES20.glViewport(0, 0, width, height)
    }

    override fun onDrawFrame() {

        //清空颜色
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT)
        //设置背景颜色
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f)

        // 程序生效
        GLES20.glUseProgram(programId)

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId)
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboId)


        // 生效顶点
        GLES20.glEnableVertexAttribArray(vPositionId)
        GLES20.glVertexAttribPointer(vPositionId, 2, GLES20.GL_FLOAT, false, 8, 0)

        // 生效纹理
        GLES20.glEnableVertexAttribArray(fPositionId)
        GLES20.glVertexAttribPointer(fPositionId, 2, GLES20.GL_FLOAT, false, 8, vertexData.size * 4)

        // 绘制
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4)

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0)
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0)
    }
}