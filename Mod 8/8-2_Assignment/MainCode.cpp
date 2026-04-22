#include <GLFW\glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>
#include <cmath>
#include <algorithm>

using namespace std;

const float DEG2RAD = 3.14159f / 180.0f;
const float WORLD_LEFT = -1.0f;
const float WORLD_RIGHT = 1.0f;
const float WORLD_TOP = -1.0f;
const float WORLD_BOTTOM = 1.0f;
const int MAX_CIRCLES = 20;

void processInput(GLFWwindow* window);
float randomFloat(float minValue, float maxValue);
float clampFloat(float value, float minValue, float maxValue);

// Different brick behaviors.
enum BRICKTYPE { REFLECTIVE, DESTRUCTABLE, PADDLE };
enum ONOFF { ON, OFF };

class Brick
{
public:
    float red, green, blue;
    float x, y, width, height;
    BRICKTYPE brick_type;
    ONOFF onoff;
    int hitsRemaining;
    int maxHits;

    Brick(BRICKTYPE bt, float xx, float yy, float ww, float hh, float rr, float gg, float bb, int hits = 1)
    {
        brick_type = bt;
        x = xx;
        y = yy;
        width = ww;
        height = hh;
        red = rr;
        green = gg;
        blue = bb;
        onoff = ON;
        hitsRemaining = hits;
        maxHits = hits;
    }

    void drawBrick() const
    {
        if (onoff == OFF)
        {
            return;
        }

        float halfWidth = width / 2.0f;
        float halfHeight = height / 2.0f;

        glColor3f(red, green, blue);
        glBegin(GL_POLYGON);
        glVertex2f(x + halfWidth, y + halfHeight);
        glVertex2f(x + halfWidth, y - halfHeight);
        glVertex2f(x - halfWidth, y - halfHeight);
        glVertex2f(x - halfWidth, y + halfHeight);
        glEnd();
    }

    void updateAppearance()
    {
        if (brick_type == DESTRUCTABLE)
        {
            if (hitsRemaining == 3)
            {
                red = 0.2f; green = 0.8f; blue = 1.0f;
            }
            else if (hitsRemaining == 2)
            {
                red = 1.0f; green = 0.7f; blue = 0.2f;
            }
            else if (hitsRemaining == 1)
            {
                red = 1.0f; green = 0.2f; blue = 0.2f;
            }
        }
    }
};

class Circle
{
public:
    float red, green, blue;
    float radius;
    float x;
    float y;
    float vx;
    float vy;
    bool active;

    Circle(float xx, float yy, float rad, float velX, float velY, float r, float g, float b)
    {
        x = xx;
        y = yy;
        radius = rad;
        vx = velX;
        vy = velY;
        red = r;
        green = g;
        blue = b;
        active = true;
    }

    float speed() const
    {
        return sqrt((vx * vx) + (vy * vy));
    }

    void limitSpeed(float minSpeed, float maxSpeed)
    {
        float currentSpeed = speed();

        if (currentSpeed == 0.0f)
        {
            vx = 0.01f;
            vy = -0.01f;
            return;
        }

        if (currentSpeed < minSpeed)
        {
            float scale = minSpeed / currentSpeed;
            vx *= scale;
            vy *= scale;
        }
        else if (currentSpeed > maxSpeed)
        {
            float scale = maxSpeed / currentSpeed;
            vx *= scale;
            vy *= scale;
        }
    }

    void changeToRandomColor()
    {
        red = randomFloat(0.2f, 1.0f);
        green = randomFloat(0.2f, 1.0f);
        blue = randomFloat(0.2f, 1.0f);
    }

    void MoveOneStep()
    {
        x += vx;
        y += vy;

        // Bounce off the world edges while preserving the angle of travel.
        if (x + radius >= WORLD_RIGHT)
        {
            x = WORLD_RIGHT - radius;
            vx = -fabs(vx) * 1.02f;
            changeToRandomColor();
        }
        else if (x - radius <= WORLD_LEFT)
        {
            x = WORLD_LEFT + radius;
            vx = fabs(vx) * 1.02f;
            changeToRandomColor();
        }

        if (y + radius >= WORLD_BOTTOM)
        {
            y = WORLD_BOTTOM - radius;
            vy = -fabs(vy) * 1.01f;
            vx *= 0.98f;
            changeToRandomColor();
        }
        else if (y - radius <= WORLD_TOP)
        {
            y = WORLD_TOP + radius;
            vy = fabs(vy) * 1.01f;
            vx *= 0.98f;
            changeToRandomColor();
        }

        limitSpeed(0.008f, 0.04f);
    }

