
#include "ExampleInOpenGL.h"

#include "Definitions.h"

#include <fstream>
#include <iostream>
#include <SDL_image.h>

ExampleInOpenGL::ExampleInOpenGL(SDL_Renderer* renderer)
{
    SDL_RendererInfo rendererInfo;
    SDL_GetRendererInfo(renderer, &rendererInfo);

    if (!strncmp(rendererInfo.name, "opengl", 6))
    {
        std::cout << "Using OpenGL." << std::endl;

        // If you want to use GLEW or some other GL extension handler, do it here!
        if (!_shaderProgram.initExtensions())
        {
            std::cerr << "Couldn't init extensions." << std::endl;
            SDL_Quit();
            exit(-1);
        }

        // compile program
        _shaderProgram.compileProgram(makePath("std.vertex").c_str(), makePath("crt.fragment").c_str());
    }

    SDL_Surface* surface = IMG_Load(makePath("bmt.png").c_str());
    _texture = SDL_CreateTextureFromSurface(renderer, surface);
    _textureRect = { (SCREEN_WIDTH - surface->w) / 2, (SCREEN_HEIGHT - surface->h) / 2, surface->w, surface->h };
    SDL_FreeSurface(surface);

    SDL_Surface* backgroundSurface = IMG_Load(makePath("background.png").c_str());
    _backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    _backgroundTextureRect = { (SCREEN_WIDTH - backgroundSurface->w) / 2, (SCREEN_HEIGHT - backgroundSurface->h) / 2, backgroundSurface->w, backgroundSurface->h };
    SDL_FreeSurface(backgroundSurface);

    SDL_SetTextureBlendMode(_backgroundTexture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_BLEND);
}

ExampleInOpenGL::~ExampleInOpenGL()
{
    SDL_DestroyTexture(_texture);
    SDL_DestroyTexture(_backgroundTexture);
}

void ExampleInOpenGL::tick(SDL_Renderer* renderer) const
{
    static SDL_Rect const screenRect{ 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };

    SDL_SetRenderTarget(renderer, nullptr);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &screenRect);
    SDL_RenderCopy(renderer, _backgroundTexture, nullptr, &_backgroundTextureRect);

    _shaderProgram.applyShader(renderer, _backgroundTextureRect, _backgroundTexture);

    SDL_RenderCopy(renderer, _texture, nullptr, &_textureRect);
}

ExampleInOpenGL::ShaderProgram::ShaderProgram()
    : _programId(0)
{
}

void ExampleInOpenGL::ShaderProgram::applyShader(SDL_Renderer* renderer, SDL_Rect rect, SDL_Texture* target) const
{
    GLint oldProgramId = 0;
    auto const oldTarget = SDL_GetRenderTarget(renderer);

    // detach texture
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

    if (_programId != 0)
    {
        glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgramId);
        _glUseProgram(_programId);

        GLint redChannel = _glGetUniformLocation(_programId, "redChannel");
        GLint greenChannel = _glGetUniformLocation(_programId, "greenChannel");
        GLint blueChannel = _glGetUniformLocation(_programId, "blueChannel");
        GLint alphaChannel = _glGetUniformLocation(_programId, "alphaChannel");
        _glUniform1f(redChannel, 0.0f);
        _glUniform1f(greenChannel, 0.0f);
        _glUniform1f(blueChannel, 0.0f);
        _glUniform1f(alphaChannel, 0.75f);
    }

    glBegin(GL_TRIANGLE_STRIP);

    // setup shader coords and vertex
    {
        VertexCoords screenCoords;
        screenCoords.x0 = rect.x;
        screenCoords.y0 = rect.y;
        screenCoords.x1 = rect.x + rect.w;
        screenCoords.y1 = rect.y + rect.h;

        VertexCoords texCoords;
        texCoords.x0 = 0.0f;
        texCoords.y0 = 0.0f;
        texCoords.x1 = 1.0f;
        texCoords.y1 = 1.0f;

        // top left
        glTexCoord2f(texCoords.x0, texCoords.y0);
        glVertex2f(screenCoords.x0, screenCoords.y0);

        // top right
        glTexCoord2f(texCoords.x1, texCoords.y0);
        glVertex2f(screenCoords.x1, screenCoords.y0);

        // bottom left
        glTexCoord2f(texCoords.x0, texCoords.y1);
        glVertex2f(screenCoords.x0, screenCoords.y1);

        // bottom right
        glTexCoord2f(texCoords.x1, texCoords.y1);
        glVertex2f(screenCoords.x1, screenCoords.y1);
    }

    glEnd();

    if (_programId != 0 && oldProgramId != 0)
    {
        _glUseProgram(oldProgramId);
    }

    SDL_SetRenderTarget(renderer, oldTarget);
}

