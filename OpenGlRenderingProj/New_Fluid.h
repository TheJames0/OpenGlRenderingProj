#ifndef NEW_FLUID
#define NEW_FLUID
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

const int N = 35;

//Used to represent all vector elements and store in array.
struct VectorField 
{
    std::vector<float> x;
    std::vector<float> y;
    std::vector<float> z;
    VectorField()
    {
        x.reserve(N * N * N);
        y.reserve(N * N * N);
        z.reserve(N * N * N);
    }
    void swap(VectorField &newval)
    {
        std::swap(x, newval.x);
        std::swap(y, newval.y);
        std::swap(z, newval.z);

        
  
    }
    void normalize(int index)
    {
        
        float mag = std::sqrt(x[index] * x[index] + y[index] * y[index]);
        this->x[index] = x[index] / mag;
        this->y[index] = y[index] / mag;
    }

};
//Only Scalar value kept in struct for expandability and easy parameter pass.
struct ScalarField 
{
    std::vector<float> v;
    ScalarField()
    {
        v.reserve(N * N * N);
    }
    void swap(ScalarField &newval)
    {
        std::swap(v, newval.v);

    }
};
//Represents an individual vector and handles operations
struct SingleVector 
{
    float x = 0;
    float y = 0;
    float z = 0;
    SingleVector(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    SingleVector FullScalarMultiply(float val)
    {
        return SingleVector(x * val, y * val, z * val);
    }
    SingleVector FullScalarAddition(float val)
    {
        return SingleVector(x + val, y + val, z + val);
    }
    SingleVector VectorMultiply(SingleVector vector)
    {
        return SingleVector(x * vector.x, y * vector.y, z * vector.z);
    }
    
    SingleVector VectorAddition(SingleVector vector) 
    {
        return SingleVector(x + vector.x, y + vector.y, z + vector.z);
    }
    
    SingleVector VectorSubtraction(SingleVector vector) {
        return SingleVector(x - vector.x, y - vector.y, z - vector.z);
    }
    void normalize()
    {

        float mag = magnitude();
        this->x = x / mag;
        this->y = y / mag;
    }
    float magnitude()
    {
        return std::sqrt((x * x) + (y * y));
    }
};
//Index convert 3d to 1d array
int IXY(int x, int y, int z, int N)
{
    return (N * N * z) + (y * N) + x;
}
//Retrieves minimum value
int min(float a, float b)
{
    return a < b ? a : b;
}
//Retrieves maximum value
int max(float a, float b)
{
    return a > b ? a : b;
}
//Retrieves samples VectorComponent from Vector field
SingleVector sample(const int N, VectorField& qf, float i, float j, float k)
{
    int x = i;
    x = max(0, min(N - 1, x));
    int y = j;
    y = max(0, min(N - 1, y));
    int z = k;
    z = max(0, min(N - 1, z));
    float nx = qf.x[IXY(x, y, z, N)];
    float ny = qf.y[IXY(x, y, z, N)];
    float nz = qf.z[IXY(x, y, z, N)];
    
    SingleVector vec(nx, ny, nz);
    return vec;
}
////linear interpolation of vector components
SingleVector lerp(SingleVector l, SingleVector r, float t)
{
    //return l + t * (r - l);
    //Below is this code adapted to vector.

    
    SingleVector b = r.VectorSubtraction(l);
    SingleVector c = b.FullScalarMultiply(t);
    SingleVector a = l.VectorAddition(c);
    return a;


}
//trilinear interpolation of velocity vectors and returns single index.
float trilerp_velocity(const int N, VectorField& qf,SingleVector p, int index)
{

    float s, t, k;
    if (index == 0) // Sample horizontal velocity
    {
        s = p.x;
        t = p.y - 0.5f;
        k = p.z - 0.5f;
    }
    else if (index == 1) // Sample vertival velocity
    {
        s = p.x - 0.5f;
        t = p.y;
        k = p.z - 0.5f;
    }
    else if (index == 2)
    {
        s = p.x - 0.5f;
        t = p.y - 0.5f;
        k = p.z;
    }
    else
    {
        printf("Error! No accessible index\n");
        assert(true);
        return 0;
    }
    float iu = floor(s);
    float iv = floor(t);
    float ik = floor(k);
    float fu = s - iu;
    float fv = t - iv;
    float fk = k - ik;
    SingleVector a = sample(N, qf, iu, iv, ik);
    SingleVector b = sample(N, qf, iu + 1, iv, ik);
    SingleVector c = sample(N, qf, iu, iv + 1, ik);
    SingleVector d = sample(N, qf, iu + 1, iv + 1, ik);
    SingleVector e = sample(N, qf, iu, iv, ik + 1);
    SingleVector f = sample(N, qf, iu + 1, iv, ik + 1);
    SingleVector g = sample(N, qf, iu, iv + 1, ik + 1);
    SingleVector h = sample(N, qf, iu + 1, iv + 1, ik + 1);
    SingleVector lerpa = lerp(a, b, fu);
    SingleVector lerpb = lerp(c, d, fu);

    SingleVector lerpc = lerp(e, f, fu);
    SingleVector lerpd = lerp(g, h, fu);

    SingleVector lerpe = lerp(lerpa, lerpb, fv);
    SingleVector lerpf = lerp(lerpc, lerpd, fv);

    SingleVector lerpg = lerp(lerpe, lerpf, fk);

    if (index == 0)
    {
        return lerpg.x;
    }
    else if (index == 1)
    {
        return lerpg.y;
    }
    else if (index == 2)
    {
        return lerpg.z;
    }
    
}
//Backtrace where vector field of effected cell originate
SingleVector backtrace_velocity(const int N, SingleVector p, float dt, VectorField& vel)
{
    SingleVector v1(trilerp_velocity(N, vel, p, 0),trilerp_velocity(N, vel, p, 1),trilerp_velocity(N, vel, p, 2));
    SingleVector p1(p.x - 0.5 * dt * v1.x, p.y - 0.5 * dt * v1.y, p.z - 0.5 * dt * v1.z);
    SingleVector v2(trilerp_velocity(N, vel, p1, 0), trilerp_velocity(N, vel, p1, 1), trilerp_velocity(N, vel, p1, 2));
    SingleVector p2(p.x - 0.75 * dt * v2.x, p.y - 0.75 * dt * v2.y, p.z - 0.75 * dt * v2.z);
    SingleVector v3(trilerp_velocity(N, vel, p2, 0), trilerp_velocity(N, vel, p2, 1), trilerp_velocity(N, vel, p2, 2));
     SingleVector a1 = v1.FullScalarMultiply(2.f/9.f);
    a1 = a1.FullScalarMultiply(dt * -1.f);
    SingleVector a2 = v2.FullScalarMultiply(1.f / 3.f);
    SingleVector a3 = v3.FullScalarMultiply(4.f / 9.f);
    p = p.VectorAddition(a1);
    p = p.VectorAddition(a2);
    p = p.VectorAddition(a3);
    p = p.FullScalarMultiply(0.9999f);
    
    return p;
}

//linear interpolation with float output
float lerpSCALAR(float l, float r, float t)
{
    return l + t * (r - l);
}
//Sample a scalar value and return float
float sampleSCALAR(const int N, ScalarField& qf, float i, float j, float k)
{
    int x = (i);
    x = max(0, min(N - 1, x));
    int y = (j);
    y = max(0, min(N - 1, y));
    int z = (k);
    z = max(0, min(N - 1, z));
   
   
    return qf.v[IXY(x,y,z,N)];
}
//Bilinear interpolation of a vector component
SingleVector trilerp(const int N, VectorField& qf, SingleVector& p)
{
    float s = p.x - 0.5f;
    float t = p.y - 0.5f;
    float k = p.z - 0.5f;
    float iu = floor(s);
    float iv = floor(t);
    float ik = floor(k);
    float fu = s - iu;
    float fv = t - iv;
    float fk = k - ik;
    SingleVector a = sample(N, qf, iu, iv, ik);
    SingleVector b = sample(N, qf, iu + 1, iv, ik);
    SingleVector c = sample(N, qf, iu, iv + 1, ik);
    SingleVector d = sample(N, qf, iu + 1, iv + 1, ik);
    SingleVector e = sample(N, qf, iu, iv, ik + 1);
    SingleVector f = sample(N, qf, iu + 1, iv, ik + 1);
    SingleVector g = sample(N, qf, iu, iv + 1, ik + 1);
    SingleVector h = sample(N, qf, iu + 1, iv + 1, ik + 1);

    SingleVector lerpa = lerp(a, b, fu);
    SingleVector lerpb = lerp(c, d, fu);

    SingleVector lerpc = lerp(e, f, fu);
    SingleVector lerpd = lerp(g, h, fu);

    SingleVector lerpe = lerp(lerpa, lerpb, fv);
    SingleVector lerpf = lerp(lerpc, lerpd, fv);

    SingleVector lerpg = lerp(lerpe, lerpf, fk);


           return lerpg;
}
//Trilinear interpolation returning float
float trilerpSCALAR(const int N, ScalarField & qf, SingleVector & p)
{
    float s = p.x - 0.5;
    float t = p.y - 0.5;
    float k = p.z - 0.5;
    float iu = floor(s);
    float iv = floor(t);
    float ik = floor(k);
    float fu = s - iu;
    float fv = t - iv;
    float fk = k - ik;
    float a = sampleSCALAR(N, qf, iu, iv, ik);
    float b = sampleSCALAR(N, qf, iu + 1, iv, ik);
    float c = sampleSCALAR(N, qf, iu, iv + 1, ik);
    float d = sampleSCALAR(N, qf, iu + 1, iv + 1, ik);
    float e = sampleSCALAR(N, qf, iu, iv, ik + 1);
    float f = sampleSCALAR(N, qf, iu + 1, iv, ik + 1);
    float g = sampleSCALAR(N, qf, iu, iv + 1, ik + 1);
    float h = sampleSCALAR(N, qf, iu + 1, iv + 1, ik + 1);

    float lerpa = lerpSCALAR(a, b, fu);
    float lerpb = lerpSCALAR(c, d, fu);

    float lerpc = lerpSCALAR(e, f, fu);
    float lerpd = lerpSCALAR(g, h, fu);

    float lerpe = lerpSCALAR(lerpa, lerpb, fv);
    float lerpf = lerpSCALAR(lerpc, lerpd, fv);

    float lerpg = lerpSCALAR(lerpe, lerpf, fk);


    return lerpg;
}
//backtrace velocity for dye advection
SingleVector backtrace(const int N, SingleVector p, float dt, VectorField& vel)
{
    SingleVector v1(trilerp(N, vel, p));
    SingleVector p1((p.x - 0.5 * dt * v1.x), (p.y - 0.5 * dt * v1.y), (p.z - 0.5 * dt * v1.z));
    SingleVector v2(trilerp(N, vel, p1));
    SingleVector p2(p.x - 0.75 * dt * v2.x, p.y - 0.75 * dt * v2.y, p.z - 0.75 * dt * v2.z);
    SingleVector v3(trilerp(N, vel, p2));
    //The following lines make equal p to...
    // a 1 .. 2 .. 3 is just each seperated part of equation split up
    //p = p + (-1.f) * dt * ((2.f / 9.f) * v1 + (1.f / 3.f) * v2 + (4.f / 9.f) * v3);
    SingleVector a1 = v1.FullScalarMultiply(2.f/9.f);
    a1 = a1.FullScalarMultiply(dt * -1.f);
    SingleVector a2 = v2.FullScalarMultiply(1.f / 3.f);
    SingleVector a3 = v3.FullScalarMultiply(4.f / 9.f);
    p = p.VectorAddition(a1);
    p = p.VectorAddition(a2);
    p = p.VectorAddition(a3);
    p = p.FullScalarMultiply(0.9999f);
    
    return p;
}
//Move fluid density with respect to vector field velocities
void advection(const int N, VectorField& vel, ScalarField& qf, ScalarField& new_qf,
    float dt, float damping = 0.9999f)
{
    for (int k = 0; k < N; k++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int i = 0; i < N; i++)
            {
                SingleVector p(i + .5f, j + .5f, k + .5f);
                p = backtrace(N, p, dt, vel);
                p = p.FullScalarMultiply(damping);
                //Aquire new values from calculated backtrace p and trilerp new value into respective cell
                new_qf.v[IXY(i, j, k, N)] = trilerpSCALAR(N, qf, p);
            }
        }
    }
}
//Move fluid velocity with respect to other vector field velocities
void advection_velocity( int N, VectorField& vel, VectorField& new_qf, float dt,
    float damping = 0.9999f)
{
    for (int k = 0; k < N; k++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int i = 0; i < N; i++)
            {
                //Determine boundary condition and advect in 3 dimensions
                if (i == 0 || i == N - 1)
                    new_qf.x[IXY(i, j, k, N)] = 0.0f;
                else
                {
                    SingleVector p(i, j + 0.5f, k + 0.5f);
                    p = backtrace_velocity(N, p, dt, vel).FullScalarMultiply(damping);
                    new_qf.x[IXY(i, j, k, N)] = trilerp_velocity(N, vel, p, 0) * damping;
                }
                if (j == 0 || j == N - 1)
                    new_qf.y[IXY(i, j, k, N)] = 0.0f;
                else
                {
                    SingleVector p(i + 0.5f, j, k + 0.5f);
                    p = backtrace_velocity(N, p, dt, vel).FullScalarMultiply(damping);
                    new_qf.y[IXY(i, j, k, N)] = trilerp_velocity(N, vel, p, 1) * damping;
                }
                if (k == 0 || k == N - 1)
                    new_qf.z[IXY(i, j, k, N)] = 0.0f;
                else
                {
                    SingleVector p(i + 0.5f, j + 0.5f, k);
                    p = backtrace_velocity(N, p, dt, vel).FullScalarMultiply(damping);
                    new_qf.z[IXY(i, j, k, N)] = trilerp_velocity(N, vel, p, 2) * damping;
                }
            }
        }
    }
   
}
//Get the value of velocity divergence ,net velocity away from cell positive = outflow, negative = inflow, 0 is optimal.
void get_divergence(const int N, VectorField& vel, ScalarField& divergence)
{
    for (int k = 0; k < N; k++)
        for (int j = 0; j < N; j++)
        {
            for (int i = 0; i < N; i++)
            {
                float vl = sample(N, vel, i - 1, j,k).x;
                float vr = sample(N, vel, i + 1, j,k).x;
                float vb = sample(N, vel, i , j - 1,k).y;
                float vt = sample(N, vel, i, j + 1,k).y;
                float vbb = sample(N, vel, i, j,k - 1).z;
                float vf = sample(N, vel, i, j,k + 1).z;

                divergence.v[IXY(i, j, k, N)] = ((vr - vl) + (vt - vb) + (vf - vbb))/4;
            }
        }
}
//Iteratively spread velocity curl gradient across vector field
void vorticity_confinement(const int N, VectorField& vel, ScalarField& curl,
    VectorField& curl_force, float  dt)
{
    static const float scale = 1e-3f;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            for (int k = 0; k < N; k++)
            {
                SingleVector at = sample(N, vel, i -1, j + 1, k + 1);
                SingleVector bt = sample(N, vel, i , j  + 1, k + 1);
                SingleVector ct = sample(N, vel, i + 1, j + 1, k + 1);
                SingleVector dt = sample(N, vel, i - 1, j + 1, k);
                SingleVector et = sample(N, vel, i, j + 1, k);
                SingleVector ft = sample(N, vel, i + 1, j + 1, k);
                SingleVector gt = sample(N, vel, i - 1, j + 1, k - 1);
                SingleVector ht = sample(N, vel, i, j + 1, k - 1);
                SingleVector it = sample(N, vel, i + 1, j + 1, k - 1);
                

                SingleVector am = sample(N, vel, i - 1, j, k + 1);
                SingleVector bm = sample(N, vel, i, j, k + 1);
                SingleVector cm = sample(N, vel, i + 1, j, k + 1);
                SingleVector dm = sample(N, vel, i - 1, j, k);
                SingleVector em = sample(N, vel, i, j, k);
                SingleVector fm = sample(N, vel, i + 1, j, k);
                SingleVector gm = sample(N, vel, i - 1, j, k - 1);
                SingleVector hm = sample(N, vel, i, j, k - 1);
                SingleVector im = sample(N, vel, i + 1, j, k - 1);

                SingleVector ab = sample(N, vel, i - 1, j - 1, k + 1);
                SingleVector bb = sample(N, vel, i, j - 1, k + 1);
                SingleVector cb = sample(N, vel, i + 1, j - 1, k + 1);
                SingleVector db = sample(N, vel, i - 1, j - 1, k);
                SingleVector eb = sample(N, vel, i, j - 1, k);
                SingleVector fb = sample(N, vel, i + 1, j - 1, k);
                SingleVector gb = sample(N, vel, i - 1, j - 1, k - 1);
                SingleVector hb = sample(N, vel, i, j - 1, k - 1);
                SingleVector ib = sample(N, vel, i + 1, j - 1, k - 1);

                float div = ((et.y - eb.y) + (bm.z - hm.z) + (fm.x - dm.x )) / 3;
                float p0 = 0;
                float p1 = 0;
                float p2 = 0;
                float p3 = 0;
                float p4 = 0;
                float p5 = 0;
                float p6 = 0;
                //Gauss Siedel solver
                for (int i = 0; i < 80; i++)
                {
                    p0 = (p1 + p2 + p3 + p4 + p5 + p6) - div / 4;
                    p1 = 4 * p0 + div - (p2 + p3 + p4 + p5 + p6);
                    p2 = 4 * p0 + div - (p1 + p3 + p4 + p5 + p6);
                    p3 = 4 * p0 + div - (p1 + p2 + p4 + p5 + p6);
                    p4 = 4 * p0 + div - (p1 + p2 + p3 + p5 + p6);
                    p5 = 4 * p0 + div - (p1 + p2 + p3 + p4 + p6);
                    p6 = 4 * p0 + div - (p1 + p2 + p3 + p4 + p5);
                }
                curl.v[IXY(i, j, k, N)] = p0;
            }
   
    for (int i = 1; i < N - 1; i++)
        for (int j = 1; j < N - 1; j++)
            for (int k = 1; k < N - 1; k++)
            {
                float left = curl.v[IXY(i - 1, j, k, N)];
                float right = curl.v[IXY(i + 1, j, k, N)];
                float up = curl.v[IXY(i , j + 1, k, N)];
                float down = curl.v[IXY(i, j - 1, k, N)];
                float forward = curl.v[IXY(i , j, k + 1, N)];
                float backward = curl.v[IXY(i, j, k - 1, N)];

                vel.x[IXY(i, j, k, N)] += (right - left) / 3;
                vel.y[IXY(i, j, k, N)] += (up - down) /3;
                vel.z[IXY(i, j, k, N)] += (forward - backward) / 3;
            }
}