    void DrawCircle() const
    {
        if (!active)
        {
            return;
        }

        glColor3f(red, green, blue);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i++)
        {
            float degInRad = i * DEG2RAD;
            glVertex2f((cos(degInRad) * radius) + x, (sin(degInRad) * radius) + y);
        }
        glEnd();
    }
};

vector<Circle> world;
vector<Brick> bricks;

float randomFloat(float minValue, float maxValue)
{
    return minValue + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxValue - minValue);
}

float clampFloat(float value, float minValue, float maxValue)
{
    if (value < minValue)
    {
        return minValue;
    }
    if (value > maxValue)
    {
        return maxValue;
    }
    return value;
}

bool circleIntersectsBrick(const Circle& circle, const Brick& brick)
{
    if (brick.onoff == OFF)
    {
        return false;
    }

    float halfWidth = brick.width / 2.0f;
    float halfHeight = brick.height / 2.0f;

    float nearestX = clampFloat(circle.x, brick.x - halfWidth, brick.x + halfWidth);
    float nearestY = clampFloat(circle.y, brick.y - halfHeight, brick.y + halfHeight);

    float dx = circle.x - nearestX;
    float dy = circle.y - nearestY;

    return (dx * dx + dy * dy) <= (circle.radius * circle.radius);
}

void resolveBrickCollision(Circle& circle, Brick& brick)
{
    if (!circleIntersectsBrick(circle, brick))
    {
        return;
    }

    float dx = circle.x - brick.x;
    float dy = circle.y - brick.y;
    float overlapX = (brick.width / 2.0f + circle.radius) - fabs(dx);
    float overlapY = (brick.height / 2.0f + circle.radius) - fabs(dy);

    // Bounce based on the shallowest overlap so the circle reacts naturally.
    if (overlapX < overlapY)
    {
        circle.vx = -circle.vx;
        if (dx > 0)
        {
            circle.x += 0.01f;
        }
        else
        {
            circle.x -= 0.01f;
        }
    }
    else
    {
        circle.vy = -circle.vy;
        if (dy > 0)
        {
            circle.y += 0.01f;
        }
        else
        {
            circle.y -= 0.01f;
        }
    }

    if (brick.brick_type == REFLECTIVE)
    {
        // Reflective bricks increase excitement by slightly speeding the circle up.
        circle.vx *= 1.03f;
        circle.vy *= 1.03f;
        circle.changeToRandomColor();
    }
    else if (brick.brick_type == DESTRUCTABLE)
    {
        // Destructible bricks visually change state before disappearing.
        brick.hitsRemaining--;
        brick.updateAppearance();
        circle.changeToRandomColor();

        if (brick.hitsRemaining <= 0)
        {
            brick.onoff = OFF;
        }
    }
    else if (brick.brick_type == PADDLE)
    {
        // The paddle adds a friction-like effect and changes the angle.
        float hitOffset = (circle.x - brick.x) / (brick.width / 2.0f);
        circle.vx += hitOffset * 0.01f;
        circle.vy = -fabs(circle.vy);
        circle.vx *= 0.97f;
        circle.changeToRandomColor();
    }

    circle.limitSpeed(0.008f, 0.045f);
}

void handleCircleCollisions()
{
    vector<Circle> newCircles;

    for (size_t i = 0; i < world.size(); i++)
    {
        for (size_t j = i + 1; j < world.size(); j++)
        {
            if (!world[i].active || !world[j].active)
            {
                continue;
            }

            float dx = world[j].x - world[i].x;
            float dy = world[j].y - world[i].y;
            float minDistance = world[i].radius + world[j].radius;
            float distanceSquared = (dx * dx) + (dy * dy);

            if (distanceSquared <= (minDistance * minDistance))
            {
                // Swap velocities for a simple elastic-style response.
                swap(world[i].vx, world[j].vx);
                swap(world[i].vy, world[j].vy);

                world[i].changeToRandomColor();
                world[j].changeToRandomColor();

                // Spawn one smaller circle to satisfy the state-change requirement.
                if (static_cast<int>(world.size() + newCircles.size()) < MAX_CIRCLES)
                {
                    float newRadius = max(0.018f, min(world[i].radius, world[j].radius) * 0.55f);
                    float newVx = randomFloat(-0.018f, 0.018f);
                    float newVy = randomFloat(-0.018f, 0.018f);
                    newCircles.push_back(Circle(
                        (world[i].x + world[j].x) / 2.0f,
                        (world[i].y + world[j].y) / 2.0f,
                        newRadius,
                        newVx,
                        newVy,
                        randomFloat(0.3f, 1.0f),
                        randomFloat(0.3f, 1.0f),
                        randomFloat(0.3f, 1.0f)
                    ));
                }

                // Small separation so the circles do not stick together.
                world[i].x -= world[i].vx;
                world[i].y -= world[i].vy;
                world[j].x -= world[j].vx;
                world[j].y -= world[j].vy;
            }
        }
    }

    for (size_t i = 0; i < newCircles.size(); i++)
    {
        world.push_back(newCircles[i]);
    }
}

