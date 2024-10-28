#ifndef Matrix4x4_h
#define Matrix4x4_h

#include <algorithm>
#include <GL/glew.h>
#include <cmath>
#include <iostream>
//変換行列
class Matrix4x4{
    //変換行列の要素
    GLfloat matrix[16];
public:
    //コンストラクタ
    Matrix4x4(){}
    // 配列の内容で初期化するコンストラクタ
    // a: GLfloat 型の 16 要素の配列
    Matrix4x4(const GLfloat *a)
    {
        std::copy(a,a+16,matrix);
    }
    // 行列の要素を右辺値として参照する
    const GLfloat &operator[](std::size_t i) const
    {
        return matrix[i];
    }
    //行列の要素を左辺値として参照する
    GLfloat &operator[](std::size_t i)
    {
        return matrix[i];
    }
    //行列の積
    Matrix4x4 operator*(const Matrix4x4 &m) const
    {
        Matrix4x4 t;
        for(int i=0;i<16;++i)
        {
            const int j(i & 3), k(i & ~3);
            t[i] =
            matrix[ 0 + j] * m[k + 0]
            +matrix[ 4 + j] * m[k + 1]
            +matrix[ 8 + j] * m[k + 2]
            +matrix[12 + j] * m[k + 3];
        }
        return t;
    }
    //変換行列の配列を返す
    const GLfloat *data() const
    {
        return matrix;
    }
    //単位行列を設定する
    void loadIdentity()
    {
        std::fill(matrix,matrix+16,0.0f);
        matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
    }
    //単位行列を作成する
    static Matrix4x4 identity()
    {
        Matrix4x4 t;
        t.loadIdentity();
        return t;
    }
    //並行移動する変換行列を作成する
    static Matrix4x4 translate(GLfloat x,GLfloat y,GLfloat z)
    {
        Matrix4x4 t;
        t.loadIdentity();
        t[12] = x;
        t[13] = y;
        t[14] = z;
        return t;
    }
    // (x, y, z) 倍に拡大縮小する変換行列を作成する
    static Matrix4x4 scale(GLfloat x, GLfloat y, GLfloat z)
    {
        Matrix4x4 t;
        t.loadIdentity();
        t[ 0] = x;
        t[ 5] = y;
        t[10] = z;
        
        return t;
    }
    // (x, y, z) を軸に a 回転する変換行列を作成する
    static Matrix4x4 rotate(GLfloat a, GLfloat x, GLfloat y, GLfloat z)
    {
        Matrix4x4 t;
        const GLfloat d(sqrt(x*x + y*y + z*z));
        if(d > 0.0f)
        {
            const GLfloat l(x / d), m(y / d), n(z / d);
            const GLfloat l2(l * l), m2(m * m), n2(n * n); const GLfloat lm(l * m), mn(m * n), nl(n * l);
            const GLfloat c(cos(a)), c1(1.0f - c), s(sin(a));
            
            t.loadIdentity();
            t[ 0] = (1.0f - l2) * c + l2;
            t[ 1] = lm * c1 + n * s; t[ 2] = nl * c1 - m * s; t[ 4] = lm * c1 - n * s;
            t[ 5] = (1.0f - m2) * c + m2;
            t[ 6] = mn * c1 + l * s; t[ 8] = nl * c1 + m * s; t[ 9] = mn * c1 - l * s;
            t[10] = (1.0f - n2) * c + n2;
        }
        return t;
    }
    //ビュー変換行列を作成する
    static Matrix4x4 lookat(
                         GLfloat ex,GLfloat ey,GLfloat ez,//eye(x,y,z)
                         GLfloat gx,GLfloat gy,GLfloat gz,//tgt(x,y,z)
                         GLfloat ux,GLfloat uy,GLfloat uz)//upper vector(x,y,z)
    {
        // 視点を原点に平行移動する変換行列
        const Matrix4x4 tv(translate(-ex, -ey, -ez));
        // t 軸 = e - g
        const GLfloat tx(ex - gx);
        const GLfloat ty(ey - gy);
        const GLfloat tz(ez - gz);
        
        // r 軸 = u x t 軸
        const GLfloat rx(uy * tz - uz * ty);
        const GLfloat ry(uz * tx - ux * tz);
        const GLfloat rz(ux * ty - uy * tx);
        
        //s軸 = t 軸 x r 軸
        const GLfloat sx(ty * rz - tz * ry);
        const GLfloat sy(tz * rx - tx * rz);
        const GLfloat sz(tx * ry - ty * rx);
        
        // s 軸の長さのチェック
        const GLfloat s2(sx * sx + sy * sy + sz * sz);
        if (s2 == 0.0f) return tv;
        
        // 回転の変換行列
        Matrix4x4 rv;
        rv.loadIdentity();
        
        // r 軸を正規化して配列変数に格納
        const GLfloat r(sqrt(rx * rx + ry * ry + rz * rz));
        rv[ 0] = rx / r;
        rv[ 4] = ry / r;
        rv[ 8] = rz / r;
        
        // s 軸を正規化して配列変数に格納
        const GLfloat s(sqrt(s2));
        rv[ 1] = sx / s;
        rv[ 5] = sy / s;
        rv[ 9] = sz / s;
        
        // t 軸を正規化して配列変数に格納
        const GLfloat t(sqrt(tx * tx + ty * ty + tz * tz));
        rv[ 2] = tx / t;
        rv[ 6] = ty / t;
        rv[10] = tz / t;
        
        // 視点の平行移動の変換行列に視線の回転の変換行列を乗じる
        return rv * tv;
    }
    
