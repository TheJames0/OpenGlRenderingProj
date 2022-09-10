
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>


/**
 * @brief Data aggregation of current and next frame
 *
 * @tparam T the type of data stored in aggregation
 */
template <typename T> struct TexPair
{
public:
    T& cur;
    T& nxt;
    TexPair(T& a, T& b) : cur(a), nxt(b)
    {
    }
    void Swap()
    {
        std::swap(cur, nxt);
    }
};

template <typename T> inline T min(T a, T b)
{
    return a < b ? a : b;
}
template <typename T> inline T max(T a, T b)
{
    return a > b ? a : b;
}
/**
 * @brief Get index of 1d array out of 2d index (i,j)
 *
 * @param i index of x-axis
 * @param j index of y-axis
 * @param N width of 2d array
 */
//inline int IXY(int x, int y,int z, int N)
//{
//    return (N * N * z)+ (y * N )+ x;
//}

template <typename T, typename SCALAR = typename Eigen::Vector3f::Scalar> inline T lerp(T l, T r, SCALAR t)
{
    return l + t * (r - l);
}

/**
 * @brief Get qf[u, v]
 */
template <typename T, typename SCALAR = typename Eigen::Vector3f::Scalar>
T sample(const int N, const std::vector<T>& qf, const SCALAR i, const SCALAR j, const SCALAR k)
{
    int x = static_cast<int>(i);
    x = max<int>(0, min<int>(N - 1, x));
    int y = static_cast<int>(j);
    y = max<int>(0, min<int>(N - 1, y));
    int z = static_cast<int>(k);
    z = max<int>(0, min<int>(N - 1, z));
    return qf[IXY(x, y,z, N)];
}

/**
 * @brief Bilinear interpolation
 *
 * @param N width
 * @param M height
 * @param p position to interpolate at
 */
template <typename T, typename VEC, typename SCALAR = typename Eigen::Vector3f::Scalar>
T bilerp(const int N, const std::vector<T>& qf, const VEC& p)
{
    Eigen::Vector3f::Scalar s = p(0) - 0.5f;
    Eigen::Vector3f::Scalar t = p(1) - 0.5f;
    Eigen::Vector3f::Scalar k = p(2) - 0.5f;
    Eigen::Vector3f::Scalar iu = floor(s);
    Eigen::Vector3f::Scalar iv = floor(t);
    Eigen::Vector3f::Scalar ik = floor(k);
    Eigen::Vector3f::Scalar fu = s - iu;
    Eigen::Vector3f::Scalar fv = t - iv;
    Eigen::Vector3f::Scalar fk = k - ik;
   T a = sample<T, Eigen::Vector3f::Scalar>(N, qf, iu, iv, ik);
   T b = sample<T, Eigen::Vector3f::Scalar>(N, qf, iu + 1, iv, ik);
   T c = sample<T, Eigen::Vector3f::Scalar>(N, qf, iu, iv + 1, ik);
   T d = sample<T, Eigen::Vector3f::Scalar>(N, qf, iu + 1, iv + 1, ik);
   T e = sample<T, Eigen::Vector3f::Scalar>(N, qf, iu, iv, ik + 1);
   T f = sample<T, Eigen::Vector3f::Scalar>(N, qf, iu + 1, iv, ik + 1);
  T g = sample<T, Eigen::Vector3f::Scalar>(N, qf, iu, iv + 1, ik + 1);
   T h = sample<T, Eigen::Vector3f::Scalar>(N, qf, iu + 1, iv + 1, ik + 1);

    T lerpa = lerp<T, Eigen::Vector3f::Scalar>(a, b, fu);
    T lerpb = lerp<T, Eigen::Vector3f::Scalar>(c, d, fu);

    T lerpc = lerp<T, Eigen::Vector3f::Scalar>(e, f, fu);
   T lerpd = lerp<T, Eigen::Vector3f::Scalar>(g, h, fu);

    T lerpe = lerp<T, Eigen::Vector3f::Scalar>(lerpa, lerpb, fv);
   T lerpf = lerp<T, Eigen::Vector3f::Scalar>(lerpc, lerpd, fv);

    T lerpg = lerp<T, Eigen::Vector3f::Scalar>(lerpe, lerpf, fk);


   //       return lerpg;
}

