package me.jiahuan.android.audiovideosample.texture

import android.content.Context
import android.graphics.BitmapFactory
import android.opengl.GLES20
import android.opengl.GLUtils
import android.util.Log
import me.jiahuan.android.audiovideosample.R
import me.jiahuan.android.audiovideosample.egl.EGLRenderer
import me.jiahuan.android.audiovideosample.util.ShaderUtils
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer

class TextureFBORenderer(private val context: Context) : EGLRenderer {

    companion object {
        private const val TAG = "TextureRenderer"
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
    // 纹理ID
    private var textureId = 0
    private var vboId = 0
    private var fboId = 0

    private var imageTextureId = 0

    private val fboRender = TextureRenderer(context)

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

    override fun onDrawFrame() {
        // 如果开启了fbo，则绘制在离屏中
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId)

        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT)
        GLES20.glClearColor(1f, 0f, 0f, 1f)

        // 程序生效
        GLES20.glUseProgram(programId)

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, imageTextureId)
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboId)


        // 生效顶点
        GLES20.glEnableVertexAttribArray(vPositionId)
        GLES20.glVertexAttribPointer(vPositionId, 2, GLES20.GL_FLOAT, false, 8, 0)

        // 生效纹理
        GLES20.glEnableVertexAttribArray(fPositionId)
        GLES20.glVertexAttribPointer(fPositionId, 2, GLES20.GL_FLOAT, false, 8, vertexData.size * 4)

        // 绘制
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4)

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0)
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0)
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0)

        fboRender.onDrawFrame(textureId)
    }

    override fun onSurfaceChanged(width: Int, height: Int) {
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId)
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
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0)

        fboRender.onSurfaceChanged(width, height)
        GLES20.glViewport(0, 0, width, height)
    }

    override fun onSurfaceCreated() {
        fboRender.onSurfaceCreate()
        // 顶点Shader源码
        val vertexSource = ShaderUtils.getRawResourceContent(context, R.raw.vertex_shader)
        val fragmentSource = ShaderUtils.getRawResourceContent(context, R.raw.fragment_shader)
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


        // 创建纹理
        val textureIds = IntArray(1)
        GLES20.glGenTextures(1, textureIds, 0)
        textureId = textureIds[0]

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId)
        // 设置环绕方式
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT)

        // 设置过滤方式
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR)

        // 创建FBO，绑定到上面创建的纹理
        val fboIds = IntArray(1)
        GLES20.glGenBuffers(1, fboIds, 0)
        fboId = fboIds[0]
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId)
        GLES20.glTexImage2D(
            GLES20.GL_TEXTURE_2D,
            0,
            GLES20.GL_RGBA,
            0,
            0,
            0,
            GLES20.GL_RGBA,
            GLES20.GL_UNSIGNED_BYTE,
            null
        )

        // 绑定一个纹理
        GLES20.glFramebufferTexture2D(
            GLES20.GL_FRAMEBUFFER,
            GLES20.GL_COLOR_ATTACHMENT0,
            GLES20.GL_TEXTURE_2D,
            textureId,
            0
        )

        if (GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER) != GLES20.GL_FRAMEBUFFER_COMPLETE) {
            Log.w(TAG, "fbo wrong")
        } else {
            Log.d(TAG, "fbo success")
        }

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0)
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0)

        imageTextureId = loadTexture(R.drawable.texture)
    }


    private fun loadTexture(src: Int): Int {
        val textureIds = IntArray(1)
        GLES20.glGenTextures(1, textureIds, 0)
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureIds[0])
        // 设置环绕方式
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT)
        // 设置过滤方式
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR)
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR)

        val bitmap = BitmapFactory.decodeResource(context.resources, src)
        GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0)
        bitmap.recycle()
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0)

        return textureIds[0]
    }
}