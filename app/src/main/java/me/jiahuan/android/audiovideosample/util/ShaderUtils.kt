package me.jiahuan.android.audiovideosample.util

import android.content.Context
import android.opengl.GLES20
import java.io.BufferedReader
import java.io.InputStreamReader

object ShaderUtils {

    /**
     * 获取RAW文件的内容
     */
    fun getRawResourceContent(context: Context, rawResId: Int): String {
        val inputStream = context.resources.openRawResource(rawResId)
        val bufferedReader = BufferedReader(InputStreamReader(inputStream))
        val sb = StringBuffer()
        var line = bufferedReader.readLine()
        while (line != null) {
            sb.append(line).append("\n")
            line = bufferedReader.readLine()
        }
        bufferedReader.close()
        return sb.toString()
    }

    /**
     * 加载着色器
     */
    private fun loadShader(shaderType: Int, source: String): Int {
        var shader = GLES20.glCreateShader(shaderType)
        if (shader != 0) {
            GLES20.glShaderSource(shader, source)
            // 编译
            GLES20.glCompileShader(shader)

            // 检测编译是否成功
            val compile = IntArray(1)
            GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS, compile, 0)
            if (compile[0] != GLES20.GL_TRUE) {
                GLES20.glDeleteShader(shader)
                shader = 0
            }
            return shader
        } else {
            return 0
        }
    }

    /**
     * 创建 m_Program
     */
    fun createProgram(vertexSource: String, fragmentSource: String): Int {
        val vertexShader = loadShader(GLES20.GL_VERTEX_SHADER, vertexSource)
        val fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentSource)

        if (vertexShader != 0 && fragmentShader != 0) {
            // 加载成功
            val program = GLES20.glCreateProgram()

            GLES20.glAttachShader(program, vertexShader)
            GLES20.glAttachShader(program, fragmentShader)

            GLES20.glLinkProgram(program)

            // 检查链接程序是否成功
            val linkStatus = IntArray(1)

            GLES20.glGetProgramiv(program, GLES20.GL_LINK_STATUS, linkStatus, 0)
            if (linkStatus[0] != GLES20.GL_TRUE) {
                GLES20.glDeleteProgram(program)
                return 0
            }
            return program
        }
        return 0
    }
}