    //直行投影変換行列を作成する
    static Matrix4x4 orthogonal(GLfloat left, GLfloat right,
                             GLfloat bottom, GLfloat top,
                             GLfloat zNear, GLfloat zFar)
    {
        Matrix4x4 t;
        const GLfloat dx(right - left);
        const GLfloat dy(top - bottom);
        const GLfloat dz(zFar - zNear);
        
        if(dx != 0.0f && dy != 0.0f && dz != 0.0f)
        {
            t.loadIdentity();
            t[ 0] = 2.0f / dx;
            t[ 5] = 2.0f / dy;
            t[10] = -2.0f / dz;
            t[12] = -(right + left) / dx;
            t[13] = -(top + bottom) / dy;
            t[14] = -(zFar + zNear) / dz;
        }
        return t;
    }
    //透視投影変換行列
    static Matrix4x4 frustum(GLfloat left, GLfloat right,
                          GLfloat bottom, GLfloat top,
                          GLfloat zNear, GLfloat zFar)
    {
        Matrix4x4 t;
        const GLfloat dx(right - left);
        const GLfloat dy(top - bottom);
        const GLfloat dz(zFar - zNear);
        
        if(dx != 0.0f && dy != 0.0f && dz != 0.0f)
        {
            t.loadIdentity();
            t[ 0] = 2.0f * zNear / dx; t[ 5] = 2.0f * zNear / dy;
            t[ 8] = (right + left) / dx; t[ 9] = (top + bottom) / dy;
            t[10] = -(zFar + zNear) / dz;
            t[11] = -1.0f;
            t[14] = -2.0f * zFar * zNear / dz;
            t[15] = 0.0f;
        }
        return t;
    }
    // 画角を指定して透視投影変換行列を作成する
    static Matrix4x4 perspective(GLfloat fovy, GLfloat aspect,
    GLfloat zNear, GLfloat zFar)
    {
        Matrix4x4 t;
        const GLfloat dz(zFar - zNear);
        
        if(dz != 0.0f)
        {
            t.loadIdentity();
            t[ 5] = 1.0f / tan(fovy * 0.5f);
            t[ 0] = t[5] / aspect;
            t[10] = -(zFar + zNear) / dz;
            t[11] = -1.0f;
            t[14] = -2.0f * zFar * zNear / dz;
            t[15] = 0.0f;
        }
        return t;
    }
    static Matrix4x4 textureRotate(GLfloat a, GLfloat x, GLfloat y, GLfloat z)
    {
        Matrix4x4 trans;
        Matrix4x4 rot;
        trans.translate(-0.5f, -0.5f, -0.5f);
        rot.rotate(a, x, y, z);
        return trans * rot;
    }
};
//std::ostream& operator<<(std::ostream& out,Matrix4x4 &t)
//{
//    for(int i=0;i<4;++i)
//    {
//        for(int j=0;j<4;++j)
//        {
//            out << t[4*i + j] << ",";
//        }
//        out << std::endl;
//    }
//    return out;
//}
#endif /* Matrix4x4_h */
