package me.jiahuan.android.audiovideosample.camera

import android.content.Context
import android.graphics.SurfaceTexture
import android.opengl.GLES11Ext
import android.opengl.GLES20
import android.opengl.Matrix
import android.util.Log
import me.jiahuan.android.audiovideosample.R
import me.jiahuan.android.audiovideosample.egl.EGLRenderer
import me.jiahuan.android.audiovideosample.util.ShaderUtils
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer


class CameraFBORenderer(private val context: Context) : EGLRenderer,
    SurfaceTexture.OnFrameAvailableListener {

    companion object {
        private const val TAG = "CameraRender"
    }

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
        0f, 1f,
        1f, 1f,
        0f, 0f,
        1f, 0f
    )

    // 纹理坐标Buffer
    private lateinit var fragmentBuffer: FloatBuffer


    private var programId = 0

    // 顶点
    private var vPositionId = 0
    // 纹理
    private var fPositionId = 0

    private var vboId = 0
    private var fboId = 0

    private var cameraTextureId = 0
    private var fboBindTextureId = 0

    private var umatrix = 0
    private val matrix = FloatArray(16)

    private lateinit var surfaceTexture: SurfaceTexture

    private val cameraFBORender by lazy {
        CameraRenderer(context)
    }


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

        // 初始化矩阵
        Matrix.setIdentityM(matrix, 0)
    }

    override fun onDrawFrame() {
        surfaceTexture.updateTexImage()

        //清空颜色
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT)
        //设置背景颜色
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f)

        // 程序生效
        GLES20.glUseProgram(programId)

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId)
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboId)
        GLES20.glUniformMatrix4fv(umatrix, 1, false, matrix, 0)


        // 生效顶点
        GLES20.glEnableVertexAttribArray(vPositionId)
        GLES20.glVertexAttribPointer(vPositionId, 2, GLES20.GL_FLOAT, false, 8, 0)

        // 生效纹理
        GLES20.glEnableVertexAttribArray(fPositionId)
        GLES20.glVertexAttribPointer(fPositionId, 2, GLES20.GL_FLOAT, false, 8, vertexData.size * 4)

        // 绘制
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4)

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0)
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0)

        cameraFBORender.onDrawFrame(fboBindTextureId)
    }

    override fun onSurfaceChanged(width: Int, height: Int) {
        cameraFBORender.onSurfaceChanged(width, height)

        GLES20.glViewport(0, 0, width, height)

        Matrix.rotateM(matrix, 0, 90f, 0f, 0f, 1f)
    }

    override fun onSurfaceCreated(width: Int, height: Int) {
        cameraFBORender.onSurfaceCreated(width, height)
        // 顶点Shader源码
        val vertexSource =
            ShaderUtils.getRawResourceContent(context, R.raw.vertex_camera_shader)
        val fragmentSource =
            ShaderUtils.getRawResourceContent(context, R.raw.fragment_camera_shader)
        // 获取program
        programId = ShaderUtils.createProgram(vertexSource, fragmentSource)


        vPositionId = GLES20.glGetAttribLocation(programId, "v_Position")
        fPositionId = GLES20.glGetAttribLocation(programId, "f_Position")
        umatrix = GLES20.glGetUniformLocation(programId, "u_Matrix")


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


        // FBO
        val fboIds = IntArray(1)
        GLES20.glGenBuffers(1, fboIds, 0)
        fboId = fboIds[0]
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId)

        // 创建纹理
        val textureIds = IntArray(1)
        GLES20.glGenTextures(1, textureIds, 0)
        fboBindTextureId = textureIds[0]

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, fboBindTextureId)

        // 设置环绕方式
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT)

        // 设置过滤方式
        GLES20.glTexParameteri(
            GLES20.GL_TEXTURE_2D,
            GLES20.GL_TEXTURE_MIN_FILTER,
            GLES20.GL_LINEAR
        )
        GLES20.glTexParameteri(
            GLES20.GL_TEXTURE_2D,
            GLES20.GL_TEXTURE_MAG_FILTER,
            GLES20.GL_LINEAR
        )

        GLES20.glTexImage2D(
            GLES20.GL_TEXTURE_2D,
            0,
            GLES20.GL_RGBA,
            width,
            height,
            0,
            GLES20.GL_RGBA,
            GLES20.GL_UNSIGNED_BYTE,
            null
        )
        GLES20.glFramebufferTexture2D(
            GLES20.GL_FRAMEBUFFER,
            GLES20.GL_COLOR_ATTACHMENT0,
            GLES20.GL_TEXTURE_2D,
            fboBindTextureId,
            0
        )
        if (GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER) != GLES20.GL_FRAMEBUFFER_COMPLETE) {
            Log.w(TAG, "fbo wrong")
        } else {
            Log.d(TAG, "fbo success")
        }

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0)
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0)

        // 扩展纹理  相机纹理
        val exTextureIds = IntArray(1)
        GLES20.glGenTextures(1, exTextureIds, 0)
        cameraTextureId = exTextureIds[0]

        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, cameraTextureId)
        GLES20.glTexParameteri(
            GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
            GLES20.GL_TEXTURE_WRAP_S,
            GLES20.GL_REPEAT
        )
        GLES20.glTexParameteri(
            GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
            GLES20.GL_TEXTURE_WRAP_T,
            GLES20.GL_REPEAT
        )
        GLES20.glTexParameteri(
            GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
            GLES20.GL_TEXTURE_MIN_FILTER,
            GLES20.GL_LINEAR
        )
        GLES20.glTexParameteri(
            GLES11Ext.GL_TEXTURE_EXTERNAL_OES,
            GLES20.GL_TEXTURE_MAG_FILTER,
            GLES20.GL_LINEAR
        )

        surfaceTexture = SurfaceTexture(cameraTextureId)
        surfaceTexture.setOnFrameAvailableListener(this)

        onSurfaceCreateListener?.onSurfaceCreated(surfaceTexture, fboBindTextureId)

        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, 0)
    }

    override fun onFrameAvailable(surfaceTexture: SurfaceTexture?) {
        // 录制

    }


    var onSurfaceCreateListener: OnSurfaceCreateListener? = null

    interface OnSurfaceCreateListener {
        fun onSurfaceCreated(surfaceTexture: SurfaceTexture, textureId: Int)
    }
}