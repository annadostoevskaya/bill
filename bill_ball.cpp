/*
Author: github.com/annadostoevskaya
File: bill_ball.cpp
Date: 16/02/23 13:40:40

Description: <empty>
*/

#define BALL_FRICTION 0.5f

inline V2DF32 ballCalcAcc(Entity *ball)
{
    return ball->v * -BALL_FRICTION;
}

internal void ballsInitRack(Entity *balls, F32 radius, F32 x, F32 y)
{
    Assert(BALL_COUNT == 16); // TODO(annad):  5! = 16 and 
    // inverse of factorial is 
    // https://math.stackexchange.com/questions/2078997/inverse-of-a-factorial
    S32 dy = 5;
    S32 dx = 5;
    S32 ballIdx = BALL_2;
    for (S32 i = dx; i > 0; i -= 1)
    {
        dy = i;
        F32 shift = ((5.0f - (F32)i) * radius);
        for (S32 j = dy; j > 0; j -= 1)
        {
            Assert(ballIdx < BALL_COUNT);
            balls[ballIdx].id = (EntityID)ballIdx;
            balls[ballIdx].p.y = (y + shift + (F32)j * (2.0f * (radius)));
            // TODO(annad): METRIX, keep going later!
            balls[ballIdx].p.x = (x + (F32)i * (2.0f * (radius))) + (2.0f * radius * 10.0f);
            balls[ballIdx].v.x = 0.0f;
            balls[ballIdx].v.y = 0.0f;
            balls[ballIdx].isInit = true;
            balls[ballIdx].isUpdated = false;
            balls[ballIdx].dtUpdate = 0.0f;
            ballIdx += 1;
        }
    }

    balls[CUE_BALL].id = CUE_BALL;
    balls[CUE_BALL].p.x = balls[BALL_16].p.x - (2.0f * radius * 10.0f);
    balls[CUE_BALL].p.y = balls[BALL_16].p.y;
    balls[CUE_BALL].v.x = 0.0f;
    balls[CUE_BALL].v.y = 0.0f;
    balls[CUE_BALL].isInit = true;
    balls[CUE_BALL].isUpdated = false;
    balls[CUE_BALL].dtUpdate = 0.0f;
}

internal void ballsInit(Table *table, Entity *balls, F32 radius, F32 x, F32 y)
{
    F32 rackPosX = (x * (F32)table->collider.w);
    F32 rackPosY = (y * (F32)table->collider.h);
    ballsInitRack(balls, radius, rackPosX, rackPosY);
}

internal Entity ballUpdate(Entity *ball, F32 dt)
{
    V2DF32 a = ballCalcAcc(ball);
    Entity updated = *ball;
    updated.p += (a * 0.5f * f32Square(dt) + ball->v * dt);
    updated.v += a * dt;
    return updated;   
}

internal B8 ballCheckTableBoardCollide(Entity *ball, F32 radius, Rect *table, V2DF32 *nvecwall)
{
    if (ball->p.x >= (F32)(table->x + table->w) - radius)
    {
        nvecwall->x = -1.0f;
        nvecwall->y =  0.0f;
        return true;
    }

    if (ball->p.x <= (F32)table->x + radius)
    {
        nvecwall->x = 1.0f;
        nvecwall->y = 0.0f;
        return true;
    }

    if (ball->p.y <= (F32)table->y + radius)
    {
        nvecwall->x =  0.0f;
        nvecwall->y = -1.0f;
        return true;
    }

    if (ball->p.y >= (F32)(table->y + table->h) - radius)
    {
        nvecwall->x = 0.0f;
        nvecwall->y = 1.0f;
        return true;
    }

    return false;
}

internal B8 ballCheckBallCollide(Entity *a, Entity *b, F32 radius)
{
    F32 d = (a->p - b->p).getLength();
    return d < (2.0f * radius);
}

internal F32 ballTimeBeforeWallCollide(Entity *ballA, Rect *table, V2DF32 *nvecwall)
{
    (void)ballA;
    (void)table;
    (void)nvecwall;
    // TODO(annad): Now, is maximum priority level! 
    return 0.0f;
}