void createBrickLayout()
{
    bricks.clear();

    // Top rows of destructible bricks with 3-hit durability.
    float startX = -0.72f;
    float topY = -0.72f;
    float brickWidth = 0.22f;
    float brickHeight = 0.10f;

    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 6; col++)
        {
            float x = startX + (col * 0.25f);
            float y = topY + (row * 0.14f);
            bricks.push_back(Brick(DESTRUCTABLE, x, y, brickWidth, brickHeight, 0.2f, 0.8f, 1.0f, 3));
        }
    }

    // Side reflective pillars.
    bricks.push_back(Brick(REFLECTIVE, -0.86f, -0.05f, 0.10f, 0.28f, 1.0f, 0.9f, 0.2f));
    bricks.push_back(Brick(REFLECTIVE, 0.86f, -0.05f, 0.10f, 0.28f, 1.0f, 0.9f, 0.2f));
    bricks.push_back(Brick(REFLECTIVE, -0.45f, 0.05f, 0.14f, 0.14f, 0.8f, 0.4f, 1.0f));
    bricks.push_back(Brick(REFLECTIVE, 0.45f, 0.05f, 0.14f, 0.14f, 0.8f, 0.4f, 1.0f));

    // A player-controlled paddle at the bottom.
    bricks.push_back(Brick(PADDLE, 0.0f, 0.88f, 0.45f, 0.08f, 1.0f, 0.5f, 0.5f));
}

int main(void)
{
    srand(static_cast<unsigned int>(time(NULL)));

    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(700, 700, "8-2 Assignment", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    createBrickLayout();

    // Start with two circles so the animation is active immediately.
    world.push_back(Circle(-0.15f, 0.35f, 0.05f, 0.014f, -0.012f, 0.2f, 1.0f, 0.3f));
    world.push_back(Circle(0.18f, 0.30f, 0.04f, -0.012f, -0.015f, 1.0f, 0.3f, 0.6f));

    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        processInput(window);

        for (size_t i = 0; i < world.size(); i++)
        {
            for (size_t j = 0; j < bricks.size(); j++)
            {
                resolveBrickCollision(world[i], bricks[j]);
            }

            world[i].MoveOneStep();
        }

        handleCircleCollisions();

        for (size_t i = 0; i < bricks.size(); i++)
        {
            bricks[i].drawBrick();
        }

        for (size_t i = 0; i < world.size(); i++)
        {
            world[i].DrawCircle();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // Move the paddle with A/D or Left/Right.
    for (size_t i = 0; i < bricks.size(); i++)
    {
        if (bricks[i].brick_type == PADDLE)
        {
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            {
                bricks[i].x -= 0.03f;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            {
                bricks[i].x += 0.03f;
            }

            float halfWidth = bricks[i].width / 2.0f;
            bricks[i].x = clampFloat(bricks[i].x, WORLD_LEFT + halfWidth, WORLD_RIGHT - halfWidth);
        }
    }

    // Press Space to add another circle.
    static bool spaceWasReleased = true;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (spaceWasReleased && static_cast<int>(world.size()) < MAX_CIRCLES)
        {
            world.push_back(Circle(
                randomFloat(-0.2f, 0.2f),
                0.55f,
                randomFloat(0.025f, 0.05f),
                randomFloat(-0.018f, 0.018f),
                randomFloat(-0.02f, -0.01f),
                randomFloat(0.2f, 1.0f),
                randomFloat(0.2f, 1.0f),
                randomFloat(0.2f, 1.0f)
            ));
        }
        spaceWasReleased = false;
    }
    else
    {
        spaceWasReleased = true;
    }
}