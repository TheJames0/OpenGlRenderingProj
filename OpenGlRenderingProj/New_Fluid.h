#ifndef NEW_FLUID
#define NEW_FLUID
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

const int N = 40;

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
        
        float mag = std::sqrtf(std::pow(x[index], 2)+ std::pow(y[index], 2)+ std::pow(z[index], 2));
        this->x[index] = x[index] / mag;
        this->y[index] = y[index] / mag;
        this->z[index] = z[index] / mag;
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
};

int IXY(int x, int y, int z, int N)
{
    return (N * N * z) + (y *N) + x;
}
int min(float a, float b)
{
    return a < b ? a : b;
}
int max(float a, float b)
{
    return a > b ? a : b;
}
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
SingleVector lerp(SingleVector l, SingleVector r, float t)
{
    //return l + t * (r - l);
    //Below is this code adapted to vector.

    
    SingleVector b = r.VectorSubtraction(l);
    SingleVector c = b.FullScalarMultiply(t);
    SingleVector a = l.VectorAddition(c);
    return c;


}
float bilerp_velocity(const int N, VectorField& qf,SingleVector p, int index)
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
    else
    {
        return lerpg.z;
    }
    
}
SingleVector backtrace_velocity(const int N, SingleVector p, float dt, VectorField& vel)
{
    SingleVector v1(bilerp_velocity(N, vel, p, 0),bilerp_velocity(N, vel, p, 1),bilerp_velocity(N, vel, p, 2));
    SingleVector p1(p.x - 0.5 * dt * v1.x, p.y - 0.5 * dt * v1.y, p.z - 0.5 * dt * v1.z);
    SingleVector v2(bilerp_velocity(N, vel, p1, 0), bilerp_velocity(N, vel, p1, 1), bilerp_velocity(N, vel, p1, 2));
    SingleVector p2(p.x - 0.75 * dt * v2.x, p.y - 0.75 * dt * v2.y, p.z - 0.75 * dt * v2.z);
    SingleVector v3(bilerp_velocity(N, vel, p2, 0), bilerp_velocity(N, vel, p2, 1), bilerp_velocity(N, vel, p2, 2));
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


float lerpSCALAR(float l, float r, float t)
{
    return l + t * (r - l);
}
float sampleSCALAR(const int N, ScalarField& qf, float i, float j, float k)
{
    int x = static_cast<int>(i);
    x = max(0, min(N - 1, x));
    int y = static_cast<int>(j);
    y = max(0, min(N - 1, y));
    int z = static_cast<int>(k);
    z = max(0, min(N - 1, z));
   
   
    return qf.v[IXY(x,y,z,N)];
}
SingleVector bilerp(const int N, VectorField& qf, SingleVector& p)
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
float bilerpSCALAR(const int N, ScalarField & qf, SingleVector & p)
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
SingleVector backtrace(const int N, SingleVector p, float dt, VectorField& vel)
{
    SingleVector v1(bilerp(N, vel, p));
    SingleVector p1((p.x - 0.5 * dt * v1.x), (p.y - 0.5 * dt * v1.y), (p.z - 0.5 * dt * v1.z));
    SingleVector v2(bilerp(N, vel, p1));
    SingleVector p2(p.x - 0.75 * dt * v2.x, p.y - 0.75 * dt * v2.y, p.z - 0.75 * dt * v2.z);
    SingleVector v3(bilerp(N, vel, p2));
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
                p = backtrace(N, p, dt, vel).FullScalarMultiply(damping);

                //Aquire new values from calculated backtrace p and bilerp new value into respective cell
                new_qf.v[IXY(i, j, k, N)] = bilerpSCALAR(N, qf, p);
            }
        }
    }
}
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
                    new_qf.x[IXY(i, j, k, N)] = bilerp_velocity(N, vel, p, 0) * damping;
                }
                if (j == 0 || j == N - 1)
                    new_qf.y[IXY(i, j, k, N)] = 0.0f;
                else
                {
                    SingleVector p(i + 0.5f, j, k + 0.5f);
                    p = backtrace_velocity(N, p, dt, vel).FullScalarMultiply(damping);
                    new_qf.y[IXY(i, j, k, N)] = bilerp_velocity(N, vel, p, 1) * damping;
                }
                if (k == 0 || k == N - 1)
                    new_qf.z[IXY(i, j, k, N)] = 0.0f;
                else
                {
                    SingleVector p(i + 0.5f, j + 0.5f, k);
                    p = backtrace_velocity(N, p, dt, vel).FullScalarMultiply(damping);
                    new_qf.z[IXY(i, j, k, N)] = bilerp_velocity(N, vel, p, 2) * damping;
                }
            }
        }
    }
   
}
void get_divergence(const int N, VectorField& vel, ScalarField& divergence)
{
    for (int k = 0; k < N; k++)
        for (int j = 0; j < N; j++)
        {
            for (int i = 0; i < N; i++)
            {
                float vl = sample(N, vel, i, j,k).x;
                float vr = sample(N, vel, i + 1, j,k).x;
                float vb = sample(N, vel, i, j,k).y;
                float vt = sample(N, vel, i, j + 1,k).y;
                float vbb = sample(N, vel, i, j,k).z;
                float vf = sample(N, vel, i, j,k + 1).z;

                divergence.v[IXY(i, j, k, N)] = vr - vl + vt - vb + vf - vbb;
            }
        }
}
void get_curl(const int N, VectorField& vel, ScalarField& curl)
{
    for (int k = 0; k < N; k++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int i = 0;i < N; i++)
            {
                SingleVector a(i, j + 0.5f, k);
                SingleVector b(i + 1.0f, j + 0.5f, k + 1.0f);
                SingleVector c(i + 0.5f, j + 1.0f, k + 1.0f);
                SingleVector d(i + 0.5f, j, k);
                SingleVector e (i, j, k + 0.5f);
                SingleVector f(i + 1.0f, j + 1.0f, k + 0.5f);
                float vl = bilerp_velocity(N, vel, a, 1);
                float vr = bilerp_velocity(N, vel, b, 1);
                float vt = bilerp_velocity(N, vel, c, 0);
                float vb = bilerp_velocity(N, vel, d, 0);

                float vf = bilerp_velocity(N, vel, e, 2);
                float vbb = bilerp_velocity(N, vel, f, 2);
                //clockwise motion in 2 2d planes??
                curl.v[IXY(i, j, k, N)] = vr - vt - vl + vb + vf - vt + vbb-vb;
            }
        }
    }
}
void vorticity_confinement(const int N, VectorField& vel, ScalarField& curl,
    VectorField& curl_force,  float curl_strength, float  dt)
{
    float scale = 1e-3f;
    for (int k = 0; k < N; k++)
        for (int j = 0; j < N; j++)
            for (int i = 0; i < N; i++)
            {
                float cl = sampleSCALAR(N, curl, i - 1, j, k);
                float cr = sampleSCALAR(N, curl, i + 1, j, k);
                float cb = sampleSCALAR(N, curl, i, j - 1, k);
                float ct = sampleSCALAR(N, curl, i, j + 1, k);
                float cc = sampleSCALAR(N, curl, i, j, k);
                float cf = sampleSCALAR(N, curl, i, j, k - 1);
                float cd = sampleSCALAR(N, curl, i, j, k + 1);
                curl_force.x[IXY(i, j, k, N)] = abs(ct) - abs(cb);
                    curl_force.y[IXY(i, j, k, N)] = abs(cl) - abs(cr);
                    curl_force.z[IXY(i, j, k, N)] = abs(cf) - abs(cd);
                 
                    curl_force.normalize(IXY(i, j, k, N));
               
                     curl_force.x[IXY(i, j, k, N)] *= curl_strength * cc * dt;
                     curl_force.y[IXY(i, j, k, N)] *= curl_strength * cc * dt;
                     curl_force.z[IXY(i, j, k, N)] *= curl_strength * cc * dt;
            }

    for (int k = 0; k < N; k++)
        for (int j = 0; j < N; j++)
        {
            for (int i = 0; i < N; i++)
            {
                SingleVector cl = sample(N, curl_force, i - 1, j, k);
                SingleVector cb = sample(N, curl_force, i, j - 1, k);
                SingleVector ce = sample(N, curl_force, i, j, k - 1);
                SingleVector cc = sample(N, curl_force, i, j, k);
                float fl = (cl.x - cc.x);
                float fb = (cb.y - cc.y);
                float fc = (ce.z - cc.z);
                vel.x[IXY(i, j, k, N)] = min(1e4f, max(-1e4f, vel.x[IXY(i, j, k, N)] + fl));
                vel.y[IXY(i, j, k, N)] = min(1e4f, max(-1e4f, vel.y[IXY(i, j, k, N)] + fb));
                vel.z[IXY(i, j, k, N)] = min(1e4f, max(-1e4f, vel.z[IXY(i, j, k, N)] + fc));
            }
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