internal F32 ballTimeBeforeBallCollide(Entity *ballA, Entity *ballB, F32 radius)
{
    // NOTE(annad): Last update, <date>
    V2DF32 d = ballB->p - ballA->p;
    F32 dl = d.getLength();
    // NOTE(annad): Block with corner cases!
    if (ballCheckBallCollide(ballA, ballB, radius))
    {
        // NOTE(annad): Already collide!
#if BILL_CFG_DEV_MODE
        DbgPrint("[INFO] Already collide%s", "!");
#endif
        return 0.0f;
    }

    F32 v = ballA->v.getLength(); 
    if (f32EpsCompare(v, 0.0f, 0.01f)) // TODO(annad): kowalski analysis.
    {                                  // is this at all possible?
        // NOTE(annad): Never collide!
        // Assert(false);
        return f32Infinity();
    }

    F32 cos = ballA->v.inner(d) / (dl * v);
    if (f32EpsCompare(cos, 0.0f, 0.001f))
    {
        // TODO(annad): Check this later
        // if one of the values goes to zero we get this case
        // Assert(false);
        return 0.0f;
    }

    // NOTE(annad): Calculate distance between collide points
    // 
    // s(fi) = 2r * (1 / cos(w)) * sin( arcsin(2r*S*(1/cos(w))) + w ) where 
    // w - angle berween distance B - A and Velocity vector
    // r  - ball's radius
    // S  - |B - A|
    //
    // See https://www.geogebra.org/m/qqy3e5q9 for more info.
    F32 s = 0.0f;
    if (f32EpsCompare(cos, 1.0f, 0.001f))
    {
        s = dl - (2.0f * radius);
    }
    else
    {
        F32 aAngle = f32ArcCos(cos);
        F32 A = 2.0f * radius;
        F32 B = dl;
        F32 sinA = f32Sin(aAngle);
        F32 sinB = (sinA / A) * B;

        // TODO(annad): kowalski analysis.
        // Can this be done better?
        sinB = sinB > 1.0f  ?  1.0f : sinB;
        sinB = sinB < -1.0f ? -1.0f : sinB;

        F32 sinC = f32Sin(f32ArcSin(sinB) - aAngle);
        F32 C = (A / sinA) * sinC;
        s = C;
        if (f32EpsCompare(s, 0.0f, 0.0001f))
        {
            return 0.0f;
        }
    }

    F32 a = 0.5f * ballCalcAcc(ballA).getLength();
    F32 D = f32Sqrt(f32Square(v) - 4.0f * a * s);
    Assert(D == D); // TODO(annad): If to f32Sqrt pass x <= 0
    F32 t = (v - D) / (2.0f * a);
    return t;
}

#if 0
internal void ballSolveCollide2Ball(Entity *a, Entity *b, Entity *c)
{
#if BILL_CFG_DEV_MODE
    DbgPrint("SolveCollide2%s", "\n");
#endif
    // TODO(annad): Rewrite the code in terms of physics
    F32 v = a->v.getLength();
    if(!f32EpsCompare(v, 0.0f, 0.0001f))
    {
        V2DF32 BA = b->p - a->p;
        V2DF32 CA = c->p - a->p;
        F32 lBA = BA.getLength();
        F32 lCA = CA.getLength();
        V2DF32 directA = { a->v.x / v, a->v.y / v };
        V2DF32 directB = { BA.x / lBA, BA.y / lBA };
        V2DF32 directC = { CA.x / lCA, CA.y / lCA };
        F32 cosB = a->v.inner(directB) / (v * directB.getLength());
        F32 cosC = a->v.inner(directC) / (v * directC.getLength());
        F32 vb = (2.0f * v * cosB) / (1.0f + 2.0f * f32Square(cosB));
        F32 vc = (2.0f * v * cosC) / (1.0f + 2.0f * f32Square(cosC));
        b->v += directB * vb;
        c->v += directC * vc;
        a->v = directA * (v - (2.0f * vb * cosB));
    }
}
#endif

internal void ballSolveCollideOneBall(Entity *a, Entity *b)
{
    F32 v = a->v.getLength();
    if(!f32EpsCompare(v, 0.0f, 0.0001f))
    {
        V2DF32 d = b->p - a->p;
        F32 dl = d.getLength();
        V2DF32 directB = { d.x / dl, d.y / dl };
        V2DF32 directA = { directB.y, -directB.x };
        F32 cosA = a->v.inner(directA) / (v * directA.getLength());
        F32 cosB = a->v.inner(directB) / (v * directB.getLength());
        a->v = directA * v * cosA;
        b->v += directB * v * cosB;
    }
}