template <typename VEC, typename SCALAR = typename VEC::Scalar>
SCALAR bilerp_velocity(const int N, const std::vector<VEC>& qf, const VEC& p, int index)
{
    SCALAR s, t , k;
    if (index == 0) // Sample horizontal velocity
    {
        s = p(0);
        t = p(1) - 0.5f;
        k = p(2) - 0.5f;
    }
    else if (index == 1) // Sample vertival velocity
    {
        s = p(0) - 0.5f;
        t = p(1);
        k = p(2) - 0.5f;
    }
    else if (index == 2)
    {
        s = p(0) - 0.5f;
        t = p(1) - 0.5f;
        k = p(2);
    }
    else
    {
        printf("Error! No accessible index\n");
        assert(true);
        return 0.0f;
    }
    SCALAR iu = floor(s);
    SCALAR iv = floor(t);
    SCALAR ik = floor(k);
    SCALAR fu = s - iu;
    SCALAR fv = t - iv;
    SCALAR fk = k - ik;
    VEC a = sample<VEC, SCALAR>(N, qf, iu, iv, ik);
    VEC b = sample<VEC, SCALAR>(N, qf, iu + 1, iv, ik);
    VEC c = sample<VEC, SCALAR>(N, qf, iu, iv + 1 , ik);
    VEC d = sample<VEC, SCALAR>(N, qf, iu + 1, iv + 1, ik);
    VEC e = sample<VEC, SCALAR>(N, qf, iu, iv, ik + 1);
    VEC f = sample<VEC, SCALAR>(N, qf, iu + 1, iv, ik + 1);
    VEC g = sample<VEC, SCALAR>(N, qf, iu, iv + 1, ik + 1);
    VEC h = sample<VEC, SCALAR>(N, qf, iu + 1, iv + 1, ik + 1);
    VEC lerpa = lerp<VEC, SCALAR>(a, b, fu);
    VEC lerpb = lerp<VEC, SCALAR>(c, d, fu);

    VEC lerpc = lerp<VEC, SCALAR>(e, f, fu);
    VEC lerpd = lerp<VEC, SCALAR>(g, h, fu);

    VEC lerpe = lerp<VEC, SCALAR>(lerpa, lerpb, fv);
    VEC lerpf = lerp<VEC, SCALAR>(lerpc, lerpd, fv);

    VEC lerpg = lerp<VEC, SCALAR>(lerpe, lerpf, fk);


    return lerpg(index);
}

/**
 * @brief Locate which point will move to position p in next dt time
 *
 * @param N width
 * @param M height
 * @param vel velocity field
 */
template <typename VEC, typename SCALAR = typename VEC::Scalar>
VEC backtrace(const int N, VEC p, SCALAR dt, const std::vector<VEC>& vel)
{
    VEC v1(bilerp<VEC, VEC>(N, vel, p));
    VEC p1(p(0) - 0.5 * dt * v1(0), p(1) - 0.5 * dt * v1(1), p(2) - 0.5 * dt * v1(2));
    VEC v2(bilerp<VEC, VEC>(N, vel, p1));
    VEC p2(p(0) - 0.75 * dt * v2(0), p(1) - 0.75 * dt * v2(1), p(2) - 0.75 * dt * v2(2));
    VEC v3(bilerp<VEC, VEC>(N, vel, p2));
    p = p + (-1.f) * dt * ((2.f / 9.f) * v1 + (1.f / 3.f) * v2 + (4.f / 9.f) * v3);
    return p * 0.9999f;
}

