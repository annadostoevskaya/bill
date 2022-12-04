
    // TODO(annad): Write Linked-List and Proroty-Queue
    F32 dt = (((F32)game_time->dt / 1000.0f));
    if(dt == 0.0f)
    {
        dt = 1.0f/60.0f;
    }
    
    F32 dt_for_balls[BALL_ENUM_COUNT] = {};
    for(S32 i = 0; i < BALL_ENUM_COUNT; i += 1)
    {
        dt_for_balls[i] = dt;
    }
    
    // printf("-=-=-=-=-=-=-=-=-=-=-=-=-=[ START ]-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
    B32 dt_is_up = false;
    while(!dt_is_up)
    {
        F32 t_table[BALL_ENUM_COUNT * BALL_ENUM_COUNT] = {};
        // TODO(annad): No dt, dt_for_balls.min
        get_table_t_before_collide(t_table, (Ball*)(game_state->balls), dt);
        
        /*         
                printf("=[ DTs ]=\n");
                for(S32 i = 0; i < BALL_ENUM_COUNT; i += 1)
                {
                    if(dt_for_balls[i] != dt && dt_for_balls[i] != 0.0f)
                    {
                        printf("^%lf ", dt_for_balls[i]);
                    }
                    else 
                    {
                        printf("%lf ", dt_for_balls[i]);
                    }
                }
                printf("\n");
                printf("=[ DTs ]=\n");
                
                printf("=[TABLE t BEFORE COLLIDE]=\n");
                for(S32 i = 0; i < BALL_ENUM_COUNT; i += 1)
                {
                    for(S32 j = 0; j < BALL_ENUM_COUNT; j += 1)
                    {
                        if(t_table[i * BALL_ENUM_COUNT + j] == 0.0f)
                        {
                            printf("%lf ", t_table[i * BALL_ENUM_COUNT + j]);
                        }
                        else
                        {
                            printf("*%lf ", t_table[i * BALL_ENUM_COUNT + j]);
                        }
                    }
                    printf("\n");
                }
                printf("=[TABLE t BEFORE COLLIDE]=\n");
                 */
        
        // Find min_t
        S32 min_t_idx = 0;
        F32 min_t = 999.0f;
        for(S32 i = 0; i < BALL_ENUM_COUNT * BALL_ENUM_COUNT; i += 1)
        {
            if(t_table[i] != 0.0f && min_t > t_table[i])
            {
                if(dt_for_balls[i / BALL_ENUM_COUNT] != 0.0f)
                {
                    min_t = t_table[i];
                    min_t_idx = i;
                }
            }
        }
        
        // If min_t not founded, update not collided balls
        if(min_t == 999.0f)
        {
            for(S32 i = 0; i < BALL_ENUM_COUNT; i += 1)
            {
                F32 t = dt_for_balls[i];
                Ball *ball_a = &game_state->balls[i];
                if(t > 0.0f)
                {
                    *ball_a = update_ball(ball_a, t);
                    dt_for_balls[i] = 0.0f;
                    break;
                }
            }
        }
        else
        {
            S32 ball_a_idx = min_t_idx / BALL_ENUM_COUNT;
            if(dt_for_balls[ball_a_idx] <= 0.0f)
            {
                Assert(dt_for_balls[ball_a_idx] < 0.0f);
            }
            else if(dt_for_balls[ball_a_idx] > 0.0f && dt_for_balls[ball_a_idx] < min_t)
            {
                Ball *ball_a = &game_state->balls[ball_a_idx];
                *ball_a = update_ball(ball_a, dt_for_balls[ball_a_idx]);
                dt_for_balls[ball_a_idx] = 0.0f;
            }
            else
            {
                
                ////////////////////////////////
                S32 collides_in_min_dt_counter = 0;
                S32 pull_balls_b_idx[BALL_ENUM_COUNT] = {};
                for(S32 i = 0; i < BALL_ENUM_COUNT; i++)
                {
                    if(t_table[ball_a_idx * BALL_ENUM_COUNT + i] == min_t)
                    {
                        pull_balls_b_idx[collides_in_min_dt_counter] = i;
                        collides_in_min_dt_counter++;
                    }
                }
                ////////////////////////////////
                Assert(collides_in_min_dt_counter >= 1);
                if(collides_in_min_dt_counter == 1)
                {
                    // __debugbreak();
                    Ball *ball_a = &game_state->balls[ball_a_idx];
                    S32 ball_b_idx = min_t_idx % BALL_ENUM_COUNT;
                    Ball *ball_b = &game_state->balls[ball_b_idx];
                    
                    Ball ball_a_copy = *ball_a;
                    Ball ball_b_copy = *ball_b;
                    ball_a_copy = update_ball(&ball_a_copy, min_t);
                    dt_for_balls[ball_a_idx] -= min_t;
                    // Assert(dt_for_balls[ball_a_idx] > 0.0f);
                    balls_collide_handle(&ball_a_copy, &ball_b_copy);
                    ball_a_copy = update_ball(&ball_a_copy, dt_for_balls[ball_a_idx]);
                    if(balls_is_collide(&ball_a_copy, &ball_b_copy))
                    {
                        printf("RESOTER: \n");
                        EvalPrintF(dt_for_balls[ball_a_idx]);
                        dt_for_balls[ball_a_idx] = 0.0f;
                    }
                    else
                    {
                        *ball_a = ball_a_copy;
                        *ball_b = ball_b_copy;
                    }
                }
                else if(collides_in_min_dt_counter == 2)
                {
                    EvalPrint(collides_in_min_dt_counter);
                    Ball *ball_a = &game_state->balls[ball_a_idx];
                    Ball ball_a_copy = *ball_a;
                    Ball ball_b1_copy = game_state->balls[pull_balls_b_idx[0]];
                    Ball ball_b2_copy = game_state->balls[pull_balls_b_idx[1]];
                    Vec2Dim<F32> ball_a_vel = {
                        ball_a_copy.vel.x / 2,
                        ball_a_copy.vel.y / 2
                    };
                    
                    balls_collide_handle(&ball_a_copy, &ball_b1_copy);
                    ball_a_copy.vel = ball_a_vel;
                    balls_collide_handle(&ball_a_copy, &ball_b2_copy);
                    dt_for_balls[ball_a_idx] -= min_t;
                    ball_a_copy = update_ball(&ball_a_copy, dt_for_balls[ball_a_idx]);
                    if(balls_is_collide(&ball_a_copy, &ball_b1_copy) || balls_is_collide(&ball_a_copy, &ball_b2_copy))
                    {
                        printf("RESTORE\n");
                        EvalPrintF(dt_for_balls[ball_a_idx]);
                        dt_for_balls[ball_a_idx] = 0.0f;
                    }
                    else
                    {
                        game_state->balls[pull_balls_b_idx[0]] = ball_b1_copy;
                        game_state->balls[pull_balls_b_idx[1]] = ball_b2_copy;
                        *ball_a = ball_a_copy;
                    }
                }
            }
        }
        
        // check, is dt calculated?
        dt_is_up = true;
        for(S32 i = 0; i < BALL_ENUM_COUNT; i += 1)
        {
            if(dt_for_balls[i] > 0.0f)
            {
                dt_is_up = false;
                break;
            }
        }
    }
    // printf("-=-=-=-=-=-=-=-=-=-=-=-=-=[  END  ]-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");
    