void ExampleInOpenGL::ShaderProgram::compileProgram(const char* vtxFile, const char* fragFile)
{
    _programId = _glCreateProgram();

    std::ifstream f(vtxFile);
    std::string source((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    GLuint const vtxShaderId = compileShader(source.c_str(), GL_VERTEX_SHADER);

    f = std::ifstream(fragFile);
    source = std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    GLuint const fragShaderId = compileShader(source.c_str(), GL_FRAGMENT_SHADER);

    if (vtxShaderId && fragShaderId)
    {
        // associate shader with program
        _glAttachShader(_programId, vtxShaderId);
        _glAttachShader(_programId, fragShaderId);
        _glLinkProgram(_programId);
        _glValidateProgram(_programId);

        // check the status of the compile/link
        GLint logLen;
        _glGetProgramIV(_programId, GL_INFO_LOG_LENGTH, &logLen);

        if (logLen > 0)
        {
            char* log = (char*)malloc(logLen * sizeof(char));
            // Show any errors as appropriate
            _glGetProgramInfoLog(_programId, logLen, &logLen, log);
            std::cout << "Program info: " << log << std::endl;
            free(log);
        }
    }
    else
    {
        throw std::exception("whoops");
    }

    if (vtxShaderId)
    {
        _glDeleteShader(vtxShaderId);
    }

    if (fragShaderId)
    {
        _glDeleteShader(fragShaderId);
    }

    std::cout << "programId = " << _programId << std::endl;
}

GLuint ExampleInOpenGL::ShaderProgram::compileShader(const char* source, GLuint shaderType) const
{
    std::cout << "Compiling shader: " << source << std::endl;

    // Create ID for shader
    GLuint result = _glCreateShader(shaderType);

    // Define shader text
    _glShaderSource(result, 1, &source, nullptr);

    // Compile shader
    _glCompileShader(result);

    //Check vertex shader for errors
    GLint shaderCompiled = GL_FALSE;
    _glGetShaderIV(result, GL_COMPILE_STATUS, &shaderCompiled);

    if (shaderCompiled != GL_TRUE)
    {
        std::cout << "Compilation error: " << result << std::endl;
        GLint logLength;
        _glGetShaderIV(result, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 0)
        {
            GLchar* log = (GLchar*)malloc(logLength);
            _glGetShaderInfoLog(result, logLength, &logLength, log);
            std::cout << "Shader compile log:" << log << std::endl;
            free(log);
        }

        _glDeleteShader(result);
        result = 0;
    }
    else
    {
        std::cout << "Shader compilation successful (result=" << result << ")" << std::endl;
    }

    return result;
}

bool ExampleInOpenGL::ShaderProgram::initExtensions()
{
    _glCreateShader = static_cast<PFNGLCREATESHADERPROC>(SDL_GL_GetProcAddress("glCreateShader"));
    _glShaderSource = static_cast<PFNGLSHADERSOURCEPROC>(SDL_GL_GetProcAddress("glShaderSource"));
    _glCompileShader = static_cast<PFNGLCOMPILESHADERPROC>(SDL_GL_GetProcAddress("glCompileShader"));
    _glGetShaderIV = static_cast<PFNGLGETSHADERIVPROC>(SDL_GL_GetProcAddress("glGetShaderiv"));
    _glGetShaderInfoLog = static_cast<PFNGLGETSHADERINFOLOGPROC>(SDL_GL_GetProcAddress("glGetShaderInfoLog"));
    _glDeleteShader = static_cast<PFNGLDELETESHADERPROC>(SDL_GL_GetProcAddress("glDeleteShader"));
    _glAttachShader = static_cast<PFNGLATTACHSHADERPROC>(SDL_GL_GetProcAddress("glAttachShader"));
    _glCreateProgram = static_cast<PFNGLCREATEPROGRAMPROC>(SDL_GL_GetProcAddress("glCreateProgram"));
    _glLinkProgram = static_cast<PFNGLLINKPROGRAMPROC>(SDL_GL_GetProcAddress("glLinkProgram"));
    _glValidateProgram = static_cast<PFNGLVALIDATEPROGRAMPROC>(SDL_GL_GetProcAddress("glValidateProgram"));
    _glGetProgramIV = static_cast<PFNGLGETPROGRAMIVPROC>(SDL_GL_GetProcAddress("glGetProgramiv"));
    _glGetProgramInfoLog = static_cast<PFNGLGETPROGRAMINFOLOGPROC>(SDL_GL_GetProcAddress("glGetProgramInfoLog"));
    _glUseProgram = static_cast<PFNGLUSEPROGRAMPROC>(SDL_GL_GetProcAddress("glUseProgram"));
    _glGetUniformLocation = static_cast<PFNGLGETUNIFORMLOCATIONPROC>(SDL_GL_GetProcAddress("glGetUniformLocation"));
    _glUniform1f = static_cast<PFNGLUNIFORM1FPROC>(SDL_GL_GetProcAddress("glUniform1f"));

    return _glCreateShader
        && _glShaderSource
        && _glCompileShader
        && _glGetShaderIV
        && _glGetShaderInfoLog
        && _glDeleteShader
        && _glAttachShader
        && _glCreateProgram
        && _glLinkProgram
        && _glValidateProgram
        && _glGetProgramIV
        && _glGetProgramInfoLog
        && _glUseProgram
        && _glGetUniformLocation
        && _glUniform1f;
}