template <typename VEC, typename SCALAR = typename VEC::Scalar>
VEC backtrace_velocity(const int N, VEC p, SCALAR dt, const std::vector<VEC>& vel)
{
    VEC v1(bilerp_velocity<VEC>(N, vel, p, 0), bilerp_velocity<VEC>(N, vel, p, 1));
    VEC p1(p(0) - 0.5 * dt * v1(0), p(1) - 0.5 * dt * v1(1), p(2) - 0.5 * dt * v1(2));
    VEC v2(bilerp_velocity<VEC>(N, vel, p1, 0), bilerp_velocity<VEC>(N, vel, p1, 1));
    VEC p2(p(0) - 0.75 * dt * v2(0), p(1) - 0.75 * dt * v2(1), p(2) - 0.75 * dt * v2(2));
    VEC v3(bilerp_velocity<VEC>(N, vel, p2, 0), bilerp_velocity<VEC>(N, vel, p2, 1));
    p = p + (-1.f) * dt * ((2.f / 9.f) * v1 + (1.f / 3.f) * v2 + (4.f / 9.f) * v3);
    return p * 0.9999f;
}

/**
 * @brief update advection of fluid property qf
 * @param N width
 * @param M height
 * @param vel velocity
 * @param dt time interval
 */
template <typename T, typename VEC, typename SCALAR = typename VEC::SCALAR >
void advection(const int N, const std::vector<VEC>& vel, const std::vector<T>& qf, std::vector<T>& new_qf,
    SCALAR dt, SCALAR damping = 0.9999f)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < N; k++)
            {
                VEC p(i + .5f, j + .5f, k + .5f);
                p = backtrace<VEC>(N, p, dt, vel) * damping;

                new_qf[IXY(i, j, k, N)] = bilerp<T, VEC,SCALAR>(N, qf, p);
            }
        }
    }
}


void advection_velocity(const int N, const std::vector<VEC>& vel, std::vector<VEC>& new_qf, SCALAR dt,
    SCALAR damping = 0.9999f)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < N; k++)
            {
                //Determine boundary condition and advect in 3 dimensions
                if (i == 0 || i == N - 1)
                    new_qf[IXY(i, j, k, N)](0) = 0.0f;
                else
                {
                    VEC p(i, j + 0.5f,k + 0.5f);
                    p = backtrace_velocity<VEC>(N, p, dt, vel) * damping;
                    new_qf[IXY(i, j, k, N)](0) = bilerp_velocity<VEC>(N, vel, p, 0) * damping;
                }
                if (j == 0 || j == N - 1)
                    new_qf[IXY(i, j, k, N)](1) = 0.0f;
                else
                {
                    VEC p(i + 0.5f, j,k + 0.5f);
                    p = backtrace_velocity<VEC>(N, p, dt, vel) * damping;
                    new_qf[IXY(i, j, k, N)](1) = bilerp_velocity<VEC>(N, vel, p, 1) * damping;
                }
                if (k == 0 || k == N - 1)
                    new_qf[IXY(i, j, k, N)](2) = 0.0f;
                else
                {
                    VEC p(i + 0.5f, j + 0.5f, k );
                    p = backtrace_velocity<VEC>(N, p, dt, vel) * damping;
                    new_qf[IXY(i, j, k, N)](2) = bilerp_velocity<VEC>(N, vel, p, 2) * damping;
                }
            }
        }
    }
}

/**
 * @brief apply global forces and damping
 * @param N width
 * @param M height
 * @param vel velocity field
 * @param g scale of gravity(-9.8f as default)
 * @param dt time interval
 */
template <typename VEC, typename SCALAR = typename VEC::Scalar>
void apply_force(const int N, std::vector<VEC>& vel, const SCALAR g = -9.8f, const SCALAR dt = 0.03f)
{
    for (int i = 0; i < N * N * N; i++)
    {
        vel.at(i) += VEC(0, g * 1.0 * dt);
    }
}

/**
 * @brief Get the divergence of velocity field
 * @param N width
 * @param M height
 * @param vel velocity field
 * @param divergence divergence of velocity field
 */
