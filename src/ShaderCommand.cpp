//
//  ShaderCommand.cpp
//  solid_fluid_coupring
//
//  Created by 須之内俊樹 on 2024/07/12.
//

#include "ShaderCommand.hpp"


//テクスチャのバインド，エンコード等の処理を，void encode()にまとめたい．

// シェーダオブジェクトのコンパイル結果を表示する
// shader: シェーダオブジェクト名
// str: コンパイルエラーが発生した場所を示す文字列
GLboolean printShaderInfoLog(GLuint shader, const char *str)
{
    // コンパイル結果を取得する
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) std::cerr << "Compile Error in " << str << std::endl;
    // シェーダのコンパイル時のログの長さを取得する
    GLsizei bufSize;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &bufSize);
    if (bufSize > 1){
        // シェーダのコンパイル時のログの内容を取得する
        std::vector<GLchar> infoLog(bufSize);
        GLsizei length;
        glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
        std::cerr << &infoLog[0] << std::endl;
    }
    return static_cast<GLboolean>(status);
}

GLboolean printProgramInfoLog(GLuint program)
{
    // コンパイル結果を取得する
    GLint status;
    glGetShaderiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) std::cerr << "Link Error." << std::endl;
    // シェーダのコンパイル時のログの長さを取得する
    GLsizei bufSize;
    glGetShaderiv(program, GL_INFO_LOG_LENGTH , &bufSize);
    if (bufSize > 1){
        // シェーダのコンパイル時のログの内容を取得する
        std::vector<GLchar> infoLog(bufSize);
        GLsizei length;
        glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
        std::cerr << &infoLog[0] << std::endl;
    }
    return static_cast<GLboolean>(status);
}

//プログラムオブジェクトを作成する
GLuint createProgram(const char *vsrc,const char *fsrc)
{
    const GLuint program(glCreateProgram());
    if(vsrc != NULL){
        // バーテックスシェーダのシェーダオブジェクトを作成する
        const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
        glShaderSource(vobj,1,&vsrc,NULL);
        glCompileShader(vobj);
        //バーテックスシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
        if(printShaderInfoLog(vobj, "vertex shader"))glAttachShader(program,vobj);
        glDeleteShader(vobj);
    }
    
    if(fsrc != NULL){
        // フラグメントシェーダのシェーダオブジェクトを作成する
        const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
        glShaderSource(fobj, 1, &fsrc, NULL);
        glCompileShader(fobj);
        //フラグメントシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
        if(printShaderInfoLog(fobj, "fragment shader"))glAttachShader(program, fobj);
        glDeleteShader(fobj);
    }
    // プログラムオブジェクトをリンクする
    glBindAttribLocation(program, 0, "position");
    glBindFragDataLocation(program, 0, "fragment");
    glBindAttribLocation(program, 1, "color");
    glLinkProgram(program);
    
    if(printProgramInfoLog(program))return program;
    
    glDeleteProgram(program);
    return 0;
}

GLuint createComputeProgram(const char *csrc)
{
    const GLuint program(glCreateProgram());
    if(csrc != NULL)
    {
        GLuint cobj = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(cobj, 1, &csrc, NULL);
        glCompileShader(cobj);
        
        if(printShaderInfoLog(cobj, "compute shader"))glAttachShader(program,cobj);
        glDeleteShader(cobj);
    }
    glLinkProgram(program);
    
    if(printProgramInfoLog(program))return program;
    glDeleteProgram(program);
    return 0;
}
//シェーダのソースファイルを読み込んだメモリを返す
bool readShaderSource(const char *name, std::vector<GLchar> &buffer)
{
    //ファイル名がNull
    if(name == NULL)return false;

    //ソースファイルを開く
    std::ifstream file(name,std::ios::binary);
    if(file.fail())
    {
        //開けなかった
        std::cerr << "Error: Can't open source file:" << name << std::endl;
        return false;
    }
    //ファイルの末尾に移動し，現在位置(ファイルサイズ)を得る
    file.seekg(0L,std::ios::end);
    GLsizei length = static_cast<GLsizei>(file.tellg());
    //ファイルサイズのメモリを確保
    buffer.resize(length + 1);
    //ファイルを先頭から読み込む
    file.seekg(0L,std::ios::beg);
    file.read(buffer.data(), length);
    buffer[length] = '\0';
    if(file.fail())
    {
        //読み込めなかった
        std::cerr << "Error: Could not read source file" << name << std::endl;
        file.close();
        return false;
    }
    //読み込み成功
    file.close();
    return true;
}
// シェーダのソースファイルを読み込んでプログラムオブジェクトを作成する
// vert: バーテックスシェーダのソースファイル名
// frag: フラグメントシェーダのソースファイル名
GLuint loadVertFragProgram(const char *vert, const char *frag)
{
    // シェーダのソースファイルを読み込む
    std::vector<GLchar> vsrc;
    const bool vstat(readShaderSource(vert, vsrc));
    std::vector<GLchar> fsrc;
    const bool fstat(readShaderSource(frag, fsrc));
    // プログラムオブジェクトを作成する
    return vstat && fstat ? createProgram(vsrc.data(), fsrc.data()) : 0;
}
GLuint loadComputeProgram(const char *comp)
{
    // シェーダのソースファイルを読み込む
    std::vector<GLchar> csrc;
    const bool vstat(readShaderSource(comp, csrc));
    // プログラムオブジェクトを作成する
    return vstat ? createComputeProgram(csrc.data()) : 0;
}
void deleteComputeShaderProgram(GLuint program) {
    glDeleteProgram(program);
}