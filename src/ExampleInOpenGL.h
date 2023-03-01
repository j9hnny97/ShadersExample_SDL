#ifndef OPENGL_EXAMPLE_H
#define OPENGL_EXAMPLE_H

#include <SDL.h>
#include <SDL_opengl.h>

class ExampleInOpenGL
{
public:
    ExampleInOpenGL(SDL_Renderer* renderer);
    ~ExampleInOpenGL();

    void tick(SDL_Renderer* renderer) const;

private:
    class ShaderProgram
    {
    public:
        ShaderProgram();

        void applyShader(SDL_Renderer* renderer, SDL_Rect rect, SDL_Texture* target) const;
        void compileProgram(const char* vtxFile, const char* fragFile);
        GLuint compileShader(const char* source, GLuint shaderType) const;
        bool initExtensions();

    private:
        struct VertexCoords
        {
            GLfloat x0 = 0.0f, y0 = 0.0f;
            GLfloat x1 = 0.0f, y1 = 0.0f;
        };

        PFNGLCREATESHADERPROC _glCreateShader = nullptr;
        PFNGLSHADERSOURCEPROC _glShaderSource = nullptr;
        PFNGLCOMPILESHADERPROC _glCompileShader = nullptr;
        PFNGLGETSHADERIVPROC _glGetShaderIV = nullptr;
        PFNGLGETSHADERINFOLOGPROC _glGetShaderInfoLog = nullptr;
        PFNGLDELETESHADERPROC _glDeleteShader = nullptr;
        PFNGLATTACHSHADERPROC _glAttachShader = nullptr;
        PFNGLCREATEPROGRAMPROC _glCreateProgram = nullptr;
        PFNGLLINKPROGRAMPROC _glLinkProgram = nullptr;
        PFNGLVALIDATEPROGRAMPROC _glValidateProgram = nullptr;
        PFNGLGETPROGRAMIVPROC _glGetProgramIV = nullptr;
        PFNGLGETPROGRAMINFOLOGPROC _glGetProgramInfoLog = nullptr;
        PFNGLUSEPROGRAMPROC _glUseProgram = nullptr;
        PFNGLGETUNIFORMLOCATIONPROC _glGetUniformLocation = nullptr;
        PFNGLUNIFORM1FPROC _glUniform1f = nullptr;

        GLuint _programId;

    } _shaderProgram;

    SDL_Texture* _backgroundTexture = nullptr;
    SDL_Rect _backgroundTextureRect;

    SDL_Texture* _texture = nullptr;
    SDL_Rect _textureRect;
};

#endif
