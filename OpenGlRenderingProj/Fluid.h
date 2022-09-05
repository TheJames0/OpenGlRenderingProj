
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>


/* 
Swap arrays prev and current*/
float min(float a, float b)
{
    return a < b ? a : b;
}
float max(float a, float b)
{
    return a > b ? a : b;
}
/**
 * @brief Get index of 1d array out of 2d index (i,j)
 *
 * @param i index of x-axis
 * @param j index of y-axis
 * @param k index of z-axis
 * @param N width/height/depth of 3d array
 */
int IXY(int i, int j,int k, int N)
{
    return (N*N*k) + (N * j) + i;
}
 float lerp(float l, float r, float t)
{
    return l + t * (r - l);
}
/**
 * @brief Get qf[u, v, k]
 */
float sample(const int N, const std::vector<T>& qf, const SCALAR u, const SCALAR v,const SCALAR k)
{
    int x = static_cast<int>(u);
    x = max<int>(0, min<int>(N - 1, x));
    int y = static_cast<int>(v);
    y = max<int>(0, min<int>(N - 1, y));
    int z = static_cast<int>(k);
    z = max<int>(0, min<int>(N - 1, z));
    return qf[IXY(x, y, z, N)];
}
/**
*@brief Bilinear interpolation
*
* @param N width
* @param M height
* @param p position to interpolate at
**/
template <typename T, typename VEC, typename SCALAR = typename VEC::Scalar>
T bilerp(const int N, const std::vector<T> &qf, const VEC & p)
{
    SCALAR s = p(0) - 0.5f;
    SCALAR t = p(1) - 0.5f;
    SCALAR z = p(2) - 0.5f;
    SCALAR iu = floor(s);
    SCALAR iv = floor(t);
    SCALAR ik = floor(z);
    SCALAR fu = s - iu;
    SCALAR fv = t - iv;
    SCALAR fz = z - ik;
    T a = sample<T, SCALAR>(N, qf, iu, iv,ik);
    T b = sample<T, SCALAR>(N, qf, iu + 1, iv,ik);
    T c = sample<T, SCALAR>(N, qf, iu, iv + 1,ik);
    T d = sample<T, SCALAR>(N, qf, iu + 1, iv + 1,ik);
    T e = sample<T, SCALAR>(N, qf, iu, iv, ik + 1);
    T f = sample<T, SCALAR>(N, qf, iu + 1, iv, ik + 1);
    T g = sample<T, SCALAR>(N, qf, iu, iv + 1, ik + 1);
    T h = sample<T, SCALAR>(N, qf, iu + 1, iv + 1, ik + 1);

    T sampa = lerp<T, SCALAR>(lerp<T, SCALAR>(a, b, fu), lerp<T, SCALAR>(c, d, fu), fv);
    T sampb = lerp<T, SCALAR>(lerp<T, SCALAR>(e, f, fu), lerp<T, SCALAR>(g, h, fu), fv);
    return lerp<T, SCALAR>(sampa, sampb, fu);
}
void swap(float &current, float &replace)
{
   
    float temp[sizeof(current)] = { 0 };
    *temp = current;
    current = replace;
    replace = *temp;
    delete(temp);

}
void advection()
{

}
void advection_velocity()
{

}
void fluidstep()
{
    //Advect velocity and dye (propogate values to other cells)
    advection_velocity();
    advection();
}
void initFluid()
{
    //Define simulation dimensions
    const int sizeX = 40, sizeY = 40, sizeZ = 40;
    //Define Time delta, grid spacing, fluid viscocity 
    //and gravity force .
    float dt = 0.001f, dx = 1.0f, damping = 0.999999f, g = -9.8f;
    const float vorticity_strength = 10.0f;
    //Gauss sidel iterative solver iterations.
    const int p_solver_iters = 120;

    const int N = sizeX * sizeY * sizeZ;
    //Define all arrays
    //Two arrays for each vector component
    std::vector<float> vel_x0(N, 0.0f);
    std::vector<float> vel_y0(N, 0.0f);
    std::vector<float> vel_x(N, 0.0f);
    std::vector<float> vel_y(N, 0.0f);
    std::vector<float> vorticity_force_x(N, 0.0f);
    std::vector<float> vorticity_force_y(N, 0.0f);
    //Scalar Array
    std::vector<float> divergence(N, 0.0f);
    std::vector<float> curl(N, 0.0f);
    std::vector<float> pressure0(N, 0.0f);
    std::vector<float> pressure(N, 0.0f);
    std::vector<float> dye(N, 0.0f);
    std::vector<float> dye0(N, 0.0f);

}