void subtract_gradient(const int N, VectorField& vel, ScalarField& pressure)
{
    for (int k = 0; k < N; k++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int i = 0; i < N; i++)
            {
                float pl = sampleSCALAR(N, pressure, i - 1, j, k);
                float pb = sampleSCALAR(N, pressure, i, j - 1, k);
                float pe = sampleSCALAR(N, pressure, i, j, k - 1);
                float pc = sampleSCALAR(N, pressure, i, j, k);
                vel.x[IXY(i, j, k, N)] -= pc - pl;
                vel.y[IXY(i, j, k, N)] -= pc - pb;
                vel.z[IXY(i, j, k, N)] -= pc - pe;
            }

        }
    }
}
void pressure_gauss_sidel(const int N, ScalarField& divergence,
    ScalarField& pressure, ScalarField& new_pressure)
{
    for (int k = 0; k < N; k++)
        for (int j = 0; j < N; j++)
        {
            for (int i = 0; i < N; i++)
            {
                float pl = sampleSCALAR(N, pressure, i - 1, j, k);
                float pr = sampleSCALAR(N, pressure, i + 1, j, k);
                float pb = sampleSCALAR(N, pressure, i, j - 1, k);
                float pt = sampleSCALAR(N, pressure, i, j + 1, k);
                float pe = sampleSCALAR(N, pressure, i, j, k - 1);
                float pf = sampleSCALAR(N, pressure, i, j, k + 1);
                float diver = divergence.v[IXY(i, j, k, N)];
                new_pressure.v[IXY(i, j, k, N)] = (pl + pr + pb + pt + pe + pf + (-1.f) * diver) / 6;
            }
        }
}
#endif