template <typename VEC, typename SCALAR = typename VEC::Scalar>
void get_divergence(const int N, const std::vector<VEC>& vel, std::vector<SCALAR>& divergence)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < N; k++)
            {
                    SCALAR vl = sample<VEC, SCALAR>(N, vel, i, j)(0);
                    SCALAR vr = sample<VEC, SCALAR>(N, vel, i + 1, j)(0);
                    SCALAR vb = sample<VEC, SCALAR>(N, vel, i, j)(1);
                    SCALAR vt = sample<VEC, SCALAR>(N, vel, i, j + 1)(1);
                    SCALAR ve = sample<VEC, SCALAR>(N, vel, i, j)(2);
                    SCALAR vf = sample<VEC, SCALAR>(N, vel, i, j + 1)(2);

                    divergence[IXY(i, j, k, N)] = vr - vl + vt - vb;
                }
            }
}

template <typename VEC, typename SCALAR = typename VEC::Scalar>
void get_curl(const int N, const std::vector<VEC>& vel, std::vector<SCALAR>& curl)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < N; k++)
            {
                SCALAR vl = bilerp_velocity<VEC>(N, vel, VEC(i, j + 0.5f), 1);
                SCALAR vr = bilerp_velocity<VEC>(N, vel, VEC(i + 1.0f, j + 0.5f), 1);
                SCALAR vt = bilerp_velocity<VEC>(N, vel, VEC(i + 0.5, j + 1.0f), 0);
                SCALAR vb = bilerp_velocity<VEC>(N, vel, VEC(i + 0.5f, j), 0);
                curl[IXY(i, j, k, N)] = vr - vt - vl + vb;
            }
        }   
}

template <typename VEC, typename SCALAR = typename VEC::Scalar>
void vorticity_confinement(const int N, std::vector<VEC>& vel, const std::vector<SCALAR>& curl,
    std::vector<VEC>& curl_force, const SCALAR curl_strength, const SCALAR dt)
{
    static const SCALAR scale = 1e-3f;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
             for (int k = 0; k < N; k++)
             {
                SCALAR cl = sample<SCALAR, SCALAR>(N, curl, i - 1, j, k);
                SCALAR cr = sample<SCALAR, SCALAR>(N, curl, i + 1, j, k);
                SCALAR cb = sample<SCALAR, SCALAR>(N, curl, i, j - 1, k);
                SCALAR ct = sample<SCALAR, SCALAR>(N, curl, i, j + 1, k);
                SCALAR cc = sample<SCALAR, SCALAR>(N, curl, i, j, k);
                SCALAR cf = sample<SCALAR, SCALAR>(N, curl, i, j, k - 1);
                SCALAR cd = sample<SCALAR, SCALAR>(N, curl, i, j, k + 1);
                curl_force[IXY(i, j, k, N)] = VEC(abs(ct) - abs(cb), abs(cl) - abs(cr), abs(cf) - abs(cd));
                curl_force[IXY(i, j, k, N)].normalize();
                curl_force[IXY(i, j, k, N)] *= curl_strength * cc * dt;
             }
        
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < N; k++)
            {
                VEC cl = sample<VEC, SCALAR>(N, curl_force, i - 1, j,k);
                VEC cb = sample<VEC, SCALAR>(N, curl_force, i, j - 1,k);
                VEC ce = sample<VEC, SCALAR>(N, curl_force, i, j, k - 1);
                VEC cc = sample<VEC, SCALAR>(N, curl_force, i, j,k);
                SCALAR fl = (cl - cc)(0);
                SCALAR fb = (cb - cc)(1);
                SCALAR fc = (ce - cc)(2);
                vel[IXY(i, j, k, N)](0) = min<SCALAR>(1e4f, max<SCALAR>(-1e4f, vel[IXY(i, j, k, N)](0) + fl));
                vel[IXY(i, j, k, N)](1) = min<SCALAR>(1e4f, max<SCALAR>(-1e4f, vel[IXY(i, j, k, N)](1) + fb));
                vel[IXY(i, j, k, N)](2) = min<SCALAR>(1e4f, max<SCALAR>(-1e4f, vel[IXY(i, j, k, N)](2) + fc));
            }
        }
}

