
#ifndef ShaderCommand_hpp
#define ShaderCommand_hpp

#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <random>

// シェーダオブジェクトのコンパイル結果を表示する
// shader: シェーダオブジェクト名
// str: コンパイルエラーが発生した場所を示す文字列


GLboolean printShaderInfoLog(GLuint shader, const char *str);
GLboolean printProgramInfoLog(GLuint program);
//プログラムオブジェクトを作成する
GLuint createProgram(const char *vsrc,const char *fsrc);
GLuint createComputeProgram(const char *csrc);
//シェーダのソースファイルを読み込んだメモリを返す
bool readShaderSource(const char *name, std::vector<GLchar> &buffer);
// シェーダのソースファイルを読み込んでプログラムオブジェクトを作成する
// vert: バーテックスシェーダのソースファイル名
// frag: フラグメントシェーダのソースファイル名
GLuint loadVertFragProgram(const char *vert, const char *frag);
GLuint loadComputeProgram(const char *comp);
void deleteComputeShaderProgram(GLuint program);
#endif