/**
 * @brief Single iteration of gauss-sidel method
 * @param N width
 * @param M height
 * @param pressure pressure field
 * @param divergence divergence of velocity field
 */
template <typename VEC, typename SCALAR = typename VEC::Scalar>
void pressure_gauss_sidel(const int N, const std::vector<SCALAR>& divergence,
    const std::vector<SCALAR>& pressure, std::vector<SCALAR>& new_pressure)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < N; k++)
            {
                SCALAR pl = sample<SCALAR, SCALAR>(N, pressure, i - 1, j,k);
                SCALAR pr = sample<SCALAR, SCALAR>(N, pressure, i + 1, j,k);
                SCALAR pb = sample<SCALAR, SCALAR>(N, pressure, i, j - 1,k);
                SCALAR pt = sample<SCALAR, SCALAR>(N, pressure, i, j + 1,k);
                SCALAR pe = sample<SCALAR, SCALAR>(N, pressure, i, j,k - 1);
                SCALAR pf = sample<SCALAR, SCALAR>(N, pressure, i, j,k + 1);
                SCALAR diver = divergence[IXY(i, j, k, N)];
                new_pressure.at(IXY(i, j, k, N)) = (pl + pr + pb + pt +pe + pf + (-1.f) * diver) /6;
            }
        }
}

/**
 * @brief Apply pressure to velocity field
 * @param N width
 * @param M height
 * @param vel velocity field
 * @param pressure pressure field
 */
template <typename VEC, typename SCALAR = typename VEC::Scalar>
void subtract_gradient(const int N, std::vector<VEC>& vel, const std::vector<SCALAR>& pressure)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < N; k++)
            {
                SCALAR pl = sample<SCALAR, SCALAR>(N, pressure, i - 1, j, k);
                SCALAR pb = sample<SCALAR, SCALAR>(N, pressure, i, j - 1, k);
                SCALAR pe = sample<SCALAR, SCALAR>(N, pressure, i, j, k - 1);
                SCALAR pc = sample<SCALAR, SCALAR>(N, pressure, i, j, k);
                vel[IXY(i, j, k, N)](0) -= pc - pl;
                vel[IXY(i, j, k, N)](1) -= pc - pb;
                vel[IXY(i, j, k, N)](2) -= pc - pe;
            }

        }
}

template <typename SCALAR> SCALAR smooth_step(SCALAR a, SCALAR x)
{
    SCALAR y = (a - x) / a;
    if (y < 0.0)
        y = 0.0;
    if (y > 1.0)
        y = 1.0;
    SCALAR rst = y * y;
    return rst;
}

/**
 * @brief add round-shape fluid momentum at point [x,y]
 *
 * @param N width
 * @param x x-position
 * @param y y-position
 * @param r radius
 * @param dir direction of added velocity
 * @param dye color
 * @param vel velocity field
 * @param value scale of added color and velocity
 */
template <typename VEC, typename SCALAR = typename VEC::Scalar>
void add_source(const int N, int x, int y, int z,int r, SCALAR value, VEC dir, std::vector<SCALAR>& dye,
    std::vector<VEC>& vel)
{
    for (int i = -r; i <= r; i++)
        for (int j = -r; j <= r; j++)
        {
            for (int k = -r; k <= r; k++)
            {
                int index = IXY(x + i, y + j, z + k, N);
                SCALAR smooth = smooth_step<SCALAR>(r * r, i * i + j * j);
                smooth *= value;
                if (index < 0 || index >= dye.size())
                    printf("Error info: index out of range {%d, %d, %d, %d, %d}\n", x, y, i, j, r);
                dye[index] = min(smooth + dye[index], 3.0f);
                vel[index] += dir * smooth * 100.0f;
            }
        }
}

/**
 * @brief Fill scalar field sf into color buffer
 * @param N width
 * @param M height
 * @param sf scalar field to show
 */




#endif